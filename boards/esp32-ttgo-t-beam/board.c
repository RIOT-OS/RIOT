/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_esp32_ttgo-t-beam
 * @{
 *
 * @file
 * @brief       Board specific definitions for TTGO T-Beam board
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"

#if MODULE_ESP32_TTGO_T_BEAM_V1_0
#include "periph/i2c.h"
#endif

#ifdef __cplusplus
 extern "C" {
#endif

#define AXP192_I2C_ADDR         (0x34)
#define AXP192_LDO234_DC23_CTL  (0x12)
#define AXP192_LDO3OUT_VOL      (0x29)
#define AXP192_LDO3_ON_BIT      (1 << 3)

void board_init(void)
{
#if MODULE_ESP32_TTGO_T_BEAM_V1_0
    uint8_t reg;

    i2c_acquire(I2C_DEV(0));
    /* enable the LDO3 power control */
    i2c_read_reg(I2C_DEV(0), AXP192_I2C_ADDR, AXP192_LDO234_DC23_CTL, &reg, 0);
    reg |= AXP192_LDO3_ON_BIT;
    i2c_write_reg(I2C_DEV(0), AXP192_I2C_ADDR, AXP192_LDO234_DC23_CTL, reg, 0);
    /* set the output voltage to 3V3 */
    i2c_write_reg(I2C_DEV(0), AXP192_I2C_ADDR, AXP192_LDO3OUT_VOL, 0xff, 0);
    i2c_release(I2C_DEV(0));
#endif
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
