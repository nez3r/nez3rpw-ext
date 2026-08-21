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
inline bool s_box_filled = false;
inline bool opt_box_gradient = false;
inline ImVec4 s_box_col = ImVec4(1.f, 1.f, 1.f, 1.f);
inline ImVec4 s_box_col2 = ImVec4(1.f, 0.f, 0.f, 1.f);

inline bool opt_name = false;
inline ImVec4 s_name_col = ImVec4(1.f, 1.f, 1.f, 1.f);

inline bool opt_health = false;
inline float s_health_size = 4.f;
inline int s_health_pos = 0;          // 0 = left, 1 = right, 2 = top
inline bool s_health_text = false;
inline ImVec4 s_health_col1 = ImVec4(0.f, 1.f, 0.f, 1.f);

inline bool opt_dist = false;
inline ImVec4 s_dist_col = ImVec4(1.f, 1.f, 1.f, 1.f);

inline bool opt_skeleton = false;
inline float s_skel_thick = 1.f;
inline ImVec4 s_skel_color = ImVec4(1.f, 1.f, 1.f, 1.f);

inline bool opt_weapon = false;
inline ImVec4 s_weapon_col = ImVec4(1.f, 1.f, 1.f, 1.f);

inline bool opt_line = false;
inline int s_line_pos = 1;                // 0 = middle, 1 = top, 2 = bottom
inline bool s_line_down = false;
inline float s_line_thick = 1.f;
inline ImVec4 s_line_col = ImVec4(1.f, 1.f, 1.f, 1.f);

inline float s_esp_dist = 300.f;
inline int s_esp_style = 0;           // 0 = full, 1 = minimal

// ─── Chams ───
inline bool opt_chams = false;
inline int s_chams_type = 0;          // 0 = solid, 1 = flat
inline ImVec4 s_chams_col = ImVec4(1.f, 0.f, 0.f, 1.f);
inline bool s_chams_rainbow = false;
inline int s_chams_rainbow_speed = 100;
inline bool opt_chams_self = false;

inline bool opt_hands = false;
inline bool s_hands_shader = false;
inline int s_hands_type = 0;
inline ImVec4 s_hands_col = ImVec4(0.f, 1.f, 0.f, 1.f);
inline bool s_hands_rainbow = false;

inline bool opt_weapon_chams = false;
inline bool s_weapon_chams_shader = false;
inline int s_weapon_chams_type = 0;
inline ImVec4 s_weapon_chams_col = ImVec4(0.f, 0.f, 1.f, 1.f);
inline bool s_weapon_chams_rainbow = false;

// ─── World ───
inline bool opt_fog = false;
inline ImVec4 s_fog_col = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
inline float s_fog_start = 0.f;
inline float s_fog_end = 100.f;

inline bool opt_sky = false;
inline ImVec4 s_sky_col = ImVec4(0.5f, 0.5f, 1.f, 1.f);
inline bool opt_sky_rainbow = false;
inline int s_sky_rainbow_ms = 100;

inline bool opt_sky_img = false;

inline bool opt_model = false;
inline float s_model_dist = 0.f;
inline float s_model_height = 0.f;
inline float s_model_rot = 0.f;

inline bool opt_world_color = false;
inline ImVec4 s_world_color = ImVec4(1.f, 1.f, 1.f, 1.f);
inline bool opt_world_color_solid = false;
inline bool opt_world_color_rainbow = false;
inline int s_world_color_rainbow_ms = 100;

// ─── Aim ───
inline bool opt_aim = false;
inline float s_aim_fov = 30.f;
inline int s_aim_bone = 0;            // 0 = head, 1 = neck, 2 = chest, 3 = hips
inline float s_aim_smooth = 5.f;
inline bool opt_aim_visible = false;
inline bool opt_aim_360 = false;
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

inline bool opt_trigger = false;
inline int s_trigger_bone = 0;        // 0 = head, 1 = neck
inline float s_trigger_dist = 100.f;
inline float s_trigger_radius = 10.f;
inline int s_trigger_delay = 0;
inline bool opt_trigger_visible = false;

// ─── Weapon ───
inline bool opt_inf_ammo = false;
inline int s_inf_ammo = 9999;
inline bool opt_rapid_fire = false;
inline float s_rapid_interval = 100.f;
inline bool opt_wallshot = false;
inline bool opt_no_recoil = false;
inline float s_no_recoil_mult = 0.f;

// ─── Misc ───
inline bool opt_set_hp = false;
inline int s_set_hp = 100;
inline bool opt_set_kills = false;
inline int s_set_kills_val = 0;
inline bool opt_set_score = false;
inline int s_set_score = 0;
inline bool opt_tps = false;
inline float s_tps_dist = 5.f;
inline bool opt_air_strafe = false;
inline float s_air_strafe = 5.f;
inline float s_air_strafe_speed = 10.f;

// ─── Antiaim ───
inline bool opt_aa = false;
inline int s_aa_pitch = 0;
inline int s_aa_yaw = 0;
inline float s_aa_speed = 0.f;
inline bool s_aa_jitter = false;
inline int s_aa_jitter_ms = 100;
inline float s_aa_range = 0.f;

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

// ─── Anticheat ───
inline bool opt_bypass = false;
inline bool opt_ac_kill = false;
inline bool opt_ac_audit = false;

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
}