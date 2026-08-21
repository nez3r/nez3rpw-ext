#include "../other/custom_string.h"
#include "visuals.hpp"
#include "../game/game.hpp"
#include "../game/player.hpp"
#include "../game/math.hpp"
#include "../protect/oxorany.hpp"
#include "../ui/menu_vars.hpp"
#include "Android_draw/draw.h"
#include "imgui.h"
#include <cmath>
#include <algorithm>

namespace visuals {

    // Проверка указателей в памяти (защита от крашей при чтении каши)
    static bool likely_ptr(uint64_t p) {
        return p > 0x10000ull && p < 0x0000FFFFFFFFFFFFull;
    }

    // Полный набор оффсетов костей для Standoff 2 (из дампа BipedMap)
    enum BoneId {
        BONE_HEAD         = 0x20,
        BONE_NECK         = 0x28,
        BONE_SPINE        = 0x30,
        BONE_SPINE1       = 0x38,
        BONE_SPINE2       = 0x40,
        BONE_L_SHOULDER   = 0x48,
        BONE_L_UPPERARM   = 0x50,
        BONE_L_FOREARM    = 0x58,
        BONE_L_HAND       = 0x60,
        BONE_R_SHOULDER   = 0x68,
        BONE_R_UPPERARM   = 0x70,
        BONE_R_FOREARM    = 0x78,
        BONE_R_HAND       = 0x80,
        BONE_PELVIS       = 0x88, // Hip
        BONE_L_UPLEG      = 0x90,
        BONE_L_LEG        = 0x98,
        BONE_L_FOOT       = 0xA0,
        BONE_L_TOE        = 0xA8,
        BONE_R_UPLEG      = 0xB0,
        BONE_R_LEG        = 0xB8,
        BONE_R_FOOT       = 0xC0,
        BONE_R_TOE        = 0xC8
    };

    // Чтение 3D позиции конкретной кости из памяти
    static Vector3 get_bone_pos(uint64_t player_ptr, uint32_t bone_offset) {
        uint64_t charView = rpm<uint64_t>(player_ptr + oxorany(0x48));
        if (!likely_ptr(charView)) return Vector3(0, 0, 0);

        uint64_t bipedMap = rpm<uint64_t>(charView + oxorany(0x48));
        if (!likely_ptr(bipedMap)) return Vector3(0, 0, 0);

        uint64_t boneTransform = rpm<uint64_t>(bipedMap + bone_offset);
        if (!likely_ptr(boneTransform)) return Vector3(0, 0, 0);

        return player::get_transform_position(boneTransform);
    }

    // --- ESP Line (как в privet: от выбранной позиции экрана до верха бокса) ---
    static void draw_esp_line(ImDrawList* dl, float cx, float y1, ImU32 color) {
        float screen_mid_x = (float)g_sw * 0.5f;
        float edge_y = 0.f;
        if (s_line_pos == 1) edge_y = 0.f;                    // top
        else if (s_line_pos == 2) edge_y = (float)g_sh;       // bottom
        else edge_y = (float)g_sh * 0.5f;                     // middle (default)
        dl->AddLine(ImVec2(screen_mid_x, edge_y), ImVec2(cx, y1), color, s_line_thick);
    }

    // --- ESP Skeleton (как в privet: используем скелет с проверками видимости) ---
    struct BoneSegment {
        uint32_t a, b;
    };

    static const BoneSegment skeleton_bones[] = {
        // Spine
        { BONE_HEAD, BONE_NECK },
        { BONE_NECK, BONE_SPINE },
        { BONE_SPINE, BONE_SPINE1 },
        { BONE_SPINE1, BONE_SPINE2 },
        { BONE_SPINE2, BONE_PELVIS },
        // Left arm
        { BONE_NECK, BONE_L_SHOULDER },
        { BONE_L_SHOULDER, BONE_L_UPPERARM },
        { BONE_L_UPPERARM, BONE_L_FOREARM },
        { BONE_L_FOREARM, BONE_L_HAND },
        // Right arm
        { BONE_NECK, BONE_R_SHOULDER },
        { BONE_R_SHOULDER, BONE_R_UPPERARM },
        { BONE_R_UPPERARM, BONE_R_FOREARM },
        { BONE_R_FOREARM, BONE_R_HAND },
        // Left leg
        { BONE_PELVIS, BONE_L_UPLEG },
        { BONE_L_UPLEG, BONE_L_LEG },
        { BONE_L_LEG, BONE_L_FOOT },
        // Right leg
        { BONE_PELVIS, BONE_R_UPLEG },
        { BONE_R_UPLEG, BONE_R_LEG },
        { BONE_R_LEG, BONE_R_FOOT }
    };

