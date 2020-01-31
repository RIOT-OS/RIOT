/******************************************************************************
*  Filename:       rf_patch_mce_hsp_4mbps.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC13x0 4Mbps High speed mode
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

#ifndef _RF_PATCH_MCE_HSP_4MBPS_H
#define _RF_PATCH_MCE_HSP_4MBPS_H

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

MCE_PATCH_TYPE patchHsp4mbpsMce[252] = { 
   0x00036075,
   0x0079000f,
   0x00000000,
   0x000c8000,
   0x0000000a,
   0x00780002,
   0x80000000,
   0x06700808,
   0x0b000000,
   0x00500104,
   0x00000000,
   0x01ff0000,
   0x04030000,
   0x017f7f26,
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
   0x05952fc8,
   0x00ff2f8f,
   0x00ff00ff,
   0x00ff00ff,
   0x00ff00ff,
   0x00faf320,
   0xaaaa0000,
   0xaaaaaaaa,
   0xaaaaaaaa,
   0x2000faf3,
   0x00580200,
   0x0f700288,
   0x00010fd0,
   0x0a400000,
   0x0d100002,
   0x0f590b08,
   0x04340f9a,
   0x067d0b08,
   0x09820f9a,
   0x0bcb0b08,
   0x00a60f9a,
   0x02ef0b08,
   0x00000f9a,
   0x00000000,
   0x00000000,
   0xc1020000,
   0xc0013162,
   0x1e008150,
   0x1a10406d,
   0x1020606f,
   0x6f131a10,
   0x16116e23,
   0x686f1612,
   0x72207000,
   0x7310730f,
   0x720c7311,
   0x720e720d,
   0x73057248,
   0x73767306,
   0xc7c07276,
   0xb0c09010,
   0xb0f07100,
   0x7218a0c0,
   0x10208132,
   0x06703952,
   0x16300020,
   0x14011101,
   0x60b16c01,
   0x60de60c3,
   0x60b1611b,
   0x60b160b1,
   0x60b160b2,
   0x60de60c3,
   0x60b1611b,
   0x60b160b1,
   0x60b160b2,
   0x60de60c3,
   0x60b1611b,
   0x60b160b1,
   0x60b160b2,
   0x60de60c3,
   0x60b1611b,
   0x60b160b1,
   0x60b660b2,
   0x60b66465,
   0x60b71220,
   0x72201210,
   0x00108181,
   0xb0709180,
   0x00006083,
   0x00000000,
   0x00000000,
   0xc0110000,
   0xc560c282,
   0x6f131820,
   0x16116e23,
   0x68c71612,
   0x95a07ce0,
   0xc4f1c100,
   0x6f13c622,
   0x16116e23,
   0x68d11612,
   0x000060b6,
   0x00000000,
   0x00000000,
   0x00000000,
   0xc3517220,
   0x9290c0f0,
   0x94926f12,
   0xb1081611,
   0x7100b0d8,
   0xa0d8b108,
   0xb480b200,
   0x91c0c140,
   0xb012b002,
   0xb013b003,
   0xb100c0c0,
   0x7100b0d0,
   0x6f12b100,
   0x94921611,
   0xa0d068f5,
   0xb483a480,
   0xb0c3b0f3,
   0x1220b0f1,
   0xc0c09760,
   0x71009780,
   0xb482a0c3,
   0xb0c1b760,
   0xb0f17100,
   0xa483a0c1,
   0xa003a760,
   0xc7c0a002,
   0x60b69010,
   0x00000000,
   0x00000000,
   0x00000000,
   0x72200000,
   0x91b0c140,
   0x94327c52,
   0x94427c42,
   0x94027c62,
   0x93227c72,
   0x7cd07cc6,
   0xc00093b0,
   0xc3f09370,
   0xc3f13180,
   0x95f10001,
   0x96119601,
   0x7ce095e0,
   0xb00695a0,
   0xb004b016,
   0xb002b014,
   0xb107b012,
   0x7100b0d7,
   0xb072a0d7,
   0x90307b20,
   0x7b309050,
   0x90609040,
   0x72767b44,
   0x9762c022,
   0x9780c0e0,
   0x1e108210,
   0xc030454c,
   0xcb4065f2,
   0x65d36952,
   0xa0c5b0f5,
   0xb0f8b201,
   0xb0e0b110,
   0xb0737100,
   0xb072b760,
   0xb0619044,
   0xa0e0b110,
   0xb202a0c5,
   0x7ca2b074,
   0xb0c19362,
   0x7100b0f1,
   0x9780c080,
   0x88907388,
   0x88a194e0,
   0x936694f1,
   0x7100b0f1,
   0x9780c310,
   0xb0f1b3b5,
   0xc0307100,
   0x7c809780,
   0xb0f19320,
   0xc0207100,
   0xb0459780,
   0xb0f1b065,
   0xb04f7100,
   0xb0f1b06f,
   0x7276a0c1,
   0x97701240,
   0x9780fff0,
   0x9760c380,
   0xb0c8a764,
   0x7100b889,
   0xa0c8b0f8,
   0xd0408152,
   0x65f29862,
   0x97823112,
   0xb0c6b0f6,
   0xb0f67100,
   0xb107a0c6,
   0x7100b0d7,
   0x7100b107,
   0x7276b107,
   0x7100c0f0,
   0x7100b107,
   0xb5b0b107,
   0xa0d769a9,
   0x8a738a62,
   0x9862e050,
   0x65f29873,
   0xa202a201,
   0x31828942,
   0x84e73d82,
   0x313784f8,
   0x31383d37,
   0xe0603d38,
   0x98789867,
   0xc00065f2,
   0x93609370,
   0x94f094e0,
   0x73067305,
   0x72047203,
   0xa002a004,
   0x9010c7c0,
   0x7cb260b6,
   0x12f09362,
   0x738869d6,
   0x94e08890,
   0x94f188a1,
   0x7c929366,
   0xb0539362,
   0x31371007,
   0x10183d37,
   0x3d383138,
   0x9867e070,
   0x65f29878,
   0x00007000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x88409850,
   0x45f32200,
   0x7000b830
};

PATCH_FUN_SPEC void rf_patch_mce_hsp_4mbps(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 252; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchHsp4mbpsMce[i];
   }
#else
   const uint32_t *pS = patchHsp4mbpsMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 31;

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
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
#endif
}

#endif
