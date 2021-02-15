/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx150x
 * @{
 *
 * @file
 * @brief       Register definitions for SX150X devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SX150X_REGS_H
#define SX150X_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Value passed into REG_ macros that take a pin when used with the 16-bit write function */
#define BOTH                    15
#define OFFSET(pin)             ((pin > 7) ? 0 : 1)

#define REG_INPUT_DISABLE(pin)  (0x00 + OFFSET(pin))
#define REG_PULL_UP(pin)        (0x06 + OFFSET(pin))
#define REG_DIR(pin)            (0x0e + OFFSET(pin))
#define REG_DATA(pin)           (0x10 + OFFSET(pin))
#define REG_OPENDRAIN(pin)      (0x0a + OFFSET(pin))

#define REG_RESET               (0x7d)

#define RESET_SEQ_1             (0x12)
#define RESET_SEQ_2             (0x34)

#define REG_CLOCK               (0x1e)
/* Sets clock to generate on its own, but not output a clock signal */
#define REG_CLOCK_INTERNALONLY  (0x40)

#define REG_MISC                (0x1f)
#define REG_MISC_MAXFREQ        (0x10)
#define REG_MISC_MINFREQ        (0x70)

#define REG_LEDDRIVERENABlE(pin) (0x20 + OFFSET(pin))

static inline int clamp(int value, int min, int max) {
    return value < min ? min : value > max ? max : value;
}

/** Register address for RegIOnX
 *
 * The addresses are irregular because different pin groups have different
 * features, and unlike in ARM memory mapped registers where vendors often
 * choose to have the indexing regular and leave invalid registers in the
 * middle ... well, that's what was chosen here.
 * */
static inline uint8_t reg_i_on(int x) {
    return 0x2a + x * 3 + 2 * (clamp(x - 4, 0, 4) + clamp(x - 12, 0, 4));
}

#ifdef __cplusplus
}
#endif

#endif /* SX150X_REGS_H */
/** @} */

