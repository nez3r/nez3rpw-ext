#pragma once

#ifndef LOGGING_HEADER
#define LOGGING_HEADER

#include <chrono>
#include <stdint.h>
#include <string>
#include <deque>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "util.hpp"

#define ay_string 
#define ay_obfuscate

struct text {
    std::string name;
    std::string bone;
    std::string damage;
};

struct log_t {
    text _text;
    int64_t creation_time;
    int64_t duration;
    int animation = 0;
    int bganimation = 0;
    int animpos1 = 20;
    int animpos2 = 0;
};

namespace cstd {
    template<class T, class A, class Predicate>
    inline void erase_if(std::deque<T, A>& c, Predicate pred) {
        c.erase(remove_if(c.begin(), c.end(), pred), c.end());
    }
}

inline ImFont* logs_font{};

inline std::deque<log_t> logs_list;

inline void add_text_outline_l(ImDrawList* draw, ImFont* font, float font_size, ImVec2 pos, ImColor col, const char* ctext) {
    float alpha = 1;//0.7f

    draw->AddText(font, font_size, pos + ImVec2(-1, -1), ImColor(0.0f, 0.0f, 0.0f, col.Value.w * alpha), ctext);
    draw->AddText(font, font_size, pos + ImVec2(0, -1), ImColor(0.0f, 0.0f, 0.0f, col.Value.w * alpha), ctext);
    draw->AddText(font, font_size, pos + ImVec2(1, -1), ImColor(0.0f, 0.0f, 0.0f, col.Value.w * alpha), ctext);
    draw->AddText(font, font_size, pos + ImVec2(-1, 0), ImColor(0.0f, 0.0f, 0.0f, col.Value.w * alpha), ctext);
    draw->AddText(font, font_size, pos + ImVec2(1, 0), ImColor(0.0f, 0.0f, 0.0f, col.Value.w * alpha), ctext);
    draw->AddText(font, font_size, pos + ImVec2(-1, 1), ImColor(0.0f, 0.0f, 0.0f, col.Value.w * alpha), ctext);
    draw->AddText(font, font_size, pos + ImVec2(0, 1), ImColor(0.0f, 0.0f, 0.0f, col.Value.w * alpha), ctext);
    draw->AddText(font, font_size, pos + ImVec2(1, 1), ImColor(0.0f, 0.0f, 0.0f, col.Value.w * alpha), ctext);
    draw->AddText(font, font_size, pos, col, ctext);
}

inline void add_text_glow(float font_size, ImDrawList* draw, ImVec2 pos, ImColor col, const char* value, int glow_size = 22) {
    const auto& size = ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, value);

    add_text_outline_l(draw, ImGui::GetFont(), font_size, pos, col, value);
    //draw->AddShadowRect(pos + ImVec2(glow_size / 4, size.y / 2), pos + ImVec2(size.x - glow_size / 4, size.y / 2), ImColor(col.Value.x, col.Value.y, col.Value.z, 0.8f), glow_size, { 0, 0 }, 0, glow_size * 1.5f);
}

inline ImColor get_accent_imcolor(const float& alpha) {
    const ImColor& accent_color = c_utils->get_accent_imc();
    return { accent_color.Value.x, accent_color.Value.y, accent_color.Value.z, alpha };
}

namespace logs {
    using namespace std::chrono;

    template<typename ... Args>
    inline void push(const text& value) {
        logs_list.push_back({
                                    value,
                                    duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(),
                                    6000
            });
    }

