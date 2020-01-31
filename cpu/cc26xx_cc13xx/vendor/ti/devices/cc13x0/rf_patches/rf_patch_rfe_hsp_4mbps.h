/******************************************************************************
*  Filename:       rf_patch_rfe_hsp_4mbps.h
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


#ifndef _RF_PATCH_RFE_HSP_4MBPS_H
#define _RF_PATCH_RFE_HSP_4MBPS_H

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

RFE_PATCH_TYPE patchHsp4mbpsRfe[321] = { 
   0x0000611f,
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
   0x00000000,
   0x00000000,
   0x00c00004,
   0x00040000,
   0x000000c0,
   0x40300007,
   0x40014000,
   0x40074003,
   0x404f400f,
   0x41cf40cf,
   0x47cf43cf,
   0x3fcf4fcf,
   0x1fcf2fcf,
   0x02070fcf,
   0x4040067f,
   0x40404040,
   0x20403040,
   0x00401040,
   0x01c000c0,
   0x07c003c0,
   0x0fc10fc0,
   0x0fc70fc3,
   0xc0500fcf,
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
   0x06f14056,
   0xc0517000,
   0x18309101,
   0x31833910,
   0x3118d008,
   0x00316f41,
   0xb1109131,
   0x16141483,
   0x70006862,
   0xc0501025,
   0xc3f49100,
   0x1420c210,
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
   0xc0526446,
   0x06311031,
   0x02c13161,
   0xc1126449,
   0x39211031,
   0x31510671,
   0x644902e1,
   0xc0517000,
   0xcc019101,
   0x6449c0e2,
   0xc1116446,
   0x6449c0c2,
   0x7100b054,
   0xb064a054,
   0x220080f0,
   0xc11140a4,
   0x6449c122,
   0x9101c051,
   0x3182c0e2,
   0x00028260,
   0xb1109132,
   0x68b7c300,
   0x12406446,
   0xb03290b0,
   0x39538253,
   0x648c3953,
   0x68c1c360,
   0x90b01280,
   0x7000b032,
   0xc1016446,
   0x6449c122,
   0xc0c2c101,
   0x82536449,
   0x12c0648c,
   0xb03290b0,
   0x64467000,
   0xc081c272,
   0xc1226449,
   0x6449c111,
   0xc111c002,
   0xc0626449,
   0x6449c331,
   0xc111c362,
   0xc3026449,
   0x6449c111,
   0x39538253,
   0xc3e2648c,
   0x2211644e,
   0xc24240e9,
   0x6449c881,
   0xc111c252,
   0xc2726449,
   0x6449cee1,
   0xc881c202,
   0xc2026449,
   0x6449c801,
   0x68fdc0b0,
   0x64467000,
   0xc801c242,
   0xc2526449,
   0x6449c011,
   0xc0e1c272,
   0xc0026449,
   0x6449c101,
   0xc301c062,
   0xc1226449,
   0x6449c101,
   0xc101c362,
   0xc3026449,
   0x6449c101,
   0x648c8253,
   0x00007000,
   0x00000000,
   0x73060000,
   0x720b7205,
   0xb050720e,
   0xb0607100,
   0x8081a050,
   0x22418092,
   0x8080453e,
   0x0410c1f1,
   0x11011630,
   0x6c011401,
   0x61526151,
   0x61546153,
   0x61566155,
   0x61596157,
   0x615d615b,
   0x6151615f,
   0xc0f28091,
   0x31210421,
   0x2a428082,
   0x16321412,
   0x14211101,
   0x61706c01,
   0x6174618b,
   0x61706151,
   0x6174618b,
   0x61616151,
   0x61616161,
   0x61616161,
   0x649b6161,
   0x64c66161,
   0x64d36161,
   0x64ff6161,
   0x666e6161,
   0x12106161,
   0x616690b0,
   0x90b01220,
   0x7306720e,
   0x12107205,
   0x61239030,
   0x00000000,
   0x00000000,
   0xc003c024,
   0x645bc3c0,
   0x9159c4b9,
   0x9160c800,
   0x9200c2d0,
   0x9260c3f0,
   0x3140c060,
   0x0010c0f1,
   0xc0213140,
   0x92100010,
   0x9221c371,
   0x00006161,
   0x00000000,
   0x73060000,
   0x82188159,
   0x108f108e,
   0x041ec0f1,
   0x041f394f,
   0x664110f2,
   0x10f010f7,
   0x14f03110,
   0x822b1008,
   0x39808210,
   0x180a10ba,
   0x140c10bc,
   0x1210c04d,
   0x722490e0,
   0x8040a054,
   0x461c2200,
   0x221080f0,
   0x61a545ae,
   0x72248244,
   0x222180f1,
   0x7b104604,
   0x662c9190,
   0x1ca581d5,
   0x1cc549e1,
   0x1cb54dd4,
   0x18b549c7,
   0x4a001cd5,
   0x42001ce7,
   0x2204b240,
   0x1a174200,
   0x105061ee,
   0x180510b5,
   0x4a001cd5,
   0x42001cf7,
   0x2204b240,
   0x16174200,
   0x620061ee,
   0x42001ce7,
   0x91c518b5,
   0x107081e5,
   0x1ce01850,
   0x100749df,
   0x10e761ee,
   0x1cf761ee,
   0x10b14200,
   0x91c11851,
   0x107081e1,
   0x1cf01410,
   0x10074ded,
   0x10f761ee,
   0xa240b241,
   0x80f11072,
   0x46042221,
   0x82086641,
   0x9290c0c0,
   0xb051b061,
   0x92701250,
   0xb0617100,
   0x7227a051,
   0x22008040,
   0x621141ae,
   0x91907b20,
   0x8203662c,
   0x31858165,
   0xb0313d85,
   0x92f3eff0,
   0x66649305,
   0xb054a003,
   0x7100b050,
   0x22018041,
   0xb064461c,
   0x222080f0,
   0x6211418b,
   0xa050b060,
   0x31828162,
   0x82033d82,
   0x92f3efe0,
   0x66649302,
   0x6161720e,
   0x00000000,
   0x00000000,
   0xb013b003,
   0xb0531201,
   0x7100b050,
   0x22018041,
   0xb063461c,
   0x91c381b3,
   0x188581d5,
   0x91651895,
   0x00007000,
   0x00000000,
   0x10250000,
   0x9100c050,
   0x1420c330,
   0x10316f03,
   0x0401c3f0,
   0x3182c022,
   0x91310021,
   0x3963b110,
   0xc3f01031,
   0xc0820401,
   0x00213182,
   0xb1109131,
   0x10313963,
   0x3182c0a2,
   0x91310021,
   0x1050b110,
   0x14053115,
   0x70009205,
   0x82d092e0,
   0x46652200,
   0x7000b2c0,
   0x22f080a0,
   0xb0304281,
   0x3162c102,
   0x80a0c001,
   0x427a1e00,
   0x427b22f0,
   0x3160e820,
   0x627b3960,
   0x1a101020,
   0x6e236f13,
   0x16121611,
   0x70006a7c
};

PATCH_FUN_SPEC void rf_patch_rfe_hsp_4mbps(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 321; i++) {
      HWREG(RFC_RFERAM_BASE + 4 * i) = patchHsp4mbpsRfe[i];
   }
#else
   const uint32_t *pS = patchHsp4mbpsRfe;
   volatile unsigned long *pD = &HWREG(RFC_RFERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 40;

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
   *pD++ = t1;
#endif
}

#endif
