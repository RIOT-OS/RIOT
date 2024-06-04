/*
 * Copyright (C) 2014 INRIA
 *               2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430
 * @{
 * @file
 * @brief       MSP430Fxzy clock initialization
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "atomic_utils.h"
#include "busy_wait.h"
#include "macros/math.h"
#include "macros/units.h"
#include "modules.h"
#include "periph_conf.h"

#ifdef RSEL3
#define RSEL_MASK (RSEL0 | RSEL1 | RSEL2 | RSEL3)
#define HAS_RSEL3 1
#else
#define RSEL_MASK (RSEL0 | RSEL1 | RSEL2)
#define HAS_RSEL3 0
#endif

uint32_t msp430_dco_freq;
static uint8_t msp430_clock_refcounts[MSP430_CLOCK_NUMOF];

static inline bool is_dco_in_use(const msp430_clock_params_t *params)
{
    return ((params->main_clock_source == MAIN_CLOCK_SOURCE_DCOCLK) ||
            (params->submain_clock_source == SUBMAIN_CLOCK_SOURCE_DCOCLK));
}

/* As low speed crystal only a standard 32.768 kHz watch crystal is allowed */
static inline bool is_valid_low_freq(uint32_t freq) {
    return (freq == 32768);
}

/* Valid parameter range for the high speed crystal by family:
 * MSP430 x1xx: 450 kHz <= f <= 8 MHz
 * MSP430 F2xx / G2xx: 400 kHz <= f <= 16 MHz
 */
static inline bool is_valid_high_freq(uint32_t freq) {
    if (IS_ACTIVE(CPU_FAM_MSP430_F2XX_G2XX)) {
        return ((freq >= KHZ(400)) && (freq <= MHZ(16)));
    }

    return ((freq >= KHZ(450)) && (freq <= MHZ(8)));
}

/* C doesn't have constexpr() or an equivalent feature to perform complex
 * compile time tests. To still do so, we rely on constant folding and dead
 * code elimination:
 * If a test doesn't pass, we call an undefined function with a meaningful
 * name as "error message". If that call is optimized out (because the
 * configuration test passes), the linking step passes. If the config is
 * invalid, the function call will not be optimized out and the linker will not
 * be able to resolve all symbols, resulting in invalid configs being caught
 * at compile time. */
static void check_config(void)
{
    /* LFXT1 can either be a low frequency 32.768 kHz watch crystal or a
     * high frequency crystal. We cannot function without this crystal */
    if (!is_valid_low_freq(clock_params.lfxt1_frequency)
            && !is_valid_high_freq(clock_params.lfxt1_frequency)) {
        extern void lfxt1_frequency_invalid(void);
        lfxt1_frequency_invalid();
    }

    /* XT2 is not required and may be configured as 0 Hz to indicate its
     * absence. If it is present, it must but a valid frequency for a high
     * frequency crystal. */
    if ((clock_params.xt2_frequency != 0) &&
            !is_valid_high_freq(clock_params.xt2_frequency)) {
        extern void xt2_frequency_invalid(void);
        xt2_frequency_invalid();
    }

    /* XT2 can only source the main system clock if it is present */
    if ((clock_params.main_clock_source == MAIN_CLOCK_SOURCE_XT2CLK)
            && (clock_params.xt2_frequency == 0)) {
        extern void xt2_as_main_clock_source_selected_but_xt2_is_not_present(void);
        xt2_as_main_clock_source_selected_but_xt2_is_not_present();
    }

    /* If the DCO is disabled (0 Hz), we cannot use it as clock source */
    if (is_dco_in_use(&clock_params)
            && (clock_params.target_dco_frequency == 0)) {
        extern void dco_configured_as_clock_source_but_is_disabled(void);
        dco_configured_as_clock_source_but_is_disabled();
    }
}

