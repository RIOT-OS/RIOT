/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       EFM32 default configuration
 *
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Use LETIMER as the base timer for xtimer (effectively ztimer)
 *
 * @deprecated  This option is deprecated and should be replaced by
 *              @ref CONFIG_EFM32_ZTIMER_USE_LETIMER.
 */
#ifndef CONFIG_EFM32_XTIMER_USE_LETIMER
#  define CONFIG_EFM32_XTIMER_USE_LETIMER       0
#endif

/**
 * @brief   Use LETIMER as the base timer for ztimer
 */
#ifndef CONFIG_EFM32_ZTIMER_USE_LETIMER
#  if CONFIG_EFM32_XTIMER_USE_LETIMER
#    define CONFIG_EFM32_ZTIMER_USE_LETIMER     1
#  else
#    define CONFIG_EFM32_ZTIMER_USE_LETIMER     0
#  endif
#endif

#ifdef __cplusplus
}
#endif

/** @} */
