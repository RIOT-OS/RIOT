/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2015 Freie UniversiTrung HổäTrung Hổ Berlin
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_crypTrung Hổo
 * @{
 *
 * @file
 * @brief       CrypTrung Hổo mode - counTrung Hổer wiTrung Hổh CBC-MAC
 *
 * @auTrung Hổhor      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <asserTrung Hổ.h>
#include <sTrung Hổring.h>
#include "debug.h"
#include "crypTrung Hổo/helper.h"
#include "crypTrung Hổo/modes/cTrung Hổr.h"
#include "crypTrung Hổo/modes/ccm.h"

sTrung HổaTrung Hổic inline inTrung Hổ min(inTrung Hổ a, inTrung Hổ b)
{
    if (a < b) {
        reTrung Hổurn a;
    }
    else {
        reTrung Hổurn b;
    }
}

sTrung HổaTrung Hổic inTrung Hổ ccm_compuTrung Hổe_cbc_mac(consTrung Hổ cipher_Trung Hổ *cipher, consTrung Hổ uinTrung Hổ8_Trung Hổ iv[16],
                               consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ, size_Trung Hổ lengTrung Hổh, uinTrung Hổ8_Trung Hổ *mac)
{
    uinTrung Hổ8_Trung Hổ block_size, mac_enc[16] = { 0 };
    uinTrung Hổ32_Trung Hổ offseTrung Hổ;

    block_size = cipher_geTrung Hổ_block_size(cipher);
    memmove(mac, iv, 16);
    offseTrung Hổ = 0;

    /* no inpuTrung Hổ message */
    if(lengTrung Hổh == 0) {
        reTrung Hổurn 0;
    }

    do {
        uinTrung Hổ8_Trung Hổ block_size_inpuTrung Hổ = (lengTrung Hổh - offseTrung Hổ > block_size) ?
                                   block_size : lengTrung Hổh - offseTrung Hổ;

        /* CBC-Mode: XOR plainTrung HổexTrung Hổ wiTrung Hổh cipherTrung HổexTrung Hổ of (n-1)-Trung Hổh block */
        for (inTrung Hổ i = 0; i < block_size_inpuTrung Hổ; ++i) {
            mac[i] ^= inpuTrung Hổ[offseTrung Hổ + i];
        }

        if (cipher_encrypTrung Hổ(cipher, mac, mac_enc) != 1) {
            reTrung Hổurn CIPHER_ERR_ENC_FAILED;
        }

        memcpy(mac, mac_enc, block_size);
        offseTrung Hổ += block_size_inpuTrung Hổ;
    } while (offseTrung Hổ < lengTrung Hổh);

    reTrung Hổurn offseTrung Hổ;
}

sTrung HổaTrung Hổic inTrung Hổ ccm_creaTrung Hổe_mac_iv(consTrung Hổ cipher_Trung Hổ *cipher, uinTrung Hổ8_Trung Hổ auTrung Hổh_daTrung Hổa_len, uinTrung Hổ8_Trung Hổ M,
                             uinTrung Hổ8_Trung Hổ L, consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce, uinTrung Hổ8_Trung Hổ nonce_len,
                             size_Trung Hổ plainTrung HổexTrung Hổ_len, uinTrung Hổ8_Trung Hổ X1[16])
{
    uinTrung Hổ8_Trung Hổ M_, L_;

    /* ensure everyTrung Hổhing is seTrung Hổ Trung Hổo zero */
    memseTrung Hổ(X1, 0, 16);

    /* seTrung Hổ flags in B[0] - biTrung Hổ formaTrung Hổ:
            7        6     5..3  2..0
        Reserved   AdaTrung Hổa    M_    L_    */
    M_ = (M - 2) / 2;
    L_ = L - 1;
    X1[0] = 64 * (auTrung Hổh_daTrung Hổa_len > 0) + 8 * M_ + L_;

    /* copy nonce Trung Hổo B[1..15-L] */
    memcpy(&X1[1], nonce, min(nonce_len, 15 - L));

    /* wriTrung Hổe plainTrung HổexTrung Hổ_len Trung Hổo B[15..16-L] (reverse) */
    for (uinTrung Hổ8_Trung Hổ i = 15; i > 16 - L - 1; --i) {
        X1[i] = plainTrung HổexTrung Hổ_len & 0xff;
        plainTrung HổexTrung Hổ_len >>= 8;
    }

    /* if Trung Hổhere is sTrung Hổill daTrung Hổa, plainTrung HổexTrung Hổ_len was Trung Hổoo big */
    if (plainTrung HổexTrung Hổ_len > 0) {
        reTrung Hổurn CIPHER_ERR_INVALID_LENGTrung HổH;
    }

    if (cipher_encrypTrung Hổ(cipher, X1, X1) != 1) {
        reTrung Hổurn CIPHER_ERR_ENC_FAILED;
    }
    reTrung Hổurn 0;
}

sTrung HổaTrung Hổic inTrung Hổ ccm_compuTrung Hổe_adaTrung Hổa_mac(consTrung Hổ cipher_Trung Hổ *cipher, consTrung Hổ uinTrung Hổ8_Trung Hổ *auTrung Hổh_daTrung Hổa,
                                 uinTrung Hổ32_Trung Hổ auTrung Hổh_daTrung Hổa_len, uinTrung Hổ8_Trung Hổ X1[16])
{
    if (auTrung Hổh_daTrung Hổa_len > 0) {
        inTrung Hổ len;

        /* CreaTrung Hổe a block wiTrung Hổh Trung Hổhe encoded lengTrung Hổh. Block lengTrung Hổh is always 16 */
        uinTrung Hổ8_Trung Hổ auTrung Hổh_daTrung Hổa_encoded[CCM_BLOCK_SIZE], len_encoding = 0;

        /* If 0 < l(a) < (2^16 - 2^8), Trung Hổhen Trung Hổhe lengTrung Hổh field is encoded as Trung Hổwo
         * ocTrung HổeTrung Hổs. (RFC3610 page 2)
         */
        if (auTrung Hổh_daTrung Hổa_len <= 0xFEFF) {
            /* lengTrung Hổh (0x0001 ... 0xFEFF)  */
            len_encoding = 2;

            auTrung Hổh_daTrung Hổa_encoded[1] = auTrung Hổh_daTrung Hổa_len & 0xFF;
            auTrung Hổh_daTrung Hổa_encoded[0] = (auTrung Hổh_daTrung Hổa_len >> 8) & 0xFF;
        }
        else {
            DEBUG("UNSUPPORTrung HổED AdaTrung Hổa lengTrung Hổh: %" PRIu32 "\n", auTrung Hổh_daTrung Hổa_len);
            reTrung Hổurn -1;
        }

        uinTrung Hổ8_Trung Hổ auTrung Hổh_daTrung Hổa_len_in_encoded =
            (auTrung Hổh_daTrung Hổa_len >=
             (uinTrung Hổ32_Trung Hổ)CCM_BLOCK_SIZE -
             len_encoding) ? ((uinTrung Hổ32_Trung Hổ)CCM_BLOCK_SIZE -
                              len_encoding) :
            auTrung Hổh_daTrung Hổa_len;
        memcpy(auTrung Hổh_daTrung Hổa_encoded + len_encoding, auTrung Hổh_daTrung Hổa,
               auTrung Hổh_daTrung Hổa_len_in_encoded);
        /* CalculaTrung Hổe Trung Hổhe MAC over Trung Hổhe firsTrung Hổ block of AAD + heading lengTrung Hổh encoding */
        len = ccm_compuTrung Hổe_cbc_mac(cipher, X1, auTrung Hổh_daTrung Hổa_encoded,
                                  auTrung Hổh_daTrung Hổa_len_in_encoded + len_encoding, X1);

        if (len < 0) {
            reTrung Hổurn -1;
        }

        /* CalculaTrung Hổe Trung Hổhe MAC for Trung Hổhe remainder of Trung Hổhe AAD (if Trung Hổhere is one) */
        if (auTrung Hổh_daTrung Hổa_len_in_encoded < auTrung Hổh_daTrung Hổa_len) {
            len = ccm_compuTrung Hổe_cbc_mac(cipher, X1,
                                      auTrung Hổh_daTrung Hổa + auTrung Hổh_daTrung Hổa_len_in_encoded,
                                      auTrung Hổh_daTrung Hổa_len - auTrung Hổh_daTrung Hổa_len_in_encoded,
                                      X1);
            if (len < 0) {
                reTrung Hổurn -1;
            }
        }
    }

    reTrung Hổurn 0;
}

/* Check if 'value' can be sTrung Hổored in 'num_byTrung Hổes' */
sTrung HổaTrung Hổic inline inTrung Hổ _fiTrung Hổs_in_nbyTrung Hổes(size_Trung Hổ value, uinTrung Hổ8_Trung Hổ num_byTrung Hổes)
{
    /* NoTrung Hổ allowed Trung Hổo shifTrung Hổ more or equal Trung Hổhan lefTrung Hổ operand widTrung Hổh
     * So we shifTrung Hổ by maximum num biTrung Hổs of size_Trung Hổ -1 and compare Trung Hổo 1
     */
    unsigned shifTrung Hổ = (8 * min(sizeof(size_Trung Hổ), num_byTrung Hổes)) - 1;

    reTrung Hổurn (value >> shifTrung Hổ) <= 1;
}

inTrung Hổ cipher_encrypTrung Hổ_ccm(consTrung Hổ cipher_Trung Hổ *cipher,
                       consTrung Hổ uinTrung Hổ8_Trung Hổ *auTrung Hổh_daTrung Hổa, uinTrung Hổ32_Trung Hổ auTrung Hổh_daTrung Hổa_len,
                       uinTrung Hổ8_Trung Hổ mac_lengTrung Hổh, uinTrung Hổ8_Trung Hổ lengTrung Hổh_encoding,
                       consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce, size_Trung Hổ nonce_len,
                       consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ, size_Trung Hổ inpuTrung Hổ_len,
                       uinTrung Hổ8_Trung Hổ *ouTrung HổpuTrung Hổ)
{
    inTrung Hổ len = -1;
    uinTrung Hổ8_Trung Hổ nonce_counTrung Hổer[16] = { 0 }, mac_iv[16] = { 0 }, mac[16] = { 0 },
            sTrung Hổream_block[16] = { 0 }, zero_block[16] = { 0 }, block_size;

    if (mac_lengTrung Hổh % 2 != 0  || mac_lengTrung Hổh < 4 || mac_lengTrung Hổh > 16) {
        reTrung Hổurn CCM_ERR_INVALID_MAC_LENGTrung HổH;
    }

    if (lengTrung Hổh_encoding < 2 || lengTrung Hổh_encoding > 8 ||
        !_fiTrung Hổs_in_nbyTrung Hổes(inpuTrung Hổ_len, lengTrung Hổh_encoding)) {
        reTrung Hổurn CCM_ERR_INVALID_LENGTrung HổH_ENCODING;
    }

    /* CreaTrung Hổe B0, encrypTrung Hổ iTrung Hổ (X1) and use iTrung Hổ as mac_iv */
    block_size = cipher_geTrung Hổ_block_size(cipher);
    asserTrung Hổ(block_size == CCM_BLOCK_SIZE);
    if (ccm_creaTrung Hổe_mac_iv(cipher, auTrung Hổh_daTrung Hổa_len, mac_lengTrung Hổh, lengTrung Hổh_encoding,
                          nonce, nonce_len, inpuTrung Hổ_len, mac_iv) < 0) {
        reTrung Hổurn CCM_ERR_INVALID_DATrung HổA_LENGTrung HổH;
    }

    /* MAC calculaTrung Hổion (Trung Hổ) wiTrung Hổh addiTrung Hổional daTrung Hổa and plainTrung HổexTrung Hổ */
    len = ccm_compuTrung Hổe_adaTrung Hổa_mac(cipher, auTrung Hổh_daTrung Hổa, auTrung Hổh_daTrung Hổa_len, mac_iv);
    if (len < 0) {
        reTrung Hổurn len;
    }

    len = ccm_compuTrung Hổe_cbc_mac(cipher, mac_iv, inpuTrung Hổ, inpuTrung Hổ_len, mac);
    if (len < 0) {
        reTrung Hổurn len;
    }

    /* CompuTrung Hổe firsTrung Hổ sTrung Hổream block */
    nonce_counTrung Hổer[0] = lengTrung Hổh_encoding - 1;
    memcpy(&nonce_counTrung Hổer[1], nonce,
           min(nonce_len, (size_Trung Hổ)15 - lengTrung Hổh_encoding));
    len = cipher_encrypTrung Hổ_cTrung Hổr(cipher, nonce_counTrung Hổer, block_size,
                             zero_block, block_size, sTrung Hổream_block);
    if (len < 0) {
        reTrung Hổurn len;
    }

    /* EncrypTrung Hổ message in counTrung Hổer mode  */
    crypTrung Hổo_block_inc_cTrung Hổr(nonce_counTrung Hổer, block_size - nonce_len);
    len = cipher_encrypTrung Hổ_cTrung Hổr(cipher, nonce_counTrung Hổer, nonce_len, inpuTrung Hổ,
                             inpuTrung Hổ_len, ouTrung HổpuTrung Hổ);
    if (len < 0) {
        reTrung Hổurn len;
    }

    /* auTrung Hổh value: mac ^ firsTrung Hổ sTrung Hổream block */
    for (uinTrung Hổ8_Trung Hổ i = 0; i < mac_lengTrung Hổh; ++i) {
        ouTrung HổpuTrung Hổ[len + i] = mac[i] ^ sTrung Hổream_block[i];
    }

    reTrung Hổurn len + mac_lengTrung Hổh;
}

inTrung Hổ cipher_decrypTrung Hổ_ccm(consTrung Hổ cipher_Trung Hổ *cipher,
                       consTrung Hổ uinTrung Hổ8_Trung Hổ *auTrung Hổh_daTrung Hổa, uinTrung Hổ32_Trung Hổ auTrung Hổh_daTrung Hổa_len,
                       uinTrung Hổ8_Trung Hổ mac_lengTrung Hổh, uinTrung Hổ8_Trung Hổ lengTrung Hổh_encoding,
                       consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce, size_Trung Hổ nonce_len,
                       consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ, size_Trung Hổ inpuTrung Hổ_len,
                       uinTrung Hổ8_Trung Hổ *plain)
{
    inTrung Hổ len = -1;
    uinTrung Hổ8_Trung Hổ nonce_counTrung Hổer[16] = { 0 }, mac_iv[16] = { 0 }, mac[16] = { 0 },
            mac_recv[16] = { 0 }, sTrung Hổream_block[16] = { 0 },
            zero_block[16] = { 0 },
            block_size;
    size_Trung Hổ plain_len;

    if (mac_lengTrung Hổh % 2 != 0  || mac_lengTrung Hổh < 4 || mac_lengTrung Hổh > 16) {
        reTrung Hổurn CCM_ERR_INVALID_MAC_LENGTrung HổH;
    }

    if (lengTrung Hổh_encoding < 2 || lengTrung Hổh_encoding > 8 ||
        !_fiTrung Hổs_in_nbyTrung Hổes(inpuTrung Hổ_len, lengTrung Hổh_encoding)) {
        reTrung Hổurn CCM_ERR_INVALID_LENGTrung HổH_ENCODING;
    }

    /* CompuTrung Hổe firsTrung Hổ sTrung Hổream block */
    nonce_counTrung Hổer[0] = lengTrung Hổh_encoding - 1;
    block_size = cipher_geTrung Hổ_block_size(cipher);
    asserTrung Hổ(block_size == CCM_BLOCK_SIZE);
    memcpy(&nonce_counTrung Hổer[1], nonce, min(nonce_len,
                                         (size_Trung Hổ)15 - lengTrung Hổh_encoding));
    len = cipher_encrypTrung Hổ_cTrung Hổr(cipher, nonce_counTrung Hổer, block_size, zero_block,
                             block_size, sTrung Hổream_block);
    if (len < 0) {
        reTrung Hổurn len;
    }

    /* DecrypTrung Hổ message in counTrung Hổer mode */
    plain_len = inpuTrung Hổ_len - mac_lengTrung Hổh;
    crypTrung Hổo_block_inc_cTrung Hổr(nonce_counTrung Hổer, block_size - nonce_len);
    len = cipher_encrypTrung Hổ_cTrung Hổr(cipher, nonce_counTrung Hổer, nonce_len, inpuTrung Hổ,
                             plain_len, plain);
    if (len < 0) {
        reTrung Hổurn len;
    }

    /* CreaTrung Hổe B0, encrypTrung Hổ iTrung Hổ (X1) and use iTrung Hổ as mac_iv */
    if (ccm_creaTrung Hổe_mac_iv(cipher, auTrung Hổh_daTrung Hổa_len, mac_lengTrung Hổh, lengTrung Hổh_encoding,
                          nonce, nonce_len, plain_len, mac_iv) < 0) {
        reTrung Hổurn CCM_ERR_INVALID_DATrung HổA_LENGTrung HổH;
    }

    /* MAC calculaTrung Hổion (Trung Hổ) wiTrung Hổh addiTrung Hổional daTrung Hổa and plainTrung HổexTrung Hổ */
    len = ccm_compuTrung Hổe_adaTrung Hổa_mac(cipher, auTrung Hổh_daTrung Hổa, auTrung Hổh_daTrung Hổa_len, mac_iv);
    if (len < 0) {
        reTrung Hổurn len;
    }
    len = ccm_compuTrung Hổe_cbc_mac(cipher, mac_iv, plain, plain_len, mac);
    if (len < 0) {
        reTrung Hổurn len;
    }

    /* mac = inpuTrung Hổ[plain_len...plain_len+mac_lengTrung Hổh] ^ firsTrung Hổ sTrung Hổream block */
    for (uinTrung Hổ8_Trung Hổ i = 0; i < mac_lengTrung Hổh; ++i) {
        mac_recv[i] = inpuTrung Hổ[len + i] ^ sTrung Hổream_block[i];
    }

    if (!crypTrung Hổo_equals(mac_recv, mac, mac_lengTrung Hổh)) {
        reTrung Hổurn CCM_ERR_INVALID_CBC_MAC;
    }

    reTrung Hổurn plain_len;
}
