import time

ITERATIONS = 100_000_000

# --- 1. Despacho tipo "Función Virtual" (Polimorfismo estándar de Python) ---

class Base:
    def do_work(self):
        raise NotImplementedError

class Derived(Base):
    def do_work(self):
        pass  # En Python, 'pass' es lo más cercano a no hacer nada

print("1. Benchmark de Polimorfismo (similar a Virtual)...")
derived_obj = Derived()
start_virtual = time.time()
for _ in range(ITERATIONS):
    derived_obj.do_work()
end_virtual = time.time()
print(f"   Tiempo total: {end_virtual - start_virtual:.6f} segundos")


# --- 2. Despacho con "Puntero a Función" (Función de primera clase) ---

def work_function():
    pass

print("\n2. Benchmark de Llamada a Función Directa...")
func_ptr = work_function
start_func_ptr = time.time()
for _ in range(ITERATIONS):
    func_ptr()
end_func_ptr = time.time()
print(f"   Tiempo total: {end_func_ptr - start_func_ptr:.6f} segundos")


# --- 3. Despacho con Wrapper (Functor) ---

class InnerObject:
    def action(self):
        pass

class Wrapper:
    def __init__(self, inner):
        self._inner = inner
    
    def __call__(self):
        self._inner.action()

print("\n3. Benchmark de Wrapper (Functor)...")
inner_obj = InnerObject()
wrapper = Wrapper(inner_obj)
start_wrapper = time.time()
for _ in range(ITERATIONS):
    wrapper()
end_wrapper = time.time()
print(f"   Tiempo total: {end_wrapper - start_wrapper:.6f} segundos")
