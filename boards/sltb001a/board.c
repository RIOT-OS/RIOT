/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
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
#include "pic.h"

#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* perform common board initialization */
    board_common_init();

#ifdef MODULE_SILABS_PIC
    /* enable the CCS811 air quality/gas sensor */
#if CCS811_ENABLED
    pic_write(CCS811_PIC_ADDR, (1 << CCS811_PIC_EN_BIT) | (1 << CCS811_PIC_WAKE_BIT));
#endif

    /* enable the IMU sensor */
#if ICM_20648_ENABLED
    pic_write(ICM20648_PIC_ADDR, 1 << ICM20648_PIC_EN_BIT);
#endif

    /* enable the environmental sensors */
#if BMP280_ENABLED || SI1133_ENABLED || SI7021_ENABLED || SI7210A_ENABLED
    pic_write(ENV_SENSE_PIC_ADDR, 1 << ENV_SENSE_PIC_BIT);
#endif

    /* enable the RGB leds */
#if RGB_LED1_ENABLED || RGB_LED2_ENABLED || RGB_LED3_ENABLED || RGB_LED4_ENABLED
    pic_write(RGB_LED_ADDR,
              (1 << RGB_LED_EN_BIT) |
              (RGB_LED1_ENABLED << RGB_LED1_EN_BIT) |
              (RGB_LED2_ENABLED << RGB_LED2_EN_BIT) |
              (RGB_LED3_ENABLED << RGB_LED3_EN_BIT) |
              (RGB_LED4_ENABLED << RGB_LED4_EN_BIT));
#endif
#endif
}
