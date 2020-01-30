/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Atmel SAM R21 Xplained
 *              Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_antenna_config(uint8_t antenna)
{
    if (antenna == RFCTL_ANTENNA_EXT){
        gpio_set(RFCTL1_PIN);
        gpio_clear(RFCTL2_PIN);
    }
    else if (antenna == RFCTL_ANTENNA_BOARD){
        gpio_clear(RFCTL1_PIN);
        gpio_set(RFCTL2_PIN);
    }
}

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    /* initialize the on-board antenna switch */
    gpio_init(RFCTL1_PIN, GPIO_OUT);
    gpio_init(RFCTL2_PIN, GPIO_OUT);
    /* set default antenna switch configuration */
    board_antenna_config(RFCTL_ANTENNA_DEFAULT);

    /* initialize the CPU */
    cpu_init();
}

#ifdef MODULE_PERIPH_I2C
#include "periph/i2c.h"
#include "net/l2util.h"
#include "net/netif_ids.h"

#define DEV_I2C     I2C_DEV(0)
#define EDBG_ADDR   (0x28)
#define TOKEN_EUI64 (0xD2)
#define TOKEN_EXT   (0xE1)

/* EDBG provides an EUI-64 for the internal radio */
size_t board_get_eui64(netif_id_t driver, unsigned idx, eui64_t *addr)
{
    if (driver != AT86RF2XX_DEVUID) {
        return 0;
    }

    if (idx != 0) {
        return 0;
    }

    i2c_acquire(DEV_I2C);

    /* dummy read form another token */
    if (i2c_read_regs(0, EDBG_ADDR, TOKEN_EXT, addr, 2, 0)) {
        goto error;
    }

    if (i2c_read_regs(0, EDBG_ADDR, TOKEN_EUI64, addr, sizeof(*addr), 0)) {
        goto error;
    }

    i2c_release(DEV_I2C);
    return sizeof(eui64_t);

error:
    i2c_release(DEV_I2C);
    return 0;
}
#endif
