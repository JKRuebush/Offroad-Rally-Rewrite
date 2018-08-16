cmake_minimum_required(VERSION 3.0.0)
project(OffroadRally VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(OffroadRally src/main.cpp src/map/rallyMap.cpp)
target_include_directories(OffroadRally PUBLIC includes)
target_compile_features(OffroadRally PUBLIC cxx_std_11)
