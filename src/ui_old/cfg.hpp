#pragma once
#include "imgui.h"

namespace cfg {
    namespace esp {
        inline bool box = false;
        inline bool name = false;
        inline bool health = false;
        inline bool distance = false;
        inline int box_type = 0;
        inline float box_rounding = 0.f;

        inline ImVec4 box_col = ImVec4(1.f, 1.f, 1.f, 1.f);
        inline ImVec4 name_col = ImVec4(1.f, 1.f, 1.f, 1.f);
        inline ImVec4 health_col = ImVec4(0.f, 1.f, 0.f, 1.f);
        inline ImVec4 distance_col = ImVec4(1.f, 1.f, 1.f, 1.f);
    }

    namespace aim {
        inline bool enabled = false;
        inline float fov = 30.f;
        inline float smooth = 0.100f;
        inline float max_distance = 300.f;
        inline int target = 0;
        inline bool visible_check = false;
        inline bool lock_line = false;
        inline bool lock_dot = false;

        inline bool aspect_ratio = false;
        inline bool aspect_stretch = false;
        inline float aspect_value = 1.0f;
        inline float aspect_stretch_value = 1.0f;
    }

    namespace other {
        inline bool crosshair = false;
        inline int crosshair_type = 0;
        inline float crosshair_size = 10.f;
        inline ImVec4 crosshair_col = ImVec4(1.f, 0.f, 0.f, 1.f);
        inline float gui_scale = 1.0f;
    }
}
