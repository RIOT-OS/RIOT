/******************************************************************************
*  Filename:       rf_patch_mce_genfsk_ant_div_pqt.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC13x0 Generic FSK PQT based antenna diversity
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

#ifndef _RF_PATCH_MCE_GENFSK_ANT_DIV_PQT_H
#define _RF_PATCH_MCE_GENFSK_ANT_DIV_PQT_H

#include <stdint.h>
#include "../inc/hw_types.h"

#ifndef MCE_PATCH_TYPE
#define MCE_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif

#ifndef RFC_MCERAM_BASE
#define RFC_MCERAM_BASE 0x21008000
#endif

#ifndef MCE_PATCH_MODE
#define MCE_PATCH_MODE 0
#endif

MCE_PATCH_TYPE patchGenfskAntDivPqtMce[415] = { 
   0x2fcf602d,
   0x00003f9d,
   0x470a031a,
   0x000f0387,
   0xff00ffcf,
   0x80000006,
   0x00070003,
   0x00003d1f,
   0x04000000,
   0x0000000f,
   0x000b0387,
   0x004340f4,
   0x80828000,
   0x00000670,
   0x0510091e,
   0x00050054,
   0x3e100200,
   0x00000061,
   0x0c30002f,
   0x0000017f,
   0xaaaa0000,
   0x0000aaaa,
   0x72200000,
   0xa32d7248,
   0x73057303,
   0x73047203,
   0x72047306,
   0x72767376,
   0x8001c7c0,
   0x90010001,
   0x08019010,
   0x720c9001,
   0x720e720d,
   0x7100b0c0,
   0xa0c0b0f0,
   0x81327218,
   0x39521020,
   0x00200670,
   0x11011630,
   0x6c011401,
   0x606c606a,
   0x62cf60e4,
   0x606a606a,
   0x606b606a,
   0x606c606a,
   0x62cf60e4,
   0x606a606a,
   0x606b606a,
   0x72201210,
   0x7310730f,
   0x81817311,
   0x91800010,
   0x6042b070,
   0x60606060,
   0x6738c030,
   0xc282c0c1,
   0x1820c470,
   0x6e236f13,
   0x16121611,
   0x78306872,
   0x78409ab0,
   0x78509ac0,
   0x78609ad0,
   0xc4829ae0,
   0x1820c5a0,
   0x12034087,
   0x16126e23,
   0x60606884,
   0x94908160,
   0x39808140,
   0x10012a70,
   0x84321611,
   0xc0f38444,
   0xc200c0f5,
   0x40ac1c01,
   0x1c10c100,
   0x4ca440a2,
   0x18031013,
   0x1a131830,
   0x39121a10,
   0x60ac689f,
   0x60ac13f3,
   0x101513f3,
   0x1850c100,
   0x1a101a15,
   0x68aa3914,
   0x7100b0d8,
   0xa0d8b108,
   0x85e0b005,
   0x44b51e00,
   0xb24060b7,
   0xb200a005,
   0xb003b480,
   0xb002b013,
   0x7229b012,
   0x7100b0d0,
   0x8140b100,
   0x71009290,
   0x8140b100,
   0x44c322f0,
   0x1c0313f0,
   0x929340cf,
   0x71009492,
   0x9295b100,
   0x71009494,
   0xb0d0b100,
   0x7000a480,
   0xc030a0d1,
   0xc0409760,
   0xb0f19780,
   0x7100b0c1,
   0xa0c1b0f1,
   0xa0037276,
   0x7000a002,
   0x7310730f,
   0xd04085e1,
   0x67389861,
   0x91c0c100,
   0xb4836488,
   0xb0c3b0f3,
   0xa0c37100,
   0x606064d6,
   0xb016b006,
   0xb014b004,
   0xb012b002,
   0x78728400,
   0x81430420,
   0x2a733983,
   0xc1f29473,
   0x31621832,
   0x31511021,
   0x00200012,
   0x3142c022,
   0x94000020,
   0x16101030,
   0x31211001,
   0x22103930,
   0x12204114,
   0x10033150,
   0x00103180,
   0x93501630,
   0x12041202,
   0x41272273,
   0x97c08430,
   0x1a8287d2,
   0x97c08450,
   0x1a8487d4,
   0x22636129,
   0x84404134,
   0x87d097c0,
   0x14021a80,
   0x97c08460,
   0x1a8087d0,
   0x61401404,
   0x78918440,
   0x97c00410,
   0x1a4287d2,
   0x78918460,
   0x97c00410,
   0x1a4487d4,
   0x31543152,
   0x06333963,
   0x38321613,
   0x31823834,
   0x31843982,
   0x95720042,
   0x90307810,
   0x78209050,
   0x90609040,
   0x31808200,
   0x78b13980,
   0x92001410,
   0x8ae2b205,
   0x93028303,
   0x31808140,
   0x39403980,
   0xc0f38141,
   0xc0140431,
   0xc0021441,
   0x69661412,
   0x847d3122,
   0x8ac0161d,
   0x39803180,
   0x312d140d,
   0xc200142d,
   0x180d3120,
   0xa04eb072,
   0xb011b06e,
   0x7276b06c,
   0xa764b764,
   0x9762c662,
   0x7255c04f,
   0xb0f1b0f6,
   0xb113b110,
   0x720d720c,
   0xa0e0720e,
   0xb405b0e3,
   0x8ad3a404,
   0x39833183,
   0x31808410,
   0x31833980,
   0x94100030,
   0x31508400,
   0x8ad33950,
   0x06f33983,
   0x1834c1f4,
   0x31343184,
   0x94000040,
   0xa0e0b089,
   0xb0c1a0e3,
   0xb228b005,
   0x84017880,
   0xc0200401,
   0x00013140,
   0x72769401,
   0x9760c030,
   0x821a6702,
   0x1a1078a0,
   0xa7609780,
   0xb113b760,
   0x672cb0f1,
   0x8ac07100,
   0x97803980,
   0xb760a760,
   0x80b0b0f1,
   0x45cd2230,
   0x10a0b064,
   0x06103920,
   0x45cb1e00,
   0x7100b069,
   0x8ad061a6,
   0x41dc22e0,
   0x392010a0,
   0x1e000610,
   0x10a345dc,
   0x3d833143,
   0x16118611,
   0x61cb9611,
   0x314310a3,
   0xd0503d83,
   0x67389863,
   0x8ac0b064,
   0x78a13980,
   0x97801410,
   0xb760a760,
   0xb0f1b113,
   0x7100672c,
   0x223080b0,
   0x61fd45f2,
   0x31428212,
   0xd0603d82,
   0x67389862,
   0x1c23671b,
   0x67024a16,
   0x67026211,
   0x31428212,
   0xd0703d82,
   0x67389862,
   0x78a0671b,
   0x97801a10,
   0xb760a760,
   0x7100b0f1,
   0xc012b069,
   0x31823172,
   0x10203d82,
   0x10031032,
   0x6217670a,
   0x7276670a,
   0x9760c660,
   0x7880978d,
   0x04018401,
   0xc0509401,
   0xa76393b0,
   0xb0e0b763,
   0xb110a0e3,
   0xb0f6b113,
   0xa0c1b0c6,
   0xb064b0f1,
   0xb0737100,
   0xb0c1879c,
   0xa760a0c6,
   0x80b0b760,
   0x463d2200,
   0x16118601,
   0xd0809601,
   0x67389861,
   0x978f6174,
   0xb0f1a0e3,
   0xa0c6b0c1,
   0x80b0b110,
   0x46422200,
   0x12607100,
   0xb0f19780,
   0x8961b88f,
   0x18018570,
   0x8a609551,
   0xa4888a71,
   0xc022a487,
   0x1c211801,
   0x14124e5f,
   0x62604a5d,
   0x42601c01,
   0xb4874e5f,
   0xb4886260,
   0xb061b041,
   0xb04e7100,
   0x80b1b06e,
   0x46912201,
   0xb0f67276,
   0x31218471,
   0x1410c260,
   0xc7e09780,
   0xc6f09760,
   0xb0f69760,
   0xa0c1b0c6,
   0x8a63b7b0,
   0x71008a74,
   0x220180b1,
   0x8ab04691,
   0x468422c0,
   0x22018991,
   0x62914684,
   0xc00081c1,
   0x847091c0,
   0x6a8881a2,
   0xc30091c1,
   0xb2019070,
   0xa044a0e0,
   0x87907000,
   0xc360140c,
   0x1cdc180c,
   0x86014a9e,
   0x96011611,
   0x9861d090,
   0x61746738,
   0xa760b073,
   0xa7b0b760,
   0xa04eb072,
   0xb011b06e,
   0xb88c623d,
   0x89a48180,
   0x31843924,
   0x91840004,
   0x860385e4,
   0x9864e0a0,
   0x67389873,
   0x46ba1e04,
   0x16108720,
   0x62bd9720,
   0x16108730,
   0x85f29730,
   0x3d821023,
   0x3d833183,
   0x986ce0b0,
   0x6738987d,
   0x72767376,
   0x72067248,
   0x72047202,
   0x73067305,
   0x13006060,
   0xb32d91b0,
   0x6738c0c0,
   0x93b0c030,
   0x64f4b0f8,
   0x1a101200,
   0xc3809780,
   0xc2809760,
   0xa0c19760,
   0x8090b0c6,
   0x44422200,
   0x1e048154,
   0x978442e1,
   0x8552b0f6,
   0x9862d0d0,
   0x8a626738,
   0x87908a73,
   0x1c018781,
   0x18014aff,
   0x4afd1ef1,
   0x1af18781,
   0x71009781,
   0x16f1b0f6,
   0xa2059781,
   0xb0f67100,
   0x62a7a0c6,
   0x22508000,
   0xb0054707,
   0xb2406309,
   0x7000a005,
   0x39833183,
   0x00233182,
   0xa5e095f3,
   0x22548004,
   0xb5e0471a,
   0x102385f2,
   0x31833982,
   0x95f20032,
   0x82007000,
   0x39803180,
   0x4f221c23,
   0x63231021,
   0x31811031,
   0x92001410,
   0x8210a205,
   0x47272210,
   0x7000b205,
   0x8210a205,
   0x472d2210,
   0x31808200,
   0x78b13980,
   0x92001410,
   0x7000b205,
   0x88409850,
   0x47392200,
   0x7000b830
};

PATCH_FUN_SPEC void rf_patch_mce_genfsk_ant_div_pqt(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 415; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchGenfskAntDivPqtMce[i];
   }
#else
   const uint32_t *pS = patchGenfskAntDivPqtMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 51;

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
   t6 = *pS++;
   t7 = *pS++;
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
   *pD++ = t6;
   *pD++ = t7;
#endif
}

#endif
