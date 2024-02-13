cmake_minimum_required(VERSION 3.5)

project(ransac LANGUAGES CXX)

##TCLAP
set (TCLAP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/tclap/include)
include_directories (${TCLAP_DIR})

##RANSAC
set (RANSAC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/RANSAC)

if (MSVC)
set (RANSAC_LIB ${RANSAC_DIR}/build/liblibransac.lib)
else()
    set (RANSAC_LIB ${RANSAC_DIR}/build/liblibransac.a)
endif()

add_custom_command(
    OUTPUT ${RANSAC_LIB}
    COMMAND mkdir -p build && cd build && cmake .. && cmake --build .
    WORKING_DIRECTORY ${RANSAC_DIR}
)

include_directories (${RANSAC_DIR})

##
add_executable(${PROJECT_NAME} main.cpp ${RANSAC_LIB}
    pc_reader.h
    pc_reader.cpp)

target_link_libraries (${PROJECT_NAME} PRIVATE ${RANSAC_LIB})
