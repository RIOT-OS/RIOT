/*
 * Copyright (C) 2020 Inria
 *               2020 Koen Zandberg <koen@bergzand.net
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_riscv_common
 * @{
 *
 * @file
 * @brief           Linker variables for the RISC-V CPU
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

_itim_start_addr = DEFINED( _itim_start_addr ) ? _itim_start_addr : 0x0;
_itim_length = DEFINED( _itim_length ) ? _itim_length : 0x0;

_rom_offset = DEFINED( _rom_offset ) ? _rom_offset : 0x0;
_slot_aux_len = DEFINED( _slot_aux_len ) ? _slot_aux_len  : 0x0;
_fw_rom_length = DEFINED( _fw_rom_length ) ? _fw_rom_length : _rom_length - _rom_offset - _slot_aux_len;

ASSERT(_fw_rom_length <= _rom_length - _rom_offset - _slot_aux_len, "Specified firmware size does not fit in ROM");
ASSERT(_fw_rom_length <= _rom_length, "Specified firmware size does not fit in ROM");
