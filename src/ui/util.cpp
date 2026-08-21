#include "util.hpp"

inline float ease(float time, float start_value, float change_value, float duration) {
    return change_value * time / duration + start_value;
}

float resolve_value(float clock, float previous_value, float new_value, float duration) {
    float value = ease(clock, previous_value, new_value - previous_value, duration);

    if (std::abs(value) < .001f) {
        value = new_value;
    }

    if (std::fmod(value, 1) < .001f) {
        value = std::floor(value);
    }
    else if (std::fmod(value, 1) > .99f) {
        value = std::ceil(value);
    }

    return value;
}

auto animation::update(float to_value, float duration) -> void {
    this->value = resolve_value(ImGui::GetIO().DeltaTime, this->value, to_value, duration);
}

auto animation_vec2::update(ImVec2 to_value, float duration) -> void {
    this->value = {
            resolve_value(ImGui::GetIO().DeltaTime, this->value.x, to_value.x, duration),
            resolve_value(ImGui::GetIO().DeltaTime, this->value.y, to_value.y, duration)
    };
}

auto animation_vec4::interpolate(ImVec4 first_value, ImVec4 second_value, bool interp, float duration) -> void {
    if (interp)
        this->update(first_value, duration);
    else
        this->update(second_value, duration);
}

auto animation_vec4::update(ImVec4 to_value, float duration) -> void {
    this->value = {
            resolve_value(ImGui::GetIO().DeltaTime, this->value.x, to_value.x, duration),
            resolve_value(ImGui::GetIO().DeltaTime, this->value.y, to_value.y, duration),
            resolve_value(ImGui::GetIO().DeltaTime, this->value.z, to_value.z, duration),
            resolve_value(ImGui::GetIO().DeltaTime, this->value.w, to_value.w, duration)
    };
}

ImVec4 color_utils::process_alpha(ImVec4 input, float alpha) {
    return { input.x, input.y, input.z, input.w * alpha };
}

ImVec4 color_utils::get_accent_imv4(float alpha, float shading) {
    return { this->accent.x * shading, this->accent.y * shading, this->accent.z * shading, this->accent.w * ImGui::GetStyle().Alpha * alpha };
}

ImVec4* color_utils::get_accent_imv4_ptr() {
    return &this->accent;
}

ImColor color_utils::get_accent_imc(float alpha, float shading) {
    return { this->accent.x * shading, this->accent.y * shading, this->accent.z * shading, this->accent.w * ImGui::GetStyle().Alpha * alpha };
}

float scaling_system_t::get(float input) {
    return input * this->current_scale;
}