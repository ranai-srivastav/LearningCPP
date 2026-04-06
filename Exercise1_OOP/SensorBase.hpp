#pragma once

#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <variant>
#include <format>
#include <iostream>


enum class SensorType {
    RAW_IMU, 
    FILTERED_IMU,
    LIDAR,
    CAMERA,
    UNKNOWN
    
};

using SensorReading_t = std::variant<int16_t, int32_t, double>;

const std::array<double, 4> SensorReadings = {1.2, 2.3, 3.4, 4.5};

std::string to_string(SensorType st);

class SensorBase{

protected:
    std::string _identifier = "";
    SensorType _sensorType = SensorType::UNKNOWN;
    SensorReading_t _lastVal;
    double _freq = -1;

public:

    SensorBase(const std::string& identifier, SensorType sensorType, double freq) {
        this->_freq = freq;
        this->_identifier = identifier;
        this->_sensorType = sensorType;
    }

    virtual ~SensorBase() = default;

    virtual std::string name() final {         // QUESTION: Considering that this is in the base class and would bemarked final, do I need to say virtual final?
        return this->_identifier;              // QUESTION: Can I mark a function "constexpr noexcept final", i.e. multiple keywords
    }

    virtual std::string type_label() const {
        return to_string(this->_sensorType);
    }

    virtual void read() = 0;

    virtual SensorReading_t last_value() const = 0;

    virtual double hz() {
        return this->_freq;
    }

    virtual void describe() const { // How can I make this final? How is this (no keyword) different from virtual final?
        std::string last_str = std::visit([](auto&& val) {
            return std::format("{}", val);
        }, this->_lastVal);

        std::printf("[%s] %s @ %f Hz - last: %s", to_string(this->_sensorType).c_str(), this->_identifier.c_str(), this->_freq, last_str.c_str());
    }

};

class SensorObserver {
public:
    virtual void on_threshold (const std::string& sensor_name, double value) = 0;
};

class PrintObserver : public SensorObserver {

public:
    PrintObserver() = default;    
    void on_threshold(const std::string& sensor_name, double value) override;
};
