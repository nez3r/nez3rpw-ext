#pragma once

#include "imgui.h"
#include "ui.hpp"
#include "menu_vars.hpp"
#include "font_awesome.hpp"
#include <cstdio>
#include <ctime>

// espFont from draw.cpp (loaded from esp_font.h, same as reference)
#include "Android_draw/draw.h"

namespace watermark {

inline void init() {
    // Fonts are initialized by draw.cpp (espFont) and c_ui->initialize_fonts() (icons)
}

struct part {
    const char* icon;   // null if no icon
    const char* text;
};

inline void render() {
    if (!opt_wm || !c_ui)
        return;

    // Use esp_font from draw.cpp for watermark text (like reference)
    // Use c_ui->get_font_icons() for icons
    ImFont* iconFont = c_ui ? c_ui->get_font_icons() : nullptr;
    if (!espFont || !iconFont)
        return;

    ImGuiIO& io = ImGui::GetIO();
    float sw = io.DisplaySize.x;
    float sc = sw / 1920.f * s_wm_scale;

    char fps_buf[16];
    float f_fps = io.Framerate;
    if (!(f_fps > 1.f) || f_fps > 10000.f) f_fps = 0.f;
    snprintf(fps_buf, sizeof(fps_buf), "FPS: %.0f", f_fps);

    time_t now = time(0);
    tm* ltm = localtime(&now);
    char time_buf[16];
    snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

    // только те части, что включены в настройках
    struct part {
        const char* icon;
        const char* text;
    };
    part parts[8];
    int n = 0;
    if (opt_wm_show_site)    parts[n++] = { ICON_FA_GLOBE, "nez3r.pw" };
    if (opt_wm_show_type)    parts[n++] = { ICON_FA_BOLT, "External" };
    if (opt_wm_show_fps)     parts[n++] = { ICON_FA_MICROCHIP, fps_buf };
    if (opt_wm_show_time)    parts[n++] = { ICON_FA_CLOCK, time_buf };
    if (opt_wm_show_version) parts[n++] = { ICON_FA_TAG, "0.39.2" };
    if (n == 0) return;

    const char* sep = " | ";

    float text_sz = 18.f * sc;
    float icon_sz = 17.f * sc;
    float icon_gap = 6.f * sc;
    ImVec2 sep_size = espFont->CalcTextSizeA(text_sz, FLT_MAX, 0.f, sep);

    // ширина: [icon+gap+text] для каждой части, между частями сепаратор
    float content_w = 0.f;
    float max_h = text_sz;
    for (int i = 0; i < n; i++) {
        if (i > 0) content_w += sep_size.x;
        if (opt_wm_icons && iconFont && parts[i].icon) content_w += icon_sz + icon_gap;
        ImVec2 ts = espFont->CalcTextSizeA(text_sz, FLT_MAX, 0.f, parts[i].text);
        content_w += ts.x;
        if (ts.y > max_h) max_h = ts.y;
    }
    if (opt_wm_icons && iconFont && icon_sz > max_h) max_h = icon_sz;

    float pad_x = 14.f * sc;
    float pad_y = 8.f * sc;
    float rounding = 8.f * sc;

    float content_h = pad_y * 2.f + max_h;
    // сторона: 0 = слева, 1 = справа
    float rx = (s_wm_side == 0)
        ? 16.f * sc
        : sw - content_w - pad_x * 2.f - 16.f * sc;
    float ry = 16.f * sc;

    ImDrawList* dl = ImGui::GetForegroundDrawList();

    dl->AddRectFilled(
        ImVec2(rx, ry),
        ImVec2(rx + content_w + pad_x * 2.f, ry + content_h),
        IM_COL32(18, 18, 18, 190),
        rounding
    );
    dl->AddRect(
        ImVec2(rx, ry),
        ImVec2(rx + content_w + pad_x * 2.f, ry + content_h),
        IM_COL32(70, 70, 70, 160),
        rounding
    );

    ImU32 txt_col = IM_COL32(
        (int)(s_wm_col.x * 255),
        (int)(s_wm_col.y * 255),
        (int)(s_wm_col.z * 255),
        (int)(s_wm_col.w * 255)
    );

    float cur_x = rx + pad_x;
    float cy = ry + content_h * 0.5f;

    for (int i = 0; i < n; i++) {
        if (i > 0) {
            cur_x += sep_size.x;
        }
        if (opt_wm_icons && iconFont && parts[i].icon) {
            ImVec2 isz = iconFont->CalcTextSizeA(icon_sz, FLT_MAX, 0.f, parts[i].icon);
            // вертикальный нюдж: у FontAwesome базовая линия ниже текстовой
            float dy = isz.y * 0.14f;
            dl->AddText(iconFont, icon_sz, ImVec2(cur_x, cy - isz.y * 0.5f - dy), txt_col, parts[i].icon);
            cur_x += isz.x + icon_gap;
        }
        ImVec2 ts = espFont->CalcTextSizeA(text_sz, FLT_MAX, 0.f, parts[i].text);
        dl->AddText(espFont, text_sz, ImVec2(cur_x, cy - ts.y * 0.5f), txt_col, parts[i].text);
        cur_x += ts.x;
    }
}

}
