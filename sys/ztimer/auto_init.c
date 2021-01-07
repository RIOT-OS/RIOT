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

#if MODULE_ZTIMER_USEC
#  if CONFIG_ZTIMER_USEC_TYPE_PERIPH_TIMER
static ztimer_periph_timer_t _ztimer_periph_timer_usec = {
    .min = CONFIG_ZTIMER_USEC_MIN
};

ztimer_clock_t *const ZTIMER_USEC_BASE = &_ztimer_periph_timer_usec.super;
#    if CONFIG_ZTIMER_USEC_BASE_FREQ == FREQ_1MHZ
ztimer_clock_t *const ZTIMER_USEC = &_ztimer_periph_timer_usec.super;
#    elif CONFIG_ZTIMER_USEC_BASE_FREQ == 250000LU
static ztimer_convert_shift_t _ztimer_convert_shift_usec;
ztimer_clock_t *const ZTIMER_USEC = &_ztimer_convert_shift_usec.super.super;
#    else
static ztimer_convert_frac_t _ztimer_convert_frac_usec;
ztimer_clock_t *const ZTIMER_USEC = &_ztimer_convert_frac_usec.super.super;
#    endif
#   else
#     error ztimer_usec selected, but no configuration available!
#  endif
#endif

/* use RTT for ZTIMER_MSEC, if used. Use RTT also for ZTIMER_USEC,
   unless module ztimer_periph_rtc is explicitly used by application */
#if MODULE_ZTIMER_PERIPH_RTT && (MODULE_ZTIMER_MSEC || (MODULE_ZTIMER_SEC && !MODULE_ZTIMER_PERIPH_RTC))
static ztimer_periph_rtt_t _ztimer_periph_timer_rtt_msec_sec;
#  define ZTIMER_RTT_INIT (&_ztimer_periph_timer_rtt_msec_sec)
#endif

#if MODULE_ZTIMER_MSEC
#  if MODULE_ZTIMER_PERIPH_RTT
ztimer_clock_t *const ZTIMER_MSEC_BASE = &_ztimer_periph_timer_rtt_msec_sec;
#    if RTT_FREQUENCY != FREQ_1KHZ
static ztimer_convert_frac_t _ztimer_convert_frac_msec;
ztimer_clock_t *const ZTIMER_MSEC = &_ztimer_convert_frac_msec_sec.super.super;
#  define ZTIMER_MSEC_CONVERT_LOWER_FREQ    RTT_FREQUENCY
#  define ZTIMER_MSEC_CONVERT_LOWER         (&_ztimer_periph_timer_rtt_msec_sec)
#    else
ztimer_clock_t *const ZTIMER_MSEC = &_ztimer_periph_timer_rtt_msec_sec;
#    endif
#  elif MODULE_ZTIMER_USEC
static ztimer_convert_frac_t _ztimer_convert_frac_msec;
ztimer_clock_t *const ZTIMER_MSEC = &_ztimer_convert_frac_msec.super.super;
ztimer_clock_t *const ZTIMER_MSEC_BASE = &_ztimer_periph_timer_usec.super;
#    define ZTIMER_MSEC_CONVERT_LOWER       ZTIMER_USEC_BASE
#    define ZTIMER_MSEC_CONVERT_LOWER_FREQ  CONFIG_ZTIMER_USEC_BASE_FREQ
#  else
#  error No suitable ZTIMER_MSEC config. Maybe add USEMODULE += ztimer_usec?
#  endif
#endif

#if MODULE_ZTIMER_SEC
#  if MODULE_ZTIMER_PERIPH_RTC
static ztimer_periph_rtc_t _ztimer_periph_timer_rtc_sec;
ztimer_clock_t *const ZTIMER_SEC = &_ztimer_periph_timer_rtc_sec;
#  elif MODULE_ZTIMER_PERIPH_RTT
static ztimer_convert_frac_t _ztimer_convert_frac_sec;
ztimer_clock_t *const ZTIMER_SEC = &_ztimer_convert_frac_sec.super.super;
ztimer_clock_t *const ZTIMER_SEC_BASE = &_ztimer_periph_timer_msec_sec.super;
#    define ZTIMER_SEC_CONVERT_LOWER        (&_ztimer_periph_timer_rtt_msec_sec)
#    define ZTIMER_SEC_CONVERT_LOWER_FREQ   RTT_FREQUENCY
#  elif MODULE_ZTIMER_USEC
static ztimer_convert_frac_t _ztimer_convert_frac_sec;
ztimer_clock_t *const ZTIMER_SEC = &_ztimer_convert_frac_sec.super.super;
ztimer_clock_t *const ZTIMER_SEC_BASE = &_ztimer_periph_timer_usec.super;
#    define ZTIMER_SEC_CONVERT_LOWER        ZTIMER_USEC_BASE
#    define ZTIMER_SEC_CONVERT_LOWER_FREQ   CONFIG_ZTIMER_USEC_BASE_FREQ
#  else
#  error No suitable ZTIMER_SEC config. Maybe add USEMODULE += ztimer_usec?
#  endif
#endif

