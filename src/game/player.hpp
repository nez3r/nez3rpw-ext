#pragma once

#include "game.hpp"
#include "../other/vector3.h"
#include "../other/custom_string.h"
#include "../protect/oxorany.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>

struct BoneMatrix {
    float position[4];
    float rotation[4];
    float scale[4];
};

namespace player {

    struct bones_t {
        Vector3 head;           // 0
        Vector3 neck;           // 1
        Vector3 spine;          // 2
        Vector3 spine1;         // 3
        Vector3 spine2;         // 4
        Vector3 l_shoulder;     // 5
        Vector3 l_arm;          // 6
        Vector3 l_forearm;      // 7
        Vector3 l_hand;         // 8
        Vector3 r_shoulder;     // 9
        Vector3 r_arm;          // 10
        Vector3 r_forearm;      // 11
        Vector3 r_hand;         // 12
        Vector3 pelvis;         // 13
        Vector3 l_thigh;        // 14
        Vector3 l_knee;         // 15
        Vector3 l_foot;         // 16
        Vector3 l_toe;          // 17
        Vector3 r_thigh;        // 18
        Vector3 r_knee;         // 19
        Vector3 r_foot;         // 20
        Vector3 r_toe;          // 21

        Vector3& operator[](int i) {
            return ((Vector3*)this)[i];
        }
    };

    struct bone_cache_entry_t {
        Vector3 offsets[22];
        bool valid = false;
    };

    inline std::unordered_map<uint64_t, bone_cache_entry_t> global_bone_cache;
    inline uint64_t last_player_manager = 0;

