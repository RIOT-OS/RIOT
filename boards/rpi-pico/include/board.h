/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         boards_rpi_pico
 * @{
 *
 * @file
 * @brief           Board specific definitions for the Raspberry Pi Pico
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED configuration
 * @{
 */
#define LED0_PIN        GPIO_PIN(0, 25)
#define LED0_ON         do {SIO->GPIO_OUT_SET.reg = 1UL << 25;} while (0)
#define LED0_OFF        do {SIO->GPIO_OUT_CLR.reg = 1UL << 25;} while (0)
#define LED0_TOGGLE     do {SIO->GPIO_OUT_XOR.reg = 1UL << 25;} while (0)
#define LED0_NAME       "LED(Green)"
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
