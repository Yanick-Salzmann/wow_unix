cmake_minimum_required(VERSION 3.24) # DO NOT overwrite this line
project(wow_unix)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
find_package(Protobuf REQUIRED)

find_package(absl REQUIRED)

find_package(Boost CONFIG REQUIRED)

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
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v0.1.36
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

FetchContent_MakeAvailable(spdlog glfw glad glm stb boost_di StormLib)

set(PROTO_FILES
        ${CMAKE_SOURCE_DIR}/src/proto/js_event.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

set(TS_PROTO_OUT_DIR "${CMAKE_SOURCE_DIR}/ui/src/app/proto")
add_custom_command(
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/ui
        OUTPUT ${TS_PROTO_OUT_DIR}/js_event.pb.ts
        COMMAND mkdir -p ${TS_PROTO_OUT_DIR}
        COMMAND npx protoc
            --ts_out=${CMAKE_SOURCE_DIR}/ui/src/app/proto
            --proto_path=${CMAKE_SOURCE_DIR}/src/proto
            ${PROTO_FILES}
        DEPENDS ${PROTO_FILES}
        COMMENT "Generating TypeScript protobuf files"
        VERBATIM
)

add_custom_target(proto_typescript ALL
        DEPENDS ${TS_PROTO_OUT_DIR}/js_event.pb.ts
)

add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/mime_types.txt
        COMMAND bash ${CMAKE_SOURCE_DIR}/fetch_mime_types.sh ${CMAKE_BINARY_DIR}/mime_types.txt
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
        src/web/app_scheme_handler.h
        src/web/app_scheme_handler.cpp
        src/utils/string_utils.h
        src/utils/string_utils.cpp
        src/web/windows_virtual_keys.h
        src/web/web_dialog_handler.h
        src/web/web_dialog_handler.cpp
        src/web/lambda_task.h
        src/web/ipc_message_handler.h
        src/web/ipc_message_handler.cpp
        ${PROTO_SRCS}
        ${PROTO_HDRS}
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
        src/io/mpq_file.cpp)

add_dependencies(wow_unix fetch_mime_types proto_typescript)

add_executable(wow_unix_browser src/main_browser.cpp)

target_include_directories(wow_unix PRIVATE
        src
        cef
        ${CMAKE_BINARY_DIR}
        ${OPENGL_INCLUDE_DIRS}
        ${stb_SOURCE_DIR}
        ${GTK3_INCLUDE_DIRS}
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}  # For generated protobuf headers
        ${boost_di_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
)

target_include_directories(wow_unix_browser PRIVATE src cef)

target_link_libraries(wow_unix PRIVATE
        glm
        glad
        spdlog::spdlog
        glfw
        OpenGL::GL
        OpenGL::EGL
        ${CMAKE_SOURCE_DIR}/libcef_dll_wrapper.a
        ${CMAKE_BINARY_DIR}/libcef.so
        ${GTK3_LIBRARIES}
        ${Protobuf_LIBRARIES}
        utf8_range
        absl::base
        absl::strings
        absl::str_format
        absl::log
        absl::log_severity
        absl::log_internal_message
        absl::log_internal_check_op
        absl::log_internal_conditions
        absl::log_internal_format
        absl::log_internal_globals
        absl::log_internal_log_sink_set
        absl::log_internal_proto
        absl::cord
        absl::status
        absl::statusor
        absl::flat_hash_map
        absl::flat_hash_set
        absl::node_hash_map
        absl::node_hash_set
        absl::inlined_vector
        StormLib::storm
)
target_link_libraries(wow_unix_browser PRIVATE spdlog::spdlog ${CMAKE_SOURCE_DIR}/libcef_dll_wrapper.a ${CMAKE_BINARY_DIR}/libcef.so)
