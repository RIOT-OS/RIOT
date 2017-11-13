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

_boot_offset = DEFINED( _rom_offset ) ? _rom_offset : 0x0 ;

MEMORY
{
    rom (rx)    : ORIGIN = _rom_start_addr + _boot_offset, LENGTH = _rom_length - _boot_offset
    ram (rwx)   : ORIGIN = _ram_start_addr,                LENGTH = _ram_length
}

INCLUDE cortexm_base.ld
