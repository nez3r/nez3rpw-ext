#include "bar.hpp"
#include "theme/theme.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <cmath>

namespace ui::bar {
    static float lrp(float a, float b, float t) { return a + (b - a) * t; }
    static float ease(float t) { return -(cosf(3.14159265f * t) - 1.f) / 2.f; }

    void set_game_alpha(float a) { g_game_alpha = a; }
    float game_alpha() { return g_game_alpha; }

    void render() {
        if (g_game_alpha < 0.001f) return;

        ImGuiIO& io = ImGui::GetIO();
        static bool state = true;
        static float prog = 0.f;
        static float lt = 0.f;

        float ct = ImGui::GetTime();

        float sc = g_sw / 1920.f;
        float bw = 350.f * sc;
        float bh = 10.f;

        ImRect r(
            ImVec2((g_sw * 0.5f) - (bw * 0.5f), g_sh - bh - 10.f),
            ImVec2((g_sw * 0.5f) + (bw * 0.5f), g_sh - 10.f)
        );

        ImVec2 wp(r.Min.x - 30.f * sc, r.Min.y - 50.f * sc);
        ImVec2 ws(bw + 60.f * sc, bh + 100.f * sc);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowPos(wp);
        ImGui::SetNextWindowSize(ws);
        ImGui::Begin("##bar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        if (ImGui::InvisibleButton("##b", ws)) {
            if (ct - lt >= 0.15f) {
                state = !state;
                lt = ct;
            }
        }
        ImGui::End();
        ImGui::PopStyleVar(2);

        float tgt = state ? 0.f : 1.f;
        float spd = 12.f * io.DeltaTime;
        if (prog < tgt) prog = ImMin(prog + spd, tgt);
        else if (prog > tgt) prog = ImMax(prog - spd, tgt);
        prog = ImClamp(prog, 0.f, 1.f);

        g_alpha = ease(prog);
        g_open = g_alpha > 0.2f;

        ImDrawList* dl = ImGui::GetForegroundDrawList();
        int ba = static_cast<int>(64 * g_game_alpha);
        if (ba > 0) {
            dl->AddRectFilled(r.Min, r.Max, IM_COL32(255, 255, 255, ba), 20.f);
        }
    }
}
