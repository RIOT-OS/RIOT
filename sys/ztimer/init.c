/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
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
 * @brief       ztimer initialization code
 *
 *
 * This file could benefit a lot from code generation...
 *
 * Anyhow, this configures ztimer as follows:
 *
 * 1. if ztimer_usec in USEMODULE:
 * 1.1. assume ztimer_usec uses periph_timer
 * 1.2a. if no config given
 * 1.2a.1a. use xtimer config if available
 * 1.2a.1b. default to TIMER_DEV(0), 32bit
 * 1.2b. else, use config
 *
 * 2. if ztimer_msec in USEMODULE:
 * 2.1a. if periph_rtt in USEMODULE: use that
 * 2.1b: else: convert from ZTIMER_USEC
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "kernel_defines.h"

#include "board.h"
#include "ztimer.h"
#include "ztimer/convert_frac.h"
#include "ztimer/convert_shift.h"
#include "ztimer/convert_muldiv64.h"
#include "ztimer/periph_timer.h"
#include "ztimer/periph_rtt.h"
#include "ztimer/periph_rtc.h"
#include "ztimer/config.h"

#include "log.h"

#define WIDTH_TO_MAXVAL(width)  (UINT32_MAX >> (32 - width))

#define FREQ_1MHZ       1000000LU
#define FREQ_250KHZ     250000LU
#define FREQ_1KHZ       1000LU
#define FREQ_1HZ        1LU

/* Step 0: define available ztimer-periphery by activated modules */

/* #if CONFIG_ZTIMER_USEC_TYPE_PERIPH_TIMER
 * replaces #if MODULE_ZTIMER_PERIPH_TIMER
 * the ztimer_periph_timer is always available
 * having an (1) config defined in ztimer/config.h
 */

#if CONFIG_ZTIMER_USEC_TYPE_PERIPH_TIMER
#  define ZTIMER_TIMER      _ztimer_periph_timer
#  define ZTIMER_TIMER_CLK  _ztimer_periph_timer.super
#  define ZTIMER_TIMER_FREQ CONFIG_ZTIMER_USEC_BASE_FREQ
#endif

#if MODULE_ZTIMER_PERIPH_RTT
#  define ZTIMER_RTT      _ztimer_periph_timer_rtt
#  define ZTIMER_RTT_CLK  _ztimer_periph_timer_rtt
#  define ZTIMER_RTT_FREQ RTT_FREQUENCY
#endif

#if MODULE_ZTIMER_PERIPH_RTC
#  define ZTIMER_RTC      _ztimer_periph_timer_rtc
#  define ZTIMER_RTC_CLK  _ztimer_periph_timer_rtc
#  define ZTIMER_RTC_FREQ FREQ_1HZ
#endif

/* Step 1: select which periphery to use for the higher level ZTIMER_*SEC
 *         selected periphery is marked for initialisation (INIT_ZTIMER_<periph>
 *         prepare defines for ztimer initialization
 */

/* ZTIMER_USEC always uses the basic timer
 * basic timer is available on all boards */
#if MODULE_ZTIMER_USEC
#  ifndef INIT_ZTIMER_TIMER
#    define INIT_ZTIMER_TIMER 1
#  endif
#endif

/* ZTIMER_MSEC prefers ZTIMER_RTT (ztimer_periph_rtt)
 * if it is available and runs at a frequency > 1kHz
 * if not it falls back to use the basic timer */
#if MODULE_ZTIMER_MSEC
#  if defined(ZTIMER_RTT) && ZTIMER_RTT_FREQ >= FREQ_1KHZ
#    define ZTIMER_MSEC_RTT 1
#    ifndef INIT_ZTIMER_RTT
#      define INIT_ZTIMER_RTT 1
#    endif
#    if ZTIMER_RTT_FREQ != FREQ_1KHZ
#      define ZTIMER_MSEC_CONVERT_LOWER_FREQ ZTIMER_RTT_FREQ
#    endif
#  else
#    define ZTIMER_MSEC_TIMER 1
#    ifndef INIT_ZTIMER_TIMER
#      define INIT_ZTIMER_TIMER 1
#    endif
#    define ZTIMER_MSEC_CONVERT_LOWER_FREQ ZTIMER_TIMER_FREQ
#  endif
#endif

/* ZTIMER_SEC prefers ZTIMER_RTT (ztimer_periph_rtt) if it is available
 * if not it prefers ZTIMER_RTC (ztimer_periph_rtc) if it is available
 * if not it falls back to use the basic timer */
