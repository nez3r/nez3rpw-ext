#pragma once

// Unified menu variables bridging new UI (ui/) with legacy code (visuals.cpp, aim.cpp)
// All variables are inline to avoid ODR violations across translation units.

#include "imgui.h"

// ─── Visuals / ESP ───
inline bool opt_esp = false;
inline bool opt_rgb_esp = false;
inline bool opt_box = false;
inline float s_box_thick = 1.f;
inline int s_box_shape = 0;           // 0 = square, 1 = corners
inline ImVec4 s_box_col = ImVec4(1.f, 1.f, 1.f, 1.f);

inline bool opt_name = false;
inline ImVec4 s_name_col = ImVec4(1.f, 1.f, 1.f, 1.f);

inline bool opt_health = false;
inline ImVec4 s_health_col1 = ImVec4(0.f, 1.f, 0.f, 1.f);

inline bool opt_dist = false;
inline ImVec4 s_dist_col = ImVec4(1.f, 1.f, 1.f, 1.f);

inline bool opt_skeleton = false;
inline float s_skel_thick = 1.f;
inline ImVec4 s_skel_color = ImVec4(1.f, 1.f, 1.f, 1.f);

inline bool opt_line = false;
inline int s_line_pos = 1;                // 0 = middle, 1 = top, 2 = bottom
inline bool s_line_down = false;
inline float s_line_thick = 1.f;
inline ImVec4 s_line_col = ImVec4(1.f, 1.f, 1.f, 1.f);

inline float s_esp_dist = 300.f;

// ─── Aim ───
inline bool opt_aim = false;
inline float s_aim_fov = 30.f;
inline int s_aim_bone = 0;            // 0 = head, 1 = neck, 2 = chest, 3 = hips
inline float s_aim_smooth = 5.f;
inline bool opt_aim_visible = false;
inline bool opt_aim_fov_draw = false;
inline bool opt_aim_info = false;

// ─── Silent Aim ───
inline bool opt_silent_aim = false;
inline float s_silent_fov = 30.f;
inline bool opt_silent_visible = false;
inline bool opt_silent_smoke_check = false;
inline int s_silent_target_bone = 0;    // 0 = head, 1 = neck, 2 = chest, 3 = hips
inline float s_silent_recoil_mult = 1.f;
inline bool opt_silent_fov_draw = false;

// ─── Weapon ───
inline bool opt_inf_ammo = false;
inline int s_inf_ammo = 9999;
inline bool opt_rapid_fire = false;
inline float s_rapid_interval = 100.f;
inline bool opt_wallshot = false;

// ─── Misc ───
inline bool opt_air_strafe = false;
inline float s_air_strafe_speed = 10.f;

// ─── Watermark ───
inline bool opt_wm = true;
inline bool opt_wm_icons = true;
inline bool opt_wm_show_site = true;
inline bool opt_wm_show_type = true;
inline bool opt_wm_show_fps = true;
inline bool opt_wm_show_time = true;
inline bool opt_wm_show_version = true;
inline int s_wm_side = 0;             // 0 = left, 1 = right
inline ImVec4 s_wm_col = ImVec4(1.f, 1.f, 1.f, 1.f);
inline float s_wm_scale = 1.f;

// ─── Settings ───
inline int s_lang = 1;                // 0 = ru, 1 = en
inline int s_scrollbar_size = 24;

inline ImVec4 cfg_style_accent = ImVec4(0.64f, 0.56f, 0.88f, 1.f); // accent color

// ─── Compatibility layer: map new variables to old cfg namespace ───
namespace cfg {
    namespace esp {
        inline bool& box = ::opt_box;
        inline bool& name = ::opt_name;
        inline bool& health = ::opt_health;
        inline bool& distance = ::opt_dist;
        inline int& box_type = ::s_box_shape;
        inline float& box_rounding = ::s_box_thick;
        inline ImVec4& box_col = ::s_box_col;
        inline ImVec4& name_col = ::s_name_col;
        inline ImVec4& health_col = ::s_health_col1;
        inline ImVec4& distance_col = ::s_dist_col;
    }
    namespace aim {
        inline bool& enabled = ::opt_aim;
        inline float& fov = ::s_aim_fov;
        inline float& smooth = ::s_aim_smooth;
        inline float& max_distance = ::s_esp_dist;
        inline int& target = ::s_aim_bone;
        inline bool& visible_check = ::opt_aim_visible;
        inline bool& lock_line = ::opt_aim_fov_draw;
        inline bool& lock_dot = ::opt_aim_info;
    }
    namespace silent {
        inline bool& enabled = ::opt_silent_aim;
        inline float& fov = ::s_silent_fov;
        inline bool& visible_check = ::opt_silent_visible;
        inline bool& smoke_check = ::opt_silent_smoke_check;
        inline int& target_bone = ::s_silent_target_bone;
        inline float& recoil_mult = ::s_silent_recoil_mult;
        inline bool& fov_display = ::opt_silent_fov_draw;
    }
    namespace other {
        inline bool& crosshair = ::opt_line;
        inline float& crosshair_size = ::s_line_thick;
        inline ImVec4& crosshair_col = ::s_line_col;
        inline float& gui_scale = ::s_wm_scale;
    }
    namespace style {
        inline ImVec4& accent = ::cfg_style_accent;
    }
}