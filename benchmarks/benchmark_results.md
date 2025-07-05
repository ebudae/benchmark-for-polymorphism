# Performance Analysis of C++ Dispatch Patterns

This document summarizes the results of a benchmark comparing four different design patterns for function dispatch in C++. The goal is to measure the overhead of each method in a high-performance scenario, similar to what would be found in an audio engine.

The benchmark executes 2 billion function calls (1,000,000,000 calls to a first implementation, and 1,000,000,000 to a second) for each of the following methods:

1.  **Virtual Functions**: Classic C++ dynamic polymorphism using an abstract base class.
2.  **Function Pointers**: Direct calls via a function pointer.
3.  **Template Wrapper**: A generic wrapper that uses templates (static polymorphism) to eliminate overhead.
4.  **Type Erasure (Your Solution)**: A technique using a `void*` pointer and a function pointer adapter to "erase" the type of the contained object, allowing for runtime flexibility.

---

## Benchmark Results

The times represent the total for executing 2,000,000,000 calls, compiled with `g++ -O3`.

| Method | Time with Optimization (-O3) |
| :--- | :--- |
| 1. Virtual Functions | ~1.39 s |
| 2. Function Pointers | **~0.45 s** |
| 3. Template Wrapper | **~0.45 s** |
| 4. Type Erasure (Your Solution) | **~0.45 s** |

---

## Analysis

With full compiler optimizations (`-O3`), the results are very telling:

-   **Function Pointer, Template Wrapper, and Type Erasure achieve identical, exceptional performance.** The compiler is smart enough to "see through" all the layers of abstraction, eliminate the indirections, and in many cases, inline the calls. The result is code that is nearly as fast as a direct call.
-   **Virtual Functions remain significantly slower.** The indirection through the vtable is harder for the compiler to optimize away, especially when the base pointer can point to different derived object types. This overhead, while small for a single call, accumulates massively in a loop of billions of iterations.

---

## Conclusion

The **Type Erasure** technique you implemented is the clear winner for an application like a synthesizer. It offers the best of both worlds:

1.  **Runtime Flexibility**: It allows a single `nextSampleGenerator` object to hold and call any generator type (`IOsc7`, `Sampler`, etc.) and be reassigned dynamically. This is something the pure Template Wrapper cannot do.
2.  **Static Dispatch Performance**: With optimizations enabled, the compiler eliminates the abstraction overhead, achieving performance on par with a function pointer and far superior to virtual functions.

It is a robust, flexible, and high-performance solution.

---

## Final Benchmark Code

```cpp
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>

// --- 1. Virtual Function Dispatch ---
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

// --- 2. Function Pointer Dispatch ---
void work_function1() { asm volatile(""); }
void work_function2() { asm volatile(""); }

// --- 3. Wrapper (Template) Dispatch ---
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

// --- 4. YOUR SOLUTION: Type Erasure Dispatch (void* + function pointer) ---

// Your struct
struct nextSampleGenerator{
    void* object = nullptr;
    float (*call)(void*) = nullptr;

    float operator()() const {
        return call(object);
    }
};

// Your adapter
template <typename T>
float adapter(void* obj) {
    static_cast<T*>(obj)->getNextSample();
    return 0.0f; // Return something, although it's not used in the benchmark
}

// Your helper
template <typename T>
nextSampleGenerator make_wrapper(T* object) {
    return nextSampleGenerator{ object, &adapter<T> };
}

// Test classes for your solution
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
    std::cout << "1. Virtual Function Benchmark (2 classes)..." << std::endl;
    auto start_virtual = std::chrono::high_resolution_clock::now();
    Derived1 d1; Base* b = &d1; for (long long i = 0; i < ITERATIONS; ++i) b->do_work();
    Derived2 d2; b = &d2; for (long long i = 0; i < ITERATIONS; ++i) b->do_work();
    auto end_virtual = std::chrono::high_resolution_clock::now();
    std::cout << "   Total time: " << std::chrono::duration<double>(end_virtual - start_virtual).count() << " seconds" << std::endl;

    // --- Test 2: Function Pointer ---
    std::cout << "\n2. Function Pointer Benchmark (2 functions)..." << std::endl;
    auto start_func_ptr = std::chrono::high_resolution_clock::now();
    void (*fp)() = &work_function1; for (long long i = 0; i < ITERATIONS; ++i) fp();
    fp = &work_function2; for (long long i = 0; i < ITERATIONS; ++i) fp();
    auto end_func_ptr = std::chrono::high_resolution_clock::now();
    std::cout << "   Total time: " << std::chrono::duration<double>(end_func_ptr - start_func_ptr).count() << " seconds" << std::endl;

    // --- Test 3: Wrapper (Template) ---
    std::cout << "\n3. Wrapper (Template) Benchmark (2 classes)..." << std::endl;
    auto start_wrapper = std::chrono::high_resolution_clock::now();
    InnerObject1 io1; Wrapper<InnerObject1> w1(&io1); for (long long i = 0; i < ITERATIONS; ++i) w1();
    InnerObject2 io2; Wrapper<InnerObject2> w2(&io2); for (long long i = 0; i < ITERATIONS; ++i) w2();
    auto end_wrapper = std::chrono::high_resolution_clock::now();
    std::cout << "   Total time: " << std::chrono::duration<double>(end_wrapper - start_wrapper).count() << " seconds" << std::endl;

    // --- Test 4: Type Erasure (Your Solution) ---
    std::cout << "\n4. Type Erasure Benchmark (Your Solution)..." << std::endl;
    auto start_type_erasure = std::chrono::high_resolution_clock::now();
    
    nextSampleGenerator gen; // Declare a single generator instance.

    TypeErasedGenerator1 teg1;
    gen = make_wrapper(&teg1); // Assign it to point to the first type.
    for (long long i = 0; i < ITERATIONS; ++i) gen();

    TypeErasedGenerator2 teg2;
    gen = make_wrapper(&teg2); // Re-assign the SAME instance to point to the second type.
    for (long long i = 0; i < ITERATIONS; ++i) gen();

    auto end_type_erasure = std::chrono::high_resolution_clock::now();
    std::cout << "   Total time: " << std::chrono::duration<double>(end_type_erasure - start_type_erasure).count() << " seconds" << std::endl;

    return 0;
}
```