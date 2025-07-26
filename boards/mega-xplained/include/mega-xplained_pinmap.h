/*
 * SPDX-FileCopyrightText: 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_mega-xplained
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Mega Xplained pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Mega Xplained specific pin numbers.
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Mapping of MCU pins to Mega Xplained pins
 * @{
 */
/*
 * DESCRIPTION Xplained API PIN          PORT   PIN
 */
#define J1_PIN1                 GPIO_PIN(PORT_C, 1)
#define J1_PIN2                 GPIO_PIN(PORT_C, 0)
#define J1_PIN3                 GPIO_PIN(PORT_D, 0)
#define J1_PIN4                 GPIO_PIN(PORT_D, 1)
#define J1_PIN5                 GPIO_PIN(PORT_B, 4)
#define J1_PIN6                 GPIO_PIN(PORT_B, 5)
#define J1_PIN7                 GPIO_PIN(PORT_B, 6)
#define J1_PIN8                 GPIO_PIN(PORT_B, 7)

#define J2_PIN1                 GPIO_PIN(PORT_A, 0)
#define J2_PIN2                 GPIO_PIN(PORT_A, 1)
#define J2_PIN3                 GPIO_PIN(PORT_A, 2)
#define J2_PIN4                 GPIO_PIN(PORT_A, 3)
#define J2_PIN5                 GPIO_PIN(PORT_A, 4)
#define J2_PIN6                 GPIO_PIN(PORT_A, 5)
#define J2_PIN7                 GPIO_PIN(PORT_A, 6)
#define J2_PIN8                 GPIO_PIN(PORT_A, 7)

#define J3_PIN1                 GPIO_PIN(PORT_B, 0)
#define J3_PIN2                 GPIO_PIN(PORT_B, 1)
#define J3_PIN3                 GPIO_PIN(PORT_B, 2)
#define J3_PIN4                 GPIO_PIN(PORT_B, 3)
#define J3_PIN5                 GPIO_PIN(PORT_D, 4)
#define J3_PIN6                 GPIO_PIN(PORT_D, 5)
#define J3_PIN7                 GPIO_PIN(PORT_C, 4)
#define J3_PIN8                 GPIO_PIN(PORT_C, 5)

#define J4_PIN1                 GPIO_PIN(PORT_C, 1)
#define J4_PIN2                 GPIO_PIN(PORT_C, 0)
#define J4_PIN3                 GPIO_PIN(PORT_D, 2)
#define J4_PIN4                 GPIO_PIN(PORT_D, 3)
#define J4_PIN5                 GPIO_PIN(PORT_D, 4)
#define J4_PIN6                 GPIO_PIN(PORT_B, 5)
#define J4_PIN7                 GPIO_PIN(PORT_B, 6)
#define J4_PIN8                 GPIO_PIN(PORT_B, 7)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
