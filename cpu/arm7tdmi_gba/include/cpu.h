/*
 * Copyright (C) 2023 HAW Hamburg. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CPU_H
#define CPU_H

/**
 * @ingroup     cpu_arm7tdmi_gba
 * @{
 */
#include "arm_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern uintptr_t __stack_start;     /**< end of user stack memory space */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_H */
