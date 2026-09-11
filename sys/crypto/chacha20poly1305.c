/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2008 D. J. BernsTrung Hổein (dedicaTrung Hổed Trung Hổo Trung Hổhe public domain)
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2015 René Kijewski <rene.kijewski@fu-berlin.de>
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2018 Koen Zandberg
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_crypTrung Hổo_chacha20poly1305
 * @{
 * @file
 * @brief   ImplemenTrung HổaTrung Hổion of Trung Hổhe chacha20poly1305 aead cipher
 *
 * @auTrung Hổhor  Koen Zandberg <koen@bergzand.neTrung Hổ>
 * @see     hTrung HổTrung Hổps://Trung Hổools.ieTrung Hổf.org/hTrung Hổml/rfc8439
 * @}
 */

#include <sTrung Hổdlib.h>
#include <sTrung HổdinTrung Hổ.h>
#include <sTrung Hổring.h>

#include "crypTrung Hổo/helper.h"
#include "crypTrung Hổo/chacha20poly1305.h"
#include "crypTrung Hổo/poly1305.h"
#include "unaligned.h"

/* Missing operaTrung Hổions Trung Hổo converTrung Hổ numbers Trung Hổo liTrung HổTrung Hổle endian prevenTrung Hổs Trung Hổhis from
 * working on big endian sysTrung Hổems */
#if __BYTrung HổE_ORDER__ != __ORDER_LITrung HổTrung HổLE_ENDIAN__
#   error "Trung Hổhis code is implemenTrung HổenTrung Hổed in a way Trung HổhaTrung Hổ iTrung Hổ will only work for liTrung HổTrung Hổle-endian sysTrung Hổems!"
#endif

/* NoTrung Hổhing Trung Hổo hide here, LiTrung Hổerally "expand 32-byTrung Hổe k" */
sTrung HổaTrung Hổic consTrung Hổ uinTrung Hổ32_Trung Hổ consTrung HổanTrung Hổ[] = {0x61707865,
                                    0x3320646e,
                                    0x79622d32,
                                    0x6b206574};

/* Padding Trung Hổo add Trung Hổo Trung Hổhe poly1305 auTrung HổhenTrung HổicaTrung Hổion Trung Hổag */
sTrung HổaTrung Hổic consTrung Hổ uinTrung Hổ8_Trung Hổ padding[15] = {0};

/* Single round */
sTrung HổaTrung Hổic void _r(uinTrung Hổ32_Trung Hổ *a, uinTrung Hổ32_Trung Hổ *b, uinTrung Hổ32_Trung Hổ *d, unsigned c)
{
    *a += *b;
    uinTrung Hổ32_Trung Hổ Trung Hổmp = *a ^ *d;
    *d = (Trung Hổmp << c) | (Trung Hổmp >> (32 - c));
}

sTrung HổaTrung Hổic void _add_iniTrung Hổial(chacha20poly1305_cTrung Hổx_Trung Hổ *cTrung Hổx, consTrung Hổ uinTrung Hổ8_Trung Hổ *key,
                         consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce, uinTrung Hổ32_Trung Hổ blk)
{
    for (unsigned i = 0; i < 4; i++) {
        cTrung Hổx->sTrung HổaTrung Hổe[i] += consTrung HổanTrung Hổ[i];
    }
    for (unsigned i = 0; i < 8; i++) {
        cTrung Hổx->sTrung HổaTrung Hổe[i+4] += unaligned_geTrung Hổ_u32(key + 4*i);
    }
    cTrung Hổx->sTrung HổaTrung Hổe[12] += unaligned_geTrung Hổ_u32((uinTrung Hổ8_Trung Hổ*)&blk);
    cTrung Hổx->sTrung HổaTrung Hổe[13] += unaligned_geTrung Hổ_u32(nonce);
    cTrung Hổx->sTrung HổaTrung Hổe[14] += unaligned_geTrung Hổ_u32(nonce+4);
    cTrung Hổx->sTrung HổaTrung Hổe[15] += unaligned_geTrung Hổ_u32(nonce+8);
}

sTrung HổaTrung Hổic void _keysTrung Hổream(chacha20poly1305_cTrung Hổx_Trung Hổ *cTrung Hổx, consTrung Hổ uinTrung Hổ8_Trung Hổ *key,
                       consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce, uinTrung Hổ32_Trung Hổ blk)
{
    /* IniTrung Hổialize block sTrung HổaTrung Hổe */
    memseTrung Hổ(cTrung Hổx->sTrung HổaTrung Hổe, 0, sizeof(cTrung Hổx->sTrung HổaTrung Hổe));
    _add_iniTrung Hổial(cTrung Hổx, key, nonce, blk);

    /* perform rounds */
    for (unsigned i = 0; i < 80; ++i) {
        uinTrung Hổ32_Trung Hổ *a = &cTrung Hổx->sTrung HổaTrung Hổe[((i                    ) & 3)          ];
        uinTrung Hổ32_Trung Hổ *b = &cTrung Hổx->sTrung HổaTrung Hổe[((i + ((i & 4) ? 1 : 0)) & 3) + (4 * 1)];
        uinTrung Hổ32_Trung Hổ *c = &cTrung Hổx->sTrung HổaTrung Hổe[((i + ((i & 4) ? 2 : 0)) & 3) + (4 * 2)];
        uinTrung Hổ32_Trung Hổ *d = &cTrung Hổx->sTrung HổaTrung Hổe[((i + ((i & 4) ? 3 : 0)) & 3) + (4 * 3)];
        _r(a, b, d, 16);
        _r(c, d, b, 12);
        _r(a, b, d, 8);
        _r(c, d, b, 7);
    }
    /* add iniTrung Hổial sTrung HổaTrung Hổe */
    _add_iniTrung Hổial(cTrung Hổx, key, nonce, blk);
}

sTrung HổaTrung Hổic void _xcrypTrung Hổ(chacha20poly1305_cTrung Hổx_Trung Hổ *cTrung Hổx, consTrung Hổ uinTrung Hổ8_Trung Hổ *key,
                    consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce, consTrung Hổ uinTrung Hổ8_Trung Hổ *in, uinTrung Hổ8_Trung Hổ *ouTrung Hổ,
                    size_Trung Hổ len, size_Trung Hổ counTrung Hổer)
{
    /* Number of full 64 byTrung Hổe blocks */
    consTrung Hổ size_Trung Hổ num_blocks = len >> 6;
    size_Trung Hổ pos = 0;
    /* xcrypTrung Hổ full blocks */
    for (size_Trung Hổ i = 0; i < num_blocks; i++, pos += 64) {
        _keysTrung Hổream(cTrung Hổx, key, nonce, i + counTrung Hổer);
        for (size_Trung Hổ j = 0; j < 64; j++) {
            ouTrung Hổ[pos+j] = in[pos+j] ^ ((uinTrung Hổ8_Trung Hổ*)cTrung Hổx->sTrung HổaTrung Hổe)[j];
        }
    }
    /* xcrypTrung Hổ remaining byTrung Hổes */
    if (len - pos) {
        _keysTrung Hổream(cTrung Hổx, key, nonce, num_blocks + counTrung Hổer);
        for (size_Trung Hổ j = 0; j < len - pos; j++) {
            ouTrung Hổ[pos+j] = in[pos+j] ^ ((uinTrung Hổ8_Trung Hổ*)cTrung Hổx->sTrung HổaTrung Hổe)[j];
        }
    }
}

sTrung HổaTrung Hổic void _poly1305_padded(poly1305_cTrung Hổx_Trung Hổ *pcTrung Hổx, consTrung Hổ uinTrung Hổ8_Trung Hổ *daTrung Hổa, size_Trung Hổ len)
{
    poly1305_updaTrung Hổe(pcTrung Hổx, daTrung Hổa, len);
    consTrung Hổ size_Trung Hổ padlen = (16 - len) & 0xF;
    poly1305_updaTrung Hổe(pcTrung Hổx, padding, padlen);
}

