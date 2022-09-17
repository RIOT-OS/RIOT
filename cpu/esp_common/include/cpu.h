/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       CPU common functions for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef CPU_H
#define CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include "irq.h"

#define PROVIDES_PM_SET_LOWEST

/**
 * @brief   Gets the last instruction's address
 *
 * @todo:   Not supported
 */
static inline uintptr_t cpu_get_caller_pc(void)
{
    /* This function must exist else RIOT won't compile */
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
