cmake_minimum_required(VERSION 3.22)

if(NOT DEFINED ANDROID_STL)
    set(ANDROID_STL c++_static CACHE STRING "NDK STL variant")
endif()

project(nez3r CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Architecture-specific definitions for syscall wrappers
if(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
    add_compile_definitions(__x86_64__)
elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    add_compile_definitions(__aarch64__)
endif()

set(COMMON_CPP_FLAGS
    -Wall
    -Wextra
    -fno-rtti
    -fno-exceptions
    -fno-builtin
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -fomit-frame-pointer
    -ffunction-sections
    -fdata-sections
    -fmerge-all-constants
    -fno-ident
    -Oz
    -g
)

set(COMMON_LINK_FLAGS
    -Wl,--gc-sections
    -Wl,--build-id=none
    -Wl,--hash-style=gnu
    -Wl,--no-undefined
    -Wl,-z,relro
    -Wl,-z,now
    -Wl,-z,noexecstack
)

# ---------------- payload ----------------
add_executable(nez3r
    src/main.cpp
    src/ui/ui.cpp
    src/ui/widgets.cpp
    src/ui/util.cpp
    src/func/visuals.cpp
    src/func/aim.cpp
    src/func/silent_aim.cpp
    src/func/infammo.cpp
    src/func/weapon_exploits.cpp
    src/func/airstrafe.cpp
    src/protect/oxorany.cpp
    includes/draw/ImGui/imgui.cpp
    includes/draw/ImGui/imgui_draw.cpp
    includes/draw/ImGui/imgui_tables.cpp
    includes/draw/ImGui/imgui_widgets.cpp
    includes/draw/ImGui/backends/imgui_impl_android.cpp
    includes/draw/ImGui/backends/imgui_impl_opengl3.cpp
    includes/draw/Android_draw/draw.cpp
    includes/draw/Android_touch/Touch.cpp
)

target_include_directories(nez3r PRIVATE
    includes/draw/ImGui
    includes/draw/ImGui/backends
    includes/draw/Android_draw
    includes/draw/Android_touch
    includes
    includes/fonts
    includes/internal
    includes/internal/ImGui
    includes/internal/ImGui/backends
    src
    src/ui
    src/func
    src/game
    src/other
    src/protect
    src/ui_reference
)

set_target_properties(nez3r PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    OUTPUT_NAME "nez3r"
)

target_compile_options(nez3r PRIVATE ${COMMON_CPP_FLAGS})

target_link_options(nez3r PRIVATE ${COMMON_LINK_FLAGS})

target_link_libraries(nez3r PRIVATE
    log
    android
    EGL
    GLESv3
    m
)