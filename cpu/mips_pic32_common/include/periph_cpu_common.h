/*
 * Copyright(C) 2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup         cpu_mips_pic32_common
 * @brief           Shared CPU specific definitions for the MIPS family.
 * @{
 *
 * @file
 * @brief           Shared CPU specific definitions for the MIPS family.
 *
 * @author          Francois Berder <francois.berder@imgtec.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
/** @} */

/**
 * @brief   Length of the CPU_ID in bytes
 */
#define CPUID_LEN           (4U)

/**
 * @brief   Override GPIO pin selection macro
 */
#define GPIO_PIN(x,y)       ((x << 4) | (y & 0xf))

/**
 * @brief   Available ports on the PIC32 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
};

/**
 * @brief   Prevent shared timer functions from being used
 */
#define PERIPH_TIMER_PROVIDES_SET

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
