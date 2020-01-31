/******************************************************************************
*  Filename:       rf_patch_mce_iqdump.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC13x0 IQ data
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

#ifndef _RF_PATCH_MCE_IQDUMP_H
#define _RF_PATCH_MCE_IQDUMP_H

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

MCE_PATCH_TYPE patchIqdumpMce[333] = { 
   0x0000602d,
   0x3f9d2fcf,
   0x003f0001,
   0x0fffff00,
   0xf80007ff,
   0x00000300,
   0x00170003,
   0x00003d1f,
   0x08000000,
   0x0000000f,
   0x00000387,
   0x00434074,
   0x80828000,
   0x000006f0,
   0x0510091e,
   0x00070054,
   0x50140000,
   0x00000050,
   0x0c30c02f,
   0x0000017f,
   0x00000000,
   0x0000aa00,
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
   0x6081607e,
   0x60fa60ec,
   0x607e607e,
   0x607e607e,
   0x6081607e,
   0x619e60ec,
   0x607e607e,
   0x607e607e,
   0x6081607e,
   0x610a60ec,
   0x607e607e,
   0x607e607e,
   0x6081607e,
   0x61ba60ec,
   0x607e607e,
   0x607e607e,
   0x6081607e,
   0x614260ec,
   0x72201210,
   0x7310730f,
   0x81817311,
   0x91800010,
   0x6042b070,
   0x66896074,
   0xc0306074,
   0xc0c16674,
   0xc470c282,
   0x6f131820,
   0x16116e23,
   0x68871612,
   0x98107840,
   0xc5a0c482,
   0x40961820,
   0x6e231203,
   0x68931612,
   0x81606074,
   0x81409490,
   0x2a703980,
   0x16111001,
   0x84448432,
   0xc0f5c0f3,
   0x1c01c200,
   0xc10040bb,
   0x40b11c10,
   0x10134cb3,
   0x18301803,
   0x1a101a13,
   0x68ae3912,
   0x13f360bb,
   0x13f360bb,
   0xc1001015,
   0x1a151850,
   0x39141a10,
   0xb0d868b9,
   0xb1087100,
   0xb200a0d8,
   0xb003b480,
   0xb002b013,
   0x7229b012,
   0x7100b0d0,
   0x8140b100,
   0x71009290,
   0x8140b100,
   0x44cb22f0,
   0x1c0313f0,
   0x929340d7,
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
   0x6674c040,
   0x91c0c100,
   0xb4836497,
   0xb0c3b0f3,
   0xa0c37100,
   0x607464de,
   0xa0e0a0c2,
   0x730fa0e3,
   0x65fe7310,
   0x6674c050,
   0xc035b0c2,
   0x99c57100,
   0xb074b888,
   0x6104b0f2,
   0xa0e0a0c2,
   0x730fa0e3,
   0x65fe7310,
   0xc000c18b,
   0x120c91b0,
   0x787a1218,
   0x789e788d,
   0xb07410a9,
   0xc020b0c2,
   0x7100b0f2,
   0xc060691c,
   0xc0356674,
   0x7100b0f2,
   0x8a3099c5,
   0x8ad16593,
   0x412f2201,
   0x1ca81080,
   0x1208452e,
   0x658a1618,
   0x65938a40,
   0x22018ad1,
   0x1090413a,
   0x1e091a19,
   0x10a9453a,
   0x8154658a,
   0x41221e04,
   0x1c4c14bc,
   0x61224e5d,
   0xa0e0a0c2,
   0x730fa0e3,
   0x65fe7310,
   0x120c721b,
   0xb0741205,
   0xc020b0c2,
   0x7100b0f2,
   0xc070694e,
   0x78ad6674,
   0xb0f2881e,
   0x8ac07100,
   0x415e2200,
   0x22108200,
   0xb201455e,
   0x8902988d,
   0x3d823182,
   0x31808940,
   0x18023d80,
   0x1e0e063e,
   0x1e2e4180,
   0x1e3e4172,
   0x10564179,
   0x3d161426,
   0x61811065,
   0x31261056,
   0x14261856,
   0x10653d26,
   0x10566181,
   0x18563136,
   0x3d361426,
   0x61811065,
   0x39761026,
   0x81549196,
   0x41551e04,
   0x1c4c161c,
   0x61554e5d,
   0xc0b01001,
   0x39119191,
   0x10001000,
   0x698c1000,
   0x31307000,
   0x1cd03d30,
   0x1ce04d9a,
   0x7000499c,
   0x700010d0,
   0x700010e0,
   0x6674c080,
   0xa0e0a0c2,
   0x730fa0e3,
   0x65fe7310,
   0xb0e0b110,
   0x80b07100,
   0x45ad2200,
   0xb20161a8,
   0x6674c090,
   0xa0e0b110,
   0xc035b0c2,
   0x99c57100,
   0xb074b888,
   0x61b4b0f2,
   0x6674c0a0,
   0xa0e0a0c2,
   0x730fa0e3,
   0x65fe7310,
   0xc000c18b,
   0x120c91b0,
   0x787a1218,
   0x789e788d,
   0xb11010a9,
   0x7100b0e0,
   0x220080b0,
   0x61cd45d2,
   0xb201b074,
   0x6674c0b0,
   0xa0e0b110,
   0xc020b0c2,
   0x7100b0f2,
   0xc03569da,
   0x7100b0f2,
   0x8a3099c5,
   0x8ad16593,
   0x41eb2201,
   0x1ca81080,
   0x120845ea,
   0x658a1618,
   0x65938a40,
   0x22018ad1,
   0x109041f6,
   0x1e091a19,
   0x10a945f6,
   0x8154658a,
   0x41de1e04,
   0x1c4c14bc,
   0x61de4e5d,
   0x6674c0c0,
   0xb006b0f8,
   0xb004b016,
   0xb002b014,
   0x8400b012,
   0x04207852,
   0x39838143,
   0x94732a73,
   0x1832c1f2,
   0x10213162,
   0x00123151,
   0x94000020,
   0x16101030,
   0x22103930,
   0x1220421c,
   0x10033150,
   0x16303180,
   0x12029350,
   0x22731204,
   0x8430422e,
   0x87d297c0,
   0x84501a82,
   0x87d497c0,
   0x62301a84,
   0x423b2263,
   0x97c08440,
   0x1a8087d0,
   0x84601402,
   0x87d097c0,
   0x14041a80,
   0x84406247,
   0x04107861,
   0x87d297c0,
   0x84601a42,
   0x04107861,
   0x87d497c0,
   0x31521a44,
   0x39633154,
   0x16130633,
   0x38343832,
   0x39823182,
   0x00423184,
   0x78209572,
   0x90509030,
   0x90407830,
   0xb2059060,
   0x9140cd90,
   0xa2057000,
   0x7100b0f2,
   0xb0f2a0c2,
   0x8180b88c,
   0x392489a4,
   0x00043184,
   0xc0d09184,
   0x73766674,
   0x72487276,
   0x72027206,
   0x73057204,
   0x60747306,
   0x88409850,
   0x46752200,
   0x7000b830,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0xc1020000,
   0xc0013162,
   0x1e008150,
   0x1a104291,
   0x10206293,
   0x6f131a10,
   0x16116e23,
   0x6a931612,
   0x00007000
};

PATCH_FUN_SPEC void rf_patch_mce_iqdump(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 333; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchIqdumpMce[i];
   }
#else
   const uint32_t *pS = patchIqdumpMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 41;

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
