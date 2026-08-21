#include "silent_aim.hpp"
#include "../game/game.hpp"
#include "../game/math.hpp"
#include "../game/player.hpp"
#include "../ui/menu_vars.hpp"
#include "../other/memory.hpp"
#include "../protect/oxorany.hpp"
#include "imgui.h"

#include <cmath>
#include <algorithm>

namespace {

    static constexpr float kPi = 3.14159265f;
    static constexpr float kVerticalFovDeg = 70.f;
    static constexpr uint64_t kOffPlayerManagerLocalPlayer = 0x70;
    static constexpr uint64_t kOffPlayerMainCameraHolder = 0x28;
    static constexpr uint64_t kOffPlayerWeaponryController = 0x88;
    static constexpr uint64_t kOffPlayerAimController = 0x80;
    static constexpr uint64_t kOffAimControllerAimingData = 0x90;
    static constexpr uint64_t kOffAimingDataPitch = 0x18;
    static constexpr uint64_t kOffAimingDataYaw = 0x28;
    static constexpr uint64_t kOffWeaponryCurrentWeapon = 0xA0;
    static constexpr uint64_t kOffWeaponViewMode = 0x44;
    static constexpr uint64_t kOffWeaponLocalTime = 0x80;
    static constexpr uint64_t kOffWeaponLodGroup = 0x88;
    static constexpr uint64_t kOffLodLayerIndex = 0x68;
    static constexpr uint64_t kOffWeaponRecoilControl = 0x160;
    static constexpr uint64_t kOffWeaponGunParameters = 0x168;
    static constexpr uint64_t kOffWeaponAccuracyData = 0x228;
    static constexpr uint64_t kOffWeaponRecoilMult = 0x240;
    static constexpr uint64_t kOffRecoilLastShotTime = 0x10;
    static constexpr uint64_t kOffRecoilPrevShotProgress = 0x14;
    static constexpr uint64_t kOffRecoilOldAimPunch = 0x1C;
    static constexpr uint64_t kOffRecoilAimPunch = 0x24;
    static constexpr uint64_t kOffRecoilApproachDelta = 0x2C;
    static constexpr uint64_t kOffRecoilLocalTime = 0x38;
    static constexpr uint64_t kOffRecoilProgress = 0x68;
    static constexpr uint64_t kOffAccuracyAngle = 0x10;
    static constexpr uint64_t kOffRecoilAngle = 0x14;

    static bool likely_ptr(uint64_t p) {
        return p > 0x10000ull && p < 0x0000FFFFFFFFFFFFull;
    }

    static bool holding_knife_or_grenade() {
        uint64_t pm = get_player_manager();
        if (!likely_ptr(pm)) return false;
        uint64_t lp = rpm<uint64_t>(pm + oxorany(0x70ULL));
        if (!likely_ptr(lp)) return false;
        uint64_t wc = rpm<uint64_t>(lp + oxorany(0x88ULL));
        if (!likely_ptr(wc)) return false;
        uint64_t weapon = rpm<uint64_t>(wc + oxorany(0xA0ULL));
        if (!likely_ptr(weapon)) return false;
        uint64_t params = rpm<uint64_t>(weapon + oxorany(0xA8ULL));
        if (!likely_ptr(params)) return false;
        int wid = rpm<int>(params + oxorany(0x18ULL));
        return (wid == 70) || (wid >= 7 && wid <= 9);
    }

    static float rad2deg(float x) {
        return x * 180.f / kPi;
    }

    static float fov_radius_px(float fov_deg, float screen_h) {
        if (screen_h <= 0.f) return 0.f;

        float clamped_fov = std::clamp(fov_deg, 1.f, 179.f);
        float half_vertical = (kVerticalFovDeg * 0.5f) * (kPi / 180.f);
        float half_oim = (clamped_fov * 0.5f) * (kPi / 180.f);

        float tan_vertical = std::tan(half_vertical);
        if (std::fabs(tan_vertical) < 0.00001f) return 0.f;

        return ((screen_h * 0.5f) / tan_vertical) * std::tan(half_oim);
    }

    static bool pass_visible_check(uint64_t player_ptr, float distance) {
        if (!opt_silent_visible) return true;
        int state = player::visibility_state(player_ptr);
        if (state == 2) return true;
        if (state == 1) return false;
        return distance < 10.f;
    }

    static int silent_bone_mode(int target_bone) {
        if (target_bone == 1) return 2;
        if (target_bone == 2) return 3;
        return 0;
    }

    static Vector3 target_point(uint64_t p, const Vector3& base) {
        Vector3 bone{};
        if (player::bone_position(p, silent_bone_mode(s_silent_target_bone), base, bone))
            return bone;
        return Vector3(base.x, base.y + 1.4f, base.z);
    }

