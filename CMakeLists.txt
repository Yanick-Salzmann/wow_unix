cmake_minimum_required(VERSION 3.24) # DO NOT overwrite this line
project(wow_unix)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

if (WIN32)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

list(APPEND OPENGL_LIBRARIES OpenGL)
list(APPEND OPENGL_LINKED_LIBRARIES OpenGL::GL)
if (UNIX)
    list(APPEND OPENGL_LIBRARIES EGL)
    list(APPEND OPENGL_LINKED_LIBRARIES OpenGL::EGL)
endif ()

find_package(OpenGL REQUIRED COMPONENTS ${OPENGL_LIBRARIES})

if (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
endif ()
find_package(FMOD REQUIRED)

find_package(CEF REQUIRED)

include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG master
)


FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG master
)

FetchContent_Declare(
        StormLib
        GIT_REPOSITORY https://github.com/ladislav-zezula/StormLib.git
        GIT_TAG master
)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)

FetchContent_Declare(
        boost_di
        GIT_REPOSITORY https://github.com/boost-ext/di.git
        GIT_TAG v1.2.0
)

FetchContent_Declare(
        boost_pfr
        GIT_REPOSITORY https://github.com/boostorg/pfr.git
        GIT_TAG 2.2.0
)

FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG v3.4.0
)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)

set(STORMLIB_BUILD_TYPE_BACKUP ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE Release)
FetchContent_MakeAvailable(StormLib)
set(CMAKE_BUILD_TYPE ${STORMLIB_BUILD_TYPE_BACKUP})
target_compile_definitions(storm PUBLIC __STORMLIB_NO_STATIC_LINK__)

FetchContent_MakeAvailable(spdlog glfw glm stb boost_di boost_pfr tomlplusplus nlohmann_json)

execute_process(COMMAND python3 -m venv .venv COMMAND .venv/bin/pip3 install -r requirements.txt WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/glad)
include(glad/cmake/CMakeLists.txt)
glad_add_library(glad_s3tc SHARED API gl:core=4.6 EXTENSIONS GL_EXT_texture_compression_s3tc)

add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/mime_types.txt
        COMMAND ${CMAKE_COMMAND} -DOUTPUT_FILE=${CMAKE_BINARY_DIR}/mime_types.txt -P ${CMAKE_SOURCE_DIR}/cmake/fetch_mime_types.cmake
        COMMENT "Fetching MIME types..."
        VERBATIM
)

add_custom_target(fetch_mime_types ALL
        DEPENDS ${CMAKE_BINARY_DIR}/mime_types.txt
)

