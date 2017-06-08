/*
* Copyright (C) 2015 Freie Universität Berlin
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

/**
* @addtogroup      cpu_efm32lg
* @{
*
* @file
* @brief           Memory definitions for the efm32lg380f128 CPU
*
* @}
*/

MEMORY
{
rom (rx)    : ORIGIN = 0x00000000, LENGTH = 131072
ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 32768
}

INCLUDE cortexm_base.ld