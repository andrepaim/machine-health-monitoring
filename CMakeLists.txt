cmake_minimum_required(VERSION 3.5)
project(mqtt_samples)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMONGOC REQUIRED libmongoc-1.0)
pkg_check_modules(LIBBSON REQUIRED libbson-1.0)

include_directories(${LIBMONGOC_INCLUDE_DIRS} ${LIBBSON_INCLUDE_DIRS})

find_package(PahoMqttCpp REQUIRED)

add_executable(sensor_monitor sensor_monitor.cpp)
target_link_libraries(sensor_monitor PahoMqttCpp::paho-mqttpp3)

add_executable(data_processor data_processor.cpp)
target_link_libraries(data_processor PahoMqttCpp::paho-mqttpp3)
target_link_libraries(data_processor ${LIBMONGOC_LIBRARIES} ${LIBBSON_LIBRARIES})
