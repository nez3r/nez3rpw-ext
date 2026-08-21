#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include <cmath>
#include <chrono>
#include <string>
#include <algorithm>
#include <chrono>

class animation {
public:
    animation(float i = 0.f) : value(i) {}

    auto update(float to_value, float duration = 0.10f) -> void;
    float value = 0.f;
};

class animation_vec2 {
public:
    animation_vec2(ImVec2 value = ImVec2(0, 0)) : value(value) {}

    auto update(ImVec2 to_value, float duration = 0.10f) -> void;
    struct ImVec2 value = { 0, 0 };
};

class animation_vec4 {
public:
    animation_vec4(ImVec4 value = ImVec4(0, 0, 0, 0)) : value(value) {}

    auto interpolate(ImVec4 first_value, ImVec4 second_value, bool interp, float duration = 0.10f) -> void;
    auto update(ImVec4 to_value, float duration = 0.10f) -> void;
    struct ImVec4 value = { 0, 0, 0, 0 };
};

class color_utils {
public:
    ImVec4 accent = ImColor(255, 211, 145).Value;
public:
    ImVec4 process_alpha(ImVec4 input, float alpha = 1.0f);
    ImVec4 get_accent_imv4(float alpha = 1.0f, float shading = 1.0f);
    ImVec4* get_accent_imv4_ptr();
    ImColor get_accent_imc(float alpha = 1.0f, float shading = 1.0f);
};

class scaling_system_t {
public:
    float current_scale{ 1 };
public:
    float get(float input);
};

inline scaling_system_t* c_scale = new scaling_system_t();
inline color_utils* c_utils = new color_utils();
