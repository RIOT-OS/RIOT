/******************************************************************************
*  Filename:       rf_patch_cpe_ieee.h
*  Revised:        $Date: 2018-05-07 15:02:01 +0200 (ma, 07 mai 2018) $
*  Revision:       $Revision: 18438 $
*
*  Description:    RF Core patch file for CC26x0 IEEE 802.15.4 PHY
*
*  Copyright (c) 2015, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef _RF_PATCH_CPE_IEEE_H
#define _RF_PATCH_CPE_IEEE_H

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

#ifndef CPE_PATCH_TYPE
#define CPE_PATCH_TYPE static const uint32_t
#endif

#ifndef SYS_PATCH_TYPE
#define SYS_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif

#ifndef _APPLY_PATCH_TAB
#define _APPLY_PATCH_TAB
#endif


CPE_PATCH_TYPE patchImageIeee[] = {
   0x2100050f,
   0x2100041d,
   0x21000539,
   0x210005b9,
   0x210004b5,
   0x2100060f,
   0x22024823,
   0x421a7dc3,
   0xd0034472,
   0x1dc04678,
   0xb5f84686,
   0x4c1f4710,
   0x200834ae,
   0x490347a0,
   0x60082008,
   0x3cec6008,
   0xbdf847a0,
   0x40045004,
   0x4c17b5f0,
   0x18612140,
   0x280278c8,
   0x4809d005,
   0x60012100,
   0x47884908,
   0x6e25bdf0,
   0x60354e07,
   0x43280760,
   0x68276620,
   0x480e6024,
   0x60274780,
   0xbdf06035,
   0x4004112c,
   0x000065a5,
   0x40044028,
   0x4c07b510,
   0x29007da1,
   0x2101d105,
   0x024875a1,
   0x393e4904,
   0x68204788,
   0xd0002800,
   0xbd104780,
   0x21000254,
   0x0000398b,
   0x6a034807,
   0x46784907,
   0x46861dc0,
   0x4788b5f8,
   0x009b089b,
   0x6a014802,
   0xd10007c9,
   0xbdf86203,
   0x40045040,
   0x0000f1ab,
   0x4605b5ff,
   0x4c03b085,
   0xb5ff4720,
   0x01dfb085,
   0x47204c01,
   0x00003d5b,
   0x00003f23,
   0x6a00480b,
   0xd00407c0,
   0x2201480a,
   0x43117801,
   0x48097001,
   0x72c84700,
   0xd006280d,
   0x00802285,
   0x18800252,
   0x60486840,
   0x48044770,
   0x0000e7fb,
   0x40045040,
   0x21000268,
   0x0000ff39,
   0x210004f9,
   0x4e1ab5f8,
   0x6b314605,
   0x09cc4819,
   0x2d0001e4,
   0x4918d011,
   0x29027809,
   0x7b00d00f,
   0xb6724304,
   0x4f152001,
   0x47b80240,
   0x38204811,
   0x09c18800,
   0xd00407c9,
   0x7ac0e016,
   0x7b40e7f0,
   0x490fe7ee,
   0x61cc6334,
   0x07c00a40,
   0x2001d00c,
   0x6af10380,
   0xd0012d00,
   0xe0004301,
   0x46084381,
   0x490762f1,
   0x63483940,
   0x47b82000,
   0xbdf8b662,
   0x21000280,
   0x21000088,
   0x21000296,
   0x00003cdf,
   0x40044040,
   0x28004907,
   0x2004d000,
   0xb6724a06,
   0x07c97809,
   0x5810d001,
   0x2080e000,
   0xb240b662,
   0x00004770,
   0x2100026b,
   0x40046058,
   0x2041b510,
   0x00c0490e,
   0x490e4788,
   0x6b884602,
   0x24906b49,
   0x04c1014b,
   0x430b0ec9,
   0x4363490a,
   0x43597c49,
   0x689b4b09,
   0xff6df7ff,
   0xb510bd10,
   0xff64f7ff,
   0xd1010004,
   0xffe2f7ff,
   0xbd104620,
   0x00003a39,
   0x40045080,
   0x21000280,
   0x40044000,
};
#define _NWORD_PATCHIMAGE_IEEE 139

#define _NWORD_PATCHSYS_IEEE 0

#define _IRQ_PATCH_0 0x2100044d
#define _IRQ_PATCH_1 0x2100048d


#ifndef _IEEE_SYSRAM_START
#define _IEEE_SYSRAM_START 0x20000000
#endif

#ifndef _IEEE_CPERAM_START
#define _IEEE_CPERAM_START 0x21000000
#endif

#define _IEEE_SYS_PATCH_FIXED_ADDR 0x20000000

#define _IEEE_PARSER_PATCH_TAB_OFFSET 0x0334
#define _IEEE_PATCH_TAB_OFFSET 0x033C
#define _IEEE_IRQPATCH_OFFSET 0x03AC
#define _IEEE_PATCH_VEC_OFFSET 0x0404

PATCH_FUN_SPEC void enterIeeeCpePatch(void)
{
#if (_NWORD_PATCHIMAGE_IEEE > 0)
   uint32_t *pPatchVec = (uint32_t *) (_IEEE_CPERAM_START + _IEEE_PATCH_VEC_OFFSET);

   memcpy(pPatchVec, patchImageIeee, sizeof(patchImageIeee));
#endif
}

PATCH_FUN_SPEC void enterIeeeSysPatch(void)
{
}

PATCH_FUN_SPEC void configureIeeePatch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_IEEE_CPERAM_START + _IEEE_PATCH_TAB_OFFSET);
   uint32_t *pIrqPatch = (uint32_t *) (_IEEE_CPERAM_START + _IEEE_IRQPATCH_OFFSET);


   pPatchTab[5] = 0;
   pPatchTab[52] = 1;
   pPatchTab[103] = 2;
   pPatchTab[60] = 3;
   pPatchTab[38] = 4;
   pPatchTab[43] = 5;

   pIrqPatch[1] = _IRQ_PATCH_0;
   pIrqPatch[9] = _IRQ_PATCH_1;
}

PATCH_FUN_SPEC void applyIeeePatch(void)
{
   enterIeeeSysPatch();
   enterIeeeCpePatch();
   configureIeeePatch();
}

PATCH_FUN_SPEC void refreshIeeePatch(void)
{
   enterIeeeCpePatch();
   configureIeeePatch();
}

PATCH_FUN_SPEC void rf_patch_cpe_ieee(void)
{
   applyIeeePatch();
}

#undef _IRQ_PATCH_0
#undef _IRQ_PATCH_1

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  _RF_PATCH_CPE_IEEE_H

