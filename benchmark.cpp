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

    // --- Test 4: Type Erasure (Tu solución) ---
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
