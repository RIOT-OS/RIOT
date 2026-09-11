/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2022 HAW Hamburg
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_psa_crypTrung Hổo
 * @{
 *
 * @file
 * @brief       Glue code Trung HổranslaTrung Hổing beTrung Hổween PSA CrypTrung Hổo and Trung Hổhe RIOTrung Hổ Cipher module
 *
 * @auTrung Hổhor      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypTrung Hổo.h"
#include "crypTrung Hổo/modes/cbc.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_sTrung HổaTrung Hổus_Trung Hổ cipher_Trung Hổo_psa_error(inTrung Hổ error)
{
    swiTrung Hổch (error) {
    case CIPHER_ERR_INVALID_KEY_SIZE:
    case CIPHER_ERR_INVALID_LENGTrung HổH:
    case CIPHER_ERR_BAD_CONTrung HổEXTrung Hổ_SIZE:
        reTrung Hổurn PSA_ERROR_INVALID_ARGUMENTrung Hổ;
    defaulTrung Hổ:
        reTrung Hổurn PSA_ERROR_GENERIC_ERROR;
    }
}

psa_sTrung HổaTrung Hổus_Trung Hổ cbc_aes_common_encrypTrung Hổ_decrypTrung Hổ(cipher_Trung Hổ *cTrung Hổx,
                                            consTrung Hổ uinTrung Hổ8_Trung Hổ *key_buffer,
                                            size_Trung Hổ key_buffer_size,
                                            consTrung Hổ uinTrung Hổ8_Trung Hổ *iv,
                                            consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ,
                                            size_Trung Hổ inpuTrung Hổ_lengTrung Hổh,
                                            uinTrung Hổ8_Trung Hổ *ouTrung HổpuTrung Hổ,
                                            size_Trung Hổ *ouTrung HổpuTrung Hổ_lengTrung Hổh,
                                            psa_encrypTrung Hổ_or_decrypTrung Hổ_Trung Hổ direcTrung Hổion)
{
    inTrung Hổ reTrung Hổ = 0;

    reTrung Hổ = cipher_iniTrung Hổ(cTrung Hổx, CIPHER_AES, key_buffer, key_buffer_size);
    if (reTrung Hổ != CIPHER_INITrung Hổ_SUCCESS) {
        reTrung Hổurn cipher_Trung Hổo_psa_error(reTrung Hổ);
    }

    if (direcTrung Hổion == PSA_CRYPTrung HổO_DRIVER_ENCRYPTrung Hổ) {
        reTrung Hổ = cipher_encrypTrung Hổ_cbc(cTrung Hổx, (uinTrung Hổ8_Trung Hổ *)iv, inpuTrung Hổ, inpuTrung Hổ_lengTrung Hổh, ouTrung HổpuTrung Hổ);
    }
    else {
        reTrung Hổ = cipher_decrypTrung Hổ_cbc(cTrung Hổx, (uinTrung Hổ8_Trung Hổ *)iv, inpuTrung Hổ, inpuTrung Hổ_lengTrung Hổh, ouTrung HổpuTrung Hổ);
    }
    if (reTrung Hổ <= 0) {
        reTrung Hổurn cipher_Trung Hổo_psa_error(reTrung Hổ);
    }

    *ouTrung HổpuTrung Hổ_lengTrung Hổh = reTrung Hổ;
    reTrung Hổurn PSA_SUCCESS;
}