    static void draw_bone_line_safe(ImDrawList* dl, uint64_t player, uint32_t bone_a, uint32_t bone_b, matrix& vm, ImU32 color) {
        Vector3 pos_a = get_bone_pos(player, bone_a);
        Vector3 pos_b = get_bone_pos(player, bone_b);

        if (pos_a.x == 0.f && pos_a.y == 0.f && pos_a.z == 0.f) return;
        if (pos_b.x == 0.f && pos_b.y == 0.f && pos_b.z == 0.f) return;

        // ФИКС БАГОВ: Если расстояние между костями аномально большое (>2.5 м),
        // значит Transform ещё не обновился — пропускаем кадр
        if (calculate_distance(pos_a, pos_b) > 2.5f) return;

        ImVec2 screen_a, screen_b;
        if (world_to_screen(pos_a, vm, screen_a) && world_to_screen(pos_b, vm, screen_b)) {
            dl->AddLine(screen_a, screen_b, color, s_line_thick > 0 ? s_line_thick : 1.5f);
        }
    }

    static void draw_skeleton(ImDrawList* dl, uint64_t player, matrix& vm, ImU32 color) {
        for (const auto& seg : skeleton_bones) {
            draw_bone_line_safe(dl, player, seg.a, seg.b, vm, color);
        }
    }

