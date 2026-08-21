#pragma once

#include "../other/memory.hpp"
#include "../ui/menu_vars.hpp"
#include "../protect/oxorany.hpp"

// Оффсеты Standoff 2 (из декомпиляции/privet), завёрнуты в oxorany
#define OFF_PM_LOCAL_PLAYER         0x70
#define OFF_PLAYER_TEAM             0x79
#define OFF_PLAYER_MOVEMENT_CTRL    0x98
#define OFF_PLAYER_WEAPONRY         0x88
#define OFF_WEAPONRY_CURRENT        0xA0
#define OFF_WC_WEAPON_PROPS         0xA8

// Проверка валидности указателя (как ok() в privet)
static inline bool ok(uint64_t p) {
    return p > 0x10000ull && p < 0x0000FFFFFFFFFFFFull;
}

// Чтение/запись через наши rpm/wpm
template<typename T>
static inline T rd(uint64_t a) { return rpm<T>(a); }
template<typename T>
static inline void wr(uint64_t a, const T& v) { wpm<T>(a, v); }

static inline uint64_t rd64(uint64_t a) { return rpm<uint64_t>(a); }
static inline uint32_t rd32(uint64_t a) { return rpm<uint32_t>(a); }
static inline uint16_t rd16(uint64_t a) { return rpm<uint16_t>(a); }
static inline float rdf(uint64_t a) { return rpm<float>(a); }

static inline void wr64(uint64_t a, uint64_t v) { wpm<uint64_t>(a, v); }
static inline void wr32(uint64_t a, uint32_t v) { wpm<uint32_t>(a, v); }
static inline void wr16(uint64_t a, uint16_t v) { wpm<uint16_t>(a, v); }
static inline void wrf(uint64_t a, float v) { wpm<float>(a, v); }

// Проверка читаемости (аналог readable() из privet)
static inline bool readable(uint64_t a, size_t len) {
    if (!ok(a)) return false;
    uint8_t tmp[16] = {0};
    if (len > 16) len = 16;
    return mem_read(a, tmp, len);
}
