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
 * @brief           Rom offset and firmware size calculations
 *
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 *                  Gaëtan Harter <gaetan.harter@inria.fr>
 *
 * @}
 */

_rom_offset = DEFINED( _rom_offset ) ? _rom_offset : 0x0;
_fw_rom_length = DEFINED( _fw_rom_length ) ? _fw_rom_length : _rom_length - _rom_offset;

ASSERT((_fw_rom_length <= _rom_length - _rom_offset), "Specified firmware size does not fit in ROM");
