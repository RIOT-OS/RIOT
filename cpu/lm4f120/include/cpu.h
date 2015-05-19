/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_LM4F1204
 * @ingroup     cpu
 * @brief       Common implementations and headers for LM4F120XL based micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the LM4F120
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef __LM4F120_H
#define __LM4F120_H

#include "cpu-conf.h"

#ifdef __cplusplus
extern "C" {
#endif

// Return the value of IPSR register - INTERRUPT PROGRAM STATUS REGISTER
extern unsigned long CPUipsrGet(void);
/**
 * @brief Initialization of the CPU
 */
extern void cpu_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __LM4F120_H */
/** @} */
