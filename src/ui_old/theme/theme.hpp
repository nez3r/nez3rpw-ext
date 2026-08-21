#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <unordered_map>

inline float g_sw, g_sh;

namespace ui {

namespace clr {
    inline ImVec4 bg           = ImVec4(18/255.f, 18/255.f, 18/255.f, 1.f);
    inline ImVec4 bg_two       = ImVec4(24/255.f, 23/255.f, 24/255.f, 1.f);
    inline ImVec4 sidebar      = ImVec4(14/255.f, 14/255.f, 14/255.f, 1.f);
    inline ImVec4 panel        = ImVec4(26/255.f, 26/255.f, 26/255.f, 1.f);
    inline ImVec4 widget       = ImVec4(34/255.f, 33/255.f, 34/255.f, 1.f);
    inline ImVec4 accent       = ImVec4(162/255.f, 144/255.f, 225/255.f, 1.f);
    inline ImVec4 accent_light = ImVec4(180/255.f, 165/255.f, 240/255.f, 1.f);
    inline ImVec4 accent_dark  = ImVec4(132/255.f, 114/255.f, 195/255.f, 1.f);
    inline ImVec4 text         = ImVec4(255/255.f, 255/255.f, 255/255.f, 1.f);
    inline ImVec4 text_light   = ImVec4(255/255.f, 255/255.f, 255/255.f, 1.f);
    inline ImVec4 text_dim     = ImVec4(90/255.f, 90/255.f, 90/255.f, 1.f);
    inline ImVec4 border       = ImVec4(255/255.f, 255/255.f, 255/255.f, 0.08f);
    inline ImVec4 border_light = ImVec4(255/255.f, 255/255.f, 255/255.f, 0.12f);
    inline ImVec4 border_dark  = ImVec4(0/255.f, 0/255.f, 0/255.f, 1.f);
    inline ImVec4 subtab_bg    = ImVec4(20/255.f, 20/255.f, 20/255.f, 1.f);
}

namespace style {
    inline std::unordered_map<std::string, float> anims;
    inline std::unordered_map<std::string, ImVec4> anim_colors;
    inline float content_w = 0.f;
    inline float content_alpha = 1.f;
    inline bool popup_open = false;
    inline std::string active_popup = "";
    inline constexpr float S = 2.5f;

    void tick();
    float anim(const std::string& id, float tgt, float spd = 12.f);
    ImVec4 anim_col(const std::string& id, const ImVec4& tgt, float spd = 12.f);
    ImU32 col(const ImVec4& c, float a = 1.f);
    bool popup();
    void close();
    void popups();
}

}