    static void zero_safe_float(uint64_t base, uint64_t off) {
        uint64_t safe_off = base + off;
        wpm<uint8_t>(safe_off, 1);
        wpm<int>(safe_off + 4, 0);
        wpm<int>(safe_off + 8, 0);
    }

    static void zero_accuracy_and_spread(uint64_t weapon) {
        uint64_t accuracy = rpm<uint64_t>(weapon + oxorany(kOffWeaponAccuracyData));
        if (likely_ptr(accuracy)) {
            wpm<float>(accuracy + oxorany(kOffAccuracyAngle), 0.f);
            wpm<float>(accuracy + oxorany(kOffRecoilAngle), 0.f);
        }

        uint64_t gp = rpm<uint64_t>(weapon + oxorany(kOffWeaponGunParameters));
        if (!likely_ptr(gp)) return;

        wpm<float>(gp + oxorany(0x168), 0.f);
        wpm<float>(gp + oxorany(0x17C), 0.f);
        wpm<float>(gp + oxorany(0x184), 0.f);

        static const uint64_t safe_offsets[] = {
            0x1EC,
            0x204,
            0x21C,
            0x234,
            0x24C,
        };
        for (uint64_t off : safe_offsets) {
            zero_safe_float(gp, off);
        }
    }

    static void restore_punch(uint64_t weapon) {
        if (!likely_ptr(weapon)) return;
        uint64_t recoil = rpm<uint64_t>(weapon + oxorany(kOffWeaponRecoilControl));
        if (!likely_ptr(recoil)) return;

        float zero[2] = { 0.f, 0.f };
        mem_write(recoil + oxorany(kOffRecoilOldAimPunch), zero, sizeof(zero));
        mem_write(recoil + oxorany(kOffRecoilAimPunch), zero, sizeof(zero));
    }

    static void set_weapon_hidden(uint64_t weapon, bool hidden) {
        if (!likely_ptr(weapon)) return;

        wpm<int>(weapon + oxorany(kOffWeaponViewMode), hidden ? 2 : 1);

        if (hidden) {
            uint64_t lod = rpm<uint64_t>(weapon + oxorany(kOffWeaponLodGroup));
            if (likely_ptr(lod)) wpm<int>(lod + oxorany(kOffLodLayerIndex), 1);
        }
    }

    static void restore_weapon(uint64_t weapon) {
        set_weapon_hidden(weapon, false);
        restore_punch(weapon);
    }

    static bool local_eye(uint64_t lp, const Vector3& fallback, Vector3& out) {
        uint64_t holder = rpm<uint64_t>(lp + oxorany(kOffPlayerMainCameraHolder));
        if (likely_ptr(holder)) {
            Vector3 cam{};
            if (player::transform_position(holder, cam) && player::sane_world_pos(cam)) {
                out = cam;
                return true;
            }
        }
        out = fallback;
        return false;
    }

    static uint64_t s_weapon = 0;
    static bool was_targeting = false;
}

