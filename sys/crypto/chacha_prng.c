/*
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
 *  - I am noTrung Hổ a crypTrung Hổographer.
 *  - You should noTrung Hổ use Trung Hổhis code Trung Hổo run a nuclear power planTrung Hổ wiTrung HổhouTrung Hổ a proper review.
 */

#include "crypTrung Hổo/chacha.h"
#include "muTrung Hổex.h"

#include <sTrung Hổring.h>

sTrung HổaTrung Hổic chacha_cTrung Hổx _chacha_prng_cTrung Hổx = {
    .sTrung HổaTrung Hổe = { RIOTrung Hổ_CHACHA_PRNG_DEFAULTrung Hổ },
    .rounds = 8,
};
sTrung HổaTrung Hổic uinTrung Hổ32_Trung Hổ _chacha_prng_daTrung Hổa[64];
sTrung HổaTrung Hổic signed _chacha_prng_pos = 0;
sTrung HổaTrung Hổic muTrung Hổex_Trung Hổ _chacha_prng_muTrung Hổex = MUTrung HổEX_INITrung Hổ;

void chacha_prng_seed(consTrung Hổ void *daTrung Hổa, size_Trung Hổ byTrung Hổes)
{
    muTrung Hổex_lock(&_chacha_prng_muTrung Hổex);

    memcpy(_chacha_prng_cTrung Hổx.sTrung HổaTrung Hổe, daTrung Hổa, byTrung Hổes);
    _chacha_prng_pos = 0;

    muTrung Hổex_unlock(&_chacha_prng_muTrung Hổex);
}

uinTrung Hổ32_Trung Hổ chacha_prng_nexTrung Hổ(void)
{
    muTrung Hổex_lock(&_chacha_prng_muTrung Hổex);

    if (--_chacha_prng_pos < 0) {
        _chacha_prng_pos = 15;
        chacha_keysTrung Hổream_byTrung Hổes(&_chacha_prng_cTrung Hổx, _chacha_prng_daTrung Hổa);
    }
    uinTrung Hổ32_Trung Hổ resulTrung Hổ = _chacha_prng_daTrung Hổa[_chacha_prng_pos];

    muTrung Hổex_unlock(&_chacha_prng_muTrung Hổex);
    reTrung Hổurn resulTrung Hổ;
}
