/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_periph_ptp
 * @{
 *
 * @file
 * @brief       Common code for PTP clocks and timers
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include "kernel_defines.h"
#include "irq.h"
#include "periph/ptp.h"

#if !defined(HAVE_PTP_TIMER_SET_U64)
void ptp_timer_set_u64(uint64_t target)
{
    unsigned irq_state = irq_disable();
    if (IS_ACTIVE(HAVE_PTP_TIMER_SET_ABSOLUTE) && IS_ACTIVE(HAVE_PTP_CLOCK_READ)) {
        /* This is slightly more efficient when the PTP clock implementation
         * uses ptp_timestamp_t natively */
        ptp_timestamp_t now;
        ptp_clock_read(&now);
        now.seconds += target / NS_PER_SEC;
        now.nanoseconds += target % NS_PER_SEC;
        while (now.nanoseconds >= NS_PER_SEC) {
            now.seconds++;
            now.nanoseconds -= NS_PER_SEC;
        }
        ptp_timer_set_absolute(&now);
    }
    else {
        ptp_timer_set_absolute_u64(ptp_clock_read_u64() + target);
    }
    irq_restore(irq_state);
}
#endif /* !defined(HAVE_PTP_TIMER_SET_U64) */
