#include "aim.hpp"
#include "../game/game.hpp"
#include "../game/math.hpp"
#include "../game/player.hpp"
#include "../ui/menu_vars.hpp"
#include "../protect/oxorany.hpp"
#include "imgui.h"
#include <cmath>
#include <algorithm>
#include <chrono>

// cfg::aim::* (enabled/fov/smooth/target/visible_check/lock_line/lock_dot/aspect_*)
// объявлены как референсы на переменные меню в menu_vars.hpp
using namespace cfg::aim;

namespace {
    static constexpr float kPi = 3.14159265f;
    static constexpr float kVerticalFovDeg = 70.f;
    static constexpr uint64_t kOffPlayerManagerLocalPlayer = 0x70;
    static constexpr uint64_t kOffPlayerAimController = 0x80;
    static constexpr uint64_t kOffAimControllerAimingData = 0x90;
    static constexpr uint64_t kOffAimingDataPitch = 0x18;
    static constexpr uint64_t kOffAimingDataYaw = 0x1C;
    static constexpr uint64_t kOffLocalPlayerMainCamera = 0xE8;
    static constexpr uint64_t kOffPlayerMainCameraCamera = 0x20;
    static constexpr uint64_t kOffPlayerMainCameraMainCamera = 0x40;
    static constexpr uint64_t kOffCameraMovementControllerCamera = 0xA0;
    static constexpr uint64_t kOffCameraNativeCamera = 0x10;
    static constexpr uint64_t kOffNativeCameraAspectRatio = 0x4F0;
    static constexpr uint64_t kOffNativeCameraFov = 0x180;

    static auto g_lastAimWrite = std::chrono::steady_clock::now();
    static constexpr int AIM_WRITE_INTERVAL_MS = 16;

    static const uint64_t g_boneOffsets[] = {
            0x20, // 0 = Head
            0x28, // 1 = Neck
            0x30, // 2 = Chest (Spine)
            0x88  // 3 = Pelvis
    };

    static bool likely_ptr(uint64_t p) {
    return p > 0x10000ull && p < 0x0000FFFFFFFFFFFFull;
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

static float normalize_yaw(float yaw) {
    while (yaw > 180.f) yaw -= 360.f;
    while (yaw < -180.f) yaw += 360.f;
    return yaw;
}

static float smooth_angle(float current, float target, float smooth) {
    float diff = target - current;
    while (diff > 180.f) diff -= 360.f;
    while (diff < -180.f) diff += 360.f;
    return current + diff * smooth;
}

static Vector3 get_bone_position(uint64_t player_ptr, int bone_idx) {
uint64_t charView = rpm<uint64_t>(player_ptr + oxorany(0x48));
if (!likely_ptr(charView)) return Vector3(0, 0, 0);

uint64_t bipedMap = rpm<uint64_t>(charView + oxorany(0x48));
if (!likely_ptr(bipedMap)) return Vector3(0, 0, 0);

int clamped_idx = std::clamp(bone_idx, 0, 3);
uint64_t boneOffset = g_boneOffsets[clamped_idx];

uint64_t boneTransform = rpm<uint64_t>(bipedMap + boneOffset);
if (!likely_ptr(boneTransform)) return Vector3(0, 0, 0);

return player::get_transform_position(boneTransform);
}

static bool pass_visible_check(uint64_t player_ptr) {
if (!visible_check) return true;
if (player::health(player_ptr) <= 0) return false;

uint64_t occ = rpm<uint64_t>(player_ptr + oxorany(0xB8));
if (likely_ptr(occ)) {
if (rpm<int>(occ + oxorany(0x34)) != 2 || rpm<int>(occ + oxorany(0x38)) == 1) return false;
}
return true;
}
}

void aim::run() {
    if (!enabled) return;

    uint64_t pm = get_player_manager();
    if (!pm) return;

    uint64_t lp = rpm<uint64_t>(pm + oxorany(kOffPlayerManagerLocalPlayer));
    if (!lp) return;

    matrix vm = player::view_matrix(lp);
    Vector3 cam_pos = player::position(lp);
    uint64_t ac = rpm<uint64_t>(lp + oxorany(kOffPlayerAimController));
    if (!ac) ac = rpm<uint64_t>(lp + 0x60);
    if (!ac) return;

    uint64_t camTr = rpm<uint64_t>(ac + oxorany(0x80));
    if (camTr) {
        Vector3 cp = player::get_transform_position(camTr);
        if (cp.x != 0.f || cp.y != 0.f || cp.z != 0.f) cam_pos = cp;
    }

    int lp_team = rpm<uint8_t>(lp + oxorany(0x79));

    uint64_t plist = rpm<uint64_t>(pm + oxorany(0x28));
    if (!plist) return;

    int pcount = rpm<int>(plist + oxorany(0x20));
    if (pcount <= 0 || pcount > 128) return;

    uint64_t buf = rpm<uint64_t>(plist + oxorany(0x18));
    if (!buf) return;

    ImVec2 center(g_sw * 0.5f, g_sh * 0.5f);
    float radius = fov_radius_px(fov, g_sh);
    float best_fov = radius;
    uint64_t best_target = 0;
    Vector3 best_bone_pos(0, 0, 0);
    ImVec2 best_screen(0, 0);

    int targetBone = std::clamp(target, 0, 3);

    for (int i = 0; i < pcount; i++) {
        uint64_t p = rpm<uint64_t>(buf + oxorany(0x30) + oxorany(0x18) * i);
        if (!p || p == lp) continue;
        if (rpm<uint8_t>(p + oxorany(0x79)) == lp_team) continue;
        if (player::health(p) <= 0) continue;

        if (!pass_visible_check(p)) continue;

        Vector3 target_pos = get_bone_position(p, targetBone);
        if (target_pos.x == 0.f && target_pos.y == 0.f && target_pos.z == 0.f) continue;
        if (calculate_distance(target_pos, cam_pos) > max_distance) continue;

        ImVec2 sp;
        if (world_to_screen(target_pos, vm, sp)) {
            float dx = sp.x - center.x;
            float dy = sp.y - center.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < best_fov) {
                best_fov = dist;
                best_target = p;
                best_bone_pos = target_pos;
                best_screen = sp;
            }
        }
    }