    inline Vector3 position(uint64_t p) noexcept {
    uint64_t MovementController = rpm<uint64_t>(p + oxorany(0x98));
    if (!MovementController) return Vector3(0, 0, 0);

    uint64_t TransformData = rpm<uint64_t>(MovementController + oxorany(0xB0));
    if (!TransformData) return Vector3(0, 0, 0);

    return rpm<Vector3>(TransformData + oxorany(0x44));
}

inline Vector3 get_transform_position(uint64_t transform) {
    if (!transform) return {0,0,0};
    uint64_t internal_transform = rpm<uint64_t>(transform + oxorany(0x10));
    if (!internal_transform) return {0,0,0};

    uint32_t index = rpm<uint32_t>(internal_transform + oxorany(0x30));
    uint64_t matrix_data = rpm<uint64_t>(internal_transform + oxorany(0x28));
    if (!matrix_data) return {0,0,0};

    uint64_t matrix_list = rpm<uint64_t>(matrix_data + oxorany(0x18));
    uint64_t matrix_indices = rpm<uint64_t>(matrix_data + oxorany(0x20));
    if (!matrix_list || !matrix_indices) return {0,0,0};

    BoneMatrix tm = rpm<BoneMatrix>(matrix_list + index * sizeof(BoneMatrix));
    Vector3 result = {tm.position[0], tm.position[1], tm.position[2]};
    int p_idx = rpm<int>(matrix_indices + index * sizeof(int));

    int depth = 0;
    while (p_idx >= 0 && depth < 64) {
        BoneMatrix p_tm = rpm<BoneMatrix>(matrix_list + p_idx * sizeof(BoneMatrix));

        float rotX = p_tm.rotation[0];
        float rotY = p_tm.rotation[1];
        float rotZ = p_tm.rotation[2];
        float rotW = p_tm.rotation[3];

        float scaleX = result.x * p_tm.scale[0];
        float scaleY = result.y * p_tm.scale[1];
        float scaleZ = result.z * p_tm.scale[2];

        result.x = p_tm.position[0] + scaleX +
                   (scaleX * ((rotY * rotY * -2.0f) - (rotZ * rotZ * 2.0f))) +
                   (scaleY * ((rotW * rotZ * -2.0f) - (rotY * rotX * -2.0f))) +
                   (scaleZ * ((rotZ * rotX * 2.0f) - (rotW * rotY * -2.0f)));
        result.y = p_tm.position[1] + scaleY +
                   (scaleX * ((rotX * rotY * 2.0f) - (rotW * rotZ * -2.0f))) +
                   (scaleY * ((rotZ * rotZ * -2.0f) - (rotX * rotX * 2.0f))) +
                   (scaleZ * ((rotW * rotX * -2.0f) - (rotZ * rotY * -2.0f)));
        result.z = p_tm.position[2] + scaleZ +
                   (scaleX * ((rotW * rotY * -2.0f) - (rotX * rotZ * -2.0f))) +
                   (scaleY * ((rotY * rotZ * 2.0f) - (rotW * rotX * -2.0f))) +
                   (scaleZ * ((rotX * rotX * -2.0f) - (rotY * rotY * 2.0f)));

        p_idx = rpm<int>(matrix_indices + p_idx * sizeof(int));
        depth++;
    }
    return result;
}

inline Vector3 camera_position(uint64_t p) noexcept {
if (!p) return {0,0,0};
uint64_t cameraHolder = rpm<uint64_t>(p + oxorany(0x28));
if (cameraHolder) {
uint64_t cameraTransform = rpm<uint64_t>(cameraHolder + oxorany(0x10));
if (cameraTransform) {
Vector3 pos = get_transform_position(cameraTransform);
if (pos.x != 0 || pos.y != 0 || pos.z != 0) return pos;
}
}
return position(p);
}

inline void apply_default_pose(bones_t& b, const Vector3& root) {
    b.head = root + Vector3(0, 1.75f, 0);
    b.neck = root + Vector3(0, 1.6f, 0);
    b.spine2 = root + Vector3(0, 1.45f, 0);
    b.spine1 = root + Vector3(0, 1.25f, 0);
    b.spine = root + Vector3(0, 1.05f, 0);
    b.pelvis = root + Vector3(0, 0.9f, 0);
    b.l_shoulder = root + Vector3(-0.2f, 1.55f, 0);
    b.l_arm = root + Vector3(-0.4f, 1.5f, 0);
    b.l_forearm = root + Vector3(-0.4f, 1.2f, 0);
    b.l_hand = root + Vector3(-0.4f, 1.0f, 0);
    b.r_shoulder = root + Vector3(0.2f, 1.55f, 0);
    b.r_arm = root + Vector3(0.4f, 1.5f, 0);
    b.r_forearm = root + Vector3(0.4f, 1.2f, 0);
    b.r_hand = root + Vector3(0.4f, 1.0f, 0);
    b.l_thigh = root + Vector3(-0.15f, 0.9f, 0);
    b.l_knee = root + Vector3(-0.15f, 0.45f, 0);
    b.l_foot = root + Vector3(-0.15f, 0.05f, 0);
    b.l_toe = root + Vector3(-0.15f, 0.0f, 0.1f);
    b.r_thigh = root + Vector3(0.15f, 0.9f, 0);
    b.r_knee = root + Vector3(0.15f, 0.45f, 0);
    b.r_foot = root + Vector3(0.15f, 0.05f, 0);
    b.r_toe = root + Vector3(0.15f, 0.0f, 0.1f);
}

inline bool is_visible(uint64_t p) noexcept {
if (!p) return false;
uint64_t objectOccludee = rpm<uint64_t>(p + oxorany(0xB0));
if (objectOccludee > 0x1000 && objectOccludee < 0x7FFFFFFFFFFF) {
int visibilityState = rpm<int>(objectOccludee + oxorany(0x34));
int occlusionState = rpm<int>(objectOccludee + oxorany(0x38));
return (visibilityState == 2 && occlusionState != 1);
}
uint64_t view = rpm<uint64_t>(p + oxorany(0x48));
if (view > 0x1000 && view < 0x7FFFFFFFFFFF) {
return rpm<bool>(view + oxorany(0x30));
}
return false;
}

inline bool get_bones(uint64_t p, bones_t& b) noexcept {
uint64_t current_pm = get_player_manager();
if (current_pm != last_player_manager) {
global_bone_cache.clear();
last_player_manager = current_pm;
}

Vector3 root = position(p);
if (root.x == 0 && root.y == 0 && root.z == 0) return false;

auto apply_cache_or_default = [&]() {
    auto it = global_bone_cache.find(p);
    if (it != global_bone_cache.end() && it->second.valid) {
        for (int i = 0; i < 22; i++) b[i] = root + it->second.offsets[i];
        return true;
    }
    apply_default_pose(b, root);
    return true;
};

if (!is_visible(p)) {
return apply_cache_or_default();
}

uint64_t view = rpm<uint64_t>(p + oxorany(0x48));
if (!view) return apply_cache_or_default();

uint64_t map = rpm<uint64_t>(view + oxorany(0x48));
if (!map) return apply_cache_or_default();

uint64_t ptrs[22];
if (!mem_read(map + oxorany(0x20), ptrs, sizeof(ptrs))) return apply_cache_or_default();

uint32_t transform_indices[22];
uint64_t matrix_list = 0;
uint64_t matrix_indices = 0;
uint32_t max_index = 0;

for (int i = 0; i < 22; i++) {
transform_indices[i] = 0xFFFFFFFF;
if (ptrs[i]) {
uint64_t internal_transform = rpm<uint64_t>(ptrs[i] + oxorany(0x10));
if (internal_transform) {
uint32_t idx = rpm<uint32_t>(internal_transform + oxorany(0x30));
transform_indices[i] = idx;
if (idx > max_index && idx < 10000) max_index = idx;

if (!matrix_list) {
uint64_t matrix_data = rpm<uint64_t>(internal_transform + oxorany(0x28));
if (matrix_data) {
matrix_list = rpm<uint64_t>(matrix_data + oxorany(0x18));
matrix_indices = rpm<uint64_t>(matrix_data + oxorany(0x20));
}
}
}
}
}

if (!matrix_list || !matrix_indices || max_index == 0) return apply_cache_or_default();

uint32_t count = max_index + 1;
if (count > 10000) return apply_cache_or_default();

std::vector<BoneMatrix> all_matrices(count);
std::vector<int> all_parents(count);

if (!mem_read(matrix_list, all_matrices.data(), count * sizeof(BoneMatrix))) return apply_cache_or_default();
if (!mem_read(matrix_indices, all_parents.data(), count * sizeof(int))) return apply_cache_or_default();

auto get_bone_pos_from_cache = [&](uint32_t index) -> Vector3 {
    if (index >= count) return {0,0,0};

    BoneMatrix tm = all_matrices[index];
    Vector3 result = {tm.position[0], tm.position[1], tm.position[2]};
    int p_idx = all_parents[index];

    int depth = 0;
    while (p_idx >= 0 && p_idx < (int)count && depth < 64) {
        BoneMatrix p_tm = all_matrices[p_idx];

        float rotX = p_tm.rotation[0];
        float rotY = p_tm.rotation[1];
        float rotZ = p_tm.rotation[2];
        float rotW = p_tm.rotation[3];

        float scaleX = result.x * p_tm.scale[0];
        float scaleY = result.y * p_tm.scale[1];
        float scaleZ = result.z * p_tm.scale[2];

        result.x = p_tm.position[0] + scaleX +
                   (scaleX * ((rotY * rotY * -2.0f) - (rotZ * rotZ * 2.0f))) +
                   (scaleY * ((rotW * rotZ * -2.0f) - (rotY * rotX * -2.0f))) +
                   (scaleZ * ((rotZ * rotX * 2.0f) - (rotW * rotY * -2.0f)));
        result.y = p_tm.position[1] + scaleY +
                   (scaleX * ((rotX * rotY * 2.0f) - (rotW * rotZ * -2.0f))) +
                   (scaleY * ((rotZ * rotZ * -2.0f) - (rotX * rotX * 2.0f))) +
                   (scaleZ * ((rotW * rotX * -2.0f) - (rotZ * rotY * -2.0f)));
        result.z = p_tm.position[2] + scaleZ +
                   (scaleX * ((rotW * rotY * -2.0f) - (rotX * rotZ * -2.0f))) +
                   (scaleY * ((rotY * rotZ * 2.0f) - (rotW * rotX * -2.0f))) +
                   (scaleZ * ((rotX * rotX * -2.0f) - (rotY * rotY * 2.0f)));

        p_idx = all_parents[p_idx];
        depth++;
    }
    return result;
};

bool any_valid = false;
for (int i = 0; i < 22; i++) {
if (transform_indices[i] != 0xFFFFFFFF) {
b[i] = get_bone_pos_from_cache(transform_indices[i]);
if (b[i].x != 0 || b[i].y != 0) any_valid = true;
} else {
b[i] = {0, 0, 0};
}
}

if (any_valid) {
float dist_to_root = (b[1] - root).magnitude();
if (dist_to_root > 0.1f && dist_to_root < 3.0f) {
auto& cache = global_bone_cache[p];
for (int i = 0; i < 22; i++) cache.offsets[i] = b[i] - root;
cache.valid = true;
return true;
}
}

return apply_cache_or_default();
}

inline uint64_t photon_ptr(uint64_t p) noexcept {
return rpm<uint64_t>(p + oxorany(0x160));
}

template<typename T>
inline T property(uint64_t p, const char* tag) noexcept {
T result{};
uint64_t PhotonPlayer = photon_ptr(p);
if (!PhotonPlayer) return result;
uint64_t PropertiesRegistry = rpm<uint64_t>(PhotonPlayer + oxorany(0x38));
if (!PropertiesRegistry) return result;
int Count = rpm<int>(PropertiesRegistry + oxorany(0x20));
if (Count <= 0) return result;
uint64_t PropertiesList = rpm<uint64_t>(PropertiesRegistry + oxorany(0x18));
if (!PropertiesList) return result;
for (int i = 0; i < Count; i++) {
uint64_t Key = rpm<uint64_t>(PropertiesList + oxorany(0x28) + oxorany(0x18) * i);
if (!Key) continue;
read_string KeyString = rpm<read_string>(Key);
std::string KeyStr = KeyString.as_utf8();
if (strstr(KeyStr.c_str(), tag)) {
uint64_t Value = rpm<uint64_t>(PropertiesList + oxorany(0x30) + oxorany(0x18) * i);
if (Value) result = rpm<T>(Value + oxorany(0x10));
break;
}
}
return result;
}

inline int health(uint64_t p) noexcept {
return property<int>(p, oxorany("health"));
}

inline read_string name(uint64_t p) noexcept {
uint64_t PhotonPlayer = photon_ptr(p);
if (!PhotonPlayer) return {};
return rpm<read_string>(rpm<uint64_t>(PhotonPlayer + oxorany(0x20)));
}

inline matrix view_matrix(uint64_t p) noexcept {
if (!p) return {};
uint64_t camera = rpm<uint64_t>(p + oxorany(0xE8));
if (!camera) return {};
uint64_t camera_obj = rpm<uint64_t>(camera + oxorany(0x20));
if (!camera_obj) return {};
uint64_t internal_camera = rpm<uint64_t>(camera_obj + oxorany(0x10));
if (!internal_camera) return {};
return rpm<matrix>(internal_camera + oxorany(0xF0));
}

inline int platform(uint64_t p) noexcept {
return property<int>(p, oxorany("pl"));
}

// Silent Aim helper functions
inline int visibility_state(uint64_t p) noexcept {
	if (!p) return 0;
	uint64_t objectOccludee = rpm<uint64_t>(p + oxorany(0xB0));
	if (objectOccludee > 0x1000 && objectOccludee < 0x7FFFFFFFFFFF) {
		int visibilityState = rpm<int>(objectOccludee + oxorany(0x34));
		int occlusionState = rpm<int>(objectOccludee + oxorany(0x38));
		if (visibilityState == 2 && occlusionState != 1) return 2;
		if (visibilityState == 1) return 1;
	}
	uint64_t view = rpm<uint64_t>(p + oxorany(0x48));
	if (view > 0x1000 && view < 0x7FFFFFFFFFFF) {
		return rpm<bool>(view + oxorany(0x30)) ? 2 : 0;
	}
	return 0;
}

inline bool bone_position(uint64_t p, int bone_idx, const Vector3& /*base*/, Vector3& out) noexcept {
	if (!p) return false;
	uint64_t charView = rpm<uint64_t>(p + oxorany(0x48));
	if (!charView) return false;
	uint64_t bipedMap = rpm<uint64_t>(charView + oxorany(0x48));
	if (!bipedMap) return false;

	static const uint64_t bone_offsets[] = {
		0x20, // 0 = Head
		0x28, // 1 = Neck
		0x30, // 2 = Spine
		0x88  // 3 = Pelvis
	};

	if (bone_idx < 0 || bone_idx > 3) bone_idx = 0;
	uint64_t boneTransform = rpm<uint64_t>(bipedMap + bone_offsets[bone_idx]);
	if (!boneTransform) return false;

	out = get_transform_position(boneTransform);
	return (out.x != 0.f || out.y != 0.f || out.z != 0.f);
}

inline bool transform_position(uint64_t transform, Vector3& out) noexcept {
	if (!transform) return false;
	uint64_t internal = rpm<uint64_t>(transform + oxorany(0x10));
	if (!internal) return false;
	out = get_transform_position(internal);
	return true;
}

inline bool sane_world_pos(const Vector3& pos) noexcept {
	return pos.x != 0.f || pos.y != 0.f || pos.z != 0.f;
}

}