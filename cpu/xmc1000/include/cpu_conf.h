/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_xmc1000 Infineon XMC1000
 * @brief           Infineon XMC1000 series specific code.
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 */

#ifndef CPU_CONF_H_
#define CPU_CONF_H_

#ifdef CPU_MODEL_XMC1100
#include "xmc1100.h"
#elif CPU_MODEL_XMC1300
#include "xmc1300.h"
#else
#warning "No cpu model defined."
#endif  /* CPU_MODEL_XMC1100 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup cpu_xmc1000_const CPU constants
 * @ingroup  cpu_xmc1000
 * @brief    No need to change these.
 * @{
 */

/**
 * @brief Length of the CPU_ID
 * @{
 */
#define CPUID_ID_LEN                    (16)
/** @} */

/**
 * @brief Default IRQ priority
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
/** @} */

/**
 * @brief Overall number of IRQs
 * @{
 */
#define CPU_IRQ_NUMOF                   (24U)
/** @} */

/**
 * @brief Frequency of the DCO1 oscillator
 * @{
 */
#define DCO1_FREQUENCY                  (64000000U)
/** @} */

/**
 * @brief Frequency of the DCO2 standby oscillator (RTC, WDT)
 * @{
 */
#define DCO2_FREQUENCY                  (32786U)
/** @} */
/** @} */

/**
 * @defgroup        cpu_xmc1000_clock Clock settings
 * @ingroup         cpu_xmc1000
 * @brief           Clock divider and peripheral clock configuration.
 * @details
 *
 * The master clock frequency is:
 *  MCLK = DC01_FREQUENCY / (2 * (XMC_CLK_IDIV + (XMC_CLK_FDIV / 256)))
 *
 * @{
 */

/**
 * @brief Fractional divider selection.
 * @details
 *
 * Sets the fractional divider to n/256. For example, setting a value
 * of 128 sets the fractional divider to 1/2. A value of 0 disables
 * the fractional divider.
 * @remark Range: [0, 255]
 * @remark has no effect if XMC_CLK_IDIV is 0.
 * @remark 0 is the default value used.
 * @{
 */
#ifndef XMC_CLK_FDIV
#define XMC_CLK_FDIV (0)
#endif
/** @} */

/**
 * @brief Divider selection.
 * @details
 *
 * Sets the (integral) divider. A value of 0 bypasses the divider
 * completely.
 *
 * IDIV  | MCLK
 * ----- | ----------:
 * 1     | 32 Mhz
 * 2     | 16 Mhz
 * 3     | 10.67 Mhz
 * 4     | 8 Mhz
 * 5     | 6.4 Mhz
 * 16    | 2 Mhz
 * 254   | 126 kHz
 * 255   | 125.5 kHz
 *
 * @remark Range: [0, 255]
 * @remark Values between 0 and 16 will be set by the SSW.
 * @remark Values > 16 will be set by RIOT during startup..
 * @remark 1 is the default value used.
 * @{
 */
#ifndef XMC_CLK_IDIV
#define XMC_CLK_IDIV (1)
#endif
/** @} */

/**
 * @brief Fast peripheral clock selection.
 * @details
 *
 * Sets the fast peripheral clock:
 *
 * PCLKSEL  | PCLK
 * -------- | -------:
 * 0        | MCLK
 * 1        | 2 x MCLK
 *
 * @remark 1 is the default value used.
 * @{
 */
#ifndef XMC_CLK_PCLKSEL
#define XMC_CLK_PCLKSEL (1)
#endif
/** @} */

/**
 * @brief Disable the clock handling by SSW
 * @details
 *
 * Setting this to 1 prevents the Start-up Software (SSW) to set the
 * clocks by writing SCU_CLKCR. There should be no reason to use this.
 *
 * @remark 0 is the default value used.
 * @{
 */
#ifndef XMC_CLK_SSW_DISABLED
#define XMC_CLK_SSW_DISABLED (0)
#endif
/** @} */
/** @} */

/**
 * @defgroup        cpu_xmc1000_gating_initial Initial gating configuration
 * @ingroup         cpu_xmc1000
 * @{
 */

/**
 * @brief Clear VADC gating on startup.
 * @{
 */
#ifndef XMC_GATING_CLEAR_VADC
#define XMC_GATING_CLEAR_VADC (0)
#endif
/** @} */

/**
 * @brief Clear CCU40 gating on startup.
 * @{
 */
#ifndef XMC_GATING_CLEAR_CCU40
#define XMC_GATING_CLEAR_CCU40 (0)
#endif
/** @} */

/**
 * @brief Clear USIC0 gating on startup.
 * @{
 */
#ifndef XMC_GATING_CLEAR_USIC0
#define XMC_GATING_CLEAR_USIC0 (0)
#endif
/** @} */

/**
 * @brief Clear WDT gating on startup.
 * @{
 */
#ifndef XMC_GATING_CLEAR_WDT
#define XMC_GATING_CLEAR_WDT (0)
#endif
/** @} */

/**
 * @brief Clear RTC gating on startup.
 * @{
 */
#ifndef XMC_GATING_CLEAR_RTC
#define XMC_GATING_CLEAR_RTC (0)
#endif
/** @} */

/**
 * @brief Disable the gating handling by SSW
 * @details
 *
 * Setting this to 1 prevents the Start-up Software (SSW) to set the
 * inital gating by writing to SCU_CGATSET0. There should be no reason
 * to use this.
 *
 * @remark 1 is the default value used.
 * @{
 */
#ifndef XMC_GATING_SSW_DISABLED
#define XMC_GATING_SSW_DISABLED (1)
#endif
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* CPU_CONF_H_ */
