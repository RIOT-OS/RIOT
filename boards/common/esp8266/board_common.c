/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_esp8266 ESP8266 Common
 * @ingroup     boards_common
 * @brief       Common files for the esp8266 board.
 * @{
 *
 * @file
 * @brief       Definitions for all esp8266 board.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board_common.h"
#include "log.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

void board_init(void)
{
    #ifdef LED0_PIN
    gpio_init (LED0_PIN, GPIO_OUT);
    LED0_OFF;
    #endif
    #ifdef LED1_PIN
    gpio_init (LED1_PIN, GPIO_OUT);
    LED1_OFF;
    #endif
    #ifdef LED2_PIN
    gpio_init (LED2_PIN, GPIO_OUT);
    LED2_OFF;
    #endif
}

extern void pwm_print_config(void);
extern void i2c_print_config(void);
extern void spi_print_config(void);
extern void uart_print_config(void);
extern void timer_print_config(void);

void board_print_config (void)
{
    LOG_INFO("\nBoard configuration:\n");

    pwm_print_config();
    i2c_print_config();
    spi_print_config();
    uart_print_config();
    timer_print_config();

    LOG_INFO("\tLED: pins=[ ");
    #ifdef LED0_PIN
    LOG_INFO("%d ", LED0_PIN);
    #endif
    #ifdef LED1_PIN
    LOG_INFO("%d ", LED1_PIN);
    #endif
    #ifdef LED2_PIN
    LOG_INFO("%d ", LED2_PIN);
    #endif
    LOG_INFO("]\n");

    LOG_INFO("\n\n");
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
