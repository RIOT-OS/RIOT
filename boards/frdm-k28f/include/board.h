/*
 * Copyright (C) 2017 Eistec AB
 * Copyright (C) 2020 Koen Zandberg
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-k28f
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-K28F
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "mtd.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_E,  6)
#define LED1_PIN            GPIO_PIN(PORT_E,  7)
#define LED2_PIN            GPIO_PIN(PORT_E,  8)

#define LED0_MASK           (1 <<  6)
#define LED1_MASK           (1 <<  7)
#define LED2_MASK           (1 <<  8)

#define LED0_ON            (GPIOE->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOE->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOE->PTOR = LED0_MASK)

#define LED1_ON            (GPIOE->PCOR = LED1_MASK)
#define LED1_OFF           (GPIOE->PSOR = LED1_MASK)
#define LED1_TOGGLE        (GPIOE->PTOR = LED1_MASK)

#define LED2_ON            (GPIOE->PCOR = LED2_MASK)
#define LED2_OFF           (GPIOE->PSOR = LED2_MASK)
#define LED2_TOGGLE        (GPIOE->PTOR = LED2_MASK)
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
/* SW2, SW3 will short these pins to ground when pushed. Both pins have external
 * pull-up resistors to VDD */
/* BTN0 is mapped to SW2 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 4)
#define BTN0_MODE           GPIO_IN
/* BTN1 is mapped to SW3 */
#define BTN1_PIN            GPIO_PIN(PORT_D, 0)
#define BTN1_MODE           GPIO_IN
/** @} */

/**
 * @name SPI NOR flash hardware configuration
 *
 * A Micron MT25QU256ABA1EW7-0SIT is present on the board
 * @{
 */
#define FRDM_K28F_NOR_PAGE_SIZE          (256)
#define FRDM_K28F_NOR_PAGES_PER_SECTOR   (64)
#define FRDM_K28F_NOR_SECTOR_COUNT       (512)
#define FRDM_K28F_NOR_FLAGS              (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K)
#define FRDM_K28F_NOR_SPI_DEV            SPI_DEV(1)
#define FRDM_K28F_NOR_SPI_CLK            SPI_CLK_10MHZ
#define FRDM_K28F_NOR_SPI_CS             SPI_HWCS(0)
#define FRDM_K28F_NOR_SPI_MODE           SPI_MODE_0
/** @} */

/** Default MTD device */
#define MTD_0 mtd0

/** mtd flash emulation device */
extern mtd_dev_t *mtd0;

/**
 * @name    FXOS8700CQ 3-axis accelerometer and magnetometer bus configuration
 * @{
 */
#define FXOS8700_PARAM_I2C          I2C_DEV(0)
#define FXOS8700_PARAM_ADDR         0x1C
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
