# C++ Dispatch Pattern Performance Benchmark

This repository contains a set of benchmarks designed to compare the performance of different function dispatch patterns in C++, with additional comparisons in Python and Rust.

The goal is to measure the overhead of each method in a high-performance scenario, simulating the demands of an audio engine or similar real-time application.

## The Patterns

The following four patterns are benchmarked in C++:

1.  **Virtual Functions**: Classic C++ dynamic polymorphism using an abstract base class.
2.  **Function Pointers**: Direct calls via a function pointer.
3.  **Template Wrapper**: A generic wrapper that uses templates (static polymorphism) to eliminate overhead.
4.  **Type Erasure**: A technique using a `void*` pointer and a function pointer adapter to "erase" the type of the contained object, allowing for runtime flexibility.

## How to Run

### C++

You will need a C++ compiler like `g++` or `clang++`.

```bash
# To compile and run without optimizations
g++ -O0 -std=c++17 benchmark.cpp -o benchmark_no_opt && ./benchmark_no_opt

# To compile and run with full optimizations (recommended)
g++ -O3 -std=c++17 benchmark.cpp -o benchmark_opt && ./benchmark_opt
```

### Python

You will need Python 3.

```bash
python3 benchmark.py
```

### Rust

You will need the Rust compiler, `rustc`.

```bash
# To compile and run without optimizations
rustc -C opt-level=0 benchmark.rs -o benchmark_no_opt_rs && ./benchmark_no_opt_rs

# To compile and run with full optimizations (recommended)
rustc -C opt-level=3 benchmark.rs -o benchmark_opt_rs && ./benchmark_opt_rs
```

## Benchmark Results

The most relevant results are from the C++ benchmark compiled with full optimizations (`-O3`). The times represent the total for executing 2,000,000,000 function calls.

| Method | Time with Optimization (-O3) |
| :--- | :--- |
| 1. Virtual Functions | ~1.39 s |
| 2. Function Pointers | **~0.45 s** |
| 3. Template Wrapper | **~0.45 s** |
| 4. Type Erasure | **~0.45 s** |

## Conclusion

The **Type Erasure** technique is the clear winner for applications that require both high performance and runtime flexibility. It provides:

1.  **Runtime Flexibility**: The ability to reassign a single wrapper object to different underlying types at runtime.
2.  **Static Dispatch Performance**: With optimizations, the compiler completely eliminates the abstraction overhead, resulting in performance identical to a direct function pointer call and far superior to virtual functions.
