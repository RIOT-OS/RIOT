/******************************************************************************
*  Filename:       rf_patch_mce_brepeat.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC13x0 for 1.2kbps and 2.4kbps Generic FSK
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

#ifndef _RF_PATCH_MCE_BREPEAT_H
#define _RF_PATCH_MCE_BREPEAT_H

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

MCE_PATCH_TYPE patchBrepeatMce[302] = { 
   0x2fcf604e,
   0x030c3f9d,
   0x070c680a,
   0x00068080,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0xf80007c0,
   0x1f0000f8,
   0xe007003f,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00070003,
   0x00003d1f,
   0x04000000,
   0x0000000f,
   0x000b0387,
   0x004348f4,
   0x80078000,
   0x00000670,
   0x0510091e,
   0x00070054,
   0x1f080100,
   0x00000031,
   0x3030002f,
   0x0000027f,
   0x00000000,
   0x0000aa00,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
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
   0x60836c01,
   0x60a06084,
   0x60836178,
   0x60836083,
   0x12106083,
   0x730f7220,
   0x73117310,
   0x00108181,
   0xb0709180,
   0x60796063,
   0x6656c030,
   0xc282c1e1,
   0x1820c470,
   0x6e236f13,
   0x16121611,
   0x7830688a,
   0x78409ab0,
   0x78509ac0,
   0xc4829ad0,
   0x1820c5a0,
   0x1203409d,
   0x16126e23,
   0x7870689a,
   0x607997e0,
   0xb1087276,
   0xb0f3b100,
   0xb0d8b0f1,
   0xb1087100,
   0xb200a0d8,
   0x87e0b760,
   0xb0f19780,
   0x7100b0c1,
   0xb0f17276,
   0xb012a0c1,
   0xb013b002,
   0xb484b003,
   0xb101b0d1,
   0x7100722c,
   0x8140b101,
   0x06f03940,
   0x06f38143,
   0x65b18161,
   0x1a133911,
   0x68c04cc3,
   0x22f08140,
   0x814044bd,
   0x16103980,
   0x85e11203,
   0x1a111403,
   0x44d01e01,
   0x1c13c211,
   0xc31148fa,
   0x48eb1c13,
   0x10301813,
   0x1ef08451,
   0xc10040e5,
   0x1a101830,
   0x68e23911,
   0x65b11030,
   0x68e53911,
   0x12f08461,
   0xc21160f7,
   0x10301813,
   0x1ef08461,
   0xc10040f7,
   0x1a101830,
   0x68f43911,
   0x65b11030,
   0x68f73911,
   0x12f08431,
   0x391165b1,
   0x844168fc,
   0x65b112f0,
   0x69013911,
   0x81a1721c,
   0x81e065a4,
   0x41052220,
   0xc040a0d1,
   0xc0309780,
   0xb0c19760,
   0x7100b0f1,
   0xa0c1b0f1,
   0x72487276,
   0xa002a003,
   0x73057248,
   0x73767306,
   0xa2007276,
   0x9010c7c0,
   0xb0066079,
   0xb004b016,
   0xb002b014,
   0x7810b012,
   0x90509030,
   0x90407820,
   0xb2059060,
   0x83038ae2,
   0x65ba9302,
   0xb064857f,
   0xc00bc00c,
   0xb072b011,
   0xa0c0a0c1,
   0xb0e6b116,
   0x22d18ab1,
   0xb0f24141,
   0x7100b0c2,
   0xa760b073,
   0xb7607378,
   0x226080b0,
   0x661c454b,
   0xb88f6141,
   0x18f08960,
   0x100f9550,
   0xa0c2720e,
   0xb0d7b201,
   0x7100b107,
   0x8ad0b041,
   0x415d22e0,
   0x22208210,
   0xb04d4536,
   0xc300b06d,
   0xb2019070,
   0x6656c040,
   0x7000a044,
   0x8180b88c,
   0x392489a4,
   0x00043184,
   0xc0509184,
   0x73766656,
   0x72487276,
   0x72027206,
   0x73057204,
   0x60797306,
   0xb32d721b,
   0x6656c060,
   0x6521b0f8,
   0xb107b0d7,
   0xb1077100,
   0xb1077100,
   0xb1077100,
   0xb1077100,
   0x8090120a,
   0x44632200,
   0x161a6605,
   0x1e0b815b,
   0x85524189,
   0x9862d070,
   0x1cba6656,
   0x18ab4da2,
   0x499e1efb,
   0x1af010b0,
   0x699c6605,
   0xc0f0a205,
   0x69a06605,
   0x6166a0d7,
   0x45a82201,
   0x61a913f2,
   0x92c21212,
   0x710085e2,
   0x1a12b101,
   0x45ab1e02,
   0x22017000,
   0x13f245b5,
   0x121261b6,
   0x710092c2,
   0x7000b101,
   0x39838143,
   0x94732a73,
   0x1a1085e0,
   0x120f1613,
   0x69c2143f,
   0xc200120b,
   0x41c91c0f,
   0x791b61cb,
   0xc40b61cf,
   0x318b18fb,
   0x8400313b,
   0x04107941,
   0x940000b0,
   0xa405b404,
   0x1e4185e1,
   0x1e3141e9,
   0x841041e1,
   0x31103180,
   0x94203980,
   0x841061ee,
   0x39803180,
   0x31101001,
   0x94201410,
   0x841061ee,
   0x39803180,
   0x94203120,
   0x84301201,
   0x87d097c0,
   0x84401401,
   0x87d097c0,
   0xc1001401,
   0x31111801,
   0x70009571,
   0x22011202,
   0x3a324201,
   0x38326202,
   0x69fd3911,
   0x85e37000,
   0x71001202,
   0xb107b88d,
   0x31818b11,
   0x14123d81,
   0x31818b21,
   0x14123d81,
   0x1e031a13,
   0x22f24607,
   0x12124619,
   0x1202621a,
   0x70009192,
   0x22b08ab0,
   0x1e3b4622,
   0x62244654,
   0x46541e7b,
   0xb889c00b,
   0x31808940,
   0x16103d80,
   0x140c3d30,
   0x226080b0,
   0x70004230,
   0x39838ab3,
   0x8ab106f3,
   0x0401cff0,
   0x1c1c3031,
   0x12004e4c,
   0x1c0c1810,
   0x80b04a4e,
   0x42412260,
   0x10c27000,
   0x3c321612,
   0x83208ae1,
   0x42502210,
   0x93016252,
   0x7000b0f2,
   0x623d101c,
   0x623d100c,
   0x62491821,
   0x62491421,
   0x624a161b,
   0x88409850,
   0x46572200,
   0x7000b830
};

PATCH_FUN_SPEC void rf_patch_mce_brepeat(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 302; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchBrepeatMce[i];
   }
#else
   const uint32_t *pS = patchBrepeatMce;
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
   t6 = *pS++;
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
   *pD++ = t6;
#endif
}

#endif
