# Análisis de Rendimiento de Patrones de Despacho en C++

Este documento resume los resultados de un benchmark que compara cuatro patrones de diseño diferentes para el despacho de llamadas en C++. El objetivo es medir la sobrecarga de cada método en un escenario de alto rendimiento, similar al que se encontraría en un motor de audio.

El benchmark ejecuta 2 mil millones de llamadas a funciones (1,000,000,000 de llamadas a una primera implementación, y 1,000,000,000 a una segunda) para cada uno de los siguientes métodos:

1.  **Funciones Virtuales**: El polimorfismo dinámico clásico de C++ usando una clase base abstracta.
2.  **Punteros a Función**: Llamadas directas a través de un puntero a función.
3.  **Wrapper con Plantillas**: Un wrapper genérico que usa plantillas (polimorfismo estático) para eliminar la sobrecarga.
4.  **Type Erasure (Tu Solución)**: Una técnica que usa un puntero `void*` y un puntero a una función adaptadora para "borrar" el tipo del objeto contenido, permitiendo flexibilidad en tiempo de ejecución.

---

## Resultados del Benchmark

Los tiempos representan el total para ejecutar 2,000,000,000 de llamadas.

| Método | Tiempo sin Optimización (-O0) | Tiempo con Optimización (-O3) |
| :--- | :--- | :--- |
| 1. Funciones Virtuales | ~1.88 s | ~1.39 s |
| 2. Punteros a Función | ~1.81 s | **~0.45 s** |
| 3. Wrapper (Plantilla) | ~3.78 s | **~0.45 s** |
| 4. Type Erasure (Tu Solución) | ~4.85 s | **~0.45 s** |

---

## Análisis

### Sin Optimización (-O0)

-   Sin optimizaciones, cada capa de indirección añade un coste notable. 
-   El **Wrapper con Plantillas** y la solución de **Type Erasure** son los más lentos porque implican múltiples saltos de puntero que el compilador no resuelve.
-   Las **Funciones Virtuales** y los **Punteros a Función** tienen un rendimiento similar, dominado por una única indirección (la búsqueda en la vtable o el salto a través del puntero a función).

### Con Optimización (-O3)

-   Aquí es donde se revela el poder del compilador moderno.
-   **Puntero a Función, Wrapper con Plantillas y Type Erasure obtienen un rendimiento idéntico y excepcional.** El compilador es capaz de "ver" a través de todas las capas de abstracción, eliminar las indirecciones y, en muchos casos, inlinear las llamadas. El resultado es un código casi tan rápido como una llamada directa.
-   Las **Funciones Virtuales** siguen siendo significativamente más lentas. La indirección a través de la vtable es más difícil de optimizar para el compilador, especialmente cuando el puntero base puede apuntar a diferentes tipos de objetos derivados. Esta sobrecarga, aunque pequeña para una sola llamada, se acumula masivamente en un bucle de mil millones de iteraciones.

---

## Conclusión

La técnica de **Type Erasure** que has implementado es la ganadora clara para una aplicación como un sintetizador. Ofrece lo mejor de ambos mundos:

1.  **Flexibilidad en Tiempo de Ejecución**: Permite que un único objeto `nextSampleGenerator` pueda contener y llamar a cualquier tipo de generador (`IOsc7`, `Sampler`, etc.) y ser reasignado dinámicamente, algo que el Wrapper con Plantillas no permite.
2.  **Rendimiento de Despacho Estático**: Con las optimizaciones activadas, el compilador elimina la sobrecarga de la abstracción, logrando un rendimiento a la par con una llamada a través de un puntero a función, y muy superior al de las funciones virtuales.

Es una solución robusta, flexible y de máximo rendimiento.

---

## Código Final del Benchmark

