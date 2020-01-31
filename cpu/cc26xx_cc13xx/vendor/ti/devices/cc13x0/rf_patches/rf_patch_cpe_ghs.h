/******************************************************************************
*  Filename:       rf_patch_cpe_ghs.h
*  Revised:        $Date: 2018-11-02 11:52:02 +0100 (fr, 02 nov 2018) $
*  Revision:       $Revision: 18756 $
*
*  Description: RF core patch for CC13x0 Generic 4FSK up to 1.5Mbps
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

#ifndef _RF_PATCH_CPE_GHS_H
#define _RF_PATCH_CPE_GHS_H

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


CPE_PATCH_TYPE patchImageGhs[] = {
   0x21000569,
   0x2100045d,
   0x21000491,
   0x21000495,
   0x210004bd,
   0x2100064d,
   0x210006fd,
   0x21000725,
   0x2100052b,
   0x210004f1,
   0x21000767,
   0x21000789,
   0x4710b5f8,
   0x460eb5f8,
   0x25012100,
   0x473004ad,
   0x7803480a,
   0xf80ff000,
   0xd00b079b,
   0x78204c12,
   0xd00728ff,
   0x702121ff,
   0x240f490e,
   0x43200224,
   0x82c83160,
   0xb5f8bdf8,
   0x47004801,
   0x2100026b,
   0x00004ce5,
   0xe0014809,
   0x0c004808,
   0x49054c06,
   0x2aff7822,
   0x7acad101,
   0x31607022,
   0x467082c8,
   0x47001c80,
   0x40086200,
   0x210007f4,
   0x08080f07,
   0xf886f000,
   0x0a0a9905,
   0xd1092a6c,
   0x61782008,
   0x1c406920,
   0x310a6038,
   0x91056120,
   0x61782000,
   0x0000bdf8,
   0x4708b4f0,
   0x4801b510,
   0x00004700,
   0x00000989,
   0xf818f000,
   0x2950b2e1,
   0x2804d00b,
   0x2806d001,
   0x490dd107,
   0x07c97809,
   0x7821d103,
   0xd4000709,
   0x490a2002,
   0x210c780a,
   0xd0024211,
   0x22804908,
   0xbdfe600a,
   0x4907b5fe,
   0x48044708,
   0x22407801,
   0x70014391,
   0x47004804,
   0x210000c8,
   0x21000117,
   0xe000e200,
   0x0000ccf1,
   0x0000d103,
   0x4605b5ff,
   0x4c03b085,
   0xb5ff4720,
   0x01deb085,
   0x47204c01,
   0x00003ff7,
   0x000041cb,
   0x4603b570,
   0x29014615,
   0x2900d006,
   0x4a11d006,
   0xf7ff4628,
   0xbd70ff67,
   0xe000480f,
   0x2405480f,
   0xd8034283,
   0x1e640840,
   0xdcf92c00,
   0x200140e3,
   0x18180340,
   0x29010b82,
   0x4906d007,
   0x31802300,
   0xf7ff4628,
   0xb2e0ff51,
   0x4902bd70,
   0x316c4b04,
   0x0000e7f6,
   0x00005c83,
   0x2386bca0,
   0x230d8300,
   0x210007c4,
   0x4e1ab5f8,
   0x6b714605,
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
   0x61cc6374,
   0x07c00a40,
   0x2001d00c,
   0x6b310380,
   0xd0012d00,
   0xe0004301,
   0x46084381,
   0x49076331,
   0x63483940,
   0x47b82000,
   0xbdf8b662,
   0x21000280,
   0x21000088,
   0x2100029b,
   0x00003f7b,
   0x40044040,
   0x4a22b510,
   0x61512100,
   0x68894921,
   0xd40900c9,
   0x4b204921,
   0x429805ca,
   0xd8016b4b,
   0xe0004313,
   0x634b4393,
   0xf7ff491d,
   0xbd10ff35,
   0x4d1ab538,
   0x28007f28,
   0x481ad127,
   0x09c08800,
   0xd12207c0,
   0x69604c12,
   0xd11e2800,
   0xf0004668,
   0x4668f88f,
   0x28017800,
   0x4668d117,
   0x28107840,
   0x2008d213,
   0x6a686160,
   0x01400940,
   0x4a0e6020,
   0x62d12100,
   0x21024a0d,
   0x21016011,
   0x60204308,
   0x43082103,
   0x60206268,
   0x4809bd38,
   0xbd384780,
   0x40044000,
   0x21000018,
   0x08930000,
   0x21000280,
   0x000068cf,
   0x21000068,
   0x40041100,
   0xe000e280,
   0x00003bc3,
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
   0x4c03b510,
   0xfedcf7ff,
   0x28006820,
   0xbd10d1fa,
   0x40041100,
   0x2041b510,
   0x00c0490e,
   0x490e4788,
   0x6b884602,
   0x24906b49,
   0x04c1014b,
   0x430b0ec9,
   0x4363490a,
   0x43597d49,
   0x689b4b09,
   0xfef9f7ff,
   0xb510bd10,
   0xfef0f7ff,
   0xd1010004,
   0xffe2f7ff,
   0xbd104620,
   0x00003c7d,
   0x40045080,
   0x21000280,
   0x40044000,
   0x8801b510,
   0x0f93050a,
   0xd1034a08,
   0x0d890589,
   0xd0012911,
   0xbd104790,
   0x46044790,
   0xd1032801,
   0xf7ffb672,
   0xb662ffc5,
   0xbd104620,
   0x00002645,
   0x4801b403,
   0xbd019001,
   0x00006fa5,
   0x00000000,
   0x00030001,
   0x001f000a,
   0x00eb0059,
   0x04ea0239,
   0x129709f9,
   0x32a11feb,
   0x660a4a78,
   0x9e8c82fa,
   0xc917b663,
   0xdeedd664,
   0xe5e0e3c1,
   0x000000ff,
};
#define _NWORD_PATCHIMAGE_GHS 247

#define _NWORD_PATCHSYS_GHS 0

#define _IRQ_PATCH_0 0x21000679


#ifndef _GHS_SYSRAM_START
#define _GHS_SYSRAM_START 0x20000000
#endif

#ifndef _GHS_CPERAM_START
#define _GHS_CPERAM_START 0x21000000
#endif

#define _GHS_SYS_PATCH_FIXED_ADDR 0x20000000

#define _GHS_PARSER_PATCH_TAB_OFFSET 0x0338
#define _GHS_PATCH_TAB_OFFSET 0x0340
#define _GHS_IRQPATCH_OFFSET 0x03BC
#define _GHS_PATCH_VEC_OFFSET 0x041C

PATCH_FUN_SPEC void enterGhsCpePatch(void)
{
#if (_NWORD_PATCHIMAGE_GHS > 0)
   uint32_t *pPatchVec = (uint32_t *) (_GHS_CPERAM_START + _GHS_PATCH_VEC_OFFSET);

   memcpy(pPatchVec, patchImageGhs, sizeof(patchImageGhs));
#endif
}

PATCH_FUN_SPEC void enterGhsSysPatch(void)
{
}

PATCH_FUN_SPEC void configureGhsPatch(void)
{
   uint8_t *pParserPatchTab = (uint8_t *) (_GHS_CPERAM_START + _GHS_PARSER_PATCH_TAB_OFFSET);
   uint8_t *pPatchTab = (uint8_t *) (_GHS_CPERAM_START + _GHS_PATCH_TAB_OFFSET);
   uint32_t *pIrqPatch = (uint32_t *) (_GHS_CPERAM_START + _GHS_IRQPATCH_OFFSET);


   pPatchTab[80] = 0;
   pPatchTab[57] = 1;
   pPatchTab[53] = 2;
   pPatchTab[55] = 3;
   pPatchTab[110] = 4;
   pPatchTab[107] = 5;
   pPatchTab[65] = 6;
   pPatchTab[13] = 7;
   pPatchTab[43] = 8;
   pPatchTab[45] = 9;
   pPatchTab[48] = 10;
   pParserPatchTab[0] = 11;

   pIrqPatch[1] = _IRQ_PATCH_0;
}

PATCH_FUN_SPEC void applyGhsPatch(void)
{
   enterGhsSysPatch();
   enterGhsCpePatch();
   configureGhsPatch();
}

PATCH_FUN_SPEC void refreshGhsPatch(void)
{
   enterGhsCpePatch();
   configureGhsPatch();
}

PATCH_FUN_SPEC void rf_patch_cpe_ghs(void)
{
   applyGhsPatch();
}

#undef _IRQ_PATCH_0

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  _RF_PATCH_CPE_GHS_H