    if (best_target) {
        if (lock_line) {
            ImDrawList* dl = ImGui::GetBackgroundDrawList();
            if (dl) {
                dl->AddLine(center, best_screen, IM_COL32(0, 0, 0, 180), 2.f);
                dl->AddLine(center, best_screen, IM_COL32(255, 255, 255, 220), 1.f);
            }
        }

        if (lock_dot) {
            ImDrawList* dl = ImGui::GetBackgroundDrawList();
            if (dl) {
                dl->AddCircleFilled(best_screen, 3.5f, IM_COL32(255, 255, 255, 230), 12);
                dl->AddCircle(best_screen, 5.5f, IM_COL32(0, 0, 0, 170), 14, 1.f);
            }
        }

        uint64_t ad = rpm<uint64_t>(ac + oxorany(kOffAimControllerAimingData));
        if (!ad) return;

        float currentPitch = rpm<float>(ad + oxorany(kOffAimingDataPitch));
        float currentYaw = rpm<float>(ad + oxorany(kOffAimingDataYaw));
        currentYaw = normalize_yaw(currentYaw);

        Vector3 dir = best_bone_pos - cam_pos;
        float dist = dir.magnitude();
        if (dist < 0.1f) return;

        float rad2deg = 57.2957795f;
        float targetPitch = -std::asin(std::clamp(dir.y / dist, -1.f, 1.f)) * rad2deg;
        float targetYaw = std::atan2(dir.x, dir.z) * rad2deg;
        targetYaw = normalize_yaw(targetYaw);

        float pitch = 0.f;
        float yaw = 0.f;

        // ПРАВИЛЬНЫЙ РАБОЧИЙ СМУЧ ДЛЯ EXTERNAL ОВЕРЛЕЯ
        float smooth_val = smooth;

        // Защита: если ползунок выкручен на самый минимум (меньше или равен 1) — включается Fast Lock
        if (smooth_val <= 1.05f) {
            pitch = targetPitch;
            yaw = targetYaw;
        } else {
            // Классический пошаговый коэффициент интерполяции.
            // При значении 2.0f наводка будет резкой (1/2 угла за кадр).
            // При значении 50.0f наводка будет максимально плавной и легитной (1/50 угла за кадр).
            float interpolation_step = 1.0f / smooth_val;

            pitch = currentPitch + (targetPitch - currentPitch) * interpolation_step;
            yaw = smooth_angle(currentYaw, targetYaw, interpolation_step);
        }

        float newPitch = std::clamp(pitch, -89.0f, 89.0f);
        float newYaw = yaw;

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastAimWrite).count() >= AIM_WRITE_INTERVAL_MS) {
            g_lastAimWrite = now;
            wpm<float>(ad + oxorany(kOffAimingDataPitch), newPitch);
            wpm<float>(ad + oxorany(kOffAimingDataYaw), newYaw);
            wpm<float>(ad + oxorany(0x24), newPitch);
            wpm<float>(ad + oxorany(0x28), newYaw);
        }
    }
}


void aim::draw_fov(ImDrawList* draw, float screen_w, float screen_h) {
    if (!draw || !enabled || screen_w <= 0.f || screen_h <= 0.f) return;

    float cx = screen_w * 0.5f;
    float cy = screen_h * 0.5f;
    float radius = fov_radius_px(fov, screen_h);

    draw->AddCircle(ImVec2(cx, cy), radius, IM_COL32(0, 0, 0, 130), 64, 2.f);
    draw->AddCircle(ImVec2(cx, cy), radius, IM_COL32(255, 255, 255, 220), 64, 1.f);
}