    void draw_esp(ImDrawList* dl, uint64_t player, matrix& vm, int lt, Vector3 local_pos) {
        if (rpm<uint8_t>(player + oxorany(0x79)) == lt) return;
        if (player::health(player) <= 0) return;

        Vector3 pos = player::position(player);
        if (pos.x == 0.f && pos.y == 0.f && pos.z == 0.f) return;

        Vector3 head_pos(pos.x, pos.y + 1.8f, pos.z);
        ImVec2 foot_s, head_s;
        if (!world_to_screen(pos, vm, foot_s)) return;
        if (!world_to_screen(head_pos, vm, head_s)) return;

        float h = foot_s.y - head_s.y;
        float w = h * 0.4f;
        float x = head_s.x - w * 0.5f;

        // --- ДИНАМИЧЕСКИЙ РАСЧЕТ RGB ЦВЕТА (RAINBOW) ---
        ImU32 box_col, name_col, hp_col, dist_col;

        if (opt_rgb_esp) {
            float rgb_time = ImGui::GetTime() * 1.5f; // Скорость бегущей волны
            float r = std::sin(rgb_time) * 0.5f + 0.5f;
            float g = std::sin(rgb_time + 2.094f) * 0.5f + 0.5f;
            float b = std::sin(rgb_time + 4.189f) * 0.5f + 0.5f;

            ImU32 rainbow = ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, 1.f));
            box_col = rainbow;
            name_col = rainbow;
            dist_col = rainbow;
            hp_col = ImGui::ColorConvertFloat4ToU32(s_health_col1); // ХП оставляем статичным для удобства
        } else {
            // Стандартные цвета из нового конфига
            box_col = ImGui::ColorConvertFloat4ToU32(s_box_col);
            name_col = ImGui::ColorConvertFloat4ToU32(s_name_col);
            hp_col = ImGui::ColorConvertFloat4ToU32(s_health_col1);
            dist_col = ImGui::ColorConvertFloat4ToU32(s_dist_col);
        }

        // ОТРИСОВКА ИСПРАВЛЕННОГО СКЕЛЕТА (как в privet)
        if (opt_skeleton) {
            draw_skeleton(dl, player, vm, name_col);
        }

        // ESP Line (как в privet)
        if (opt_line) {
            ImU32 line_col = ImGui::ColorConvertFloat4ToU32(s_line_col);
            draw_esp_line(dl, head_s.x, head_s.y, line_col);
        }

        // ОТРИСОВКА КВАДРАТОВ (BOX)
        if (opt_box) {
            if (s_box_shape == 0) {
                dl->AddRect(ImVec2(x, head_s.y), ImVec2(x+w, foot_s.y), box_col, s_box_thick, 0, 1.5f);
            } else {
                float cw = w * 0.25f;
                dl->AddLine(ImVec2(x, head_s.y), ImVec2(x, head_s.y+cw), box_col, 1.5f);
                dl->AddLine(ImVec2(x, head_s.y), ImVec2(x+cw, head_s.y), box_col, 1.5f);
                dl->AddLine(ImVec2(x+w, head_s.y), ImVec2(x+w, head_s.y+cw), box_col, 1.5f);
                dl->AddLine(ImVec2(x+w, head_s.y), ImVec2(x+w-cw, head_s.y), box_col, 1.5f);
                dl->AddLine(ImVec2(x, foot_s.y), ImVec2(x, foot_s.y-cw), box_col, 1.5f);
                dl->AddLine(ImVec2(x, foot_s.y), ImVec2(x+cw, foot_s.y), box_col, 1.5f);
                dl->AddLine(ImVec2(x+w, foot_s.y), ImVec2(x+w, foot_s.y-cw), box_col, 1.5f);
                dl->AddLine(ImVec2(x+w, foot_s.y), ImVec2(x+w-cw, foot_s.y), box_col, 1.5f);
            }
        }

        // ПОЛОСКА ЗДОРОВЬЯ (HEALTH)
        if (opt_health) {
            int hp = player::health(player);
            if (hp > 100) hp = 100;
            float hp_h = h * (hp / 100.f);
            dl->AddRectFilled(ImVec2(x-6, foot_s.y), ImVec2(x-3, head_s.y), IM_COL32(0,0,0,150));
            dl->AddRectFilled(ImVec2(x-5, foot_s.y-hp_h), ImVec2(x-4, foot_s.y), hp_col);
        }

        float dx = pos.x - local_pos.x;
        float dy = pos.y - local_pos.y;
        float dz = pos.z - local_pos.z;
        float distance = sqrtf(dx * dx + dy * dy + dz * dz);

        // ИМЕНА ИГРОКОВ (NAME) — шрифт esp из esp_font.h
        if (opt_name) {
            std::string player_name = player::name(player).as_utf8();
            if (player_name.empty()) player_name = "Player";
            float fs = 18.f;
            ImVec2 text_size = espFont->CalcTextSizeA(fs, FLT_MAX, 0.0f, player_name.c_str());
            dl->AddText(espFont, fs, ImVec2(head_s.x - (text_size.x / 2.0f), head_s.y - 15.0f), name_col, player_name.c_str());
        }

        // ДИСТАНЦИЯ (DISTANCE) — шрифт esp
        if (opt_dist) {
            char dist_buf[32];
            snprintf(dist_buf, sizeof(dist_buf), "[%.1fm]", distance);
            float fs = 18.f;
            ImVec2 text_size = espFont->CalcTextSizeA(0.f, FLT_MAX, 0.0f, dist_buf);
            dl->AddText(espFont, fs, ImVec2(foot_s.x - (text_size.x / 2.0f), foot_s.y + 5.0f), dist_col, dist_buf);
        }
    }

    void draw() {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        if (!dl) return;

        uint64_t pm = get_player_manager();
        if (!pm) return;

        uint64_t lp = rpm<uint64_t>(pm + oxorany(0x70));
        if (!lp) return;

        Vector3 local_pos = player::position(lp);
        matrix vm = player::view_matrix(lp);
        int lt = rpm<uint8_t>(lp + oxorany(0x79));

        uint64_t pl = rpm<uint64_t>(pm + oxorany(0x28));
        if (!pl) return;

        int pc = rpm<int>(pl + oxorany(0x20));
        if (pc <= 0 || pc > 64) return;

        uint64_t buf = rpm<uint64_t>(pl + oxorany(0x18));
        if (!buf) return;

        for (int i = 0; i < pc; i++) {
            uint64_t p = rpm<uint64_t>(buf + oxorany(0x30) + oxorany(0x18) * i);
            if (!p || p == lp) continue;
            draw_esp(dl, p, vm, lt, local_pos);
        }

        // КАСТОМНЫЙ ПРИЦЕЛ ПО ЦЕНТРУ ЭКРАНА
        if (opt_line) {
            float cx = g_sw * 0.5f;
            float cy = g_sh * 0.5f;
            float size = s_line_thick;
            ImU32 color = ImGui::ColorConvertFloat4ToU32(s_line_col);

            if (s_line_down == false) {  // 0 = cross
                dl->AddLine(ImVec2(cx - size, cy), ImVec2(cx + size, cy), color, 1.5f);
                dl->AddLine(ImVec2(cx, cy - size), ImVec2(cx, cy + size), color, 1.5f);
            }
            else if (s_line_down == true) {  // 1 = dot/circle
                dl->AddCircleFilled(ImVec2(cx, cy), size * 0.4f, color, 12);
            }
            else {
                dl->AddCircle(ImVec2(cx, cy), size, color, 32, 1.5f);
            }
        }
    }
}