/******************************************************************************
*  Filename:       rf_patch_rfe_genfsk.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC26x0 Generic FSK
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


#ifndef _RF_PATCH_RFE_GENFSK_H
#define _RF_PATCH_RFE_GENFSK_H

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

RFE_PATCH_TYPE patchGenfskRfe[431] = { 
   0x000061a9,
   0x1307147f,
   0x00080053,
   0x1f2e24f1,
   0x0ab03f13,
   0xff07003f,
   0x40004030,
   0x40034001,
   0x400f4007,
   0x40cf404f,
   0x43cf41cf,
   0x4fcf47cf,
   0x2fcf3fcf,
   0x0fcf1fcf,
   0x00000000,
   0x0008000f,
   0x003f0000,
   0x00400000,
   0x0000003f,
   0x00680004,
   0x00dc000e,
   0x00430006,
   0x0005001a,
   0x00000000,
   0x00000002,
   0x0000003f,
   0x00040000,
   0x000000c0,
   0x00c00004,
   0x00070000,
   0x9100c050,
   0xc0707000,
   0x70009100,
   0x00213182,
   0xb1109131,
   0x81017000,
   0xa100b101,
   0x91323182,
   0x9101b110,
   0x81411011,
   0x404f2241,
   0x700006f1,
   0x9101c051,
   0x39101830,
   0xd0083183,
   0x6f413118,
   0x91310031,
   0x1483b110,
   0x685b1614,
   0x10257000,
   0x9100c050,
   0xc0c0c3f4,
   0x6f031420,
   0x04411031,
   0x22f08250,
   0x26514070,
   0x3182c022,
   0x91310021,
   0x3963b110,
   0x04411031,
   0x3182c082,
   0x91310021,
   0x3963b110,
   0xc0a21031,
   0x00213182,
   0xb1109131,
   0x31151050,
   0x92051405,
   0x643f7000,
   0x1031c052,
   0x31610631,
   0x644202c1,
   0x1031c112,
   0x06713921,
   0x02e13151,
   0x70006442,
   0x659d658e,
   0x8220c088,
   0x39803950,
   0x40a31e00,
   0x3001c041,
   0x1a181418,
   0x8230c089,
   0x39803960,
   0x40ad1e00,
   0x3001c041,
   0x1a191419,
   0x9136643c,
   0x9134b110,
   0xb054b110,
   0xa0547100,
   0x80f0b064,
   0x40b32200,
   0x90b01240,
   0x8253b032,
   0x39533953,
   0x643f6489,
   0xc122c111,
   0xc1706442,
   0xc11168c6,
   0x6442c0c2,
   0x68cbc170,
   0x9100c050,
   0x92987227,
   0x16141615,
   0x10531042,
   0x8221c1f0,
   0x39313131,
   0x31313981,
   0xb270b051,
   0x72276576,
   0xb2709299,
   0x10731062,
   0x8231c3f0,
   0x39213121,
   0x101b3981,
   0xc0e26576,
   0x82603182,
   0x39803180,
   0xb0610002,
   0x91327100,
   0xa051b110,
   0x7227b061,
   0x68f7c230,
   0x12800000,
   0xb03290b0,
   0xc0507000,
   0x72279100,
   0x10629299,
   0xc3f01073,
   0x31218231,
   0x39813921,
   0xb270b051,
   0x72276582,
   0xb2709298,
   0x10531042,
   0x8221c1f0,
   0x39313131,
   0x31313981,
   0x6582101a,
   0xb061a051,
   0xc0b07227,
   0x643f691c,
   0xc122c101,
   0xc1016442,
   0x6442c0c2,
   0x643c1a15,
   0xb1109135,
   0x64898253,
   0x90b012c0,
   0x7000b032,
   0xc272643f,
   0x6442c081,
   0xc111c122,
   0xc0026442,
   0x6442c111,
   0xc331c062,
   0xc3626442,
   0x6442c111,
   0xc111c302,
   0x82536442,
   0x64893953,
   0x6447c3e2,
   0x41442211,
   0xc881c242,
   0xc2526442,
   0x6442c111,
   0xcee1c272,
   0xc2026442,
   0x6442c881,
   0xc801c202,
   0xc0b06442,
   0x70006958,
   0xc242643f,
   0x6442c801,
   0xc011c252,
   0xc2726442,
   0x6442c0e1,
   0xc101c002,
   0xc0626442,
   0x6442c301,
   0xc101c122,
   0xc3626442,
   0x6442c101,
   0xc101c302,
   0x82536442,
   0x70006489,
   0x7100b061,
   0x1c231412,
   0x91334d7e,
   0x7000b110,
   0xb1109132,
   0x70006976,
   0x7100b061,
   0x1c321813,
   0x9132498a,
   0x7000b110,
   0xb1109133,
   0x70006982,
   0x6447c0c2,
   0xc0c21015,
   0x64471612,
   0x14153141,
   0x3180c0c0,
   0x10541405,
   0x040478b0,
   0xc0e67000,
   0x82613186,
   0x0401cc00,
   0x10671416,
   0xc3f08261,
   0x14170401,
   0x73067000,
   0x720b7205,
   0xb050720e,
   0x80817100,
   0xa050b060,
   0x22418092,
   0x808045c9,
   0x0410c1f1,
   0x11011630,
   0x6c011401,
   0x61dd61dc,
   0x61df61de,
   0x61e161e0,
   0x61e461e2,
   0x61e861e6,
   0x633961ea,
   0x8091633c,
   0x0421c0f2,
   0x80823121,
   0x14122a42,
   0x11011632,
   0x6c011421,
   0x621161f3,
   0x633c61f9,
   0x621161f3,
   0x633c61f9,
   0x61eb61eb,
   0x61eb61eb,
   0x61eb61eb,
   0x61eb6498,
   0x61eb64fd,
   0x61eb652e,
   0x61eb655a,
   0x121061eb,
   0x720e90b0,
   0x72057306,
   0x90301210,
   0xcff061ad,
   0xc1d4673f,
   0xc3c0c003,
   0x78406454,
   0x78609150,
   0x78709210,
   0x78809220,
   0x78909230,
   0x78a09240,
   0x78509260,
   0x783091f0,
   0x82109190,
   0x06f03940,
   0x31101001,
   0x92001410,
   0xa0bc61eb,
   0xa054a0e2,
   0x225080f0,
   0x8040461b,
   0x472e2200,
   0xa0406213,
   0x318d822d,
   0x8210398d,
   0x0410c0f1,
   0x821a1009,
   0x041a394a,
   0x39808210,
   0x100e0410,
   0x10bc10ab,
   0x646310c2,
   0xcfe07229,
   0xb013673f,
   0x66cdb003,
   0xb050b053,
   0xb064b054,
   0x66abb013,
   0x22e08210,
   0x66b4463e,
   0x80417100,
   0x472e2201,
   0x221080f0,
   0x22f04651,
   0xb064471b,
   0x423e2231,
   0x66d3b063,
   0x22e08210,
   0x6676463e,
   0xb064623e,
   0x318f816f,
   0xdfd03d8f,
   0x673f92ff,
   0x80417100,
   0x472e2201,
   0x80f0b064,
   0x426b2250,
   0x8211b063,
   0x466622c1,
   0x670866d3,
   0x22d18211,
   0x66764658,
   0x81616258,
   0x31818172,
   0x31823d81,
   0xefc03d82,
   0x930292f1,
   0x6211673f,
   0x91c081b0,
   0x829781d3,
   0x18d3a290,
   0x0bf34e85,
   0x1ce31613,
   0x91c34aaa,
   0x143b81e3,
   0x1cba6296,
   0x1e234691,
   0x1ce34a91,
   0xb2904e91,
   0x42912207,
   0x1a1ba290,
   0x1ce3629c,
   0x91c34aaa,
   0x183b81e3,
   0x4ea61cab,
   0x4aa81c9b,
   0x42aa1cbc,
   0x821010b2,
   0x42a322d0,
   0x221080f0,
   0x646346aa,
   0x62aa66ab,
   0x629a10ab,
   0x629a109b,
   0x82307000,
   0x0410c0f1,
   0x7100b063,
   0x10bc6aae,
   0x7000b0e0,
   0x91c281b2,
   0x820181d2,
   0x81511812,
   0x82411812,
   0x3d813181,
   0x4ac41c12,
   0xb032b0e2,
   0x673fcfb0,
   0x1421c7f1,
   0xc8124ec8,
   0x91729162,
   0xb0e1b031,
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
   0xb0e24ae7,
   0x46eb2221,
   0xcfa0b032,
   0x2221673f,
   0xa0e242eb,
   0x8231b032,
   0xc0f03941,
   0x1e010401,
   0x161842f9,
   0x3010c010,
   0x47071c08,
   0x3c101060,
   0xc7f11006,
   0x4efd1461,
   0x9166c816,
   0x31818171,
   0x1c163d81,
   0x91764b04,
   0xc006b031,
   0x70001208,
   0x31818161,
   0x82403d81,
   0x18013980,
   0x4b1a1cf1,
   0x80b01401,
   0x471a22c0,
   0xb033b0bc,
   0x92f1ef90,
   0x673f930f,
   0xa0037000,
   0xb064b063,
   0x655ab0ef,
   0x80407100,
   0x472e2200,
   0x652eb064,
   0x7100a0ef,
   0x22008040,
   0xb064472e,
   0x623eb003,
   0x81628201,
   0x3d823182,
   0x92f1ef80,
   0x673f9302,
   0x655aa003,
   0x80a261eb,
   0x61eb6463,
   0x7100b050,
   0x92e061eb,
   0x220082d0,
   0xb2c04740,
   0x80a07000,
   0x435c22f0,
   0xc102b030,
   0xc0013162,
   0x1e0080a0,
   0x22f04355,
   0xf5d04356,
   0x39603160,
   0x10206356,
   0x6f131a10,
   0x16116e23,
   0x6b571612,
   0x00007000
};

PATCH_FUN_SPEC void rf_patch_rfe_genfsk(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 431; i++) {
      HWREG(RFC_RFERAM_BASE + 4 * i) = patchGenfskRfe[i];
   }
#else
   const uint32_t *pS = patchGenfskRfe;
   volatile unsigned long *pD = &HWREG(RFC_RFERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 53;

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
