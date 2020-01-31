/******************************************************************************
*  Filename:       rf_patch_mce_ghs.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC13x0 Generic 4FSK up to 1.5Mbps
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

#ifndef _RF_PATCH_MCE_GHS_H
#define _RF_PATCH_MCE_GHS_H

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

MCE_PATCH_TYPE patchGhsMce[301] = { 
   0x2fcf603c,
   0x00f03f9d,
   0x0f30003d,
   0x003f0ff0,
   0x0000ff00,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00070003,
   0x31fd31fd,
   0x04000000,
   0x001d000f,
   0x000b0387,
   0x004340f4,
   0x80828000,
   0x00000f90,
   0x0510091e,
   0x00050054,
   0x11010000,
   0x0000003c,
   0x3030002f,
   0x0000027f,
   0xd3910000,
   0x0000193d,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x72487220,
   0x7303a32d,
   0x72037305,
   0x73067304,
   0x73767204,
   0xc7c07276,
   0x00018001,
   0x90109001,
   0x90010801,
   0x720d720c,
   0xb0c0720e,
   0xb0f07100,
   0x7218a0c0,
   0x10208132,
   0x06703952,
   0x16300020,
   0x14011101,
   0x60796c01,
   0x60f4607c,
   0x607961da,
   0x60796079,
   0x6079607a,
   0x60f4607c,
   0x607961da,
   0x60796079,
   0x1210607a,
   0x730f7220,
   0x73117310,
   0x00108181,
   0xb0709180,
   0x606f6051,
   0x606f6646,
   0x663cc030,
   0xc282c141,
   0x1820c470,
   0x6e236f13,
   0x16121611,
   0x78306882,
   0x78409ac0,
   0xc4829ad0,
   0x1820c5a0,
   0x12034093,
   0x16126e23,
   0x83606890,
   0x606f97e0,
   0x94908160,
   0x39808140,
   0x10012a70,
   0x84321611,
   0xc0f38444,
   0xc200c0f5,
   0x40ba1c01,
   0x1c10c100,
   0x4cb240b0,
   0x18031013,
   0x1a131830,
   0x39121a10,
   0x60ba68ad,
   0x60ba13f3,
   0x101513f3,
   0x1850c100,
   0x1a101a15,
   0x68b83914,
   0x7100b0d8,
   0xa0d8b108,
   0xb760b200,
   0x97808ac0,
   0xb0c1b0f1,
   0xb0f17100,
   0x7276a0c1,
   0xb003b480,
   0xb002b013,
   0x7229b012,
   0x7100b0d0,
   0x8140b100,
   0x71009290,
   0x8140b100,
   0x44d322f0,
   0x1c0313f0,
   0x929340df,
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
   0x663cc040,
   0x92f0c000,
   0x92a08ad0,
   0x91c0c110,
   0xb2f06496,
   0x92a082b0,
   0xb0f3b483,
   0x7100b0c3,
   0x64e6a0c3,
   0xb006606f,
   0xb004b016,
   0xb002b014,
   0x8400b012,
   0x04207872,
   0x39838143,
   0x94732a73,
   0x1832c1f2,
   0x10213162,
   0x00123151,
   0x94000020,
   0x16101030,
   0x22103930,
   0x12204124,
   0x10033150,
   0x16a03180,
   0x12029350,
   0x22731204,
   0x84304136,
   0x87d297c0,
   0x84501a82,
   0x87d497c0,
   0x61381a84,
   0x41432263,
   0x97c08440,
   0x1a8087d0,
   0x84601402,
   0x87d097c0,
   0x14041a80,
   0x8440614f,
   0x04107881,
   0x87d297c0,
   0x84601a42,
   0x04107881,
   0x87d497c0,
   0x31521a44,
   0x39633154,
   0x16130633,
   0x38343832,
   0x39823182,
   0x00423184,
   0x78509572,
   0x78109360,
   0x90509030,
   0x90407820,
   0xb2059060,
   0x6965cb40,
   0x936087e0,
   0xa0c5b0f5,
   0x83038ae2,
   0xc00c9302,
   0x8140c00b,
   0x39803180,
   0x81413940,
   0x0431c0f3,
   0x1441c014,
   0x1412c002,
   0x31226979,
   0xb0d1b101,
   0xb0727100,
   0xa04ea0d1,
   0xb011b06e,
   0xa041b06c,
   0xa487a488,
   0x720d720c,
   0x7276720e,
   0xa764b764,
   0x9760c440,
   0xc020c062,
   0xc07e9780,
   0xb0e0c07f,
   0xb0c1b0f1,
   0xb0f5b0c5,
   0xb7607100,
   0xa0c5a0e0,
   0x220080b0,
   0x621345a2,
   0x7100b88f,
   0x978eb073,
   0xb201b074,
   0x85708961,
   0x95511801,
   0x8a718a60,
   0x1c211801,
   0x14124db9,
   0x61ba49b7,
   0x41ba1c01,
   0xb4874db9,
   0xb48861ba,
   0xb0f1b061,
   0xb0417100,
   0xb0f1978f,
   0xb04e7100,
   0x8a73b06e,
   0x70008552,
   0x8180b88c,
   0x392489a4,
   0x00043184,
   0xc0509184,
   0x7850663c,
   0x73769360,
   0x72487276,
   0x72027206,
   0x73057204,
   0x606f7306,
   0x91b01300,
   0xc060b32d,
   0xb0f8663c,
   0x78606509,
   0x12009360,
   0x97801a10,
   0x9760c380,
   0x9760c280,
   0xb0c6a0c1,
   0x22008090,
   0x81544451,
   0x41ec1e04,
   0x16943914,
   0xb0f69784,
   0xd0708552,
   0x663c9862,
   0x8a738a62,
   0x9862e080,
   0x663c9873,
   0x87818790,
   0x4a101c01,
   0x1ef11801,
   0x87814a0e,
   0x97811af1,
   0xb0f67100,
   0x978116f1,
   0x7100a205,
   0xa0c6b0f6,
   0x821d61c6,
   0x418b1e0d,
   0x9880c030,
   0x88a48893,
   0x31343133,
   0x422522fd,
   0x3d343d33,
   0x30d32afd,
   0x821230d4,
   0x163d622b,
   0x3cd43cd3,
   0x82121a3d,
   0x94e3622b,
   0xb05394f4,
   0xc030618b,
   0x88929880,
   0x3d323132,
   0x313388a3,
   0xe0903d33,
   0x98739862,
   0x7000663c,
   0x88409850,
   0x463d2200,
   0x7000b830,
   0x22f08150,
   0xb0704259,
   0x3162c102,
   0x8150c001,
   0x42521e00,
   0x425322f0,
   0x3160e5a0,
   0x62533960,
   0x1a101020,
   0x6e236f13,
   0x16121611,
   0x70006a54
};

PATCH_FUN_SPEC void rf_patch_mce_ghs(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 301; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchGhsMce[i];
   }
#else
   const uint32_t *pS = patchGhsMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 37;

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
