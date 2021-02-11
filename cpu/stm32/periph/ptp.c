/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       PTP clock and timer implementation
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include <inttypes.h>
#include <string.h>

#include "assert.h"
#include "atomic_utils.h"
#include "bit.h"
#include "macros/units.h"
#include "periph/ptp.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Workaround for typos in vendor files; drop when fixed upstream */
#ifndef ETH_PTPTSCR_TSSSR
#define ETH_PTPTSCR_TSSSR ETH_PTPTSSR_TSSSR
#endif
#ifndef ETH_PTPTSCR_TSSARFE
#define ETH_PTPTSCR_TSSARFE ETH_PTPTSSR_TSSARFE
#endif

/* PTPSSIR is the number of nanoseconds to add onto the sub-second register
 * (the one counting the nanoseconds part of the timestamp with the
 * configuration we chose here). It is therefore the resolution of the clock
 * in nanoseconds. (Note that the accuracy is expected to be way worse than
 * the resolution.)
 */
#ifndef STM32_PTPSSIR
#if CLOCK_CORECLOCK > MHZ(200)
/* Go for 10 ns resolution on CPU clocked higher than 200 MHz */
#define STM32_PTPSSIR       (10LLU)
#elif CLOCK_CORECLOCK > MHZ(100)
/* Go for 20 ns resolution on CPU clocked higher than 100 MHz */
#define STM32_PTPSSIR       (20LLU)
#else
/* Go for 50 ns resolution on CPU all other CPUs */
#define STM32_PTPSSIR       (50LLU)
#endif /* CLOCK_CORECLOCK */
#endif /* !STM32_PTPSSIR */

/**
 * @brief   Return the result of x / y, scientifically rounded
 * @param   x       Number to divide
 * @param   y       @p x should be divided by this
 * @return  x/y, scientifically rounded
 * @pre     Both @p x and @p y are compile time constant integers and the
 *          expressions are evaluated without side-effects
 */
#define ROUNDED_DIV(x, y)   (((x) + ((y) / 2)) / (y))

static const uint32_t ptpssir = STM32_PTPSSIR;
static const uint32_t ptptsar = ROUNDED_DIV(NS_PER_SEC * (1ULL << 32), CLOCK_AHB * STM32_PTPSSIR);

void ptp_init(void)
{
    /* The PTP clock is initialized during periph_init(), while stm32_eth is
     * initialized during auto_init(). As auto_init() depends on periph_init(),
     * we can be sure that the PTP clock is always the first to use the
     * Ethernet MAC. The Ethernet driver will skip the common initialization
     * part when the PTP clock is used. */
    stm32_eth_common_init();

    /* In the following, the steps described in "Programming steps for
     * system time generation initialization" on page 1805 in RM0410 Rev4
     * are done */

    /* Mask the time stamp trigger interrupt */
    ETH->MACIMR |= ETH_MACIMR_TSTIM;
    /* Set TSE bit in time stamp register to enable time stamping */
    ETH->PTPTSCR |= ETH_PTPTSCR_TSE;
    /* Use decimal mode (subsecond register counts nanoseconds, not in
     * 2^(-31) seconds) */
    ETH->PTPTSCR |= ETH_PTPTSCR_TSSSR;
    /* Set subsecond increment register. This will be added onto the subsecond
     * register whenever a 32 bit accumulator register overflows*/
    ETH->PTPSSIR = ptpssir;
    ptp_clock_adjust_speed(0);
    /* Wait new PTPSAR value becomes active */
    while (ETH->PTPTSCR & ETH_PTPTSCR_TSARU) { }
    /* Enable fine grained correction now */
    ETH->PTPTSCR |= ETH_PTPTSCR_TSFCU;

    static const ptp_timestamp_t initial_time = {
        .seconds = 0,
        .nanoseconds = 0
    };
    ptp_clock_set(&initial_time);
    if (IS_USED(MODULE_PERIPH_ETH)) {
        /* enable timestamping of all received frames */
        ETH->PTPTSCR |= ETH_PTPTSSR_TSSARFE;
    }
    DEBUG("[periph_ptp] Initialized with PTPSAR = %" PRIu32 ", PTPSSIR = %" PRIu32 "\n",
          ptptsar, ptpssir);
}

