#include "Sensors.hpp"

    ImuSensor::ImuSensor(const std::string& identifier):
    SensorBase(identifier, SensorType::RAW_IMU, 200){
        SensorBase::_lastVal = -1.0;
        sensorReading_iter = SensorReadings.begin();
    }

    void ImuSensor::read() {
        std::cout << std::format("Read {}-{}", to_string(_sensorType), _identifier.c_str());
        _lastVal = *sensorReading_iter;
        ++sensorReading_iter;
        if (sensorReading_iter == SensorReadings.end()) {
            sensorReading_iter = SensorReadings.begin();
        }
    };

    SensorReading_t ImuSensor::last_value() const {
        return std::get<double>(SensorBase::_lastVal);
    }


///////////////// LIDAR SENSOR //////////////////////////////////

    LidarSensor::LidarSensor(const std::string& identifier, const int32_t max_range):
    SensorBase(identifier, SensorType::LIDAR, 10){
        maxRange = max_range;

        scanBuffer.resize(maxRange / 10);
        gen = std::mt19937(rd());
        get_uniform_random = std::uniform_int_distribution<>(0, 1000);
    }

    LidarSensor::LidarSensor(LidarSensor&& other) noexcept:
    SensorBase(other._identifier, other._sensorType, other.hz()){
        if(&other != this) {

            this->_freq= other._freq;
            this->_identifier= other._identifier;
            this->maxRange = other.maxRange;
            this->_sensorType = other._sensorType;
            this->_lastVal = other._lastVal;
            this->scanBuffer = std::move(other.scanBuffer);

            other._freq = 10;
            other.maxRange = -1;
            other._identifier = "";
            other._sensorType = SensorType::LIDAR;
            other._lastVal = -1;  
            other.scanBuffer.clear();
        }
    }


    LidarSensor& LidarSensor::operator=(LidarSensor&& other) noexcept {
        if(&other != this) {

            this->_freq= other._freq;
            this->_identifier= other._identifier;
            this->maxRange = other.maxRange;
            this->_sensorType = other._sensorType;
            this->_lastVal = other._lastVal;
            this->scanBuffer = std::move(other.scanBuffer);

            other._freq = 10;
            other.maxRange = -1;
            other._identifier = "";
            other._sensorType = SensorType::LIDAR;
            other._lastVal = -1;   
            other.scanBuffer.clear();
        }
        return *this;
    }

    void LidarSensor::read() {
        for(auto& val_i : LidarSensor::scanBuffer) {
            val_i = get_uniform_random(gen);
        }
    };
    
    SensorReading_t LidarSensor::last_value() const {
        return this->scanBuffer.at((this->scanBuffer).size() - 1);
    };

////////////////// Filtered IMU //////////////////////////
    void  FilteredImu::read() {

        head = (head + 1) % sensorBuffer.size();
        size_t currIdx = head;

        sensorBuffer.at(currIdx) = *ImuSensor::sensorReading_iter;
        if(sensorReading_iter == SensorReadings.end()) {
            sensorReading_iter = SensorReadings.begin();
        }
        else {
            sensorReading_iter++;
        }

        SensorReading_t sum = 0.;
        for(const SensorReading_t& val: sensorBuffer) {
            sum = std::get<double>(sum) + std::get<double>(val);
        }
        this->_lastVal = std::get<double>(sum) / 3.0;
    }

    FilteredImu::FilteredImu(const std::string& identifier): 
    ImuSensor(identifier) {
        // QUESTION: How do I invoke the parent constructor?
        this->_sensorType = SensorType::FILTERED_IMU;
        std::fill(sensorBuffer.begin(), sensorBuffer.end(), 0);
        head = 0;
    };

/////////////// Camera /////////////////////
    CameraSensor::CameraSensor(const std::string& identifier, const std::string& resolution):
        SensorBase(identifier, SensorType::CAMERA, 30){
        this->resolution = resolution;
    }

    void CameraSensor::read(){
        this->_lastVal = ++val;
    }

    void CameraSensor::describe() const {
        SensorBase::describe();
        std::cout << resolution << std::endl; 
    }

    SensorReading_t CameraSensor::last_value() const {
        return std::get<int32_t>(_lastVal);
    }

////////////// Print Observer //////////////////////
void PrintObserver::on_threshold(const std::string& sensor_name, double value) {
    std::cout << std::format("{} exceeds {} \n", sensor_name, value);
}

/////////// TO STRING //////////////////
std::string to_string(SensorType st){
    switch (st){
        case SensorType::RAW_IMU:
            return "RAW_IMU";
        case SensorType::FILTERED_IMU:
            return "FILTERED_IMU";
        case SensorType::LIDAR:
            return "LIDAR";
        case SensorType::CAMERA:
            return "CAMERA";
        default:
            return "Unknown";
    }
}