static void wait_for_crystals_to_stabilize(void)
{
    /* The MSP430x1xx MCU family have an oscillator fault detector that sets
     * the OFIFG flag when a fault is detected, which can trigger an IRQ
     * and/or be polled in software. We go for polling here.
     *
     * There are some foot guns to take into account:
     *
     * 1. The LFXT1 clock is only monitored in high speed mode. So for the
     *    typical setup of using a 32.768 kHz watch crystal one can only wait
     *    for some preconfigured static value and hope this was long enough
     *    to stabilize. (But that seems to work fine in practise.)
     * 2. If no high speed crystal is in use (e.g. LFXT1 is used with a
     *    watch crystal and XT2 is not connected), the fault flag will
     *    stick and cannot be cleared. Waiting for the flag to clear would
     *    result in getting stuck during boot.
     *
     * Because of 1, we go for best effort by just waiting UINT16_MAX busy
     * loops for the watch crystal to stabilize. This is common practise and
     * seems to work just fine.
     * Because of 2 we don't wait for OFIFG if no high speed crystal is in use.
     *
     * We wait for watch crystal first, as chances are good that by then the
     * high speed crystal (if any) is stable or about to become stable as well,
     * so that the second wait is fast.
     */

    if (is_valid_low_freq(clock_params.lfxt1_frequency)) {
        /* wait best effort for watch crystal to stabilize */
        busy_wait(UINT16_MAX);
    }

    if (is_valid_high_freq(clock_params.lfxt1_frequency)
            || is_valid_high_freq(clock_params.xt2_frequency)) {
        /* at least one high frequency crystal present, wait for that to
         * stabilize */
        do {
            /* Clear oscillator fault flag */
            IFG1 &= ~(OFIFG);
            /* Now wait for at least 50 μs so that the oscillator fault flag is
             * reliably set again if the oscillators are not stable yet.
             * The datasheet indicates that counting down form UINT8_MAX when
             * running from DCO with default settings (as during boot) is taking
             * at least 50 μs */
            busy_wait(UINT8_MAX);
            /* Finally, check if the oscillator fault flag was reset by hardware */
        } while (IFG1 & OFIFG);
    }
}

static void calibrate_dco(void)
{
    /* We assume that the LFXT1 frequency is stable. Now we compute what the
     * ratio of the (unstable) DCO frequency to the stable LFXT1 frequency
     * (available as auxiliary clock with a clock divider of 8) and use a timer
     * to capture the actual clock ratio */
    const uint16_t clock_ratio = DIV_ROUND(clock_params.target_dco_frequency,
            (clock_params.lfxt1_frequency / 8));

    /* The CCI2B input on capture/compare unit 2 of timer A has the auxiliary
     * clock as internal input. Hence, for every tick of the auxiliary clock
     * we will get one capture of the continuously running timer. The diff
     * of two subsequent captures matches then the actual clock ratio. */
    TACCTL2 = CCIS_1    /* CCI2B as capture input */
            | CM_1      /* capture on rising edge */
            | CAP;      /* capture mode instead of compare mode */
    /* configure timer A to run from submain clock, which now is sourced
     * by DCO without divider*/
    TACTL = TASSEL_2    /* submain clock (DCO) as source */
          | TACLR       /* clear timer */
          | MC_2;       /* continuous mode, count up to UINT16_MAX */

    /* First, binary search for the correct RSELx settings (which have a
     * higher impact on the actual frequency than the DCOCTL settings).
     * We're starting with a highest bit. */

    uint8_t bcsctl1 = BCSCTL1 & ~(RSEL_MASK);
    uint8_t rselx = 0;

    /*
     * Note: For MSP430F2xx (HAS_RSEL3 == 1) the bit in RSEL3 is actually
     * ignored if an external resistor is used for the DCO. Still, setting
     * it won't hurt */
    const uint8_t rsel_max_bit = (HAS_RSEL3) ? BIT3 : BIT2;

    for (uint8_t iter = rsel_max_bit; iter != 0; iter >>= 1) {
        BCSCTL1 = bcsctl1 | rselx | iter;

        /* busy wait for timer to capture */
        while (!(TACCTL2 & CCIFG)) { }
        /* clear capture flag */
        TACCTL2 &= ~CCIFG;
        uint16_t start = TACCR2;

        /* busy wait for timer to capture */
        while (!(TACCTL2 & CCIFG)) { }
        /* clear capture flag */
        TACCTL2 &= ~CCIFG;
        uint16_t stop = TACCR2;

        uint16_t actual_clock_ratio = stop - start;
        if (actual_clock_ratio <= clock_ratio) {
            /* DCO too slow, or just fast enough --> keep the bit */
            rselx |= iter;
        }
    }

    /* Now do the same dance again, but for DCOCTL */
    uint8_t dcoctl = 0;
    uint16_t actual_clock_ratio;

    for (uint8_t iter = 0x80; iter != 0; iter >>= 1) {
        DCOCTL = dcoctl | iter;

        /* busy wait for timer to capture */
        while (!(TACCTL2 & CCIFG)) { }
        /* clear capture flag */
        TACCTL2 &= ~CCIFG;
        uint16_t start = TACCR2;

        /* busy wait for timer to capture */
        while (!(TACCTL2 & CCIFG)) { }
        /* clear capture flag */
        TACCTL2 &= ~CCIFG;
        uint16_t stop = TACCR2;

        actual_clock_ratio = stop - start;
        if (actual_clock_ratio <= clock_ratio) {
            /* DCO too slow, or just fast enough --> keep the bit */
            dcoctl |= iter;
        }
    }

    /* stopping timer used fore calibration */
    TACCTL2 = 0;
    TACTL = 0;

    /* calculate actual DCO frequency */
    msp430_dco_freq = (clock_params.lfxt1_frequency * actual_clock_ratio) >> 3;
}