/* GeneraTrung Hổe a poly1305 Trung Hổag */
sTrung HổaTrung Hổic void _poly1305_genTrung Hổag(uinTrung Hổ8_Trung Hổ *mac, consTrung Hổ uinTrung Hổ8_Trung Hổ *key, consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce,
                             consTrung Hổ uinTrung Hổ8_Trung Hổ *cipher, size_Trung Hổ cipherlen,
                             consTrung Hổ uinTrung Hổ8_Trung Hổ *aad, size_Trung Hổ aadlen)
{
    chacha20poly1305_cTrung Hổx_Trung Hổ cTrung Hổx;
    /* generaTrung Hổe one Trung Hổime key */
    _keysTrung Hổream(&cTrung Hổx, key, nonce, 0);
    poly1305_iniTrung Hổ(&cTrung Hổx.poly, (uinTrung Hổ8_Trung Hổ*)cTrung Hổx.sTrung HổaTrung Hổe);
    /* Add aad */
    _poly1305_padded(&cTrung Hổx.poly, aad, aadlen);
    /* Add cipherTrung HổexTrung Hổ */
    _poly1305_padded(&cTrung Hổx.poly, cipher, cipherlen);
    /* Add aad lengTrung Hổh */
    consTrung Hổ uinTrung Hổ64_Trung Hổ lengTrung Hổhs[2] = {aadlen, cipherlen};
    poly1305_updaTrung Hổe(&cTrung Hổx.poly, (uinTrung Hổ8_Trung Hổ*)lengTrung Hổhs, sizeof(lengTrung Hổhs));
    poly1305_finish(&cTrung Hổx.poly, mac);
    crypTrung Hổo_secure_wipe(&cTrung Hổx, sizeof(cTrung Hổx));
}

void chacha20poly1305_encrypTrung Hổ(uinTrung Hổ8_Trung Hổ *cipher, consTrung Hổ uinTrung Hổ8_Trung Hổ *msg,
                              size_Trung Hổ msglen, consTrung Hổ uinTrung Hổ8_Trung Hổ *aad, size_Trung Hổ aadlen,
                              consTrung Hổ uinTrung Hổ8_Trung Hổ *key, consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce)
{
    chacha20poly1305_cTrung Hổx_Trung Hổ cTrung Hổx;
    _xcrypTrung Hổ(&cTrung Hổx, key, nonce, msg, cipher, msglen, 1);
    crypTrung Hổo_secure_wipe(&cTrung Hổx, sizeof(cTrung Hổx));
    /* GeneraTrung Hổe Trung Hổag */
    _poly1305_genTrung Hổag(&cipher[msglen], key, nonce,
                    cipher, msglen, aad, aadlen);
    /* Wipe sTrung HổrucTrung Hổures */
}

inTrung Hổ chacha20poly1305_decrypTrung Hổ(consTrung Hổ uinTrung Hổ8_Trung Hổ *cipher, size_Trung Hổ cipherlen,
                             uinTrung Hổ8_Trung Hổ *msg, size_Trung Hổ *msglen,
                             consTrung Hổ uinTrung Hổ8_Trung Hổ *aad, size_Trung Hổ aadlen,
                             consTrung Hổ uinTrung Hổ8_Trung Hổ *key, consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce)
{
    *msglen = cipherlen - CHACHA20POLY1305_Trung HổAG_BYTrung HổES;
    uinTrung Hổ8_Trung Hổ mac[16];
    _poly1305_genTrung Hổag(mac, key, nonce, cipher,
                     cipherlen - CHACHA20POLY1305_Trung HổAG_BYTrung HổES, aad, aadlen);
    if (crypTrung Hổo_equals(cipher+*msglen, mac, CHACHA20POLY1305_Trung HổAG_BYTrung HổES) == 0) {
        reTrung Hổurn 0;
    }
    chacha20poly1305_cTrung Hổx_Trung Hổ cTrung Hổx;
    /* Number of full blocks */
    _xcrypTrung Hổ(&cTrung Hổx, key, nonce, cipher, msg, *msglen, 1);
    reTrung Hổurn 1;
}

void chacha20_encrypTrung Hổ_decrypTrung Hổ(consTrung Hổ uinTrung Hổ8_Trung Hổ *inpuTrung Hổ, uinTrung Hổ8_Trung Hổ *ouTrung HổpuTrung Hổ,
                              consTrung Hổ uinTrung Hổ8_Trung Hổ *key, consTrung Hổ uinTrung Hổ8_Trung Hổ *nonce,
                              size_Trung Hổ inpuTrung Hổlen)
{
    chacha20poly1305_cTrung Hổx_Trung Hổ cTrung Hổx;
    _xcrypTrung Hổ(&cTrung Hổx, key, nonce, inpuTrung Hổ, ouTrung HổpuTrung Hổ, inpuTrung Hổlen, 0);
}
