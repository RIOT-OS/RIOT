/*
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup     arm7_common
 * @brief       LPC2387 Newlib gettimeofday() system call glue
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#include <sys/time.h>
#include "kernel_types.h"

#if defined MODULE_RTC
#   include "periph/rtc.h"
#elif defined MODULE_VTIMER
#   include "vtimer.h"
#endif

/* Declared as external, without a definition. */
/* This will cause a linktime error, if _gettimeofday() is referenced, */
/* and neither rtc nor vtimer were linked in. */
extern void __gettimeofday_syscall_is_not_implemented_without_vtimer_or_rtc_module(void);

int _gettimeofday(struct timeval *tp, void *restrict tzp)
{
    (void) tzp;

#if defined MODULE_RTC
    rtc_time(tp);
#elif defined MODULE_VTIMER
    vtimer_gettimeofday(tp);
#else
    (void) tp;
    __gettimeofday_syscall_is_not_implemented_without_vtimer_or_rtc_module();
#endif

    return 0;
}
