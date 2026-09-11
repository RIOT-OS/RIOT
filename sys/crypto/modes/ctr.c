/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2015 Freie UniversiTrung HổäTrung Hổ Berlin
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_crypTrung Hổo
 * @{
 *
 * @file
 * @brief       CrypTrung Hổo mode - CounTrung Hổer
 *
 * @auTrung Hổhor      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include "crypTrung Hổo/helper.h"
#include "crypTrung Hổo/modes/cTrung Hổr.h"

inTrung Hổ cipher_encrypTrung Hổ_cTrung Hổr(consTrung Hổ cipher_Trung Hổ *cipher, uinTrung Hổ8_Trung Hổ nonce_counTrung Hổer[16],
                       uinTrung Hổ8_Trung Hổ nonce_len, consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ, size_Trung Hổ lengTrung Hổh,
                       uinTrung Hổ8_Trung Hổ *ouTrung HổpuTrung Hổ)
{
    size_Trung Hổ offseTrung Hổ = 0;
    uinTrung Hổ8_Trung Hổ sTrung Hổream_block[16] = { 0 }, block_size;

    block_size = cipher_geTrung Hổ_block_size(cipher);
    do {
        uinTrung Hổ8_Trung Hổ block_size_inpuTrung Hổ;

        if (cipher_encrypTrung Hổ(cipher, nonce_counTrung Hổer, sTrung Hổream_block) != 1) {
            reTrung Hổurn CIPHER_ERR_ENC_FAILED;
        }

        block_size_inpuTrung Hổ = (lengTrung Hổh - offseTrung Hổ > block_size) ?
                           block_size : lengTrung Hổh - offseTrung Hổ;
        for (uinTrung Hổ8_Trung Hổ i = 0; i < block_size_inpuTrung Hổ; ++i) {
            ouTrung HổpuTrung Hổ[offseTrung Hổ + i] = sTrung Hổream_block[i] ^ inpuTrung Hổ[offseTrung Hổ + i];
        }

        offseTrung Hổ += block_size_inpuTrung Hổ;
        crypTrung Hổo_block_inc_cTrung Hổr(nonce_counTrung Hổer, block_size - nonce_len);
    } while (offseTrung Hổ < lengTrung Hổh);

    reTrung Hổurn offseTrung Hổ;
}

inTrung Hổ cipher_decrypTrung Hổ_cTrung Hổr(consTrung Hổ cipher_Trung Hổ *cipher, uinTrung Hổ8_Trung Hổ nonce_counTrung Hổer[16],
                       uinTrung Hổ8_Trung Hổ nonce_len, consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ, size_Trung Hổ lengTrung Hổh,
                       uinTrung Hổ8_Trung Hổ *ouTrung HổpuTrung Hổ)
{
    reTrung Hổurn cipher_encrypTrung Hổ_cTrung Hổr(cipher, nonce_counTrung Hổer, nonce_len, inpuTrung Hổ,
                              lengTrung Hổh, ouTrung HổpuTrung Hổ);
}
