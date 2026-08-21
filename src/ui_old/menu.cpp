#define IMGUI_DEFINE_MATH_OPERATORS
#include "menu.hpp"
#include "bar.hpp"
#include "cfg.hpp"
#include "theme/theme.hpp"
#include "widgets/widgets.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "Android_draw/draw.h"
#include "../protect/oxorany.hpp"
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

namespace ui::menu {
    using namespace style;
    using namespace widgets;

    bool opt_skeleton = true;
    bool opt_rgb_esp = false;

    static float S = 1.f;
    static float content_w = 0.f;
    static float content_alpha = 1.f;

    static float ma = 0.f;
    static int tab = 0;
    static bool tsw = false;
    static int ttab = 0;

    // Анимации вкладок
    static float tab_alpha[] = {1.f, 0.f, 0.f, 0.f};
    static float tab_slide[] = {0.f, 20.f, 20.f, 20.f};

    static ImVec2 menu_pos = ImVec2(0, 0);
    static bool menu_pos_init = false;
    static bool dragging_menu = false;
    static ImVec2 drag_start_pos = ImVec2(0, 0);
    static ImVec2 drag_start_menu = ImVec2(0, 0);

    // Размеры меню
    static ImVec2 menu_size = ImVec2(950.f, 580.f);
    static float sw = 170.f;
    static float hh = 45.f;
    static float sth = 44.f;

    static float title_time = 0.f;
    static char config_name[64] = "default";
    static std::vector<std::string> config_list;
    static bool configs_loaded = false;
    static std::string config_path = "/data/local/tmp/unnamed_configs/";

    static const char* tabs[] = {"VISUALS", "AIMBOT", "MISC", "CONFIG"};
    static constexpr int tc = 4;

    static float lrp(float a, float b, float t) { return a + (b - a) * t; }

    // Цветовая схема
    static ImColor accent_col(float alpha = 1.f) {
        return ImColor(0.35f, 0.75f, 1.0f, alpha);
    }

    static ImColor dark_col(float alpha = 1.f) {
        return ImColor(0.08f, 0.09f, 0.11f, alpha);
    }

    static ImColor panel_col(float alpha = 1.f) {
        return ImColor(0.11f, 0.12f, 0.15f, alpha);
    }

    static ImColor border_col(float alpha = 1.f) {
        return ImColor(0.20f, 0.22f, 0.26f, alpha);
    }

    static void create_config_directory() { mkdir(config_path.c_str(), 0777); }

    static void scan_configs() {
        config_list.clear();
        DIR* dir = opendir(config_path.c_str());
        if (dir) {
            struct dirent* e;
            while ((e = readdir(dir))) {
                std::string n = e->d_name;
                if (n.size() > 4 && n.substr(n.size()-4) == ".cfg") config_list.push_back(n.substr(0, n.size()-4));
            }
            closedir(dir);
        }
    }

    static void save_config(const std::string& name) {
        create_config_directory();
        std::ofstream f(config_path + name + ".cfg");
        if (f.is_open()) {
            f << "esp_box " << cfg::esp::box << "\n";
            f << "esp_name " << cfg::esp::name << "\n";
            f << "esp_health " << cfg::esp::health << "\n";
            f << "esp_distance " << cfg::esp::distance << "\n";
            f << "esp_box_type " << cfg::esp::box_type << "\n";
            f << "esp_box_rounding " << cfg::esp::box_rounding << "\n";
            f << "esp_skeleton " << opt_skeleton << "\n";
            f << "esp_rgb " << opt_rgb_esp << "\n";
            f << "aim_enabled " << cfg::aim::enabled << "\n";
            f << "aim_fov " << cfg::aim::fov << "\n";
            f << "aim_smooth " << cfg::aim::smooth << "\n";
            f << "aim_max_distance " << cfg::aim::max_distance << "\n";
            f << "aim_target " << cfg::aim::target << "\n";
            f << "aim_visible_check " << cfg::aim::visible_check << "\n";
            f << "aim_lock_line " << cfg::aim::lock_line << "\n";
            f << "aim_lock_dot " << cfg::aim::lock_dot << "\n";
            f << "aim_aspect_ratio " << cfg::aim::aspect_ratio << "\n";
            f << "other_crosshair " << cfg::other::crosshair << "\n";
            f << "other_crosshair_type " << cfg::other::crosshair_type << "\n";
            f << "other_crosshair_size " << cfg::other::crosshair_size << "\n";
            f << "other_gui_scale " << cfg::other::gui_scale << "\n";
            f.close();
            if (std::find(config_list.begin(), config_list.end(), name) == config_list.end()) config_list.push_back(name);
        }
    }

