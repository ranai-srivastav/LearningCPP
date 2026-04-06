# C++ Cheat Sheet

## Value Categories

| Concept | Description |
|---|---|
| **lvalue** | Has a name and persists in memory beyond the current expression. Can appear on the left of `=`. Example: `int x = 5;` — `x` is an lvalue |
| **rvalue** | A temporary with no persistent name. About to be destroyed. Example: `5`, `x + y`, return values |
| **rvalue reference `&&`** | A reference that only binds to rvalues (temporaries). Signals "this object is about to die, you may steal its resources" |

---

## std:: Utilities

| Concept | Description |
|---|---|
| **`std::move(x)`** | Casts `x` to an rvalue reference. Does NOT move data — just enables the move constructor/assignment to be called. Leaves `x` in a valid but unspecified state |
| **`std::forward<T>(x)`** | Perfect forwarding — preserves the value category of `x` when passing through a template. Used in generic code |
| **`std::visit(fn, variant)`** | Calls `fn` with the currently held value in the variant. `fn` must handle all types in the variant — use a generic lambda for this |
| **`std::get<T>(variant)`** | Extracts the value of type `T` from a variant. Throws `std::bad_variant_access` if the variant doesn't currently hold `T` |
| **`std::move` (algorithm)** | `std::move(begin, end, dest)` — moves a range of elements into a destination. Different from the cast `std::move(x)` |

---

## Smart Pointers

| Concept | Description |
|---|---|
| **`std::unique_ptr<T>`** | Sole owner of a heap object. Not copyable — only movable. Zero overhead over a raw pointer. Destructor frees memory automatically |
| **`std::shared_ptr<T>`** | Shared ownership via reference counting. Copyable — each copy increments the count. Memory freed when count reaches 0. Has control block overhead |
| **`std::weak_ptr<T>`** | Non-owning observer of a `shared_ptr`. Does not increment ref count. Must be locked (`weak.lock()`) to access the object — returns null if already freed |
| **`std::make_unique<T>(args)`** | Preferred way to create a `unique_ptr`. Exception-safe, no raw `new` |
| **`std::make_shared<T>(args)`** | Preferred way to create a `shared_ptr`. Allocates object and control block in one allocation |

---

## Special Member Functions

| Concept | Description |
|---|---|
| **Copy constructor** | `T(const T& other)` — creates a new object as a deep copy of `other`. Called when passing by value or initializing from existing object |
| **Move constructor** | `T(T&& other)` — creates a new object by stealing resources from `other`. `other` left in valid but unspecified state |
| **Copy assignment** | `T& operator=(const T& other)` — replaces contents of existing object with a deep copy. Returns `*this` |
| **Move assignment** | `T& operator=(T&& other)` — replaces contents by stealing from `other`. Always check `this != &other`. Returns `*this` |
| **Destructor** | `~T()` — called when object goes out of scope or is deleted. Must be `virtual` in base classes if deleting through a base pointer |
| **`= default`** | Tells compiler to generate the default implementation |
| **`= delete`** | Explicitly prevents the compiler from generating this function |

---

## Polymorphism Keywords

| Concept | Description |
|---|---|
| **`virtual`** | Enables dynamic dispatch — the call is resolved at runtime via the vtable based on the actual object type, not the pointer type |
| **`= 0`** | Makes a function pure virtual — no default implementation, derived classes must implement it. Makes the class abstract (non-instantiable) |
| **`override`** | Declares that this function overrides a virtual function in the base. Compiler error if no matching virtual exists. Use on every overriding method |
| **`final` (on method)** | Prevents derived classes from overriding this method further |
| **`final` (on class)** | Prevents the class from being inherited |
| **`virtual final`** | Virtual (goes through vtable) but no further overriding allowed. Needed when declaring `final` in the base class itself |
| **vtable** | A per-class static table of function pointers to virtual method implementations. Each object holds a hidden `vptr` pointing to its class's vtable, set up during construction |

---

## Type System

| Concept | Description |
|---|---|
| **`std::variant<A, B, C>`** | A type-safe union — holds exactly one value of type A, B, or C at a time. No undefined behavior unlike raw unions |
| **`const T*`** | Pointer to a const T — the data cannot be modified, but the pointer can be reassigned |
| **`T* const`** | Const pointer to T — the pointer cannot be reassigned, but the data can be modified |
| **`const T* const`** | Both the pointer and the data are immutable |
| **Object slicing** | When a derived object is stored by value in a base type — the derived-specific data is "sliced off". Prevented by using pointers or references |

---

## Compilation Pipeline

