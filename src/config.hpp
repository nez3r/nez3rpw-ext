#pragma once

#include "json.hpp"
#include "menu_vars.hpp"
#include "protect/oxorany.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>

namespace config {

    inline const char* CONFIG_DIR = "/data/local/tmp/nez3rpw/";
    inline const char* CONFIG_PREFIX = "nez3rpw_";
    inline const char* CONFIG_EXT = ".json";

    inline std::string path_of(const char* name) {
        return std::string(CONFIG_DIR) + name + CONFIG_EXT;
    }

    inline std::string slot_name(int n) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%s%d", CONFIG_PREFIX, n);
        return std::string(buf);
    }

    inline std::string slot_path(int n) {
        return path_of(slot_name(n).c_str());
    }

    // Create config directory if it doesn't exist
    inline void ensure_dir() {
        struct stat st = {0};
        if (stat(CONFIG_DIR, &st) == -1) {
            mkdir(CONFIG_DIR, 0755);
        }
    }

    // Read and parse JSON without exceptions
    inline bool read_config(const char* name, nlohmann::json& j) {
        std::ifstream f(path_of(name));
        if (!f.is_open()) return false;
        std::stringstream ss;
        ss << f.rdbuf();
        f.close();
        std::string s = ss.str();
        if (s.empty()) return false;
        j = nlohmann::json::parse(s, nullptr, false);
        if (j.is_discarded() || !j.is_object()) return false;
        return true;
    }

    // Save JSON to file
    inline bool write_config(const char* name, const nlohmann::json& j) {
        ensure_dir();
        std::ofstream f(path_of(name));
        if (!f.is_open()) return false;
        f << j.dump(4);
        f.close();
        return true;
    }

    // Get list of existing slot numbers (sorted)
    inline std::vector<int> list_slots() {
        ensure_dir();
        std::vector<int> out;
        DIR* d = opendir(CONFIG_DIR);
        if (!d) return out;

        struct dirent* e;
        while ((e = readdir(d))) {
            if (!e) continue;
            std::string n = e->d_name;
            if (n.compare(0, strlen(CONFIG_PREFIX), CONFIG_PREFIX) != 0) continue;
            if (n.size() <= strlen(CONFIG_PREFIX) + strlen(CONFIG_EXT)) continue;
            if (n.substr(n.size() - strlen(CONFIG_EXT)) != CONFIG_EXT) continue;

            std::string num_str = n.substr(strlen(CONFIG_PREFIX), n.size() - strlen(CONFIG_PREFIX) - strlen(CONFIG_EXT));
            if (num_str.empty()) continue;

            bool ok = true;
            for (char c : num_str) if (c < '0' || c > '9') { ok = false; break; }
            if (!ok) continue;

            int v = 0;
            for (char c : num_str) v = v * 10 + (c - '0');
            if (v >= 1 && v <= 1000) out.push_back(v);
        }
        closedir(d);
        std::sort(out.begin(), out.end());
        return out;
    }

    // Get next free slot number
    inline int next_free_slot() {
        auto slots = list_slots();
        if (slots.empty()) return 1;
        return slots.back() + 1;
    }

    // Check if slot exists
    inline bool slot_exists(int n) {
        if (n < 1 || n > 1000) return false;
        std::ifstream f(slot_path(n));
        return f.is_open();
    }

    // Serialize all menu variables to JSON
    inline nlohmann::json serialize() {
        nlohmann::json j;

        // ESP
        j["esp"]["enabled"] = opt_esp;
        j["esp"]["rgb"] = opt_rgb_esp;
        j["esp"]["box"] = opt_box;
        j["esp"]["box_thick"] = s_box_thick;
        j["esp"]["box_shape"] = s_box_shape;
        j["esp"]["box_color"] = {s_box_col.x, s_box_col.y, s_box_col.z, s_box_col.w};
        j["esp"]["name"] = opt_name;
        j["esp"]["name_color"] = {s_name_col.x, s_name_col.y, s_name_col.z, s_name_col.w};
        j["esp"]["health"] = opt_health;
        j["esp"]["health_color"] = {s_health_col1.x, s_health_col1.y, s_health_col1.z, s_health_col1.w};
        j["esp"]["distance"] = opt_dist;
        j["esp"]["distance_color"] = {s_dist_col.x, s_dist_col.y, s_dist_col.z, s_dist_col.w};
        j["esp"]["skeleton"] = opt_skeleton;
        j["esp"]["skeleton_thick"] = s_skel_thick;
        j["esp"]["skeleton_color"] = {s_skel_color.x, s_skel_color.y, s_skel_color.z, s_skel_color.w};
        j["esp"]["line"] = opt_line;
        j["esp"]["line_pos"] = s_line_pos;
        j["esp"]["line_down"] = s_line_down;
        j["esp"]["line_thick"] = s_line_thick;
        j["esp"]["line_color"] = {s_line_col.x, s_line_col.y, s_line_col.z, s_line_col.w};
        j["esp"]["render_dist"] = s_esp_dist;

        // Aim
        j["aim"]["enabled"] = opt_aim;
        j["aim"]["fov"] = s_aim_fov;
        j["aim"]["bone"] = s_aim_bone;
        j["aim"]["smooth"] = s_aim_smooth;
        j["aim"]["visible_check"] = opt_aim_visible;
        j["aim"]["fov_draw"] = opt_aim_fov_draw;
        j["aim"]["aim_info"] = opt_aim_info;

        // Silent Aim
        j["silent"]["enabled"] = opt_silent_aim;
        j["silent"]["fov"] = s_silent_fov;
        j["silent"]["bone"] = s_silent_target_bone;
        j["silent"]["visible_check"] = opt_silent_visible;
        j["silent"]["smoke_check"] = opt_silent_smoke_check;
        j["silent"]["recoil_mult"] = s_silent_recoil_mult;
        j["silent"]["fov_draw"] = opt_silent_fov_draw;

        // Weapon
        j["weapon"]["inf_ammo"] = opt_inf_ammo;
        j["weapon"]["inf_ammo_val"] = s_inf_ammo;
        j["weapon"]["rapid_fire"] = opt_rapid_fire;
        j["weapon"]["rapid_interval"] = s_rapid_interval;
        j["weapon"]["wallshot"] = opt_wallshot;

        // Misc
        j["misc"]["air_strafe"] = opt_air_strafe;
        j["misc"]["air_strafe_speed"] = s_air_strafe_speed;

        // Watermark
        j["watermark"]["enabled"] = opt_wm;
        j["watermark"]["icons"] = opt_wm_icons;
        j["watermark"]["show_site"] = opt_wm_show_site;
        j["watermark"]["show_type"] = opt_wm_show_type;
        j["watermark"]["show_fps"] = opt_wm_show_fps;
        j["watermark"]["show_time"] = opt_wm_show_time;
        j["watermark"]["show_version"] = opt_wm_show_version;
        j["watermark"]["side"] = s_wm_side;
        j["watermark"]["color"] = {s_wm_col.x, s_wm_col.y, s_wm_col.z, s_wm_col.w};
        j["watermark"]["scale"] = s_wm_scale;

        // Settings
        j["settings"]["lang"] = s_lang;
        j["settings"]["scrollbar_size"] = s_scrollbar_size;
        j["settings"]["accent_color"] = {cfg_style_accent.x, cfg_style_accent.y, cfg_style_accent.z, cfg_style_accent.w};

        return j;
    }

    // Helper to get color from JSON
    inline ImVec4 get_color(const nlohmann::json& obj, const char* key, ImVec4 def = ImVec4(1,1,1,1)) {
        if (!obj.contains(key) || !obj[key].is_array() || obj[key].size() != 4) return def;
        return ImVec4(obj[key][0].get<float>(), obj[key][1].get<float>(), obj[key][2].get<float>(), obj[key][3].get<float>());
    }

    // Deserialize JSON to menu variables
    inline void deserialize(const nlohmann::json& j) {
        auto get_bool = [&](const nlohmann::json& obj, const char* key, bool def) -> bool {
            return obj.contains(key) ? obj[key].get<bool>() : def;
        };
        auto get_int = [&](const nlohmann::json& obj, const char* key, int def) -> int {
            return obj.contains(key) ? obj[key].get<int>() : def;
        };
        auto get_float = [&](const nlohmann::json& obj, const char* key, float def) -> float {
            return obj.contains(key) ? obj[key].get<float>() : def;
        };

        if (j.contains("esp")) {
            auto& e = j["esp"];
            opt_esp = get_bool(e, "enabled", opt_esp);
            opt_rgb_esp = get_bool(e, "rgb", opt_rgb_esp);
            opt_box = get_bool(e, "box", opt_box);
            s_box_thick = get_float(e, "box_thick", s_box_thick);
            s_box_shape = get_int(e, "box_shape", s_box_shape);
            s_box_col = get_color(e, "box_color", s_box_col);
            opt_name = get_bool(e, "name", opt_name);
            s_name_col = get_color(e, "name_color", s_name_col);
            opt_health = get_bool(e, "health", opt_health);
            s_health_col1 = get_color(e, "health_color", s_health_col1);
            opt_dist = get_bool(e, "distance", opt_dist);
            s_dist_col = get_color(e, "distance_color", s_dist_col);
            opt_skeleton = get_bool(e, "skeleton", opt_skeleton);
            s_skel_thick = get_float(e, "skeleton_thick", s_skel_thick);
            s_skel_color = get_color(e, "skeleton_color", s_skel_color);
            opt_line = get_bool(e, "line", opt_line);
            s_line_pos = get_int(e, "line_pos", s_line_pos);
            s_line_down = get_bool(e, "line_down", s_line_down);
            s_line_thick = get_float(e, "line_thick", s_line_thick);
            s_line_col = get_color(e, "line_color", s_line_col);
            s_esp_dist = get_float(e, "render_dist", s_esp_dist);
        }

        if (j.contains("aim")) {
            auto& a = j["aim"];
            opt_aim = get_bool(a, "enabled", opt_aim);
            s_aim_fov = get_float(a, "fov", s_aim_fov);
            s_aim_bone = get_int(a, "bone", s_aim_bone);
            s_aim_smooth = get_float(a, "smooth", s_aim_smooth);
            opt_aim_visible = get_bool(a, "visible_check", opt_aim_visible);
            opt_aim_fov_draw = get_bool(a, "fov_draw", opt_aim_fov_draw);
            opt_aim_info = get_bool(a, "aim_info", opt_aim_info);
        }

        if (j.contains("silent")) {
            auto& s = j["silent"];
            opt_silent_aim = get_bool(s, "enabled", opt_silent_aim);
            s_silent_fov = get_float(s, "fov", s_silent_fov);
            s_silent_target_bone = get_int(s, "bone", s_silent_target_bone);
            opt_silent_visible = get_bool(s, "visible_check", opt_silent_visible);
            opt_silent_smoke_check = get_bool(s, "smoke_check", opt_silent_smoke_check);
            s_silent_recoil_mult = get_float(s, "recoil_mult", s_silent_recoil_mult);
            opt_silent_fov_draw = get_bool(s, "fov_draw", opt_silent_fov_draw);
        }

        if (j.contains("weapon")) {
            auto& w = j["weapon"];
            opt_inf_ammo = get_bool(w, "inf_ammo", opt_inf_ammo);
            s_inf_ammo = get_int(w, "inf_ammo_val", s_inf_ammo);
            opt_rapid_fire = get_bool(w, "rapid_fire", opt_rapid_fire);
            s_rapid_interval = get_float(w, "rapid_interval", s_rapid_interval);
            opt_wallshot = get_bool(w, "wallshot", opt_wallshot);
        }

        if (j.contains("misc")) {
            auto& m = j["misc"];
            opt_air_strafe = get_bool(m, "air_strafe", opt_air_strafe);
            s_air_strafe_speed = get_float(m, "air_strafe_speed", s_air_strafe_speed);
        }

        if (j.contains("watermark")) {
            auto& w = j["watermark"];
            opt_wm = get_bool(w, "enabled", opt_wm);
            opt_wm_icons = get_bool(w, "icons", opt_wm_icons);
            opt_wm_show_site = get_bool(w, "show_site", opt_wm_show_site);
            opt_wm_show_type = get_bool(w, "show_type", opt_wm_show_type);
            opt_wm_show_fps = get_bool(w, "show_fps", opt_wm_show_fps);
            opt_wm_show_time = get_bool(w, "show_time", opt_wm_show_time);
            opt_wm_show_version = get_bool(w, "show_version", opt_wm_show_version);
            s_wm_side = get_int(w, "side", s_wm_side);
            s_wm_col = get_color(w, "color", s_wm_col);
            s_wm_scale = get_float(w, "scale", s_wm_scale);
        }

        if (j.contains("settings")) {
            auto& s = j["settings"];
            s_lang = get_int(s, "lang", s_lang);
            s_scrollbar_size = get_int(s, "scrollbar_size", s_scrollbar_size);
            cfg_style_accent = get_color(s, "accent_color", cfg_style_accent);
        }

        apply_lang(s_lang);
        c_utils->accent = cfg_style_accent;
    }

    // Save to slot number
    inline bool save_slot(int n) {
        if (n < 1 || n > 1000) return false;
        nlohmann::json j = serialize();
        return write_config(slot_name(n).c_str(), j);
    }

    // Load from slot number
    inline bool load_slot(int n) {
        if (n < 1 || n > 1000) return false;
        nlohmann::json j;
        if (!read_config(slot_name(n).c_str(), j)) return false;
        deserialize(j);
        return true;
    }

    // Save by filename (for "New" button)
    inline bool save(int n) {
        return save_slot(n);
    }

    // Load by filename
    inline bool load(const std::string& name) {
        nlohmann::json j;
        if (!read_config(name.c_str(), j)) return false;
        deserialize(j);
        return true;
    }

    // Delete slot
    inline bool delete_slot(int n) {
        if (n < 1 || n > 1000) return false;
        return std::remove(slot_path(n).c_str()) == 0;
    }

    inline bool delete_config(const std::string& name) {
        return std::remove(path_of(name.c_str()).c_str()) == 0;
    }

} // namespace config