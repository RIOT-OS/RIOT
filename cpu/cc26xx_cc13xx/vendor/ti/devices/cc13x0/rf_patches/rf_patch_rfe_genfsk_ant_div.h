/******************************************************************************
*  Filename:       rf_patch_rfe_genfsk_ant_div.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC13x0 Generic FSK antenna diversity
*
*  Copyright (c) 2015-2019, Texas Instruments Incorporated
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


#ifndef _RF_PATCH_RFE_GENFSK_ANT_DIV_H
#define _RF_PATCH_RFE_GENFSK_ANT_DIV_H

#include <stdint.h>
#include "../inc/hw_types.h"

#ifndef RFE_PATCH_TYPE
#define RFE_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif

#ifndef RFC_RFERAM_BASE
#define RFC_RFERAM_BASE 0x2100C000
#endif

#ifndef RFE_PATCH_MODE
#define RFE_PATCH_MODE 0
#endif

RFE_PATCH_TYPE patchGenfskAntDivRfe[365] = { 
   0x0000613b,
   0x1307147f,
   0x35f1004d,
   0x0003ffa6,
   0x003f0a91,
   0xf00ff000,
   0x40004030,
   0x40034001,
   0x400f4007,
   0x40cf404f,
   0x43cf41cf,
   0x4fcf47cf,
   0x2fcf3fcf,
   0x0fcf1fcf,
   0x000f0000,
   0x00000008,
   0x0000003f,
   0x003f0040,
   0x00040000,
   0x000e0068,
   0x000600dc,
   0x001a0043,
   0x00000005,
   0x00020000,
   0x003f0000,
   0x00000000,
   0x00c00004,
   0x00040000,
   0x000000c0,
   0xc0500007,
   0x70009100,
   0x9100c070,
   0x31827000,
   0x91310021,
   0x7000b110,
   0xb1018101,
   0x3182a100,
   0xb1109132,
   0x10119101,
   0x22418141,
   0x06f1404e,
   0xc0517000,
   0x18309101,
   0x31833910,
   0x3118d008,
   0x00316f41,
   0xb1109131,
   0x16141483,
   0x7000685a,
   0xc0501025,
   0xc3f49100,
   0x1420c0c0,
   0x10316f03,
   0xc0220441,
   0x00213182,
   0xb1109131,
   0x10313963,
   0xc0820441,
   0x00213182,
   0xb1109131,
   0x10313963,
   0x3182c0a2,
   0x91310021,
   0x1050b110,
   0x14053115,
   0x70009205,
   0xc052643e,
   0x06311031,
   0x02c13161,
   0xc1126441,
   0x39211031,
   0x31510671,
   0x644102e1,
   0xc0517000,
   0xcc019101,
   0x6441c0e2,
   0xc111643e,
   0x6441c0c2,
   0x7100b054,
   0xb064a054,
   0x220080f0,
   0xc111409c,
   0x6441c122,
   0x9101c051,
   0x3182c0e2,
   0x00028260,
   0xb1109132,
   0x68afc300,
   0x1240643e,
   0xb03290b0,
   0x39538253,
   0x64843953,
   0x68b9c360,
   0x90b01280,
   0x7000b032,
   0xc101643e,
   0x6441c122,
   0xc0c2c101,
   0x82536441,
   0x12c06484,
   0xb03290b0,
   0x643e7000,
   0xc081c272,
   0xc1226441,
   0x6441c111,
   0xc111c002,
   0xc0626441,
   0x6441c331,
   0xc111c362,
   0xc3026441,
   0x6441c111,
   0x39538253,
   0xc3e26484,
   0x22116446,
   0xc24240e1,
   0x6441c881,
   0xc111c252,
   0xc2726441,
   0x6441cee1,
   0xc881c202,
   0xc2026441,
   0x6441c801,
   0x68f5c0b0,
   0x643e7000,
   0xc801c242,
   0xc2526441,
   0x6441c011,
   0xc0e1c272,
   0xc0026441,
   0x6441c101,
   0xc301c062,
   0xc1226441,
   0x6441c101,
   0xc101c362,
   0xc3026441,
   0x6441c101,
   0x64848253,
   0x00007000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x73060000,
   0x720b7205,
   0xb050720e,
   0x80817100,
   0xa050b060,
   0x22418092,
   0x8080455b,
   0x0410c1f1,
   0x11011630,
   0x6c011401,
   0x616f616e,
   0x61716170,
   0x61736172,
   0x61766174,
   0x617a6178,
   0x62b6617c,
   0x809162b9,
   0x0421c0f2,
   0x80823121,
   0x14122a42,
   0x11011632,
   0x6c011421,
   0x61a36186,
   0x62b9618c,
   0x61a36186,
   0x62b9618c,
   0x617e617e,
   0x617e617e,
   0x617e617e,
   0x617e6493,
   0x617e64be,
   0x617e64cb,
   0x617e64f7,
   0x617e66c6,
   0x90b01210,
   0x7306720e,
   0x12107205,
   0x613f9030,
   0x66bccff0,
   0xc003c1c4,
   0x6453c3c0,
   0x91507840,
   0x92107850,
   0x92207860,
   0x92307870,
   0x92407880,
   0x92607890,
   0x91907830,
   0x39408210,
   0x100106f0,
   0x14103110,
   0x66c29200,
   0x8210617e,
   0x0410c0f1,
   0x821a1009,
   0x041a394a,
   0x39808210,
   0x100e0410,
   0xa0bca040,
   0x78a080e1,
   0x90e10401,
   0x80f0a054,
   0x45bc2250,
   0x22008040,
   0x61b446ab,
   0x10ab822d,
   0x10c210bc,
   0xb0136462,
   0x6649b003,
   0xb050b053,
   0xb064b054,
   0x65fcb013,
   0x82106621,
   0x45cf22e0,
   0x7100662a,
   0x22018041,
   0x80f046ab,
   0x41af2250,
   0x221080f0,
   0xb06445df,
   0x41cf2231,
   0x664fb063,
   0xb06461cf,
   0x318f816f,
   0xdfe03d8f,
   0x66bc92ff,
   0x80417100,
   0x46ab2201,
   0x80f0b064,
   0x41f52250,
   0x8211b063,
   0x45f422c1,
   0x6698664f,
   0x816161e6,
   0x31818172,
   0x31823d81,
   0x61af3d82,
   0x3d8380f3,
   0x4e0818d3,
   0x16130bf3,
   0x4a201ce3,
   0x81e391c3,
   0x620d143b,
   0x4a201ce3,
   0x81e391c3,
   0x1cab183b,
   0x1c9b4e1c,
   0x1cbc4a1e,
   0x10b24220,
   0x22d08210,
   0x80f0421a,
   0x46202210,
   0x62206462,
   0x621110ab,
   0x6211109b,
   0x82307000,
   0x0410c0f1,
   0x7100b063,
   0x10bc6a24,
   0x7000b0e0,
   0x91c281b2,
   0x820181d2,
   0x81511812,
   0x82411812,
   0x3d813181,
   0x4a3b1c12,
   0xb032b0e2,
   0x92f2dfd0,
   0xc7f166bc,
   0x4e3f1421,
   0x9162c812,
   0x31829172,
   0x16223942,
   0x002080e0,
   0xb03190e0,
   0x12087000,
   0xc800c006,
   0x91709160,
   0x82017000,
   0x91c081b0,
   0x181081d0,
   0x18108151,
   0x80e11406,
   0x31828242,
   0x1c203d82,
   0x22214a67,
   0xb0e24673,
   0x1002b032,
   0x92f2dfc0,
   0x102066bc,
   0x22216273,
   0x1a324273,
   0x4e731c20,
   0xb032a0e2,
   0xdfb01002,
   0x66bc92f2,
   0x82311020,
   0xc0f03941,
   0x1e010401,
   0x16184281,
   0x3010c010,
   0x46971c08,
   0x3c101060,
   0xc7f11006,
   0x4e851461,
   0x9166c816,
   0x31818171,
   0x1c163d81,
   0x91764a8c,
   0x39463186,
   0x80e01686,
   0x048078b8,
   0x90e00060,
   0xc006b031,
   0x70001208,
   0x31818161,
   0x82403d81,
   0x18013980,
   0x4aaa1cf1,
   0x80b01401,
   0x46aa22c0,
   0xb033b0bc,
   0x92f1efa0,
   0x66bc930f,
   0x82017000,
   0x31828162,
   0xef903d82,
   0x930292f1,
   0xa00366bc,
   0x617e64f7,
   0x646280a2,
   0xb050617e,
   0x617e7100,
   0x82d092e0,
   0x46bd2200,
   0x7000b2c0,
   0x22f080a0,
   0xb03042d9,
   0x3162c102,
   0x80a0c001,
   0x42d21e00,
   0x42d322f0,
   0x3160eda0,
   0x62d33960,
   0x1a101020,
   0x6e236f13,
   0x16121611,
   0x70006ad4
};

PATCH_FUN_SPEC void rf_patch_rfe_genfsk_ant_div(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 365; i++) {
      HWREG(RFC_RFERAM_BASE + 4 * i) = patchGenfskAntDivRfe[i];
   }
#else
   const uint32_t *pS = patchGenfskAntDivRfe;
   volatile unsigned long *pD = &HWREG(RFC_RFERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 45;

   do {
      t1 = *pS++;
      t2 = *pS++;
      t3 = *pS++;
      t4 = *pS++;
      t5 = *pS++;
      t6 = *pS++;
      t7 = *pS++;
      t8 = *pS++;
      *pD++ = t1;
      *pD++ = t2;
      *pD++ = t3;
      *pD++ = t4;
      *pD++ = t5;
      *pD++ = t6;
      *pD++ = t7;
      *pD++ = t8;
   } while (--nIterations);

   t1 = *pS++;
   t2 = *pS++;
   t3 = *pS++;
   t4 = *pS++;
   t5 = *pS++;
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
#endif
}

#endif
