cmake_minimum_required(VERSION 3.21)

project(maxpayne-server LANGUAGES CXX)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/MaxPayne.cmake")

find_package(ENet REQUIRED)

add_executable(maxpayne-server main.cc)

maxpayne_configure(maxpayne-server 23)

target_link_libraries(maxpayne-server PRIVATE
    ENet::ENet
    ws2_32
    winmm)
