/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Interface for configuring the clock tree of STM32 CPUs
 *
 * @todo            This interface should probably be moved and implemented for
 *                  every STM32 CPU
 *
 * @author          Hauke Petersen <hauke.pertersen@fu-berlin.de>
*/

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configure the high speed clock domain (main system clock)
 *
 * This function initializes and configures the main systems clock(s). For this,
 * the following actions are carried out:
 * - enable the HSI
 * - use the HSI as system clock (so now we are always in a defined state)
 * - configure flash wait states and AHB/APB bus dividers
 * - [opt] enable the HSE / MSI
 * - configure the PLL
 * - use PLL as system clock
 * - turn off the HSI if unused
 *
 * All of these options are configurable through the board's periph_conf.h
 */
void stmclk_init_sysclk(void);

/**
 * @brief   Enable the internal high speed clock (HSI)
 */
void stmclk_enable_hsi(void);

/**
 * @brief   Disable the internal high speed clock (HSI)
 *
 * @note    The HSI is only disabled, if it is at that point not used to drive
 *          the system clock or the PLL
 */
void stmclk_disable_hsi(void);

/**
 * @brief   Configure and enable the low speed clock domain
 *
 * The actual clock used as input for the low frequency clock can be either the
 * external low speed clock (LSE) or the internal low speed clock (LSI). This
 * is configured in the board's periph_conf.h file.
 */
void stmclk_enable_lfclk(void);

/**
 * @brief   Disable the low frequency clock domain
 *
 * @note    When calling this function, better know what you do: be sure that
 *          the clock is not needed by any peripheral before calling this
 *          function.
 */
void stmclk_disable_lfclk(void);

/**
 * @brief   Unlock write access to the backup domain control
 */
void stmclk_dbp_unlock(void);

/**
 * @brief   Lock write access to backup control domain
 */
void stmclk_dbp_lock(void);

/**
 * @brief   Check whether write access to the backup domain is locked
 *
 * @retval  true: locked
 * @retval  false: unlocked
 */
bool stmclk_dbp_is_locked(void);

#ifdef __cplusplus
}
#endif

/** @} */