void default_clock_init(void)
{
    /* This function contains only dead code and the call should be fully
     * optimized out by the compiler. In fact, we rely on this: If it is
     * not fully optimized out, we will get linker errors. */
    check_config();

#ifdef __MSP430_HAS_WDT__
    /* Stopping watchdog timer, clock initialization may take a while. */
    WDTCTL = WDTPW | WDTHOLD;
#endif

    /* set LFXT1 and XT2 hardware settings, keep RSEL to reset value (only
     * RSEL 2 bit set) */
    uint8_t bcsctl1 = RSEL2;
    if (clock_params.xt2_frequency == 0) {
        bcsctl1 |= XT2OFF;
    }
    if (clock_params.lfxt1_frequency > 32768) {
        bcsctl1 |= XTS;
    }

    /* We want to calibrate DCO against the auxiliary clock domain, which is
     * hardwired to use LFXT1 as clock source. Because LFXT1 is always using
     * a crystal (and mostly even an accurate watch crystal), we can assume
     * that the auxiliary clock to be accurate (enough). The algorithm we
     * use for clock calibration assumes that multiple DCO ticks match one
     * tick on the auxiliary clock. We use the highest possible clock divider
     * for now, to increase the range this works. */
    if (clock_params.target_dco_frequency != 0) {
        bcsctl1 |= AUXILIARY_CLOCK_DIVIDE_BY_8;
    }

    BCSCTL1 = bcsctl1;

    uint8_t bcsctl2 = 0;

    if (clock_params.has_r_osc) {
        bcsctl2 = DCOR;
    }

    BCSCTL2 = bcsctl2;

    /* In case we run from a crystal, we need that to stabilize. If we run from
     * DCO instead, we also need the crystals to become stable so that we can
     * calibrate the DCO against it.
     * ==> Wait for crystal to be stable regardless of config */
    wait_for_crystals_to_stabilize();

    /* If a target DCO frequency was configured, we calibrate the DCO to run at
     * that - even if the DCO is not used to source either main or submain
     * clock. The application may switch the clock source later on to DCO, e.g.
     * to run by default at a low clock to reduce power consumption and go to
     * "turbo mode" for event processing */
    if (clock_params.target_dco_frequency != 0) {
        calibrate_dco();
    }

    /* Crystals are now stable and the DCO has been calibrated, if the
     * board config asked for a calibrated DCO clock. We can now safely
     * apply the clock sources and also apply the clock dividers */

    /* BCSCTL1 contains now freshly calibrated RSELx parameters in addition
     * to the auxiliary clock divider we used for calibration. We read the
     * settings back and clear the clock divider to apply the one provided
     * by the board settings. */
    bcsctl1 = BCSCTL1 & ~(AUXILIARY_CLOCK_DIVIDE_BY_8);
    BCSCTL1 = bcsctl1 | clock_params.auxiliary_clock_divier;

    /* apply main and submain clock source and clock dividers */
    BCSCTL2 = bcsctl2
            | clock_params.main_clock_source
            | clock_params.submain_clock_source
            | clock_params.main_clock_divier
            | clock_params.submain_clock_divier;

    /* if DCO is not used at all, disable it to preserve power */
    if (clock_params.target_dco_frequency == 0) {
        /* Setting bit SCG0 in the status register (SR) disables the DCO.
         * We do so in assembly, as r2 is not memory mapped. */
        __asm__ __volatile__ (
            "bis %[scg0], SR"           "\n\t"
            : /* no outputs */
            : /* inputs: */
              [scg0]        "i"(SCG0) /* bitmask to set SCGO0 as immediate */
            : /* no clobbers */
        );
    }
}

