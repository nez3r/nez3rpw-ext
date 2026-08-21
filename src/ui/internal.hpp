#pragma once

// IMGUI INTERNAL METHODS

#include "widgets.hpp"

using namespace ImGui;

float calculate_width()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID active_id = ImGui::GetActiveID();
    bool any_scrollbar_active = window->ScrollbarX || window->ScrollbarY;
    float offset = any_scrollbar_active ? 16 : 0;
    float w = window->Size.x - g.Style.WindowPadding.x * 2 - offset;

    if (w < 0.0f)
    {
        float region_max_x = ImGui::GetContentRegionAvail().x;
        w = ImMax(1.0f, region_max_x - window->DC.CursorPos.x + w);
    }
    w = ImFloor(w);
    return w;
}

static void ColorEditRestoreH(const float* col, float* H)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ColorEditCurrentID != 0);
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;
    *H = g.ColorEditSavedHue;
}

static void ColorEditRestoreHS(const float* col, float* H, float* S, float* V)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ColorEditCurrentID != 0);
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;

    // When S == 0, H is undefined.
    // When H == 1 it wraps around to 0.
    if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
        *H = g.ColorEditSavedHue;

    // When V == 0, S is undefined.
    if (*V == 0.0f)
        *S = g.ColorEditSavedSat;
}

static void RenderArrowsForVerticalBar(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w, float alpha)
{
    draw_list->AddRectFilled(pos + ImVec2(0, -2), { pos.x + bar_w, pos.y + 2 }, ImColor(1.0f, 1.0f, 1.0f, GetStyle().Alpha), 3);
}
