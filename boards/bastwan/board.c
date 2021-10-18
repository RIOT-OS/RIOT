/*
 * Copyright (C) 2021  Lokotius Filzer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    /* initialize the boards LED */
    gpio_init(LED0_PIN, GPIO_OUT);

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
