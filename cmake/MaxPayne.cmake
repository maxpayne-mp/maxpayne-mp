include_guard(GLOBAL)

if(NOT WIN32)
    message(FATAL_ERROR "maxpayne so pode ser compilado para Windows")
endif()

get_filename_component(MAXPAYNE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de build" FORCE)
endif()

option(MAXPAYNE_ENABLE_LTO "Otimizacao em tempo de link no Release" ON)

function(maxpayne_configure target standard)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD ${standard}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        PREFIX "")

    target_compile_definitions(${target} PRIVATE
        UNICODE
        _UNICODE
        _CRT_SECURE_NO_WARNINGS)

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W3
            $<$<COMPILE_LANGUAGE:CXX>:/permissive->
            $<$<CONFIG:Release>:/Gy>)
        target_link_options(${target} PRIVATE /DEBUG)
    endif()

    foreach(config IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${config}" upper)
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${upper} "${MAXPAYNE_ROOT}/${config}"
            LIBRARY_OUTPUT_DIRECTORY_${upper} "${MAXPAYNE_ROOT}/${config}"
            ARCHIVE_OUTPUT_DIRECTORY_${upper} "${MAXPAYNE_ROOT}/${config}")
    endforeach()

    if(MAXPAYNE_ENABLE_LTO)
        set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    endif()
endfunction()
