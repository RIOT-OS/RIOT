/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32f1
 * @{
 *
 * @file
 * @brief           OpenCM9.04 specific definitions for the STM32F103CB
 *
 * @author          Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08003000, LENGTH = 128K-0x3000
    ram (xrw)   : ORIGIN = 0x20000000, LENGTH = 20K
    cpuid (r)   : ORIGIN = 0x1ffff7e8, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld
