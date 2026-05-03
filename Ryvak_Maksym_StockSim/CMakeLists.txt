cmake_minimum_required(VERSION 3.10)
project(StockSim)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# Add all source files in src/ automatically
file(GLOB SOURCES "src/*.cpp")

add_executable(stocksim ${SOURCES} main.cpp)

# Enable warnings to help catch common bugs
target_compile_options(stocksim PRIVATE -Wall -Wextra)
