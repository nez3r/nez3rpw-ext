#pragma once
#include "imgui.h"

// Акцентный цвет меню. Новое UI использует c_utils->accent (util.hpp).
namespace cfg {

namespace style {
    inline ImVec4 accent = ImVec4(162/255.f, 144/255.f, 225/255.f, 1.f);
}

}
