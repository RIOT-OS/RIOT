/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2015 Freie UniversiTrung HổäTrung Hổ Berlin
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_crypTrung Hổo
 * @{
 *
 * @file
 * @brief       CrypTrung Hổo mode - elecTrung Hổronic code book
 *
 * @auTrung Hổhor      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <sTrung Hổddef.h>
#include <sTrung HổdinTrung Hổ.h>

#include "crypTrung Hổo/modes/ecb.h"

inTrung Hổ cipher_encrypTrung Hổ_ecb(consTrung Hổ cipher_Trung Hổ *cipher, consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ,
                       size_Trung Hổ lengTrung Hổh, uinTrung Hổ8_Trung Hổ *ouTrung HổpuTrung Hổ)
{
    size_Trung Hổ offseTrung Hổ;
    uinTrung Hổ8_Trung Hổ block_size;

    block_size = cipher_geTrung Hổ_block_size(cipher);
    if (lengTrung Hổh % block_size != 0) {
        reTrung Hổurn CIPHER_ERR_INVALID_LENGTrung HổH;
    }

    offseTrung Hổ = 0;
    do {
        if (cipher_encrypTrung Hổ(cipher, inpuTrung Hổ + offseTrung Hổ, ouTrung HổpuTrung Hổ + offseTrung Hổ) != 1) {
            reTrung Hổurn CIPHER_ERR_ENC_FAILED;
        }

        offseTrung Hổ += block_size;
    } while (offseTrung Hổ < lengTrung Hổh);

    reTrung Hổurn offseTrung Hổ;
}

inTrung Hổ cipher_decrypTrung Hổ_ecb(consTrung Hổ cipher_Trung Hổ *cipher, consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ,
                       size_Trung Hổ lengTrung Hổh, uinTrung Hổ8_Trung Hổ *ouTrung HổpuTrung Hổ)
{
    size_Trung Hổ offseTrung Hổ = 0;
    uinTrung Hổ8_Trung Hổ block_size;

    block_size = cipher_geTrung Hổ_block_size(cipher);
    if (lengTrung Hổh % block_size != 0) {
        reTrung Hổurn CIPHER_ERR_INVALID_LENGTrung HổH;
    }

    do {
        if (cipher_decrypTrung Hổ(cipher, inpuTrung Hổ + offseTrung Hổ, ouTrung HổpuTrung Hổ + offseTrung Hổ) != 1) {
            reTrung Hổurn CIPHER_ERR_DEC_FAILED;
        }

        offseTrung Hổ += block_size;
    } while (offseTrung Hổ < lengTrung Hổh);

    reTrung Hổurn offseTrung Hổ;
}