void ztimer_init(void)
{
#if MODULE_ZTIMER_USEC
#  if CONFIG_ZTIMER_USEC_TYPE_PERIPH_TIMER
    LOG_DEBUG(
        "ztimer_init(): ZTIMER_USEC using periph timer %u, freq %lu, width %u\n",
        CONFIG_ZTIMER_USEC_DEV, CONFIG_ZTIMER_USEC_BASE_FREQ,
        CONFIG_ZTIMER_USEC_WIDTH);

    ztimer_periph_timer_init(&_ztimer_periph_timer_usec, CONFIG_ZTIMER_USEC_DEV,
                             CONFIG_ZTIMER_USEC_BASE_FREQ,
                             WIDTH_TO_MAXVAL(CONFIG_ZTIMER_USEC_WIDTH));
#  endif
#  if CONFIG_ZTIMER_USEC_BASE_FREQ != FREQ_1MHZ
#    if CONFIG_ZTIMER_USEC_BASE_FREQ == FREQ_250KHZ
    LOG_DEBUG("ztimer_init(): ZTIMER_USEC convert_shift %lu to 1000000\n",
              CONFIG_ZTIMER_USEC_BASE_FREQ);
    ztimer_convert_shift_up_init(&_ztimer_convert_shift_usec,
                                 &_ztimer_periph_timer_usec.super, 2);
#    else
    LOG_DEBUG("ztimer_init(): ZTIMER_USEC convert_frac %lu to 1000000\n",
              CONFIG_ZTIMER_USEC_BASE_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_usec,
                             &_ztimer_periph_timer_usec.super,
                             FREQ_1MHZ, CONFIG_ZTIMER_USEC_BASE_FREQ);
#    endif
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
#  ifdef MODULE_PM_LAYERED
    LOG_DEBUG("ztimer_init(): ZTIMER_USEC setting required_pm_mode to %i\n",
              CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE);
    ZTIMER_USEC->required_pm_mode = CONFIG_ZTIMER_USEC_REQUIRED_PM_MODE;
#  endif
#endif

#ifdef ZTIMER_RTT_INIT
    LOG_DEBUG("ztimer_init(): initializing rtt\n");
    ztimer_periph_rtt_init(ZTIMER_RTT_INIT);
#endif

#if MODULE_ZTIMER_MSEC
#  if ZTIMER_MSEC_CONVERT_LOWER_FREQ
    LOG_DEBUG("ztimer_init(): ZTIMER_MSEC convert_frac from %lu to 1000\n",
              (long unsigned)ZTIMER_MSEC_CONVERT_LOWER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_msec,
                             ZTIMER_MSEC_CONVERT_LOWER,
                             FREQ_1KHZ, ZTIMER_MSEC_CONVERT_LOWER_FREQ);
#  endif
#  ifdef CONFIG_ZTIMER_MSEC_ADJUST
    LOG_DEBUG("ztimer_init(): ZTIMER_MSEC setting adjust value to %i\n",
              CONFIG_ZTIMER_MSEC_ADJUST);
    ZTIMER_MSEC->adjust = CONFIG_ZTIMER_MSEC_ADJUST;
#  endif
#  ifdef MODULE_PM_LAYERED
    LOG_DEBUG("ztimer_init(): ZTIMER_MSEC setting required_pm_mode to %i\n",
              CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE);
    ZTIMER_MSEC->required_pm_mode = CONFIG_ZTIMER_MSEC_REQUIRED_PM_MODE;
#  endif
#endif

#if MODULE_ZTIMER_SEC
#  if MODULE_ZTIMER_PERIPH_RTC
    LOG_DEBUG("ztimer_init(): initializing rtc\n");
    ztimer_periph_rtc_init(&_ztimer_periph_timer_rtc_sec);
#  else
    LOG_DEBUG("ztimer_init(): ZTIMER_SEC convert_frac from %lu to 1000\n",
              (long unsigned)ZTIMER_SEC_CONVERT_LOWER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_sec,
                             ZTIMER_SEC_CONVERT_LOWER,
                             FREQ_1HZ, ZTIMER_SEC_CONVERT_LOWER_FREQ);
#  endif
#endif
}
