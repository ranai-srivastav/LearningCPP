#pragma once

#include "SensorBase.hpp"
#include <iostream>
#include <format>
#include <vector>
#include <random>


/// @brief Implements the basic IMU sensor
class ImuSensor: public SensorBase {

protected:
    std::array<double, 4>::const_iterator sensorReading_iter;

public:
    ImuSensor(const std::string& identifier);
    void read() override;
    SensorReading_t last_value() const override;

};

class FilteredImu: public ImuSensor {
    std::array<SensorReading_t, 3> sensorBuffer;
    size_t head = 0;

    public:
    FilteredImu(const std::string& identifier);

    void read() override;

};


/// @brief Implemented a LIDAR sensor with move semantics
class LidarSensor: public SensorBase {

private:
    std::vector<SensorReading_t> scanBuffer;
    int32_t maxRange;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> get_uniform_random;


public:
    LidarSensor(const std::string& identifier, const int32_t max_range);

    // Move Constructor
    LidarSensor(LidarSensor&& other) noexcept;

    // Move assignment operator
    LidarSensor& operator=(LidarSensor&& other) noexcept;

    void read() override;
    SensorReading_t last_value() const override;

};

class CameraSensor: public SensorBase {
private:
    short val = 0; 
    std::string resolution = "";   
public:
    CameraSensor(const std::string& identifier, const std::string& resolution);
    void read() override;
    void describe() const override;
    SensorReading_t last_value() const override;

};