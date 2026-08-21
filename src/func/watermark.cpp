#include "watermark.hpp"
#include "imgui.h"
#include "imgui_internal.h"

namespace watermark {
    void draw() {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        if (!dl) return;

        float fps = ImGui::GetIO().Framerate;
        char buf[64];
        snprintf(buf, sizeof(buf), "unnamed | FPS: %.0f", fps);

        ImVec2 pos(10, 10);

        // Тень
        dl->AddText(ImVec2(pos.x + 1, pos.y + 1), IM_COL32(0, 0, 0, 200), buf);
        // Белый текст
        dl->AddText(pos, IM_COL32(255, 255, 255, 255), buf);
    }
}