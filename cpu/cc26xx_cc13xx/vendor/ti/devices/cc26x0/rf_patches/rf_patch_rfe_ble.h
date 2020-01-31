/******************************************************************************
*  Filename:       rf_patch_rfe_ble.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC26x0 BLE
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

RFE_PATCH_TYPE patchBleRfe[315] = { 
   0x00006154,
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
   0x00000000,
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
   0x22f08250,
   0x26514084,
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
   0x40b52200,
   0x90b01240,
   0xc2f0b032,
   0xc11168c0,
   0x6456c122,
   0x68c5c0b0,
   0x9101c051,
   0x3182c0e2,
   0x00028260,
   0xb1109132,
   0x39538253,
   0x649d3953,
   0x68d3c050,
   0x12800000,
   0xb03290b0,
   0x64537000,
   0xc122c101,
   0xc1016456,
   0x6456c0c2,
   0x649d8253,
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
   0x649d3953,
   0x645bc3e2,
   0x40fc2211,
   0xc881c242,
   0xc2526456,
   0x6456c111,
   0xcee1c272,
   0xc2026456,
   0x6456c881,
   0xc801c202,
   0xc0b06456,
   0x70006910,
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
   0x7000649d,
   0x3162c102,
   0x80a0c001,
   0x41361e00,
   0x61381a10,
   0x1a101020,
   0x6e236f13,
   0x16121611,
   0x70006938,
   0x82d092e0,
   0x453f2200,
   0x7000b2c0,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x72057306,
   0x720e720b,
   0x7100b050,
   0xb0608081,
   0x8092a050,
   0x92f1eff0,
   0x653e9302,
   0x45782241,
   0xc1f18080,
   0x16300410,
   0x14011101,
   0x618b6c01,
   0x618d618c,
   0x618f618e,
   0x61916190,
   0x61956193,
   0x61996197,
   0x62736270,
   0xc0f28091,
   0x31210421,
   0x2a428082,
   0x16321412,
   0x14211101,
   0x61a66c01,
   0x61ac61c7,
   0x61a66273,
   0x61ac61c7,
   0x619b6273,
   0x619b619b,
   0x619b619b,
   0x64ac619b,
   0x64d9619b,
   0x64e6619b,
   0x6512619b,
   0x652e619b,
   0x8082619b,
   0x92f2dfe0,
   0xb0b0653e,
   0xb0b161a2,
   0x72057306,
   0x6158b030,
   0x653ecfd0,
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
   0xa054619b,
   0x225080f0,
   0x804045cf,
   0x46662200,
   0xc80061c7,
   0x81599160,
   0x8091b050,
   0x462e2241,
   0x653ecfc0,
   0x31828212,
   0x39423982,
   0x64771028,
   0x12f18212,
   0x102f0412,
   0x142f311f,
   0x1420c140,
   0x396d6f0d,
   0xc3f410de,
   0xc082044e,
   0x002e3182,
   0xc0a2396d,
   0x002d3182,
   0x398a821a,
   0x31808220,
   0xc00b3980,
   0x78ac180b,
   0x39408230,
   0xc0111002,
   0xc0103001,
   0x18021801,
   0x00213182,
   0x919126c1,
   0xb013b003,
   0xb053b063,
   0x14398203,
   0x22018041,
   0x81b44666,
   0x81d591c4,
   0x1cb51895,
   0x1cc54e1c,
   0x91654a54,
   0x221080f0,
   0x622c420c,
   0xb110913d,
   0xb110913e,
   0x920f9165,
   0x14f98159,
   0x10bc18ab,
   0x225080f0,
   0x221041c7,
   0x620c462c,
   0x653ecfb0,
   0x8230b063,
   0xc0f21000,
   0x10020420,
   0x3001c011,
   0x1801c010,
   0x31821802,
   0x26c10021,
   0x91919191,
   0xb003b013,
   0xb053b063,
   0xb054b064,
   0x80417100,
   0x46662201,
   0xb064b063,
   0x225080f0,
   0x81b141c7,
   0x81d191c1,
   0x91611891,
   0x6244b031,
   0x31828212,
   0x39423982,
   0x64771028,
   0x82058159,
   0x82201459,
   0x180bc00b,
   0xc08078ac,
   0xb0637100,
   0x620c6a62,
   0x81628201,
   0x3d823182,
   0x92f1efa0,
   0x653e9302,
   0x619ba003,
   0x647780a2,
   0xb050619b,
   0x619b7100
};

PATCH_FUN_SPEC void rf_patch_rfe_ble(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 315; i++) {
      HWREG(RFC_RFERAM_BASE + 4 * i) = patchBleRfe[i];
   }
#else
   const uint32_t *pS = patchBleRfe;
   volatile unsigned long *pD = &HWREG(RFC_RFERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 39;

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
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
#endif
}

#endif
