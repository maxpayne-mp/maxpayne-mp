cmake_minimum_required(VERSION 3.21)

project(maxpayne LANGUAGES C CXX)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/MaxPayne.cmake")

option(MAXPAYNE_BUILD_MP "Compilar maxpayne-mp" ON)
option(MAXPAYNE_BUILD_SERVER "Compilar maxpayne-server" ON)
option(MAXPAYNE_BUILD_LAUNCHER "Compilar maxpayne-launcher" ON)

if(MAXPAYNE_BUILD_MP)
    add_subdirectory(maxpayne-mp)
endif()

if(MAXPAYNE_BUILD_SERVER)
    add_subdirectory(maxpayne-server)
endif()

if(MAXPAYNE_BUILD_LAUNCHER)
    add_subdirectory(maxpayne-injector)
endif()
