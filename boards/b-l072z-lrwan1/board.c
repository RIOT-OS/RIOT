/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_b-l072z-lrwan1
 * @{
 *
 * @file
 * @brief       Board specific implementations for the ST B-L072Z-LRWAN1 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "mtd_eeprom.h"

#if MODULE_MTD_EEPROM
static mtd_eeprom_t eeprom_mtd = MTD_EEPROM_INIT_VAL;
MTD_XFA_ADD(eeprom_mtd, 0);
#endif /* MODULE_MTD_EEPROM */

void board_init(void)
{
#if defined(MODULE_SX1276)
    /* Enable TCXO */
    gpio_init(RADIO_TCXO_VCC_PIN, GPIO_OUT);
    gpio_set(RADIO_TCXO_VCC_PIN);
#endif
}
