cmake_minimum_required(VERSION 3.10)
project(simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simulation 
    src/main.cpp 
    src/rng_utils.cpp 
    src/stream_utils.cpp 
    src/poisson.cpp
)

target_include_directories(simulation PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/pcg-cpp-0.98/include
)