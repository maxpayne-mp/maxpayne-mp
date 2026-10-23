cmake_minimum_required(VERSION 3.21)

project(maxpayne-mp LANGUAGES C CXX)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/MaxPayne.cmake")

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "maxpayne-mp precisa ser compilado em 32 bits (use -A Win32 ou -DMAXPAYNE_BUILD_MP=OFF)")
endif()

find_package(ENet REQUIRED)

add_library(maxpayne-mp SHARED
    main.cc
    lib/mouse.cpp
    lib/utils.cpp
    lib/safetyhook.cpp
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_impl_dx9.cpp
    lib/imgui/imgui_impl_win32.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/Zydis/Zydis.c)

maxpayne_configure(maxpayne-mp 23)

target_compile_definitions(maxpayne-mp PRIVATE
    MAXPAYNEMP_EXPORTS
    _USRDLL)

target_link_libraries(maxpayne-mp PRIVATE
    ENet::ENet
    ws2_32
    winmm
    dbghelp
    psapi
    shell32
    gdi32
    dwmapi
    d3d9)

if(MSVC)
    target_link_options(maxpayne-mp PRIVATE /MANIFESTUAC:NO)
endif()
