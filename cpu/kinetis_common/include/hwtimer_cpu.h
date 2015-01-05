/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_common_hwtimer Kinetis hwtimer
 * @ingroup     cpu_kinetis_common
 * @brief       Driver uses Freescale Low Power Timer lptmr0.
 *              There are four clock sources available:
 *
 *                  LPTIMER_CLKSRC_MCGIRCLK - slow or fast internal reference clock
 *                  LPTIMER_CLKSRC_LPO - PMC 1kHz output
 *                  LPTIMER_CLKSRC_ERCLK32K - OSC32KCLK or the RTC clock
 *                  LPTIMER_CLKSRC_OSCERCLK - system oscillator output
 *
 *              Tested clock sources:
 *
 *                  LPO - 1kHz
 *                  RTC - 32768Hz
 *
 *              Possibly, additional settings in System Integration Module are necessary.
 *              Please consult the Reference Manual of your MCU for proper clock settings.
 *
 *              ### LPTMR Configuration Example (for cpu-conf.h) ###
 *
 *                  #define LPTIMER_CLKSRC                   LPTIMER_CLKSRC_LPO
 *                  #define LPTIMER_CLKEN()                  (SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK)
 *                  #define LPTIMER_CLKDIS()                 (SIM->SCGC5 &= ~SIM_SCGC5_PTMR_MASK)
 *                  #define LPTIMER_CNR_NEEDS_LATCHING       1
 *
 *              Optional settings:
 *
 *                  #define LPTIMER_DEV                      LPTMR0
 *                  #define LPTIMER_IRQ_PRIO                 1
 *                  #define LPTIMER_IRQ_CHAN                 LPTMR0_IRQn
 * @{

 * @file
 * @brief       Interface definition for the Kinetis hwtimer driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#include "cpu-conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock settings for the lptmr timer
 */
#define LPTIMER_CLKSRC_MCGIRCLK          0    /**< internal reference clock (4MHz) */
#define LPTIMER_CLKSRC_LPO               1    /**< PMC 1kHz output */
#define LPTIMER_CLKSRC_ERCLK32K          2    /**< RTC clock 32768Hz */
#define LPTIMER_CLKSRC_OSCERCLK          3    /**< system oscillator output, clock from RF-Part */

#ifndef LPTIMER_CLKSRC
#define LPTIMER_CLKSRC                   LPTIMER_CLKSRC_LPO    /**< default clock source */
#endif

#if (LPTIMER_CLKSRC == LPTIMER_CLKSRC_MCGIRCLK)
#define LPTIMER_CLK_PRESCALE    1
#define LPTIMER_SPEED           1000000
#elif (LPTIMER_CLKSRC == LPTIMER_CLKSRC_OSCERCLK)
#define LPTIMER_CLK_PRESCALE    1
#define LPTIMER_SPEED           1000000
#elif (LPTIMER_CLKSRC == LPTIMER_CLKSRC_ERCLK32K)
#define LPTIMER_CLK_PRESCALE    0
#define LPTIMER_SPEED           32768
#else
#define LPTIMER_CLK_PRESCALE    0
#define LPTIMER_SPEED           1000
#endif

#ifndef LPTIMER_DEV
/** default Low Power Timer device */
#define LPTIMER_DEV             LPTMR0
#endif

#ifndef LPTIMER_IRQ_PRIO
/** IRQ priority for hwtimer interrupts */
#define LPTIMER_IRQ_PRIO        1
#endif

#ifndef LPTIMER_IRQ_CHAN
/** IRQ channel for hwtimer interrupts */
#define LPTIMER_IRQ_CHAN        LPTMR0_IRQn
#endif

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS    1                /**< the CPU implementation supports 1 HW timer */
#define HWTIMER_MAXTICKS     (0xFFFFFFFF)     /**< simulating 32-bit timer behavior */
#define HWTIMER_SPEED        LPTIMER_SPEED    /**< speed depends on clock source and prescale */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
