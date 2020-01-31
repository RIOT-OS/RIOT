/******************************************************************************
*  Filename:       rf_patch_mce_sl_longrange.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC13x0 Simplelink Long range
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

#ifndef _RF_PATCH_MCE_SL_LONGRANGE_H
#define _RF_PATCH_MCE_SL_LONGRANGE_H

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

MCE_PATCH_TYPE patchSl_longrangeMce[256] = { 
   0x146f603e,
   0x333c3c33,
   0x3cc3cccc,
   0x2fcf0005,
   0xdb3e0f9d,
   0x00007f7f,
   0x00020001,
   0x00000003,
   0x000c0003,
   0x00cc000f,
   0x003c00c3,
   0xcccc0033,
   0x33cccc33,
   0x0f003333,
   0x00000f0f,
   0x00070003,
   0x0000001f,
   0x04000000,
   0x0000000f,
   0x00010387,
   0x004348c0,
   0x80048000,
   0x000006f0,
   0x0524091e,
   0x00050054,
   0x48200800,
   0x00000048,
   0x7f7f001f,
   0x3c33014c,
   0xcccc333c,
   0x00003cc3,
   0x72487220,
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
   0x60766074,
   0x610e608a,
   0x60746074,
   0x60756074,
   0x606b1220,
   0x72201210,
   0x7310730f,
   0x81817311,
   0x91800010,
   0x6052b070,
   0x606a606a,
   0xc282c1e1,
   0x1820c470,
   0x6e236f13,
   0x16121611,
   0xc482687a,
   0x1820c810,
   0x12034087,
   0x16126e23,
   0xd0006884,
   0x606a9170,
   0x7310730f,
   0x91c0c000,
   0x8170c009,
   0x06703980,
   0x1e101610,
   0xc0b74499,
   0xc01ec008,
   0x1e2060aa,
   0xc0f7449f,
   0xc03ec018,
   0x1e4060aa,
   0xc13744a5,
   0xc07ec038,
   0x1e8060aa,
   0xc1774468,
   0xc0fec078,
   0x65f9c030,
   0x7100b0d8,
   0xa0d8b108,
   0xb480b200,
   0xb013b003,
   0xb012b002,
   0x39468146,
   0x0446c0f4,
   0xb0d01616,
   0xc13cc004,
   0x9290c070,
   0xc0707811,
   0x06321012,
   0x6f2314c2,
   0x71009493,
   0x3921b100,
   0x161468c2,
   0x44c01c64,
   0x0bf17811,
   0x1012c070,
   0x14c20632,
   0x94936f23,
   0xb1007100,
   0x68d13921,
   0xc6d5c4f4,
   0x81afc066,
   0x81e064f7,
   0x40e32210,
   0xc07060dd,
   0xc00f1610,
   0x68e564f7,
   0xc030a0d1,
   0xc0409760,
   0xb0f19780,
   0x7100b0c1,
   0xa0c1b0f1,
   0xa0037276,
   0x7220a002,
   0x061f606a,
   0x00f9316f,
   0x04411091,
   0x87da97c1,
   0x1091061a,
   0x97c10451,
   0x061b87db,
   0x14ba311b,
   0x147a3919,
   0x94936fa3,
   0x7100929e,
   0x7000b100,
   0x8ae4b0f8,
   0x93044112,
   0x9400c1f0,
   0x941078a0,
   0x8177721b,
   0x39871076,
   0x97c70677,
   0x161887d8,
   0x9867d040,
   0x782065f9,
   0x78309430,
   0x78409440,
   0x78509450,
   0x655a9460,
   0x98107890,
   0xb007656d,
   0xb104b017,
   0xc205b0d4,
   0x65e1c004,
   0x1c5465f2,
   0x91904938,
   0x81511614,
   0x41331e01,
   0x1c411671,
   0xa0d74d33,
   0x79cf79be,
   0x95ff95ee,
   0x65f21204,
   0x1c541614,
   0x9190414b,
   0xa2056145,
   0x65f9c050,
   0xa0d4a0d7,
   0x72027206,
   0x72037204,
   0x73057204,
   0x73767306,
   0x606a7276,
   0x3181c061,
   0xcff3c002,
   0x16116e12,
   0x16116e12,
   0x6e13c7e0,
   0x6e121611,
   0x69631611,
   0x6e12cff0,
   0x69691611,
   0xb0067000,
   0xb004b016,
   0xb002b014,
   0x7870b012,
   0x90509030,
   0x90407880,
   0xb2059060,
   0xc090b072,
   0xb11793b0,
   0xb116b0e7,
   0x7100b0e6,
   0xb107b073,
   0xa0e7a0e7,
   0x227080b0,
   0xa0e645a2,
   0xb0d7a0e7,
   0x7100b88e,
   0xb107b116,
   0xb061b041,
   0x93b0c0f0,
   0x8964b88f,
   0x95543114,
   0x7100a044,
   0xb04db107,
   0xb074b06d,
   0x7100b201,
   0x7000b107,
   0xb889b0d7,
   0x31848944,
   0x97243d84,
   0x97307860,
   0x69abc050,
   0xc2018740,
   0x3d601410,
   0x31648304,
   0x18043d64,
   0x3520cff0,
   0x93040404,
   0x93b0c0b0,
   0xd060b069,
   0x65f99864,
   0xb1077100,
   0x617da0d7,
   0x120a1209,
   0x140965d7,
   0x1e17141a,
   0x65d741d4,
   0x140a1419,
   0x41d41e37,
   0x140965d7,
   0x65d7141a,
   0x140a1419,
   0x3c8a3c89,
   0x71007000,
   0x7100b107,
   0x87f0b107,
   0x39803180,
   0x39818801,
   0x65c27000,
   0x10ac109b,
   0x10bd65c2,
   0x14db318d,
   0x318d10cd,
   0x318a14dc,
   0x149b14a9,
   0x95eb149c,
   0x700095fc,
   0xb079a0d7,
   0xb1047100,
   0x8820b0d7,
   0x98507000,
   0x22008840,
   0xb83045fa,
   0x00007000
};

PATCH_FUN_SPEC void rf_patch_mce_sl_longrange(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 256; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchSl_longrangeMce[i];
   }
#else
   const uint32_t *pS = patchSl_longrangeMce;
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
