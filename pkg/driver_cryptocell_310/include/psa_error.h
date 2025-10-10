/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 driver APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "crys_ecpki_error.h"
#include "crys_ec_mont_edw_error.h"
#include "crys_hash_error.h"
#include "ssi_aes_error.h"
#include "crys_chacha_error.h"

/**
 * @brief   Convert CryptoCell CRYS errors to PSA status values
 *
 * @param   error   Error value of type @c CRYSError_t
 * @return  @ref psa_status_t
 */
psa_status_t CRYS_to_psa_error(CRYSError_t error);

/**
 * @brief   Convert CryptoCell SaSi errors to PSA status values
 *
 * @param   error  Error value of type @c SaSiStatus
 * @return  @ref psa_status_t
 */
psa_status_t SaSi_to_psa_error(SaSiStatus error);

/**
 * @brief   Function to print CryptoCell Error values in clear text.
 *
 * @param   status  Error value of type @c SaSiStatus or @c CRYSError_t
 * @return  const char*
 */
const char *cryptocell310_status_to_humanly_readable(uint32_t status);

#ifdef __cplusplus
}
#endif

/** @} */
