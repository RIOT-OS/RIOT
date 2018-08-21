/*
 * Copyright (C) 2017 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author          Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "atmega_regs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Kernel configuration
 *
 * Since printf seems to get memory allocated by the linker/avr-libc the stack
 * size tested sucessfully even with pretty small stacks.k
 * @{
 */

/* keep THREAD_STACKSIZE_IDLE > THREAD_EXTRA_STACKSIZE_PRINTF
 * to avoid not printing of debug in interrupts
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF    (128)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (512)
#endif

#define THREAD_STACKSIZE_IDLE      (129)
#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
