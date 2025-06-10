#pragma once

#include "RP2350.h"

// Based on hardware/regs/xosc.h and 8.2.8
#define XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ    0xaa0u
#define XOSC_CTRL_FREQ_RANGE_VALUE_10_30MHZ     0xaa1u
#define XOSC_CTRL_FREQ_RANGE_VALUE_25_60MHZ    0xaa2u
#define XOSC_CTRL_FREQ_RANGE_VALUE_40_100MHZ    0xaa3u
#define XOSC_CTRL_ENABLE_VALUE_DISABLE 0xd1eu
#define XOSC_CTRL_ENABLE_VALUE_ENABLE 0xfabu
#define XOSC_CTRL_ENABLE_LSB    12u
#define XOSC_STATUS_STABLE_BITS   0x80000000u
#define XOSC_HZ MHZ(12)
#define REF_DIV 2
#define VCO_FREQ 750000000u
#define PD1 6
#define PD2 2
#define PLL_PWR_PD_BITS 0x00000001u
#define PLL_PWR_VCOPD_BITS 0x00000020u
#define PLL_CS_LOCK_BITS 0x80000000u
#define PLL_PRIM_POSTDIV1_LSB 16u
#define PLL_PRIM_POSTDIV2_LSB 12u
#define PLL_PWR_POSTDIVPD_BITS   0x00000008u
#define CLK_PERI_CTRL_ENABLE_BIT 1 << 11
#define XOSC_FREQ 12000000
#define POSTDIV1 6
#define POSTDIV2 2
#define CPUFREQ 125000000
#define CLOCK_XOSC_MAX              MHZ(15) /**< Maximum crystal frequency */
#define CLOCK_XOSC_MIN              MHZ(5)  /**< Minimum crystal frequency */
#define CLOCK_XOSC              (XOSC_HZ) /**< Crystal frequency */
#define PLL_POSTDIV_MIN             (1U)    /**< Minimum value of the post PLL clock divers */
#define PLL_POSTDIV_MAX             (7U)    /**< Maximum value of the post PLL clock divers */
#define PLL_VCO_FEEDBACK_SCALE_MIN  (16U)   /**< Minimum value of the PLL VCO feedback scaler */
#define PLL_VCO_FEEDBACK_SCALE_MAX  (320U)  /**< Maximum value of the PLL VCO feedback scaler */
#define PLL_REF_DIV_MIN             (1U)    /**< Minimum value of the clock divider applied before
                                             *   feeding in the reference clock into the PLL */
#define PLL_REF_DIV_MAX             (1U)    /**< Minimum value of the clock divider applied before
                                             *   feeding in the reference clock into the PLL */

#if (PLL_VCO_FEEDBACK_SCALE_MIN < PLL_VCO_FEEDBACK_SCALE_MIN) || (PLL_VCO_FEEDBACK_SCALE_MAX > PLL_VCO_FEEDBACK_SCALE_MAX)
#error "Value for PLL_VCO_FEEDBACK_SCALE out of range, check config"
#endif
#if (PLL_REF_DIV_MIN < PLL_REF_DIV_MIN) || (PLL_REF_DIV_MAX > PLL_REF_DIV_MAX)
#error "Value for PLLxosc_sleep_REF_DIV out of range, check config"
#endif
#if (PLL_POSTDIV_MIN < PLL_POSTDIV_MIN) || (PLL_POSTDIV_MAX > PLL_POSTDIV_MAX)
#error "Value for PLL_POSTDIV out of range, check config"
#endif
#if ((CLOCK_XOSC > CLOCK_XOSC_MAX) || (CLOCK_XOSC < CLOCK_XOSC_MIN))
#error "Value for CLOCK_XOSC out of range, check config"
#endif

#define PDIV ((PD1 << PLL_PRIM_POSTDIV1_LSB) | (PD2 << PLL_PRIM_POSTDIV2_LSB))
#define FBDIV ((VCO_FREQ / XOSC_HZ) / REF_DIV)


/**
 * @brief   Configures the Crystal to run.
 *
 * @param   f_ref       Desired frequency in Hz
 *
 * @pre     1 MHz <= @p f_ref <= 15 MHz.
 *
 * The reference hardware manual suggests to use a 12 MHz crystal.
 */
void xosc_start(void);

/**
 * @brief   Stop the crystal.
 */
void xosc_stop(void);

/**
 * @brief   Sleep for a given number of cycles.
 */
void xosc_sleep(int32_t cycles);

/**
 * @brief   Reset the clock system.
 *
 * This function resets the clock system to a known state.
 * It is recommended to call this function before configuring the clock system.
 */
void clock_reset(void);

/**
 * @brief Configures the XOSC and then sets CLK_SYS, PLL_SYS and CLK_PERI to it
 * @warning Make sure to call clock_reset() before this function to reset the clock system
 * @note RP2350 Docs Chapter 8, mostly 8.2 for more details
*/
void cpu_clock_init(void);
