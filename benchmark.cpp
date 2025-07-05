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