__attribute__((weak, alias("default_clock_init"))) void clock_init(void);

uint32_t PURE msp430_submain_clock_freq(void)
{
    uint16_t shift = (clock_params.submain_clock_divier >> 1) & 0x3;
    switch (clock_params.submain_clock_source) {
    case SUBMAIN_CLOCK_SOURCE_LFXT1CLK:
        /* the submain clock cannot explicitly select LFXT1 or XT2, but really
         * is "crystal" or DCO. If the selection is "crystal", it will default
         * to XT2 as clock source (if enabled) and falls back to LFXT1. */
        if (clock_params.xt2_frequency) {
            return clock_params.xt2_frequency >> shift;
        }
        return clock_params.lfxt1_frequency >> shift;
    default:
    case SUBMAIN_CLOCK_SOURCE_DCOCLK:
        return msp430_dco_freq >> shift;
    }
}

uint32_t PURE msp430_auxiliary_clock_freq(void)
{
    uint16_t shift = (clock_params.auxiliary_clock_divier >> 4) & 0x3;
    return clock_params.lfxt1_frequency >> shift;
}

void msp430_clock_acquire(msp430_clock_t clock)
{
    assume((unsigned)clock < MSP430_CLOCK_NUMOF);
    uint8_t before = atomic_fetch_add_u8(&msp430_clock_refcounts[clock], 1);
    (void)before;
    assert(before < UINT8_MAX);
}

void msp430_clock_release(msp430_clock_t clock)
{
    assume((unsigned)clock < MSP430_CLOCK_NUMOF);
    uint8_t before = atomic_fetch_sub_u8(&msp430_clock_refcounts[clock], 1);
    (void)before;
    assert(before > 0);
}

void pm_set_lowest(void)
{
    /* disable IRQs, wait two cycles for this to take effect, backup
     * state register */
    uint16_t state;
    __asm__ volatile(
        "bic %[gie], SR"                    "\n\t"
        "nop"                               "\n\t"
        "nop"                               "\n\t"
        "mov.w SR, %[state]"                "\n\t"
        : [state]   "=r"(state)
        : [gie]     "i"(GIE)
        : "memory"
    );

    /* When applying the power safe mode, we want to be able to wake up again.
     * So set global interrupt enable then. */
    state |= GIE;
    /* disabling CPU works always, even when keeping the clocks running */
    state |= CPUOFF | SCG0;

    if (msp430_clock_refcounts[MSP430_CLOCK_SUBMAIN] == 0) {
        state |= SCG1;
    }

    if (msp430_clock_refcounts[MSP430_CLOCK_AUXILIARY] == 0) {
        state |= OSCOFF;
    }

    /* write new state */
    __asm__ volatile(
        "mov.w %[state], SR"                "\n\t"
        : /* no outputs */
        : [state]   "r"(state)
        : "memory"
    );
}
