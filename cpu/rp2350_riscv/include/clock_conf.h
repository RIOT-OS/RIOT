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

#include "RP2350.h"
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
/** Reference divider for the PLL, set to 2 as per hardware manual */
#define PLL_REF_DIV 2u
/** VCO frequency for the PLL, set to 750 MHz as per hardware manual */
#define PLL_VCO_FREQ 750000000u
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
/** Default CPU frequency in Hz, set to 125 MHz as per hardware manual */
#define CPUFREQ 125000000u
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
/** Minimum value of the clock divider applied before
 * feeding in the reference clock into the PLL */
#define PLL_REF_DIV_MIN 1u
/** Minimum value of the clock divider applied before feeding in
 *  the reference clock into the PLL */
#define PLL_REF_DIV_MAX 1u
/** PLL feedback divider value, set to 125 as per hardware manual */
#define PLL_FEEDBACK_DIVIDER_VALUE 125u
/** Enable bit for the system clock control register to select the peripheral
 * clock */
#define CLK_SYS_PERI_CTRL_ENABLE_BIT (1u << 0u)
/** Selected field value for the system clock control register
* to select the peripheral clock */
#define CLK_SYS_SELECTED_PERI_FIELD_VALUE 2u
/** RIOT core clock frequency defined as the CPU frequency */
#define CLOCK_CORECLOCK MHZ(12u)

#if (PLL_VCO_FEEDBACK_SCALE_MIN < PLL_VCO_FEEDBACK_SCALE_MIN) || \
(PLL_VCO_FEEDBACK_SCALE_MAX > PLL_VCO_FEEDBACK_SCALE_MAX)
#  error "Value for PLL_VCO_FEEDBACK_SCALE out of range, check config"
#endif
#if (PLL_REF_DIV_MIN < PLL_REF_DIV_MIN) || (PLL_REF_DIV_MAX > PLL_REF_DIV_MAX)
#  error "Value for PLLxosc_sleep_REF_DIV out of range, check config"
#endif
#if (PLL_POSTDIV_MIN < PLL_POSTDIV_MIN) || (PLL_POSTDIV_MAX > PLL_POSTDIV_MAX)
#  error "Value for PLL_POSTDIV out of range, check config"
#endif
#if ((CLOCK_XOSC > CLOCK_XOSC_MAX) || (CLOCK_XOSC < CLOCK_XOSC_MIN))
#  error "Value for CLOCK_XOSC out of range, check config"
#endif

/** Post divider for the PLL, calculated based on the post divider values */
#define PDIV ((PLL_PD1 << PLL_PRIM_POSTDIV1_LSB) | (PLL_PD2 << PLL_PRIM_POSTDIV2_LSB))
/** Feedback divider for the PLL, calculated based on the VCO frequency and
* reference clock frequency */
#define FBDIV ((PLL_VCO_FREQ / XOSC_HZ) / PLL_REF_DIV)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configures the Crystal to run.
 * @note    The reference hardware manual suggests to use a 12 MHz crystal, which we
 *          use by default.
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
 * clock system
 * @see RP2350 Docs Chapter 8, mostly 8.2 for more details
 */
void cpu_clock_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
