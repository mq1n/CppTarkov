cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(ExampleApp CXX)

file(GLOB ExampleApp_HEADERS
    "src/*.hpp"
)
file(GLOB ExampleApp_SOURCES
    "src/*.cpp"
)

add_executable(${PROJECT_NAME} WIN32 ${ExampleApp_HEADERS} ${ExampleApp_SOURCES})

target_link_libraries(${PROJECT_NAME} ws2_32 crypt32 Wldap32 ${EXTRA_LIBS} TarkovAPI)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_CXX_EXTENSIONS OFF)
