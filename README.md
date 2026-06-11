# Práctica 4 — API de Hilos
## Laboratorio de Sistemas Operativos
**Universidad de Antioquia — Facultad de Ingeniería — Ingeniería de Sistemas**

---

## 1. Integrantes

| Nombre completo | Correo | Documento |
|---|---|---|
| Yuliana Corrales Castaño | yuliana.corralesc@udea.edu.co | 39193015 |

---

## 2. Descripción general

Esta práctica implementa dos programas en C usando la API de hilos POSIX (Pthreads):

1. **pi.c / pi_p.c** — Cálculo de π mediante integración numérica, primero de forma serial y luego paralelizada con T hilos usando Data Parallelism.
2. **fibonacci.c** — Generación de la secuencia de Fibonacci usando un hilo trabajador que escribe en memoria compartida.

---

## 3. Estructura del repositorio

```
laboratorio4/
├── src/
│   ├── pi.c            # Versión serial del cálculo de π
│   ├── pi_p.c          # Versión paralela del cálculo de π
│   └── fibonacci.c     # Generador de secuencia de Fibonacci con hilos
├── analisis.ipynb      # Notebook con análisis de rendimiento
└── README.md           # Este archivo
```

---

## 4. Documentación de funciones

### `pi.c`

| Función | Descripción |
|---|---|
| `double f(double x)` | Evalúa el integrando `4 / (1 + x²)`. La integral de esta función en [0,1] es igual a π. |
| `double GetTime()` | Retorna el tiempo actual en segundos con precisión de nanosegundos usando `clock_gettime(CLOCK_MONOTONIC)`. |
| `double CalcPi(int n)` | Aproxima π dividiendo [0,1] en `n` rectángulos (regla del punto medio) y sumando sus áreas. Retorna la aproximación de π. |
| `int main(int argc, char *argv[])` | Recibe `n` por línea de comandos, mide el tiempo de ejecución de `CalcPi` e imprime el resultado y el tiempo. |

### `pi_p.c`

| Función | Descripción |
|---|---|
| `double f(double x)` | Igual que en `pi.c`. Evalúa `4 / (1 + x²)`. |
| `double GetTime()` | Igual que en `pi.c`. Retorna tiempo en segundos. |
| `void *funcion_hilo(void *arg)` | Función ejecutada por cada hilo. Recibe un `HiloArgs *` con su sub-rango `[inicio, fin)`. Calcula la suma parcial localmente sin mutex y la guarda en `args->resultado`. |
| `int main(int argc, char *argv[])` | Recibe `n` y `T` por línea de comandos. Divide el rango `[0, n)` entre `T` hilos, los crea con `pthread_create`, recolecta sus sumas parciales con `pthread_join` y calcula π final. |

**Estructura `HiloArgs`:**
```c
typedef struct {
    int inicio;       // primer índice del sub-rango
    int fin;          // último índice + 1 (exclusivo)
    int n;            // total de rectángulos global
    double resultado; // suma parcial calculada por el hilo
} HiloArgs;
```

### `fibonacci.c`

| Función | Descripción |
|---|---|
| `void *hilo_fibonacci(void *arg)` | Función del hilo trabajador. Recibe `FibArgs *` con el puntero al arreglo compartido y `N`. Genera los `N` primeros números de Fibonacci y los almacena en el arreglo. |
| `int main(int argc, char *argv[])` | Recibe `N` por línea de comandos. Asigna memoria dinámica con `malloc`, crea el hilo trabajador, espera con `pthread_join` y luego imprime el arreglo. |

**Estructura `FibArgs`:**
```c
typedef struct {
    long long *arreglo; // puntero al arreglo compartido
    int N;              // cantidad de elementos a generar
} FibArgs;
```

---

## 5. Compilación

