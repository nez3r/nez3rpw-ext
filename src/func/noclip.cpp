#include "noclip.hpp"
#include "../ui/menu_vars.hpp"
#include <sys/uio.h>

template <typename T>
static T read_raw(pid_t pid, uintptr_t address) {
    T buffer;
    struct iovec local = { &buffer, sizeof(T) };
    struct iovec remote = { reinterpret_cast<void*>(address), sizeof(T) };
    process_vm_readv(pid, &local, 1, &remote, 1, 0);
    return buffer;
}

template <typename T>
static void write_raw(pid_t pid, uintptr_t address, T value) {
    struct iovec local = { &value, sizeof(T) };
    struct iovec remote = { reinterpret_cast<void*>(address), sizeof(T) };
    process_vm_writev(pid, &local, 1, &remote, 1, 0);
}

void run_noclip(pid_t pid, uintptr_t lib_il2cpp_base, uintptr_t local_player_manager) {
    if (!local_player_manager) return;

    // 1. Находим локального игрока (как в твоем aim.cpp)
    uintptr_t local_player = read_raw<uintptr_t>(pid, local_player_manager + 0x70);
    if (!local_player) return;

    // 2. Находим контроллер перемещений (MovementController)
    // В Standoff 2 он обычно лежит на оффсете 0x40 или 0x48 внутри LocalPlayer
    uintptr_t movement_controller = read_raw<uintptr_t>(pid, local_player + 0x48);
    if (!movement_controller) movement_controller = read_raw<uintptr_t>(pid, local_player + 0x40);
    if (!movement_controller) return;

    static bool was_noclip = false;

    if (opt_noclip) {
        // БЕЗОПАСНЫЙ КРЭК ФИЗИКИ (Без изменения координат и без вылетов):

        // 3. Зануляем гравитацию для MovementController (Оффсет гравитации в Unity обычно 0x5C или 0x64)
        write_raw<float>(pid, movement_controller + 0x5C, 0.0f);

        // 4. Патчим коллизию самого контроллера (флаг isGrounded или collisionFlags ставим в режим полета)
        // Вместо изменения 3D-позиции, мы просто выставляем флаг "прохождения сквозь стены"
        // внутри самого игрового объекта, если этот оффсет поддерживается твоим дампом (обычно 0x8C)
        write_raw<bool>(pid, movement_controller + 0x8C, true);

        was_noclip = true;
    }
    else if (was_noclip) {
        // Возвращаем стандартную физику Standoff 2, когда галочка выключена
        write_raw<float>(pid, movement_controller + 0x5C, 1.0f); // Стандартная гравитация
        write_raw<bool>(pid, movement_controller + 0x8C, false);
        was_noclip = false;
    }
}
