/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 *
 * @addtogroup      cpu_cc3200
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Attilio Dona'
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "cc3200.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Configuration of default stack sizes
 *
 * @{
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF   (512)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (1024)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE           (256)
#endif
/** @} */

/**
 * @brief   Stack size used for the exception (ISR) stack
 * @{
 */
#ifndef ISR_STACKSIZE
#define ISR_STACKSIZE                   (512U)
#endif
/** @} */



#define CPU_DEFAULT_IRQ_PRIO    (1U) /**< The default priority is 1 for every
 	 	 	 	 	 	 	 	 	 	 interrupt, 0 is the highest possible
 	 	 	 	 	 	 	 	 	 	 priority. */
#define CPU_IRQ_NUMOF           PERIPH_COUNT_IRQn /**< number of interrupt
 	 	 	 	 	 	 	 	 	 	 	 	 	   sources*/
#define CPU_FLASH_BASE          START_OF_RAM /**< code is booted into RAM */

/**
 * @brief   length of CPU ID for @ref cpuid_get() in @ref periph/cpuid.h
 */
#ifndef CPUID_ID_LEN
#define CPUID_ID_LEN                    8
#endif


/**
 * Define the nominal CPU core clock
 */
#define F_CPU               80000000

#define SEC_TO_TICKS(sec)   80000000*sec /**< Convert seconds to  clock ticks */
#define MSEC_TO_TICKS(msec) 80000*msec /**< Convert millisecs to  clock ticks */
#define USEC_TO_TICKS(usec) 80*usec /**< Convert microseconds to  clock ticks */


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* __CPU_CONF_H */
/** @} */
