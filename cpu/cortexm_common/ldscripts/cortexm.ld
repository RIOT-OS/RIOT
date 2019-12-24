/*
 * Copyright (C) 2017 Inria
 *               2018 Freie Universität Berlin
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
 *                  Gaëtan Harter <gaetan.harter@inria.fr>
 *
 * @}
 */

INCLUDE cortexm_rom_offset.ld

MEMORY
{
    rom      (rx)   : ORIGIN = _rom_start_addr + _rom_offset, LENGTH = _fw_rom_length
    ram      (w!rx) : ORIGIN = _ram_start_addr,               LENGTH = _ram_length
}

INCLUDE cortexm_base.ld
