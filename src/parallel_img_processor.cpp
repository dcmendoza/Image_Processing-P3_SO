// parallel_img_processor.cpp

#include "parallel_img_processor.h"
#include "../buddy_system/stb_image.h"
#include "../buddy_system/stb_image_write.h"
#include <cmath>
#include <iostream>
#include <omp.h>

unsigned char* cargar_imagen_omp(const std::string& ruta,
                                int& ancho,
                                int& alto,
                                int& canales) {
    unsigned char* buffer = stbi_load(ruta.c_str(), &ancho, &alto, &canales, 0);
    if (!buffer) {
        std::cerr << "Error al cargar imagen: " << ruta << std::endl;
        return nullptr;
    }
    return buffer;
}

void procesar_imagen_omp(int ancho, int alto, int canales) {
    std::cout << "Imagen cargada (OpenMP Concurrencia):" << std::endl;
    std::cout << "Dimensiones: " << ancho << " x " << alto << std::endl;
    std::cout << "Canales: " << canales << std::endl;
}

// Interpolación bilineal simple con relleno negro fuera de límites
static inline unsigned char bilinear(const unsigned char* buf,
                                    int width, int height, int channels,
                                    float x, float y, int c) {
    int x0 = static_cast<int>(floor(x));
    int y0 = static_cast<int>(floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    if (x0 < 0 || x1 >= width || y0 < 0 || y1 >= height)
        return 0;

    float dx = x - x0;
    float dy = y - y0;

    auto pix = [&](int yy, int xx, int cc) {
        return static_cast<float>(buf[(yy * width + xx) * channels + cc]);
    };

    float v00 = pix(y0, x0, c);
    float v10 = pix(y0, x1, c);
    float v01 = pix(y1, x0, c);
    float v11 = pix(y1, x1, c);

    float v0 = v00 * (1 - dx) + v10 * dx;
    float v1 = v01 * (1 - dx) + v11 * dx;
    float v  = v0  * (1 - dy) + v1  * dy;

    return static_cast<unsigned char>(v);
}

void rotar_imagen_omp(const unsigned char* buffer,
                    int ancho, int alto, int canales,
                    int angulo,
                    const std::string& salida) {
    float rad = angulo * M_PI / 180.0f;
    float cosA = cos(rad);
    float sinA = sin(rad);
    float cx = ancho / 2.0f;
    float cy = alto  / 2.0f;

    unsigned char* rotBuf = new unsigned char[ancho * alto * canales];

    #pragma omp parallel for collapse(2)
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            float xr = x - cx;
            float yr = y - cy;
            float xp =  xr * cosA + yr * sinA + cx;
            float yp = -xr * sinA + yr * cosA + cy;
            for (int c = 0; c < canales; ++c) {
                rotBuf[(y * ancho + x) * canales + c] =
                    bilinear(buffer, ancho, alto, canales, xp, yp, c);
            }
        }
    }

    if (!stbi_write_jpg(salida.c_str(), ancho, alto, canales,
                        rotBuf, ancho * canales)) {
        std::cerr << "Error al guardar imagen rotada: " << salida << std::endl;
    }
    delete[] rotBuf;
}

void escalar_imagen_omp(const unsigned char* buffer,
                        int ancho, int alto, int canales,
                        float factor,
                        const std::string& salida) {
    int newW = static_cast<int>(ancho * factor);
    int newH = static_cast<int>(alto  * factor);
    unsigned char* escBuf = new unsigned char[newW * newH * canales];

    #pragma omp parallel for collapse(2)
    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            float srcX = x / factor;
            float srcY = y / factor;
            for (int c = 0; c < canales; ++c) {
                escBuf[(y * newW + x) * canales + c] =
                    bilinear(buffer, ancho, alto, canales, srcX, srcY, c);
            }
        }
    }

    if (!stbi_write_jpg(salida.c_str(), newW, newH, canales,
                        escBuf, newW * canales)) {
        std::cerr << "Error al guardar imagen escalada: " << salida << std::endl;
    }
    delete[] escBuf;
}
