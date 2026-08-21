#include "widgets.hpp"
#include "internal.hpp"
#include "util.hpp"

#include <cstring>
#include <cstdio>
#include <string>

// Уникальный ImGui-ID для виджета: "label##<адрес данных>". Видимый текст
// остаётся прежним, а контейнеры с одинаковой меткой ("цвет","тип","кость")
// получают разные ID -> нет конфликтов "two items with same ID".
static std::string uid_label(const char* label, const void* store) {
    char b[72];
    snprintf(b, sizeof(b), "%s##%p", label, store);
    return std::string(b);
}

// Чистые виджеты на стандартном ImGui 1.92 (проектный imgui) — API как в
// референсе (custom_elements_t), но без зависимости от внутренностей старого
// imgui (ColorEditOptions/NextWindowData.Flags и пр.), которых в 1.92 нет.

using namespace ImGui;

static void push_dark(float a = 1.0f) {
    PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.12f, a));
    PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.16f, 0.16f, 0.16f, a));
    PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, a));
    PushStyleColor(ImGuiCol_CheckMark, c_utils->get_accent_imv4(1.f));
    PushStyleColor(ImGuiCol_SliderGrab, c_utils->get_accent_imv4(1.f));
    PushStyleColor(ImGuiCol_SliderGrabActive, c_utils->get_accent_imv4(1.f, 0.8f));
    PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.12f, a));
    PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.16f, 0.16f, a));
    PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.20f, 0.20f, a));
    PushStyleColor(ImGuiCol_Header, ImVec4(0.16f, 0.16f, 0.16f, a));
    PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.20f, 0.20f, 0.20f, a));
    PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.24f, 0.24f, 0.24f, a));
    PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.09f, 0.09f, 0.09f, a));
    PushStyleColor(ImGuiCol_Border, ImVec4(0.16f, 0.16f, 0.16f, a));
    PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.f));
}
static void pop_dark() {
    PopStyleColor(15);
}

static void color_swatch(const char* label, ImVec4* c) {
    std::string btn = std::string("##sw_") + label;
    std::string pop = std::string("##swp_") + label;
    ImGui::SameLine(0, c_scale->get(6));
    float sz = ImGui::GetFrameHeight();
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(btn.c_str(), ImVec2(sz, sz));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p, p + ImVec2(sz, sz), ImGui::ColorConvertFloat4ToU32(*c), 3.f);
    dl->AddRect(p, p + ImVec2(sz, sz), IM_COL32(200, 200, 200, 90), 3.f);
    if (ImGui::IsItemClicked(0)) {
        ImGui::OpenPopup(pop.c_str());
    }
    if (ImGui::BeginPopup(pop.c_str())) {
        ImGui::ColorPicker4("##swp", (float*)c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::EndPopup();
    }
}

bool custom_elements_t::toggle(const char* label, bool* v, ImVec4* prov_color) {
    push_dark();
    bool pressed = ImGui::Checkbox(label, v);
    if (prov_color)
        color_swatch(label, prov_color);
    pop_dark();
    return pressed;
}

bool custom_elements_t::toggle(const char* label, bool* v, ImVec4* prov_color, ImVec4* prov_color2) {
    push_dark();
    bool pressed = ImGui::Checkbox(label, v);
    if (prov_color)
        color_swatch(label, prov_color);
    if (prov_color2)
        color_swatch(label, prov_color2);
    pop_dark();
    return pressed;
}

bool custom_elements_t::slider_float(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) {
    push_dark();
    bool r = ImGui::SliderFloat(label, v, v_min, v_max, format ? format : "%.1f", flags);
    pop_dark();
    return r;
}

bool custom_elements_t::slider_int(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags) {
    push_dark();
    bool r = ImGui::SliderInt(label, v, v_min, v_max, format ? format : "%d", flags);
    pop_dark();
    return r;
}

static const char* Items_ArrayGetter(void* data, int idx) {
    const char* const* items = (const char* const*)data;
    return items[idx];
}

bool custom_elements_t::combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items) {
    (void)height_in_items;
    push_dark();
    bool r = ImGui::Combo(uid_label(label, current_item).c_str(), current_item, items, items_count);
    pop_dark();
    return r;
}

bool custom_elements_t::combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items) {
    (void)height_in_items;
    push_dark();
    bool r = ImGui::Combo(uid_label(label, current_item).c_str(), current_item, items_separated_by_zeros);
    pop_dark();
    return r;
}

struct combo_old_wrap_t { bool (*g)(void*, int, const char**); void* ud; };
static const char* combo_old_getter(void* data, int idx) {
    combo_old_wrap_t* w = (combo_old_wrap_t*)data;
    const char* s = nullptr;
    w->g(w->ud, idx, &s);
    return s ? s : "";
}

bool custom_elements_t::combo(const char* label, int* current_item, bool (*old_getter)(void*, int, const char**), void* user_data, int items_count, int popup_max_height_in_items) {
    combo_old_wrap_t w{ old_getter, user_data };
    return combo(label, current_item, combo_old_getter, &w, items_count, popup_max_height_in_items);
}

bool custom_elements_t::combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items) {
    (void)popup_max_height_in_items;
    push_dark();
    bool r = ImGui::BeginCombo(uid_label(label, current_item).c_str(), getter(user_data, *current_item));
    if (r) {
        for (int i = 0; i < items_count; i++) {
            const char* item = getter(user_data, i);
            bool selected = (i == *current_item);
            if (ImGui::Selectable(item, selected)) {
                *current_item = i;
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
        pop_dark();
        return *current_item != -1;
    }
    pop_dark();
    return false;
}

void custom_elements_t::multi_combo(const char* label, bool* combos[], const char* items[], int items_count) {
    push_dark();
    if (ImGui::BeginCombo(label, "Select")) {
        for (int i = 0; i < items_count; i++)
            ImGui::Selectable(items[i], &((*combos)[i]));
        ImGui::EndCombo();
    }
    pop_dark();
}

bool custom_elements_t::selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg) {
    push_dark();
    bool r = ImGui::Selectable(label, selected, flags, size_arg);
    pop_dark();
    return r;
}

bool custom_elements_t::selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg) {
    push_dark();
    bool r = ImGui::Selectable(label, p_selected, flags, size_arg);
    pop_dark();
    return r;
}

bool custom_elements_t::color_edit_4(const char* label, float col[4], ImGuiColorEditFlags flags) {
    push_dark();
    bool r = ImGui::ColorEdit4(label, col, flags);
    pop_dark();
    return r;
}

bool custom_elements_t::button(const char* label, const ImVec2& size_arg) {
    push_dark();
    bool r = ImGui::Button(label, size_arg);
    pop_dark();
    return r;
}

void custom_elements_t::custom_text(const char* v, ImFont* font) {
    PushFont(font);
    ImGui::Text("%s", v);
    PopFont();
}

bool custom_elements_t::list_box(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int height_in_items) {
    (void)height_in_items;
    push_dark();
    bool r = ImGui::ListBox(label, current_item, getter, user_data, items_count);
    pop_dark();
    return r;
}

bool custom_elements_t::list_box(const char* label, int* current_item, const char* const items[], int items_count, int height_items) {
    (void)height_items;
    push_dark();
    bool r = ImGui::ListBox(label, current_item, Items_ArrayGetter, (void*)items, items_count);
    pop_dark();
    return r;
}

bool custom_elements_t::begin_listbox(const char* label, const ImVec2& size_arg) {
    push_dark();
    return ImGui::BeginListBox(label, size_arg);
}

void custom_elements_t::end_listbox() {
    ImGui::EndListBox();
    pop_dark();
}
