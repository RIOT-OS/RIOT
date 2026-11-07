/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Clock configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include <stdint.h>
#include "macros/units.h"

/** 1-15 MHz range
 * @see hardware/regs/xosc.h and chapter 8.2.8
 */
#define XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ 0xaa0u
/** 10-30 MHz range */
#define XOSC_CTRL_FREQ_RANGE_VALUE_10_30MHZ 0xaa1u
/** 25-60 MHz range */
#define XOSC_CTRL_FREQ_RANGE_VALUE_25_60MHZ 0xaa2u
/** 40-100 MHz range */
#define XOSC_CTRL_FREQ_RANGE_VALUE_40_100MHZ 0xaa3u
/** Disable the XOSC */
#define XOSC_CTRL_ENABLE_VALUE_DISABLE 0xd1eu
/** Enable the XOSC */
#define XOSC_CTRL_ENABLE_VALUE_ENABLE 0xfabu
/** LSB of the enable bit */
#define XOSC_CTRL_ENABLE_LSB 12u
/** Stable bit in the XOSC status register */
#define XOSC_STATUS_STABLE_BITS 0x80000000u
/** Default crystal frequency is 12 MHz */
#define XOSC_HZ MHZ(12u)
/** Reference divider for the PLL */
#define PLL_REF_DIV 1u
/** VCO frequency for the PLL, derived from the PLL parameters (1500 MHz) */
#define PLL_VCO_FREQ ((XOSC_HZ / PLL_REF_DIV) * PLL_FEEDBACK_DIVIDER_VALUE)
/** Post divider 1 for the PLL, set to 6 as per hardware manual */
#define PLL_PD1 6u
/** Post divider 2 for the PLL, set to 2 as per hardware manual */
#define PLL_PD2 2u
/** Power down bits for the PLL */
#define PLL_PWR_PD_BITS 0x00000001u
/** VCO power down bits for the PLL */
#define PLL_PWR_VCOPD_BITS 0x00000020u
/** Lock bit in the PLL control status register */
#define PLL_CS_LOCK_BITS 0x80000000u
/** LSB of the post divider 1 in the PLL primary register */
#define PLL_PRIM_POSTDIV1_LSB 16u
/** LSB of the post divider 2 in the PLL primary register */
#define PLL_PRIM_POSTDIV2_LSB 12u
/** Post divider power down bits for the PLL */
#define PLL_PWR_POSTDIVPD_BITS 0x00000008u
/** Enable bit for the peripheral clock control register */
#define CLK_PERI_CTRL_ENABLE_BIT (1u << 11u)
/** CPU frequency in Hz, same as @ref CLOCK_CORECLOCK */
#define CPUFREQ CLOCK_CORECLOCK
/** Maximum crystal frequency */
#define CLOCK_XOSC_MAX MHZ(15u)
/** Minimum crystal frequency */
#define CLOCK_XOSC_MIN MHZ(5u)
/** Crystal frequency */
#define CLOCK_XOSC (XOSC_HZ)
/** Minimum value of the post PLL clock divers */
#define PLL_POSTDIV_MIN 1u
/** Maximum value of the post PLL clock divers */
#define PLL_POSTDIV_MAX 7u
/** Minimum value of the PLL VCO feedback scaler */
#define PLL_VCO_FEEDBACK_SCALE_MIN 16u
/** Maximum value of the PLL VCO feedback scaler */
#define PLL_VCO_FEEDBACK_SCALE_MAX 320u
/** Minimum value of the clock divider applied before feeding the reference
 *  clock into the PLL */
#define PLL_REF_DIV_MIN 1u
/** Maximum value of the clock divider applied before feeding the reference
 *  clock into the PLL */
#define PLL_REF_DIV_MAX 63u
/** PLL feedback divider value, set to 125 as per hardware manual */
#define PLL_FEEDBACK_DIVIDER_VALUE 125u
/** Enable bit for the system clock control register to select the peripheral
 *  clock */
