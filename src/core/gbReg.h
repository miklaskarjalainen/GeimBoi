#ifndef GB_REG_H
#define GB_REG_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

typedef u8 gb_reg8_t;

typedef union gb_reg16 {
    struct {
        u8 low;
        u8 high;
    } nibble;
    u16 value;
} gb_reg16_t;

#define GB_REG_AF 0
#define GB_REG_BC 1
#define GB_REG_DE 2
#define GB_REG_HL 3
#define GB_REG_SP 4
#define GB_REG_PC 5
#define GB_REG_COUNT 6

#define GB_BIT(x) (1 << x)
#define GB_IS_BIT(from, bit) (from & GB_BIT(bit))
#define GB_FLAG_ZERO GB_BIT(7)
#define GB_FLAG_SUBS GB_BIT(6)
#define GB_FLAG_HALF GB_BIT(5)
#define GB_FLAG_CARR GB_BIT(4)

#endif
