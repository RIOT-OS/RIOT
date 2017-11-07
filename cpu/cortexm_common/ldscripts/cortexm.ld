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
 * @brief           Memory definitions for the cortexm family
 *
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

boot_offset = DEFINED( rom_offset ) ? rom_offset : 0x0 ;
ccmram_length = DEFINED( ccmram_len ) ? ccmram_len : 0x0 ;
ccmram_start_addr = DEFINED( ccmram_addr ) ? ccmram_addr : 0x10000000 ;

MEMORY
{
    rom (rx)    : ORIGIN = rom_start_addr + boot_offset, LENGTH = rom_length - boot_offset
    ram (rwx)   : ORIGIN = ram_start_addr,               LENGTH = ram_length
    ccmram (rwx): ORIGIN = ccmram_start_addr,            LENGTH = ccmram_length
}

INCLUDE cortexm_base.ld
