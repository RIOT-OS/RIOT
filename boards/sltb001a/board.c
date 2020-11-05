/*
 * Copyright (C) 2015-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sltb001a
 * @{
 *
 * @file
 * @brief       Board specific implementations SLTB001A board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef MODULE_SILABS_PIC
#include "pic.h"
#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

#ifndef RIOTBOOT
    /* perform common board initialization */
    board_common_init();

#ifdef MODULE_SILABS_PIC
#ifdef MODULE_CCS811
    /* enable the CCS811 air quality/gas sensor */
    pic_write(CCS811_PIC_ADDR, (1 << CCS811_PIC_EN_BIT) | (1 << CCS811_PIC_WAKE_BIT));
#endif

#if ICM_20648_ENABLED
    /* enable the IMU sensor */
    pic_write(ICM20648_PIC_ADDR, 1 << ICM20648_PIC_EN_BIT);
#endif

#if defined(MODULE_BMP280) || defined(MODULE_SI7021) || SI1133_ENABLED || SI7210_ENABLED
    /* enable the environmental sensors */
    pic_write(ENV_SENSE_PIC_ADDR, 1 << ENV_SENSE_PIC_BIT);
#endif

#if RGB_LED1_ENABLED || RGB_LED2_ENABLED || RGB_LED3_ENABLED || RGB_LED4_ENABLED
    /* enable the RGB leds */
    pic_write(RGB_LED_ADDR,
              (1 << RGB_LED_EN_BIT) |
              (RGB_LED1_ENABLED << RGB_LED1_EN_BIT) |
              (RGB_LED2_ENABLED << RGB_LED2_EN_BIT) |
              (RGB_LED3_ENABLED << RGB_LED3_EN_BIT) |
              (RGB_LED4_ENABLED << RGB_LED4_EN_BIT));
#endif
#endif
#endif
}
