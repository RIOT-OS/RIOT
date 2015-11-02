/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_xmc1000
 * @{
 *
 * @file
 * @brief           Implementation specific CPU module gating
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#ifndef PERIPH_GATING_H_
#define PERIPH_GATING_H_

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup cpu_xmc1000_gating XMC1000 module gating
 * @ingroup cpu_xmc1000
 * @{
 */

/**
 * @brief Used to wrap code that needs access to protected bits.
 *
 * @details
 * Wraps code that needs access to bits that are protected by the bit
 * protection scheme. See memory organization chapter in XMC1100
 * reference manual (7.4.2.1).
 * @hideinitializer
 * @{
 */
#define BIT_PROTECTED(Expr)                             \
    do {                                                \
        SCU_GENERAL->PASSWD = 0x000000C0UL;             \
        Expr;                                           \
        SCU_GENERAL->PASSWD = 0x000000C3UL;             \
    } while (0)
/** @} */

/**
 * @brief Clear gating.
 *
 * @details
 * Disables the gating of a module and thus enables its clock.
 * @param [in] Module One of VADC, CCU40, USIC0, WDT, RTC.
 * @hideinitializer
 * @{
 */
#define GATING_CLEAR(Module) BIT_PROTECTED(             \
        SET_BIT(SCU_CLK->CGATCLR0,                      \
                SCU_CLK_CGATCLR0_ ## Module ## _Pos));
/** @} */

/**
 * @brief Set gating.
 *
 * @details
 * Enable the gating of a module and thus disables its clock.
 * @param [in] Module One of VADC, CCU40, USIC0, WDT, RTC.
 * @hideinitializer
 * @{
 */
#define GATING_SET(Module) BIT_PROTECTED(               \
        SET_BIT(SCU_CLK->CGATSET0,                      \
                SCU_CLK_CGATSET0_ ## Module ## _Pos));
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_GATING_H_ */
