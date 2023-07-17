cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(gclub LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_library(headers INTERFACE
    include/gclub_model.hpp
    include/t_file_parser.hpp
)
target_include_directories(headers INTERFACE include)

add_executable(${PROJECT_NAME}
    src/gclub_model.cpp
    src/main.cpp
    src/t_file_parser.cpp
)

target_link_libraries(${PROJECT_NAME}
    headers
)