#define CLK_SYS_PERI_CTRL_ENABLE_BIT (1u << 0u)
/** Selected field value for the system clock control register to select the
 *  peripheral clock */
#define CLK_SYS_SELECTED_PERI_FIELD_VALUE 2u
/** Minimum VCO frequency of the PLL */
#define PLL_VCO_FREQ_MIN MHZ(750u)
/** Maximum VCO frequency of the PLL */
#define PLL_VCO_FREQ_MAX MHZ(1600u)
/** RIOT core clock frequency (125 MHz), derived from PLL parameters */
#define CLOCK_CORECLOCK (PLL_VCO_FREQ / (PLL_PD1 * PLL_PD2))
/** CLK_REF integer divider value */
#define CLK_REF_DIV_VALUE 1u
/** Clock frequency of CLK_REF, which is fed by the XOSC */
#define CLOCK_REF (CLOCK_XOSC / CLK_REF_DIV_VALUE)
/** Number of CLK_REF cycles per tick of the tick generators (1 µs ticks) */
#define TICK_CYCLES (CLOCK_REF / MHZ(1u))

#if (PLL_FEEDBACK_DIVIDER_VALUE < PLL_VCO_FEEDBACK_SCALE_MIN) || \
    (PLL_FEEDBACK_DIVIDER_VALUE > PLL_VCO_FEEDBACK_SCALE_MAX)
#  error "Value for PLL_FEEDBACK_DIVIDER_VALUE out of range, check config"
#endif
#if (PLL_REF_DIV < PLL_REF_DIV_MIN) || (PLL_REF_DIV > PLL_REF_DIV_MAX)
#  error "Value for PLL_REF_DIV out of range, check config"
#endif
#if (PLL_PD1 < PLL_POSTDIV_MIN) || (PLL_PD1 > PLL_POSTDIV_MAX)
#  error "Value for PLL_PD1 out of range, check config"
#endif
#if (PLL_PD2 < PLL_POSTDIV_MIN) || (PLL_PD2 > PLL_POSTDIV_MAX)
#  error "Value for PLL_PD2 out of range, check config"
#endif
#if ((CLOCK_XOSC > CLOCK_XOSC_MAX) || (CLOCK_XOSC < CLOCK_XOSC_MIN))
#  error "Value for CLOCK_XOSC out of range, check config"
#endif
#if ((PLL_VCO_FREQ > PLL_VCO_FREQ_MAX) || (PLL_VCO_FREQ < PLL_VCO_FREQ_MIN))
#  error "Value for PLL_VCO_FREQ out of range, check config"
#endif
#if (CLOCK_REF % MHZ(1u))
#  error "CLOCK_REF must be a multiple of 1 MHz for 1 µs ticks"
#endif

/** Post divider for the PLL, calculated based on the post divider values */
#define PDIV ((PLL_PD1 << PLL_PRIM_POSTDIV1_LSB) | (PLL_PD2 << PLL_PRIM_POSTDIV2_LSB))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configures the Crystal to run.
 * @note    The reference hardware manual suggests to use a 12 MHz crystal,
 *          which we use by default.
 */
void xosc_start(void);

/**
 * @brief   Stop the crystal.
 */
void xosc_stop(void);

/**
 * @brief   Sleep for a given time in milliseconds.
 * @param   milliseconds  The time to sleep in milliseconds.
 */
void xosc_sleep(uint32_t milliseconds);

/**
 * @brief   Reset the clock system.
 *
 * This function resets the clock system to a known state.
 * It is recommended to call this function before configuring the clock system.
 */
void clock_reset(void);

/**
 * @brief Configures the XOSC and then sets CLK_SYS, PLL_SYS and CLK_PERI to it
 * @pre Make sure to call clock_reset() before this function to reset the
 *      clock system.
 * @see RP2350 Docs Chapter 8, mostly 8.2 for more details
 */
void cpu_clock_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
