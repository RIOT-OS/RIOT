/******************************************************************************
*  Filename:       rf_patch_rfe_ble.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC1350 Bluetooth Low Energy
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


#ifndef _RF_PATCH_RFE_BLE_H
#define _RF_PATCH_RFE_BLE_H

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

RFE_PATCH_TYPE patchBleRfe[311] = { 
   0x0000612c,
   0x0002147f,
   0x00050006,
   0x0008000f,
   0x00520048,
   0x003fff80,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x40004030,
   0x40034001,
   0x400f4007,
   0x40cf404f,
   0x43cf41cf,
   0x4fcf47cf,
   0x2fcf3fcf,
   0x0fcf1fcf,
   0x00000000,
   0x00000000,
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
   0x003f0000,
   0x00000000,
   0x00c00004,
   0x00040000,
   0x000000c0,
   0x00000007,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
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
   0x40632241,
   0x700006f1,
   0x9101c051,
   0x39101830,
   0xd0083183,
   0x6f413118,
   0x91310031,
   0x1483b110,
   0x686f1614,
   0x10257000,
   0x9100c050,
   0xc140c3f4,
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
   0x64537000,
   0x1031c052,
   0x31610631,
   0x645602c1,
   0x1031c112,
   0x06713921,
   0x02e13151,
   0x70006456,
   0x9101c051,
   0xc0e2cc01,
   0x64536456,
   0xc0c2c111,
   0xb0546456,
   0xa0547100,
   0x80f0b064,
   0x40b12200,
   0x90b01240,
   0xc2f0b032,
   0xc11168bc,
   0x6456c122,
   0x68c1c0b0,
   0x9101c051,
   0x3182c0e2,
   0x00028260,
   0xb1109132,
   0x68cbc230,
   0x00000000,
   0x12800000,
   0xb03290b0,
   0x64537000,
   0xc122c101,
   0xc1016456,
   0x6456c0c2,
   0x64998253,
   0x90b012c0,
   0x7000b032,
   0xc2726453,
   0x6456c081,
   0xc111c122,
   0xc0026456,
   0x6456c111,
   0xc331c062,
   0xc3626456,
   0x6456c111,
   0xc111c302,
   0x82536456,
   0x64993953,
   0x645bc3e2,
   0x40f62211,
   0xc881c242,
   0xc2526456,
   0x6456c111,
   0xcee1c272,
   0xc2026456,
   0x6456c881,
   0xc801c202,
   0xc0b06456,
   0x7000690a,
   0xc2426453,
   0x6456c801,
   0xc011c252,
   0xc2726456,
   0x6456c0e1,
   0xc101c002,
   0xc0626456,
   0x6456c301,
   0xc101c122,
   0xc3626456,
   0x6456c101,
   0xc101c302,
   0x82536456,
   0x70006499,
   0x00000000,
   0x00000000,
   0x72057306,
   0x720e720b,
   0x7100b050,
   0xb0608081,
   0x8092a050,
   0x92f1eff0,
   0x664f9302,
   0x45502241,
   0xc1f18080,
   0x16300410,
   0x14011101,
   0x61636c01,
   0x61656164,
   0x61676166,
   0x61696168,
   0x616d616b,
   0x6171616f,
   0x624c6249,
   0xc0f28091,
   0x31210421,
   0x2a428082,
   0x16321412,
   0x14211101,
   0x617e6c01,
   0x618461a0,
   0x617e624c,
   0x618461a0,
   0x6173624c,
   0x61736173,
   0x61736173,
   0x64a86173,
   0x64d36173,
   0x64e06173,
   0x650c6173,
   0x66596173,
   0x80826173,
   0x92f2dfe0,
   0xb0b0664f,
   0xb0b1617a,
   0x72057306,
   0x6130b030,
   0x664fcfd0,
   0xc003c284,
   0x6468c3c0,
   0x91507890,
   0x31107860,
   0x14107861,
   0x78509200,
   0x78613140,
   0x31400010,
   0x00107871,
   0x78b09210,
   0x78819260,
   0x78309221,
   0x78413140,
   0x92300010,
   0x91f0c010,
   0x61736655,
   0x80f0a054,
   0x45a82250,
   0x22008040,
   0x61a0463f,
   0x9160c800,
   0xb0508159,
   0x22418091,
   0xcfc04607,
   0x8212664f,
   0x39823182,
   0x10283942,
   0x82126477,
   0x041212f1,
   0x311f102f,
   0xc140142f,
   0x6f0d1420,
   0x10de396d,
   0x044ec3f4,
   0x3182c082,
   0x396d002e,
   0x3182c0a2,
   0x821a002d,
   0x8220398a,
   0x39803180,
   0x180bc00b,
   0x823078ac,
   0x10023940,
   0x3001c011,
   0x1801c010,
   0x31821802,
   0x26c10021,
   0xb0039191,
   0xb063b013,
   0x8203b053,
   0x80411439,
   0x463f2201,
   0x91c481b4,
   0x189581d5,
   0x4df51cb5,
   0x4a2d1cc5,
   0x80f09165,
   0x41e52210,
   0x913d6205,
   0x913eb110,
   0x9165b110,
   0x8159920f,
   0x18ab14f9,
   0x80f010bc,
   0x41a02250,
   0x46052210,
   0xcfb061e5,
   0xb063664f,
   0x10008230,
   0x0420c0f2,
   0xc0111002,
   0xc0103001,
   0x18021801,
   0x00213182,
   0x919126c1,
   0xb0139191,
   0xb063b003,
   0xb064b053,
   0x7100b054,
   0x22018041,
   0xb063463f,
   0x80f0b064,
   0x41a02250,
   0x91c181b1,
   0x189181d1,
   0xb0319161,
   0x8212621d,
   0x39823182,
   0x10283942,
   0x81596477,
   0x14598205,
   0xc00b8220,
   0x78ac180b,
   0x7100c080,
   0x6a3bb063,
   0x820161e5,
   0x31828162,
   0xefa03d82,
   0x930292f1,
   0xa003664f,
   0x80a26173,
   0x61736477,
   0x7100b050,
   0x92e06173,
   0x220082d0,
   0xb2c04650,
   0x80a07000,
   0x426c22f0,
   0xc102b030,
   0xc0013162,
   0x1e0080a0,
   0x22f04265,
   0xe6d04266,
   0x39603160,
   0x10206266,
   0x6f131a10,
   0x16116e23,
   0x6a671612,
   0x00007000
};

PATCH_FUN_SPEC void rf_patch_rfe_ble(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 311; i++) {
      HWREG(RFC_RFERAM_BASE + 4 * i) = patchBleRfe[i];
   }
#else
   const uint32_t *pS = patchBleRfe;
   volatile unsigned long *pD = &HWREG(RFC_RFERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 38;

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
