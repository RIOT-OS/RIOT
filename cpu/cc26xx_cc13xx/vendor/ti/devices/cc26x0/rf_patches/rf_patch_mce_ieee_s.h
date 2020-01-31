/******************************************************************************
*  Filename:       rf_patch_mce_ieee_s.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC26x0 IEEE 802.15.4 Single Ended Output
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

#ifndef _RF_PATCH_MCE_IEEE_S_H
#define _RF_PATCH_MCE_IEEE_S_H

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

MCE_PATCH_TYPE patchZigbeeXsIsMce[256] = { 
   0xf703605f,
   0x70399b3a,
   0x039bb3af,
   0x39b33af7,
   0x9b3aaf70,
   0xb3aff703,
   0x3af77039,
   0xaf70039b,
   0x08fcb9b3,
   0x8fc664c5,
   0xfc644c50,
   0xc64cc508,
   0x64c5508f,
   0x4c5008fc,
   0xc5088fc6,
   0x508ffc64,
   0x0fcfc64c,
   0x7f7f079c,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000003,
   0x0000001f,
   0x80000000,
   0x0004000c,
   0x000114c4,
   0x00000009,
   0x00008000,
   0x002b0670,
   0x0a11121d,
   0x0b600000,
   0x40100000,
   0x00000040,
   0x1e1e0006,
   0x0000001e,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x72200000,
   0x720d720c,
   0x7248720e,
   0x7203a32d,
   0x73057204,
   0x73767306,
   0xc7c07276,
   0xb0c09010,
   0xa0c07100,
   0x7218b0f0,
   0x10208132,
   0x06703952,
   0x16300020,
   0x14011101,
   0x609b6c01,
   0x60c860a9,
   0x6147612c,
   0x609b609b,
   0x609b609c,
   0x609b609b,
   0x609b609b,
   0x609b609b,
   0x609b609c,
   0x609b609b,
   0x609b609b,
   0x609b609b,
   0x609b609c,
   0x609b609b,
   0x609b609b,
   0x609b609b,
   0x60a0609c,
   0x60a0664e,
   0x60a11220,
   0x730f1210,
   0x73117310,
   0x00108181,
   0xb0709180,
   0xc301606d,
   0xc420c282,
   0x6f131820,
   0x16116e23,
   0x68ad1612,
   0xc810c482,
   0x40ba1820,
   0x6e231203,
   0x68b71612,
   0x60a072ab,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0xc1307276,
   0xb0f391c0,
   0xb0d8b108,
   0xb1087100,
   0xb200a0d8,
   0x1e008ab0,
   0xb76040da,
   0xb0f19780,
   0x7100b0c1,
   0xb013b483,
   0xb012b003,
   0xb0f1b002,
   0x7276a0c1,
   0x7100b0c3,
   0xa0c3b0f3,
   0xc0301000,
   0xc0209760,
   0xb0c19780,
   0xb0f17100,
   0x7276a0c1,
   0xa0037248,
   0x7248a002,
   0x73067305,
   0x72767376,
   0x9010c7c0,
   0x000060a0,
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
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0xc0307276,
   0xb00291b0,
   0xb006b004,
   0x90507a10,
   0x7a209030,
   0x90409060,
   0x94507810,
   0x94607820,
   0xc0f18160,
   0xc0120410,
   0x14023110,
   0x94306f20,
   0x6f201612,
   0x84209440,
   0x84213180,
   0x39813181,
   0x94100010,
   0xb2051200,
   0x31858165,
   0x39853145,
   0x120e3945,
   0x12061217,
   0x940012f0,
   0x9400c190,
   0xb119b011,
   0xa0ecb0e9,
   0x7100b089,
   0xa0e9b119,
   0xb0ecb11c,
   0x7100a404,
   0x659bb11c,
   0x417e1e76,
   0x45471eaf,
   0x1c90c140,
   0x12054d47,
   0x12761202,
   0x9070c300,
   0xc070b200,
   0xc0c065fa,
   0x7a309400,
   0x61859410,
   0x919f1647,
   0x1e008150,
   0x1c704185,
   0x7100498a,
   0x22008090,
   0x61684470,
   0x7276a205,
   0x72047203,
   0x73067305,
   0xa004a002,
   0x7248a006,
   0x73067305,
   0x72767376,
   0x9010c7c0,
   0x120960a0,
   0xc0cc120a,
   0xb88e120d,
   0x1c898928,
   0x1c8a49a6,
   0x61b34dac,
   0x12001089,
   0x100a1880,
   0x61b310db,
   0x1200108a,
   0x10091880,
   0x168b10db,
   0x161d61b3,
   0x41b81e8d,
   0x619f908c,
   0x1e8210bf,
   0x149541c3,
   0x1e821612,
   0x312545c3,
   0x00058180,
   0x1e8b9185,
   0x1a8b49c6,
   0x18b0c070,
   0x11011630,
   0x6c011401,
   0x908c908c,
   0x908c908c,
   0x908c908c,
   0x908c908c,
   0x1000b082,
   0x8923b88e,
   0xb083b083,
   0xb88e1000,
   0x1e8f8924,
   0x1ca349e4,
   0x1ca449e9,
   0x700049eb,
   0x4de91c93,
   0x4deb1c94,
   0x1a1e7000,
   0x161e61ed,
   0xc04061ed,
   0x49f41ce0,
   0x164010e0,
   0x700049f7,
   0xb085c00e,
   0xc00e7000,
   0x7000b084,
   0x88409850,
   0x45fb2200,
   0x7000b830
};

PATCH_FUN_SPEC void rf_patch_mce_ieee_s(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 256; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchZigbeeXsIsMce[i];
   }
#else
   const uint32_t *pS = patchZigbeeXsIsMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 32;

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
#endif
}

#endif
