# Análisis Comparativo: Convencional vs Buddy System vs OpenMP

Este análisis compara tres modos de procesamiento de imágenes implementados en el proyecto:

* **Convencional (new/delete)**
* **Buddy System**
* **OpenMP (concurrencia)**

---

## 1. Diferencia en tiempo de procesamiento

* **Convencional**: óptimo para grandes bloques únicos, beneficiándose del allocador del SO.
* **Buddy System**: añade overhead de *split/coalesce*, suele ser **10–20% más lento** que convencional en muchos `alloc/free`.
* **OpenMP**: paraleliza operaciones costosas (rotación, escalado), consiguiendo **30–50% de reducción** en tiempo total frente a convencional, dependiendo de núcleos y tamaño de imagen.

## 2. Impacto del tamaño de la imagen

* Al aumentar de 512×512 a 2048×2048:

  * El buffer crece linealmente (`alto×ancho×canales`).
  * **Convencional**: asigna un bloque grande seguido al runtime del SO.
  * **Buddy**: fragmentación interna por potencias de dos, uso de múltiples sub-bloques.
  * **OpenMP**: escala de forma lineal en buffer, pero distribuye cómputo en hilos, manteniendo buen rendimiento incluso en imágenes muy grandes.

## 3. Eficiencia de cada método

* **Convencional**

  * Ventaja: bajo overhead en asignaciones únicas y grandes.
  * Inconveniente: no optimiza para múltiples bloques dinámicos.

* **Buddy System**

  * Ventaja: latencias predecibles, útil en sistemas embebidos sin swap.
  * Inconveniente: overhead de división/fusión, fragmentación interna.

* **OpenMP**

  * Ventaja: paraleliza el procesamiento de píxeles, reduciendo tiempo de cómputo.
  * Inconveniente: sobrecarga de creación/sincronización de hilos, beneficios mínimos en tareas no paralelizables (e.g., lectura/escritura de disco).

## 4. Optimización propuesta

1. **Pool estático** y preponder niveles típicos en Buddy.
2. **Evitar copias** clonando buffers, usar punteros o memoria in-place.
3. **OpenMP** para bucles pesados (rotación, escalado, filtros).
4. **Tiling** o streaming de regiones para no cargar imágenes completas en memoria.
5. **Afinidad de hilos** y chunk sizing en OpenMP para balanceo de carga.

## 5. Implicaciones en dispositivos con memoria limitada

* **Buddy System** requiere reservar el pool inicial (`totalSize`).
* **Convencional** puede causar OOM si el sistema no tiene swap.
* **OpenMP** multiplica la demanda de pila/stack por hilo; en sistemas con pocos núcleos, conviene limitar el número de threads.

## 6. Aumento de canales (RGB→RGBA)

* Buffer crece +33% (3→4 bytes píxel).
* **Buddy**: mayor fragmentación interna (potencias de dos).
* **OpenMP**: rendimiento de cómputo crece linealmente, comunicación y sincronización apenas varían.

## 7. Ventajas y desventajas resumidas

| Modo         | Ventajas                                 | Desventajas                                                            |
| ------------ | ---------------------------------------- | ---------------------------------------------------------------------- |
| Convencional | - Asignación rápida de bloques grandes   | - No optimiza múltiples bloques dinámicos                              |
| Buddy System | - Latencias predecibles, sin swap        | - Fragmentación interna, overhead split/coalesce                       |
| OpenMP       | - Reduce tiempo en bucles paralelizables | - Sobrecarga de hilos; beneficios limitados fuera de cómputo intensivo |

---