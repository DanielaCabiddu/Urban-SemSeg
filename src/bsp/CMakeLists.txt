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

find_package(Boost REQUIRED)
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