```bash
cd laboratorio4/src

# Versión serial de π
gcc -o pi_s pi.c -lm

# Versión paralela de π
gcc -o pi_p pi_p.c -lpthread -lm

# Fibonacci
gcc -o fibonacci fibonacci.c -lpthread
```

---

## 6. Ejecución

```bash
# π serial con n rectángulos
./pi_s <n>

# π paralelo con n rectángulos y T hilos
./pi_p <n> <T>

# Fibonacci con N elementos
./fibonacci <N>
```

**Ejemplos:**
```bash
./pi_s 2000000000
./pi_p 2000000000 4
./fibonacci 15
```

---

## 7. Pruebas realizadas

### Prueba de correctitud — `pi_s`
```
$ ./pi_s 1000000
pi = 3.1415926536
Tiempo: 0.004521 segundos
```
✅ Valor correcto de π con 10 decimales.

### Prueba de correctitud — `pi_p`
```
$ ./pi_p 1000000 4
pi = 3.1415926536  (T=4 hilos)
Tiempo: 0.002341 segundos
```
✅ Mismo resultado que la versión serial.

### Prueba de correctitud — `fibonacci`
```
$ ./fibonacci 15
Secuencia de Fibonacci (15 elementos):
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377
```
✅ Secuencia correcta.

### Prueba de rendimiento — n = 2,000,000,000

| Versión | Hilos | Tiempo (s) |
|---|---|---|
| Serial (`pi_s`) | — | 5.906052 |
| Paralela (`pi_p`) | 1 | 5.965188 |
| Paralela (`pi_p`) | 2 | 3.629916 |
| Paralela (`pi_p`) | 4 | 2.294890 |
| Paralela (`pi_p`) | 8 | 1.592615 |

### Prueba de casos borde — `fibonacci`
```
$ ./fibonacci 1
Secuencia de Fibonacci (1 elementos):
0

$ ./fibonacci 2
Secuencia de Fibonacci (2 elementos):
0 1
```
✅ Maneja correctamente N=1 y N=2.

---

## 8. Problemas presentados y soluciones

| Problema | Causa | Solución |
|---|---|---|
| `undefined reference to main` al compilar | Los archivos `.c` se crearon vacíos desde VS Code antes de guardar el contenido | Se recrearon los archivos directamente desde la terminal con `cat > archivo.c << 'EOF'` |
| `Command 'code' not found` al ejecutar `code .` desde Ubuntu | VS Code no estaba instalado en Windows todavía | Se instaló VS Code en Windows y se conectó a WSL mediante la extensión WSL de Microsoft |
| `NameError: name 'null' is not defined` en el notebook | El JSON del notebook se pegó directamente en una celda de Python en lugar de guardarse como archivo `.ipynb` | Se generó el archivo correctamente con un script Python usando `json.dump` |
| `ValueError: Exceeds the limit (4300 digits)` en Fibonacci | Los valores de Fibonacci con N=200,000 tienen miles de dígitos y Python no puede convertirlos a string directamente | Se modificó la celda para imprimir solo los primeros 10 valores en lugar de los últimos |
| Usuario con mayúsculas rechazado en Ubuntu | Linux no acepta mayúsculas en nombres de usuario | Se usó `yulianacorrales` en minúsculas |
| WSL sin distribuciones instaladas | `wsl --install` no completó la instalación automática | Se habilitaron las características de Windows manualmente con `dism.exe` y se instaló Ubuntu 22.04 desde Microsoft Store |

---

## 9. Análisis de rendimiento

**CPU:** 8 núcleos lógicos (WSL2 sobre Windows)
**n:** 2,000,000,000 rectángulos

| N (Hilos) | Tp (segundos) | Speedup (Ts/Tp) | Eficiencia (Speedup/N) |
|---|---|---|---|
| 1 | 5.965188 | 0.9901 | 0.9901 |
| 2 | 3.629916 | 1.6270 | 0.8135 |
| 4 | 2.294890 | 2.5733 | 0.6433 |
| 8 | 1.592615 | 3.7081 | 0.4635 |

