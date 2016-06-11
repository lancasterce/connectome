cmake_minimum_required(VERSION 3.5)
project(connectome)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    connectome.csv
    main.cpp
    postsynaptic.csv
    synapse.cpp
    synapse.h)

add_executable(connectome ${SOURCE_FILES})