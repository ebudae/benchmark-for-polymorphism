use std::time::{Instant, Duration};

const ITERATIONS: u64 = 1_000_000_000;

// --- 1. Dynamic Dispatch (like Virtual Functions) ---

trait DoWork {
    fn do_work(&self);
}

struct ConcreteWorker;

impl DoWork for ConcreteWorker {
    #[inline(never)] // Prevent the compiler from being too clever
    fn do_work(&self) {
        unsafe { std::arch::asm!(""); }
    }
}

fn run_dynamic_dispatch(worker: &dyn DoWork) {
    for _ in 0..ITERATIONS {
        worker.do_work();
    }
}

// --- 2. Function Pointer ---

#[inline(never)]
fn work_function() {
    unsafe { std::arch::asm!(""); }
}

fn run_function_pointer(func: fn()) {
    for _ in 0..ITERATIONS {
        func();
    }
}

// --- 3. Static Dispatch (Generic Wrapper) ---

struct InnerObject;

impl InnerObject {
    #[inline(never)]
    fn action(&self) {
        unsafe { std::arch::asm!(""); }
    }
}

// A generic function is the most idiomatic way to do this in Rust
fn run_static_dispatch<F: Fn()>(f: F) {
    for _ in 0..ITERATIONS {
        f();
    }
}


fn main() {
    // --- Test 1: Dynamic Dispatch ---
    println!("1. Dynamic Dispatch (dyn Trait) Benchmark...");
    let worker = ConcreteWorker;
    let start = Instant::now();
    run_dynamic_dispatch(&worker);
    let duration = start.elapsed();
    println!("   Total time: {:.6?} seconds", duration.as_secs_f64());

    // --- Test 2: Function Pointer ---
    println!("\n2. Function Pointer Benchmark...");
    let start = Instant::now();
    run_function_pointer(work_function);
    let duration = start.elapsed();
    println!("   Total time: {:.6?} seconds", duration.as_secs_f64());

    // --- Test 3: Static Dispatch (Generics) ---
    println!("\n3. Static Dispatch (Generics) Benchmark...");
    let inner = InnerObject;
    let wrapper = || inner.action(); // Using a closure is the most idiomatic approach
    let start = Instant::now();
    run_static_dispatch(wrapper);
    let duration = start.elapsed();
    println!("   Total time: {:.6?} seconds", duration.as_secs_f64());
}