    static void load_config(const std::string& name) {
        std::ifstream f(config_path + name + ".cfg");
        if (f.is_open()) {
            std::string line;
            while (std::getline(f, line)) {
                std::istringstream iss(line);
                std::string key; iss >> key;
                if (key == "esp_box") iss >> cfg::esp::box;
                else if (key == "esp_name") iss >> cfg::esp::name;
                else if (key == "esp_health") iss >> cfg::esp::health;
                else if (key == "esp_distance") iss >> cfg::esp::distance;
                else if (key == "esp_box_type") iss >> cfg::esp::box_type;
                else if (key == "esp_box_rounding") iss >> cfg::esp::box_rounding;
                else if (key == "esp_skeleton") iss >> opt_skeleton;
                else if (key == "esp_rgb") iss >> opt_rgb_esp;
                else if (key == "aim_enabled") iss >> cfg::aim::enabled;
                else if (key == "aim_fov") iss >> cfg::aim::fov;
                else if (key == "aim_smooth") iss >> cfg::aim::smooth;
                else if (key == "aim_max_distance") iss >> cfg::aim::max_distance;
                else if (key == "aim_target") iss >> cfg::aim::target;
            }
            f.close();
        }
    }

    static void delete_config(const std::string& name) {
        remove((config_path + name + ".cfg").c_str());
        auto it = std::find(config_list.begin(), config_list.end(), name);
        if (it != config_list.end()) config_list.erase(it);
    }

    static void tick() {
        float dt = ImGui::GetIO().DeltaTime;
        title_time += dt * 1.2f;

        for (int i = 0; i < tc; i++) {
            if (tab == i) {
                tab_alpha[i] = lrp(tab_alpha[i], 1.f, ImClamp(12.f * dt, 0.f, 1.f));
                tab_slide[i] = lrp(tab_slide[i], 0.f, ImClamp(12.f * dt, 0.f, 1.f));
            } else {
                tab_alpha[i] = lrp(tab_alpha[i], 0.f, ImClamp(15.f * dt, 0.f, 1.f));
                tab_slide[i] = lrp(tab_slide[i], 20.f, ImClamp(15.f * dt, 0.f, 1.f));
            }
        }

        if (tsw) { tab = ttab; tsw = false; }

        if (!configs_loaded) { create_config_directory(); scan_configs(); configs_loaded = true; }
    }

