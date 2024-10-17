/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file
 * @brief       Define structures für SE slot configurations
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_SE_CONFIG_H
#define PSA_CRYPTO_PSA_CRYPTO_SE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
#include "atca.h"
#endif

/**
 * @brief   Structure containing device specific configuration data.
 *
 *          This will be stored in the driver's persistent data to
 *          manage the device.
 */
typedef union {
    #if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
        psa_atca_slot_config_t slots[16];
    #endif
} psa_se_config_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PSA_CRYPTO_PSA_CRYPTO_SE_CONFIG_H */
