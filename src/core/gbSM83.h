#ifndef _GB_SM83_H
#define _GB_SM83_H

#include "gbReg.h"

#define GB_REG_AF(regs) (regs)[0].value
#define GB_REG_BC(regs) (regs)[1].value
#define GB_REG_DE(regs) (regs)[2].value
#define GB_REG_HL(regs) (regs)[3].value
#define GB_REG_SP(regs) (regs)[4].value
#define GB_REG_PC(regs) (regs)[5].value
#define GB_REG_COUNT 6

#define GB_REG_A(regs) (regs[0].nibble.high)
#define GB_REG_F(regs) (regs[0].nibble.low)
#define GB_REG_B(regs) (regs[1].nibble.high)
#define GB_REG_C(regs) (regs[1].nibble.low)
#define GB_REG_D(regs) (regs[2].nibble.high)
#define GB_REG_E(regs) (regs[2].nibble.low)
#define GB_REG_H(regs) (regs[3].nibble.high)
#define GB_REG_L(regs) (regs[3].nibble.low)


typedef struct gb_sm83 {
    gb_reg16_t regs[GB_REG_COUNT];
} gb_sm83_t;

gb_sm83_t gb_cpu_create(void);

#endif
