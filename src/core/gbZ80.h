#ifndef GB_Z80_H
#define GB_Z80_H

#include "gbReg.h"

typedef struct gbZ80 {
    gb_reg16_t regs[GB_REG_COUNT];
} gbZ80_t;

#endif
