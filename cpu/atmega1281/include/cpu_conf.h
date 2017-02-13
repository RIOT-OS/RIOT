/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
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
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
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
 * size tested sucessfully even with pretty small stacks.
 * @{
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF    (128)

#ifndef THREAD_STACKSIZE_DEFAULT
#   define THREAD_STACKSIZE_DEFAULT   (256)
#endif

#ifndef THREAD_STACKSIZE_IDLE
#   define THREAD_STACKSIZE_IDLE      (128)
#endif

#ifndef ISR_STACKSIZE
#   define ISR_STACKSIZE              (0)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