add_executable(wow_unix src/main.cpp
        src/gl/window.h
        src/gl/window.cpp
        src/web/web_core.h
        src/web/web_core.cpp
        src/web/web_application.h
        src/web/web_application.cpp
        src/gl/vertex_buffer.cpp
        src/gl/vertex_buffer.h
        src/gl/index_buffer.h
        src/gl/index_buffer.cpp
        src/gl/program.h
        src/gl/program.cpp
        src/gl/mesh.h
        src/gl/mesh.cpp
        src/gl/texture.h
        src/gl/texture.cpp
        src/gl/stb_loader.cpp
        src/gl/shared_texture.h
        src/gl/shared_texture.cpp
        src/gl/bindable_texture.h
        src/web/web_client.h
        src/web/web_client.cpp
        src/web/schemes/app_scheme_handler.h
        src/web/schemes/app_scheme_handler.cpp
        src/utils/string_utils.h
        src/utils/string_utils.cpp
        src/web/windows_virtual_keys.h
        src/web/web_dialog_handler.h
        src/web/web_dialog_handler.cpp
        src/web/lambda_task.h
        src/web/ipc_message_handler.h
        src/web/ipc_message_handler.cpp
        src/utils/dialog_utils.h
        src/utils/dialog_utils.cpp
        src/utils/di.h
        src/utils/di.cpp
        src/web/event/event_manager.h
        src/web/event/event_manager.cpp
        src/web/event/shell_events.h
        src/web/event/shell_events.cpp
        src/io/mpq_manager.h
        src/io/mpq_manager.cpp
        src/io/dbc/dbc_file.h
        src/io/dbc/dbc_structs.h
        src/io/dbc/dbc_manager.h
        src/io/dbc/dbc_manager.cpp
        src/io/mpq_file.h
        src/io/mpq_file.cpp
        src/web/event/ui_event_system.h
        src/web/event/ui_event_system.cpp
        src/io/blp/blp_file.h
        src/io/blp/blp_file.cpp
        src/utils/io.h
        src/utils/io.cpp
        src/web/schemes/blp_scheme_handler.h
        src/web/schemes/blp_scheme_handler.cpp
        src/web/schemes/minimap_scheme_handler.h
        src/web/schemes/minimap_scheme_handler.cpp
        src/io/minimap/minimap_provider.h
        src/io/minimap/minimap_provider.cpp
        src/scene/world_frame.h
        src/scene/world_frame.cpp
        src/utils/log_utils.h
        src/scene/map_manager.h
        src/scene/map_manager.cpp
        src/utils/constants.h
        src/utils/constants.cpp
        src/config/config_manager.cpp
        src/config/config_manager.h
        src/io/terrain/adt_tile.h
        src/io/terrain/adt_tile.cpp
        src/io/terrain/adt_chunk.h
        src/io/terrain/adt_chunk.cpp
        src/utils/work_pool.h
        src/utils/work_pool.cpp
        src/scene/texture_manager.h
        src/scene/texture_manager.cpp
        src/scene/gpu_dispatcher.h
        src/scene/gpu_dispatcher.cpp
        src/scene/camera.h
        src/scene/camera.cpp
        src/utils/math.h
        src/utils/math.cpp
        src/scene/frustum.h
        src/scene/frustum.cpp
        src/io/terrain/wdt_file.h
        src/io/terrain/wdt_file.cpp
        src/scene/scene_info.h
        src/utils/system_stats.h
        src/utils/system_stats.cpp
        src/scene/sky/sky_sphere.h
        src/scene/sky/sky_sphere.cpp
        src/scene/sky/light_manager.hpp
        src/scene/sky/light_manager.cpp
        src/gl/uniform_buffer.hpp
        src/gl/uniform_buffer.cpp
        src/scene/sky/light_data.hpp
        src/scene/sky/light_data.cpp
        src/utils/common_utils.hpp
        src/audio/audio_manager.hpp
        src/audio/audio_manager.cpp
        src/audio/zone_music_manager.hpp
        src/audio/zone_music_manager.cpp
        src/audio/fmod_utils.hpp
        src/audio/fmod_utils.cpp
        src/web/event/js_event.h)

list(APPEND DEFINITIONS -DGLM_ENABLE_EXPERIMENTAL)
if (UNIX)
    list(APPEND DEFINITIONS -rdynamic -Wno-multichar)
endif ()

target_compile_options(wow_unix PRIVATE ${DEFINITIONS})

if (WIN32)
    target_compile_definitions(wow_unix PRIVATE
            ssize_t=int64_t
            NOMINMAX
    )
endif ()

add_dependencies(wow_unix fetch_mime_types glad_s3tc)

add_executable(wow_unix_browser src/main_browser.cpp)

target_compile_options(wow_unix_browser PRIVATE ${DEFINITIONS})

if (WIN32)
    target_compile_definitions(wow_unix_browser PRIVATE
            ssize_t=int64_t
            NOMINMAX
    )
endif ()

target_include_directories(wow_unix PRIVATE
        src
        ${CMAKE_BINARY_DIR}
        ${OPENGL_INCLUDE_DIRS}
        ${stb_SOURCE_DIR}
        ${GTK3_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${boost_di_SOURCE_DIR}/include
        ${boost_pfr_SOURCE_DIR}/include
        ${tomlplusplus_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/gladsources/glad_s3tc/include
        ${FMOD_INCLUDE_DIRS}
)

target_include_directories(wow_unix_browser PRIVATE src)

target_link_libraries(wow_unix PRIVATE
        glad_s3tc
        glm
        spdlog::spdlog
        glfw
        ${OPENGL_LINKED_LIBRARIES}
        CEF::CEF
        CEF::Wrapper
        ${GTK3_LIBRARIES}
        StormLib::storm
        ${FMOD_LIBRARIES}
        nlohmann_json::nlohmann_json
)
target_link_libraries(wow_unix_browser PRIVATE spdlog::spdlog CEF::CEF CEF::Wrapper)