| Stage | Input | Output | What it does |
|---|---|---|---|
| **Preprocessor** | `.cpp` + `.hpp` | `.ii` | Text substitution: expands `#include`, `#define`, `#pragma once`. Knows nothing about C++ |
| **Compiler** | `.ii` | `.s` | Parses C++, type-checks, instantiates templates, optimizes, emits assembly |
| **Assembler** | `.s` | `.o` | Converts assembly to machine code. Leaves external symbols unresolved |
| **Linker** | `.o` files | binary | Resolves all symbol references across object files. Catches missing or duplicate definitions |

---

## Build & Headers

| Concept | Description |
|---|---|
| **Translation unit** | One `.cpp` file + everything `#include`d into it after preprocessing. The compiler's unit of work — each produces one `.o` |
| **`#pragma once`** | Prevents a header from being included more than once in the same translation unit. Does not prevent multiple translation units from each including it |
| **`inline`** | Allows a function to be defined in a header included by multiple translation units without linker errors. Multiple definitions are expected and the linker keeps one |
| **Template definitions in headers** | Templates must be in headers because the compiler needs the full definition in the same translation unit where they are instantiated |
| **`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`** | Generates `compile_commands.json` — used by clangd (VSCode intellisense) to know your compiler flags and standard |

---

## Concurrency

### Core Concepts

| Concept | Description |
|---|---|
| **Data race** | Two threads access the same memory simultaneously and at least one writes — undefined behavior |
| **Critical section** | A block of code that must not be executed by more than one thread at a time |
| **Mutual exclusion (mutex)** | A lock that only one thread can hold at a time — all others block until it's released |
| **Deadlock** | Two or more threads each waiting for a lock the other holds — program hangs forever |

### Primitives

| Concept | Description |
|---|---|
| **`std::thread t(fn, args...)`** | Spawns a new thread running `fn`. Thread runs immediately on construction |
| **`t.join()`** | Blocks the calling thread until `t` finishes. Must be called before `t` is destroyed or program terminates |
| **`t.detach()`** | Lets the thread run independently — caller no longer responsible for it. Dangerous if thread outlives its data |
| **`std::mutex`** | Basic mutual exclusion lock. `lock()` blocks until acquired, `unlock()` releases. Never call these directly — use a guard |
| **`std::lock_guard<std::mutex> g(m)`** | RAII wrapper — locks `m` on construction, unlocks on destruction (scope exit). Preferred over manual lock/unlock |
| **`std::unique_lock<std::mutex> g(m)`** | Like `lock_guard` but more flexible — can unlock early, used with condition variables |
| **`std::atomic<T>`** | Lock-free thread-safe wrapper for simple types (int, bool, pointer). Cheaper than a mutex for single-variable access |

### Mutex and Class Design

| Rule | Reason |
|---|---|
| `std::mutex` is non-copyable and non-movable | Copying a mutex is undefined — what does it mean to copy a lock? |
| Copy constructor of a class with a mutex must lock the source | Otherwise you read data while another thread may be writing it |
| Move constructor of a class with a mutex should lock the source then transfer | Mutex itself cannot be moved — transfer the data, leave source in a safe empty state |
| `mutable std::mutex` | Needed when locking inside a `const` method — mutex must be modifiable even on const objects |
| Static mutex for static data | If the data being protected is `static`, the mutex must also be `static` — one lock for the one shared instance |

### Common Patterns

```cpp
// Basic lock_guard usage
std::mutex m;
void increment() {
    std::lock_guard<std::mutex> lock(m);  // locked here
    count++;
}                                          // unlocked here (destructor)

// Spawning and joining threads
std::vector<std::thread> pool;
for (int i = 0; i < 10; i++)
    pool.emplace_back(some_fn);
for (auto& t : pool)
    t.join();

// atomic for simple counters — no mutex needed
std::atomic<int> count{0};
count++;  // thread-safe
```

### thread_safety.cpp — Notes from Exercise

- `count` and `mutex_count` were made `static` — one shared instance across all `SafeCounter` objects. This is why the total reaches 10,000: all threads share the same counter
- `lock_guard` was commented out — without it, `count += 1` is a data race (read-modify-write is not atomic). Result is unpredictable and less than 10,000
- Copy/move constructors on a class with a mutex are tricky: the mutex cannot be copied or moved. You copy/move the data while holding the source's lock, and default-construct a fresh mutex in the new object
- Destructor concern in the exercise: if a thread is destroyed while holding a lock, the lock_guard destructor still runs (RAII) — the mutex is released. This is one of the key benefits of lock_guard over manual lock/unlock
