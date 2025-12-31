set(CEF_ROOT "" CACHE PATH "Root directory of CEF installation")

if(WIN32)
    set(CEF_PLATFORM "windows")
    set(CEF_LIB_SUFFIX ".lib")
    set(CEF_DLL_SUFFIX ".dll")
    set(CEF_WRAPPER_LIB_NAME "libcef_dll_wrapper")
    set(CEF_LIB_NAME "libcef")
elseif(APPLE)
    set(CEF_PLATFORM "macos")
    set(CEF_LIB_SUFFIX ".a")
    set(CEF_WRAPPER_LIB_NAME "libcef_dll_wrapper")
    set(CEF_LIB_NAME "cef")
else()
    set(CEF_PLATFORM "linux")
    set(CEF_LIB_SUFFIX ".a")
    set(CEF_WRAPPER_LIB_NAME "libcef_dll_wrapper")
    set(CEF_LIB_NAME "cef")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CEF_BUILD_TYPE "Debug")
else()
    set(CEF_BUILD_TYPE "Release")
endif()

find_path(CEF_INCLUDE_DIR
        NAMES cef_app.h
        HINTS
        ${CEF_ROOT}/include
        ${CEF_ROOT}
        $ENV{CEF_ROOT}/include
        $ENV{CEF_ROOT}
        /usr/local/include/cef
        /usr/include/cef
        /opt/cef/include
)

if(CEF_INCLUDE_DIR)
    get_filename_component(CEF_INCLUDE_DIR_PARENT "${CEF_INCLUDE_DIR}" DIRECTORY)
    if(CEF_INCLUDE_DIR MATCHES "include$")
        set(CEF_ROOT_DIR "${CEF_INCLUDE_DIR_PARENT}")
    else()
        set(CEF_ROOT_DIR "${CEF_INCLUDE_DIR}")
    endif()
endif()

if(WIN32)
    find_library(CEF_LIBRARY
            NAMES libcef
            HINTS
            ${CEF_ROOT}/${CEF_BUILD_TYPE}
            ${CEF_ROOT}/Debug
            ${CEF_ROOT}/Release
            ${CEF_ROOT_DIR}/${CEF_BUILD_TYPE}
            ${CEF_ROOT_DIR}/Debug
            ${CEF_ROOT_DIR}/Release
            $ENV{CEF_ROOT}/${CEF_BUILD_TYPE}
            $ENV{CEF_ROOT}/Debug
            $ENV{CEF_ROOT}/Release
    )

    find_library(CEF_WRAPPER_LIBRARY
            NAMES libcef_dll_wrapper
            HINTS
            ${CEF_ROOT}/${CEF_BUILD_TYPE}
            ${CEF_ROOT}/Debug
            ${CEF_ROOT}/Release
            ${CEF_ROOT}/libcef_dll_wrapper/${CEF_BUILD_TYPE}
            ${CEF_ROOT}/build/libcef_dll_wrapper/${CEF_BUILD_TYPE}
            ${CEF_ROOT_DIR}/libcef_dll_wrapper/${CEF_BUILD_TYPE}
            ${CEF_ROOT_DIR}/build/libcef_dll_wrapper/${CEF_BUILD_TYPE}
            ${CEF_ROOT_DIR}/${CEF_BUILD_TYPE}
            $ENV{CEF_ROOT}/libcef_dll_wrapper/${CEF_BUILD_TYPE}
            $ENV{CEF_ROOT}/build/libcef_dll_wrapper/${CEF_BUILD_TYPE}
    )

    find_file(CEF_DLL
            NAMES libcef.dll
            HINTS
            ${CEF_ROOT}/${CEF_BUILD_TYPE}
            ${CEF_ROOT}/Debug
            ${CEF_ROOT}/Release
            ${CEF_ROOT_DIR}/${CEF_BUILD_TYPE}
            ${CEF_ROOT_DIR}/Release
            $ENV{CEF_ROOT}/${CEF_BUILD_TYPE}
            $ENV{CEF_ROOT}/Debug
            $ENV{CEF_ROOT}/Release
    )
elseif(APPLE)
    find_library(CEF_LIBRARY
            NAMES "Chromium Embedded Framework"
            HINTS
            ${CEF_ROOT}/${CEF_BUILD_TYPE}
            ${CEF_ROOT}/Release
            ${CEF_ROOT}
            ${CEF_ROOT_DIR}/${CEF_BUILD_TYPE}
            ${CEF_ROOT_DIR}/Release
            ${CEF_ROOT_DIR}
            $ENV{CEF_ROOT}/${CEF_BUILD_TYPE}
            $ENV{CEF_ROOT}/Release
            $ENV{CEF_ROOT}
            PATH_SUFFIXES
            "Chromium Embedded Framework.framework"
    )

    find_library(CEF_WRAPPER_LIBRARY
            NAMES cef_dll_wrapper libcef_dll_wrapper
            HINTS
            ${CEF_ROOT}/libcef_dll_wrapper
            ${CEF_ROOT}/build/libcef_dll_wrapper/${CEF_BUILD_TYPE}
            ${CEF_ROOT}/build/libcef_dll_wrapper/Release
            ${CEF_ROOT_DIR}/libcef_dll_wrapper
            ${CEF_ROOT_DIR}/build/libcef_dll_wrapper/${CEF_BUILD_TYPE}
            $ENV{CEF_ROOT}/libcef_dll_wrapper
            $ENV{CEF_ROOT}/build/libcef_dll_wrapper/${CEF_BUILD_TYPE}
    )
