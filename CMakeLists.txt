cmake_minimum_required(VERSION 3.10)
project(MosaicMaker)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(${OpenCV_INCLUDE_DIRS})
file(GLOB SOURCES "src/*.cpp")

add_executable(mosaic ${SOURCES})
target_link_libraries(mosaic ${OpenCV_LIBS} pthread)
