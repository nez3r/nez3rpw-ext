#pragma once

#include "imgui.h"

struct cvars
{
    struct cbools
    {
        bool enable_esp;
        bool enable_box;
        bool enable_line;
        bool enable_hpbar;
        bool enable_name;
        bool enable_skeleton;
        bool crash;

        bool enable_aimbot;
        bool enable_silent;
        bool enable_firecheck;

        bool enable_invisible_exploit;
        bool enable_untouchable_check;

        bool enable_hitboxes;

        bool *enable_bone;
    };

    struct cfloats
    {
        float box_rounding  = 0.f;
        float box_thickness = 1.f;

        float aimbot_fov;
        float aimbot_fovy;
        float aimbot_smooth;
    };

    struct cintegers
    {
        int fov_type;
        int aiming_type;
    };

    struct ccolors
    {
        ImColor box_color = ImColor(255, 255, 255, 255);
        ImColor line_color = ImColor(255, 255, 255, 255);
        ImColor outline_color = ImColor(0, 0, 0, 255);
        ImColor fov_color = ImColor(0, 0, 0, 255);
        ImColor skeleton_visible_color = ImColor(0, 255, 0, 255);
        ImColor skeleton_invisible_color = ImColor(255, 0, 0, 255);
    };

    cbools bools{};
    cfloats floats{};
    cintegers integers{};
    ccolors colors{};

    static cvars &vars();
};

inline cvars &cvars::vars() {
    static cvars instance{};
    return instance;
}