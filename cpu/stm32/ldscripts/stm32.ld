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

ccmram_length = DEFINED( _ccmram_length ) ? _ccmram_length : 0x0 ;

sram4_addr = DEFINED( _sram4_length ) ? 0x28000000 : 0x0 ;
sram4_length = DEFINED( _sram4_length ) ? _sram4_length : 0x0 ;

fmc_ram_addr = DEFINED( _fmc_ram_addr ) ? _fmc_ram_addr : 0x0 ;
fmc_ram_length = DEFINED( _fmc_ram_length ) ? _fmc_ram_length : 0x0 ;

MEMORY
{
    ccmram  : ORIGIN = 0x10000000, LENGTH = ccmram_length
    sram4   : ORIGIN = sram4_addr, LENGTH = sram4_length
    fmcram  : ORIGIN = fmc_ram_addr, LENGTH = fmc_ram_length
}

SECTIONS
{
    .heap2 ALIGN(4) (NOLOAD) :
    {
        _sheap2 = ORIGIN(sram4);
        _eheap2 = ORIGIN(sram4) + LENGTH(sram4);
    } > sram4

    .heap4 ALIGN(4) (NOLOAD) :
    {
        _sheap3 = ORIGIN(fmcram);
        _eheap3 = ORIGIN(fmcram) + LENGTH(fmcram);
    } > fmcram
}

INCLUDE cortexm.ld
