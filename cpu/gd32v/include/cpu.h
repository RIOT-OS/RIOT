/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_gd32v
 * @{
 *
 * @file
 * @brief       CPU specific definitions
 */

#include "cpu_conf.h"
#include "cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Returns the address of running application in flash
 */
static inline uint32_t cpu_get_image_baseaddr(void)
{
    extern uint8_t _start;
    return (uint32_t)&_start;
}

/**
 * @brief   Starts another image in flash
 */
static inline void cpu_jump_to_image(uint32_t addr)
{
    __asm__ volatile ("jr %0" :: "r" (addr));
}

#ifdef __cplusplus
}
#endif

/** @} */
