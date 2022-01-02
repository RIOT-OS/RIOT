/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cortexm_common
 * @{
 *
 * @file
 * @brief           Memory definitions for the Cortex-M family
 *
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

ccmram_length = DEFINED( ccmram_len ) ? ccmram_len : 0x0 ;
sram4_length = DEFINED( sram4_length ) ? sram4_length : 0x0 ;

MEMORY
{
    ccmram  : ORIGIN = 0x10000000, LENGTH = ccmram_length
    sram4   : ORIGIN = 0x28000000, LENGTH = sram4_length
}

SECTIONS
{
    .heap2 ALIGN(4) (NOLOAD) :
    {
        _sheap1 = . ;
        _eheap1 = ORIGIN(sram4) + LENGTH(sram4);
    } > sram4
}

INCLUDE cortexm.ld
