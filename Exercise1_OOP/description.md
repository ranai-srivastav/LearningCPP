# Exercise 1: Multi-Sensor Pipeline

**Domain:** AV onboard sensor stack. You are building the software abstraction layer that manages a heterogeneous set of sensors.

---

## System Description

Design and implement a `SensorPipeline` that manages a collection of different sensor types polymorphically. The pipeline does not know or care which concrete sensor it holds — it just drives them through a common interface.

---

## Requirements

### 1. Abstract Base: `SensorBase`

Define an abstract class with the following interface:

- `name()` — returns a string identifier for the sensor (e.g. `"imu_0"`)
- `type_label()` — returns the sensor category as a string (e.g. `"IMU"`, `"LiDAR"`, `"Camera"`)
- `read()` — performs one read cycle; returns nothing, but updates internal state
- `last_value()` — returns the most recent reading as a `double` (simplified — one scalar per sensor)
- `hz()` — returns the sensor's nominal update rate in Hz

Rules:
- All five methods must be overridable by derived classes
- The class must be safe to delete through a base pointer
- `read()`, `last_value()`, and `hz()` are mandatory to implement in derived classes
- `name()` and `type_label()` — think about whether a default in the base makes sense, or whether they should be pure virtual

Also add a **non-virtual** method `describe()` on the base that prints one line:
```
[type_label] name @ hz Hz — last: value
```
This method calls the virtual methods internally. Do not implement it in derived classes.

---

### 2. Concrete Sensors

Implement three sensor types. Each stores its own internal state and simulates a reading.

**`ImuSensor`**
- Type label: `"IMU"`
- `hz()`: 200
- `read()`: generates a new simulated angular velocity (counter-based or small float — your choice)
- Constructor takes a `name` string

**`LidarSensor`**
- Type label: `"LiDAR"`
- `hz()`: 10
- `read()`: simulates a range reading (distance in meters)
- Constructor takes a `name` string and a `max_range` double
- Internally maintain a `std::vector<double>` scan buffer (pre-allocated to `max_range / 10` slots in the constructor — this is the resource you will move)
- Implement an explicit **move constructor** and **move assignment operator** that transfer the buffer and reset the moved-from object to a safe empty state

**`CameraSensor`**
- Type label: `"Camera"`
- `hz()`: 30
- `read()`: simulates an exposure or brightness value
- Constructor takes a `name` string and a `resolution` string (e.g. `"1920x1080"`)
- `describe()` should also append the resolution — figure out the right way to do this without breaking the base class contract

---

### 3. Derived Sensor: `FilteredImu`

Inherits from `ImuSensor`. Overrides `read()` to apply a **3-sample moving average** over the last three raw readings. `last_value()` returns the filtered value.

- Type label: `"IMU/Filtered"` — override `type_label()`
- Everything else inherits from `ImuSensor`

---

### 4. `SensorObserver`

Define an abstract observer interface:

- `on_threshold(const std::string& sensor_name, double value)` — called when a reading exceeds a threshold; pure virtual

Implement one concrete observer: `PrintObserver`, which prints:
```
ALERT [sensor_name]: value exceeded threshold (value)
```

The pipeline stores a **raw non-owning pointer** to the observer. The caller is responsible for ensuring the observer outlives the pipeline. Think carefully about what this implies for construction order in `main()`.

---

### 5. `SensorPipeline`

Owns a collection of sensors. Requirements:

- **Non-copyable, movable** — delete copy constructor and copy assignment; implement move constructor and move assignment
- `add(std::unique_ptr<SensorBase>)` — takes ownership; caller must explicitly `std::move()` in
- `tick_all()` — calls `read()` on every sensor; if a reading exceeds a fixed threshold (e.g. `50.0`), notify the observer if one is registered
- `report()` — calls `describe()` on every sensor
- `find(const std::string& name)` — returns a raw non-owning `SensorBase*` to the matching sensor, or `nullptr`
- `count_by_type(const std::string& type_label)` — returns how many sensors match that type label
- `merge(SensorPipeline&& other)` — moves all sensors out of `other` and into `this`; after the call, `other` must be empty
- `register_observer(SensorObserver* obs)` — stores the raw pointer; does not take ownership

---

### 6. `main()`

Demonstrate the full system:
1. Construct a `PrintObserver` — note where it lives and why order matters
2. Build a primary pipeline; register the observer
3. Add one `ImuSensor`, one `LidarSensor`, one `CameraSensor`, one `FilteredImu` — use `std::make_unique` and `std::move` explicitly
4. Build a secondary pipeline and add one more `LidarSensor` to it
5. Merge the secondary pipeline into the primary
6. Call `tick_all()` three times
7. Call `report()`
8. Use `find()` to retrieve a sensor by name and print its last value via the raw pointer
9. Move the entire primary pipeline into a new variable; call `report()` on the new owner
10. Print the count of `"IMU"` sensors — observe what result you get for `FilteredImu` and reason about it

---

## Concepts This Exercises

**OOP / Polymorphism**
- Pure virtual / abstract base classes
- `virtual` destructor — and what breaks without it
- `override` keyword — use it on every overriding method; understand what it enforces
- The difference between **overriding** and **hiding**
- Non-virtual interface (NVI) pattern via `describe()`
- Observer pattern with a raw non-owning pointer

**Pointers**
- Raw non-owning pointers for observers — lifetime responsibility is on the caller
- `SensorBase*` returned from `find()` — safe to use, unsafe to delete
- Why storing polymorphic objects by value causes object slicing (you won't do it, but understand why `unique_ptr` prevents it)

**Move Semantics**
- `unique_ptr` passed into `add()` — forces explicit `std::move()` at the call site
- Explicit move constructor + move assignment on `LidarSensor` — transferring the scan buffer
- Move-only `SensorPipeline` — `merge()` drains sensors out of the source pipeline
- Moving the pipeline itself in step 9 — what happens to the moved-from pipeline?

---

## Constraints

- C++17
- No third-party libraries
- Use `override` on every overriding method
- No raw owning pointers anywhere
- `SensorPipeline` must be move-only (copy operations deleted)
