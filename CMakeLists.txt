cmake_minimum_required(VERSION 3.10)
project(MosaicMaker)

# 添加 Crow 依赖
include(FetchContent)
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow
    GIT_TAG v1.0+5
)
FetchContent_MakeAvailable(Crow)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system date_time)
set(CMAKE_CXX_STANDARD 17)

include_directories(${OpenCV_INCLUDE_DIRS})
file(GLOB SOURCES "src/*.cpp")

# 添加 WebUI 相关文件
add_executable(MosaicCpp src/main.cpp ${SOURCES})
target_link_libraries(MosaicCpp PRIVATE 
    ${OpenCV_LIBS}
    Crow::Crow
    pthread
)

# 复制静态文件到构建目录
file(COPY ${CMAKE_SOURCE_DIR}/webui/static DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/webui/templates DESTINATION ${CMAKE_BINARY_DIR})