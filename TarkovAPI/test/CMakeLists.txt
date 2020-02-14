cmake_minimum_required(VERSION 3.2)
project(TarkovAPITests CXX)

file(GLOB TarkovAPITests_SOURCES
    "*.cpp"
)

# Test explorer doesn't works due than some reasons...
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../../Bin)

add_executable(${PROJECT_NAME} ${TarkovAPITests_SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_CXX_EXTENSIONS OFF)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER Tests
)

target_link_libraries(${PROJECT_NAME} ws2_32 crypt32 Wldap32 ${EXTRA_LIBS} TarkovAPI)

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
)