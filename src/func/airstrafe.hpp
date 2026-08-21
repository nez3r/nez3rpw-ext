#pragma once

#include "offsets_privet.hpp"

namespace airstrafe {
    void tick(uint64_t p);
    void late(void* p, bool local);
}