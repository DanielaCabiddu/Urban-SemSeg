cmake_minimum_required(VERSION 3.5)

project(bsp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option (USE_CEREAL OFF)

################## STXXL

add_definitions(-DSTXXL)

set (STXXL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/stxxl)
set (STXXL_BUILD ${STXXL_DIR}/build)
set (STXXL_LIB ${STXXL_BUILD}/lib/libstxxl.a)

file(MAKE_DIRECTORY ${STXXL_BUILD})

if(MSVC)
    set (STXXL_LIB ${STXXL_BUILD}/lib/Release/stxxl.lib)

    add_custom_command(
              OUTPUT ${STXXL_LIB}
              COMMAND cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . --config Release ##${CMAKE_BUILD_TYPE}
              #DEPENDS ${SOURCE_FILES} /tmp/bin/create_foo_hh main.cpp
              WORKING_DIRECTORY ${STXXL_BUILD}
            )

    else()
            add_custom_command(
                    OUTPUT ${STXXL_LIB}
                    COMMAND cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build .
                    #DEPENDS ${SOURCE_FILES} /tmp/bin/create_foo_hh main.cpp
                    WORKING_DIRECTORY ${STXXL_BUILD}
            )
endif()

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../external/stxxl/include)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../external/stxxl/build/include)

################## LIBLAS

set (LIBLAS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/libLAS)
set (LIBLAS_BUILD ${LIBLAS_DIR}/build)

file(MAKE_DIRECTORY ${LIBLAS_BUILD})

if (APPLE)
    set (LIBLAS_LIB ${LIBLAS_BUILD}/bin/Release/liblas.dylib)
elseif (UNIX)
    set (LIBLAS_LIB ${LIBLAS_BUILD}/bin/Release/liblas.so)
    elseif (MSVC)
        set (LIBLAS_LIB ${LIBLAS_BUILD}/bin/Release/las.dll)
endif()

add_custom_command(
  OUTPUT ${LIBLAS_LIB}
  COMMAND cmake -DCMAKE_BUILD_TYPE=Release -DWITH_GEOTIFF=OFF -DBUILD_OSGEO4W=OFF .. && cmake --build .
  #DEPENDS ${SOURCE_FILES} /tmp/bin/create_foo_hh main.cpp
  WORKING_DIRECTORY ${LIBLAS_BUILD}
)

find_package(Boost)

if (NOT BOOST_FOUND AND MSVC)
    if(NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})
       if(NOT DEFINED ENV{VCPKG_ROOT})
           if(WIN32)
               set(VCPKG_ROOT $ENV{HOMEDRIVE}$ENV{HOMEPATH}/vcpkg_cpptruths)
           else()
               set(VCPKG_ROOT $ENV{HOME}/.vcpkg_cpptruths)
           endif()
       else()
           set(VCPKG_ROOT $ENV{VCPKG_ROOT})
       endif()

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
                   COMMAND ${VCPKG_EXEC} install boost-iostreams --triplet x64-windows-static --recurse
                   WORKING_DIRECTORY ${VCPKG_ROOT})

               execute_process(
                           COMMAND ${VCPKG_EXEC} install boost-program-options --triplet x64-windows-static --recurse
                           WORKING_DIRECTORY ${VCPKG_ROOT})

                       execute_process(
                                   COMMAND ${VCPKG_EXEC} install boost-serialization --triplet x64-windows-static --recurse
                                   WORKING_DIRECTORY ${VCPKG_ROOT})

                               execute_process(
                                           COMMAND ${VCPKG_EXEC} install boost-thread --triplet x64-windows-static --recurse
                                           WORKING_DIRECTORY ${VCPKG_ROOT})

       message ("Setting Boost MACROS")
       set (Boost_INCLUDE_DIRS "${VCPKG_ROOT}/packages/" )

   endif()
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../external/libLAS/include)

######### EXTERNALS

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/point-cloud)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../external/OOCTriTile/include)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../external/tclap/include)

if (USE_CEREAL)
    add_definitions(-DUSE_CEREAL)
    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../external/cereal/include)
endif()


if(MSVC)
    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../../external/dirent_win)
endif()

add_executable(${PROJECT_NAME} main.cpp ${STXXL_LIB} ${LIBLAS_LIB})

target_link_libraries(${PROJECT_NAME} ${STXXL_LIB} ${LIBLAS_LIB})
