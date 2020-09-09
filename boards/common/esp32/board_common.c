/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp32
 * @{
 *
 * @file
 * @brief       Common declarations and functions for all ESP32 boards.
 *
 * This file contains default declarations and functions that are valid
 * for all ESP32 boards.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"
#include "esp_common.h"
#include "log.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "rom/ets_sys.h"

#ifdef __cplusplus
 extern "C" {
#endif

void board_init_common(void)
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

extern void adc_print_config(void);
extern void dac_print_config(void);
extern void pwm_print_config(void);
extern void i2c_print_config(void);
extern void spi_print_config(void);
extern void uart_print_config(void);
extern void can_print_config(void);

void print_board_config(void)
{
    ets_printf("\nBoard configuration:\n");

    #if MODULE_PERIPH_ADC
    adc_print_config();
    #endif
    #if MODULE_PERIPH_DAC
    dac_print_config();
    #endif
    #if MODULE_PERIPH_PWM
    pwm_print_config();
    #endif
    #if MODULE_PERIPH_I2C
    i2c_print_config();
    #endif
    #if MODULE_PERIPH_SPI
    spi_print_config();
    #endif
    #if MODULE_PERIPH_UART
    uart_print_config();
    #endif

    #ifdef MODULE_PERIPH_CAN
    can_print_config();
    #endif

    ets_printf("\tLED\t\tpins=[ ");
    #ifdef LED0_PIN
    ets_printf("%d ", LED0_PIN);
    #endif
    #ifdef LED1_PIN
    ets_printf("%d ", LED1_PIN);
    #endif
    #ifdef LED2_PIN
    ets_printf("%d ", LED2_PIN);
    #endif
    ets_printf("]\n");

    ets_printf("\tBUTTONS\t\tpins=[ ");
    #ifdef BUTTON0_PIN
    ets_printf("%d ", BUTTON0_PIN);
    #endif
    #ifdef BUTTON2_PIN
    ets_printf("%d ", BUTTON1_PIN);
    #endif
    #ifdef BUTTON3_PIN
    ets_printf("%d ", BUTTON2_PIN);
    #endif
    ets_printf("]\n");

    ets_printf("\n");
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
