/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