void silent_aim::run() {
    if (!opt_silent_aim) {
        if (was_targeting) {
            restore_weapon(s_weapon);
            was_targeting = false;
            s_weapon = 0;
        }
        return;
    }

    uint64_t pm = get_player_manager();
    if (!likely_ptr(pm)) return;

    uint64_t lp = rpm<uint64_t>(pm + oxorany(kOffPlayerManagerLocalPlayer));
    if (!likely_ptr(lp)) return;

    uint64_t wc = rpm<uint64_t>(lp + oxorany(kOffPlayerWeaponryController));
    if (!likely_ptr(wc)) return;

    uint64_t weapon = rpm<uint64_t>(wc + oxorany(kOffWeaponryCurrentWeapon));
    if (!likely_ptr(weapon)) return;

    if (holding_knife_or_grenade()) {
        if (was_targeting) {
            restore_weapon(s_weapon);
            was_targeting = false;
            s_weapon = 0;
        }
        return;
    }

    matrix vm = player::view_matrix(lp);
    Vector3 local_pos = player::position(lp);
    int local_team = rpm<uint8_t>(lp + oxorany(0x79));

    uint64_t pl = rpm<uint64_t>(pm + oxorany(0x28));
    if (!likely_ptr(pl)) return;
    int pc = rpm<int>(pl + oxorany(0x20));
    if (pc <= 0 || pc > 64) return;
    uint64_t lb = rpm<uint64_t>(pl + oxorany(0x18));
    if (!likely_ptr(lb)) return;

    const ImVec2 center(g_sw * 0.5f, g_sh * 0.5f);
    const float radius = fov_radius_px(s_silent_fov, g_sh);
    float best_dist = radius + 1.f;
    Vector3 best_target(0.f, 0.f, 0.f);
    bool found = false;

    for (int i = 0; i < pc; i++) {
        uint64_t p = rpm<uint64_t>(lb + oxorany(0x30) + oxorany(0x18) * i);
        if (!likely_ptr(p) || p == lp) continue;

        uint8_t team = rpm<uint8_t>(p + oxorany(0x79));
        if (team == 0 || team == static_cast<uint8_t>(local_team)) continue;

        int health = player::health(p);
        if (health <= 0) continue;

        Vector3 pos = player::position(p);
        if (pos.x == 0.f && pos.y == 0.f && pos.z == 0.f) continue;

        float distance = calculate_distance(pos, local_pos);
        if (distance > s_esp_dist) continue;
        if (!pass_visible_check(p, distance)) continue;

        Vector3 tp = target_point(p, pos);
        ImVec2 sp;
        if (!world_to_screen(tp, vm, sp)) continue;

        float dx = sp.x - center.x;
        float dy = sp.y - center.y;
        float d = std::sqrt(dx * dx + dy * dy);
        if (d <= radius && d < best_dist) {
            best_dist = d;
            best_target = tp;
            found = true;
        }
    }

    if (!found) {
        if (was_targeting) {
            restore_weapon(s_weapon);
            was_targeting = false;
            s_weapon = 0;
        }
        return;
    }

    uint64_t recoil = rpm<uint64_t>(weapon + oxorany(kOffWeaponRecoilControl));
    if (!likely_ptr(recoil)) return;

    uint64_t aim_controller = rpm<uint64_t>(lp + oxorany(kOffPlayerAimController));
    uint64_t aiming_data = likely_ptr(aim_controller)
                               ? rpm<uint64_t>(aim_controller + oxorany(kOffAimControllerAimingData))
                               : 0;

    float cur_pitch = 0.f;
    float cur_yaw = 0.f;
    if (likely_ptr(aiming_data)) {
        cur_pitch = rpm<float>(aiming_data + oxorany(kOffAimingDataPitch));
        cur_yaw = rpm<float>(aiming_data + oxorany(kOffAimingDataYaw));
    }

    Vector3 eye;
    local_eye(lp, Vector3(local_pos.x, local_pos.y + 1.67f, local_pos.z), eye);
    Vector3 direction(eye.x - best_target.x, eye.y - best_target.y, eye.z - best_target.z);

    float dist = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (dist < 0.001f) return;

    float mult = rpm<float>(weapon + oxorany(kOffWeaponRecoilMult));
    if (!(mult >= 0.05f && mult <= 100.f)) mult = 1.f;
    mult *= std::clamp(s_silent_recoil_mult, 0.05f, 10.f);

    float dp = rad2deg(std::atan2(direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z))) - cur_pitch;
    float dy = rad2deg(-std::atan2(direction.x, -direction.z)) - std::fmod(cur_yaw, 360.f);
    dy = std::fmod(dy + 180.f, 360.f);
    if (dy < 0.f) dy += 360.f;
    dy -= 180.f;

    float punch[2] = { -dp / mult, dy / mult };

    set_weapon_hidden(weapon, true);

    float time = rpm<float>(weapon + oxorany(kOffWeaponLocalTime));
    if (!std::isfinite(time) || time < 0.f) time = 0.f;

    wpm<float>(recoil + oxorany(kOffRecoilLastShotTime), time);
    wpm<float>(recoil + oxorany(kOffRecoilPrevShotProgress), 0.f);
    wpm<float>(recoil + oxorany(kOffRecoilApproachDelta), 0.f);
    wpm<float>(recoil + oxorany(kOffRecoilLocalTime), time);
    wpm<float>(recoil + oxorany(kOffRecoilProgress), 0.f);

    mem_write(recoil + oxorany(kOffRecoilOldAimPunch), punch, sizeof(punch));
    mem_write(recoil + oxorany(kOffRecoilAimPunch), punch, sizeof(punch));

    zero_accuracy_and_spread(weapon);

    s_weapon = weapon;
    was_targeting = true;
}

void silent_aim::draw_fov(ImDrawList* draw, float screen_w, float screen_h) {
    if (!draw || !opt_silent_aim || !opt_silent_fov_draw || screen_w <= 0.f || screen_h <= 0.f) return;

    float cx = screen_w * 0.5f;
    float cy = screen_h * 0.5f;
    float radius = fov_radius_px(s_silent_fov, screen_h);

    draw->AddCircle(ImVec2(cx, cy), radius, IM_COL32(0, 0, 0, 130), 64, 2.f);
    draw->AddCircle(ImVec2(cx, cy), radius, IM_COL32(255, 220, 0, 220), 64, 1.f);
}