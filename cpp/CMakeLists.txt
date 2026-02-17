cmake_minimum_required(VERSION 3.16)
project(GeoSiteChecker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# ========================================
# Статическая сборка для Windows
# ========================================
if(WIN32)
    # Флаги для статической линковки runtime
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++ -Os")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -static -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    
    # Для статической сборки Qt
    add_definitions(-DQT_STATICBUILD)
    
    # Отключаем автоматическое добавление манифеста (конфликты со статическим Qt)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

# Автоопределение статического Qt
if(EXISTS "${CMAKE_PREFIX_PATH}/lib/cmake/Qt6/Qt6Config.cmake")
    message(STATUS "Qt6 path: ${CMAKE_PREFIX_PATH}")
endif()

# Try system Protobuf first; if not found or ABI-incompatible, build from source
option(GEOCHECKER_FETCH_PROTOBUF "Build protobuf from source via FetchContent" OFF)

if(NOT GEOCHECKER_FETCH_PROTOBUF)
    find_package(Protobuf QUIET)
    if(NOT Protobuf_FOUND)
        message(STATUS "System Protobuf not found, will build from source")
        set(GEOCHECKER_FETCH_PROTOBUF ON)
    endif()
endif()

if(GEOCHECKER_FETCH_PROTOBUF)
    include(FetchContent)
    set(protobuf_BUILD_TESTS OFF CACHE INTERNAL "")
    set(protobuf_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(protobuf_INSTALL OFF CACHE INTERNAL "")
    set(protobuf_BUILD_PROTOBUF_BINARIES ON CACHE INTERNAL "")
    set(protobuf_BUILD_PROTOC_BINARIES ON CACHE INTERNAL "")
    set(protobuf_ABSL_PROVIDER "module" CACHE INTERNAL "")
    FetchContent_Declare(
        protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        GIT_TAG        v29.3
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(protobuf)
    # After FetchContent, protoc target and protobuf::libprotobuf are available
    set(PROTOC_EXECUTABLE $<TARGET_FILE:protoc>)
    set(Protobuf_INCLUDE_DIRS ${protobuf_SOURCE_DIR}/src)
else()
    find_program(PROTOC_EXECUTABLE protoc REQUIRED)
endif()

# Qt6 (optional - for GUI)
find_package(Qt6 COMPONENTS Widgets Core Gui Concurrent QUIET)
set(BUILD_GUI ${Qt6_FOUND})

# Generate protobuf C++ files (protoc puts them in proto/ subdir)
set(PROTO_FILE ${CMAKE_SOURCE_DIR}/../proto/routercommon.proto)
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(PROTO_SUBDIR ${PROTO_GEN_DIR}/proto)
set(PROTO_SRCS ${PROTO_SUBDIR}/routercommon.pb.cc)
set(PROTO_HDRS ${PROTO_SUBDIR}/routercommon.pb.h)

add_custom_command(
    OUTPUT ${PROTO_SRCS} ${PROTO_HDRS}
    COMMAND ${PROTOC_EXECUTABLE} --cpp_out=${PROTO_GEN_DIR}
            -I${CMAKE_SOURCE_DIR}/.. ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating protobuf C++ code"
)
add_custom_target(proto_gen DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

# Core library (parser logic)
add_library(geochecker_core STATIC
    src/dat_parser.cpp
    src/geochecker_types.cpp
    ${PROTO_SRCS}
)
add_dependencies(geochecker_core proto_gen)
target_include_directories(geochecker_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROTO_SUBDIR}
)
target_link_libraries(geochecker_core PUBLIC
    protobuf::libprotobuf
)
if(WIN32)
    target_link_libraries(geochecker_core PUBLIC ws2_32)
endif()

# CLI application (always builds)
add_executable(geochecker-cli src/main_cli.cpp)
target_link_libraries(geochecker-cli PRIVATE geochecker_core)

# GUI application (when Qt6 found)
if(BUILD_GUI)
    # Force mainwindow.cpp rebuild when lang_ui_config.hpp changes
    set(LANG_UI_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/include/lang_ui_config.hpp)
    set(LANG_UI_STAMP ${CMAKE_CURRENT_BINARY_DIR}/lang_ui_config.stamp)
    add_custom_command(
        OUTPUT ${LANG_UI_STAMP}
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.cpp
        COMMAND ${CMAKE_COMMAND} -E touch ${LANG_UI_STAMP}
        DEPENDS ${LANG_UI_CONFIG}
        COMMENT "lang_ui_config.hpp changed, forcing mainwindow rebuild"
    )
    add_custom_target(lang_ui_config_stamp ALL DEPENDS ${LANG_UI_STAMP})

    # Ensure object file directory exists (avoids "path not found" when Ninja runs g++ on Windows)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/geochecker.dir/src")
    add_executable(geochecker WIN32
        src/main.cpp
        src/mainwindow.cpp
        include/mainwindow.hpp
        include/lang_ui_config.hpp
    )
    # App icon (Windows: in .exe; Linux: window and launcher)
    set(APP_ICON_ICO "${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon.ico")
    if(EXISTS "${APP_ICON_ICO}")
        qt_add_resources(geochecker ICON_RSC
            PREFIX /icons
            BASE "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            FILES "${APP_ICON_ICO}"
        )
        if(WIN32)
            set(APP_RC "${CMAKE_CURRENT_SOURCE_DIR}/resources/geochecker.rc")
            target_sources(geochecker PRIVATE "${APP_RC}")
        endif()
        target_compile_definitions(geochecker PRIVATE GEOCHECKER_HAS_APP_ICON=1)
    endif()
    set_target_properties(geochecker PROPERTIES AUTOMOC ON)
    target_include_directories(geochecker PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROTO_SUBDIR}
    )
    target_link_libraries(geochecker PRIVATE
        geochecker_core
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Concurrent
        protobuf::libprotobuf
    )
    add_dependencies(geochecker lang_ui_config_stamp)
    message(STATUS "Building with Qt6 GUI: geochecker")
endif()

message(STATUS "CLI build: geochecker-cli")

if(NOT EXISTS ${PROTO_FILE})
    message(FATAL_ERROR "Proto file not found: ${PROTO_FILE}")
endif()