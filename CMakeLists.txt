cmake_minimum_required(VERSION 3.5)
project(connectome)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    connectome.csv
    main.cpp
    postsynaptic.csv
    synapse.cpp
    synapse.h ring.cpp)

add_executable(connectome ${SOURCE_FILES})

#require MPI for this project:
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(connectome ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
    set_target_properties(connectome PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(connectome PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()