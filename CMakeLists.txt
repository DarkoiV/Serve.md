cmake_minimum_required(VERSION 3.22) 
project(servemd)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Og)
endif()

add_executable(${PROJECT_NAME})
add_custom_target(run 
    COMMAND ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/example")
add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(source)
