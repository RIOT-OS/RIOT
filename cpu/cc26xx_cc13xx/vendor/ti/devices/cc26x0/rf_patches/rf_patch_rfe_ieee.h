/******************************************************************************
*  Filename:       rf_patch_rfe_ieee.h
*  Revised:        $Date: 2019-01-31 15:04:25 +0100 (to, 31 jan 2019) $
*  Revision:       $Revision: 18842 $
*
*  Description: RF core patch for CC26x0 IEEE 802.15.4. Contains fix to correct RSSIMAXVAL calculation.
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


#ifndef _RF_PATCH_RFE_IEEE_H
#define _RF_PATCH_RFE_IEEE_H

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

RFE_PATCH_TYPE patchZigbeeXsIsRfe[304] = { 
   0x00006154,
   0x07f7177f,
   0x004507ff,
   0x0000000f,
   0x002e0004,
   0x0000003f,
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
   0xc122c111,
   0xc0516456,
   0xc0e29101,
   0x82603182,
   0x91320002,
   0xc300b110,
   0x645368c8,
   0x90b01240,
   0xc300b032,
   0xc24068ce,
   0x128068d0,
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
   0x40f82211,
   0xc881c242,
   0xc2526456,
   0x6456c111,
   0xcee1c272,
   0xc2026456,
   0x6456c881,
   0xc801c202,
   0xc0b06456,
   0x7000690c,
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
   0x41321e00,
   0x61341a10,
   0x1a101020,
   0x6e236f13,
   0x16121611,
   0x70006934,
   0x82d092e0,
   0x453b2200,
   0x7000b2c0,
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
   0x72057306,
   0x720e720b,
   0x7100b050,
   0xb0608081,
   0x8092a050,
   0x92f1ef90,
   0x653a9302,
   0x45782241,
   0xc1f18080,
   0x16300410,
   0x14011101,
   0x61896c01,
   0x618b618a,
   0x618d618c,
   0x618f618e,
   0x61936191,
   0x61976195,
   0x625d625a,
   0x31218091,
   0x2a428082,
   0x16321412,
   0x14211101,
   0x61a76c01,
   0x61ad61c1,
   0x61a7625d,
   0x61ad61c1,
   0x6199625d,
   0x61996199,
   0x61996199,
   0x64ac6199,
   0x64d56199,
   0x64e26199,
   0x650e6199,
   0x652a6199,
   0x80826199,
   0x92f2df80,
   0x1210653a,
   0x61a290b0,
   0x90b01220,
   0x72057306,
   0x90301210,
   0xcf706158,
   0xc284653a,
   0xc3c0c003,
   0x78506468,
   0x78609150,
   0x78613110,
   0x92001410,
   0x31407880,
   0x00107861,
   0x78713140,
   0x92100010,
   0x92207890,
   0x926078a0,
   0xa0546199,
   0x225080f0,
   0x804045c9,
   0x46502200,
   0xcf6061c1,
   0x821e653a,
   0x06f910e9,
   0x10ea394e,
   0x10ac06fa,
   0x06fe394e,
   0x647710c2,
   0x10cb822d,
   0x91907820,
   0xb013661b,
   0xb063b053,
   0xb054b050,
   0xb003b064,
   0x225080f0,
   0x710041c1,
   0x22018041,
   0x22414650,
   0xb06441ec,
   0x81b0b063,
   0x81df91c0,
   0x220080f0,
   0x8090464e,
   0x464e2240,
   0x18d310f3,
   0x0bf34e01,
   0x1ce31613,
   0x91c34a4e,
   0x143b81e3,
   0x1ce36206,
   0x91c34a4e,
   0x183b81e3,
   0x4e171cab,
   0x4a191c9b,
   0x424e1cbc,
   0x10b210bc,
   0x662b6477,
   0xb063662b,
   0xb0637100,
   0xb0637100,
   0x10ab61e2,
   0x109b620a,
   0x7837620a,
   0x18707840,
   0xc0011a10,
   0x16176e71,
   0x78376a20,
   0xc0061208,
   0x9160c800,
   0x10007000,
   0x10f01000,
   0x18108201,
   0x6d716d71,
   0x14061816,
   0x16176e70,
   0x1c177841,
   0x78374638,
   0x1e881618,
   0x1060464b,
   0x81513d30,
   0x80f11810,
   0x41c12251,
   0x81719160,
   0x3d813181,
   0x4a491c10,
   0xb0319170,
   0x70001278,
   0x10001000,
   0x61e2662b,
   0x81628201,
   0x3d823182,
   0x92f1ef50,
   0x653a9302,
   0x6199a003,
   0x647780a2,
   0xb0506199,
   0x61997100
};

PATCH_FUN_SPEC void rf_patch_rfe_ieee(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 304; i++) {
      HWREG(RFC_RFERAM_BASE + 4 * i) = patchZigbeeXsIsRfe[i];
   }
#else
   const uint32_t *pS = patchZigbeeXsIsRfe;
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
#endif
}

#endif
