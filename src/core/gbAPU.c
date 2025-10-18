// SPDX-License-Identifier: GPL-2.0-only

#include "gbAPU.h"

void gb_apu_init(gb_apu_t* apu) {
    apu->enabled = 0;
}

void gb_apu_clock(gb_apu_t* apu, u16 t_cycles) {
}
