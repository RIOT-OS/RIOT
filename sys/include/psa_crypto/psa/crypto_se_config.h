/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
 
#pragma once
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
#include "atca.h"
#endif
typedef union {
    #if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
        psa_atca_slot_config_t slots[16];
    #endif
} psa_se_config_t;
 
#ifdef __cplusplus
}
#endif