#if MODULE_ZTIMER_SEC
#  ifdef ZTIMER_RTT
#    define ZTIMER_SEC_RTT
#    ifndef INIT_ZTIMER_RTT
#      define INIT_ZTIMER_RTT 1
#    endif
#    define ZTIMER_SEC_CONVERT_LOWER_FREQ ZTIMER_RTT_FREQ
#  else
#    ifdef ZTIMER_RTC
#      define ZTIMER_SEC_RTC
#      ifndef INIT_ZTIMER_RTC
#        define INIT_ZTIMER_RTC 1
#      endif
#    else
#      define ZTIMER_SEC_TIMER
#      ifndef INIT_ZTIMER_TIMER
#        define INIT_ZTIMER_TIMER 1
#      endif
#      define ZTIMER_SEC_CONVERT_LOWER_FREQ ZTIMER_TIMER_FREQ
#    endif
#  endif
#endif

/* Step 2: setup static memory for used ztimer-periphery */

#if INIT_ZTIMER_TIMER
static ztimer_periph_timer_t ZTIMER_TIMER = {
    .min = CONFIG_ZTIMER_USEC_MIN
};
#endif

#if INIT_ZTIMER_RTT
static ztimer_periph_rtt_t ZTIMER_RTT;
#endif

#if INIT_ZTIMER_RTC
static ztimer_periph_rtc_t ZTIMER_RTC;
#endif

/* Step 3: setup constants for ztimers and memory for converters */

#if MODULE_ZTIMER_USEC
#  ifdef ZTIMER_TIMER
ztimer_clock_t *const ZTIMER_USEC_BASE = &ZTIMER_TIMER_CLK;
#  else
#    error No suitable ZTIMER_USEC config. Basic timer configuration missing?
#  endif
#  if ZTIMER_TIMER_FREQ == FREQ_1MHZ
ztimer_clock_t *const ZTIMER_USEC = &ZTIMER_TIMER_CLK;
#  elif ZTIMER_TIMER_FREQ == 250000LU
static ztimer_convert_shift_t _ztimer_convert_shift_usec;
ztimer_clock_t *const ZTIMER_USEC = &_ztimer_convert_shift_usec.super.super;
#  else
static ztimer_convert_frac_t _ztimer_convert_frac_usec;
ztimer_clock_t *const ZTIMER_USEC = &_ztimer_convert_frac_usec.super.super;
#  endif
#endif

#if MODULE_ZTIMER_MSEC
#  ifdef ZTIMER_MSEC_RTT
ztimer_clock_t *const ZTIMER_MSEC_BASE = &ZTIMER_RTT_CLK;
#  elif defined(ZTIMER_MSEC_TIMER)
ztimer_clock_t *const ZTIMER_MSEC_BASE = &ZTIMER_TIMER_CLK;
#  else
#    error No suitable ZTIMER_MSEC config. No rtt or basic timer?
#  endif
#  ifdef ZTIMER_MSEC_CONVERT_LOWER_FREQ
static ztimer_convert_frac_t _ztimer_convert_frac_msec;
ztimer_clock_t *const ZTIMER_MSEC = &_ztimer_convert_frac_msec.super.super;
#  else
ztimer_clock_t *const ZTIMER_MSEC = &ZTIMER_RTT_CLK;
#   endif
#endif

#if MODULE_ZTIMER_SEC
#  ifdef ZTIMER_SEC_RTC
ztimer_clock_t *const ZTIMER_SEC_BASE = &ZTIMER_RTC_CLK;
ztimer_clock_t *const ZTIMER_SEC = &ZTIMER_RTC_CLK;
#  elif defined(ZTIMER_SEC_RTT)
ztimer_clock_t *const ZTIMER_SEC_BASE = &ZTIMER_RTT_CLK;
#  elif defined(ZTIMER_SEC_TIMER)
ztimer_clock_t *const ZTIMER_SEC_BASE = &ZTIMER_TIMER_CLK;
#  else
#    error No suitable ZTIMER_SEC config. No rtc, rtt or basic timer?
#  endif

#  ifdef ZTIMER_SEC_CONVERT_LOWER_FREQ
static ztimer_convert_frac_t _ztimer_convert_frac_sec;
ztimer_clock_t *const ZTIMER_SEC = &_ztimer_convert_frac_sec.super.super;
#  endif
#endif

