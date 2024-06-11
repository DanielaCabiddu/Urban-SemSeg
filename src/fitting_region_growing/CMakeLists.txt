cmake_minimum_required(VERSION 3.5)

project(fit_region_growing LANGUAGES CXX)

##TCLAP
set (TCLAP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/tclap/include)
include_directories (${TCLAP_DIR})

find_package(CGAL REQUIRED COMPONENTS Core)

add_executable(${PROJECT_NAME}-ransac main-ransac.cpp)
add_executable(${PROJECT_NAME}-fit main-fit.cpp)

target_link_libraries(${PROJECT_NAME}-ransac PUBLIC CGAL::CGAL CGAL::CGAL_Core)
target_link_libraries(${PROJECT_NAME}-fit PUBLIC CGAL::CGAL CGAL::CGAL_Core)
