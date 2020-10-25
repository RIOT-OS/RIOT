/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_frdm-kl43z NXP FRDM-KL43Z Board
 * @ingroup     boards
 * @brief       Support for the NXP FRDM-KL43Z
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-KL43Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   FOPT setting
 */
/* Disable ROM bootloader, launch user application from flash */
#define KINETIS_FOPT        (0xff & ~(NV_FOPT_BOOTSRC_SEL_MASK | NV_FOPT_BOOTPIN_OPT_MASK))

/**
 * @name    LED pin definitions and handlers
 * @{
 */
/* LEDs are named LED1, LED2 in the original board schematics, we remap the LEDs
 * to 0-indexed: NXP LED1 -> RIOT LED0, NXP LED2 -> RIOT LED1 */
#define LED0_PIN            GPIO_PIN(PORT_D,  5)
#define LED1_PIN            GPIO_PIN(PORT_E, 31)

#define LED0_MASK           (1 <<  5)
#define LED1_MASK           (1 << 31)

#define LED0_ON             (GPIOD->PCOR = LED0_MASK)
#define LED0_OFF            (GPIOD->PSOR = LED0_MASK)
#define LED0_TOGGLE         (GPIOD->PTOR = LED0_MASK)

#define LED1_ON             (GPIOE->PCOR = LED1_MASK)
#define LED1_OFF            (GPIOE->PSOR = LED1_MASK)
#define LED1_TOGGLE         (GPIOE->PTOR = LED1_MASK)
/** @} */

/**
 * @name    xtimer configuration
 * @{
 */
#if KINETIS_XTIMER_SOURCE_PIT
/* PIT xtimer configuration */
#define XTIMER_DEV                  (TIMER_PIT_DEV(0))
#define XTIMER_CHAN                 (0)
/* Default xtimer settings should work on the PIT */
#else
/* LPTMR xtimer configuration */
#define XTIMER_DEV                  (TIMER_LPTMR_DEV(0))
#define XTIMER_CHAN                 (0)
/* LPTMR is 16 bits wide and runs at 32768 Hz (clocked by the RTC) */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (5)
#define XTIMER_ISR_BACKOFF          (5)
#define XTIMER_OVERHEAD             (4)
#define XTIMER_HZ                   (32768ul)
#endif
/** @} */

/**
 * @name    MAG3110 3-axis magnetometer bus configuration
 * @{
 */
#define MAG3110_PARAM_I2C       I2C_DEV(0)
#define MAG3110_PARAM_ADDR      0x0E
/** @} */

/**
 * @name    MMA8451Q 3-axis accelerometer bus configuration
 * @{
 */
#define MMA8X5X_PARAM_I2C       I2C_DEV(0)
#define MMA8X5X_PARAM_ADDR      0x1D
#define MMA8X5X_PARAM_TYPE      (MMA8X5X_TYPE_MMA8451)
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