void ztimer_init(void)
{
/* Step 4: initialize used ztimer-periphery */
#if INIT_ZTIMER_TIMER
    LOG_DEBUG(
        "ztimer_init(): ZTIMER_TIMER using periph timer %u, freq %lu, width %u\n",
        CONFIG_ZTIMER_USEC_DEV, CONFIG_ZTIMER_USEC_BASE_FREQ,
        CONFIG_ZTIMER_USEC_WIDTH);
    ztimer_periph_timer_init(&ZTIMER_TIMER, CONFIG_ZTIMER_USEC_DEV,
                             ZTIMER_TIMER_FREQ, WIDTH_TO_MAXVAL(CONFIG_ZTIMER_USEC_WIDTH));
#  ifdef MODULE_PM_LAYERED
    LOG_DEBUG("ztimer_init(): ZTIMER_TIMER setting block_pm_mode to %i\n",
              CONFIG_ZTIMER_TIMER_BLOCK_PM_MODE);
    ZTIMER_TIMER_CLK.block_pm_mode = CONFIG_ZTIMER_TIMER_BLOCK_PM_MODE;
#  endif
#endif

#if INIT_ZTIMER_RTT
    LOG_DEBUG("ztimer_init(): initializing rtt\n");
    ztimer_periph_rtt_init(&ZTIMER_RTT);
#  ifdef MODULE_PM_LAYERED
    LOG_DEBUG("ztimer_init(): ZTIMER_RTT setting block_pm_mode to %i\n",
              CONFIG_ZTIMER_RTT_BLOCK_PM_MODE);
    ZTIMER_RTT_CLK.block_pm_mode = CONFIG_ZTIMER_RTT_BLOCK_PM_MODE;
#  endif
#endif

#if INIT_ZTIMER_RTC
    LOG_DEBUG("ztimer_init(): initializing rtc\n");
    ztimer_periph_rtc_init(&ZTIMER_RTC);
#  ifdef MODULE_PM_LAYERED
    LOG_DEBUG("ztimer_init(): ZTIMER_RTC setting block_pm_mode to %i\n",
              CONFIG_ZTIMER_RTC_BLOCK_PM_MODE);
    ZTIMER_RTC_CLK.block_pm_mode = CONFIG_ZTIMER_RTC_BLOCK_PM_MODE;
#  endif
#endif

/* Step 5: initialize ztimers requested */
#if MODULE_ZTIMER_USEC
#  if ZTIMER_TIMER_FREQ != FREQ_1MHZ
#    if ZTIMER_TIMER_FREQ == FREQ_250KHZ
    LOG_DEBUG("ztimer_init(): ZTIMER_USEC convert_shift %lu to 1000000\n",
              ZTIMER_TIMER_FREQ);
    ztimer_convert_shift_up_init(&_ztimer_convert_shift_usec,
                                 ZTIMER_USEC_BASE, 2);
#    else
    LOG_DEBUG("ztimer_init(): ZTIMER_USEC convert_frac %lu to 1000000\n",
              ZTIMER_TIMER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_usec, ZTIMER_USEC_BASE,
                             FREQ_1MHZ, ZTIMER_TIMER_FREQ);
#    endif
#  else
    LOG_DEBUG("ztimer_init(): ZTIMER_USEC without conversion\n");
#  endif
#  ifdef CONFIG_ZTIMER_USEC_ADJUST_SET
    LOG_DEBUG("ztimer_init(): ZTIMER_USEC setting adjust_set value to %i\n",
              CONFIG_ZTIMER_USEC_ADJUST_SET );
    ZTIMER_USEC->adjust_set = CONFIG_ZTIMER_USEC_ADJUST_SET;
#  endif
#  ifdef CONFIG_ZTIMER_USEC_ADJUST_SLEEP
    LOG_DEBUG("ztimer_init(): ZTIMER_USEC setting adjust_sleep value to %i\n",
              CONFIG_ZTIMER_USEC_ADJUST_SLEEP );
    ZTIMER_USEC->adjust_sleep = CONFIG_ZTIMER_USEC_ADJUST_SLEEP;
#  endif
#endif

#if MODULE_ZTIMER_MSEC
#  if ZTIMER_MSEC_CONVERT_LOWER_FREQ
    LOG_DEBUG("ztimer_init(): ZTIMER_MSEC convert_frac from %lu to 1000\n",
              (long unsigned)ZTIMER_MSEC_CONVERT_LOWER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_msec, ZTIMER_MSEC_BASE,
                             FREQ_1KHZ, ZTIMER_MSEC_CONVERT_LOWER_FREQ);
#  endif
#  ifdef CONFIG_ZTIMER_MSEC_ADJUST
    LOG_DEBUG("ztimer_init(): ZTIMER_MSEC setting adjust value to %i\n",
              CONFIG_ZTIMER_MSEC_ADJUST);
    ZTIMER_MSEC->adjust = CONFIG_ZTIMER_MSEC_ADJUST;
#  endif
#endif

#if MODULE_ZTIMER_SEC
#  if ZTIMER_SEC_CONVERT_LOWER_FREQ
    LOG_DEBUG("ztimer_init(): ZTIMER_SEC convert_frac from %lu to 1\n",
              (long unsigned)ZTIMER_SEC_CONVERT_LOWER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_sec, ZTIMER_SEC_BASE,
                             FREQ_1HZ, ZTIMER_SEC_CONVERT_LOWER_FREQ);
#  endif
#endif
}
