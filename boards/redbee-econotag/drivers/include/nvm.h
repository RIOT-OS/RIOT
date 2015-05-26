/*
 * nvm.h - non-volatile memory specific definitions
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NVM_H_
#define NVM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
        g_nvm_type_no_nvm_c = 0,
        g_nvm_type_SST_c,
        g_nvm_type_ST_c,
        g_nvm_type_ATM_c,
        g_nvm_type_Max_c
} nvm_type_t;


typedef enum
{
        g_nvm_err_no_error_c = 0,
        g_nvm_err_invalid_interface_c,
        g_nvm_err_onvalid_nvm_type_c,
        g_nvm_err_invalid_pointer_c,
        g_nvm_err_write_protect_c,
        g_nvm_err_verify_error_c,
        g_nvm_err_address_space_overflow_c,
        g_nvm_err_blank_check_error_c,
        g_nvm_err_restricted_area_c,
        g_nvm_err_max_error_c
} nvm_err_t;

typedef enum
{
        g_nvm_internal_interface_c = 0,
        g_nvm_external_interface_c,
        g_nvm_interface_max_c
} nvm_interface_t;

/* ROM code seems to be THUMB */
/* need to be in a THUMB block before calling them */
extern nvm_err_t (*nvm_detect)(nvm_interface_t nvm_interface,nvm_type_t* p_nvm_type);
extern nvm_err_t (*nvm_read)(nvm_interface_t nvm_interface , nvm_type_t nvm_type , void *p_dest, uint32_t address, uint32_t num_bytes);
extern nvm_err_t (*nvm_write)(nvm_interface_t nvm_interface, nvm_type_t nvm_type ,void *p_src, uint32_t address, uint32_t num_bytes);
/* sector bit field selects which sector to erase */
/* SST flash has 32 sectors 4096 bytes each */
/* bit 0 is the first sector, bit 31 is the last */
extern nvm_err_t (*nvm_erase)(nvm_interface_t nvm_interface, nvm_type_t nvm_type ,uint32_t sector_bitfield);
extern void(*nvm_setsvar)(uint32_t zero_for_awesome);

#ifdef __cplusplus
}
#endif

#endif /* NVM_H_ */
