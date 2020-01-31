/******************************************************************************
*  Filename:       rf_patch_cpe_ble_priv_1_2.h
*  Revised:        $Date: 2018-05-07 15:02:01 +0200 (ma, 07 mai 2018) $
*  Revision:       $Revision: 18438 $
*
*  Description:    RF Core patch file for CC1350 Bluetooth Low Energy with privacy 1.2 support
*
*  Copyright (c) 2015, Texas Instruments Incorporated
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

#ifndef _RF_PATCH_CPE_BLE_PRIV_1_2_H
#define _RF_PATCH_CPE_BLE_PRIV_1_2_H

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

#ifndef CPE_PATCH_TYPE
#define CPE_PATCH_TYPE static const uint32_t
#endif

#ifndef SYS_PATCH_TYPE
#define SYS_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif

#ifndef _APPLY_PATCH_TAB
#define _APPLY_PATCH_TAB
#endif


CPE_PATCH_TYPE patchImageBlePriv12[] = {
   0x210005fd,
   0x2100043d,
   0x2100044f,
   0x2100044f,
   0x21000475,
   0x210006c9,
   0x21000499,
   0x2100071f,
   0x490b4c0c,
   0x28ff7820,
   0x7ac8d101,
   0x20077020,
   0x4c08e006,
   0x28ff7820,
   0x21ffd007,
   0x49047021,
   0x0224240f,
   0x31604320,
   0x467082c8,
   0x47001c80,
   0x40086200,
   0x21000740,
   0xf8e8f000,
   0x0a0a9905,
   0xd1092a6c,
   0x61782008,
   0x1c406920,
   0x310a6038,
   0x91056120,
   0x61782000,
   0x0000bdf8,
   0x4905b570,
   0xb6724a05,
   0x28017908,
   0x2001dc02,
   0x1d127088,
   0x4710b662,
   0x21000298,
   0x00004a81,
   0x4605b5ff,
   0x4c03b085,
   0xb5ff4720,
   0x01deb085,
   0x47204c01,
   0x00003ff7,
   0x000041cb,
   0x4d53b5fe,
   0x462c4628,
   0x90003040,
   0x7e014627,
   0x78383760,
   0xd0022900,
   0xd10707c0,
   0x09c1e050,
   0x07c0d04e,
   0x7d20d14c,
   0xd5490640,
   0x31724629,
   0x20064a48,
   0x98004790,
   0x28007e00,
   0x7d20d007,
   0xd5010640,
   0xe0002003,
   0x26132001,
   0x6f68e008,
   0x28010f80,
   0x2006d002,
   0xe0014606,
   0x26072003,
   0x02312201,
   0x1a890412,
   0x02008a7a,
   0x43020412,
   0x35806f6b,
   0x68a89501,
   0x47a84d37,
   0x2e062201,
   0x2e07d002,
   0xe007d002,
   0xe00543c0,
   0x70797839,
   0x70394311,
   0x61089901,
   0xda012800,
   0x55022039,
   0x7e809800,
   0xd0022800,
   0x201e2106,
   0x6a61e002,
   0x201f1f89,
   0x6ca162a1,
   0x64e04788,
   0xbdfe2000,
   0x47804826,
   0x4822bdfe,
   0x78413060,
   0xd0022900,
   0x21007001,
   0x48217041,
   0x470038b0,
   0x4e1cb5f8,
   0x4635481f,
   0x7fec3540,
   0x09e14637,
   0x6db1d01a,
   0xd0172901,
   0x29007f69,
   0x07a1d002,
   0xe011d502,
   0xd10f07e1,
   0x06497d39,
   0x2103d50c,
   0x77e94321,
   0x6f314780,
   0x29010f89,
   0x2100d002,
   0x76793720,
   0xbdf877ec,
   0xbdf84780,
   0x31404909,
   0x28157508,
   0x281bd008,
   0x281dd008,
   0x490ad008,
   0x18400080,
   0x47706980,
   0x47704808,
   0x47704808,
   0x47704808,
   0x21000144,
   0x000100af,
   0x0000e801,
   0x00010603,
   0x0001018d,
   0x000114c0,
   0x210005b1,
   0x2100059b,
   0x210004d5,
   0x4e1ab5f8,
   0x6b714605,
   0x09cc4819,
   0x2d0001e4,
   0x4918d011,
   0x29027809,
   0x7b00d00f,
   0xb6724304,
   0x4f152001,
   0x47b80240,
   0x38204811,
   0x09c18800,
   0xd00407c9,
   0x7ac0e016,
   0x7b40e7f0,
   0x490fe7ee,
   0x61cc6374,
   0x07c00a40,
   0x2001d00c,
   0x6b310380,
   0xd0012d00,
   0xe0004301,
   0x46084381,
   0x49076331,
   0x63483940,
   0x47b82000,
   0xbdf8b662,
   0x21000280,
   0x21000088,
   0x2100029b,
   0x00003f7b,
   0x40044040,
   0x28004907,
   0x2004d000,
   0xb6724a06,
   0x07c97809,
   0x5810d001,
   0x2080e000,
   0xb240b662,
   0x00004770,
   0x2100026b,
   0x40046058,
   0x2041b510,
   0x00c0490e,
   0x490e4788,
   0x6b884602,
   0x24906b49,
   0x04c1014b,
   0x430b0ec9,
   0x4363490a,
   0x43597d49,
   0x689b4b09,
   0xfed3f7ff,
   0xb510bd10,
   0xfecaf7ff,
   0xd1010004,
   0xffe2f7ff,
   0xbd104620,
   0x00003c7d,
   0x40045080,
   0x21000280,
   0x40044000,
   0x000000ff,
};
#define _NWORD_PATCHIMAGE_BLE_PRIV_1_2 202

#define _NWORD_PATCHSYS_BLE_PRIV_1_2 0



#ifndef _BLE_PRIV_1_2_SYSRAM_START
#define _BLE_PRIV_1_2_SYSRAM_START 0x20000000
#endif

#ifndef _BLE_PRIV_1_2_CPERAM_START
#define _BLE_PRIV_1_2_CPERAM_START 0x21000000
#endif

#define _BLE_PRIV_1_2_SYS_PATCH_FIXED_ADDR 0x20000000

#define _BLE_PRIV_1_2_PARSER_PATCH_TAB_OFFSET 0x0338
#define _BLE_PRIV_1_2_PATCH_TAB_OFFSET 0x0340
#define _BLE_PRIV_1_2_IRQPATCH_OFFSET 0x03BC
#define _BLE_PRIV_1_2_PATCH_VEC_OFFSET 0x041C

PATCH_FUN_SPEC void enterBlePriv12CpePatch(void)
{
#if (_NWORD_PATCHIMAGE_BLE_PRIV_1_2 > 0)
   uint32_t *pPatchVec = (uint32_t *) (_BLE_PRIV_1_2_CPERAM_START + _BLE_PRIV_1_2_PATCH_VEC_OFFSET);

   memcpy(pPatchVec, patchImageBlePriv12, sizeof(patchImageBlePriv12));
#endif
}

PATCH_FUN_SPEC void enterBlePriv12SysPatch(void)
{
}

PATCH_FUN_SPEC void configureBlePriv12Patch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_BLE_PRIV_1_2_CPERAM_START + _BLE_PRIV_1_2_PATCH_TAB_OFFSET);


   pPatchTab[1] = 0;
   pPatchTab[112] = 1;
   pPatchTab[104] = 2;
   pPatchTab[105] = 3;
   pPatchTab[110] = 4;
   pPatchTab[65] = 5;
   pPatchTab[53] = 6;
   pPatchTab[48] = 7;
}

PATCH_FUN_SPEC void applyBlePriv12Patch(void)
{
   enterBlePriv12SysPatch();
   enterBlePriv12CpePatch();
   configureBlePriv12Patch();
}

PATCH_FUN_SPEC void refreshBlePriv12Patch(void)
{
   enterBlePriv12CpePatch();
   configureBlePriv12Patch();
}

PATCH_FUN_SPEC void rf_patch_cpe_ble_priv_1_2(void)
{
   applyBlePriv12Patch();
}


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  _RF_PATCH_CPE_BLE_PRIV_1_2_H

