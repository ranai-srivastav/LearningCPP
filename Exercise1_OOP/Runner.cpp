#include "SensorPipeline.cpp"
#include "Sensors.hpp"
#include <format>

int main() {

    PrintObserver po;
    (void) po;

    std::unique_ptr<ImuSensor> imuRaw = std::make_unique<ImuSensor>("imu1");
    std::unique_ptr<FilteredImu> filteredIMU = std::make_unique<FilteredImu>("better_imu");
    std::unique_ptr<CameraSensor> cam = std::make_unique<CameraSensor>("cam", "640x480");
    std::unique_ptr<LidarSensor> li = std::make_unique<LidarSensor>("velodyne", 15);
    std::unique_ptr<LidarSensor> dar = std::make_unique<LidarSensor>("ouster", 100);

    SensorPipeline pipe = SensorPipeline();
    pipe.register_observer(&po);
    pipe.add(std::move(imuRaw));
    pipe.add(std::move(filteredIMU));
    pipe.add(std::move(cam));
    pipe.add(std::move(li));

    SensorPipeline line = SensorPipeline();
    line.add(std::move(dar));
    pipe.merge(std::move(line));  // QUESTION: line does not exist after this

    pipe.tick_all();
    pipe.report();
    
    std::cout << std::format("Found {} of type lidar \n", pipe.count_by_type(SensorType::LIDAR));

    SensorPipeline other_pipeline = SensorPipeline(std::move(pipe));
    other_pipeline.report();
    std::cout << std::format("Found {} of type IMU \n", pipe.count_by_type(SensorType::RAW_IMU));

}