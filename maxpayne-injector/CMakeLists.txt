cmake_minimum_required(VERSION 3.21)

project(maxpayne-launcher LANGUAGES CXX)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/MaxPayne.cmake")

add_executable(maxpayne-launcher main.cc)

maxpayne_configure(maxpayne-launcher 20)
