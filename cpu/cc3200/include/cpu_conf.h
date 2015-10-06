/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
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
     * @brief   ARM Cortex-M cc3200 specific CPU configuration
     * @{
     */
#define CPU_DEFAULT_IRQ_PRIO            INT_PRIORITY_LVL_1

    /** @} */

    /**
     * @brief   length of CPU ID for @ref cpuid_get() in @ref periph/cpuid.h
     */
#ifndef CPUID_ID_LEN
#define CPUID_ID_LEN                    8
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* __CPU_CONF_H */
/** @} */
