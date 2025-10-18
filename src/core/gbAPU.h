/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _GB_APU_H
#define _GB_APU_H

#include "gbReg.h"

typedef struct gb_apu {
	u8 enabled : 1;

	struct {
		u8 active : 1;
	} channel1;
	struct {
		u8 active : 1;
	} channel2;
	struct {
		u8 active : 1;
	} channel3;
	struct {
		u8 active : 1;
	} channel4;

} gb_apu_t;

void gb_apu_init(gb_apu_t* apu);
void gb_apu_clock(gb_apu_t* apu, u16 t_cycles);

#endif
