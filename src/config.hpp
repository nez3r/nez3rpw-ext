#pragma once

#include "json.hpp"
#include "menu_vars.hpp"
#include "protect/oxorany.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>

namespace config {

    inline const char* CONFIG_DIR = "/sdcard/nez3rpw";
    inline const char* CONFIG_PREFIX = "nez3rpw_";
    inline const char* CONFIG_EXT = ".json";

    // Create config directory if it doesn't exist
    inline void ensure_dir() {
        struct stat st = {0};
        if (stat(CONFIG_DIR, &st) == -1) {
            mkdir(CONFIG_DIR, 0755);
        }
    }

    // Get list of existing config files (sorted by number)
    inline std::vector<std::string> get_config_files() {
        ensure_dir();
        std::vector<std::string> files;

        DIR* dir = opendir(CONFIG_DIR);
        if (!dir) return files;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name.size() > strlen(CONFIG_PREFIX) + strlen(CONFIG_EXT) &&
                name.rfind(CONFIG_PREFIX, 0) == 0 &&
                name.size() >= 4 && name.substr(name.size() - 4) == CONFIG_EXT) {
                files.push_back(name);
            }
        }
        closedir(dir);

        // Sort by number in filename
        std::sort(files.begin(), files.end(), [](const std::string& a, const std::string& b) {
            auto extract_num = [](const std::string& s) -> int {
                size_t start = strlen(CONFIG_PREFIX);
                size_t end = s.size() - strlen(CONFIG_EXT);
                if (start >= end) return 0;
                try {
                    return std::stoi(s.substr(start, end - start));
                } catch (...) {
                    return 0;
                }
            };
            return extract_num(a) < extract_num(b);
        });

        return files;
    }

    // Get next config number
    inline int get_next_config_number() {
        auto files = get_config_files();
        if (files.empty()) return 1;

        std::string last = files.back();
        size_t start = strlen(CONFIG_PREFIX);
        size_t end = last.size() - strlen(CONFIG_EXT);
        try {
            return std::stoi(last.substr(start, end - start)) + 1;
        } catch (...) {
            return 1;
        }
    }

    // Build full path for config file
    inline std::string get_config_path(int num) {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s/%s%d%s", CONFIG_DIR, CONFIG_PREFIX, num, CONFIG_EXT);
        return std::string(buf);
    }

    // Build full path for config file by name
    inline std::string get_config_path(const std::string& name) {
        return std::string(CONFIG_DIR) + "/" + name;
    }

    // Serialize all menu variables to JSON
    inline nlohmann::json serialize() {
        nlohmann::json j;

        // Visuals / ESP
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

    // Deserialize JSON to menu variables
    inline void deserialize(const nlohmann::json& j) {
        auto get_bool = [](const nlohmann::json& obj, const char* key, bool def = false) -> bool {
            return obj.contains(key) ? obj[key].get<bool>() : def;
        };
        auto get_int = [](const nlohmann::json& obj, const char* key, int def = 0) -> int {
            return obj.contains(key) ? obj[key].get<int>() : def;
        };
        auto get_float = [](const nlohmann::json& obj, const char* key, float def = 0.f) -> float {
            return obj.contains(key) ? obj[key].get<float>() : def;
        };
        auto get_color = [](const nlohmann::json& obj, const char* key, ImVec4 def = ImVec4(1,1,1,1)) -> ImVec4 {
            if (!obj.contains(key) || !obj[key].is_array() || obj[key].size() != 4) return def;
            return ImVec4(obj[key][0].get<float>(), obj[key][1].get<float>(), obj[key][2].get<float>(), obj[key][3].get<float>());
        };

        if (j.contains("esp")) {
            auto& e = j["esp"];
            opt_esp = get_bool(e, "enabled");
            opt_rgb_esp = get_bool(e, "rgb");
            opt_box = get_bool(e, "box");
            s_box_thick = get_float(e, "box_thick", 1.f);
            s_box_shape = get_int(e, "box_shape", 0);
            s_box_col = get_color(e, "box_color");
            opt_name = get_bool(e, "name");
            s_name_col = get_color(e, "name_color");
            opt_health = get_bool(e, "health");
            s_health_col1 = get_color(e, "health_color");
            opt_dist = get_bool(e, "distance");
            s_dist_col = get_color(e, "distance_color");
            opt_skeleton = get_bool(e, "skeleton");
            s_skel_thick = get_float(e, "skeleton_thick", 1.f);
            s_skel_color = get_color(e, "skeleton_color");
            opt_line = get_bool(e, "line");
            s_line_pos = get_int(e, "line_pos", 1);
            s_line_down = get_bool(e, "line_down", false);
            s_line_thick = get_float(e, "line_thick", 1.f);
            s_line_col = get_color(e, "line_color");
            s_esp_dist = get_float(e, "render_dist", 300.f);
        }

        if (j.contains("aim")) {
            auto& a = j["aim"];
            opt_aim = get_bool(a, "enabled");
            s_aim_fov = get_float(a, "fov", 30.f);
            s_aim_bone = get_int(a, "bone", 0);
            s_aim_smooth = get_float(a, "smooth", 5.f);
            opt_aim_visible = get_bool(a, "visible_check");
            opt_aim_fov_draw = get_bool(a, "fov_draw");
            opt_aim_info = get_bool(a, "aim_info");
        }

        if (j.contains("silent")) {
            auto& s = j["silent"];
            opt_silent_aim = get_bool(s, "enabled");
            s_silent_fov = get_float(s, "fov", 30.f);
            s_silent_target_bone = get_int(s, "bone", 0);
            opt_silent_visible = get_bool(s, "visible_check");
            opt_silent_smoke_check = get_bool(s, "smoke_check");
            s_silent_recoil_mult = get_float(s, "recoil_mult", 1.f);
            opt_silent_fov_draw = get_bool(s, "fov_draw");
        }

        if (j.contains("weapon")) {
            auto& w = j["weapon"];
            opt_inf_ammo = get_bool(w, "inf_ammo");
            s_inf_ammo = get_int(w, "inf_ammo_val", 9999);
            opt_rapid_fire = get_bool(w, "rapid_fire");
            s_rapid_interval = get_float(w, "rapid_interval", 100.f);
            opt_wallshot = get_bool(w, "wallshot");
        }

        if (j.contains("misc")) {
            auto& m = j["misc"];
            opt_air_strafe = get_bool(m, "air_strafe");
            s_air_strafe_speed = get_float(m, "air_strafe_speed", 10.f);
        }

        if (j.contains("watermark")) {
            auto& w = j["watermark"];
            opt_wm = get_bool(w, "enabled", true);
            opt_wm_icons = get_bool(w, "icons", true);
            opt_wm_show_site = get_bool(w, "show_site", true);
            opt_wm_show_type = get_bool(w, "show_type", true);
            opt_wm_show_fps = get_bool(w, "show_fps", true);
            opt_wm_show_time = get_bool(w, "show_time", true);
            opt_wm_show_version = get_bool(w, "show_version", true);
            s_wm_side = get_int(w, "side", 0);
            s_wm_col = get_color(w, "color");
            s_wm_scale = get_float(w, "scale", 1.f);
        }

        if (j.contains("settings")) {
            auto& s = j["settings"];
            s_lang = get_int(s, "lang", 1);
            s_scrollbar_size = get_int(s, "scrollbar_size", 24);
            cfg_style_accent = get_color(s, "accent_color", ImVec4(0.64f, 0.56f, 0.88f, 1.f));
        }

        // Apply language change
        apply_lang(s_lang);
        c_utils->accent = cfg_style_accent;
    }

    // Save config to file (by number)
    inline bool save(int num) {
        ensure_dir();
        std::string path = get_config_path(num);
        nlohmann::json j = serialize();
        std::ofstream f(path);
        if (!f.is_open()) return false;
        f << j.dump(4);
        f.close();
        return true;
    }

    // Save config to file (by name)
    inline bool save(const std::string& name) {
        ensure_dir();
        std::string path = get_config_path(name);
        nlohmann::json j = serialize();
        std::ofstream f(path);
        if (!f.is_open()) return false;
        f << j.dump(4);
        f.close();
        return true;
    }

    // Load config from file (by number)
    inline bool load(int num) {
        std::string path = get_config_path(num);
        std::ifstream f(path);
        if (!f.is_open()) return false;
        nlohmann::json j;
        try {
            f >> j;
            deserialize(j);
            f.close();
            return true;
        } catch (...) {
            f.close();
            return false;
        }
    }

    // Load config from file (by name)
    inline bool load(const std::string& name) {
        std::string path = get_config_path(name);
        std::ifstream f(path);
        if (!f.is_open()) return false;
        nlohmann::json j;
        try {
            f >> j;
            deserialize(j);
            f.close();
            return true;
        } catch (...) {
            f.close();
            return false;
        }
    }

    // Delete config file
    inline bool delete_config(int num) {
        std::string path = get_config_path(num);
        return std::remove(path.c_str()) == 0;
    }

    inline bool delete_config(const std::string& name) {
        std::string path = get_config_path(name);
        return std::remove(path.c_str()) == 0;
    }

} // namespace config