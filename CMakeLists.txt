cmake_minimum_required(VERSION 3.5)
project(mqtt_samples)

set(CMAKE_CXX_STANDARD 11)

find_package(PahoMqttCpp REQUIRED)

add_executable(sensor_monitor sensor_monitor.cpp)
target_link_libraries(sensor_monitor PahoMqttCpp::paho-mqttpp3)

add_executable(data_processor data_processor.cpp)
target_link_libraries(data_processor PahoMqttCpp::paho-mqttpp3)
