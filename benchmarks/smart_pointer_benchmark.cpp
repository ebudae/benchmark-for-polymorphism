#include <iostream>
#include <chrono>
#include <memory> // For std::unique_ptr and std::shared_ptr
#include <vector>

// A simple object to allocate and deallocate
class MyObject {
public:
    int data; // To ensure it's not a zero-sized object

    MyObject() : data(0) {
        // Prevent compiler from optimizing away constructor calls
        asm volatile(""); 
    }
    ~MyObject() {
        // Prevent compiler from optimizing away destructor calls
        asm volatile("");
    }
};

int main() {
    const long long ITERATIONS = 100000000; // 100 million iterations

    std::cout << "Benchmarking Smart Pointer Overhead (Iterations: " << ITERATIONS << ")" << std::endl;

    // --- 1. Raw Pointers ---
    std::cout << "\n1. Raw Pointers (new/delete)..." << std::endl;
    auto start_raw = std::chrono::high_resolution_clock::now();
    for (long long i = 0; i < ITERATIONS; ++i) {
        MyObject* obj = new MyObject();
        // Simulate some work with the object to prevent optimization
        obj->data = i;
        asm volatile("");
        delete obj;
    }
    auto end_raw = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_raw = end_raw - start_raw;
    std::cout << "   Total time: " << duration_raw.count() << " seconds" << std::endl;

    // --- 2. std::unique_ptr ---
    std::cout << "\n2. std::unique_ptr..." << std::endl;
    auto start_unique = std::chrono::high_resolution_clock::now();
    for (long long i = 0; i < ITERATIONS; ++i) {
        std::unique_ptr<MyObject> obj = std::make_unique<MyObject>();
        // Simulate some work
        obj->data = i;
        asm volatile("");
    } // obj is destroyed here automatically
    auto end_unique = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_unique = end_unique - start_unique;
    std::cout << "   Total time: " << duration_unique.count() << " seconds" << std::endl;

    // --- 3. std::shared_ptr (Creation/Destruction) ---
    std::cout << "\n3. std::shared_ptr (Creation/Destruction)..." << std::endl;
    auto start_shared_create = std::chrono::high_resolution_clock::now();
    for (long long i = 0; i < ITERATIONS; ++i) {
        std::shared_ptr<MyObject> obj = std::make_shared<MyObject>();
        // Simulate some work
        obj->data = i;
        asm volatile("");
    } // obj is destroyed here automatically
    auto end_shared_create = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_shared_create = end_shared_create - start_shared_create;
    std::cout << "   Total time: " << duration_shared_create.count() << " seconds" << std::endl;

    // --- 4. std::shared_ptr (Copying Overhead) ---
    // This test measures the cost of copying a shared_ptr, which increments the reference count.
    // We create one shared_ptr and then copy it many times.
    std::cout << "\n4. std::shared_ptr (Copying Overhead)..." << std::endl;
    std::shared_ptr<MyObject> base_shared_ptr = std::make_shared<MyObject>();
    auto start_shared_copy = std::chrono::high_resolution_clock::now();
    for (long long i = 0; i < ITERATIONS; ++i) {
        std::shared_ptr<MyObject> copy_ptr = base_shared_ptr; // Copying the shared_ptr
        // Simulate some work with the copy
        copy_ptr->data = i;
        asm volatile("");
    } // copy_ptr is destroyed here, decrementing ref count
    auto end_shared_copy = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_shared_copy = end_shared_copy - start_shared_copy;
    std::cout << "   Total time: " << duration_shared_copy.count() << " seconds" << std::endl;

    return 0;
}