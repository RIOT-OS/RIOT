/******************************************************************************
*  Filename:       rf_patch_rfe_wb_dsss.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC13x0 Wideband DSSS
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


#ifndef _RF_PATCH_RFE_WB_DSSS_H
#define _RF_PATCH_RFE_WB_DSSS_H

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

RFE_PATCH_TYPE patchWb_dsssRfe[429] = { 
   0x000061a3,
   0x1307147f,
   0x24f1004d,
   0x3f131f2e,
   0x003f0ab0,
   0x0000ff07,
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
   0x6599658a,
   0x8220c088,
   0x39803950,
   0x409f1e00,
   0x3001c041,
   0x1a181418,
   0x8230c089,
   0x39803960,
   0x40a91e00,
   0x3001c041,
   0x1a191419,
   0x9136643c,
   0x9134b110,
   0xb054b110,
   0xa0547100,
   0x80f0b064,
   0x40af2200,
   0x90b01240,
   0x8253b032,
   0x39533953,
   0x643f6485,
   0xc122c111,
   0xc1706442,
   0xc11168c2,
   0x6442c0c2,
   0x68c7c170,
   0x9100c050,
   0x92987227,
   0x16141615,
   0x10531042,
   0x8221c1f0,
   0x39313131,
   0x31313981,
   0xb270b051,
   0x72276572,
   0xb2709299,
   0x10731062,
   0x8231c3f0,
   0x39213121,
   0x101b3981,
   0xc0e26572,
   0x82603182,
   0x39803180,
   0xb0610002,
   0x91327100,
   0xa051b110,
   0x7227b061,
   0x68f3c230,
   0x12800000,
   0xb03290b0,
   0xc0507000,
   0x72279100,
   0x10629299,
   0xc3f01073,
   0x31218231,
   0x39813921,
   0xb270b051,
   0x7227657e,
   0xb2709298,
   0x10531042,
   0x8221c1f0,
   0x39313131,
   0x31313981,
   0x657e101a,
   0xb061a051,
   0xc0b07227,
   0x643f6918,
   0xc122c101,
   0xc1016442,
   0x6442c0c2,
   0x643c1a15,
   0xb1109135,
   0x64858253,
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
   0x64853953,
   0x6447c3e2,
   0x41402211,
   0xc881c242,
   0xc2526442,
   0x6442c111,
   0xcee1c272,
   0xc2026442,
   0x6442c881,
   0xc801c202,
   0xc0b06442,
   0x70006954,
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
   0x70006485,
   0x7100b061,
   0x1c231412,
   0x91334d7a,
   0x7000b110,
   0xb1109132,
   0x70006972,
   0x7100b061,
   0x1c321813,
   0x91324986,
   0x7000b110,
   0xb1109133,
   0x7000697e,
   0x6447c0c2,
   0xc0c21015,
   0x64471612,
   0x14153141,
   0x3180c0c0,
   0x10541405,
   0x040478a0,
   0xc0e67000,
   0xcc013186,
   0x10671416,
   0xc3f08261,
   0x14170401,
   0x73067000,
   0x720b7205,
   0xb050720e,
   0x80817100,
   0xa050b060,
   0x22418092,
   0x808045c3,
   0x0410c1f1,
   0x11011630,
   0x6c011401,
   0x61d761d6,
   0x61d961d8,
   0x61db61da,
   0x61de61dc,
   0x61e261e0,
   0x633661e4,
   0x80916339,
   0x0421c0f2,
   0x80823121,
   0x14122a42,
   0x11011632,
   0x6c011421,
   0x620b61ee,
   0x633961f4,
   0x620b61ee,
   0x633961f4,
   0x61e661e6,
   0x61e661e6,
   0x61e661e6,
   0x61e66494,
   0x61e664f9,
   0x61e6652a,
   0x61e66556,
   0x61e66746,
   0x90b01210,
   0x7306720e,
   0x12107205,
   0x61a79030,
   0x673ccff0,
   0xc003c1d4,
   0x6454c3c0,
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
   0x67429200,
   0xa0bc61e6,
   0xa054a0e2,
   0x225080f0,
   0x80404615,
   0x472b2200,
   0xa040620d,
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
   0xb013673c,
   0x66c8b003,
   0xb050b053,
   0xb064b054,
   0x66a5b013,
   0x22e08210,
   0x66ae4638,
   0x80417100,
   0x472b2201,
   0x221080f0,
   0x22f0464b,
   0xb0644718,
   0x42382231,
   0x66ceb063,
   0x22e08210,
   0x66704638,
   0xb0646238,
   0x318f816f,
   0xdfd03d8f,
   0x673c92ff,
   0x80417100,
   0x472b2201,
   0x80f0b064,
   0x42652250,
   0x8211b063,
   0x466022c1,
   0x670566ce,
   0x22d18211,
   0x66704652,
   0x81616252,
   0x31818172,
   0x31823d81,
   0xefc03d82,
   0x930292f1,
   0x620b673c,
   0x91c081b0,
   0x829781d3,
   0x18d3a290,
   0x0bf34e7f,
   0x1ce31613,
   0x91c34aa4,
   0x143b81e3,
   0x1cba6290,
   0x1e23468b,
   0x1ce34a8b,
   0xb2904e8b,
   0x428b2207,
   0x1a1ba290,
   0x1ce36296,
   0x91c34aa4,
   0x183b81e3,
   0x4ea01cab,
   0x4aa21c9b,
   0x42a41cbc,
   0x821010b2,
   0x429d22d0,
   0x221080f0,
   0x646346a4,
   0x62a466a5,
   0x629410ab,
   0x6294109b,
   0x82307000,
   0x0410c0f1,
   0x7100b063,
   0x10bc6aa8,
   0x7000b0e0,
   0x91c281b2,
   0x820181d2,
   0x81511812,
   0x82411812,
   0x3d813181,
   0x4abf1c12,
   0xb032b0e2,
   0x92f2dfb0,
   0xc7f1673c,
   0x4ec31421,
   0x9162c812,
   0xb0319172,
   0x7000b0e1,
   0xc006c008,
   0x9160c800,
   0x70009170,
   0x81b08201,
   0x81d091c0,
   0x81511810,
   0x14061810,
   0x824280e1,
   0x3d823182,
   0x4ae41c20,
   0x2221b0e2,
   0xb03246e8,
   0xdfa01005,
   0x673c92f5,
   0x42e82221,
   0xb032a0e2,
   0x39418231,
   0x0401c0f0,
   0x42f61e01,
   0xc0101618,
   0x1c083010,
   0x10604704,
   0x10063c10,
   0x1461c7f1,
   0xc8164efa,
   0x81719166,
   0x3d813181,
   0x4b011c16,
   0xb0319176,
   0xc008c006,
   0x81617000,
   0x3d813181,
   0x39808240,
   0x1cf11801,
   0x14014b17,
   0x22c080b0,
   0xb0bc4717,
   0xef90b033,
   0x930f92f1,
   0x7000673c,
   0xb063a003,
   0xb0efb064,
   0x71006556,
   0x22008040,
   0xb064472b,
   0xa0ef652a,
   0x80407100,
   0x472b2200,
   0xb003b064,
   0x82016238,
   0x31828162,
   0xef803d82,
   0x930292f1,
   0xa003673c,
   0x61e66556,
   0x646380a2,
   0xb05061e6,
   0x61e67100,
   0x82d092e0,
   0x473d2200,
   0x7000b2c0,
   0x22f080a0,
   0xb0304359,
   0x3162c102,
   0x80a0c001,
   0x43521e00,
   0x435322f0,
   0x3160f5a0,
   0x63533960,
   0x1a101020,
   0x6e236f13,
   0x16121611,
   0x70006b54
};

PATCH_FUN_SPEC void rf_patch_rfe_wb_dsss(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 429; i++) {
      HWREG(RFC_RFERAM_BASE + 4 * i) = patchWb_dsssRfe[i];
   }
#else
   const uint32_t *pS = patchWb_dsssRfe;
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
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
#endif
}

#endif
