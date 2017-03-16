/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32f4
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32F413ZH
 *
 * @author          Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 1536K
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 256K
    ccmram (rwx): ORIGIN = 0x10000000, LENGTH = 64K
    cpuid (r)   : ORIGIN = 0x1fff7a10, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld
