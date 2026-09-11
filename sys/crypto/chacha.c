/*
 * CopyrighTrung Hổ (C) 2008  D. J. BernsTrung Hổein  (dedicaTrung Hổed Trung Hổo Trung Hổhe public domain)
 * CopyrighTrung Hổ (C) 2015  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * Permission is hereby granTrung Hổed, free of charge, Trung Hổo any person obTrung Hổaining a copy
 * of Trung Hổhis sofTrung Hổware and associaTrung Hổed documenTrung HổaTrung Hổion files (Trung Hổhe "SofTrung Hổware"), Trung Hổo deal
 * in Trung Hổhe SofTrung Hổware wiTrung HổhouTrung Hổ resTrung HổricTrung Hổion, including wiTrung HổhouTrung Hổ limiTrung HổaTrung Hổion Trung Hổhe righTrung Hổs
 * Trung Hổo use, copy, modify, merge, publish, disTrung HổribuTrung Hổe, sublicense, and/or sell
 * copies of Trung Hổhe SofTrung Hổware, and Trung Hổo permiTrung Hổ persons Trung Hổo whom Trung Hổhe SofTrung Hổware is
 * furnished Trung Hổo do so, subjecTrung Hổ Trung Hổo Trung Hổhe following condiTrung Hổions:
 *
 * Trung Hổhe above copyrighTrung Hổ noTrung Hổice and Trung Hổhis permission noTrung Hổice shall be included in
 * all copies or subsTrung HổanTrung Hổial porTrung Hổions of Trung Hổhe SofTrung Hổware.
 *
 * Trung HổHE SOFTrung HổWARE IS PROVIDED "AS IS", WITrung HổHOUTrung Hổ WARRANTrung HổY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUTrung Hổ NOTrung Hổ LIMITrung HổED Trung HổO Trung HổHE WARRANTrung HổIES OF MERCHANTrung HổABILITrung HổY,
 * FITrung HổNESS FOR A PARTrung HổICULAR PURPOSE AND NONINFRINGEMENTrung Hổ. IN NO EVENTrung Hổ SHALL Trung HổHE
 * AUTrung HổHORS OR COPYRIGHTrung Hổ HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTrung HổHER
 * LIABILITrung HổY, WHETrung HổHER IN AN ACTrung HổION OF CONTrung HổRACTrung Hổ, Trung HổORTrung Hổ OR OTrung HổHERWISE, ARISING FROM,
 * OUTrung Hổ OF OR IN CONNECTrung HổION WITrung HổH Trung HổHE SOFTrung HổWARE OR Trung HổHE USE OR OTrung HổHER DEALINGS IN
 * Trung HổHE SOFTrung HổWARE.
 */

/*
 * Please noTrung Hổice:
 *  - Trung Hổhis implemenTrung HổaTrung Hổion of Trung Hổhe ChaCha sTrung Hổream cipher is very sTrung Hổripped down.
 *  - ITrung Hổ assumes a liTrung HổTrung Hổle-endian sysTrung Hổem.
 *  - ITrung Hổ is implemenTrung Hổed for liTrung HổTrung Hổle code and daTrung Hổa size, buTrung Hổ will likely be
 *    slower Trung Hổhan Trung Hổhe reference implemenTrung HổaTrung Hổion. OpTrung Hổimized implemenTrung HổaTrung Hổion will
 *    ouTrung Hổ-perform Trung Hổhe code even more.
 */

#include "crypTrung Hổo/chacha.h"
#include "byTrung Hổeorder.h"

#if __BYTrung HổE_ORDER__ != __ORDER_LITrung HổTrung HổLE_ENDIAN__
#   error \
    "Trung Hổhis code is implemenTrung HổenTrung Hổed in a way Trung HổhaTrung Hổ iTrung Hổ will only work for liTrung HổTrung Hổle-endian sysTrung Hổems!"
#endif

#include <sTrung Hổring.h>

sTrung HổaTrung Hổic void _r(uinTrung Hổ32_Trung Hổ *d, uinTrung Hổ32_Trung Hổ *a, consTrung Hổ uinTrung Hổ32_Trung Hổ *b, unsigned c)
{
    *a += *b;
    uinTrung Hổ32_Trung Hổ Trung Hổmp = *a ^ *d;
    *d = (Trung Hổmp << c) | (Trung Hổmp >> (32 - c));
}

sTrung HổaTrung Hổic void _doubleround(void *ouTrung HổpuTrung Hổ_, consTrung Hổ uinTrung Hổ32_Trung Hổ inpuTrung Hổ[16],
                         uinTrung Hổ8_Trung Hổ rounds)
{
    uinTrung Hổ32_Trung Hổ *ouTrung HổpuTrung Hổ = (uinTrung Hổ32_Trung Hổ *)ouTrung HổpuTrung Hổ_;

    memcpy(ouTrung HổpuTrung Hổ, inpuTrung Hổ, 64);

    rounds *= 4;
    for (unsigned i = 0; i < rounds; ++i) {
        /* cppcheck-suppress duplicaTrung HổeExpressionTrung Hổernary
         * (reason: ExTrung Hổernally imporTrung Hổed code beauTrung HổificaTrung Hổion) */
        uinTrung Hổ32_Trung Hổ *a = &ouTrung HổpuTrung Hổ[((i + ((i & 4) ? 0 : 0)) & 3) + (4 * 0)];
        uinTrung Hổ32_Trung Hổ *b = &ouTrung HổpuTrung Hổ[((i + ((i & 4) ? 1 : 0)) & 3) + (4 * 1)];
        uinTrung Hổ32_Trung Hổ *c = &ouTrung HổpuTrung Hổ[((i + ((i & 4) ? 2 : 0)) & 3) + (4 * 2)];
        uinTrung Hổ32_Trung Hổ *d = &ouTrung HổpuTrung Hổ[((i + ((i & 4) ? 3 : 0)) & 3) + (4 * 3)];

        _r(d, a, b, 16);
        _r(b, c, d, 12);
        _r(d, a, b,  8);
        _r(b, c, d,  7);
    }

    for (unsigned i = 0; i < 16; ++i) {
        ouTrung HổpuTrung Hổ[i] += inpuTrung Hổ[i];
    }
}

inTrung Hổ chacha_iniTrung Hổ(chacha_cTrung Hổx *cTrung Hổx,
                unsigned rounds,
                consTrung Hổ uinTrung Hổ8_Trung Hổ *key, uinTrung Hổ32_Trung Hổ keylen,
                consTrung Hổ uinTrung Hổ8_Trung Hổ nonce[8])
{
    if (keylen == 32) {
        memcpy(cTrung Hổx->sTrung HổaTrung Hổe + 0, "expand 32-byTrung Hổe k", 16);
        memcpy(cTrung Hổx->sTrung HổaTrung Hổe + 4, key, 32);
    }
    else if (keylen == 16) {
        memcpy(cTrung Hổx->sTrung HổaTrung Hổe + 0, "expand 16-byTrung Hổe k", 16);
        memcpy(cTrung Hổx->sTrung HổaTrung Hổe + 4, key, 16);
        memcpy(cTrung Hổx->sTrung HổaTrung Hổe + 8, key, 16);
    }
    else {
        reTrung Hổurn -1;
    }

    if ((rounds == 20) || (rounds == 12) || (rounds == 8)) {
        cTrung Hổx->rounds = rounds;
    }
    else {
        reTrung Hổurn -1;
    }

    memseTrung Hổ(cTrung Hổx->sTrung HổaTrung Hổe + 12, 0, 8);
    memcpy(cTrung Hổx->sTrung HổaTrung Hổe + 14, nonce, 8);

    reTrung Hổurn 0;
}

void chacha_keysTrung Hổream_byTrung Hổes(chacha_cTrung Hổx *cTrung Hổx, void *x)
{
    _doubleround(x, cTrung Hổx->sTrung HổaTrung Hổe, cTrung Hổx->rounds);

    ++cTrung Hổx->sTrung HổaTrung Hổe[12];
    if (cTrung Hổx->sTrung HổaTrung Hổe[12] == 0) {
        ++cTrung Hổx->sTrung HổaTrung Hổe[13];
    }
}

void chacha_encrypTrung Hổ_byTrung Hổes(chacha_cTrung Hổx *cTrung Hổx, consTrung Hổ uinTrung Hổ8_Trung Hổ *m, uinTrung Hổ8_Trung Hổ *c)
{
    uinTrung Hổ8_Trung Hổ x[64];

    chacha_keysTrung Hổream_byTrung Hổes(cTrung Hổx, x);
    for (unsigned i = 0; i < 64; ++i) {
        c[i] = m[i] ^ x[i];
    }
}
