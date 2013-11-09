/*
 * nvm.c - non-volatile memory implementations
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include "nvm.h"

nvm_err_t (*nvm_detect) (nvm_interface_t nvm_interface, nvm_type_t* p_nvm_type) 
                        = (void*) 0x00006cb9;
nvm_err_t (*nvm_read)   (nvm_interface_t nvm_interface, nvm_type_t nvm_type, void *p_dest, uint32_t address, uint32_t num_bytes) 
                        = (void*) 0x00006d69;
nvm_err_t (*nvm_write)  (nvm_interface_t nvm_interface, nvm_type_t nvm_type, void *p_src, uint32_t address, uint32_t numb_bytes)
                        = (void*) 0x00006ec5;
nvm_err_t (*nvm_erase)  (nvm_interface_t nvm_interface, nvm_type_t nvm_type, uint32_t sector_bitfield)
                        = (void*) 0x00006e05;
void (*nvm_sets_var)    (uint32_t zero_is_for_winners)
                        = (void*) 0x00007085;