else()
    find_library(CEF_LIBRARY
            NAMES cef libcef
            HINTS
            ${CEF_ROOT}/${CEF_BUILD_TYPE}
            ${CEF_ROOT}/Release
            ${CEF_ROOT}
            ${CEF_ROOT_DIR}/${CEF_BUILD_TYPE}
            ${CEF_ROOT_DIR}/Release
            ${CEF_ROOT_DIR}
            $ENV{CEF_ROOT}/${CEF_BUILD_TYPE}
            $ENV{CEF_ROOT}/Release
            $ENV{CEF_ROOT}
            /usr/local/lib
            /usr/lib
            /usr/lib/x86_64-linux-gnu
            /opt/cef/lib
    )

    find_library(CEF_WRAPPER_LIBRARY
            NAMES cef_dll_wrapper libcef_dll_wrapper
            HINTS
            ${CEF_ROOT}/libcef_dll_wrapper
            ${CEF_ROOT}/build/libcef_dll_wrapper
            ${CEF_ROOT_DIR}/libcef_dll_wrapper
            ${CEF_ROOT_DIR}/build/libcef_dll_wrapper
            $ENV{CEF_ROOT}/libcef_dll_wrapper
            $ENV{CEF_ROOT}/build/libcef_dll_wrapper
            /usr/local/lib
            /usr/lib
            /opt/cef/lib
    )

    find_file(CEF_SO
            NAMES libcef.so
            HINTS
            ${CEF_ROOT}/${CEF_BUILD_TYPE}
            ${CEF_ROOT}/Release
            ${CEF_ROOT}
            ${CEF_ROOT_DIR}/${CEF_BUILD_TYPE}
            ${CEF_ROOT_DIR}/Release
            ${CEF_ROOT_DIR}
            $ENV{CEF_ROOT}/${CEF_BUILD_TYPE}
            $ENV{CEF_ROOT}/Release
            $ENV{CEF_ROOT}
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CEF
        REQUIRED_VARS CEF_LIBRARY CEF_WRAPPER_LIBRARY CEF_INCLUDE_DIR
)

if(CEF_FOUND)
    set(CEF_INCLUDE_DIRS ${CEF_INCLUDE_DIR})
    if(CEF_INCLUDE_DIR MATCHES "include$")
        get_filename_component(CEF_PARENT_DIR "${CEF_INCLUDE_DIR}" DIRECTORY)
        list(APPEND CEF_INCLUDE_DIRS ${CEF_PARENT_DIR})
    endif()

    set(CEF_LIBRARIES ${CEF_WRAPPER_LIBRARY} ${CEF_LIBRARY})

    if(NOT TARGET CEF::CEF)
        add_library(CEF::CEF UNKNOWN IMPORTED)
        set_target_properties(CEF::CEF PROPERTIES
                IMPORTED_LOCATION "${CEF_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${CEF_INCLUDE_DIRS}"
        )
        if(WIN32 AND CEF_DLL)
            set_target_properties(CEF::CEF PROPERTIES
                    IMPORTED_IMPLIB "${CEF_LIBRARY}"
            )
        endif()
    endif()

    if(NOT TARGET CEF::Wrapper)
        add_library(CEF::Wrapper STATIC IMPORTED)
        set_target_properties(CEF::Wrapper PROPERTIES
                IMPORTED_LOCATION "${CEF_WRAPPER_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${CEF_INCLUDE_DIRS}"
        )
        if(TARGET CEF::CEF)
            set_target_properties(CEF::Wrapper PROPERTIES
                    INTERFACE_LINK_LIBRARIES CEF::CEF
            )
        endif()
    endif()

    if(WIN32 AND CEF_DLL)
        set(CEF_RUNTIME_LIBRARY ${CEF_DLL})
    elseif(UNIX AND NOT APPLE AND CEF_SO)
        set(CEF_RUNTIME_LIBRARY ${CEF_SO})
    endif()
endif()

mark_as_advanced(CEF_INCLUDE_DIR CEF_LIBRARY CEF_WRAPPER_LIBRARY CEF_DLL CEF_SO)