```cpp
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>

// --- 1. Despacho con Función Virtual ---
class Base {
public:
    virtual ~Base() = default;
    virtual void do_work() = 0;
};
class Derived1 : public Base {
public:
    void do_work() override { asm volatile(""); }
};
class Derived2 : public Base {
public:
    void do_work() override { asm volatile(""); }
};

// --- 2. Despacho con Puntero a Función ---
void work_function1() { asm volatile(""); }
void work_function2() { asm volatile(""); }

// --- 3. Despacho con Wrapper (Plantilla) ---
template<typename T>
class Wrapper {
private:
    T* inner;
public:
    Wrapper(T* obj) : inner(obj) {}
    void operator()() { inner->action(); }
};
class InnerObject1 {
public:
    void action() { asm volatile(""); }
};
class InnerObject2 {
public:
    void action() { asm volatile(""); }
};

// --- 4. TU SOLUCIÓN: Despacho con Type Erasure (void* + puntero a función) ---

// Tu struct
struct nextSampleGenerator{
    void* objeto = nullptr;
    float (*call)(void*) = nullptr;

    float operator()() const {
        return call(objeto);
    }
};

// Tu adaptador
template <typename T>
float adaptador(void* obj) {
    static_cast<T*>(obj)->getNextSample();
    return 0.0f; // Devolvemos algo, aunque no se use en el benchmark
}

// Tu helper
template <typename T>
nextSampleGenerator hacer_wrapper(T* objeto) {
    return nextSampleGenerator{ objeto, &adaptador<T> };
}

// Clases de prueba para tu solución
class TypeErasedGenerator1 {
public:
    void getNextSample() { asm volatile(""); }
};

class TypeErasedGenerator2 {
public:
    void getNextSample() { asm volatile(""); }
};


// --- Benchmark ---

int main() {
    const long long ITERATIONS = 1000000000;

    // --- Test 1: Virtual Function ---
    std::cout << "1. Benchmark de Función Virtual (2 clases)..." << std::endl;
    auto start_virtual = std::chrono::high_resolution_clock::now();
    Derived1 d1; Base* b = &d1; for (long long i = 0; i < ITERATIONS; ++i) b->do_work();
    Derived2 d2; b = &d2; for (long long i = 0; i < ITERATIONS; ++i) b->do_work();
    auto end_virtual = std::chrono::high_resolution_clock::now();
    std::cout << "   Tiempo total: " << std::chrono::duration<double>(end_virtual - start_virtual).count() << " segundos" << std::endl;

    // --- Test 2: Function Pointer ---
    std::cout << "\n2. Benchmark de Puntero a Función (2 funciones)..." << std::endl;
    auto start_func_ptr = std::chrono::high_resolution_clock::now();
    void (*fp)() = &work_function1; for (long long i = 0; i < ITERATIONS; ++i) fp();
    fp = &work_function2; for (long long i = 0; i < ITERATIONS; ++i) fp();
    auto end_func_ptr = std::chrono::high_resolution_clock::now();
    std::cout << "   Tiempo total: " << std::chrono::duration<double>(end_func_ptr - start_func_ptr).count() << " segundos" << std::endl;

    // --- Test 3: Wrapper (Template) ---
    std::cout << "\n3. Benchmark de Wrapper (Template) (2 clases)..." << std::endl;
    auto start_wrapper = std::chrono::high_resolution_clock::now();
    InnerObject1 io1; Wrapper<InnerObject1> w1(&io1); for (long long i = 0; i < ITERATIONS; ++i) w1();
    InnerObject2 io2; Wrapper<InnerObject2> w2(&io2); for (long long i = 0; i < ITERATIONS; ++i) w2();
    auto end_wrapper = std::chrono::high_resolution_clock::now();
    std::cout << "   Tiempo total: " << std::chrono::duration<double>(end_wrapper - start_wrapper).count() << " segundos" << std::endl;

    // --- Test 4: Type Erasure (Tu Solución) ---
    std::cout << "\n4. Benchmark de Type Erasure (Tu Solución)..." << std::endl;
    auto start_type_erasure = std::chrono::high_resolution_clock::now();
    
    nextSampleGenerator gen; // Declaramos una única instancia del generador.

    TypeErasedGenerator1 teg1;
    gen = hacer_wrapper(&teg1); // La asignamos para que apunte al primer tipo.
    for (long long i = 0; i < ITERATIONS; ++i) gen();

    TypeErasedGenerator2 teg2;
    gen = hacer_wrapper(&teg2); // Re-asignamos la MISMA instancia para que apunte al segundo tipo.
    for (long long i = 0; i < ITERATIONS; ++i) gen();

    auto end_type_erasure = std::chrono::high_resolution_clock::now();
    std::cout << "   Tiempo total: " << std::chrono::duration<double>(end_type_erasure - start_type_erasure).count() << " segundos" << std::endl;

    return 0;
}
```