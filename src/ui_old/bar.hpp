#pragma once

namespace ui::bar {
    inline bool g_open = false;
    inline float g_alpha = 0.f;
    inline float g_game_alpha = 1.f;

    void render();
    void set_game_alpha(float a);
    float game_alpha();
}
