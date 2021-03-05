/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer
 * @{
 *
 * @file
 * @brief       ztimer default configuration
 *
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZTIMER_CONFIG_H
#define ZTIMER_CONFIG_H

#include "board.h"
#include "periph_conf.h"

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief   Use periph_timer as the base timer for ZTIMER_USEC
 */
#define CONFIG_ZTIMER_USEC_TYPE_PERIPH_TIMER (1)

/* for ZTIMER_USEC, use xtimer configuration if available and no ztimer
 * specific configuration is set. */
#if CONFIG_ZTIMER_USEC_TYPE_PERIPH_TIMER
#  ifndef CONFIG_ZTIMER_USEC_DEV
#    ifdef XTIMER_DEV
#      define CONFIG_ZTIMER_USEC_DEV            XTIMER_DEV
#    endif
#  endif
#  ifndef CONFIG_ZTIMER_USEC_BASE_FREQ
#    ifdef XTIMER_HZ
#      define CONFIG_ZTIMER_USEC_BASE_FREQ      XTIMER_HZ
#    endif
#  endif
#  ifndef CONFIG_ZTIMER_USEC_WIDTH
#    ifdef XTIMER_WIDTH
#      define CONFIG_ZTIMER_USEC_WIDTH          XTIMER_WIDTH
#    endif
#  endif
#endif

/**
 * @brief   Default timer device for ZTIMER_USEC
 */
#ifndef CONFIG_ZTIMER_USEC_DEV
#define CONFIG_ZTIMER_USEC_DEV          (TIMER_DEV(0))
#endif

/**
 * @brief   ZTIMER_USEC optimal minimum value for ztimer_set()
 *
 * When scheduling an ISR every timer will be set to:
 * max(CONFIG_ZTIMER_USEC_MIN, value).
 *
 * This value only applies if the counter object used for ZTIMER_USEC is
 * periph_timer. This is supposed to be defined per-device in e.g., board.h.
 */
#ifndef CONFIG_ZTIMER_USEC_MIN
#define CONFIG_ZTIMER_USEC_MIN              (10)
#endif

/**
   @brief   ZTIMER_USEC counter object width
 */
#ifndef CONFIG_ZTIMER_USEC_WIDTH
#  if (TIMER_0_MAX_VALUE) == 0xffff
#    define CONFIG_ZTIMER_USEC_WIDTH        (16)
#  elif (TIMER_0_MAX_VALUE) == 0xffffffUL
#    define CONFIG_ZTIMER_USEC_WIDTH        (24)
#  else
#    define CONFIG_ZTIMER_USEC_WIDTH        (32)
#  endif
#endif

/**
 * @brief   The frequency of ZTIMER_USEC_BASE (base ztimer for ZTIMER_USEC)
 */
#ifndef CONFIG_ZTIMER_USEC_BASE_FREQ
#define CONFIG_ZTIMER_USEC_BASE_FREQ    (1000000LU)
#endif

/**
 * @brief   The frequency of ZTIMER_MSEC_BASE (base ztimer for ZTIMER_MSEC)
 */
#ifdef MODULE_ZTIMER_PERIPH_RTT
#  define CONFIG_ZTIMER_MSEC_BASE_FREQ   (RTT_FREQUENCY)
# else
#  define CONFIG_ZTIMER_MSEC_BASE_FREQ   (CONFIG_ZTIMER_USEC_BASE_FREQ)
#endif  /* MODULE_ZTIMER_PERIPH_RTT */

/**
 * @brief   The minimum pm mode required for ZTIMER_TIMER to run.
 */
#ifndef CONFIG_ZTIMER_TIMER_BLOCK_PM_MODE
#  ifdef CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE
#    define CONFIG_ZTIMER_TIMER_BLOCK_PM_MODE CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE
#  else
#    define CONFIG_ZTIMER_TIMER_BLOCK_PM_MODE ZTIMER_CLOCK_NO_REQUIRED_PM_MODE
#  endif
#endif

/**
 * @brief   The minimum pm mode required for ZTIMER_RTT to run
 */
#ifndef CONFIG_ZTIMER_RTT_BLOCK_PM_MODE
#  ifdef CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE
#    define CONFIG_ZTIMER_RTT_BLOCK_PM_MODE CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE
#  else
#    define CONFIG_ZTIMER_RTT_BLOCK_PM_MODE ZTIMER_CLOCK_NO_REQUIRED_PM_MODE
#  endif
#endif

/**
 * @brief   The minimum pm mode required for ZTIMER_RTC to run
 */
#ifndef CONFIG_ZTIMER_RTC_BLOCK_PM_MODE
#  ifdef CONFIG_ZTIMER_SEC_REQUIRED_PM_MODE
#    define CONFIG_ZTIMER_RTC_BLOCK_PM_MODE CONFIG_ZTIMER_SEC_REQUIRED_PM_MODE
#  else
#    define CONFIG_ZTIMER_RTC_BLOCK_PM_MODE ZTIMER_CLOCK_NO_REQUIRED_PM_MODE
#  endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER_CONFIG_H */
/** @} */
