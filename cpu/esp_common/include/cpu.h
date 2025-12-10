/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       CPU common functions for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

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

/** @} */
