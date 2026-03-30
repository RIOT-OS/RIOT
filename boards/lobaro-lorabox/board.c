/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_lobaro_lorabox
 * @{
 *
 * @file
 * @brief       Board initialization code for Lobaro LoraBox
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
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
    gpio_init(EN3V3_PIN, GPIO_OUT);
}
