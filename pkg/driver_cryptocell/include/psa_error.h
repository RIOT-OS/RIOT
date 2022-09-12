/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_driver_cryptocell
 * @{
 *
 * @file
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 driver APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_ERROR_H
#define PSA_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "crys_hash_error.h"
#include "ssi_aes_error.h"

/**
 * @brief   Convert CryptoCell CRYS errors to PSA status values
 *
 * @param   error CRYSErrot_t
 * @return  @ref psa_status_t
 */
psa_status_t CRYS_to_psa_error(CRYSError_t error);

/**
 * @brief   Convert CryptoCell SaSi errors to PSA status values
 *
 * @param   error  SaSiStatus
 * @return  @ref psa_status_t
 */
psa_status_t SaSi_to_psa_error(SaSiStatus error);

#ifdef __cplusplus
}
#endif

#endif /* PSA_ERROR_H */
/** @} */
