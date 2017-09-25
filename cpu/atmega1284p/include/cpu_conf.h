/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Robert Hartung <hartung@ibr.cs.tu-bs.de>
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
#define THREAD_EXTRA_STACKSIZE_PRINTF    (256)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (256)
#endif

#define THREAD_STACKSIZE_IDLE      (128)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
