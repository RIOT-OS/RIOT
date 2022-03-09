/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2017 RWTH Aachen, Josua Arndt
 *               2018 Matthew Blue
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega_common
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author          Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author          Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author          Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "atmega_regs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_EXTRA_STACKSIZE_PRINTF    (128)

/**
 * @name           Kernel configuration
 *
 *                 Since printf seems to get memory allocated by the
 *                 linker/avr-libc the stack size tested successfully
 *                 even with pretty small stacks.
 * @{
 */
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (512)
#endif

/* keep THREAD_STACKSIZE_IDLE > THREAD_EXTRA_STACKSIZE_PRINTF
 * to avoid not printing of debug in interrupts
 */
#ifndef THREAD_STACKSIZE_IDLE
#if MODULE_XTIMER || MODULE_ZTIMER64
/* xtimer's 64 bit arithmetic doesn't perform well on 8 bit archs. In order to
 * prevent a stack overflow when an timer triggers while the idle thread is
 * running, we have to increase the stack size then
 */
#define THREAD_STACKSIZE_IDLE      (192)
#else
#define THREAD_STACKSIZE_IDLE      (128)
#endif
#endif
/** @} */

/**
 * @brief   Attribute for memory sections required by SRAM PUF
 */
#define PUF_SRAM_ATTRIBUTES __attribute__((used, section(".noinit")))

/**
 * @brief   Declare the heap_stats function as available
 */
#define HAVE_HEAP_STATS

/**
 * @brief   This arch uses the inlined IRQ API.
 */
#define IRQ_API_INLINED     (1)

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