    void render() {
        ImGui::GetIO().FontGlobalScale = cfg::other::gui_scale;
        bar::render();
        float dt = ImGui::GetIO().DeltaTime;
        ma = lrp(ma, bar::g_open ? 1.f : 0.f, ImClamp(12.f * dt, 0.f, 1.f));
        if (ma > 0.01f) { ImDrawList* bg = ImGui::GetBackgroundDrawList(); bg->AddRectFilled(ImVec2(0,0), ImVec2(g_sw,g_sh), IM_COL32(0,0,0,(int)(200*ma*bar::game_alpha()))); }
        if (ma < 0.01f) return;
        tick();

        if (!menu_pos_init) { menu_pos = ImVec2((g_sw - menu_size.x)*0.5f, (g_sh - menu_size.y)*0.5f); menu_pos_init = true; }

        // Стили
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ma);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6,4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8,10));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 3.f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 4.f);

        // Цвета
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.09f, 0.11f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.11f, 0.12f, 0.15f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.14f, 0.17f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.18f, 0.20f, 0.24f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.22f, 0.24f, 0.28f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.20f, 0.22f, 0.26f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.13f, 0.14f, 0.17f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.22f, 0.26f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.75f, 1.0f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.35f, 0.75f, 1.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.35f, 0.75f, 1.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.45f, 0.85f, 1.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.87f, 0.90f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.35f, 0.75f, 1.0f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.35f, 0.75f, 1.0f, 0.4f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.35f, 0.75f, 1.0f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.20f, 0.22f, 0.26f, 1.f));

        // Основное окно с заголовком внутри
        ImGui::SetNextWindowPos(menu_pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(menu_size, ImGuiCond_Always);

        ImGui::Begin("MenuWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

        ImGuiWindow* w_ctx = ImGui::GetCurrentWindow();
        ImDrawList* dl = w_ctx->DrawList;
        ImVec2 p = w_ctx->DC.CursorPos;

        // Drag зона (верхняя часть)
        ImVec2 drag_min(menu_pos.x, menu_pos.y), drag_max(menu_pos.x + menu_size.x, menu_pos.y + 35.f);
        if (!dragging_menu && ImGui::IsMouseClicked(0)) {
            ImVec2 mp = ImGui::GetMousePos();
            if (mp.x>=drag_min.x && mp.x<=drag_max.x && mp.y>=drag_min.y && mp.y<=drag_max.y) {
                dragging_menu=true; drag_start_pos=mp; drag_start_menu=menu_pos;
            }
        }
        if (dragging_menu) {
            if (ImGui::IsMouseDown(0)) {
                ImVec2 mp=ImGui::GetMousePos();
                menu_pos=ImVec2(drag_start_menu.x+mp.x-drag_start_pos.x, drag_start_menu.y+mp.y-drag_start_pos.y);
            } else dragging_menu=false;
        }

        // Верхняя панель с заголовком
        dl->AddRectFilled(p, ImVec2(p.x+menu_size.x, p.y+35.f), panel_col(ma), 6.f, ImDrawFlags_RoundCornersTop);
        dl->AddRectFilled(p, ImVec2(p.x+menu_size.x, p.y+2.f), accent_col(ma), 1.f);

        // Название в верхней панели
        ImGui::PushFont(fontMedium);
        const char* title = "unnamed";
        ImVec2 ts = ImGui::CalcTextSize(title);
        dl->AddText(ImVec2(p.x + 15.f, p.y + 8.f), accent_col(ma), title);

        // Версия справа
        const char* ver = "v0.39.2";
        ImVec2 vs = ImGui::CalcTextSize(ver);
        dl->AddText(ImVec2(p.x + menu_size.x - vs.x - 15.f, p.y + 8.f), ImColor(0.5f, 0.55f, 0.6f, ma), ver);
        ImGui::PopFont();

        // Фон под заголовком
        dl->AddRectFilled(ImVec2(p.x, p.y+35.f), ImVec2(p.x+menu_size.x, p.y+menu_size.y), dark_col(ma), 6.f, ImDrawFlags_RoundCornersBottom);

        // Левая панель
        dl->AddRectFilled(ImVec2(p.x, p.y+35.f), ImVec2(p.x+sw, p.y+menu_size.y), panel_col(ma), 6.f, ImDrawFlags_RoundCornersBottomLeft);

        // Разделитель
        dl->AddLine(ImVec2(p.x+sw, p.y+45.f), ImVec2(p.x+sw, p.y+menu_size.y-10.f), border_col(ma), 1.f);

        // Вкладки
        for (int i = 0; i < tc; i++) {
            float ty = p.y + 50.f + i*sth;
            ImVec2 tmin(p.x+10.f, ty), tmax(p.x+sw-10.f, ty+sth-8.f), tsz(tmax.x-tmin.x, tmax.y-tmin.y);
            bool sel = (tab == i);

            if (sel) {
                dl->AddRectFilled(tmin, tmax, accent_col(0.15f * ma), 4.f);
                dl->AddRectFilled(ImVec2(tmin.x, tmin.y), ImVec2(tmin.x+3.f, tmax.y), accent_col(ma), 2.f);
            }

            ImGui::PushFont(fontMedium);
            dl->AddText(ImVec2(tmin.x+15.f, tmin.y+8.f), sel ? accent_col(ma) : ImColor(0.5f, 0.55f, 0.6f, ma), tabs[i]);
            ImGui::PopFont();

            ImGui::SetCursorScreenPos(tmin);
            ImGui::InvisibleButton(("##tab"+std::to_string(i)).c_str(), tsz);
            if (ImGui::IsItemClicked() && tab != i && !tsw) { ttab = i; tsw = true; }
        }

        // Контент
        float top_offset = 50.f;
        ImGui::SetCursorPos(ImVec2(sw+15.f + tab_slide[tab], top_offset));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ma * tab_alpha[tab]);
        ImGui::BeginChild("ContentChild", ImVec2(menu_size.x-sw-30.f, menu_size.y-top_offset-15.f), false, ImGuiWindowFlags_NavFlattened);

        if (tab == 0) {
            // VISUALS
            ImGui::TextColored(accent_col(ma), "VISUALS");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Box ESP", &cfg::esp::box);
            ImGui::Checkbox("Name ESP", &cfg::esp::name);
            ImGui::Checkbox("Health Bar", &cfg::esp::health);
            ImGui::Checkbox("Distance", &cfg::esp::distance);
            ImGui::Checkbox("Skeleton", &opt_skeleton);
            ImGui::Checkbox("RGB ESP", &opt_rgb_esp);

            ImGui::Spacing();
            ImGui::Combo("Box Style", &cfg::esp::box_type, "Full\0Corner\0");
            ImGui::SliderFloat("Box Rounding", &cfg::esp::box_rounding, 0.f, 10.f, "%.0f");

            if (!opt_rgb_esp) {
                ImGui::ColorEdit4("Box Color", (float*)&cfg::esp::box_col, ImGuiColorEditFlags_NoInputs);
                ImGui::ColorEdit4("Name Color", (float*)&cfg::esp::name_col, ImGuiColorEditFlags_NoInputs);
            }
        }
        else if (tab == 1) {
            // AIMBOT
            ImGui::TextColored(accent_col(ma), "AIMBOT");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Enable Aimbot", &cfg::aim::enabled);
            if (cfg::aim::enabled) {
                ImGui::SliderFloat("FOV", &cfg::aim::fov, 1.f, 180.f, "%.0f");
                ImGui::SliderFloat("Smoothness", &cfg::aim::smooth, 1.f, 50.f, "%.1f");
                ImGui::SliderFloat("Max Distance", &cfg::aim::max_distance, 10.f, 500.f, "%.0f");
                ImGui::Combo("Target", &cfg::aim::target, "Head\0Neck\0Chest\0Pelvis\0");
                ImGui::Checkbox("Visible Check", &cfg::aim::visible_check);
                ImGui::Checkbox("Show FOV Circle", &cfg::aim::lock_line);
                ImGui::Checkbox("Aspect Ratio Hack", &cfg::aim::aspect_ratio);
            }
        }
        else if (tab == 2) {
            // MISC
            ImGui::TextColored(accent_col(ma), "MISC");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Custom Crosshair", &cfg::other::crosshair);
            if (cfg::other::crosshair) {
                ImGui::Combo("Crosshair Type", &cfg::other::crosshair_type, "Cross\0Dot\0Circle\0");
                ImGui::SliderFloat("Size", &cfg::other::crosshair_size, 2.f, 50.f, "%.0f");
                ImGui::ColorEdit4("Color", (float*)&cfg::other::crosshair_col, ImGuiColorEditFlags_NoInputs);
            }
        }
        else if (tab == 3) {
            // CONFIG
            ImGui::TextColored(accent_col(ma), "CONFIGURATION");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Config Name:");
            ImGui::InputText("##configname", config_name, IM_ARRAYSIZE(config_name));
            ImGui::Spacing();

            // Кнопки Save и Load
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.75f, 1.0f, 0.3f));
            if (ImGui::Button("SAVE CONFIG", ImVec2(-1, 30))) {
                save_config(config_name);
            }
            ImGui::PopStyleColor();
            ImGui::Spacing();

            if (ImGui::Button("LOAD CONFIG", ImVec2(-1, 30))) {
                load_config(config_name);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::TextColored(accent_col(ma), "Saved Configs:");
            ImGui::Spacing();

            if (config_list.empty()) {
                ImGui::TextColored(ImColor(0.5f, 0.55f, 0.6f, ma), "No configs saved");
            } else {
                for (const auto& cfg_file : config_list) {
                    ImGui::PushID(cfg_file.c_str());

                    ImGui::Text("%s", cfg_file.c_str());
                    ImGui::SameLine();

                    if (ImGui::Button("LOAD", ImVec2(60, 25))) {
                        load_config(cfg_file);
                    }
                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.15f, 0.15f, 1.f));
                    if (ImGui::Button("DELETE", ImVec2(60, 25))) {
                        delete_config(cfg_file);
                    }
                    ImGui::PopStyleColor();

                    ImGui::PopID();
                    ImGui::Spacing();
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SliderFloat("GUI Scale", &cfg::other::gui_scale, 0.5f, 2.5f, "x%.2f");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.15f, 0.15f, 1.f));
            if (ImGui::Button("EXIT", ImVec2(-1, 35))) { shutdown(); exit(0); }
            ImGui::PopStyleColor();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::End();

        ImGui::PopStyleColor(17);
        ImGui::PopStyleVar(10);
    }
}