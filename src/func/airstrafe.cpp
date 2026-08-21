#include "airstrafe.hpp"
#include "offsets_privet.hpp"
#include "../game/player.hpp"
#include "../protect/oxorany.hpp"

namespace airstrafe {

#define ASTR_MC_TRANSLATION_DATA  0xB0
#define ASTR_MC_TRANSLATION_PARS  0xA8
#define ASTR_PARS_JUMP_PARAMS     0x50
#define ASTR_TD_MOVE_VECTOR       0x68
#define ASTR_JP_JUMP_MOVE_SPEED   0x60

inline bool active = false;
inline float orig_jump_speed = 0.f;

static void restore(uint64_t p) {
    if (!active) return;
    if (!ok(p)) { active = false; return; }
    uint64_t mc = rd64(p + OFF_PLAYER_MOVEMENT_CTRL);
    if (!ok(mc)) { active = false; return; }
    uint64_t pars = rd64(mc + ASTR_MC_TRANSLATION_PARS);
    if (!ok(pars)) { active = false; return; }
    uint64_t jp = rd64(pars + ASTR_PARS_JUMP_PARAMS);
    if (!ok(jp)) { active = false; return; }
    wrf(jp + ASTR_JP_JUMP_MOVE_SPEED, orig_jump_speed);
    active = false;
}

void tick(uint64_t p) {
    if (!opt_air_strafe) { restore(p); return; }
    if (!ok(p)) return;
    uint64_t mc = rd64(p + OFF_PLAYER_MOVEMENT_CTRL);
    if (!ok(mc)) return;
    uint64_t data = rd64(mc + ASTR_MC_TRANSLATION_DATA);
    if (!ok(data)) return;
    uint64_t pars = rd64(mc + ASTR_MC_TRANSLATION_PARS);
    if (!ok(pars)) return;
    uint64_t jp = rd64(pars + ASTR_PARS_JUMP_PARAMS);
    if (!ok(jp)) return;

    if (!active) {
        orig_jump_speed = rdf(jp + ASTR_JP_JUMP_MOVE_SPEED);
        active = true;
    }

    if (readable(data + ASTR_TD_MOVE_VECTOR, sizeof(Vector3)))
        *(Vector3*)(data + ASTR_TD_MOVE_VECTOR) = Vector3(0.f, 0.f, 0.f);

    wrf(jp + ASTR_JP_JUMP_MOVE_SPEED, s_air_strafe_speed);
}

void late(void* p, bool local) {
    if (!local) return;
    tick((uint64_t)p);
}

}
