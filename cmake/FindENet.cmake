include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

set(MAXPAYNE_ENET_ROOT "C:/enet-master" CACHE PATH "Pasta raiz do ENet")

find_path(ENET_INCLUDE_DIR
    NAMES enet/enet.h
    HINTS "${MAXPAYNE_ENET_ROOT}" ENV ENET_ROOT
    PATH_SUFFIXES include)

find_library(ENET_LIBRARY_RELEASE
    NAMES enet enet64
    HINTS "${MAXPAYNE_ENET_ROOT}" ENV ENET_ROOT
    PATH_SUFFIXES . Release build/Release build lib)

find_library(ENET_LIBRARY_DEBUG
    NAMES enetd enet enet64
    HINTS "${MAXPAYNE_ENET_ROOT}" ENV ENET_ROOT
    PATH_SUFFIXES Debug build/Debug . build lib)

select_library_configurations(ENET)

find_package_handle_standard_args(ENet REQUIRED_VARS ENET_LIBRARY ENET_INCLUDE_DIR)

if(ENet_FOUND AND NOT TARGET ENet::ENet)
    add_library(ENet::ENet UNKNOWN IMPORTED)
    set_target_properties(ENet::ENet PROPERTIES
        IMPORTED_LOCATION "${ENET_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ENET_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "ws2_32;winmm")
    if(ENET_LIBRARY_RELEASE)
        set_property(TARGET ENet::ENet APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(ENet::ENet PROPERTIES IMPORTED_LOCATION_RELEASE "${ENET_LIBRARY_RELEASE}")
    endif()
    if(ENET_LIBRARY_DEBUG)
        set_property(TARGET ENet::ENet APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
        set_target_properties(ENet::ENet PROPERTIES IMPORTED_LOCATION_DEBUG "${ENET_LIBRARY_DEBUG}")
    endif()
endif()

mark_as_advanced(ENET_INCLUDE_DIR ENET_LIBRARY_RELEASE ENET_LIBRARY_DEBUG)
