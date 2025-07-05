use std::time::{Instant, Duration};

const ITERATIONS: u64 = 1_000_000_000;

// --- 1. Despacho Dinámico (como Funciones Virtuales) ---

trait DoWork {
    fn do_work(&self);
}

struct ConcreteWorker;

impl DoWork for ConcreteWorker {
    #[inline(never)] // Prevenir que el compilador sea demasiado listo
    fn do_work(&self) {
        unsafe { std::arch::asm!(""); }
    }
}

fn run_dynamic_dispatch(worker: &dyn DoWork) {
    for _ in 0..ITERATIONS {
        worker.do_work();
    }
}

// --- 2. Puntero a Función ---

#[inline(never)]
fn work_function() {
    unsafe { std::arch::asm!(""); }
}

fn run_function_pointer(func: fn()) {
    for _ in 0..ITERATIONS {
        func();
    }
}

// --- 3. Despacho Estático (Wrapper genérico) ---

struct InnerObject;

impl InnerObject {
    #[inline(never)]
    fn action(&self) {
        unsafe { std::arch::asm!(""); }
    }
}

// Wrapper genérico que usa despacho estático
struct GenericWrapper<T> {
    inner: T,
}

impl<T: Fn()> FnOnce<()> for GenericWrapper<T> {
    type Output = ();
    extern "rust-call" fn call_once(self, _args: ()) -> Self::Output {
        (self.inner)();
    }
}

impl<T: FnMut()> FnMut<()> for GenericWrapper<T> {
    extern "rust-call" fn call_mut(&mut self, _args: ()) -> Self::Output {
        (self.inner)();
    }
}

fn run_static_dispatch<F: Fn()>(f: F) {
    for _ in 0..ITERATIONS {
        f();
    }
}


fn main() {
    // --- Test 1: Despacho Dinámico ---
    println!("1. Benchmark de Despacho Dinámico (dyn Trait)...");
    let worker = ConcreteWorker;
    let start = Instant::now();
    run_dynamic_dispatch(&worker);
    let duration = start.elapsed();
    println!("   Tiempo total: {:.6?} segundos", duration.as_secs_f64());

    // --- Test 2: Puntero a Función ---
    println!("\n2. Benchmark de Puntero a Función...");
    let start = Instant::now();
    run_function_pointer(work_function);
    let duration = start.elapsed();
    println!("   Tiempo total: {:.6?} segundos", duration.as_secs_f64());

    // --- Test 3: Despacho Estático (Genéricos) ---
    println!("\n3. Benchmark de Despacho Estático (Genéricos)...");
    let inner = InnerObject;
    let wrapper = || inner.action(); // Usamos un cierre, es lo más idiomático
    let start = Instant::now();
    run_static_dispatch(wrapper);
    let duration = start.elapsed();
    println!("   Tiempo total: {:.6?} segundos", duration.as_secs_f64());
}