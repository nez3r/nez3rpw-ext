#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "ui.hpp"
#include "vars.h"

// Совместимая обёртка: старое меню звалось ui::menu::render()/ui::draw::init_fonts().
// Тоггл — тап по белой пилюле внизу экрана (как в старом bar), дальше рендерим новое UI.
namespace ui::draw {
    inline void init_fonts() { c_ui->initialize(); c_ui->initialize_fonts(); }
}

// ui::bar compatibility namespace (used by main.cpp for game alpha blending)
namespace ui::bar {
    inline bool g_open = false;
    inline float g_alpha = 0.f;
    inline float g_game_alpha = 1.f;

    inline void set_game_alpha(float a) { g_game_alpha = a; }
    inline float game_alpha() { return g_game_alpha; }
    inline void render() {}  // no-op, actual bar rendering is inside ui::menu::render()
}

namespace ui::menu {

inline bool g_open = false;

// Само меню (новое UI). Бар открытия рисуется отдельно в main.cpp.
inline void render() {
    c_ui->render(&cvars::vars());
}

inline void render_menu() {
    c_ui->render(&cvars::vars());
}

}
