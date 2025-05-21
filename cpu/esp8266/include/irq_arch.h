/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */


#include "irq_arch_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Software interrupt types
 *
 * These definitions are used to distinguish different types of software
 * interrupts in software interrupt handler.
 */
#define ETS_SOFT_INT_NONE       0
#define ETS_SOFT_INT_YIELD      1
#define ETS_SOFT_INT_HDL_MAC    2

#ifdef __cplusplus
}
#endif
