#include "infammo.hpp"
#include "offsets_privet.hpp"
#include "../game/player.hpp"
#include "../protect/oxorany.hpp"

namespace infammo {

#define IAMMO_WP_AMMUNITION    0x130
#define IAMMO_AMMO_MAG_CAP     0x10
#define IAMMO_AMMO_CAP         0x12
#define IAMMO_AMMO_MAG_CAP_SV  0x14
#define IAMMO_AMMO_CAP_SV      0x20
#define IAMMO_AMMO_VALUE       999

void tick(uint64_t p) {
    if (!opt_inf_ammo) return;
    if (!ok(p) || player::health(p) <= 0) return;
    uint64_t wr = rd64(p + OFF_PLAYER_WEAPONRY);
    if (!ok(wr)) return;
    uint64_t wc = rd64(wr + OFF_WEAPONRY_CURRENT);
    if (!ok(wc)) return;
    uint64_t pr = rd64(wc + OFF_WC_WEAPON_PROPS);
    if (!ok(pr)) return;
    uint64_t am = rd64(pr + IAMMO_WP_AMMUNITION);
    if (!ok(am)) return;

    int av = s_inf_ammo > 0 ? s_inf_ammo : IAMMO_AMMO_VALUE;
    // plain-поля ёмкости (клиентский вид)
    if (readable(am + IAMMO_AMMO_MAG_CAP, 2))
        wr16(am + IAMMO_AMMO_MAG_CAP, (uint16_t)av);
    if (readable(am + IAMMO_AMMO_CAP, 2))
        wr16(am + IAMMO_AMMO_CAP, (uint16_t)av);

    // _SV (SecuredValue): игра читает магазин именно отсюда. Прямой memcpy
    // всего 12 байт ломает контрольную сумму (бан). Пишем ТОЛЬКО value
    // (offset +8), salt/key (0..7) не трогаем -> контрольная сумма цела,
    // магазин реально обновляется.
    if (readable(am + IAMMO_AMMO_MAG_CAP_SV + 8, 4))
        wr32(am + IAMMO_AMMO_MAG_CAP_SV + 8, (uint32_t)av);
    if (readable(am + IAMMO_AMMO_CAP_SV + 8, 4))
        wr32(am + IAMMO_AMMO_CAP_SV + 8, (uint32_t)av);
}

void late(void* p, bool local) {
    if (!local) return;
    tick((uint64_t)p);
}

}