void ptp_clock_adjust_speed(int32_t correction)
{
    uint64_t offset = ptptsar;
    offset *= correction;
    offset >>= 32;
    uint32_t adjusted_ptptsar = ptptsar + (uint32_t)offset;
    /* Value to add onto the 32 bit accumulator register (which causes the
     * value in ETH->PTPSSIR to be added onto the subsection register on
     * overflow) */
    ETH->PTPTSAR = adjusted_ptptsar;
    /* Wait for pending clock speed adjustments to complete */
    while (ETH->PTPTSCR & ETH_PTPTSCR_TSARU) { }
    /* Load new PTPTSAR value to hardware */
    ETH->PTPTSCR |= ETH_PTPTSCR_TSARU;
    DEBUG("[periph_ptp] Using PTPSAR = %" PRIu32 ", PTPSSIR = %" PRIu32 "\n",
          adjusted_ptptsar, ptpssir);
}

void ptp_clock_adjust(int64_t offset)
{
    unsigned state = irq_disable();
    ptp_timestamp_t ts;
    uint64_t abs_offset = (offset < 0) ? -offset : offset;
    ptp_ns2ts(&ts, abs_offset);
    ETH->PTPTSHUR = ts.seconds;
    ETH->PTPTSLUR = (offset < 0) ? (1UL << 31) | ts.nanoseconds : ts.nanoseconds;
    while (ETH->PTPTSCR & (ETH_PTPTSCR_TSSTU | ETH_PTPTSCR_TSSTI)) {
        /* wait until new time value can be set */
    }
    ETH->PTPTSCR |= ETH_PTPTSCR_TSSTU;
    irq_restore(state);
    DEBUG("[periph_ptp] Updated time by %c%" PRIu32 ".%09" PRIu32 "\n",
          (offset < 0) ? '-' : '+', (uint32_t)ts.seconds, ts.nanoseconds);
}

void ptp_clock_set(const ptp_timestamp_t *time)
{
    assert(time && time->nanoseconds < NS_PER_SEC);
    unsigned state = irq_disable();
    /* First, set the timestamp update registers */
    ETH->PTPTSHUR = time->seconds;
    ETH->PTPTSLUR = time->nanoseconds;
    /* From the data sheet (regarding setting TSSTI):
     * > Both the TSSTU and TSSTI bits must be read as zero before you can set
     * > this bit.
     */
    while (ETH->PTPTSCR & (ETH_PTPTSCR_TSSTU | ETH_PTPTSCR_TSSTI)) {
        /* wait until new time value can be set */
    }
    /* Now, ask the peripheral to atomically set the clock from the update
     * registers */
    ETH->PTPTSCR |= ETH_PTPTSCR_TSSTI;
    irq_restore(state);
}

void ptp_clock_read(ptp_timestamp_t *timestamp)
{
    unsigned irq_state = irq_disable();
    /* Read first high register, then low, then again high. If the value in
     * high register changed between the reads, we start again to prevent
     * corrupted timestamps being passed to the user.     */
    do {
        timestamp->seconds = ETH->PTPTSHR;
        timestamp->nanoseconds = ETH->PTPTSLR;
    } while (timestamp->seconds != ETH->PTPTSHR);

    /* TODO: Most significant bit of ETH->PTPTSLR is the sign bit of the time
     * stamp. Because the seconds register is unsigned, an overflow is not
     * expected before year 2106. It is not clear from the data sheet, how the
     * time stamp is to be interpreted when the negative bit is set. For now,
     * we just ignore this potential source of problems. */
    irq_restore(irq_state);
}

#if IS_USED(MODULE_PERIPH_PTP_TIMER)
void ptp_timer_clear(void)
{
    const atomic_bit_u32_t tsite = atomic_bit_u32(&ETH->PTPTSCR, ETH_PTPTSCR_TSITE_Pos);
    atomic_clear_bit_u32(tsite);
}

void ptp_timer_set_absolute(const ptp_timestamp_t *target)
{
    assert(target);
    DEBUG("[periph_ptp] Set timer: %" PRIu32 ".%" PRIu32 "\n",
          (uint32_t)target->seconds, target->nanoseconds);
    unsigned state = irq_disable();
    /* Mask PTP timer IRQ first, so that an interrupt is not triggered
     * too early. (The target time is not set atomically.) */
    ETH->MACIMR |= ETH_MACIMR_TSTIM;
    /* Set target time */
    ETH->PTPTTHR = target->seconds;
    ETH->PTPTTLR = target->nanoseconds;
    /* Enable PTP timer IRQ */
    ETH->PTPTSCR |= ETH_PTPTSCR_TSITE;
    /* Unmask the time stamp trigger interrupt */
    ETH->MACIMR &= ~ETH_MACIMR_TSTIM;
    irq_restore(state);
    DEBUG("PTPTSCR: 0x%08x, MACIMR: 0x%08x, MACSR: 0x%08x\n",
          (unsigned)ETH->PTPTSCR, (unsigned)ETH->MACIMR, (unsigned)ETH->MACSR);
}
#endif /* IS_USED(MODULE_PTP_TIMER) */
