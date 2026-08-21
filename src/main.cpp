#include "Android_draw/draw.h"
#include "ui/menu.hpp"
#include "other/memory.hpp"
#include "game/game.hpp"
#include "func/visuals.hpp"
#include "func/aim.hpp"
#include "func/silent_aim.hpp"
#include "func/infammo.hpp"
#include "func/weapon_exploits.hpp"
#include "func/airstrafe.hpp"
#include "func/offsets_privet.hpp"
#include "ui/watermark.hpp"
#include "protect/oxorany.hpp"
#include <cstdio>
#include <thread>
#include <chrono>

// размеры экрана (юзаются во всём проекте: aim, visuals, math)
float g_sw = 0.f, g_sh = 0.f;

static void print_status(const char* status) {
    printf(oxorany("\033[2J\033[H\033[1;38;2;162;144;225m[unnamed cheat]\033[0m \033[1;37m%s\033[0m\n"), status);
}

static void launch_standoff() {
    system(oxorany("am start -n com.standoff/com.standoff.MainActivity"));
}

int main() {
    screen_config();

    int max_size = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    int min_size = (displayInfo.height < displayInfo.width ? displayInfo.height : displayInfo.width);

    g_sw = static_cast<float>(max_size);
    g_sh = static_cast<float>(min_size);

    native_window_screen_x = max_size;
    native_window_screen_y = max_size;

    if (!initGUI_draw(native_window_screen_x, native_window_screen_y, true)) return -1;

    touch::init(max_size, min_size, (uint8_t)displayInfo.orientation);

    print_status(oxorany("Game detect"));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    print_status(oxorany("start cheat...."));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    launch_standoff();

    game::init();

    static float alpha = 0.f;
    static bool prev = false;
    static bool game_started = false;

    while (true) {
        drawBegin();

        bool run = game::valid();

#if defined(__x86_64__)
        bool is_landscape = (displayInfo.orientation == 0 || displayInfo.orientation == 2);
#else
        bool is_landscape = (displayInfo.orientation == 1 || displayInfo.orientation == 3);
#endif

        if (run && !prev) {
            if (!game_started) {
                print_status(oxorany("Game detect"));
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                print_status(oxorany("start cheat...."));
                game_started = true;
            } else {
                print_status(oxorany("Game detect"));
            }
            prev = true;
        } else if (!run && prev) {
            print_status(oxorany("game closed"));
            prev = false;
            game_started = false;
        }

        if (is_landscape) {
            ImGuiIO& io = ImGui::GetIO();
            float dt = io.DeltaTime;
            if (dt <= 0.f || dt > 0.1f) dt = 0.016f;

            float target = run ? 1.f : 0.f;
            float spd = run ? 4.f : 6.f;

            if (alpha < target) {
                alpha += dt * spd;
                if (alpha > target) alpha = target;
            } else if (alpha > target) {
                alpha -= dt * spd;
                if (alpha < target) alpha = target;
            }

            ui::bar::set_game_alpha(alpha);

            // Бар (полоска открытия) — берём из ui_old: тап по пилюле внизу
            // переключает ui::menu::g_open, дальше рендерим новое меню.
            {
                static bool bar_state = true; // true = меню скрыто
                static float bar_lt = 0.f;

                ImGuiIO& io = ImGui::GetIO();
                float sc = g_sw / 1920.f;
                float bw = 350.f * sc;
                float bh = 10.f;

                ImRect r(
                    ImVec2((g_sw * 0.5f) - (bw * 0.5f), g_sh - bh - 10.f),
                    ImVec2((g_sw * 0.5f) + (bw * 0.5f), g_sh - 10.f)
                );

                ImVec2 wp(r.Min.x - 30.f * sc, r.Min.y - 50.f * sc);
                ImVec2 ws(bw + 60.f * sc, bh + 100.f * sc);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                ImGui::SetNextWindowPos(wp);
                ImGui::SetNextWindowSize(ws);
                ImGui::Begin("##openbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
                {
                    float ct = ImGui::GetTime();
                    if (ImGui::InvisibleButton("##openbtn", ws)) {
                        if (ct - bar_lt >= 0.15f) { bar_state = !bar_state; bar_lt = ct; }
                    }
                }
                ImGui::End();
                ImGui::PopStyleVar(2);

                ui::menu::g_open = !bar_state;
                ui::bar::g_open = ui::menu::g_open;

                ImDrawList* dl = ImGui::GetForegroundDrawList();
                int ba = static_cast<int>(120 * alpha);
                if (ba > 0) {
                    dl->AddRectFilled(r.Min, r.Max, IM_COL32(255, 255, 255, ba), 20.f);
                }
            }

            // Само меню (новое UI) — только когда открыто
            if (ui::menu::g_open) {
                ui::menu::render_menu();
            }

            if (run && proc::lib != 0) {
                uintptr_t local_player_manager = get_player_manager();
                game::check_lib(local_player_manager);

                visuals::draw();
                aim::run();
                aim::draw_fov(ImGui::GetBackgroundDrawList(), g_sw, g_sh);
                silent_aim::run();
                silent_aim::draw_fov(ImGui::GetBackgroundDrawList(), g_sw, g_sh);

                // Локальный игрок для фич Misc (infammo / rapid_fire / wallshot / airstrafe)
                uint64_t local_player = rpm<uint64_t>(local_player_manager + 0x70);
                if (ok(local_player)) {
                    infammo::tick(local_player);
                    wepx::tick(local_player);
                    airstrafe::tick(local_player);
                }
            }

            // Ватермарк рисуем отдельно, всегда (независимо от открытости меню)
            watermark::render();
}

        bool vis = ui::menu::g_open;
        drawEnd();
        usleep(vis ? 1500 : 4000);
    }

    shutdown();
    return 0;
}
