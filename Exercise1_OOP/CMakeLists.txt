cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET( CMAKE_CXX_FLAGS  "-Wall -Werror -Wpedantic")


project(SensorSim)

add_executable(SensorSim
    SensorBase.hpp
    Sensors.hpp

    Sensors.cpp
    SensorPipeline.cpp
    Runner.cpp)



target_include_directories(SensorSim PUBLIC
    "${PROJECT_SOURCE_DIR}")

target_compile_features(SensorSim PUBLIC cxx_std_20)


