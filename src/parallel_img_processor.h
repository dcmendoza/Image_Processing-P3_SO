#ifndef PARALLEL_IMG_PROCESSOR_H
#define PARALLEL_IMG_PROCESSOR_H

#include <string>

// Carga imagen en buffer lineal, devuelve puntero o nullptr
unsigned char* cargar_imagen_omp(const std::string& ruta,
                                int& ancho,
                                int& alto,
                                int& canales);

// Muestra info de la imagen cargada
void procesar_imagen_omp(int ancho, int alto, int canales);

// Rota (OpenMP) y guarda JPG
void rotar_imagen_omp(const unsigned char* buffer,
                      int ancho, int alto, int canales,
                      int angulo,
                      const std::string& salida);

// Escala (OpenMP) y guarda JPG
void escalar_imagen_omp(const unsigned char* buffer,
                        int ancho, int alto, int canales,
                        float factor,
                        const std::string& salida);

#endif // PARALLEL_IMG_PROCESSOR_H