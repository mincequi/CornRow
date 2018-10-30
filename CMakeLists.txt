cmake_minimum_required(VERSION 3.10)

project(cornrow)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

find_package(Qt5Core REQUIRED)

add_subdirectory(ble)
add_subdirectory(common)
add_subdirectory(cornrowd)
add_subdirectory(thirdparty/bluez-qt)
add_subdirectory(thirdparty/gstreamermm-dsp)