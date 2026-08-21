#ifndef NATIVESURFACE_DRAW_H
#define NATIVESURFACE_DRAW_H

#include <iostream>
#include <thread>
#include <chrono>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES3/gl3platform.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl32.h>

#include "native_surface/ANativeWindowCreator.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_android.h"
#include "Android_touch/Touch.hpp"

using namespace std;
using namespace std::chrono_literals;

extern android::ANativeWindowCreator::DisplayInfo displayInfo;
extern bool g_Initialized;
extern ImGuiWindow *g_window;
// Old fonts (kept for compatibility with legacy code)
extern ImFont* fontBold;
extern ImFont* fontMedium;
extern ImFont* fontDesc;
extern ImFont* espFont;
extern int native_window_screen_x, native_window_screen_y;

bool init_egl(uint32_t _screen_x, uint32_t _screen_y, bool log = false);
bool initGUI_draw(uint32_t _screen_x, uint32_t _screen_y, bool log = false);
bool ImGui_init();
void screen_config();
void drawBegin();
void drawEnd();
void shutdown();

#endif
