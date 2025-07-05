import time

ITERATIONS = 100_000_000 # Reduced for Python's performance

# --- 1. "Virtual Function"-like Dispatch (Standard Python Polymorphism) ---

class Base:
    def do_work(self):
        raise NotImplementedError

class Derived(Base):
    def do_work(self):
        pass # In Python, 'pass' is the closest to a no-op

print("1. Polymorphism (Virtual-like) Benchmark...")
derived_obj = Derived()
start_virtual = time.time()
for _ in range(ITERATIONS):
    derived_obj.do_work()
end_virtual = time.time()
print(f"   Total time: {end_virtual - start_virtual:.6f} seconds")


# --- 2. "Function Pointer"-like Dispatch (First-Class Function) ---

def work_function():
    pass

print("\n2. Direct Function Call Benchmark...")
func_ptr = work_function
start_func_ptr = time.time()
for _ in range(ITERATIONS):
    func_ptr()
end_func_ptr = time.time()
print(f"   Total time: {end_func_ptr - start_func_ptr:.6f} seconds")


# --- 3. Wrapper (Functor) Dispatch ---

class InnerObject:
    def action(self):
        pass

class Wrapper:
    def __init__(self, inner):
        self._inner = inner
    
    def __call__(self):
        self._inner.action()

print("\n3. Wrapper (Functor) Benchmark...")
inner_obj = InnerObject()
wrapper = Wrapper(inner_obj)
start_wrapper = time.time()
for _ in range(ITERATIONS):
    wrapper()
end_wrapper = time.time()
print(f"   Total time: {end_wrapper - start_wrapper:.6f} seconds")