    inline void render(ImVec2 screen_size, ImDrawList* draw, int log_height) {
        int64_t current_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        cstd::erase_if(logs_list, [current_time](auto& it) { return (it.animpos2 >= 20 && it.bganimation <= 0); });

        int count = 0;
        for (int i = 0; i < logs_list.size(); i++) {
            if (count > 10) break;
            if (logs_list[i].animpos1 > 0) logs_list[i].animpos1 -= 1;
            if (logs_list[i].animpos1 > 0 && logs_list[i].animation < 450) logs_list[i].animation += 20;
            if (logs_list[i].animpos1 > 0 && logs_list[i].bganimation < 450) logs_list[i].bganimation += 20;
            if (logs_list[i].animpos1 == 0 && logs_list[i].animation > 0) logs_list[i].animation -= 1;
            if (logs_list[i].animpos1 == 0 && logs_list[i].bganimation > 0) logs_list[i].bganimation -= 1;
            if (logs_list[i].bganimation <= 60 && logs_list[i].animpos2 < 750) logs_list[i].animpos2 += 1;
            //draw->AddRectFilled(ImVec2(screen_size.x / 2 - ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i].text.c_str()).x / 2 - 10, screen_size.y / 2 + 60+50*ImGui::GetIO().FontGlobalScale+60*ImGui::GetIO().FontGlobalScale + log_height * count - 10 - logs_list[i].animpos1 + logs_list[i].animpos2), ImVec2(screen_size.x / 2 + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i].text.c_str()).x / 2 + 10, screen_size.y / 2 + 60+50*ImGui::GetIO().FontGlobalScale+60*ImGui::GetIO().FontGlobalScale + log_height * count + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i].text.c_str()).y+10- logs_list[i].animpos1 + logs_list[i].animpos2), ImColor(0, 0, 0, logs_list[i].bganimation/3.5), 2);

            std::string full_text = ay_string("Hit ");
            full_text += logs_list[i]._text.name;
            full_text += ay_string(" in ");
            full_text += logs_list[i]._text.bone;
            full_text += ay_string(" for ");
            full_text += logs_list[i]._text.damage;
            full_text += ay_string(" damage");

            const ImVec2& begin_point = { 20, 20 };

            const auto& tex_beg = ImVec2(begin_point.x, begin_point.y + log_height * count - logs_list[i].animpos1 + logs_list[i].animpos2) - ImVec2(5, 5);
            const auto& tex_end = tex_beg + ImVec2(ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate("Hit ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.name.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" in ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.bone.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" for approx. ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.damage.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" damage")).x, ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, full_text.c_str()).y) + ImVec2(5 * 2, 5 * 2);

            draw->AddRectFilled(tex_beg, tex_end, ImColor(15, 15, 15, logs_list[i].animation), 3);
            draw->AddRect(tex_beg, tex_end, ImColor(24, 24, 24, logs_list[i].animation), 3);

            add_text_outline_l(draw, ImGui::GetFont(), 12, ImVec2(begin_point.x, begin_point.y + log_height * count - logs_list[i].animpos1 + logs_list[i].animpos2), ImColor(255, 255, 255, logs_list[i].animation), ay_obfuscate("Hit "));
            add_text_glow(12, draw, ImVec2(begin_point.x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate("Hit ")).x, begin_point.y + log_height * count - logs_list[i].animpos1 + logs_list[i].animpos2), get_accent_imcolor(static_cast<float>(logs_list[i].animation / 255.f)), logs_list[i]._text.name.c_str());
            add_text_outline_l(draw, ImGui::GetFont(), 12, ImVec2(begin_point.x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate("Hit ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.name.c_str()).x, begin_point.y + log_height * count - logs_list[i].animpos1 + logs_list[i].animpos2), ImColor(255, 255, 255, logs_list[i].animation), ay_obfuscate(" in "));
            add_text_glow(12, draw, ImVec2(begin_point.x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate("Hit ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.name.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" in ")).x, begin_point.y + log_height * count - logs_list[i].animpos1 + logs_list[i].animpos2), get_accent_imcolor(static_cast<float>(logs_list[i].animation / 255.f)), logs_list[i]._text.bone.c_str());
            add_text_outline_l(draw, ImGui::GetFont(), 12, ImVec2(begin_point.x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate("Hit ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.name.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" in ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.bone.c_str()).x, begin_point.y + log_height * count - logs_list[i].animpos1 + logs_list[i].animpos2), ImColor(255, 255, 255, logs_list[i].animation), ay_obfuscate(" for approx. "));
            add_text_glow(12, draw, ImVec2(begin_point.x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate("Hit ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.name.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" in ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.bone.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" for approx. ")).x, begin_point.y + log_height * count - logs_list[i].animpos1 + logs_list[i].animpos2), get_accent_imcolor(static_cast<float>(logs_list[i].animation / 255.f)), logs_list[i]._text.damage.c_str());
            add_text_outline_l(draw, ImGui::GetFont(), 12, ImVec2(begin_point.x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate("Hit ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.name.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" in ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.bone.c_str()).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, ay_obfuscate(" for approx. ")).x + ImGui::GetFont()->CalcTextSizeA(12, FLT_MAX, 0, logs_list[i]._text.damage.c_str()).x, begin_point.y + log_height * count - logs_list[i].animpos1 + logs_list[i].animpos2), ImColor(255, 255, 255, logs_list[i].animation), ay_obfuscate(" damage"));

            ++count;
        }

    }
}

#endif