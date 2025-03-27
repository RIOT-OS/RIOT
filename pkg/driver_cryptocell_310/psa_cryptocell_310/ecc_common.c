/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @brief       Common ECC functions used by all PSA Crypto wrappers
 *              for the CryptoCell 310 ECC APIs.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa_error.h"
#include "psa_cryptocell_310_ecc_common.h"
#include "cryptocell_310_util.h"
#include "string_utils.h"

#define ENABLE_DEBUG    0
#include "debug.h"

extern CRYS_RND_State_t *rndState_ptr;

CRYS_ECPKI_Domain_t *pDomain;
SaSiRndGenerateVectWorkFunc_t rndGenerateVectFunc;

psa_status_t cryptocell_310_common_ecc_generate_key_pair(uint8_t *priv_key_buffer,
                                                 uint8_t *pub_key_buffer,
                                                 uint32_t *priv_key_buffer_length,
                                                 uint32_t *pub_key_buffer_length,
                                                 CRYS_ECPKI_DomainID_t domain)
{
    CRYS_ECPKI_UserPrivKey_t priv_key;
    CRYS_ECPKI_UserPublKey_t pub_key;
    CRYS_ECPKI_KG_FipsContext_t FipsBuff;
    CRYS_ECPKI_KG_TempData_t TempECCKGBuff;
    CRYSError_t ret;

    rndGenerateVectFunc = CRYS_RND_GenerateVector;
    pDomain = (CRYS_ECPKI_Domain_t *)CRYS_ECPKI_GetEcDomain(domain);

    cryptocell_310_enable();
    ret = CRYS_ECPKI_GenKeyPair(rndState_ptr, rndGenerateVectFunc, pDomain, &priv_key, &pub_key,
                                &TempECCKGBuff, &FipsBuff);
    cryptocell_310_disable();
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECPKI_GenKeyPair failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    ret = CRYS_ECPKI_ExportPrivKey(&priv_key, priv_key_buffer, priv_key_buffer_length);
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECPKI_ExportPrivKey failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    ret = CRYS_ECPKI_ExportPublKey(&pub_key, CRYS_EC_PointUncompressed, pub_key_buffer,
                                   pub_key_buffer_length);
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECPKI_ExportPubKey failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    explicit_bzero(&priv_key, sizeof(priv_key));
    return PSA_SUCCESS;
}

psa_status_t cryptocell_310_common_ecc_sign(const uint8_t *priv_key,
                                         uint32_t priv_key_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *signature,
                                         size_t *signature_length,
                                         CRYS_ECPKI_HASH_OpMode_t hash_mode,
                                         CRYS_ECPKI_DomainID_t domain)
{
    CRYS_ECDSA_SignUserContext_t SignUserContext;
    CRYS_ECPKI_UserPrivKey_t user_priv_key;
    CRYSError_t ret = 0;

    if (!cryptocell_310_data_within_ram(priv_key) ||
        !cryptocell_310_data_within_ram(input)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    rndGenerateVectFunc = CRYS_RND_GenerateVector;
    pDomain = (CRYS_ECPKI_Domain_t *)CRYS_ECPKI_GetEcDomain(domain);

    ret = CRYS_ECPKI_BuildPrivKey(pDomain, priv_key, priv_key_size, &user_priv_key);
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECPKI_BuildPrivKey failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        goto done;
    }

    cryptocell_310_enable();
    ret = CRYS_ECDSA_Sign(rndState_ptr, rndGenerateVectFunc,
                          &SignUserContext, &user_priv_key, hash_mode, (uint8_t *)input, input_length,
                          signature, (uint32_t *)signature_length);
    cryptocell_310_disable();
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECDSA_Sign failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        goto done;
    }

done:
    explicit_bzero(&user_priv_key, sizeof(user_priv_key));
    return CRYS_to_psa_error(ret);
}

psa_status_t cryptocell_310_common_ecc_verify(const uint8_t *pub_key,
                                           size_t pub_key_size,
                                           const uint8_t *input,
                                           size_t input_length,
                                           const uint8_t *signature,
                                           size_t signature_length,
                                           CRYS_ECPKI_HASH_OpMode_t hash_mode,
                                           CRYS_ECPKI_DomainID_t domain)
{
    CRYS_ECDSA_VerifyUserContext_t VerifyUserContext;
    CRYS_ECPKI_UserPublKey_t user_pub_key;
    CRYSError_t ret = 0;

    if (!cryptocell_310_data_within_ram(pub_key) ||
        !cryptocell_310_data_within_ram(input) ||
        !cryptocell_310_data_within_ram(signature)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    pDomain = (CRYS_ECPKI_Domain_t *)CRYS_ECPKI_GetEcDomain(domain);

    /**
     * For more security, use CRYS_ECPKI_BuildPublKeyPartlyCheck or
     * CRYS_ECPKI_BuildPublKeyFullCheck -> Those take longer and use more memory space
     * */
    ret = CRYS_ECPKI_BuildPublKey(pDomain, (uint8_t *)pub_key, pub_key_size, &user_pub_key);
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECPKI_BuildPublKey failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    cryptocell_310_enable();
    ret =  CRYS_ECDSA_Verify(&VerifyUserContext, &user_pub_key, hash_mode, (uint8_t *)signature,
                             signature_length, (uint8_t *)input, input_length);
    cryptocell_310_disable();

    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECDSA_Verify failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }
    return PSA_SUCCESS;
}
