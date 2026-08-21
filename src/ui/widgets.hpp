#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "util.hpp"

#include <string>
#include <vector>
#include <map>

struct arrow_t {
    ImFont* font{};
    const char* glyph{};
    const char* glyph_up{};
};

class custom_elements_t {
public:
    arrow_t arrow_elem;

    bool toggle(const char* label, bool* v, ImVec4* prov_color = 0);

    bool slider_float(const char* label, float* v, float v_min, float v_max, const char* format = nullptr, ImGuiSliderFlags flags = 0);
    bool slider_int(const char* label, int* v, int v_min, int v_max, const char* format = nullptr, ImGuiSliderFlags flags = 0);

    bool combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
    bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items = -1);
    bool combo(const char* label, int* current_item, bool (*old_getter)(void*, int, const char**), void* user_data, int items_count, int popup_max_height_in_items = -1);
    bool combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items = -1);

    void multi_combo(const char* label, bool* combos[], const char* items[], int items_count);

    bool selectable(const char* label, bool selected, ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = { 0, 0 });
    bool selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = { 0, 0 });

    bool color_edit_4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);

    bool button(const char* label, const ImVec2& size_arg = { 0, 0 });

    void custom_text(const char* v, ImFont* font);

    bool list_box(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int height_in_items = -1);
    bool list_box(const char* label, int* current_item, const char* const items[], int items_count, int height_items = -1);

    bool begin_listbox(const char* label, const ImVec2& size_arg);

    void end_listbox();

    bool toggle(const char *label, bool *v, ImVec4 *prov_color, ImVec4 *prov_color2);
};

inline custom_elements_t* c_widgets = new custom_elements_t();