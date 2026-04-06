#include "SensorBase.hpp"
#include "Sensors.hpp"

#include <iostream>
#include <memory>

int main() {

    // Step 1: Observer must be constructed before the pipeline (lifetime)
    PrintObserver observer;

    // Step 2: Primary pipeline + register observer
    SensorPipeline primary;
    primary.register_observer(&observer);

    // Step 3: Add sensors — must use make_unique + explicit std::move
    primary.add(std::make_unique<ImuSensor>("imu_0"));
    primary.add(std::make_unique<LidarSensor>("lidar_0", 100.0));
    primary.add(std::make_unique<CameraSensor>("cam_0", "1920x1080"));
    primary.add(std::make_unique<FilteredImu>("imu_filtered_0"));

    // Step 4: Secondary pipeline with one more lidar
    SensorPipeline secondary;
    secondary.add(std::make_unique<LidarSensor>("lidar_1", 200.0));

    // Step 5: Merge secondary into primary — secondary must be empty after
    primary.merge(std::move(secondary));

    // Step 6: Tick three times — observer should fire on any reading > 50.0
    primary.tick_all();
    primary.tick_all();
    primary.tick_all();

    // Step 7: Report — calls describe() on every sensor
    primary.report();

    // Step 8: Find by name and print last value via raw pointer
    SensorBase* found = primary.find("lidar_0");
    if (found) {
        std::cout << "Found: " << found->name()
                  << " last_value=" << found->last_value() << "\n";
    } else {
        std::cout << "lidar_0 not found\n";
    }

    // Step 9: Move the entire pipeline into a new owner
    SensorPipeline new_owner = std::move(primary);
    new_owner.report();

    // Step 10: Count IMU sensors — observe what FilteredImu returns
    unsigned int imu_count = new_owner.count_by_type("IMU");
    std::cout << "IMU count: " << imu_count << "\n";
    // Q: Does FilteredImu count here? Depends on what type_label() returns for it.

    return 0;
}
