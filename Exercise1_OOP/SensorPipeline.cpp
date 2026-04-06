#include "SensorBase.hpp"
#include <memory>

class SensorPipeline {
private:
    std::vector<std::unique_ptr<SensorBase>> sensorList;
    SensorObserver* sensorObserver;

public:
    SensorPipeline(const SensorPipeline& other) = delete;
    SensorPipeline& operator=(const SensorPipeline& other) = delete;

    SensorPipeline() : sensorObserver(nullptr) {}

    SensorBase* find(const std::string& name) {
        for(auto& sb: sensorList) {
            if(sb->name() == name) return sb.get();
        }
        return nullptr;
    }

    SensorPipeline(SensorPipeline&& other) noexcept {
        if(&other != this) {
            this->sensorList = std::move(other.sensorList);
            this->sensorObserver = std::move(other.sensorObserver);

            other.sensorList.clear();
            other.sensorObserver = nullptr;
        }
    }

    SensorPipeline& operator=(SensorPipeline&& other){
        if(this != &other) { // QUESTION: Why does this line work? `this` is of type T* and T&& is a reference to a reference so T** other the hood. &other emans address of T** to give T***?  

            this->sensorList = std::move(other.sensorList);
            this->sensorObserver = std::move(other.sensorObserver);

            other.sensorList.clear();
            other.sensorObserver = nullptr;
        }
        return *this;
    }

    void add(std::unique_ptr<SensorBase> sensor){
        sensorList.push_back(std::move(sensor));
    }

    void tick_all(){ 
        for(std::unique_ptr<SensorBase>& sb: sensorList) {
            sb->read();
            double sensorVal;
            std::visit([&](auto&& val) {                                                                                                                                                                                                                  
                sensorVal = static_cast<double>(val);                                                                                                                                                                                        
            }, sb->last_value()); 

            if(sensorVal >= 50.) {
                if(this->sensorObserver != nullptr) {
                    this->sensorObserver->on_threshold(sb->name(), sensorVal);
                }
            }
        } 
    }

    void report() const {
        for(const std::unique_ptr<SensorBase>& sb: sensorList) {
            sb->describe(); // SensorBase has a public describe. Why is this complaining???
        } 
    }

    unsigned int count_by_type(const SensorType& type_label) const {
        unsigned int count = 0;
        for(const std::unique_ptr<SensorBase>& sb: sensorList) {
            if(sb->type_label() == to_string(type_label)) {
                ++count;
            } 
        } 
        return count;
    }

    void merge(SensorPipeline&& other){
        this->sensorList.reserve(this->sensorList.size() + other.sensorList.size());

        for(std::unique_ptr<SensorBase>& sensor: other.sensorList){
            this->sensorList.push_back(std::move(sensor));
        }

        other.sensorList.clear();
    }

    void register_observer(SensorObserver* obs) {
        this->sensorObserver = obs;
    }
};