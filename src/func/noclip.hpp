#pragma once
#include <unistd.h>
#include <cstdint>

// Глобальные переменные для ImGui меню (defined in ui/menu_vars.hpp)
#include "../ui/menu_vars.hpp"

// Функция запуска ноуклипа в главном цикле
void run_noclip(pid_t pid, uintptr_t lib_il2cpp_base, uintptr_t local_player);
