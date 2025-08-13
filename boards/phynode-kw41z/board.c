/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_phynode-kw41z
 * @{
 *
 * @file
 * @brief       Board specific initialization for the PhyNODE KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      José Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
/* If further modules are supported (e.g. HDC1010 sensor or the EPD) they have
 * to be added here so V_PERIPH pin is set and the devices are powered */
#if defined(MODULE_MMA8X5X) || defined(MODULE_TCS37727) || \
    defined(MODULE_CCS811)
    gpio_init(V_PERIPH_PIN, GPIO_OUT);
    gpio_set(V_PERIPH_PIN);
#endif
}
