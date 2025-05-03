# README.md

Proyecto de procesamiento de imágenes en C++ que compara tres métodos de gestión de memoria y procesamiento:

* **Convencional (new/delete)**: uso de `new`/`delete` y estructuras clásicas.
* **Buddy System**: `BuddyAllocator` propio sobre un pool fijo.
* **OpenMP (concurrencia)**: paralelización de operaciones intensivas de píxeles.

---

## Características

* Carga, rotación y escalado de imágenes JPEG.
* Comparativa de tiempo y memoria entre:

  * Convencional (new/delete)
  * Buddy System Optimizado (pool de 32 MB)
  * OpenMP (paralelismo por hilos)
* Informes de rendimiento: tiempo (ms) y memoria (MB) consumidos.

## Prerrequisitos

* GNU Make
* g++ (C++17, soporte OpenMP)
* POSIX (`posix_memalign`, `getrusage`)

## Estructura de directorios

```bash
Image_Processing-P2_SO/
├── buddy_system/           # BuddyAllocator y STB wrappers
│   ├── buddy_allocator.h
│   ├── buddy_allocator.cpp
│   ├── stb_wrapper.cpp
│   └── Makefile
├── src/                    # Programa CLI y procesadores
│   ├── conv_img_processor.cpp
│   ├── buddy_img_processor.cpp
│   ├── parallel_img_processor.cpp  # OpenMP
│   ├── main.cpp
│   └── Makefile
├── img/                    # Imágenes de prueba
│   ├── testImg01.jpg
│   └── testImg02.jpg
└── README.md               # Este archivo
```

## Compilación

1. Compilar módulo Buddy System:

   ```bash
   cd buddy_system
   make
   ```
2. Compilar programa principal (incluye OpenMP):

   ```bash
   cd ../src
   make
   ```

> El `Makefile` de `src/` invoca `make -C ../buddy_system` y añade `-fopenmp` al linker de OpenMP.

## Uso

En `src/`, tras compilar:

```bash
./Parcial2_Danna <entrada.jpg> <salida.jpg> [opciones]
```

### Opciones

* `-angulo N`   : rota N grados.
* `-escalar F`  : escala por factor F (0.1–4.0).
* `-buddy`      : usa Buddy System.
* `-omp`        : usa procesamiento concurrente con OpenMP.

### Ejemplos

1. Convencional (rotación):

   ```bash
   ./Parcial2_Danna ../img/testImg01.jpg salida.jpg -angulo 90
   ```
2. Buddy System (rotar + escalar):

   ```bash
   ./Parcial2_Danna ../img/testImg01.jpg salida.jpg -angulo 45 -escalar 1.5 -buddy
   ```
3. OpenMP (rotar + escalar en paralelo):

   ```bash
   ./Parcial2_Danna ../img/testImg01.jpg salida.jpg -angulo 45 -escalar 1.5 -omp
   ```

## Estadísticas y limpieza

* El programa imprime tiempo (ms) y memoria (MB).
* Para limpiar:

  ```bash
  cd buddy_system && make clean
  cd ../src      && make clean
  ```

## Licencia

MIT. Consulte cabeceras `stb_image.h` y `stb_image_write.h` para detalles.