cmake_minimum_required(VERSION 3.20)

project(comm_bridge)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(COMM_BRIDGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(extern/pybind11)

find_package(Python 3 COMPONENTS Interpreter Development REQUIRED)

set(COMM_BRIDGE_CPP_SRC_DIR "${COMM_BRIDGE_DIR}/cpp/src/")
set(COMM_BRIDGE_PY_SRC_DIR "${COMM_BRIDGE_DIR}/python/")


file(GLOB_RECURSE SOURCE_FILES ${COMM_BRIDGE_CPP_SRC_DIR}/*.cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(COMM_BRIDGE_LIB_NAME "Comm_Bridge")


add_library(${COMM_BRIDGE_LIB_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${COMM_BRIDGE_LIB_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${COMM_BRIDGE_PY_SRC_DIR}/lib")

target_link_libraries(${COMM_BRIDGE_LIB_NAME} PUBLIC pybind11::module)

target_include_directories(${COMM_BRIDGE_LIB_NAME}
    PUBLIC pybind11_INCLUDE_DIRS
    PUBLIC extern/serial/include
)

pybind11_extension(${COMM_BRIDGE_LIB_NAME})

set(MY_SOURCES
    ${COMM_BRIDGE_CPP_SRC_DIR}/Communicator.cpp
    ${COMM_BRIDGE_CPP_SRC_DIR}/Messages.cpp
    ${COMM_BRIDGE_CPP_SRC_DIR}/SerialCommunicator.cpp
)

set(COMM_BRIDGE_PY_MODULE "${COMM_BRIDGE_PY_SRC_DIR}/lib/*.so")
