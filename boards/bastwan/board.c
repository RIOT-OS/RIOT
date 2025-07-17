/*
 * SPDX-FileCopyrightText: 2021 Lokotius Filzer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_bastwan
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the bastwan board
 *
 *
 * @author      Lokotius Filzer <h-filzer@pixelsalat.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_SX127X
#include "sx127x_params.h"
#endif

void board_init(void)
{

#ifdef TX_SWITCH_PWR_PIN
    /* Enables pwr to the Antenna switch circuit */
    gpio_init(TX_SWITCH_PWR_PIN, GPIO_OUT);
    gpio_set(TX_SWITCH_PWR_PIN);
#endif /* TX_SWITCH_PWR_PIN */
       /* initialize board specific pins for LoRa */
#ifdef MODULE_SX127X
    /* Enable the pwr for the tcxo of the radio module*/
    gpio_init(TCXO_PWR_PIN, GPIO_OUT);
    gpio_set(TCXO_PWR_PIN);
    /* Enable Toggling on TX/RX between RFI and RFO on the antenna switch SKY13373 */
    gpio_init(TX_OUTPUT_SEL_PIN, GPIO_OUT);
    gpio_write(TX_OUTPUT_SEL_PIN, !SX127X_PARAM_PASELECT);
#endif /* USEMODULE_SX127X */
}
