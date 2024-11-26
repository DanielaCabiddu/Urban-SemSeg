cmake_minimum_required(VERSION 3.5)

project(cgal LANGUAGES CXX)

##TCLAP
set (TCLAP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/tclap/include)
include_directories (${TCLAP_DIR})

find_package (Boost)

if (NOT Boost_FOUND AND MSVC)
    if(NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})
        #if(NOT DEFINED ENV{VCPKG_ROOT})
            #if(WIN32)
                set(VCPKG_ROOT $ENV{HOMEDRIVE}$ENV{HOMEPATH}/vcpkg_cpptruths)
            #else()
            #    set(VCPKG_ROOT $ENV{HOME}/.vcpkg_cpptruths)
            #endif()
        #else()
        #    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
        #endif()

        if(NOT EXISTS ${VCPKG_ROOT})
            message("Cloning vcpkg in ${VCPKG_ROOT}")
            execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
            # If a reproducible build is desired (and potentially old libraries are # ok), uncomment the
            # following line and pin the vcpkg repository to a specific githash.
            # execute_process(COMMAND git checkout 745a0aea597771a580d0b0f4886ea1e3a94dbca6 WORKING_DIRECTORY ${VCPKG_ROOT})
        else()
            # The following command has no effect if the vcpkg repository is in a detached head state.
            message("Auto-updating vcpkg in ${VCPKG_ROOT}")
            execute_process(COMMAND git pull WORKING_DIRECTORY ${VCPKG_ROOT})
        endif()

        if(NOT EXISTS ${VCPKG_ROOT}/README.md)
            message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg *****")
        endif()

        if(WIN32)
            set(BOOST_INCLUDEDIR ${VCPKG_ROOT}/installed/x86-windows/include)
            set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
            set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
        else()
            set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
            set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
        endif()

        if(NOT EXISTS ${VCPKG_EXEC})
            message("Bootstrapping vcpkg in ${VCPKG_ROOT}")
            execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
        endif()

        if(NOT EXISTS ${VCPKG_EXEC})
            message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
        endif()

        set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

        #message(STATUS "***** Checking project third party dependencies in ${VCPKG_ROOT} *****")
        set(VCPKG_PLATFORM_TOOLSET v142)

        execute_process(
            COMMAND ${VCPKG_EXEC} install boost --triplet x64-windows-static --recurse
            WORKING_DIRECTORY ${VCPKG_ROOT})

        execute_process(
            COMMAND ${VCPKG_EXEC} install liblas --triplet x64-windows-static --recurse
            WORKING_DIRECTORY ${VCPKG_ROOT})

        set (Boost_INCLUDE_DIRS "${VCPKG_ROOT}/installed/x64-windows-static/include" )
        set (Boost_INCLUDE_DIR "${Boost_INCLUDE_DIRS}" )


    endif()
endif()

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../external/CGAL-5.6.2/include)
find_package (CGAL REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/CGAL-5.6.2 NO_DEFAULT_PATH)

message(${CGAL_INCLUDE_DIRS})
include_directories(${CGAL_INCLUDE_DIRS})

set (LIBLAS_DIR ${CGAL_INCLUDE_DIRS}/../external/LAStools/LASlib)  ### https://github.com/CGAL/LAStools.git
include_directories(${LIBLAS_DIR}/inc)
message (${LIBLAS_DIR}/inc)

set (LIBZIP_DIR ${CGAL_INCLUDE_DIRS}/../external/LAStools/LASzip)
include_directories(${LIBZIP_DIR}/src)
message (${LIBZIP_DIR}/src)

include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../external/cinolib/external/eigen)

add_executable(${PROJECT_NAME}-shape-detection-ransac main-ransac.cpp)
add_executable(${PROJECT_NAME}-shape-detection-reg-growing main-fit.cpp)
add_executable(${PROJECT_NAME}-scaline-normals main-scanline-normals.cpp)

#target_link_libraries(${PROJECT_NAME}-ransac PUBLIC CGAL::CGAL CGAL::CGAL_Core)
target_link_libraries(${PROJECT_NAME}-scaline-normals PUBLIC ${LIBLAS_DIR}/../build-Windows/Release/las.lib)
