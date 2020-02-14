cmake_minimum_required(VERSION 3.2)
project(TarkovAPI CXX)

file(GLOB TarkovAPI_HEADERS
    "src/*.hpp"
)
file(GLOB TarkovAPI_SOURCES
    "src/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${TarkovAPI_HEADERS} ${TarkovAPI_SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(${PROJECT_SOURCE_DIR}/test)
