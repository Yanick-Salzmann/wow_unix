set(FMOD_ROOT "" CACHE PATH "Root directory of FMOD installation")

find_path(FMOD_INCLUDE_DIR
        NAMES fmod.hpp fmod.h
        HINTS
        ${FMOD_ROOT}/api/core/inc
        /usr/local/include/fmod
        /usr/include/fmod
        /opt/fmod/api/core/inc
        $ENV{FMOD_ROOT}/api/core/inc
)

find_library(FMOD_LIBRARY
        NAMES fmod fmodL
        HINTS
        ${FMOD_ROOT}/api/core/lib/x86_64
        /usr/local/lib
        /usr/lib
        /usr/lib/x86_64-linux-gnu
        /opt/fmod/api/core/lib/x86_64
        $ENV{FMOD_ROOT}/api/core/lib/x86_64
)

find_path(FMOD_STUDIO_INCLUDE_DIR
        NAMES fmod_studio.hpp fmod_studio.h
        HINTS
        ${FMOD_ROOT}/api/studio/inc
        /usr/local/include/fmod
        /usr/include/fmod
        /opt/fmod/api/studio/inc
        $ENV{FMOD_ROOT}/api/studio/inc
)

find_library(FMOD_STUDIO_LIBRARY
        NAMES fmodstudio fmodstudioL
        HINTS
        ${FMOD_ROOT}/api/studio/lib/x86_64
        /usr/local/lib
        /usr/lib
        /usr/lib/x86_64-linux-gnu
        /opt/fmod/api/studio/lib/x86_64
        $ENV{FMOD_ROOT}/api/studio/lib/x86_64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FMOD
        REQUIRED_VARS FMOD_LIBRARY FMOD_INCLUDE_DIR
)

if(FMOD_FOUND)
    set(FMOD_INCLUDE_DIRS ${FMOD_INCLUDE_DIR})
    set(FMOD_LIBRARIES ${FMOD_LIBRARY})

    if(FMOD_STUDIO_INCLUDE_DIR AND FMOD_STUDIO_LIBRARY)
        list(APPEND FMOD_INCLUDE_DIRS ${FMOD_STUDIO_INCLUDE_DIR})
        list(APPEND FMOD_LIBRARIES ${FMOD_STUDIO_LIBRARY})
        set(FMOD_STUDIO_FOUND TRUE)
    endif()

    if(NOT TARGET FMOD::Core)
        add_library(FMOD::Core UNKNOWN IMPORTED)
        set_target_properties(FMOD::Core PROPERTIES
                IMPORTED_LOCATION "${FMOD_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${FMOD_INCLUDE_DIR}"
        )
    endif()

    if(FMOD_STUDIO_FOUND AND NOT TARGET FMOD::Studio)
        add_library(FMOD::Studio UNKNOWN IMPORTED)
        set_target_properties(FMOD::Studio PROPERTIES
                IMPORTED_LOCATION "${FMOD_STUDIO_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${FMOD_STUDIO_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(FMOD_INCLUDE_DIR FMOD_LIBRARY FMOD_STUDIO_INCLUDE_DIR FMOD_STUDIO_LIBRARY)