**Ts (serial) = 5.906052 segundos**

---

## 10. Conclusiones

1. **La paralelización mejora el rendimiento pero no de forma lineal.** Con 8 hilos se obtuvo un Speedup de ~3.7, lejos del ideal de 8. Esto confirma que existen costos inherentes al paralelismo que limitan la ganancia real.

2. **El overhead de Pthreads es real y medible.** `Tp(1) = 5.965s` es ligeramente mayor que `Ts = 5.906s`. La diferencia corresponde al costo de crear el hilo, inicializar la estructura `HiloArgs` y ejecutar `pthread_join`, costos que la versión serial no tiene.

3. **La eficiencia disminuye conforme aumenta el número de hilos.** Con 1 hilo la eficiencia es ~0.99 (casi perfecta), pero con 8 hilos cae a ~0.46. Esto se explica por la Ley de Amdahl: la parte no paralelizable del programa (setup, suma final) y los costos de sincronización ponen un techo al Speedup máximo.

4. **Evitar mutex dentro del bucle es clave para el rendimiento.** La estrategia de acumular sumas parciales en variables locales por hilo y combinarlas al final elimina la contención en el recurso compartido, que sería el cuello de botella más grande si se usara un mutex por cada iteración.

5. **`pthread_join` cumple un doble rol fundamental.** Además de sincronizar la finalización del hilo, garantiza la visibilidad de memoria: todos los escrituras realizadas por el hilo trabajador son visibles para `main` después de que `join` retorna, evitando condiciones de carrera sin necesidad de mecanismos adicionales.

6. **WSL2 introduce limitaciones en el rendimiento paralelo.** Al correr sobre una capa de virtualización sobre Windows, el acceso a los núcleos físicos no es tan directo como en Linux nativo. Esto puede explicar por qué el Speedup con 8 hilos no se acerca más al ideal teórico.

7. **El modelo de memoria compartida de Pthreads es eficiente para este tipo de problema.** Pasar un puntero al arreglo de Fibonacci al hilo trabajador evita cualquier copia de datos — ambos hilos acceden a la misma región de memoria, lo que es O(1) en costo de comunicación independientemente del tamaño de N.

---

## 11. Manifiesto de transparencia — Uso de IA generativa

Durante el desarrollo de esta práctica se utilizó **Claude (Anthropic)** como apoyo en los siguientes puntos:

- **Configuración del entorno:** Guía paso a paso para instalar WSL2, Ubuntu 22.04 y VS Code en Windows desde cero, incluyendo la resolución de errores de instalación.
- **Generación del código base:** Los archivos `pi.c`, `pi_p.c` y `fibonacci.c` fueron generados con asistencia de IA y revisados para verificar que cumplen con los requisitos del enunciado.
- **Depuración de errores:** Resolución de errores de compilación (`undefined reference to main`), errores de entorno (`Command 'code' not found`) y errores de ejecución en Python (`ValueError` de dígitos).
- **Generación del notebook:** La estructura del archivo `analisis.ipynb` y el código de análisis fueron generados con asistencia de IA.
- **Redacción del README:** Este documento fue generado con asistencia de IA a partir de los resultados reales obtenidos en la máquina.

Todo el código fue ejecutado, probado y verificado por la estudiante en su entorno local.

---

## 12. Referencias

- Arpaci-Dusseau, R. & Arpaci-Dusseau, A. *Operating Systems: Three Easy Pieces*.
  - Capítulo 26: Threads Intro
  - Capítulo 27: Thread API
- Documentación oficial de POSIX Threads: https://man7.org/linux/man-pages/man7/pthreads.7.html
- Manual de `pthread_create`: https://man7.org/linux/man-pages/man3/pthread_create.3.html
- Manual de `pthread_join`: https://man7.org/linux/man-pages/man3/pthread_join.3.html