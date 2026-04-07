/**
* Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file       bmi270_legacy.c
* @date       2023-05-03
* @version    v2.86.1
*
*/

/* BMI270_legacy offers same low-power features as in BMI160 */

/***************************************************************************/

/*!             Header files
 ****************************************************************************/
#include "bmi270_legacy.h"

/******************************************************************************/
/*!  @name         Structure Declaration                                      */
/******************************************************************************/

/*! @name Structure to define tap configuration */
struct bmi2_tap_config *bmi270_legacy_config;

/***************************************************************************/

/*!              Global Variable
 ****************************************************************************/

/*! @name  Global array that stores the configuration file of BMI270_LEGACY */
const uint8_t bmi270_legacy_config_file[] = {
    0xc8, 0x2e, 0x00, 0x2e, 0x80, 0x2e, 0x3c, 0xb2, 0xc8, 0x2e, 0x00, 0x2e, 0x80, 0x2e, 0x9b, 0x03, 0x80, 0x2e, 0xa4,
    0xb3, 0x80, 0x2e, 0xb8, 0x03, 0xc8, 0x2e, 0x00, 0x2e, 0x80, 0x2e, 0xc4, 0xb2, 0x50, 0x30, 0x21, 0x2e, 0x59, 0xf5,
    0x10, 0x30, 0x21, 0x2e, 0x4a, 0xf1, 0x21, 0x2e, 0x6a, 0xf5, 0x80, 0x2e, 0xe5, 0x01, 0x00, 0x5d, 0x01, 0x00, 0x22,
    0x00, 0x77, 0x00, 0x00, 0x10, 0x00, 0x10, 0xd1, 0x00, 0x45, 0xf5, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x45, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5d, 0x00, 0x00, 0x88, 0x00, 0x05, 0xe0, 0xaa, 0x38, 0x05, 0xe0, 0x90,
    0x30, 0x00, 0x00, 0x30, 0x0a, 0x80, 0x40, 0x10, 0x27, 0xe8, 0x73, 0x04, 0x30, 0x00, 0x02, 0x00, 0x01, 0x00, 0x30,
    0x10, 0x0b, 0x09, 0x08, 0xfa, 0x00, 0x96, 0x00, 0x4b, 0x09, 0x11, 0x00, 0x11, 0x00, 0x02, 0x00, 0x00, 0x00, 0x22,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x06, 0x00, 0x09, 0x00,
    0x82, 0x00, 0x06, 0x00, 0x06, 0x00, 0x08, 0x00, 0x50, 0x00, 0x00, 0x00, 0x4c, 0x04, 0x02, 0x00, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x32, 0x01, 0xe6, 0x78, 0x84, 0x00, 0x9c, 0x6c,
    0x07, 0x00, 0x64, 0x75, 0xaa, 0x7e, 0x5f, 0x05, 0xbe, 0x0a, 0x5f, 0x05, 0x96, 0xe8, 0xef, 0x41, 0x01, 0x00, 0x0c,
    0x00, 0x0c, 0x00, 0x4a, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x0c, 0x00, 0xf0, 0x3c, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x9a, 0x00, 0x9a,
    0x01, 0x9f, 0x01, 0x88, 0x01, 0x96, 0x01, 0x84, 0x01, 0x00, 0x0c, 0xff, 0x0f, 0x00, 0x04, 0xc0, 0x00, 0x5b, 0xf5,
    0x98, 0xf1, 0x7b, 0x01, 0x7a, 0x01, 0x1e, 0xf2, 0xfd, 0xf5, 0xfc, 0xf5, 0x76, 0x01, 0x7e, 0x01, 0x80, 0x00, 0xa0,
    0x00, 0x5f, 0xff, 0xc8, 0x00, 0xcb, 0x00, 0xcb, 0x00, 0xd2, 0x00, 0x81, 0x01, 0x69, 0xf5, 0xe0, 0x00, 0x3f, 0xff,
    0x19, 0xf4, 0x58, 0xf5, 0x66, 0xf5, 0xff, 0x00, 0x64, 0xf5, 0xc0, 0xf1, 0xba, 0xf1, 0xae, 0xf1, 0xa0, 0x00, 0xdf,
    0x00, 0xa2, 0x01, 0xfa, 0x00, 0xfc, 0x00, 0xff, 0x3f, 0xff, 0xfb, 0x00, 0x38, 0x00, 0x30, 0xb3, 0x01, 0xba, 0x01,
    0xbc, 0x01, 0xc2, 0x01, 0xca, 0x01, 0xff, 0x7f, 0xff, 0x01, 0x8e, 0x01, 0x74, 0xf7, 0x00, 0x40, 0x5f, 0x0f, 0xeb,
    0x00, 0x7f, 0xff, 0xc2, 0xf5, 0x68, 0xf7, 0x59, 0x0f, 0x4d, 0x0f, 0x53, 0x0f, 0x72, 0x0f, 0x58, 0xf7, 0x5b, 0xf7,
    0x75, 0x0f, 0x00, 0x80, 0x86, 0x00, 0x64, 0x0f, 0x77, 0x0f, 0xb3, 0xf1, 0x71, 0x0f, 0x6c, 0xf7, 0xb9, 0xf1, 0xc6,
    0xf1, 0x00, 0xe0, 0x00, 0xff, 0xd1, 0xf5, 0x79, 0x0f, 0x7c, 0x0f, 0xff, 0x03, 0x00, 0xfc, 0xf0, 0x3f, 0x8b, 0x00,
    0x90, 0x00, 0x95, 0x00, 0x92, 0x00, 0x98, 0x00, 0x8d, 0x00, 0xa2, 0x00, 0xb9, 0x00, 0x2d, 0xf5, 0xca, 0xf5, 0x7c,
    0x01, 0x20, 0xf2, 0xdb, 0x01, 0x00, 0x08, 0x00, 0xf8, 0xe8, 0x03, 0x01, 0x80, 0xb8, 0x7e, 0xe1, 0x7a, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xa9, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1a, 0x24, 0x22, 0x00, 0x80, 0x2e, 0x00, 0xb0, 0x1d, 0x52, 0x00, 0x2e, 0x60, 0x40, 0x41, 0x40, 0x0d, 0xbc,
    0x98, 0xbc, 0xc0, 0x2e, 0x01, 0x0a, 0x0f, 0xb8, 0x1f, 0x52, 0x53, 0x3c, 0x52, 0x40, 0x40, 0x40, 0x4b, 0x00, 0x82,
    0x16, 0x26, 0xb9, 0x01, 0xb8, 0x41, 0x40, 0x10, 0x08, 0x97, 0xb8, 0x01, 0x08, 0xc0, 0x2e, 0x11, 0x30, 0x01, 0x08,
    0x43, 0x86, 0x25, 0x40, 0x04, 0x40, 0xd8, 0xbe, 0x2c, 0x0b, 0x22, 0x11, 0x54, 0x42, 0x03, 0x80, 0x4b, 0x0e, 0xf6,
    0x2f, 0xb8, 0x2e, 0x21, 0x50, 0x10, 0x50, 0x23, 0x52, 0x05, 0x2e, 0xd5, 0x00, 0xfb, 0x7f, 0x00, 0x2e, 0x13, 0x40,
    0x93, 0x42, 0x41, 0x0e, 0xfb, 0x2f, 0x98, 0x2e, 0x91, 0x03, 0x98, 0x2e, 0x87, 0xcf, 0x01, 0x2e, 0x70, 0x01, 0x00,
    0xb2, 0xfb, 0x6f, 0x0b, 0x2f, 0x01, 0x2e, 0x69, 0xf7, 0xb1, 0x3f, 0x01, 0x08, 0x01, 0x30, 0xf0, 0x5f, 0x23, 0x2e,
    0x70, 0x01, 0x21, 0x2e, 0x69, 0xf7, 0x80, 0x2e, 0x2e, 0x02, 0xf0, 0x5f, 0xb8, 0x2e, 0x03, 0x2e, 0xfc, 0xf5, 0x01,
    0x2e, 0xc0, 0xf8, 0x25, 0x54, 0x27, 0x56, 0x82, 0x08, 0x0b, 0x2e, 0x69, 0xf7, 0xcb, 0x0a, 0x29, 0x58, 0x80, 0x90,
    0xdd, 0xbe, 0x4c, 0x08, 0x5f, 0xb9, 0x59, 0x22, 0x80, 0x90, 0x07, 0x2f, 0x03, 0x34, 0xc3, 0x08, 0xf2, 0x3a, 0x0a,
    0x08, 0x02, 0x35, 0xc0, 0x90, 0x4a, 0x0a, 0x48, 0x22, 0xc0, 0x2e, 0x23, 0x2e, 0xfc, 0xf5, 0x01, 0x2e, 0x7e, 0x01,
    0x01, 0x8a, 0xf0, 0x50, 0x50, 0x41, 0x1a, 0x25, 0x03, 0xbd, 0x71, 0x82, 0x02, 0xbe, 0x2f, 0xb9, 0x42, 0x42, 0x81,
    0xbd, 0x45, 0x41, 0x4f, 0xba, 0xf1, 0x7f, 0xbf, 0xb8, 0x0f, 0xb8, 0x5c, 0xb9, 0x24, 0x7f, 0x00, 0xb2, 0x31, 0x7f,
    0xeb, 0x7f, 0xd2, 0x7f, 0x08, 0x2f, 0x10, 0x6f, 0x00, 0x90, 0x0b, 0x2f, 0x20, 0x6f, 0x00, 0x90, 0x08, 0x2f, 0x30,
    0x6f, 0x00, 0x90, 0x05, 0x2f, 0x01, 0x30, 0x23, 0x2e, 0x7d, 0x00, 0xd0, 0x6f, 0x98, 0x2e, 0x95, 0xcf, 0x05, 0x2e,
    0x7d, 0x00, 0x80, 0x90, 0x00, 0x30, 0x2b, 0x52, 0x2f, 0x56, 0x07, 0x2f, 0x2b, 0x58, 0x00, 0x2e, 0x10, 0x43, 0x63,
    0x0e, 0xfc, 0x2f, 0x81, 0x84, 0x25, 0x2e, 0x7d, 0x00, 0x09, 0x2e, 0x7e, 0x01, 0x02, 0x85, 0x13, 0x41, 0x04, 0x41,
    0xb1, 0xbd, 0xb1, 0xb9, 0x44, 0xbe, 0x82, 0x40, 0x44, 0xba, 0x24, 0xbd, 0xb3, 0x7f, 0x5c, 0x05, 0x24, 0xb9, 0x01,
    0x56, 0x2d, 0x58, 0x92, 0x7f, 0xa5, 0x7f, 0x80, 0x7f, 0xc0, 0x7f, 0x73, 0x7f, 0x64, 0x7f, 0x00, 0x2e, 0xf2, 0x6f,
    0x40, 0x7f, 0x51, 0x7f, 0x00, 0x2e, 0x90, 0x40, 0xf2, 0x7f, 0x00, 0x90, 0x04, 0x2f, 0x51, 0x6f, 0x00, 0x30, 0x40,
    0x42, 0x45, 0x2c, 0x62, 0x6f, 0xc1, 0x40, 0x98, 0x2e, 0x74, 0xc0, 0x51, 0x6f, 0x00, 0x2e, 0x43, 0x40, 0xc0, 0xb2,
    0x2d, 0x2f, 0x62, 0x6f, 0xa5, 0x6f, 0x84, 0x40, 0xc5, 0x0e, 0x07, 0x2f, 0x75, 0x6f, 0x10, 0x30, 0x45, 0x41, 0x40,
    0xa1, 0x05, 0x30, 0x05, 0x22, 0x20, 0x1a, 0x02, 0x2f, 0x00, 0x30, 0x40, 0x42, 0x2c, 0x2d, 0x10, 0x30, 0x18, 0x28,
    0x93, 0x6f, 0x40, 0x42, 0xc3, 0x0e, 0x25, 0x2f, 0xc0, 0x6f, 0x00, 0x90, 0x22, 0x2f, 0x45, 0x6f, 0x10, 0x30, 0xc5,
    0x14, 0x46, 0xbf, 0xb3, 0xbf, 0xb7, 0x0b, 0x2b, 0x58, 0x65, 0x01, 0x2f, 0x5e, 0x0b, 0x30, 0x25, 0x1a, 0x00, 0x2f,
    0x0b, 0x43, 0x01, 0x89, 0x2d, 0x2e, 0x79, 0x01, 0x67, 0x0e, 0xf7, 0x2f, 0x80, 0x7f, 0xc3, 0x7f, 0x0e, 0x2d, 0xb2,
    0x6f, 0xc2, 0x0e, 0x08, 0x2f, 0x10, 0x30, 0x40, 0x42, 0x02, 0x30, 0x74, 0x6f, 0x63, 0x6f, 0x04, 0x41, 0x00, 0xa1,
    0x02, 0x22, 0xc0, 0x42, 0x00, 0x2e, 0x62, 0x6f, 0x40, 0x6f, 0x73, 0x6f, 0x01, 0x80, 0xc1, 0x86, 0x81, 0x84, 0x41,
    0x82, 0x03, 0xa2, 0x62, 0x7f, 0x73, 0x7f, 0xa4, 0x2f, 0xeb, 0x6f, 0xd0, 0x6f, 0x81, 0x6f, 0x10, 0x5f, 0x80, 0x2e,
    0x95, 0xcf, 0x03, 0x2e, 0x7f, 0x01, 0x41, 0x84, 0x50, 0x50, 0x90, 0x40, 0x82, 0x40, 0x83, 0xbd, 0xbf, 0xb9, 0x2c,
    0xba, 0xfb, 0x7f, 0xc0, 0xb2, 0xe4, 0x7f, 0x0b, 0x30, 0x36, 0x2f, 0x07, 0x2e, 0x7e, 0x00, 0xc0, 0x90, 0x04, 0x2f,
    0xc1, 0x86, 0x27, 0x2e, 0x7e, 0x00, 0x37, 0x2e, 0x7f, 0x00, 0xa4, 0xbd, 0x04, 0xbd, 0x34, 0xb8, 0x41, 0x40, 0x91,
    0xbc, 0x91, 0xb9, 0xb3, 0x7f, 0x24, 0xb9, 0x09, 0x52, 0xc0, 0x7f, 0xd2, 0x7f, 0x98, 0x2e, 0xb3, 0xc0, 0xd1, 0x6f,
    0xb2, 0x6f, 0x51, 0x28, 0x41, 0x0f, 0x11, 0x30, 0x0d, 0x2f, 0xc2, 0x0e, 0x07, 0x2e, 0x7f, 0x00, 0x19, 0x28, 0x04,
    0x2f, 0xc0, 0xa6, 0x04, 0x2f, 0x21, 0x2e, 0x7f, 0x00, 0x02, 0x2d, 0x21, 0x2e, 0x7f, 0x00, 0x04, 0x2c, 0x02, 0x30,
    0x02, 0x30, 0x25, 0x2e, 0x7f, 0x00, 0xc0, 0x6f, 0x07, 0x2e, 0x7f, 0x00, 0x58, 0x0f, 0xfb, 0x6f, 0xe0, 0x6f, 0xb0,
    0x5f, 0x4a, 0x22, 0x80, 0x2e, 0x95, 0xcf, 0xe0, 0x6f, 0x01, 0x30, 0x98, 0x2e, 0x95, 0xcf, 0x00, 0x30, 0x21, 0x2e,
    0x7e, 0x00, 0xfb, 0x6f, 0xb0, 0x5f, 0xb8, 0x2e, 0x21, 0x2e, 0x59, 0xf5, 0x10, 0x30, 0xc0, 0x2e, 0x21, 0x2e, 0x4a,
    0xf1, 0x80, 0x2e, 0x00, 0xc1, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x01,
    0x34, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x2e, 0xd5, 0x00, 0x16, 0xb8, 0x02, 0x34, 0x4a, 0x0c, 0x21, 0x2e, 0x2d, 0xf5, 0xc0, 0x2e, 0x23,
    0x2e, 0xd5, 0x00, 0x20, 0x50, 0xf6, 0x7f, 0xe7, 0x7f, 0x00, 0x2e, 0x35, 0x5c, 0x00, 0x2e, 0x87, 0x41, 0xff, 0xbf,
    0xff, 0xbb, 0xc0, 0x91, 0x02, 0x2f, 0x37, 0x30, 0x2f, 0x2e, 0x69, 0xf5, 0xb8, 0x8f, 0x06, 0x32, 0xc7, 0x41, 0xfe,
    0x09, 0xc0, 0xb3, 0x04, 0x2f, 0x17, 0x30, 0x2f, 0x2e, 0x95, 0x01, 0x2d, 0x2e, 0x61, 0xf5, 0xf6, 0x6f, 0xe7, 0x6f,
    0xe0, 0x5f, 0xc8, 0x2e, 0x30, 0x50, 0xe5, 0x7f, 0xf6, 0x7f, 0xd7, 0x7f, 0x00, 0x2e, 0x35, 0x5a, 0x00, 0x2e, 0x46,
    0x41, 0x6f, 0xbf, 0x6f, 0xbb, 0x80, 0x91, 0x02, 0x2f, 0x36, 0x30, 0x2d, 0x2e, 0x69, 0xf5, 0x46, 0x30, 0x0f, 0x2e,
    0xa4, 0xf1, 0xbe, 0x09, 0x77, 0x8b, 0x80, 0xb3, 0x06, 0x2f, 0x0d, 0x2e, 0x6b, 0x01, 0x84, 0xaf, 0x02, 0x2f, 0x16,
    0x30, 0x2d, 0x2e, 0xde, 0x00, 0x86, 0x30, 0x46, 0x43, 0x00, 0x2e, 0xf6, 0x6f, 0xe5, 0x6f, 0xd7, 0x6f, 0xd0, 0x5f,
    0xc8, 0x2e, 0x10, 0x50, 0xfb, 0x7f, 0x98, 0x2e, 0x56, 0xc7, 0x98, 0x2e, 0x49, 0xc3, 0x02, 0x30, 0x30, 0x30, 0x11,
    0x30, 0xfb, 0x6f, 0xf0, 0x5f, 0x25, 0x2e, 0x7e, 0x00, 0x25, 0x2e, 0xce, 0x00, 0x21, 0x2e, 0xdf, 0x00, 0x23, 0x2e,
    0x7c, 0x00, 0x23, 0x2e, 0xfe, 0x00, 0x21, 0x2e, 0x78, 0x01, 0xb8, 0x2e, 0x11, 0x30, 0x81, 0x08, 0x01, 0x2e, 0x6a,
    0xf7, 0x71, 0x3f, 0x23, 0xbd, 0x01, 0x08, 0x02, 0x0a, 0xc0, 0x2e, 0x21, 0x2e, 0x6a, 0xf7, 0x40, 0x50, 0x98, 0x2e,
    0xbd, 0x0e, 0x50, 0x32, 0x98, 0x2e, 0x48, 0x03, 0x00, 0x30, 0xf0, 0x7f, 0xe0, 0x7f, 0x21, 0x2e, 0x69, 0xf5, 0x00,
    0x2e, 0x00, 0x2e, 0xd0, 0x2e, 0x00, 0x2e, 0x01, 0x80, 0x08, 0xa2, 0xfb, 0x2f, 0x05, 0x2e, 0xa8, 0x00, 0x2f, 0xbc,
    0xae, 0xbc, 0xad, 0xbd, 0x0f, 0xb8, 0x9f, 0xb8, 0x01, 0x0a, 0x05, 0x2e, 0x93, 0x00, 0xbf, 0xb9, 0xaf, 0xb8, 0x03,
    0x0a, 0x01, 0x0a, 0x21, 0x2e, 0x7a, 0x00, 0x98, 0x2e, 0xeb, 0xb5, 0x03, 0x2e, 0x9d, 0x01, 0x21, 0x2e, 0x7b, 0x00,
    0x40, 0xb2, 0x1a, 0x2f, 0x05, 0x2e, 0x8a, 0x00, 0x01, 0x52, 0x98, 0x2e, 0xc7, 0xc1, 0x03, 0x2e, 0xa8, 0x00, 0x9c,
    0xbc, 0x9f, 0xb8, 0x23, 0x2e, 0xdd, 0x00, 0x40, 0x90, 0xf0, 0x7f, 0x03, 0x2f, 0x01, 0x52, 0x98, 0x2e, 0x4c, 0xb6,
    0x03, 0x2d, 0x03, 0x52, 0x98, 0x2e, 0x4c, 0xb6, 0xe0, 0x7f, 0x98, 0x2e, 0x4b, 0x02, 0x10, 0x30, 0x21, 0x2e, 0x7c,
    0x00, 0x01, 0x2e, 0x99, 0x01, 0x00, 0xb2, 0x07, 0x2f, 0x01, 0x2e, 0x7b, 0x00, 0x00, 0xb2, 0x03, 0x2f, 0x05, 0x50,
    0x07, 0x52, 0x98, 0x2e, 0x07, 0xcc, 0x01, 0x2e, 0x87, 0x01, 0x00, 0xb2, 0x35, 0x2f, 0x05, 0x2e, 0x8a, 0x00, 0x09,
    0x52, 0x98, 0x2e, 0xc7, 0xc1, 0x03, 0x2e, 0x93, 0x01, 0x40, 0xb2, 0xf0, 0x7f, 0x08, 0x2f, 0x01, 0x2e, 0x7b, 0x00,
    0x00, 0xb2, 0x04, 0x2f, 0x00, 0x30, 0x21, 0x2e, 0x93, 0x01, 0x98, 0x2e, 0x88, 0xb5, 0x01, 0x2e, 0x79, 0x00, 0x00,
    0xb2, 0x1e, 0x2f, 0x98, 0x2e, 0x78, 0xb4, 0x98, 0x2e, 0x49, 0xb5, 0x09, 0x50, 0x98, 0x2e, 0x4d, 0xc3, 0x09, 0x50,
    0x98, 0x2e, 0x5a, 0xc7, 0x98, 0x2e, 0xf9, 0x02, 0x81, 0x3f, 0x01, 0x2e, 0x79, 0x01, 0x01, 0x08, 0x09, 0x52, 0xd0,
    0x7f, 0x21, 0x2e, 0x79, 0x01, 0x98, 0x2e, 0xff, 0xc5, 0xd1, 0x6f, 0x08, 0x0a, 0x21, 0x2e, 0x79, 0x01, 0x98, 0x2e,
    0xc3, 0xb1, 0x10, 0x30, 0x21, 0x2e, 0x7c, 0x00, 0x01, 0x2e, 0x95, 0x01, 0x00, 0xb2, 0x04, 0x2f, 0x98, 0x2e, 0x2e,
    0x02, 0x00, 0x30, 0x21, 0x2e, 0x95, 0x01, 0x01, 0x2e, 0x6b, 0x01, 0x04, 0xae, 0x0b, 0x2f, 0x01, 0x2e, 0x87, 0x01,
    0x00, 0xb2, 0x07, 0x2f, 0x09, 0x52, 0x98, 0x2e, 0x74, 0x0e, 0x00, 0xb2, 0x02, 0x2f, 0x10, 0x30, 0x21, 0x2e, 0xd7,
    0x00, 0x01, 0x2e, 0xd7, 0x00, 0x00, 0x90, 0x90, 0x2e, 0x72, 0xb1, 0x01, 0x2e, 0x6e, 0x01, 0x00, 0xb2, 0x04, 0x2f,
    0x98, 0x2e, 0x15, 0x0e, 0x00, 0x30, 0x21, 0x2e, 0xde, 0x00, 0x01, 0x2e, 0xde, 0x00, 0x00, 0xb2, 0x12, 0x2f, 0x01,
    0x2e, 0x6b, 0x01, 0x00, 0x90, 0x02, 0x2f, 0x98, 0x2e, 0x05, 0x0e, 0x09, 0x2d, 0x98, 0x2e, 0x5d, 0x0d, 0x01, 0x2e,
    0x6b, 0x01, 0x04, 0x90, 0x02, 0x2f, 0x50, 0x32, 0x98, 0x2e, 0x48, 0x03, 0x00, 0x30, 0x21, 0x2e, 0xde, 0x00, 0x01,
    0x2e, 0xd6, 0x00, 0x00, 0xb2, 0x90, 0x2e, 0x8a, 0xb1, 0x01, 0x2e, 0xd6, 0x00, 0x01, 0x31, 0x01, 0x08, 0x00, 0xb2,
    0x04, 0x2f, 0x98, 0x2e, 0x47, 0xcb, 0x10, 0x30, 0x21, 0x2e, 0x7c, 0x00, 0x81, 0x30, 0x01, 0x2e, 0xd6, 0x00, 0x01,
    0x08, 0x00, 0xb2, 0x61, 0x2f, 0x03, 0x2e, 0x89, 0x00, 0x01, 0x2e, 0x6b, 0x01, 0x98, 0xbc, 0x98, 0xb8, 0x05, 0xb2,
    0x13, 0x58, 0x23, 0x2f, 0x07, 0x90, 0x0d, 0x54, 0x00, 0x30, 0x37, 0x2f, 0x15, 0x41, 0x04, 0x41, 0xdc, 0xbe, 0x44,
    0xbe, 0xdc, 0xba, 0x2c, 0x01, 0x61, 0x00, 0x13, 0x56, 0x4a, 0x0f, 0x0c, 0x2f, 0xd1, 0x42, 0x94, 0xb8, 0xc1, 0x42,
    0x11, 0x30, 0x05, 0x2e, 0x6a, 0xf7, 0x2c, 0xbd, 0x2f, 0xb9, 0x80, 0xb2, 0x08, 0x22, 0x98, 0x2e, 0xf5, 0x03, 0x21,
    0x2d, 0x61, 0x30, 0x23, 0x2e, 0x6b, 0x01, 0x98, 0x2e, 0xf5, 0x03, 0x00, 0x30, 0x21, 0x2e, 0x5a, 0xf5, 0x18, 0x2d,
    0xd1, 0x7f, 0x50, 0x30, 0x98, 0x2e, 0x48, 0x03, 0x13, 0x52, 0x0b, 0x50, 0x50, 0x42, 0x70, 0x30, 0x11, 0x54, 0x42,
    0x42, 0x7e, 0x82, 0xd2, 0x6f, 0x80, 0xb2, 0x42, 0x42, 0x05, 0x2f, 0x21, 0x2e, 0x6b, 0x01, 0x10, 0x30, 0x98, 0x2e,
    0xf5, 0x03, 0x03, 0x2d, 0x60, 0x30, 0x21, 0x2e, 0x6b, 0x01, 0x01, 0x2e, 0x6b, 0x01, 0x06, 0x90, 0x18, 0x2f, 0x01,
    0x2e, 0x78, 0x00, 0x0f, 0x54, 0x0b, 0x52, 0xd0, 0x7f, 0x98, 0x2e, 0x7a, 0xc1, 0xd1, 0x6f, 0x08, 0x1a, 0x40, 0x30,
    0x08, 0x2f, 0x21, 0x2e, 0x6b, 0x01, 0x20, 0x30, 0x98, 0x2e, 0x24, 0xb6, 0x50, 0x32, 0x98, 0x2e, 0x48, 0x03, 0x05,
    0x2d, 0x98, 0x2e, 0x1e, 0x0e, 0x00, 0x30, 0x21, 0x2e, 0x6b, 0x01, 0x41, 0x30, 0x01, 0x2e, 0xd6, 0x00, 0x01, 0x08,
    0x00, 0xb2, 0x26, 0x2f, 0x01, 0x2e, 0x71, 0x01, 0x00, 0xb2, 0x0d, 0x2f, 0x01, 0x90, 0x1d, 0x2f, 0x05, 0x2e, 0xbf,
    0x00, 0x07, 0x2e, 0x72, 0x01, 0xa1, 0x32, 0x17, 0x58, 0x98, 0x2e, 0xae, 0xb7, 0x00, 0x30, 0x21, 0x2e, 0x71, 0x01,
    0x12, 0x2d, 0x15, 0x58, 0xa3, 0x32, 0x10, 0x41, 0x11, 0x41, 0x18, 0xb9, 0x04, 0x41, 0x98, 0xbc, 0x41, 0x0a, 0x94,
    0x0a, 0x98, 0x2e, 0xa0, 0xb7, 0x11, 0x30, 0x21, 0x2e, 0x72, 0x01, 0x21, 0x2e, 0x7b, 0x01, 0x23, 0x2e, 0x71, 0x01,
    0x10, 0x30, 0x21, 0x2e, 0x7c, 0x00, 0x00, 0x30, 0x21, 0x2e, 0xd6, 0x00, 0x18, 0x2d, 0x01, 0x2e, 0x6b, 0x01, 0x03,
    0xaa, 0x01, 0x2f, 0x98, 0x2e, 0x2b, 0x0e, 0x01, 0x2e, 0x6b, 0x01, 0x3f, 0x80, 0x03, 0xa2, 0x01, 0x2f, 0x00, 0x2e,
    0x02, 0x2d, 0x98, 0x2e, 0x41, 0x0e, 0x30, 0x30, 0x98, 0x2e, 0x38, 0xb6, 0x00, 0x30, 0x21, 0x2e, 0xd7, 0x00, 0x50,
    0x32, 0x98, 0x2e, 0x48, 0x03, 0x01, 0x2e, 0x7c, 0x00, 0x00, 0xb2, 0x29, 0x2f, 0x98, 0x2e, 0xf5, 0xcb, 0x03, 0x2e,
    0x6c, 0x01, 0x19, 0x54, 0x41, 0x0a, 0xbc, 0x84, 0x23, 0x2e, 0x7a, 0x01, 0x83, 0x80, 0x74, 0x30, 0x23, 0x40, 0x05,
    0x40, 0x1b, 0x50, 0x81, 0x40, 0xdc, 0x08, 0x18, 0xb9, 0x11, 0x42, 0xd3, 0xbe, 0xe4, 0x6f, 0xc5, 0xbc, 0xdd, 0x0a,
    0x12, 0x42, 0x59, 0x0a, 0x11, 0x42, 0xd0, 0x7f, 0xf1, 0x31, 0x00, 0x40, 0xf2, 0x6f, 0x25, 0xbd, 0x01, 0x08, 0x02,
    0x0a, 0xc0, 0x7f, 0x98, 0x2e, 0xa8, 0xcf, 0x06, 0xbc, 0xc1, 0x6f, 0xd2, 0x6f, 0x08, 0x0a, 0x80, 0x42, 0x98, 0x2e,
    0x0c, 0x02, 0x00, 0x30, 0x21, 0x2e, 0x9d, 0x01, 0x21, 0x2e, 0x99, 0x01, 0x21, 0x2e, 0x7c, 0x00, 0x21, 0x2e, 0x87,
    0x01, 0x80, 0x2e, 0x09, 0xb0, 0x01, 0x2e, 0x80, 0x01, 0x01, 0x40, 0x90, 0x50, 0x9f, 0xbd, 0x13, 0xbd, 0xbf, 0xb9,
    0x2c, 0xb9, 0xfb, 0x7f, 0xe2, 0x7f, 0xc0, 0xb2, 0x0b, 0x30, 0x62, 0x2f, 0x07, 0x2e, 0xce, 0x00, 0x31, 0x54, 0xc0,
    0x90, 0x0b, 0x2f, 0x9b, 0x42, 0xc1, 0x86, 0x9b, 0x42, 0x27, 0x2e, 0xce, 0x00, 0x37, 0x2e, 0xcf, 0x00, 0x37, 0x2e,
    0xd0, 0x00, 0x37, 0x2e, 0xd1, 0x00, 0x8b, 0x42, 0x01, 0x84, 0x17, 0xbc, 0x83, 0x40, 0x32, 0xbd, 0x99, 0xbc, 0x9a,
    0xba, 0xba, 0xbd, 0x0e, 0xb8, 0x28, 0xb9, 0xba, 0xb9, 0x09, 0x58, 0x1a, 0x25, 0x0d, 0x2e, 0xcf, 0x00, 0x77, 0x82,
    0x85, 0x7f, 0xa6, 0x7f, 0x74, 0x7f, 0x93, 0x7f, 0xd2, 0x7f, 0xc0, 0x7f, 0x98, 0x2e, 0xd1, 0xc3, 0x03, 0x2e, 0xcf,
    0x00, 0x08, 0x1a, 0xb0, 0x7f, 0x01, 0x30, 0x01, 0x2f, 0x23, 0x2e, 0xd1, 0x00, 0x01, 0x2e, 0xd1, 0x00, 0xd1, 0x6f,
    0x41, 0x0e, 0x14, 0x2f, 0xc1, 0x6f, 0x40, 0xb2, 0x0b, 0x2f, 0x43, 0xb2, 0x09, 0x2f, 0x09, 0x54, 0x31, 0x56, 0x98,
    0x2e, 0x0b, 0xc4, 0x00, 0x90, 0x06, 0x2f, 0xb1, 0x6f, 0x23, 0x2e, 0xd0, 0x00, 0x03, 0x2d, 0xb1, 0x6f, 0x23, 0x2e,
    0xd0, 0x00, 0xd1, 0x6f, 0x23, 0x2e, 0xd1, 0x00, 0x03, 0x2e, 0xd1, 0x00, 0x41, 0x82, 0x23, 0x2e, 0xd1, 0x00, 0x09,
    0x50, 0x31, 0x52, 0x12, 0x40, 0x52, 0x42, 0x00, 0x2e, 0x12, 0x40, 0x52, 0x42, 0x00, 0x2e, 0x00, 0x40, 0x40, 0x42,
    0x00, 0x2e, 0x03, 0x2e, 0xd0, 0x00, 0xe0, 0x6f, 0x98, 0x2e, 0x95, 0xcf, 0xb1, 0x6f, 0x23, 0x2e, 0xcf, 0x00, 0x06,
    0x2d, 0x37, 0x2e, 0xce, 0x00, 0xe0, 0x6f, 0x01, 0x30, 0x98, 0x2e, 0x95, 0xcf, 0xfb, 0x6f, 0x70, 0x5f, 0xb8, 0x2e,
    0xd0, 0x50, 0x80, 0x7f, 0x91, 0x7f, 0xd7, 0x7f, 0xc5, 0x7f, 0xb3, 0x7f, 0xa2, 0x7f, 0xe4, 0x7f, 0xf6, 0x7f, 0x7b,
    0x7f, 0x00, 0x2e, 0x35, 0x50, 0x00, 0x2e, 0x01, 0x40, 0x9f, 0xbc, 0x9f, 0xb8, 0x40, 0x90, 0x02, 0x2f, 0x31, 0x30,
    0x23, 0x2e, 0x69, 0xf5, 0x38, 0x82, 0x61, 0x7f, 0x20, 0x30, 0x41, 0x40, 0x23, 0x2e, 0xd6, 0x00, 0x03, 0x2e, 0xd6,
    0x00, 0x08, 0x08, 0x00, 0xb2, 0x11, 0x2f, 0x33, 0x50, 0x1a, 0x25, 0x12, 0x40, 0x32, 0x7f, 0x73, 0x82, 0x12, 0x40,
    0x42, 0x7f, 0x00, 0x2e, 0x00, 0x40, 0x50, 0x7f, 0x98, 0x2e, 0x6a, 0xd6, 0x01, 0x2e, 0x61, 0xf7, 0x01, 0x31, 0x01,
    0x0a, 0x21, 0x2e, 0x61, 0xf7, 0x80, 0x30, 0x03, 0x2e, 0xd6, 0x00, 0x08, 0x08, 0x00, 0xb2, 0x40, 0x2f, 0x03, 0x2e,
    0x89, 0x00, 0x17, 0xbc, 0x96, 0xbc, 0x0f, 0xb8, 0x9f, 0xb8, 0x40, 0x90, 0x21, 0x2e, 0x6f, 0x01, 0x10, 0x30, 0x01,
    0x30, 0x2a, 0x2f, 0x03, 0x2e, 0x6b, 0x01, 0x44, 0xb2, 0x05, 0x2f, 0x47, 0xb2, 0x00, 0x30, 0x2d, 0x2f, 0x21, 0x2e,
    0xd6, 0x00, 0x2b, 0x2d, 0x03, 0x2e, 0xfd, 0xf5, 0x9e, 0xbc, 0x9f, 0xb8, 0x40, 0x90, 0x14, 0x2f, 0x03, 0x2e, 0xfc,
    0xf5, 0x99, 0xbc, 0x9f, 0xb8, 0x40, 0x90, 0x0e, 0x2f, 0x03, 0x2e, 0x49, 0xf1, 0x37, 0x54, 0x4a, 0x08, 0x40, 0x90,
    0x08, 0x2f, 0x98, 0x2e, 0xe9, 0x01, 0x00, 0xb2, 0x10, 0x30, 0x03, 0x2f, 0x50, 0x30, 0x21, 0x2e, 0x6b, 0x01, 0x10,
    0x2d, 0x98, 0x2e, 0x24, 0xb6, 0x00, 0x30, 0x21, 0x2e, 0xd6, 0x00, 0x0a, 0x2d, 0x05, 0x2e, 0x69, 0xf7, 0x2d, 0xbd,
    0x2f, 0xb9, 0x80, 0xb2, 0x01, 0x2f, 0x21, 0x2e, 0xd7, 0x00, 0x23, 0x2e, 0xd6, 0x00, 0x60, 0x6f, 0xe1, 0x31, 0x01,
    0x42, 0x00, 0x2e, 0xf6, 0x6f, 0xe4, 0x6f, 0x80, 0x6f, 0x91, 0x6f, 0xa2, 0x6f, 0xb3, 0x6f, 0xc5, 0x6f, 0xd7, 0x6f,
    0x7b, 0x6f, 0x30, 0x5f, 0xc8, 0x2e, 0xa0, 0x50, 0x82, 0x7f, 0x90, 0x7f, 0xd7, 0x7f, 0xc5, 0x7f, 0xb3, 0x7f, 0xa1,
    0x7f, 0xe4, 0x7f, 0xf6, 0x7f, 0x7b, 0x7f, 0x00, 0x2e, 0x35, 0x54, 0x00, 0x2e, 0x80, 0x40, 0x0f, 0xbc, 0x0f, 0xb8,
    0x00, 0x90, 0x02, 0x2f, 0x30, 0x30, 0x21, 0x2e, 0x69, 0xf5, 0xb7, 0x84, 0x62, 0x7f, 0x98, 0x2e, 0xe9, 0x01, 0x00,
    0xb2, 0x90, 0x2e, 0x7f, 0xb3, 0x05, 0x2e, 0xa0, 0x00, 0x03, 0x2e, 0x8c, 0x00, 0x01, 0x2e, 0x8e, 0x00, 0xa3, 0xbd,
    0x9f, 0xb8, 0x0f, 0xb8, 0x08, 0x0a, 0xbf, 0xb9, 0xa4, 0xbc, 0x03, 0x0a, 0x22, 0xbe, 0x9f, 0xb8, 0x07, 0x2e, 0x96,
    0x00, 0x41, 0x0a, 0x4f, 0xba, 0x05, 0x2e, 0x98, 0x00, 0xb3, 0xbd, 0x0c, 0x0b, 0x01, 0x2e, 0x90, 0x00, 0xbf, 0xb9,
    0x2f, 0xbd, 0x03, 0x2e, 0x9f, 0x00, 0x2f, 0xb9, 0x0f, 0xbc, 0xe3, 0x0a, 0x0f, 0xb8, 0x9f, 0xbc, 0x9a, 0x0a, 0x9f,
    0xb8, 0x90, 0x0a, 0x91, 0x0a, 0x25, 0x2e, 0x79, 0x00, 0x05, 0x2e, 0xc1, 0xf5, 0x2e, 0xbd, 0x2e, 0xb9, 0x01, 0x2e,
    0x7b, 0x00, 0x31, 0x30, 0x8a, 0x04, 0x00, 0x90, 0x0b, 0x2f, 0x01, 0x2e, 0x6b, 0x01, 0x04, 0xa2, 0x07, 0x2f, 0x01,
    0x2e, 0x79, 0x00, 0x00, 0x90, 0x03, 0x2f, 0x01, 0x2e, 0xd9, 0x00, 0x00, 0xb2, 0x19, 0x2f, 0x3b, 0x50, 0x09, 0x52,
    0x98, 0x2e, 0x01, 0x02, 0x05, 0x2e, 0xd8, 0x00, 0x25, 0x2e, 0x87, 0x01, 0x05, 0x2e, 0xd8, 0x00, 0x80, 0x90, 0x02,
    0x2f, 0x12, 0x30, 0x25, 0x2e, 0xd8, 0x00, 0x01, 0x2e, 0xd9, 0x00, 0x00, 0xb2, 0x10, 0x30, 0x05, 0x2e, 0x79, 0x00,
    0x01, 0x2f, 0x21, 0x2e, 0x79, 0x00, 0x25, 0x2e, 0xd9, 0x00, 0x98, 0x2e, 0xf2, 0x01, 0x00, 0xb2, 0x22, 0x30, 0x21,
    0x30, 0x03, 0x2f, 0x01, 0x2e, 0x7b, 0x00, 0x00, 0x90, 0x05, 0x2f, 0x01, 0x2e, 0x7a, 0x00, 0x00, 0x90, 0x30, 0x30,
    0x01, 0x30, 0x41, 0x22, 0x01, 0x2e, 0x94, 0x01, 0x08, 0x1a, 0x0e, 0x2f, 0x23, 0x2e, 0x94, 0x01, 0x33, 0x30, 0x3d,
    0x50, 0x0b, 0x09, 0x01, 0x40, 0x39, 0x56, 0x46, 0xbe, 0x4b, 0x08, 0x4c, 0x0a, 0x01, 0x42, 0x0a, 0x80, 0x25, 0x52,
    0x01, 0x42, 0x00, 0x2e, 0x01, 0x2e, 0x79, 0x00, 0x00, 0xb2, 0x1f, 0x2f, 0x03, 0x2e, 0xc0, 0xf5, 0xf0, 0x30, 0x48,
    0x08, 0x47, 0xaa, 0x74, 0x30, 0x07, 0x2e, 0xdc, 0x00, 0x61, 0x22, 0x4b, 0x1a, 0x05, 0x2f, 0x07, 0x2e, 0x66, 0xf5,
    0xbf, 0xbd, 0xbf, 0xb9, 0xc0, 0x90, 0x0b, 0x2f, 0x3f, 0x56, 0x04, 0x30, 0xd4, 0x42, 0xd2, 0x42, 0x81, 0x04, 0x24,
    0xbd, 0xfe, 0x80, 0x81, 0x84, 0xc4, 0x42, 0x23, 0x2e, 0xdc, 0x00, 0x02, 0x42, 0x02, 0x32, 0x25, 0x2e, 0x62, 0xf5,
    0x05, 0x2e, 0x6d, 0x01, 0x81, 0x80, 0x03, 0x2e, 0x8a, 0x00, 0x21, 0x2e, 0x6d, 0x01, 0x0f, 0x56, 0x62, 0x6f, 0x4b,
    0x08, 0x00, 0x31, 0x80, 0x42, 0x40, 0xb2, 0x0b, 0x2f, 0xf2, 0x3e, 0x01, 0x2e, 0xca, 0xf5, 0x82, 0x08, 0x25, 0x2e,
    0xca, 0xf5, 0x05, 0x2e, 0x59, 0xf5, 0xe0, 0x3f, 0x90, 0x08, 0x25, 0x2e, 0x59, 0xf5, 0xf6, 0x6f, 0xe4, 0x6f, 0xa1,
    0x6f, 0xb3, 0x6f, 0xc5, 0x6f, 0xd7, 0x6f, 0x7b, 0x6f, 0x82, 0x6f, 0x90, 0x6f, 0x60, 0x5f, 0xc8, 0x2e, 0xc0, 0x50,
    0x80, 0x7f, 0x92, 0x7f, 0xd5, 0x7f, 0xc4, 0x7f, 0xb3, 0x7f, 0xa1, 0x7f, 0xe7, 0x7f, 0xf6, 0x7f, 0x7b, 0x7f, 0x00,
    0x2e, 0x35, 0x50, 0x00, 0x2e, 0x02, 0x40, 0x2f, 0xbd, 0x2f, 0xb9, 0x80, 0x90, 0x02, 0x2f, 0x32, 0x30, 0x25, 0x2e,
    0x69, 0xf5, 0x37, 0x80, 0x00, 0x2e, 0x00, 0x40, 0x60, 0x7f, 0x98, 0x2e, 0xe9, 0x01, 0x0b, 0x30, 0x5b, 0x7f, 0x40,
    0x7f, 0x02, 0x32, 0x64, 0x6f, 0x25, 0x52, 0x43, 0x56, 0x0b, 0x30, 0x80, 0x2e, 0x65, 0xb4, 0x62, 0x09, 0x40, 0xb3,
    0x0b, 0x2f, 0x00, 0xb2, 0x03, 0x2f, 0x0b, 0x2e, 0x79, 0x00, 0x40, 0x91, 0x03, 0x2f, 0xc2, 0x8a, 0x25, 0x2e, 0x64,
    0xf5, 0x4b, 0x43, 0x25, 0x2e, 0x60, 0xf5, 0x21, 0x09, 0x00, 0xb3, 0x79, 0x2f, 0x98, 0x2e, 0xf2, 0x01, 0x00, 0xb2,
    0x06, 0x2f, 0x01, 0x2e, 0x7b, 0x00, 0x00, 0xb2, 0x02, 0x2f, 0x40, 0x6f, 0x00, 0x90, 0x0a, 0x2f, 0x01, 0x2e, 0xdb,
    0x00, 0x00, 0x90, 0x19, 0x2f, 0x10, 0x30, 0x21, 0x2e, 0xdb, 0x00, 0x00, 0x30, 0x98, 0x2e, 0x0d, 0xb6, 0x13, 0x2d,
    0x01, 0x2e, 0xc3, 0xf5, 0x0c, 0xbc, 0x0f, 0xb8, 0x12, 0x30, 0x10, 0x04, 0x03, 0xb0, 0x26, 0x25, 0x45, 0x50, 0x07,
    0x52, 0x98, 0x2e, 0x01, 0x02, 0x10, 0x30, 0x21, 0x2e, 0x99, 0x01, 0x02, 0x30, 0x50, 0x7f, 0x25, 0x2e, 0xdb, 0x00,
    0x40, 0x6f, 0x00, 0xb2, 0x03, 0x2f, 0x05, 0x2e, 0x7a, 0x00, 0x80, 0x90, 0x0e, 0x2f, 0x05, 0x2e, 0xda, 0x00, 0x80,
    0x90, 0x36, 0x2f, 0x11, 0x30, 0x02, 0x30, 0x23, 0x2e, 0xda, 0x00, 0x23, 0x2e, 0x74, 0x01, 0x25, 0x2e, 0x7d, 0x00,
    0x25, 0x2e, 0x9e, 0x01, 0x2c, 0x2d, 0x05, 0x2e, 0x9e, 0x01, 0x81, 0x82, 0x23, 0x2e, 0x9e, 0x01, 0x12, 0x30, 0x4a,
    0x08, 0x40, 0xb2, 0x05, 0x2f, 0x03, 0x2e, 0x58, 0xf5, 0x98, 0xbc, 0x9e, 0xb8, 0x43, 0x90, 0x1a, 0x2f, 0x01, 0x2e,
    0xc1, 0xf5, 0x0e, 0xbd, 0x30, 0x30, 0x2e, 0xb9, 0x82, 0x04, 0x01, 0x52, 0x47, 0x50, 0x62, 0x7f, 0x98, 0x2e, 0x01,
    0x02, 0x01, 0x2e, 0xdd, 0x00, 0x01, 0x90, 0x49, 0x50, 0x03, 0x52, 0x02, 0x2f, 0x62, 0x6f, 0x98, 0x2e, 0x01, 0x02,
    0x12, 0x30, 0x25, 0x2e, 0x9d, 0x01, 0x00, 0x30, 0x21, 0x2e, 0xda, 0x00, 0x40, 0x6f, 0x52, 0x7f, 0x00, 0x2e, 0x52,
    0x6f, 0x80, 0x90, 0x05, 0x2f, 0x02, 0x30, 0x25, 0x2e, 0x94, 0x01, 0x25, 0x54, 0x25, 0x2e, 0x64, 0xf5, 0x25, 0x52,
    0x23, 0x2e, 0x60, 0xf5, 0x02, 0x32, 0x43, 0x56, 0x0b, 0x30, 0x00, 0x90, 0x0b, 0x2f, 0x37, 0x2e, 0xd8, 0x00, 0x41,
    0x50, 0x21, 0x2e, 0x5a, 0xf2, 0x98, 0x2e, 0xdd, 0x03, 0x40, 0x6f, 0x02, 0x32, 0x25, 0x52, 0x43, 0x56, 0x0b, 0x30,
    0x09, 0x2e, 0x60, 0xf5, 0x62, 0x09, 0x40, 0x91, 0x90, 0x2e, 0xc9, 0xb3, 0x61, 0x09, 0x40, 0x91, 0x90, 0x2e, 0xc9,
    0xb3, 0x80, 0x6f, 0x92, 0x6f, 0xa1, 0x6f, 0xb3, 0x6f, 0xc4, 0x6f, 0xd5, 0x6f, 0x7b, 0x6f, 0xf6, 0x6f, 0xe7, 0x6f,
    0x40, 0x5f, 0xc8, 0x2e, 0x4b, 0x52, 0x00, 0x51, 0x52, 0x40, 0x47, 0x40, 0xf8, 0xbc, 0x9c, 0xb9, 0x01, 0x2e, 0x9f,
    0x00, 0xf3, 0x7f, 0x8f, 0xbd, 0x3f, 0xba, 0x1a, 0x25, 0x78, 0xb8, 0x59, 0x56, 0xd3, 0x08, 0x75, 0x8c, 0x73, 0x8a,
    0xeb, 0x7f, 0xfc, 0xbf, 0xd4, 0x7f, 0x0b, 0x30, 0xfc, 0xbb, 0x1c, 0x0b, 0x4b, 0x7f, 0x8b, 0x43, 0x4b, 0x43, 0x21,
    0x2e, 0xe4, 0x00, 0x00, 0xb3, 0xa7, 0x7f, 0xb5, 0x7f, 0xc6, 0x7f, 0x93, 0x7f, 0x90, 0x2e, 0x35, 0xb5, 0x01, 0x2e,
    0xfe, 0x00, 0x00, 0xb2, 0x0b, 0x2f, 0x4f, 0x52, 0x01, 0x2e, 0xf9, 0x00, 0x82, 0x7f, 0x98, 0x2e, 0xbb, 0xcc, 0x0b,
    0x30, 0x37, 0x2e, 0xfe, 0x00, 0x82, 0x6f, 0x93, 0x6f, 0x1a, 0x25, 0xc0, 0xb2, 0x8b, 0x7f, 0x14, 0x2f, 0x26, 0xbc,
    0x25, 0xbd, 0x06, 0xb8, 0x2f, 0xb9, 0x80, 0xb2, 0x14, 0xb0, 0x0c, 0x2f, 0x51, 0x50, 0x53, 0x54, 0x0b, 0x30, 0x0b,
    0x2e, 0xa0, 0x00, 0x57, 0x58, 0x1b, 0x42, 0x9b, 0x42, 0x6c, 0x09, 0x2b, 0x2e, 0xa0, 0x00, 0x0b, 0x42, 0x8b, 0x42,
    0x86, 0x7f, 0x70, 0x84, 0x5b, 0x50, 0xd8, 0x08, 0x55, 0x52, 0x09, 0x50, 0x72, 0x7f, 0x63, 0x7f, 0x98, 0x2e, 0xc2,
    0xc0, 0xd1, 0x6f, 0x62, 0x6f, 0xd1, 0x0a, 0x01, 0x2e, 0xf9, 0x00, 0xc5, 0x6f, 0xb4, 0x6f, 0x72, 0x6f, 0x4f, 0x52,
    0x4d, 0x5c, 0x98, 0x2e, 0x06, 0xcd, 0x53, 0x6f, 0x90, 0x6f, 0x51, 0x52, 0xc0, 0xb2, 0x04, 0xbd, 0x54, 0x40, 0xaf,
    0xb9, 0x45, 0x40, 0xd1, 0x7f, 0x02, 0x30, 0x06, 0x2f, 0xc0, 0xb2, 0x02, 0x30, 0x03, 0x2f, 0x53, 0x5c, 0x12, 0x30,
    0x94, 0x43, 0x85, 0x43, 0x03, 0xbf, 0x6f, 0xbb, 0x80, 0xb3, 0x20, 0x2f, 0x36, 0x6f, 0x26, 0x01, 0x46, 0x6f, 0x6e,
    0x03, 0x45, 0x42, 0xc0, 0x90, 0x29, 0x2e, 0xfa, 0x00, 0x53, 0x52, 0x14, 0x2f, 0x53, 0x5c, 0x00, 0x2e, 0x93, 0x41,
    0x86, 0x41, 0xe3, 0x04, 0xae, 0x07, 0x80, 0xab, 0x04, 0x2f, 0x80, 0x91, 0x0a, 0x2f, 0x86, 0x6f, 0x73, 0x0f, 0x07,
    0x2f, 0x83, 0x6f, 0xc0, 0xb2, 0x04, 0x2f, 0x54, 0x42, 0x45, 0x42, 0x12, 0x30, 0x04, 0x2c, 0x11, 0x30, 0x02, 0x2c,
    0x11, 0x30, 0x11, 0x30, 0x02, 0xbc, 0x0f, 0xb8, 0xc2, 0x7f, 0x00, 0x90, 0x06, 0x2f, 0x31, 0x30, 0x23, 0x2e, 0x78,
    0x01, 0x23, 0x2e, 0xdf, 0x00, 0x0b, 0x2c, 0x01, 0x30, 0x01, 0x2e, 0xdf, 0x00, 0x05, 0x2e, 0x78, 0x01, 0x10, 0x1a,
    0x02, 0x2f, 0x21, 0x2e, 0x78, 0x01, 0x01, 0x2d, 0x01, 0x30, 0xf0, 0x6f, 0x98, 0x2e, 0x95, 0xcf, 0xc1, 0x6f, 0xa0,
    0x6f, 0x98, 0x2e, 0x95, 0xcf, 0xd2, 0x6f, 0x21, 0x52, 0x01, 0x2e, 0xfa, 0x00, 0x82, 0x40, 0x50, 0x42, 0x11, 0x2c,
    0x42, 0x42, 0x10, 0x30, 0x31, 0x30, 0x21, 0x2e, 0xfe, 0x00, 0x23, 0x2e, 0x78, 0x01, 0x23, 0x2e, 0xdf, 0x00, 0xf0,
    0x6f, 0x01, 0x30, 0x98, 0x2e, 0x95, 0xcf, 0xa0, 0x6f, 0x01, 0x30, 0x98, 0x2e, 0x95, 0xcf, 0x00, 0x2e, 0xeb, 0x6f,
    0x00, 0x5f, 0xb8, 0x2e, 0x07, 0x2e, 0x9f, 0x00, 0x3b, 0xbc, 0x60, 0x50, 0xbf, 0xbd, 0x0c, 0xb8, 0xf0, 0x7f, 0xc0,
    0xb2, 0xeb, 0x7f, 0x2d, 0x2f, 0x07, 0x2e, 0xff, 0x00, 0xc3, 0x40, 0x01, 0x2e, 0x69, 0x01, 0x03, 0x1a, 0x11, 0x2f,
    0x5f, 0x52, 0x27, 0x2e, 0x69, 0x01, 0x50, 0x40, 0xa0, 0x7f, 0x78, 0x80, 0x43, 0x40, 0xd0, 0x7f, 0xb3, 0x7f, 0x98,
    0x2e, 0x64, 0xcf, 0xd0, 0x6f, 0x07, 0x80, 0xa3, 0x6f, 0x13, 0x42, 0x00, 0x2e, 0xb3, 0x6f, 0x03, 0x42, 0x03, 0x30,
    0x01, 0x2e, 0xdf, 0x00, 0x01, 0xb2, 0x01, 0x2f, 0x02, 0x90, 0x00, 0x2f, 0x13, 0x30, 0x1a, 0x25, 0x7c, 0x88, 0x01,
    0x2e, 0xff, 0x00, 0x5d, 0x52, 0x09, 0x54, 0x98, 0x2e, 0xc4, 0xce, 0xc1, 0x6f, 0xf0, 0x6f, 0x98, 0x2e, 0x95, 0xcf,
    0x04, 0x2d, 0x01, 0x30, 0xf0, 0x6f, 0x98, 0x2e, 0x95, 0xcf, 0xeb, 0x6f, 0xa0, 0x5f, 0xb8, 0x2e, 0x60, 0x51, 0x0a,
    0x25, 0x36, 0x88, 0xf4, 0x7f, 0xeb, 0x7f, 0x00, 0x32, 0x6b, 0x52, 0x32, 0x30, 0x13, 0x30, 0x98, 0x2e, 0x15, 0xcb,
    0x0a, 0x25, 0x33, 0x84, 0xd2, 0x7f, 0x43, 0x30, 0x09, 0x50, 0x67, 0x52, 0x98, 0x2e, 0x95, 0xc1, 0xd2, 0x6f, 0x61,
    0x52, 0x98, 0x2e, 0xd7, 0xc7, 0x2a, 0x25, 0xb0, 0x86, 0xc0, 0x7f, 0xd3, 0x7f, 0xaf, 0x84, 0x63, 0x50, 0xf1, 0x6f,
    0x98, 0x2e, 0x4d, 0xc8, 0x2a, 0x25, 0xae, 0x8a, 0xaa, 0x88, 0xf2, 0x6e, 0x65, 0x50, 0xc1, 0x6f, 0xd3, 0x6f, 0xf4,
    0x7f, 0x98, 0x2e, 0xb6, 0xc8, 0xe0, 0x6e, 0x00, 0xb2, 0x32, 0x2f, 0x6d, 0x54, 0x83, 0x86, 0xf1, 0x6f, 0xc3, 0x7f,
    0x04, 0x30, 0x30, 0x30, 0xf4, 0x7f, 0xd0, 0x7f, 0xb2, 0x7f, 0xe3, 0x30, 0xc5, 0x6f, 0x56, 0x40, 0x45, 0x41, 0x28,
    0x08, 0x03, 0x14, 0x0e, 0xb4, 0x08, 0xbc, 0x82, 0x40, 0x10, 0x0a, 0x69, 0x54, 0x26, 0x05, 0x91, 0x7f, 0x44, 0x28,
    0xa3, 0x7f, 0x98, 0x2e, 0xd9, 0xc0, 0x08, 0xb9, 0x33, 0x30, 0x53, 0x09, 0xc1, 0x6f, 0xd3, 0x6f, 0xf4, 0x6f, 0x83,
    0x17, 0x47, 0x40, 0x6c, 0x15, 0xb2, 0x6f, 0xbe, 0x09, 0x75, 0x0b, 0x90, 0x42, 0x45, 0x42, 0x51, 0x0e, 0x32, 0xbc,
    0x02, 0x89, 0xa1, 0x6f, 0x7e, 0x86, 0xf4, 0x7f, 0xd0, 0x7f, 0xb2, 0x7f, 0x04, 0x30, 0x91, 0x6f, 0xd6, 0x2f, 0xeb,
    0x6f, 0xa0, 0x5e, 0xb8, 0x2e, 0x01, 0x2e, 0x77, 0xf7, 0x09, 0xbc, 0x0f, 0xb8, 0x00, 0xb2, 0x10, 0x50, 0xfb, 0x7f,
    0x10, 0x30, 0x0b, 0x2f, 0x03, 0x2e, 0x8a, 0x00, 0x96, 0xbc, 0x9f, 0xb8, 0x40, 0xb2, 0x05, 0x2f, 0x03, 0x2e, 0x68,
    0xf7, 0x9e, 0xbc, 0x9f, 0xb8, 0x40, 0xb2, 0x07, 0x2f, 0x03, 0x2e, 0x6a, 0x01, 0x41, 0x90, 0x01, 0x2f, 0x98, 0x2e,
    0x0d, 0xb6, 0x03, 0x2c, 0x00, 0x30, 0x21, 0x2e, 0x6a, 0x01, 0xfb, 0x6f, 0xf0, 0x5f, 0xb8, 0x2e, 0x20, 0x50, 0xe0,
    0x7f, 0xfb, 0x7f, 0x00, 0x2e, 0x61, 0x50, 0x98, 0x2e, 0x3b, 0xc8, 0x63, 0x50, 0x98, 0x2e, 0xa7, 0xc8, 0x05, 0x50,
    0x98, 0x2e, 0x55, 0xcc, 0xe1, 0x6f, 0x65, 0x50, 0x98, 0x2e, 0xe0, 0xc9, 0xfb, 0x6f, 0x00, 0x30, 0xe0, 0x5f, 0x21,
    0x2e, 0x6a, 0x01, 0xb8, 0x2e, 0x03, 0xbc, 0x21, 0x2e, 0x6c, 0x01, 0x03, 0x2e, 0x6c, 0x01, 0x40, 0xb2, 0x10, 0x30,
    0x21, 0x2e, 0x7c, 0x00, 0x01, 0x30, 0x05, 0x2f, 0x05, 0x2e, 0x6f, 0x01, 0x80, 0x90, 0x01, 0x2f, 0x23, 0x2e, 0x6f,
    0xf5, 0xc0, 0x2e, 0x21, 0x2e, 0x70, 0x01, 0x30, 0x25, 0x00, 0x30, 0x21, 0x2e, 0x5a, 0xf5, 0x10, 0x50, 0x21, 0x2e,
    0xde, 0x00, 0x21, 0x2e, 0xd6, 0x00, 0xfb, 0x7f, 0x98, 0x2e, 0xf5, 0x03, 0x40, 0x30, 0x21, 0x2e, 0x6b, 0x01, 0xfb,
    0x6f, 0xf0, 0x5f, 0x03, 0x25, 0x80, 0x2e, 0x24, 0xb6, 0x01, 0x2e, 0xa8, 0x00, 0x31, 0x25, 0x08, 0xbd, 0x71, 0x30,
    0x20, 0x50, 0x2c, 0xb9, 0x01, 0x09, 0xf2, 0x7f, 0x00, 0x91, 0xeb, 0x7f, 0x0a, 0x2f, 0x01, 0x30, 0xf0, 0x6f, 0x98,
    0x2e, 0x95, 0xcf, 0x13, 0x30, 0x27, 0x2e, 0x74, 0x01, 0xeb, 0x6f, 0xc0, 0x2e, 0x00, 0x30, 0xe0, 0x5f, 0x01, 0x2e,
    0x74, 0x01, 0x00, 0xb2, 0x05, 0x2f, 0x06, 0x30, 0xc3, 0x50, 0x98, 0x2e, 0x84, 0xb6, 0x2d, 0x2e, 0x74, 0x01, 0x05,
    0x2e, 0x73, 0x01, 0x8b, 0x80, 0xc3, 0x52, 0x04, 0x42, 0x98, 0x2e, 0xbf, 0xb6, 0x10, 0x25, 0xf0, 0x6f, 0xf1, 0x7f,
    0x98, 0x2e, 0x95, 0xcf, 0xf0, 0x6f, 0x00, 0x90, 0x02, 0x2f, 0x01, 0x2e, 0x75, 0x01, 0x02, 0x2d, 0x21, 0x2e, 0x75,
    0x01, 0xeb, 0x6f, 0xe0, 0x5f, 0xb8, 0x2e, 0x0a, 0x82, 0x02, 0x30, 0x12, 0x42, 0x41, 0x0e, 0xfc, 0x2f, 0x37, 0x80,
    0xc5, 0x52, 0x11, 0x42, 0x00, 0x2e, 0xc7, 0x52, 0x01, 0x42, 0xb8, 0x2e, 0x07, 0x88, 0x46, 0x8e, 0x06, 0x41, 0x3c,
    0x8b, 0x32, 0x1a, 0x42, 0x41, 0xc7, 0x41, 0x16, 0x2f, 0x80, 0x91, 0x23, 0x2f, 0x57, 0x0f, 0x16, 0x30, 0x0c, 0x2f,
    0x06, 0x80, 0x00, 0x2e, 0x00, 0x40, 0x00, 0xb2, 0x04, 0x2f, 0x45, 0x82, 0x00, 0x2e, 0x41, 0x40, 0xd1, 0x0f, 0x02,
    0x2f, 0x06, 0x30, 0x06, 0x43, 0x46, 0x43, 0x80, 0xb3, 0x11, 0x2f, 0x03, 0x43, 0x47, 0x43, 0xb8, 0x2e, 0xd7, 0x0e,
    0x0c, 0x2f, 0x43, 0x82, 0x17, 0x04, 0x41, 0x40, 0x41, 0x0f, 0x07, 0x2f, 0x03, 0x43, 0x42, 0x83, 0x47, 0x43, 0x00,
    0x2e, 0x43, 0x40, 0xc1, 0x86, 0x43, 0x42, 0xb8, 0x2e, 0xb8, 0x2e, 0x88, 0x88, 0xc0, 0x50, 0x39, 0x8d, 0x88, 0x81,
    0xe2, 0x7f, 0x02, 0x30, 0x05, 0x40, 0xf2, 0x7f, 0x41, 0xab, 0x22, 0x30, 0x95, 0x22, 0x86, 0x41, 0x27, 0x5e, 0x9a,
    0x00, 0x04, 0x41, 0x37, 0x18, 0x66, 0x01, 0xc9, 0x56, 0xeb, 0x00, 0xc1, 0x7f, 0xb3, 0x7f, 0xd5, 0x7f, 0x48, 0x82,
    0x39, 0x80, 0x82, 0x40, 0xa0, 0x7f, 0x91, 0x7f, 0x8b, 0x7f, 0x06, 0x30, 0x6f, 0x5a, 0xcb, 0x58, 0x6f, 0x56, 0x98,
    0x2e, 0x67, 0xcc, 0x91, 0x6f, 0x7b, 0x82, 0xc6, 0x6f, 0x42, 0x40, 0x81, 0x84, 0x42, 0x42, 0x43, 0x82, 0x83, 0x8b,
    0x42, 0x40, 0x86, 0x89, 0x7b, 0x82, 0x84, 0x87, 0x71, 0x7f, 0x80, 0xb2, 0x95, 0x7f, 0x64, 0x7f, 0x50, 0x7f, 0x02,
    0x2f, 0xc5, 0x40, 0x41, 0x8b, 0xc5, 0x42, 0x87, 0x89, 0x67, 0x40, 0x07, 0x0f, 0x85, 0x8b, 0x41, 0x7f, 0x82, 0x8d,
    0x47, 0x40, 0x3c, 0x2f, 0x81, 0x41, 0x01, 0x0e, 0xe1, 0x6f, 0x2d, 0x2f, 0x46, 0x8c, 0x90, 0x6f, 0x86, 0x41, 0x07,
    0x40, 0xfe, 0x0e, 0x02, 0x2f, 0x04, 0x41, 0x00, 0xb3, 0x21, 0x2f, 0x44, 0x82, 0x04, 0x30, 0x41, 0x40, 0x71, 0x00,
    0xf9, 0x0e, 0x36, 0x2f, 0x46, 0x41, 0x80, 0xa7, 0x01, 0x30, 0x04, 0x30, 0x0d, 0x2f, 0xe4, 0x6f, 0x00, 0x2e, 0x04,
    0x41, 0x74, 0x0f, 0x04, 0x30, 0x07, 0x2f, 0x80, 0x90, 0x00, 0x2f, 0xc1, 0x42, 0x03, 0x86, 0x81, 0x84, 0xc2, 0x42,
    0x01, 0x42, 0x14, 0x30, 0x42, 0x85, 0x41, 0x43, 0xa1, 0x42, 0x00, 0x2e, 0x82, 0x40, 0x80, 0x90, 0x1c, 0x2f, 0x01,
    0x42, 0x1b, 0x2d, 0x06, 0x42, 0x19, 0x2c, 0x04, 0x30, 0xb2, 0x6f, 0xba, 0x04, 0x02, 0x1e, 0x80, 0x43, 0x12, 0x30,
    0xc0, 0x6f, 0x23, 0x30, 0x98, 0x2e, 0x90, 0xb6, 0x0e, 0x2c, 0x04, 0x30, 0xb2, 0x6f, 0xc0, 0x6f, 0xba, 0x00, 0x51,
    0x6f, 0x01, 0x86, 0x4a, 0x1c, 0xc1, 0x42, 0x22, 0x30, 0xe1, 0x6f, 0x13, 0x30, 0x98, 0x2e, 0x90, 0xb6, 0x04, 0x30,
    0x52, 0x6f, 0xcd, 0x52, 0x40, 0x6f, 0xc4, 0x7f, 0x98, 0x2e, 0xa0, 0xcc, 0x41, 0x6f, 0x70, 0x6f, 0x42, 0x40, 0xcf,
    0x52, 0x98, 0x2e, 0xa0, 0xcc, 0x41, 0x6f, 0x70, 0x6f, 0x42, 0x40, 0x01, 0x40, 0xd3, 0x6f, 0xd3, 0x00, 0xcb, 0x1e,
    0xcf, 0x52, 0x13, 0x42, 0xb0, 0x7f, 0x98, 0x2e, 0xa0, 0xcc, 0xb0, 0x6f, 0x3e, 0x84, 0xd1, 0x6f, 0x82, 0x40, 0x03,
    0x40, 0x51, 0x04, 0x59, 0x1c, 0x01, 0x42, 0x02, 0x82, 0xa0, 0x6f, 0x42, 0x40, 0x03, 0x40, 0xd3, 0x0f, 0x00, 0x30,
    0x14, 0x30, 0x60, 0x22, 0xc5, 0x6f, 0x40, 0x91, 0x01, 0x2f, 0x53, 0x0e, 0x26, 0x2f, 0xe5, 0x6f, 0x47, 0x85, 0x00,
    0x2e, 0x83, 0x40, 0x59, 0x0f, 0x20, 0x2f, 0x62, 0x6f, 0x4a, 0x8f, 0x85, 0x40, 0xc7, 0x41, 0x7f, 0x8d, 0x6f, 0x0f,
    0xa6, 0x15, 0x17, 0x30, 0x04, 0x30, 0x0f, 0x2f, 0xe0, 0x6f, 0x0b, 0x80, 0x00, 0x2e, 0x07, 0x40, 0x3e, 0x08, 0x00,
    0xb2, 0x00, 0x30, 0x06, 0x2f, 0x59, 0x1a, 0x04, 0x2f, 0xfd, 0x12, 0xc0, 0x90, 0x13, 0x30, 0x4b, 0x22, 0x06, 0x25,
    0x71, 0x25, 0xc0, 0xb3, 0x04, 0x2f, 0xa4, 0x42, 0xa4, 0x42, 0x83, 0x82, 0x84, 0x42, 0x44, 0x42, 0x00, 0x2e, 0x8b,
    0x6f, 0x40, 0x5f, 0xb8, 0x2e, 0x40, 0x50, 0x0a, 0x25, 0x3c, 0x80, 0xfb, 0x7f, 0x01, 0x42, 0xd2, 0x7f, 0xe3, 0x7f,
    0x32, 0x30, 0x10, 0x25, 0x98, 0x2e, 0x7a, 0xc1, 0xfb, 0x6f, 0xc0, 0x5f, 0xb8, 0x2e, 0x70, 0x50, 0x0a, 0x25, 0x39,
    0x80, 0xf3, 0x7f, 0x03, 0x42, 0xa1, 0x7f, 0xc2, 0x7f, 0xd1, 0x7f, 0x03, 0x30, 0x03, 0x43, 0xe4, 0x7f, 0xbb, 0x7f,
    0x22, 0x30, 0x10, 0x25, 0x98, 0x2e, 0x7a, 0xc1, 0xd2, 0x6f, 0x02, 0x17, 0x04, 0x08, 0xc1, 0x6f, 0x0c, 0x09, 0x04,
    0x1a, 0x10, 0x30, 0x04, 0x30, 0x20, 0x22, 0x01, 0xb2, 0x14, 0x2f, 0xd1, 0x58, 0x14, 0x09, 0xf3, 0x30, 0x93, 0x08,
    0x24, 0xbd, 0x44, 0xba, 0x94, 0x0a, 0x02, 0x17, 0xf3, 0x6f, 0x4c, 0x08, 0x9a, 0x08, 0x8a, 0x0a, 0x9d, 0x52, 0x51,
    0x08, 0x41, 0x58, 0x94, 0x08, 0x28, 0xbd, 0x98, 0xb8, 0xe4, 0x6f, 0x51, 0x0a, 0x01, 0x43, 0x00, 0x2e, 0xbb, 0x6f,
    0x90, 0x5f, 0xb8, 0x2e, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x01, 0x2e, 0x5d, 0xf7, 0x08, 0xbc, 0x80, 0xac, 0x0e, 0xbb, 0x02, 0x2f,
    0x00, 0x30, 0x41, 0x04, 0x82, 0x06, 0xc0, 0xa4, 0x00, 0x30, 0x11, 0x2f, 0x40, 0xa9, 0x03, 0x2f, 0x40, 0x91, 0x0d,
    0x2f, 0x00, 0xa7, 0x0b, 0x2f, 0x80, 0xb3, 0x6f, 0x58, 0x02, 0x2f, 0x90, 0xa1, 0x26, 0x13, 0x20, 0x23, 0x80, 0x90,
    0x10, 0x30, 0x01, 0x2f, 0xcc, 0x0e, 0x00, 0x2f, 0x00, 0x30, 0xb8, 0x2e, 0x77, 0x56, 0x71, 0x54, 0xd0, 0x40, 0xc4,
    0x40, 0x0b, 0x2e, 0xfd, 0xf3, 0x77, 0x52, 0x90, 0x42, 0x94, 0x42, 0x95, 0x42, 0x05, 0x30, 0x79, 0x50, 0x0f, 0x88,
    0x06, 0x40, 0x04, 0x41, 0x96, 0x42, 0xc5, 0x42, 0x48, 0xbe, 0x73, 0x30, 0x0d, 0x2e, 0x6f, 0x01, 0x4f, 0xba, 0x84,
    0x42, 0x03, 0x42, 0x81, 0xb3, 0x02, 0x2f, 0x2b, 0x2e, 0x6f, 0xf5, 0x06, 0x2d, 0x05, 0x2e, 0x77, 0xf7, 0x75, 0x56,
    0x93, 0x08, 0x25, 0x2e, 0x77, 0xf7, 0x73, 0x54, 0x25, 0x2e, 0xc2, 0xf5, 0x07, 0x2e, 0xfd, 0xf3, 0x42, 0x30, 0xb4,
    0x33, 0xda, 0x0a, 0x4c, 0x00, 0x27, 0x2e, 0xfd, 0xf3, 0x43, 0x40, 0xd4, 0x3f, 0xdc, 0x08, 0x43, 0x42, 0x00, 0x2e,
    0x00, 0x2e, 0x43, 0x40, 0x24, 0x30, 0xdc, 0x0a, 0x43, 0x42, 0x04, 0x80, 0x03, 0x2e, 0xfd, 0xf3, 0x4a, 0x0a, 0x23,
    0x2e, 0xfd, 0xf3, 0x61, 0x34, 0xc0, 0x2e, 0x01, 0x42, 0x00, 0x2e, 0xd0, 0x51, 0xfb, 0x7f, 0x98, 0x2e, 0xcf, 0x0d,
    0x5a, 0x25, 0x98, 0x2e, 0xf6, 0x0d, 0x6b, 0x87, 0x81, 0x54, 0xe1, 0x7f, 0xa3, 0x7f, 0xb3, 0x7f, 0xb2, 0x88, 0x7b,
    0x52, 0xc2, 0x7f, 0x65, 0x8b, 0x7d, 0x56, 0x84, 0x7f, 0x61, 0x7f, 0x75, 0x7f, 0xd0, 0x7f, 0x95, 0x7f, 0x53, 0x7f,
    0x14, 0x30, 0x7f, 0x54, 0x81, 0x6f, 0x42, 0x7f, 0x00, 0x2e, 0x53, 0x40, 0x45, 0x8c, 0x42, 0x40, 0x90, 0x41, 0xbb,
    0x83, 0x86, 0x41, 0xd8, 0x04, 0x16, 0x06, 0x00, 0xac, 0x81, 0x7f, 0x02, 0x2f, 0x02, 0x30, 0xd3, 0x04, 0x10, 0x06,
    0xc1, 0x84, 0x01, 0x30, 0xc1, 0x02, 0x0b, 0x16, 0x04, 0x09, 0x14, 0x01, 0x99, 0x02, 0xc1, 0xb9, 0xaf, 0xbc, 0x59,
    0x0a, 0x64, 0x6f, 0x51, 0x43, 0xa1, 0xb4, 0x12, 0x41, 0x13, 0x41, 0x41, 0x43, 0x35, 0x7f, 0x64, 0x7f, 0x26, 0x31,
    0xe5, 0x6f, 0xd4, 0x6f, 0x98, 0x2e, 0x37, 0xca, 0x32, 0x6f, 0x75, 0x6f, 0x83, 0x40, 0x42, 0x41, 0x23, 0x7f, 0x12,
    0x7f, 0xf6, 0x30, 0x40, 0x25, 0x51, 0x25, 0x98, 0x2e, 0x37, 0xca, 0x14, 0x6f, 0x20, 0x05, 0x70, 0x6f, 0x25, 0x6f,
    0x69, 0x07, 0xa2, 0x6f, 0x31, 0x6f, 0x0b, 0x30, 0x04, 0x42, 0x9b, 0x42, 0x8b, 0x42, 0x55, 0x42, 0x32, 0x7f, 0x40,
    0xa9, 0xc3, 0x6f, 0x71, 0x7f, 0x02, 0x30, 0xd0, 0x40, 0xc3, 0x7f, 0x03, 0x2f, 0x40, 0x91, 0x15, 0x2f, 0x00, 0xa7,
    0x13, 0x2f, 0x00, 0xa4, 0x11, 0x2f, 0x84, 0xbd, 0x98, 0x2e, 0x79, 0xca, 0x55, 0x6f, 0x89, 0x54, 0x54, 0x41, 0x82,
    0x00, 0xf3, 0x3f, 0x45, 0x41, 0xcb, 0x02, 0xf6, 0x30, 0x98, 0x2e, 0x37, 0xca, 0x33, 0x6f, 0xa4, 0x6f, 0xc1, 0x42,
    0x03, 0x2c, 0x00, 0x43, 0xa4, 0x6f, 0x33, 0x6f, 0x00, 0x2e, 0x42, 0x6f, 0x55, 0x6f, 0x91, 0x40, 0x42, 0x8b, 0x00,
    0x41, 0x41, 0x00, 0x01, 0x43, 0x55, 0x7f, 0x14, 0x30, 0xc1, 0x40, 0x95, 0x40, 0x4d, 0x02, 0xc5, 0x6f, 0x87, 0x50,
    0x68, 0x0e, 0x75, 0x6f, 0xd1, 0x42, 0xa3, 0x7f, 0x8a, 0x2f, 0x09, 0x2e, 0x6f, 0x01, 0x01, 0xb3, 0x22, 0x2f, 0x81,
    0x58, 0x90, 0x6f, 0x17, 0x30, 0x13, 0x41, 0xb6, 0x6f, 0xe4, 0x7f, 0x00, 0x2e, 0x91, 0x41, 0x14, 0x40, 0x92, 0x41,
    0x15, 0x40, 0x17, 0x2e, 0x6f, 0xf5, 0xb6, 0x7f, 0xd0, 0x7f, 0xcb, 0x7f, 0x98, 0x2e, 0x00, 0x0c, 0x07, 0x15, 0xc2,
    0x6f, 0x14, 0x0b, 0x29, 0x2e, 0x6f, 0xf5, 0xc3, 0xa3, 0xc1, 0x8f, 0xe4, 0x6f, 0xd0, 0x6f, 0xe6, 0x2f, 0x14, 0x30,
    0x05, 0x2e, 0x6f, 0xf5, 0x14, 0x0b, 0x29, 0x2e, 0x6f, 0xf5, 0x44, 0x2d, 0x83, 0x54, 0x01, 0x32, 0x89, 0x58, 0x05,
    0x30, 0x41, 0x50, 0x83, 0x40, 0xd8, 0x08, 0x91, 0x01, 0xb8, 0xbd, 0x38, 0xb5, 0xe6, 0x7f, 0x0a, 0x16, 0xb1, 0x6f,
    0x2a, 0xbb, 0xa6, 0xbd, 0x1c, 0x01, 0x06, 0xbc, 0x52, 0x40, 0x06, 0x0a, 0x53, 0x40, 0x45, 0x03, 0xb1, 0x7f, 0xf6,
    0x30, 0x98, 0x2e, 0x37, 0xca, 0x1a, 0xbd, 0x16, 0xb6, 0x86, 0xba, 0x00, 0xa9, 0xaa, 0x0a, 0x67, 0x52, 0x0f, 0x2f,
    0x00, 0x91, 0x67, 0x52, 0x03, 0x2f, 0x67, 0x5a, 0x55, 0x0f, 0x67, 0x52, 0x08, 0x2f, 0x3f, 0xa1, 0x04, 0x2f, 0x3f,
    0x91, 0x03, 0x2f, 0x89, 0x58, 0xd4, 0x0f, 0x00, 0x2f, 0x89, 0x54, 0x12, 0x25, 0xf2, 0x33, 0x98, 0x2e, 0xd9, 0xc0,
    0xe4, 0x6f, 0xf5, 0x37, 0x45, 0x09, 0x21, 0x85, 0x05, 0x43, 0x05, 0x30, 0x85, 0x52, 0x51, 0x0e, 0x01, 0x32, 0x89,
    0x58, 0xc5, 0x2f, 0x25, 0x54, 0x09, 0x2e, 0x77, 0xf7, 0x22, 0x0b, 0x29, 0x2e, 0x77, 0xf7, 0xfb, 0x6f, 0x30, 0x5e,
    0xb8, 0x2e, 0x10, 0x50, 0x01, 0x2e, 0x6b, 0x01, 0x00, 0xb2, 0xfb, 0x7f, 0x5d, 0x2f, 0x01, 0xb2, 0x54, 0x2f, 0x02,
    0xb2, 0x4e, 0x2f, 0x03, 0x90, 0x63, 0x2f, 0x8f, 0x50, 0x39, 0x82, 0x02, 0x40, 0x81, 0x88, 0x91, 0x54, 0x41, 0x40,
    0x97, 0x56, 0x04, 0x42, 0x00, 0x2e, 0x94, 0x40, 0x95, 0x40, 0xd8, 0xbe, 0x2c, 0x0b, 0x45, 0x40, 0x6c, 0x01, 0x55,
    0x42, 0x0c, 0x17, 0x45, 0x40, 0x2c, 0x03, 0x54, 0x42, 0x53, 0x0e, 0xf2, 0x2f, 0x99, 0x56, 0x3e, 0x82, 0xe2, 0x40,
    0xc3, 0x40, 0x28, 0xbd, 0x93, 0x0a, 0x43, 0x40, 0xda, 0x00, 0x53, 0x42, 0x8a, 0x16, 0x43, 0x40, 0x9a, 0x02, 0x52,
    0x42, 0x00, 0x2e, 0x41, 0x40, 0x25, 0x54, 0x4a, 0x0e, 0x3b, 0x2f, 0x3a, 0x82, 0x00, 0x30, 0x41, 0x40, 0x21, 0x2e,
    0x77, 0x0f, 0x40, 0xb2, 0x0a, 0x2f, 0x98, 0x2e, 0x61, 0x0c, 0x98, 0x2e, 0x2b, 0x0e, 0x98, 0x2e, 0x41, 0x0e, 0xfb,
    0x6f, 0xf0, 0x5f, 0x00, 0x30, 0x80, 0x2e, 0x38, 0xb6, 0x95, 0x54, 0x8b, 0x56, 0x83, 0x42, 0x8f, 0x86, 0x74, 0x30,
    0x93, 0x54, 0xc4, 0x42, 0x11, 0x30, 0x23, 0x2e, 0x6b, 0x01, 0xa1, 0x42, 0x23, 0x30, 0x27, 0x2e, 0x6e, 0x01, 0x21,
    0x2e, 0x6d, 0x01, 0xba, 0x82, 0x18, 0x2c, 0x81, 0x42, 0x30, 0x30, 0x21, 0x2e, 0x6b, 0x01, 0x13, 0x2d, 0x21, 0x30,
    0x00, 0x30, 0x23, 0x2e, 0x6b, 0x01, 0x21, 0x2e, 0x7b, 0xf7, 0x0c, 0x2d, 0x77, 0x30, 0x98, 0x2e, 0x1f, 0x0c, 0x8d,
    0x50, 0x0c, 0x82, 0x12, 0x30, 0x40, 0x42, 0x25, 0x2e, 0x6b, 0x01, 0x2f, 0x2e, 0x7b, 0xf7, 0xfb, 0x6f, 0xf0, 0x5f,
    0xb8, 0x2e, 0x70, 0x50, 0x0a, 0x25, 0x39, 0x86, 0xfb, 0x7f, 0xe1, 0x32, 0x62, 0x30, 0x98, 0x2e, 0xc2, 0xc4, 0x41,
    0x56, 0xa5, 0x6f, 0xab, 0x08, 0x91, 0x6f, 0x4b, 0x08, 0x9b, 0x56, 0xc4, 0x6f, 0x23, 0x09, 0x4d, 0xba, 0x93, 0xbc,
    0x8c, 0x0b, 0xd1, 0x6f, 0x0b, 0x09, 0x81, 0x52, 0x9d, 0x5e, 0x56, 0x42, 0xaf, 0x09, 0x4d, 0xba, 0x23, 0xbd, 0x94,
    0x0a, 0xe5, 0x6f, 0x68, 0xbb, 0xeb, 0x08, 0xbd, 0xb9, 0x63, 0xbe, 0xfb, 0x6f, 0x52, 0x42, 0xe3, 0x0a, 0xc0, 0x2e,
    0x43, 0x42, 0x90, 0x5f, 0x87, 0x50, 0x03, 0x2e, 0x25, 0xf3, 0x12, 0x40, 0x00, 0x40, 0x28, 0xba, 0x9b, 0xbc, 0x88,
    0xbd, 0x93, 0xb4, 0xe3, 0x0a, 0x89, 0x16, 0x08, 0xb6, 0xc0, 0x2e, 0x19, 0x00, 0x62, 0x02, 0x10, 0x50, 0xfb, 0x7f,
    0x98, 0x2e, 0x5d, 0x0d, 0x01, 0x2e, 0x6b, 0x01, 0x31, 0x30, 0x08, 0x04, 0xfb, 0x6f, 0x01, 0x30, 0xf0, 0x5f, 0x23,
    0x2e, 0x6d, 0x01, 0x21, 0x2e, 0x6e, 0x01, 0xb8, 0x2e, 0x01, 0x2e, 0x6e, 0x01, 0x03, 0x2e, 0x6d, 0x01, 0x48, 0x0e,
    0x01, 0x2f, 0x80, 0x2e, 0x05, 0x0e, 0xb8, 0x2e, 0x9f, 0x50, 0x21, 0x34, 0x01, 0x42, 0x82, 0x30, 0xc1, 0x32, 0x25,
    0x2e, 0x62, 0xf5, 0x01, 0x00, 0x22, 0x30, 0x01, 0x40, 0x4a, 0x0a, 0x01, 0x42, 0xb8, 0x2e, 0x9f, 0x54, 0xf0, 0x3b,
    0x83, 0x40, 0xd8, 0x08, 0xa1, 0x52, 0x83, 0x42, 0x00, 0x30, 0x83, 0x30, 0x50, 0x42, 0xc4, 0x32, 0x27, 0x2e, 0x64,
    0xf5, 0x94, 0x00, 0x50, 0x42, 0x40, 0x42, 0xd3, 0x3f, 0x84, 0x40, 0x7d, 0x82, 0xe3, 0x08, 0x40, 0x42, 0x83, 0x42,
    0xb8, 0x2e, 0x95, 0x52, 0x00, 0x30, 0x40, 0x42, 0x7c, 0x86, 0x71, 0x52, 0x09, 0x2e, 0x62, 0x0f, 0x77, 0x54, 0xc4,
    0x42, 0xd3, 0x86, 0x54, 0x40, 0x55, 0x40, 0x94, 0x42, 0x85, 0x42, 0x21, 0x2e, 0x6e, 0x01, 0x42, 0x40, 0x25, 0x2e,
    0xfd, 0xf3, 0xc0, 0x42, 0x7e, 0x82, 0x05, 0x2e, 0xd7, 0x00, 0x80, 0xb2, 0x14, 0x2f, 0x05, 0x2e, 0x89, 0x00, 0x27,
    0xbd, 0x2f, 0xb9, 0x80, 0x90, 0x02, 0x2f, 0x21, 0x2e, 0x6f, 0xf5, 0x0c, 0x2d, 0x07, 0x2e, 0x63, 0x0f, 0x14, 0x30,
    0x1c, 0x09, 0x05, 0x2e, 0x77, 0xf7, 0x75, 0x56, 0x47, 0xbe, 0x93, 0x08, 0x94, 0x0a, 0x25, 0x2e, 0x77, 0xf7, 0xa3,
    0x54, 0x50, 0x42, 0x4a, 0x0e, 0xfc, 0x2f, 0xb8, 0x2e, 0x50, 0x50, 0x02, 0x30, 0x43, 0x86, 0xa1, 0x50, 0xfb, 0x7f,
    0xe3, 0x7f, 0xd2, 0x7f, 0xc0, 0x7f, 0xb1, 0x7f, 0x00, 0x2e, 0x41, 0x40, 0x00, 0x40, 0x48, 0x04, 0x98, 0x2e, 0x74,
    0xc0, 0x1e, 0xaa, 0xd3, 0x6f, 0x14, 0x30, 0xb1, 0x6f, 0xe3, 0x22, 0xc0, 0x6f, 0x52, 0x40, 0xe4, 0x6f, 0x4c, 0x0e,
    0x12, 0x42, 0xd3, 0x7f, 0xeb, 0x2f, 0x03, 0x2e, 0x78, 0x0f, 0x40, 0x90, 0x11, 0x30, 0x03, 0x2f, 0x23, 0x2e, 0x78,
    0x0f, 0x02, 0x2c, 0x00, 0x30, 0xd0, 0x6f, 0xfb, 0x6f, 0xb0, 0x5f, 0xb8, 0x2e, 0x40, 0x50, 0xf1, 0x7f, 0x0a, 0x25,
    0x3c, 0x86, 0xeb, 0x7f, 0x41, 0x33, 0x22, 0x30, 0x98, 0x2e, 0xc2, 0xc4, 0xd3, 0x6f, 0xf4, 0x30, 0xdc, 0x09, 0xa7,
    0x58, 0xc2, 0x6f, 0x94, 0x09, 0xa9, 0x58, 0x6a, 0xbb, 0xdc, 0x08, 0xb4, 0xb9, 0xb1, 0xbd, 0xa5, 0x5a, 0x95, 0x08,
    0x21, 0xbd, 0xf6, 0xbf, 0x77, 0x0b, 0x51, 0xbe, 0xf1, 0x6f, 0xeb, 0x6f, 0x52, 0x42, 0x54, 0x42, 0xc0, 0x2e, 0x43,
    0x42, 0xc0, 0x5f, 0x50, 0x50, 0xbb, 0x50, 0x31, 0x30, 0x11, 0x42, 0xfb, 0x7f, 0x7b, 0x30, 0x0b, 0x42, 0x11, 0x30,
    0x02, 0x80, 0x23, 0x33, 0x01, 0x42, 0x03, 0x00, 0x07, 0x2e, 0x80, 0x03, 0x05, 0x2e, 0xd5, 0x00, 0x33, 0x52, 0xe2,
    0x7f, 0xd3, 0x7f, 0xc0, 0x7f, 0x98, 0x2e, 0x9c, 0x0e, 0xd1, 0x6f, 0x08, 0x0a, 0x1a, 0x25, 0x7b, 0x86, 0xd0, 0x7f,
    0x01, 0x33, 0x12, 0x30, 0x98, 0x2e, 0xc2, 0xc4, 0xd1, 0x6f, 0x08, 0x0a, 0x00, 0xb2, 0x0d, 0x2f, 0xe3, 0x6f, 0x01,
    0x2e, 0x80, 0x03, 0x51, 0x30, 0xc7, 0x86, 0x23, 0x2e, 0x21, 0xf2, 0x08, 0xbc, 0xc0, 0x42, 0x98, 0x2e, 0x91, 0x03,
    0x00, 0x2e, 0x00, 0x2e, 0xd0, 0x2e, 0xb0, 0x6f, 0x0b, 0xb8, 0x03, 0x2e, 0x1b, 0x00, 0x08, 0x1a, 0xb0, 0x7f, 0x70,
    0x30, 0x04, 0x2f, 0x21, 0x2e, 0x21, 0xf2, 0x00, 0x2e, 0x00, 0x2e, 0xd0, 0x2e, 0x98, 0x2e, 0x6d, 0xc0, 0x98, 0x2e,
    0x5d, 0xc0, 0xab, 0x50, 0x98, 0x2e, 0x46, 0xc3, 0xad, 0x50, 0x98, 0x2e, 0xfc, 0xc5, 0x5d, 0x50, 0x98, 0x2e, 0x64,
    0xcf, 0xb1, 0x50, 0x21, 0x2e, 0x7e, 0x01, 0xaf, 0x56, 0xb3, 0x52, 0x27, 0x2e, 0x7f, 0x01, 0x23, 0x2e, 0x80, 0x01,
    0xb5, 0x50, 0x98, 0x2e, 0x53, 0xc7, 0xb7, 0x50, 0x98, 0x2e, 0x44, 0xcb, 0x10, 0x30, 0x98, 0x2e, 0x0d, 0xb6, 0x20,
    0x26, 0xc0, 0x6f, 0x02, 0x31, 0x12, 0x42, 0xeb, 0x33, 0x0b, 0x42, 0x37, 0x80, 0x01, 0x30, 0x01, 0x42, 0xf3, 0x37,
    0xbd, 0x52, 0xc1, 0x50, 0x44, 0x40, 0xa2, 0x0a, 0x42, 0x42, 0x8b, 0x31, 0x09, 0x2e, 0x5e, 0xf7, 0xbf, 0x54, 0xe3,
    0x08, 0x83, 0x42, 0x1b, 0x42, 0x23, 0x33, 0x4b, 0x00, 0xbc, 0x84, 0x0b, 0x40, 0x33, 0x30, 0x83, 0x42, 0x0b, 0x42,
    0xe0, 0x7f, 0xd1, 0x7f, 0x98, 0x2e, 0x0c, 0x02, 0xd1, 0x6f, 0x80, 0x30, 0x40, 0x42, 0x03, 0x30, 0xe0, 0x6f, 0xb9,
    0x54, 0x04, 0x30, 0x00, 0x2e, 0x00, 0x2e, 0x01, 0x89, 0x62, 0x0e, 0xfa, 0x2f, 0x43, 0x42, 0x11, 0x30, 0xfb, 0x6f,
    0xc0, 0x2e, 0x01, 0x42, 0xb0, 0x5f, 0xc1, 0x4a, 0x00, 0x00, 0x6d, 0x57, 0x00, 0x00, 0x77, 0x8e, 0x00, 0x00, 0xe0,
    0xff, 0xff, 0xff, 0xd3, 0xff, 0xff, 0xff, 0xe5, 0xff, 0xff, 0xff, 0xee, 0xe1, 0xff, 0xff, 0x7c, 0x13, 0x00, 0x00,
    0x46, 0xe6, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1,
    0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00,
    0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e,
    0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80, 0x2e, 0x00, 0xc1, 0x80,
    0x2e, 0x00, 0xc1
};

/*! @name  Global array that stores the feature input configuration of BMI270_LEGACY */
const struct bmi2_feature_config bmi270_legacy_feat_in[BMI270_LEGACY_MAX_FEAT_IN] = {
    { .type = BMI2_CONFIG_ID, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_CONFIG_ID_STRT_ADDR },
    { .type = BMI2_MAX_BURST_LEN, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_MAX_BURST_LEN_STRT_ADDR },
    { .type = BMI2_CRT_GYRO_SELF_TEST, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_CRT_GYRO_SELF_TEST_STRT_ADDR },
    { .type = BMI2_ABORT_CRT_GYRO_SELF_TEST, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_ABORT_STRT_ADDR },
    { .type = BMI2_AXIS_MAP, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_AXIS_MAP_STRT_ADDR },
    { .type = BMI2_GYRO_SELF_OFF, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_GYRO_SELF_OFF_STRT_ADDR },
    { .type = BMI2_NVM_PROG_PREP, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_NVM_PROG_PREP_STRT_ADDR },
    { .type = BMI2_ANY_MOTION, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_ANY_MOT_STRT_ADDR },
    { .type = BMI2_NO_MOTION, .page = BMI2_PAGE_1, .start_addr = BMI270_LEGACY_NO_MOT_STRT_ADDR },
    { .type = BMI2_ORIENTATION, .page = BMI2_PAGE_2, .start_addr = BMI270_LEGACY_ORIENT_STRT_ADDR },
    { .type = BMI2_HIGH_G, .page = BMI2_PAGE_2, .start_addr = BMI270_LEGACY_HIGH_G_STRT_ADDR },
    { .type = BMI2_LOW_G, .page = BMI2_PAGE_2, .start_addr = BMI270_LEGACY_LOW_G_STRT_ADDR },
    { .type = BMI2_FLAT, .page = BMI2_PAGE_3, .start_addr = BMI270_LEGACY_FLAT_STRT_ADDR },
    { .type = BMI2_SIG_MOTION, .page = BMI2_PAGE_3, .start_addr = BMI270_LEGACY_SIG_MOT_STRT_ADDR },
    { .type = BMI2_STEP_DETECTOR, .page = BMI2_PAGE_4, .start_addr = BMI270_LEGACY_STEP_COUNT_STRT_ADDR },
    { .type = BMI2_STEP_COUNTER, .page = BMI2_PAGE_4, .start_addr = BMI270_LEGACY_STEP_COUNT_STRT_ADDR },
    { .type = BMI2_STEP_ACTIVITY, .page = BMI2_PAGE_4, .start_addr = BMI270_LEGACY_STEP_COUNT_STRT_ADDR },
    { .type = BMI2_GYRO_GAIN_UPDATE, .page = BMI2_PAGE_4, .start_addr = BMI270_LEGACY_GYRO_USERGAIN_UPDATE_STRT_ADDR },
    { .type = BMI2_SINGLE_TAP, .page = BMI2_PAGE_5, .start_addr = BMI270_LEGACY_TAP_DETECT_1_STRT_ADDR },
    { .type = BMI2_DOUBLE_TAP, .page = BMI2_PAGE_5, .start_addr = BMI270_LEGACY_TAP_DETECT_1_STRT_ADDR },
    { .type = BMI2_TRIPLE_TAP, .page = BMI2_PAGE_5, .start_addr = BMI270_LEGACY_TAP_DETECT_1_STRT_ADDR },
    { .type = BMI2_TAP_DETECTOR_1, .page = BMI2_PAGE_5, .start_addr = BMI270_LEGACY_TAP_DETECT_1_STRT_ADDR },
    { .type = BMI2_TAP_DETECTOR_2, .page = BMI2_PAGE_6, .start_addr = BMI270_LEGACY_TAP_DETECT_2_STRT_ADDR },
};

/*! @name  Global array that stores the feature output configuration */
const struct bmi2_feature_config bmi270_legacy_feat_out[BMI270_LEGACY_MAX_FEAT_OUT] = {
    { .type = BMI2_STEP_COUNTER, .page = BMI2_PAGE_0, .start_addr = BMI270_LEGACY_STEP_CNT_OUT_STRT_ADDR },
    { .type = BMI2_STEP_ACTIVITY, .page = BMI2_PAGE_0, .start_addr = BMI270_LEGACY_STEP_ACT_OUT_STRT_ADDR },
    { .type = BMI2_ORIENTATION, .page = BMI2_PAGE_0, .start_addr = BMI270_LEGACY_ORIENT_HIGH_G_OUT_STRT_ADDR },
    { .type = BMI2_HIGH_G, .page = BMI2_PAGE_0, .start_addr = BMI270_LEGACY_ORIENT_HIGH_G_OUT_STRT_ADDR },
    { .type = BMI2_GYRO_GAIN_UPDATE, .page = BMI2_PAGE_0, .start_addr = BMI270_LEGACY_GYR_USER_GAIN_OUT_STRT_ADDR },
    { .type = BMI2_GYRO_CROSS_SENSE, .page = BMI2_PAGE_0, .start_addr = BMI270_LEGACY_GYRO_CROSS_SENSE_STRT_ADDR },
    { .type = BMI2_NVM_STATUS, .page = BMI2_PAGE_0, .start_addr = BMI270_LEGACY_NVM_VFRM_OUT_STRT_ADDR },
    { .type = BMI2_VFRM_STATUS, .page = BMI2_PAGE_0, .start_addr = BMI270_LEGACY_NVM_VFRM_OUT_STRT_ADDR }
};

/*! @name  Global array that stores the feature interrupts of BMI270_LEGACY */
struct bmi2_map_int bmi270_legacy_map_int[BMI270_LEGACY_MAX_INT_MAP] = {
    { .type = BMI2_SIG_MOTION, .sens_map_int = BMI270_LEGACY_INT_SIG_MOT_MASK },
    { .type = BMI2_STEP_COUNTER, .sens_map_int = BMI270_LEGACY_INT_STEP_COUNTER_MASK },
    { .type = BMI2_STEP_DETECTOR, .sens_map_int = BMI270_LEGACY_INT_STEP_DETECTOR_MASK },
    { .type = BMI2_STEP_ACTIVITY, .sens_map_int = BMI270_LEGACY_INT_STEP_ACTIVITY_MASK },
    { .type = BMI2_HIGH_G, .sens_map_int = BMI270_LEGACY_INT_HIGH_G_MASK },
    { .type = BMI2_LOW_G, .sens_map_int = BMI270_LEGACY_INT_LOW_G_MASK },
    { .type = BMI2_TAP, .sens_map_int = BMI270_LEGACY_INT_TAP_MASK },
    { .type = BMI2_FLAT, .sens_map_int = BMI270_LEGACY_INT_FLAT_MASK },
    { .type = BMI2_NO_MOTION, .sens_map_int = BMI270_LEGACY_INT_NO_MOT_MASK },
    { .type = BMI2_ANY_MOTION, .sens_map_int = BMI270_LEGACY_INT_ANY_MOT_MASK },
    { .type = BMI2_ORIENTATION, .sens_map_int = BMI270_LEGACY_INT_ORIENT_MASK },
    { .type = BMI2_SINGLE_TAP, .sens_map_int = BMI270_LEGACY_INT_SINGLE_TAP_MASK },
    { .type = BMI2_DOUBLE_TAP, .sens_map_int = BMI270_LEGACY_INT_DOUBLE_TAP_MASK },
    { .type = BMI2_TRIPLE_TAP, .sens_map_int = BMI270_LEGACY_INT_TRIPLE_TAP_MASK }
};

/******************************************************************************/

/*!         Local Function Prototypes
 ******************************************************************************/

/*!
 * @brief This internal API gets tap configurations like data register enable,
 * tap sensitivity threshold, max gesture duration, quite time after gesture,
 * axis selection and wait for timeout.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_tap_config(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API gets tap configurations like data register enable,
 * tap sensitivity threshold, max gesture duration and quite time after gesture
 * from primary page of tap detector.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_tap_config       |
 *  Structure parameters    |               Description
 *--------------------------|--------------------------------------------------
 *                          | By enabling this bit, accel data according to the
 *  data_reg_en             | user defined accel configuration is taken for tap
 *                          | detector feature.
 * -------------------------|---------------------------------------------------
 *  tap_sens_thres          | Scaling factor of additional threshold increment
 *                          | for detection of positive and negative peak of tap.
 * -------------------------|---------------------------------------------------
 *  max_gest_dur            | Maximum duration after the first tap within which the
 *                          | second and/or third tap have to be performed for being
 *                          | detected as double-tap.
 * -------------------------|---------------------------------------------------
 *  quite_time_after_gest   | Minimum quite time between the two gesture detection.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_config_page_primary(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API gets tap configurations like time out and
 * axis selection from secondary page of tap detector.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_tap_config       |
 *  Structure parameters    |               Description
 * -------------------------|---------------------------------------------------
 *  wait_for_timeout        | Wait for the duration set by max_gest_dur after the
 *                          | first tap and report the tap-gesture based on number
 *                          | of taps detected.
 * -------------------------|---------------------------------------------------
 *                          | Selection of axis from 3D-acceleration signal vector
 *  axis_sel                | for tap detection.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_config_page_secondary(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API sets tap configurations like data register enable,
 * tap sensitivity threshold, max gesture duration, quite time after gesture,
 * axis selection and wait for timeout.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_tap_config(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API sets tap configurations like data register enable,
 * tap sensitivity threshold, max gesture duration and quite time after gesture
 * from primary page of tap detector.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_tap_config       |
 *  Structure parameters    |               Description
 *--------------------------|--------------------------------------------------
 *                          | By enabling this bit, accel data according to the
 *  data_reg_en             | user defined accel configuration is taken for tap
 *                          | detector feature.
 * -------------------------|---------------------------------------------------
 *  tap_sens_thres          | Scaling factor of additional threshold increment
 *                          | for detection of positive and negative peak of tap.
 * -------------------------|---------------------------------------------------
 *  max_gest_dur            | Maximum duration after the first tap within which the
 *                          | second and/or third tap have to be performed for being
 *                          | detected as double-tap.
 * -------------------------|---------------------------------------------------
 *  quite_time_after_gest   | Minimum quite time between the two gesture detection.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_config_page_primary(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API sets the tap configurations like time out and
 * axis selection to secondary page of tap detector.
 *
 * @param[out]      tap        : Void pointer to store bmi2_tap_config structure.
 * @param[in, out]  bmi2_dev   : Void pointer to store bmi2_dev structure.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_tap_config       |
 *  Structure parameters    |               Description
 * -------------------------|---------------------------------------------------
 *  wait_for_timeout        | Wait for the duration set by max_gest_dur after the
 *                          | first tap and report the tap-gesture based on number
 *                          | of taps detected.
 * -------------------------|---------------------------------------------------
 *                          | Selection of axis from 3D-acceleration signal vector
 *  axis_sel                | for tap detection.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_config_page_secondary(void *tap, void *bmi2_dev);

/*!
 * @brief This internal API is used to validate the device pointer for
 * null conditions.
 *
 * @param[in] dev : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t null_ptr_check(const struct bmi2_dev *dev);

/*!
 * @brief This internal API enables the selected sensor/features.
 *
 * @param[in]       sensor_sel    : Selects the desired sensor.
 * @param[in, out]  dev           : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t sensor_enable(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API disables the selected sensor/features.
 *
 * @param[in]       sensor_sel    : Selects the desired sensor.
 * @param[in, out]  dev           : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t sensor_disable(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API selects the sensors/features to be enabled or
 * disabled.
 *
 * @param[in]  sens_list    : Pointer to select the sensor.
 * @param[in]  n_sens       : Number of sensors selected.
 * @param[out] sensor_sel   : Gets the selected sensor.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t select_sensor(const uint8_t *sens_list, uint8_t n_sens, uint64_t *sensor_sel);

/*!
 * @brief This internal API is used to enable/disable any-motion feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables any-motion.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables any-motion.
 * BMI2_ENABLE  | Enables any-motion.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_any_motion(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable no-motion feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables no-motion.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables no-motion.
 * BMI2_ENABLE  | Enables no-motion.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_no_motion(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable sig-motion feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables sig-motion.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables sig-motion.
 * BMI2_ENABLE  | Enables sig-motion.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_sig_motion(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable step detector feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables step-detector.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables step detector
 * BMI2_ENABLE  | Enables step detector
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_detector(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable step counter feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables step counter.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables step counter
 * BMI2_ENABLE  | Enables step counter
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_counter(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * single tap.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables single tap.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables single tap.
 * BMI2_ENABLE  | Enables single tap
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_single_tap(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * double tap.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables double tap.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables double tap.
 * BMI2_ENABLE  | Enables double tap
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_double_tap(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * triple tap.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables triple tap.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables triple tap.
 * BMI2_ENABLE  | Enables triple tap
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_triple_tap(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable orientation feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables orientation.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables orientation
 * BMI2_ENABLE  | Enables orientation
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_orientation(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable high-g feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables high-g.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables high-g
 * BMI2_ENABLE  | Enables high-g
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_high_g(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable low-g feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables low-g.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables low-g
 * BMI2_ENABLE  | Enables low-g
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_low_g(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable flat feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables flat.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables flat
 * BMI2_ENABLE  | Enables flat
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_flat(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API gives an option to enable offset correction
 * feature of gyroscope, either internally or by the host.
 *
 * @param[in] enable    : Enables/Disables self-offset correction.
 * @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  enable      |  Description
 * -------------|---------------
 * BMI2_ENABLE  | gyroscope offset correction values are set internally
 * BMI2_DISABLE | gyroscope offset correction values has to be set by host
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_gyro_self_offset_corr(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable gyroscope user gain
 * feature.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables gyroscope user gain.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables gyroscope user gain
 * BMI2_ENABLE  | Enables gyroscope user gain
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_gyro_user_gain(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets any-motion configurations like axes select,
 * duration, threshold and output-configuration.
 *
 * @param[in]      config      : Structure instance of bmi2_any_motion_config.
 * @param[in, out] dev         : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *  bmi2_any_motion_config  |
 *  Structure parameters    |                   Description
 *--------------------------|--------------------------------------------------
 *                          | Defines the number of  consecutive data points for
 *                          | which the threshold condition must be respected,
 *                          | for interrupt assertion. It is expressed in  50 Hz
 *  duration                | samples (20 msec).
 *                          | Range is 0 to 163sec.
 *                          | Default value is 5 = 100ms.
 * -------------------------|---------------------------------------------------
 *                          | Slope threshold value for in 5.11g format.
 *  threshold               | Range is 0 to 1g.
 *                          | Default value is 0xAA = 83mg.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_any_motion_config(const struct bmi2_any_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets no-motion configurations like axes select,
 * duration, threshold and output-configuration.
 *
 * @param[in]      config       : Structure instance of bmi2_no_motion_config.
 * @param[in, out] dev          : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *  bmi2_no_motion_config   |
 *  Structure parameters    |                   Description
 *--------------------------|--------------------------------------------------
 *                          | Defines the number of  consecutive data points for
 *                          | which the threshold condition must be respected,
 *                          | for interrupt assertion. It is expressed in  50 Hz
 *  duration                | samples (20 msec).
 *                          | Range is 0 to 163sec.
 *                          | Default value is 5 = 100ms.
 * -------------------------|---------------------------------------------------
 *                          | Slope threshold value for no-motion detection.
 *  threshold               | Range is 0 to 1g.
 *                          | Default value is 0x90 = 70mg.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_no_motion_config(const struct bmi2_no_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets sig-motion configurations like block-size,
 * output-configuration and other parameters.
 *
 * @param[in]      config        : Structure instance of bmi2_sig_motion_config.
 * @param[in, out] dev           : Structure instance of bmi2_dev.
 *
 *----------------------------------------------------------------------------
 *  bmi2_sig_motion_config  |
 *  Structure parameters    |                   Description
 * -------------------------|---------------------------------------------------
 *                          | Defines the duration after which the significant
 *  block_size              | motion interrupt is triggered. It is expressed in
 *                          | 50 Hz samples (20 ms). Default value is 0xFA=5sec.
 *--------------------------|---------------------------------------------------
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_sig_motion_config(const struct bmi2_sig_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets step counter/detector/activity configurations.
 *
 * @param[in] config      : Structure instance of bmi2_step_config.
 * @param[in] dev         : Structure instance of bmi2_dev.
 *
 *---------------------------------------------------------------------------
 *    bmi2_step_config      |
 *  Structure parameters    |                 Description
 *--------------------------|--------------------------------------------------
 *                          | The Step-counter will trigger output every time
 *                          | the number of steps are counted. Holds implicitly
 *  water-mark level        | a 20x factor, so the range is 0 to 10230,
 *                          | with resolution of 20 steps.
 * -------------------------|---------------------------------------------------
 *  reset counter           | Flag to reset the counted steps.
 * -------------------------|---------------------------------------------------
 *  step_buffer_size        | Step counter buffer size
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_config(const struct bmi2_step_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets tap configurations like sensitivity,
 * single, double and triple tap enable and output-configuration.
 *
 * @param[in]      config         : Structure instance of bmi2_tap_config.
 * @param[in, out] dev            : Structure instance of bmi2_dev.
 *
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_tap_detector_config(struct bmi2_tap_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets orientation configurations like upside/down
 * detection, symmetrical modes, blocking mode, theta, hysteresis and output
 * configuration.
 *
 * @param[in]      config         : Structure instance of bmi2_orient_config.
 * @param[in, out] dev            : Structure instance of bmi2_dev.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_orient_config    |
 *   Structure parameters   |               Description
 *--------------------------|--------------------------------------------------
 *  upside/down             |  Enables upside/down detection, if set to 1.
 *  detection               |
 * -------------------------|---------------------------------------------------
 *                          | Sets the mode:
 *      mode                | Values 0 or 3 - symmetrical.
 *                          | Value 1       - high asymmetrical
 *                          | Value 2       - low asymmetrical
 * -------------------------|---------------------------------------------------
 *                          | Enable - no orientation interrupt will be set.
 *      blocking            | Default value: 3, the most restrictive blocking.
 * -------------------------|---------------------------------------------------
 *                          | Threshold angle used in blocking mode.
 *      theta               | Theta = 64 * (tan(angle)^2)
 *                          | Default value: 40, equivalent to 38 degrees angle.
 * -------------------------|---------------------------------------------------
 *                          | Acceleration hysteresis for Orientation detection
 *                          | is expressed in 5.11g format.
 *      hysteresis          | Default value is 128 = 0.0625g.
 *                          | Range is 0 to 1g.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_orient_config(const struct bmi2_orient_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets high-g configurations like threshold,
 * hysteresis, duration, and output configuration.
 *
 * @param[in]      config         : Structure instance of bmi2_high_g_config.
 * @param[in, out] dev            : Structure instance of bmi2_dev.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *      bmi2_high_g_config  |
 *      Structure parameter |               Description
 *--------------------------|--------------------------------------------------
 *      threshold           |  The acceleration threshold above which the
 *                          |  high_g motion is signaled.
 *                          |  Threshold value for high_g feature.
 *                          |  Range is 0 to 16g.
 *                          |  Default value is 10000 = 4.9g.
 * -------------------------|---------------------------------------------------
 *                          | Hysteresis value for high_g feature.
 *      hysteresis          | Range is 0 to 2g.
 *                          | Default value is 1000 = 0.49g.
 * -------------------------|---------------------------------------------------
 *                          | Holds the duration in 200 Hz samples (5 ms) for
 *                          | which the threshold has to be exceeded.
 *      duration            | Default value 4 = 20 msec.
 *                          | Range is 0 to 20sec.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_high_g_config(const struct bmi2_high_g_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets low-g configurations like threshold,
 * hysteresis, duration, and output configuration.
 *
 * @param[in]      config         : Structure instance of bmi2_low_g_config.
 * @param[in, out] dev            : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *    bmi2_low_g_config     |
 *   Structure parameters   |               Description
 *--------------------------|--------------------------------------------------
 *  threshold               |  The acceleration threshold above which the
 *                          |  low-g motion is signaled.
 *                          |  Range is 0 to 1g.
 *                          |  Default value is 512 = 0.25g.
 * -------------------------|---------------------------------------------------
 *                          | Hysteresis value for low_g feature.
 *                          | Range is 0 to 0.5g.
 *  hysteresis              | Default value is 256 = 0.125g.
 * -------------------------|---------------------------------------------------
 *                          | Holds the duration in 50 Hz samples (20 ms) for
 *                          | which the threshold has to be exceeded.
 *  duration                | Range is 0 to 82 sec.
 *                          | Default value is 0 = 0 ms.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_low_g_config(const struct bmi2_low_g_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets flat configurations like theta, blocking,
 * hold-time, hysteresis, and output configuration.
 *
 * @param[in]      config         : Structure instance of bmi2_flat_config.
 * @param[in, out] dev            : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *      bmi2_flat_config    |
 *      Structure parameters|               Description
 *--------------------------|--------------------------------------------------
 *                          | Sets the theta angle, used for detecting flat
 *                          | position.
 *      theta               | Theta = 64 * (tan(angle)^2);
 *                          | Default value is 8, equivalent to 20 degrees angle
 * -------------------------|---------------------------------------------------
 *                          | Hysteresis for Theta Flat detection.
 *      hysteresis          | Default value is 9 = 2.5 degrees, corresponding
 *                          | to the default Theta angle of 20 degrees.
 * -------------------------|---------------------------------------------------
 *                          | Sets the blocking mode. If blocking is set, no
 *                          | Flat interrupt will be triggered.
 *      blocking            | Default value is 2, the most restrictive blocking
 *                          | mode.
 * -------------------------|---------------------------------------------------
 *                          | Holds the duration in 50Hz samples for which the
 *                          | condition has to be respected.
 *      hold-time           | Default value is 32 = 640 msec.
 *                          | Range is 0 to 5.1 sec.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_flat_config(const struct bmi2_flat_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets any-motion configurations like axes select,
 * duration, threshold and output-configuration.
 *
 * @param[out] config        : Structure instance of bmi2_any_motion_config.
 * @param[in, out]  dev      : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *  bmi2_any_motion_config  |
 *  Structure parameters    |                   Description
 *--------------------------|--------------------------------------------------
 *                          | Defines the number of  consecutive data points for
 *                          | which the threshold condition must be respected,
 *                          | for interrupt assertion. It is expressed in  50 Hz
 *  duration                | samples (20 msec).
 *                          | Range is 0 to 163sec.
 *                          | Default value is 5 = 100ms.
 * -------------------------|---------------------------------------------------
 *                          | Slope threshold value for in 5.11g format.
 *  threshold               | Range is 0 to 1g.
 *                          | Default value is 0xAA = 83mg.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_any_motion_config(struct bmi2_any_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets no-motion configurations like axes select,
 * duration, threshold and output-configuration.
 *
 * @param[out]      config    : Structure instance of bmi2_no_motion_config.
 * @param[in, out]  dev       : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *  bmi2_no_motion_config   |
 *  Structure parameters    |                   Description
 *--------------------------|--------------------------------------------------
 *                          | Defines the number of  consecutive data points for
 *                          | which the threshold condition must be respected,
 *                          | for interrupt assertion. It is expressed in  50 Hz
 *  duration                | samples (20 msec).
 *                          | Range is 0 to 163sec.
 *                          | Default value is 5 = 100ms.
 * -------------------------|---------------------------------------------------
 *                          | Slope threshold value for no-motion detection.
 *  threshold               | Range is 0 to 1g.
 *                          | Default value is 0x90 = 70mg.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_no_motion_config(struct bmi2_no_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets sig-motion configurations like block-size,
 * output-configuration and other parameters.
 *
 * @param[out]      config    : Structure instance of bmi2_sig_motion_config.
 * @param[in, out]  dev       : Structure instance of bmi2_dev.
 *
 *----------------------------------------------------------------------------
 *  bmi2_sig_motion_config  |
 *  Structure parameters    |                   Description
 * -------------------------|---------------------------------------------------
 *                          | Defines the duration after which the significant
 *  block_size              | motion interrupt is triggered. It is expressed in
 *                          | 50 Hz samples (20 ms). Default value is 0xFA=5sec.
 *--------------------------|---------------------------------------------------
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_sig_motion_config(struct bmi2_sig_motion_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets step counter/detector/activity configurations.
 *
 * @param[out] config      : Structure instance of bmi2_step_config.
 * @param[in]  dev         : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *    bmi2_step_config      |
 *  Structure parameters    |                 Description
 *--------------------------|--------------------------------------------------
 *                          | The Step-counter will trigger output every time
 *                          | the number of steps are counted. Holds implicitly
 *  water-mark level        | a 20x factor, so the range is 0 to 10230,
 *                          | with resolution of 20 steps.
 * -------------------------|---------------------------------------------------
 *  reset counter           | Flag to reset the counted steps.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_step_config(struct bmi2_step_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets tap configurations like sensitivity,
 * single, double and triple tap enable and output-configuration.
 *
 * @param[out]      config    : Structure instance of bmi2_tap_config.
 * @param[in, out]  dev       : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_tap_detector_config(struct bmi2_tap_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets orientation configurations like upside/down
 * detection, symmetrical modes, blocking mode, theta, hysteresis and output
 * configuration.
 *
 * @param[out]     config         : Structure instance of bmi2_orient_config.
 * @param[in, out] dev            : Structure instance of bmi2_dev.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *    bmi2_orient_config    |
 *   Structure parameters   |               Description
 *--------------------------|--------------------------------------------------
 *  upside/down             |  Enables upside/down detection, if set to 1.
 *  detection               |
 * -------------------------|---------------------------------------------------
 *                          | Sets the mode:
 *      mode                | Values 0 or 3 - symmetrical.
 *                          | Value 1       - high asymmetrical
 *                          | Value 2       - low asymmetrical
 * -------------------------|---------------------------------------------------
 *                          | Enable - no orientation interrupt will be set.
 *      blocking            | Default value: 3, the most restrictive blocking.
 * -------------------------|---------------------------------------------------
 *                          | Threshold angle used in blocking mode.
 *      theta               | Theta = 64 * (tan(angle)^2)
 *                          | Default value: 40, equivalent to 38 degrees angle.
 * -------------------------|---------------------------------------------------
 *                          | Acceleration hysteresis for Orientation detection
 *                          | is expressed in 5.11g format.
 *      hysteresis          | Default value is 128 = 0.0625g.
 *                          | Range is 0 to 1g.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_orient_config(struct bmi2_orient_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets high-g configurations like threshold,
 * hysteresis, duration, and output configuration.
 *
 * @param[out]     config         : Structure instance of bmi2_high_g_config.
 * @param[in, out] dev            : Structure instance of bmi2_dev.
 *
 * @verbatim
 *-----------------------------------------------------------------------------
 *      bmi2_high_g_config  |
 *      Structure parameter |               Description
 *--------------------------|--------------------------------------------------
 *      threshold           |  The acceleration threshold above which the
 *                          |  high_g motion is signaled.
 *                          |  Holds threshold in 5.11 g format.
 *                          |  Default is 3072 = 1.5 g.
 *                          |  Range is 0 to 16g.
 * -------------------------|---------------------------------------------------
 *                          | Acceleration hysteresis for high-g detection
 *                          | is expressed in 1.11g format.
 *      hysteresis          | Default value is 256 = 0.125 g.
 *                          | Range is 0 to 2g.
 *                          | Should be smaller than threshold.
 * -------------------------|---------------------------------------------------
 *                          | Holds the duration in 200 Hz samples (5 ms) for
 *                          | which the threshold has to be exceeded.
 *      duration            |Default value 4 = 20 msec.
 *                          | Range is 0 to 20sec.
 * -------------------------|---------------------------------------------------
 *  select_x, select_y,     |
 *       select_z           |  Selects the feature on a per-axis basis
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_high_g_config(struct bmi2_high_g_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets low-g configurations like threshold,
 * hysteresis, duration, and output configuration.
 *
 * @param[out]     config     : Structure instance of bmi2_low_g_config.
 * @param[in, out] dev        : Structure instance of bmi2_dev.
 *
 * @verbatim
 *    bmi2_low_g_config     |
 *   Structure parameters   |               Description
 *--------------------------|--------------------------------------------------
 *  threshold               |  The acceleration threshold above which the
 *                          |  low-g motion is signaled.
 *                          |  Holds threshold in 5.11 g format.
 *                          |  Default is 3072 = 1.5 g.
 * -------------------------|---------------------------------------------------
 *                          |  Acceleration hysteresis for low-g detection
 *  hysteresis              | is expressed in 1.11g format.
 *                          | Default value is 512 = 0.250 g.
 *                          | Should be smaller than threshold.
 * -------------------------|---------------------------------------------------
 *                          | Holds the duration in 50 Hz samples (20 ms) for
 *                          | which the threshold has to be exceeded.
 *  duration                | Default value 0 = 1 validation sample = (0+1)*20
 *                          | = 20 ms.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_low_g_config(struct bmi2_low_g_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets flat configurations like theta, blocking,
 * hold-time, hysteresis, and output configuration.
 *
 * @param[out]     config           : Structure instance of bmi2_flat_config.
 * @param[in, out] dev              : Structure instance of bmi2_dev.
 *
 * @verbatim
 *----------------------------------------------------------------------------
 *      bmi2_flat_config    |
 *      Structure parameters|               Description
 *--------------------------|--------------------------------------------------
 *                          | Theta angle, used for detecting flat
 *                          | position.
 *      theta               | Theta = 64 * (tan(angle)^2);
 *                          | Default value is 8, equivalent to 20 degrees angle
 * -------------------------|---------------------------------------------------
 *                          | Hysteresis for Theta Flat detection.
 *      hysteresis          | Default value is 9 = 2.5 degrees, corresponding
 *                          | to the default Theta angle of 20 degrees.
 * -------------------------|---------------------------------------------------
 *                          | Sets the blocking mode. If blocking is set, no
 *                          | Flat interrupt will be triggered.
 *      blocking            | Default value is 2, the most restrictive blocking
 *                          | mode.
 * -------------------------|---------------------------------------------------
 *                          | Holds the duration in 50Hz samples for which the
 *                          | condition has to be respected.
 *      hold-time           | Default value is 32 = 640 m-sec.
 *                          | Range is 0 to 5.1 sec.
 * -------------------------|---------------------------------------------------
 * @endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_flat_config(struct bmi2_flat_config *config, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the output values of high-g.
 *
 * @param[out] high_g_out       : Pointer to the stored high-g output.
 * @param[in]  dev              : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_high_g_output(uint8_t *high_g_out, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the output values of step activity.
 *
 * @param[out] step_act         : Pointer to the stored step activity data.
 * @param[in]  dev      : Structure instance of bmi2_dev.
 *
 * *step_act  |  Output
 * -----------|------------
 * 0x00       |  STILL
 * 0x01       |  WALKING
 * 0x02       |  RUNNING
 * 0x03       |  UNKNOWN
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_step_activity_output(uint8_t *step_act, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the output values of orientation: portrait-
 * landscape and face up-down.
 *
 * @param[out] orient_out      : Structure pointer to the orientation data.
 * @param[in]  dev             : Structure instance of bmi2_dev.
 *
 *
 * portrait   |
 * landscape  |  Output
 * -----------|------------
 * 0x00       |  PORTRAIT UPRIGHT
 * 0x01       |  LANDSCAPE LEFT
 * 0x02       |  PORTRAIT UPSIDE DOWN
 * 0x03       |  LANDSCAPE RIGHT
 *
 * Face       |
 * up-down    |  Output
 * -----------|------------
 * 0x00       |  FACE-UP
 * 0x01       |  FACE-DOWN
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_orient_output(struct bmi2_orientation_output *orient_out, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the output values of step counter.
 *
 * @param[out] step_count       : Pointer to the stored step counter data.
 * @param[in]  dev              : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_step_counter_output(uint32_t *step_count, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the error status related to NVM.
 *
 * @param[out] nvm_err_stat     : Stores the NVM error status.
 * @param[in]  dev              : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_nvm_error_status(struct bmi2_nvm_err_status *nvm_err_stat, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets the error status related to virtual frames.
 *
 * @param[out] vfrm_err_stat    : Stores the VFRM related error status.
 * @param[in]  dev              : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_vfrm_error_status(struct bmi2_vfrm_err_status *vfrm_err_stat, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to get enable status of gyroscope user gain
 * update.
 *
 * @param[out] status         : Stores status of gyroscope user gain update.
 * @param[in]  dev            : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_user_gain_upd_status(uint8_t *status, struct bmi2_dev *dev);

/*!
 * @brief This internal API enables/disables compensation of the gain defined
 * in the GAIN register.
 *
 * @param[in] enable    : Enables/Disables gain compensation
 * @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  enable      |  Description
 * -------------|---------------
 * BMI2_ENABLE  | Enable gain compensation.
 * BMI2_DISABLE | Disable gain compensation.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t enable_gyro_gain(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to extract the output feature configuration
 * details like page and start address from the look-up table.
 *
 * @param[out] feat_output      : Structure that stores output feature
 *                              configurations.
 * @param[in] type              : Type of feature or sensor.
 * @param[in] dev               : Structure instance of bmi2_dev.
 *
 * @return Returns the feature found flag.
 *
 * @retval  BMI2_FALSE : Feature not found
 *          BMI2_TRUE  : Feature found
 */
static uint8_t extract_output_feat_config(struct bmi2_feature_config *feat_output,
                                          uint8_t type,
                                          const struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable/disable step activity detection.
 *
 * @param[in] dev            : Structure instance of bmi2_dev.
 * @param[in] enable         : Enables/Disables step activity.
 *
 * Enable       |  Description
 * -------------|---------------
 * BMI2_DISABLE | Disables step activity
 * BMI2_ENABLE  | Enables step activity
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_step_activity(uint8_t enable, struct bmi2_dev *dev);

/*!
 * @brief This internal API sets feature configuration to the sensor.
 *
 * @param[in]       sens_cfg     : Structure instance of bmi2_sens_config.
 * @param[in]       loop         : Variable to loop the sensor feature.
 * @param[in, out]  dev          : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t set_feat_config(struct bmi2_sens_config *sens_cfg, uint8_t loop, struct bmi2_dev *dev);

/*!
 * @brief This internal API gets feature configuration from the sensor.
 *
 * @param[in]       sens_cfg     : Structure instance of bmi2_sens_config.
 * @param[in]       loop         : Variable to loop the sensor feature.
 * @param[in, out]  dev          : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t get_feat_config(struct bmi2_sens_config *sens_cfg, uint8_t loop, struct bmi2_dev *dev);

/*!
 * @brief This internal API selects the sensors/features to be enabled or
 * disabled.
 *
 * @param[in]  sens_list    : Pointer to select the sensor.
 * @param[in]  count        : Number of sensors selected.
 * @param[in] sensor_sel    : Gets the selected sensor.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t sens_select(const uint8_t *sens_list, uint8_t count, uint64_t *sensor_sel);

/*!
 * @brief This internal API is used to enable main sensors like accel, gyro, aux and temperature.
 *
 * @param[in] sensor_sel    : Enables the selected sensor.
 * @param[in, out]  dev     : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t enable_main_sensors(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to enable sensor features.
 *
 * @param[in] sensor_sel    : Enables features of selected sensor.
 * @param[in, out]  dev     : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t enable_sensor_features(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to disable main sensors like accel, gyro, aux and temperature.
 *
 * @param[in] sensor_sel    : Disables the selected sensor.
 * @param[in, out]  dev     : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t disable_main_sensors(uint64_t sensor_sel, struct bmi2_dev *dev);

/*!
 * @brief This internal API is used to disable sensor features.
 *
 * @param[in] sensor_sel    : Disables features of selected sensor.
 * @param[in, out]  dev     : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
static int8_t disable_sensor_features(uint64_t sensor_sel, struct bmi2_dev *dev);

/***************************************************************************/

/*!         User Interface Definitions
 ****************************************************************************/

/*!
 *  @brief This API:
 *  1) Updates the device structure with address of the configuration file.
 *  2) Initializes BMI270_LEGACY sensor.
 *  3) Writes the configuration file.
 *  4) Updates the feature offset parameters in the device structure.
 *  5) Updates the maximum number of pages, in the device structure.
 */
int8_t bmi270_legacy_init(struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if (rslt == BMI2_OK)
    {
        /* Assign chip id of BMI270_LEGACY */
        dev->chip_id = BMI270_LEGACY_CHIP_ID;

        /* Get the size of config array */
        dev->config_size = sizeof(bmi270_legacy_config_file);

        /* Enable the variant specific features if any */
        dev->variant_feature = BMI2_CRT_RTOSK_ENABLE | BMI2_GYRO_CROSS_SENS_ENABLE;

        /* An extra dummy byte is read during SPI read */
        if (dev->intf == BMI2_SPI_INTF)
        {
            dev->dummy_byte = 1;
        }
        else
        {
            dev->dummy_byte = 0;
        }

        /* If configuration file pointer is not assigned any address */
        if (!dev->config_file_ptr)
        {
            /* Give the address of the configuration file array to
             * the device pointer
             */
            dev->config_file_ptr = bmi270_legacy_config_file;
        }

        /* Initialize BMI2 sensor */
        rslt = bmi2_sec_init(dev);
        if (rslt == BMI2_OK)
        {
            /* Assign the offsets of the feature input
             * configuration to the device structure
             */
            dev->feat_config = bmi270_legacy_feat_in;

            /* Assign the offsets of the feature output to
             * the device structure
             */
            dev->feat_output = bmi270_legacy_feat_out;

            /* Assign the maximum number of pages to the
             * device structure
             */
            dev->page_max = BMI270_LEGACY_MAX_PAGE_NUM;

            /* Assign maximum number of input sensors
             * features to device structure
             */
            dev->input_sens = BMI270_LEGACY_MAX_FEAT_IN;

            /* Assign maximum number of output sensors
             * features to device structure
             */
            dev->out_sens = BMI270_LEGACY_MAX_FEAT_OUT;

            /* Assign the tap get configurations to function pointer */
            dev->get_tap_config = get_tap_config;

            /* Assign the tap set configurations to function pointer */
            dev->set_tap_config = set_tap_config;

            /* Assign the offsets of the feature interrupt
             * to the device structure
             */
            dev->map_int = bmi270_legacy_map_int;

            /* Assign maximum number of feature interrupts
             * to device structure
             */
            dev->sens_int_map = BMI270_LEGACY_MAX_INT_MAP;

            /* Get the gyroscope cross axis sensitivity */
            rslt = bmi2_get_gyro_cross_sense(dev);
        }
    }

    return rslt;
}

/*!
 * @brief This API selects the sensors/features to be enabled.
 */
int8_t bmi270_legacy_sensor_enable(const uint8_t *sens_list, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to select sensor */
    uint64_t sensor_sel = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_list != NULL))
    {
        /* Get the selected sensors */
        rslt = select_sensor(sens_list, n_sens, &sensor_sel);
        if (rslt == BMI2_OK)
        {
            /* Enable the selected sensors */
            rslt = sensor_enable(sensor_sel, dev);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API selects the sensors/features to be disabled.
 */
int8_t bmi270_legacy_sensor_disable(const uint8_t *sens_list, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to select sensor */
    uint64_t sensor_sel = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_list != NULL))
    {
        /* Get the selected sensors */
        rslt = select_sensor(sens_list, n_sens, &sensor_sel);
        if (rslt == BMI2_OK)
        {
            /* Disable the selected sensors */
            rslt = sensor_disable(sensor_sel, dev);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API sets the sensor/feature configuration.
 */
int8_t bmi270_legacy_set_sensor_config(struct bmi2_sens_config *sens_cfg, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define loop */
    uint8_t loop;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_cfg != NULL))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;

        for (loop = 0; loop < n_sens; loop++)
        {
            if ((sens_cfg[loop].type == BMI2_ACCEL) || (sens_cfg[loop].type == BMI2_GYRO) ||
                (sens_cfg[loop].type == BMI2_AUX) || (sens_cfg[loop].type == BMI2_GYRO_GAIN_UPDATE))
            {
                rslt = bmi2_set_sensor_config(&sens_cfg[loop], 1, dev);
            }
            else
            {
                /* Disable Advance power save if enabled for auxiliary
                 * and feature configurations
                 */
                if (aps_stat == BMI2_ENABLE)
                {
                    /* Disable advance power save if
                     * enabled
                     */
                    rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
                }

                if (rslt == BMI2_OK)
                {
                    rslt = set_feat_config(sens_cfg, loop, dev);
                }
                /* Return error if any of the set configurations fail */
                else
                {
                    break;
                }
            }
        }

        /* Enable Advance power save if disabled while configuring and
         * not when already disabled
         */
        if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
        {
            rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API gets the sensor/feature configuration.
 */
int8_t bmi270_legacy_get_sensor_config(struct bmi2_sens_config *sens_cfg, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define loop */
    uint8_t loop;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_cfg != NULL))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;
        for (loop = 0; loop < n_sens; loop++)
        {
            if ((sens_cfg[loop].type == BMI2_ACCEL) || (sens_cfg[loop].type == BMI2_GYRO) ||
                (sens_cfg[loop].type == BMI2_AUX) || (sens_cfg[loop].type == BMI2_GYRO_GAIN_UPDATE))
            {
                rslt = bmi2_get_sensor_config(&sens_cfg[loop], 1, dev);
            }
            else
            {
                /* Disable Advance power save if enabled for auxiliary
                 * and feature configurations
                 */
                if ((sens_cfg[loop].type >= BMI2_MAIN_SENS_MAX_NUM) || (sens_cfg[loop].type == BMI2_AUX))
                {

                    if (aps_stat == BMI2_ENABLE)
                    {
                        /* Disable advance power save if
                         * enabled
                         */
                        rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
                    }
                }

                if (rslt == BMI2_OK)
                {
                    rslt = get_feat_config(sens_cfg, loop, dev);
                }
                /* Return error if any of the get configurations fail */
                else
                {
                    break;
                }
            }
        }

        /* Enable Advance power save if disabled while configuring and
         * not when already disabled
         */
        if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
        {
            rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API gets the feature data.
 */
int8_t bmi270_legacy_get_feature_data(struct bmi2_feat_sensor_data *feature_data, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define loop */
    uint8_t loop;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (feature_data != NULL))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;
        for (loop = 0; loop < n_sens; loop++)
        {
            if ((feature_data[loop].type == BMI2_GYRO_GAIN_UPDATE) ||
                (feature_data[loop].type == BMI2_GYRO_CROSS_SENSE))
            {
                rslt = bmi2_get_feature_data(&feature_data[loop], 1, dev);
            }
            else
            {
                /* Disable Advance power save if enabled for feature
                 * configurations
                 */
                if (feature_data[loop].type >= BMI2_MAIN_SENS_MAX_NUM)
                {
                    if (aps_stat == BMI2_ENABLE)
                    {
                        /* Disable advance power save if
                         * enabled
                         */
                        rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
                    }
                }

                if (rslt == BMI2_OK)
                {
                    switch (feature_data[loop].type)
                    {
                        case BMI2_STEP_COUNTER:

                            /* Get step counter output */
                            rslt = get_step_counter_output(&feature_data[loop].sens_data.step_counter_output, dev);
                            break;

                        case BMI2_STEP_ACTIVITY:

                            /* Get step activity output */
                            rslt = get_step_activity_output(&feature_data[loop].sens_data.activity_output, dev);
                            break;

                        case BMI2_ORIENTATION:

                            /* Get orientation output */
                            rslt = get_orient_output(&feature_data[loop].sens_data.orient_output, dev);
                            break;

                        case BMI2_HIGH_G:

                            /* Get high-g output */
                            rslt = get_high_g_output(&feature_data[loop].sens_data.high_g_output, dev);
                            break;

                        case BMI2_NVM_STATUS:

                            /* Get NVM error status  */
                            rslt = get_nvm_error_status(&feature_data[loop].sens_data.nvm_status, dev);
                            break;

                        case BMI2_VFRM_STATUS:

                            /* Get VFRM error status  */
                            rslt = get_vfrm_error_status(&feature_data[loop].sens_data.vfrm_status, dev);
                            break;

                        default:
                            rslt = BMI2_E_INVALID_SENSOR;
                            break;
                    }
                }

                /* Return error if any of the get sensor data fails */
                if (rslt != BMI2_OK)
                {
                    break;
                }
            }

            /* Enable Advance power save if disabled while
             * configuring and not when already disabled
             */
            if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
            {
                rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
            }
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API updates the gyroscope user-gain.
 */
int8_t bmi270_legacy_update_gyro_user_gain(const struct bmi2_gyro_user_gain_config *user_gain, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to select sensor */
    uint8_t sens_sel[2] = { BMI2_GYRO, BMI2_GYRO_GAIN_UPDATE };

    /* Structure to define sensor configurations */
    struct bmi2_sens_config sens_cfg;

    /* Variable to store status of user-gain update module */
    uint8_t status = 0;

    /* Variable to define count */
    uint8_t count = 100;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (user_gain != NULL))
    {
        /* Select type of feature */
        sens_cfg.type = BMI2_GYRO_GAIN_UPDATE;

        /* Get the user gain configurations */
        rslt = bmi270_legacy_get_sensor_config(&sens_cfg, 1, dev);
        if (rslt == BMI2_OK)
        {
            /* Get the user-defined ratio */
            sens_cfg.cfg.gyro_gain_update = *user_gain;

            /* Set rate ratio for all axes */
            rslt = bmi270_legacy_set_sensor_config(&sens_cfg, 1, dev);
        }

        /* Disable gyroscope */
        if (rslt == BMI2_OK)
        {
            rslt = bmi270_legacy_sensor_disable(&sens_sel[0], 1, dev);
        }

        /* Enable gyroscope user-gain update module */
        if (rslt == BMI2_OK)
        {
            rslt = bmi270_legacy_sensor_enable(&sens_sel[1], 1, dev);
        }

        /* Set the command to trigger the computation */
        if (rslt == BMI2_OK)
        {
            rslt = bmi2_set_command_register(BMI2_USR_GAIN_CMD, dev);
        }

        if (rslt == BMI2_OK)
        {
            /* Poll until enable bit of user-gain update is 0 */
            while (count--)
            {
                rslt = get_user_gain_upd_status(&status, dev);
                if ((rslt == BMI2_OK) && (status == 0))
                {
                    /* Enable compensation of gain defined
                     * in the GAIN register
                     */
                    rslt = enable_gyro_gain(BMI2_ENABLE, dev);

                    /* Enable gyroscope */
                    if (rslt == BMI2_OK)
                    {
                        rslt = bmi270_legacy_sensor_enable(&sens_sel[0], 1, dev);
                    }

                    break;
                }

                dev->delay_us(10000, dev->intf_ptr);
            }

            /* Return error if user-gain update is failed */
            if ((rslt == BMI2_OK) && (status != 0))
            {
                rslt = BMI2_E_GYR_USER_GAIN_UPD_FAIL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API reads the compensated gyroscope user-gain values.
 */
int8_t bmi270_legacy_read_gyro_user_gain(struct bmi2_gyro_user_gain_data *gyr_usr_gain, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define register data */
    uint8_t reg_data[3] = { 0 };

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (gyr_usr_gain != NULL))
    {
        /* Get the gyroscope compensated gain values */
        rslt = bmi2_get_regs(BMI2_GYR_USR_GAIN_0_ADDR, reg_data, 3, dev);
        if (rslt == BMI2_OK)
        {
            /* Gyroscope user gain correction X-axis */
            gyr_usr_gain->x = (int8_t)BMI2_GET_BIT_POS0(reg_data[0], BMI2_GYR_USR_GAIN_X);

            /* Gyroscope user gain correction Y-axis */
            gyr_usr_gain->y = (int8_t)BMI2_GET_BIT_POS0(reg_data[1], BMI2_GYR_USR_GAIN_Y);

            /* Gyroscope user gain correction z-axis */
            gyr_usr_gain->z = (int8_t)BMI2_GET_BIT_POS0(reg_data[2], BMI2_GYR_USR_GAIN_Z);
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API maps/unmaps feature interrupts to that of interrupt pins.
 */
int8_t bmi270_legacy_map_feat_int(const struct bmi2_sens_int_config *sens_int, uint8_t n_sens, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define loop */
    uint8_t loop;

    /* Null-pointer check */
    rslt = null_ptr_check(dev);
    if ((rslt == BMI2_OK) && (sens_int != NULL))
    {
        for (loop = 0; loop < n_sens; loop++)
        {
            switch (sens_int[loop].type)
            {
                case BMI2_SIG_MOTION:
                case BMI2_TAP:
                case BMI2_ORIENTATION:
                case BMI2_HIGH_G:
                case BMI2_LOW_G:
                case BMI2_FLAT:
                case BMI2_ANY_MOTION:
                case BMI2_NO_MOTION:
                case BMI2_STEP_COUNTER:
                case BMI2_STEP_DETECTOR:
                case BMI2_STEP_ACTIVITY:

                    rslt = bmi2_map_feat_int(sens_int[loop].type, sens_int[loop].hw_int_pin, dev);
                    break;
                default:
                    rslt = BMI2_E_INVALID_SENSOR;
                    break;
            }

            /* Return error if interrupt mapping fails */
            if (rslt != BMI2_OK)
            {
                break;
            }
        }
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/***************************************************************************/

/*!         Local Function Definitions
 ****************************************************************************/

/*!
 * @brief This internal API is used to get the tap configurations.
 */
static int8_t get_tap_config(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt;

    rslt = get_config_page_primary(tap, bmi2_dev);

    if (rslt == BMI2_OK)
    {
        rslt = get_config_page_secondary(tap, bmi2_dev);
    }

    return rslt;
}

/*!
 * @brief This internal API is used to get the tap configurations
 * from primary page.
 */
static int8_t get_config_page_primary(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Structure to define BMI2 sensor configurations */
    struct bmi2_dev *dev = (struct bmi2_dev *)bmi2_dev;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint8_t lsb = 0;

    /* Variable to define MSB */
    uint8_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Structure to define tap configurations */
    bmi270_legacy_config = (struct bmi2_tap_config *)tap;

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TAP_DETECTOR_1, dev);

    if (feat_found == BMI2_TRUE)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for tap select */
            idx = tap_config.start_addr;

            /* Get word to calculate single tap, double tap, triple tap, data register
             * enable and output configuration
             */
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get filtered tap data */
            bmi270_legacy_config->data_reg_en = (lsb_msb & BMI2_TAP_DATA_REG_EN_MASK) >> BMI2_TAP_DATA_REG_EN_POS;

            /* Get word to calculate tap_sens_thres */
            idx = 4;
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get scaling factor of threshold */
            bmi270_legacy_config->tap_sens_thres = lsb_msb;

            /* Get word to calculate max_gest_dur */
            idx = 6;
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get maximum duration between each taps */
            bmi270_legacy_config->max_gest_dur = lsb_msb;

            /* Get word to calculate quite_time_after_gest */
            idx = 14;
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get minimum quite time between the two gesture detection */
            bmi270_legacy_config->quite_time_after_gest = lsb_msb;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to get the tap configurations
 * from secondary page.
 */
static int8_t get_config_page_secondary(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Structure to define BMI2 sensor configurations */
    struct bmi2_dev *dev = (struct bmi2_dev *)bmi2_dev;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint8_t lsb = 0;

    /* Variable to define MSB */
    uint8_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TAP_DETECTOR_2, dev);

    /* Structure to define tap configurations */
    bmi270_legacy_config = (struct bmi2_tap_config *)tap;

    if (feat_found == BMI2_TRUE)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for tap select */
            idx = tap_config.start_addr;

            /* Get word to calculate wait_for_timeout */
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Get number of taps detected based on wait time */
            bmi270_legacy_config->wait_for_timeout = lsb_msb;

            idx = 4;

            /* Get word to calculate axis_sel */
            lsb = feat_config[idx++];
            msb = feat_config[idx++];
            lsb_msb = ((uint16_t)msb << 8) | lsb;

            /* Select axis for tap detection */
            bmi270_legacy_config->axis_sel = (lsb_msb & BMI2_TAP_AXIS_SEL_MASK);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to set the tap configurations.
 */
static int8_t set_tap_config(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt;

    rslt = set_config_page_primary(tap, bmi2_dev);

    if (rslt == BMI2_OK)
    {
        rslt = set_config_page_secondary(tap, bmi2_dev);
    }

    return rslt;
}

/*!
 * @brief This internal API is used to set the tap configurations
 * to primary page.
 */
static int8_t set_config_page_primary(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Structure to define BMI2 sensor configurations */
    struct bmi2_dev *dev = (struct bmi2_dev *)bmi2_dev;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Structure to define tap configurations */
    bmi270_legacy_config = (struct bmi2_tap_config *)tap;

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TAP_DETECTOR_1, dev);

    if (feat_found == BMI2_TRUE)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for tap select */
            idx = tap_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set filtered tap data */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_TAP_DATA_REG_EN, bmi270_legacy_config->data_reg_en);

            /* Starting address of tap_sens_thres represented in word length */
            idx = 2;

            /* Set scaling factor of threshold */
            *(data_p + idx) = bmi270_legacy_config->tap_sens_thres;

            /* Starting address of max_gest_dur represented in word length */
            idx = 3;

            /* Set maximum duration between each taps */
            *(data_p + idx) = bmi270_legacy_config->max_gest_dur;

            /* Starting address of quite_time_after_gest represented in word length */
            idx = 7;

            /* Set minimum quite time between the two gesture detection */
            *(data_p + idx) = bmi270_legacy_config->quite_time_after_gest;

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - tap_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index <= idx; index++)
            {
                feat_config[tap_config.start_addr + index] = *((uint8_t *) data_p + tap_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to set the tap configurations
 * to secondary page.
 */
static int8_t set_config_page_secondary(void *tap, void *bmi2_dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Structure to define BMI2 sensor configurations */
    struct bmi2_dev *dev = (struct bmi2_dev *)bmi2_dev;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Structure to define tap configurations */
    bmi270_legacy_config = (struct bmi2_tap_config *)tap;

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TAP_DETECTOR_2, dev);

    if (feat_found == BMI2_TRUE)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for tap select */
            idx = tap_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set wait time */
            *(data_p + idx) = bmi270_legacy_config->wait_for_timeout;

            /* Starting address of axis_sel represented in word length */
            idx = 2;

            /* Select axis for tap detection */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_TAP_AXIS_SEL, bmi270_legacy_config->axis_sel);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - tap_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[tap_config.start_addr + index] = *((uint8_t *) data_p + tap_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to validate the device structure pointer for
 * null conditions.
 */
static int8_t null_ptr_check(const struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_us == NULL))
    {
        /* Device structure pointer is not valid */
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This internal API selects the sensor/features to be enabled or
 * disabled.
 */
static int8_t select_sensor(const uint8_t *sens_list, uint8_t n_sens, uint64_t *sensor_sel)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Variable to define loop */
    uint8_t count;

    for (count = 0; count < n_sens;)
    {
        if (sens_list[count] == BMI2_ACCEL)
        {
            *sensor_sel |= BMI2_ACCEL_SENS_SEL;
            count++;
        }

        if (sens_list[count] == BMI2_GYRO)
        {
            *sensor_sel |= BMI2_GYRO_SENS_SEL;
            count++;
        }

        if (sens_list[count] == BMI2_AUX)
        {
            *sensor_sel |= BMI2_AUX_SENS_SEL;
            count++;
        }

        if (sens_list[count] == BMI2_TEMP)
        {
            *sensor_sel |= BMI2_TEMP_SENS_SEL;
            count++;
        }

        if (count < n_sens)
        {
            rslt = sens_select(sens_list, count, sensor_sel);
            count++;
        }
    }

    return rslt;
}

/*!
 * @brief This internal API enables the selected sensor/features.
 */
static int8_t sensor_enable(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    rslt = enable_main_sensors(sensor_sel, dev);

    if ((rslt == BMI2_OK) && (sensor_sel & ~(BMI2_MAIN_SENSORS)))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;
        if (aps_stat == BMI2_ENABLE)
        {
            /* Disable advance power save if enabled */
            rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
        }

        if (rslt == BMI2_OK)
        {
            rslt = enable_sensor_features(sensor_sel, dev);

            /* Enable Advance power save if disabled while
             * configuring and not when already disabled
             */
            if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
            {
                rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
            }
        }
    }

    return rslt;
}

/*!
 * @brief This internal API disables the selected sensors/features.
 */
static int8_t sensor_disable(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to get the status of advance power save */
    uint8_t aps_stat = 0;

    rslt = disable_main_sensors(sensor_sel, dev);

    if ((rslt == BMI2_OK) && (sensor_sel & ~(BMI2_MAIN_SENSORS)))
    {
        /* Get status of advance power save mode */
        aps_stat = dev->aps_status;
        if (aps_stat == BMI2_ENABLE)
        {
            /* Disable advance power save if enabled */
            rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
        }

        if (rslt == BMI2_OK)
        {
            rslt = disable_sensor_features(sensor_sel, dev);

            /* Enable Advance power save if disabled while
            * configuring and not when already disabled */
            if ((aps_stat == BMI2_ENABLE) && (rslt == BMI2_OK))
            {
                rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
            }
        }
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable any motion feature.
 */
static int8_t set_any_motion(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for any-motion */
    struct bmi2_feature_config any_mot_config = { 0, 0, 0 };

    /* Search for any-motion feature and extract its configurations details */
    feat_found = bmi2_extract_input_feat_config(&any_mot_config, BMI2_ANY_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where any-motion feature resides */
        rslt = bmi2_get_feat_config(any_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of any-motion axes */
            idx = any_mot_config.start_addr + BMI2_ANY_MOT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_ANY_NO_MOT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_ANY_MOT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_ANY_MOT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable no-motion feature.
 */
static int8_t set_no_motion(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for no-motion */
    struct bmi2_feature_config no_mot_config = { 0, 0, 0 };

    /* Search for no-motion feature and extract its configurations details */
    feat_found = bmi2_extract_input_feat_config(&no_mot_config, BMI2_NO_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where any/no-motion feature resides */
        rslt = bmi2_get_feat_config(no_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of no-motion axes */
            idx = no_mot_config.start_addr + BMI2_NO_MOT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_ANY_NO_MOT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_NO_MOT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_NO_MOT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable step detector feature.
 */
static int8_t set_step_detector(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step detector */
    struct bmi2_feature_config step_det_config = { 0, 0, 0 };

    /* Search for step detector feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_det_config, BMI2_STEP_DETECTOR, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step detector feature resides */
        rslt = bmi2_get_feat_config(step_det_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of step detector */
            idx = step_det_config.start_addr + BMI2_STEP_COUNT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_STEP_DET_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_STEP_DETECT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_STEP_DETECT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable step counter feature.
 */
static int8_t set_step_counter(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step counter */
    struct bmi2_feature_config step_count_config = { 0, 0, 0 };

    /* Search for step counter feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_count_config, BMI2_STEP_COUNTER, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step-counter feature resides */
        rslt = bmi2_get_feat_config(step_count_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of step counter */
            idx = step_count_config.start_addr + BMI2_STEP_COUNT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_STEP_COUNT_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_STEP_COUNT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_STEP_COUNT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable step activity detection.
 */
static int8_t set_step_activity(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step activity */
    struct bmi2_feature_config step_act_config = { 0, 0, 0 };

    /* Search for step activity feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_act_config, BMI2_STEP_ACTIVITY, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step-activity
         * feature resides
         */
        rslt = bmi2_get_feat_config(step_act_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of step activity */
            idx = step_act_config.start_addr + BMI2_STEP_COUNT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_STEP_ACT_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_STEP_ACT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_STEP_ACT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable sig-motion feature.
 */
static int8_t set_sig_motion(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for sig-motion */
    struct bmi2_feature_config sig_mot_config = { 0, 0, 0 };

    /* Search for sig-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&sig_mot_config, BMI2_SIG_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where sig-motion feature resides */
        rslt = bmi2_get_feat_config(sig_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of sig-motion */
            idx = sig_mot_config.start_addr + BMI2_SIG_MOT_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BIT_POS0(feat_config[idx], BMI2_SIG_MOT_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_SIG_MOTION_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_SIG_MOTION_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * single tap.
 */
static int8_t set_single_tap(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_SINGLE_TAP, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of tap */
            idx = tap_config.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BIT_POS0(feat_config[idx], BMI2_TAP_SINGLE_TAP_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_SINGLE_TAP_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_SINGLE_TAP_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * double tap.
 */
static int8_t set_double_tap(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_DOUBLE_TAP, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of tap */
            idx = tap_config.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_TAP_DOUBLE_TAP_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_DOUBLE_TAP_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_DOUBLE_TAP_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable tap feature through
 * triple tap.
 */
static int8_t set_triple_tap(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for tap */
    struct bmi2_feature_config tap_config = { 0, 0, 0 };

    /* Search for tap feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&tap_config, BMI2_TRIPLE_TAP, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where tap feature resides */
        rslt = bmi2_get_feat_config(tap_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of tap */
            idx = tap_config.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_TAP_TRIPLE_TAP_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_TRIPLE_TAP_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_TRIPLE_TAP_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable orientation feature.
 */
static int8_t set_orientation(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for orientation */
    struct bmi2_feature_config orient_config = { 0, 0, 0 };

    /* Search for orientation feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&orient_config, BMI2_ORIENTATION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where orientation feature resides */
        rslt = bmi2_get_feat_config(orient_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of orientation */
            idx = orient_config.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BIT_POS0(feat_config[idx], BMI2_ORIENT_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_ORIENT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_ORIENT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable high-g feature.
 */
static int8_t set_high_g(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for high-g */
    struct bmi2_feature_config high_g_config = { 0, 0, 0 };

    /* Search for high-g feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&high_g_config, BMI2_HIGH_G, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where high-g feature resides */
        rslt = bmi2_get_feat_config(high_g_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of high-g */
            idx = high_g_config.start_addr + BMI2_HIGH_G_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_HIGH_G_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_HIGH_G_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_HIGH_G_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable low-g feature.
 */
static int8_t set_low_g(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for low-g */
    struct bmi2_feature_config low_g_config = { 0, 0, 0 };

    /* Search for low-g feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&low_g_config, BMI2_LOW_G, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where low-g feature resides */
        rslt = bmi2_get_feat_config(low_g_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of low-g */
            idx = low_g_config.start_addr + BMI2_LOW_G_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_LOW_G_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_LOW_G_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_LOW_G_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable flat feature.
 */
static int8_t set_flat(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for flat */
    struct bmi2_feature_config flat_config = { 0, 0, 0 };

    /* Search for flat feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&flat_config, BMI2_FLAT, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where flat feature resides */
        rslt = bmi2_get_feat_config(flat_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of flat */
            idx = flat_config.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BIT_POS0(feat_config[idx], BMI2_FLAT_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_FLAT_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_FLAT_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gives an option to enable self-offset correction
 * feature of gyroscope, either internally or by the host.
 */
static int8_t set_gyro_self_offset_corr(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for self-offset correction */
    struct bmi2_feature_config self_off_corr_cfg = { 0, 0, 0 };

    /* Search for self-offset correction and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&self_off_corr_cfg, BMI2_GYRO_SELF_OFF, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where self-offset
         * correction feature resides
         */
        rslt = bmi2_get_feat_config(self_off_corr_cfg.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of self-offset correction */
            idx = self_off_corr_cfg.start_addr;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_GYR_SELF_OFF_CORR_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_GYRO_SELF_OFF_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_GYRO_SELF_OFF_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable/disable gyroscope user gain
 * feature.
 */
static int8_t set_gyro_user_gain(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for gyroscope user gain */
    struct bmi2_feature_config gyr_user_gain_cfg = { 0, 0, 0 };

    /* Search for user gain feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&gyr_user_gain_cfg, BMI2_GYRO_GAIN_UPDATE, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where user gain feature resides */
        rslt = bmi2_get_feat_config(gyr_user_gain_cfg.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for enable/disable of user gain */
            idx = gyr_user_gain_cfg.start_addr + BMI2_GYR_USER_GAIN_FEAT_EN_OFFSET;

            /* Set the feature enable bit */
            feat_config[idx] = BMI2_SET_BITS(feat_config[idx], BMI2_GYR_USER_GAIN_FEAT_EN, enable);

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);

            if ((rslt == BMI2_OK) && (enable == BMI2_ENABLE))
            {
                dev->sens_en_stat |= BMI2_GYRO_GAIN_UPDATE_SEL;
            }
            else
            {
                dev->sens_en_stat &= ~BMI2_GYRO_GAIN_UPDATE_SEL;
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets any-motion configurations like axes select,
 * duration, threshold and output-configuration.
 */
static int8_t set_any_motion_config(const struct bmi2_any_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define count */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for any motion */
    struct bmi2_feature_config any_mot_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for any-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&any_mot_config, BMI2_ANY_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where any-motion feature resides */
        rslt = bmi2_get_feat_config(any_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for any-motion select */
            idx = any_mot_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set duration */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ANY_NO_MOT_DUR, config->duration);

            /* Set x-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_X_SEL, config->select_x);

            /* Set y-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_Y_SEL, config->select_y);

            /* Set z-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_Z_SEL, config->select_z);

            /* Increment offset by 1 word to set threshold and output configuration */
            idx++;

            /* Set threshold */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ANY_NO_MOT_THRES, config->threshold);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - any_mot_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[any_mot_config.start_addr +
                            index] = *((uint8_t *) data_p + any_mot_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets no-motion configurations like axes select,
 * duration, threshold and output-configuration.
 */
static int8_t set_no_motion_config(const struct bmi2_no_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define count */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for no-motion */
    struct bmi2_feature_config no_mot_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for no-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&no_mot_config, BMI2_NO_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where no-motion feature resides */
        rslt = bmi2_get_feat_config(no_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for no-motion select */
            idx = no_mot_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set duration */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ANY_NO_MOT_DUR, config->duration);

            /* Set x-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_X_SEL, config->select_x);

            /* Set y-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_Y_SEL, config->select_y);

            /* Set z-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ANY_NO_MOT_Z_SEL, config->select_z);

            /* Increment offset by 1 word to set threshold and output configuration */
            idx++;

            /* Set threshold */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ANY_NO_MOT_THRES, config->threshold);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - no_mot_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[no_mot_config.start_addr +
                            index] = *((uint8_t *) data_p + no_mot_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets sig-motion configurations like block-size,
 * output-configuration and other parameters.
 */
static int8_t set_sig_motion_config(const struct bmi2_sig_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for sig-motion */
    struct bmi2_feature_config sig_mot_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for sig-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&sig_mot_config, BMI2_SIG_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where sig-motion feature resides */
        rslt = bmi2_get_feat_config(sig_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for sig-motion select */
            idx = sig_mot_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set parameter 1 */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_SIG_MOT_PARAM_1, config->block_size);

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - sig_mot_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[sig_mot_config.start_addr +
                            index] = *((uint8_t *) data_p + sig_mot_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/* @brief This internal API sets step counter configurations like water-mark
 * level, reset-counter and output-configuration step detector and activity.
 */
static int8_t set_step_config(const struct bmi2_step_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step counter 4 */
    struct bmi2_feature_config step_count_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for step counter feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_count_config, BMI2_STEP_COUNTER, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step counter resides */
        rslt = bmi2_get_feat_config(step_count_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes */
            idx = step_count_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set water-mark level */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_STEP_COUNT_WM_LEVEL, config->watermark_level);

            /* Set reset-counter */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_STEP_COUNT_RST_CNT, config->reset_counter);

            /* Increment offset by 1 word  to set output
             * configuration of step detector and step activity
             */
            idx++;

            /* Set step buffer size */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_STEP_BUFFER_SIZE, config->step_buffer_size);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - step_count_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[step_count_config.start_addr +
                            index] = *((uint8_t *) data_p + step_count_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/* @brief This internal API sets tap configurations like sensitivity,
 * single, double and triple tap enable and output-configuration.
 */
static int8_t set_tap_detector_config(struct bmi2_tap_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    rslt = dev->set_tap_config(config, dev);

    return rslt;
}

/*!
 * @brief This internal API sets orientation configurations like upside/down
 * detection, symmetrical modes, blocking mode, theta, hysteresis and output
 * configuration.
 */
static int8_t set_orient_config(const struct bmi2_orient_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for orient */
    struct bmi2_feature_config orient_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for orient feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&orient_config, BMI2_ORIENTATION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where orient feature resides */
        rslt = bmi2_get_feat_config(orient_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for orient select */
            idx = orient_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set upside/down detection */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ORIENT_UP_DOWN, config->ud_en);

            /* Set symmetrical modes */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ORIENT_SYMM_MODE, config->mode);

            /* Set blocking mode */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ORIENT_BLOCK_MODE, config->blocking);

            /* Set theta */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_ORIENT_THETA, config->theta);

            /* Increment offset by 1 more word to set hysteresis and output configuration */
            idx++;

            /* Set hysteresis */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_ORIENT_HYST, config->hysteresis);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - orient_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[orient_config.start_addr +
                            index] = *((uint8_t *) data_p + orient_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets high-g configurations like threshold,
 * hysteresis, duration, and out0put configuration.
 */
static int8_t set_high_g_config(const struct bmi2_high_g_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for high-g */
    struct bmi2_feature_config high_g_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for high-g feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&high_g_config, BMI2_HIGH_G, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where high-g feature resides */
        rslt = bmi2_get_feat_config(high_g_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for high-g select */
            idx = high_g_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set threshold */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_HIGH_G_THRES, config->threshold);

            /* Increment offset by 1 more word to set hysteresis */
            idx++;

            /* Set hysteresis */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_HIGH_G_HYST, config->hysteresis);

            /* Set x-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_HIGH_G_X_SEL, config->select_x);

            /* Set y-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_HIGH_G_Y_SEL, config->select_y);

            /* Set z-select */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_HIGH_G_Z_SEL, config->select_z);

            /* Increment offset by 1 more word to set duration and output configuration */
            idx++;

            /* Set duration */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_HIGH_G_DUR, config->duration);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - high_g_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[high_g_config.start_addr +
                            index] = *((uint8_t *) data_p + high_g_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets low-g configurations like threshold,
 * hysteresis, duration, and output configuration.
 */
static int8_t set_low_g_config(const struct bmi2_low_g_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for low-g */
    struct bmi2_feature_config low_g_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for low-g feature and extract its configuration details  */
    feat_found = bmi2_extract_input_feat_config(&low_g_config, BMI2_LOW_G, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where low-g feature resides */
        rslt = bmi2_get_feat_config(low_g_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for low-g select */
            idx = low_g_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set threshold */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_LOW_G_THRES, config->threshold);

            /* Increment offset by 1 more word to set hysteresis */
            idx++;

            /* Set hysteresis */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_LOW_G_HYST, config->hysteresis);

            /* Increment offset by 1 more word to set duration and output configuration */
            idx++;

            /* Set duration */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_LOW_G_DUR, config->duration);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - low_g_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[low_g_config.start_addr + index] = *((uint8_t *) data_p + low_g_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets flat configurations like theta, blocking,
 * hold-time, hysteresis, and output configuration.
 */
static int8_t set_flat_config(const struct bmi2_flat_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define index */
    uint8_t index = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for flat */
    struct bmi2_feature_config flat_config = { 0, 0, 0 };

    /* Copy the feature configuration address to a local pointer */
    uint16_t *data_p = (uint16_t *) (void *)feat_config;

    /* Search for flat feature and extract its configuration details  */
    feat_found = bmi2_extract_input_feat_config(&flat_config, BMI2_FLAT, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where flat feature resides */
        rslt = bmi2_get_feat_config(flat_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for flat select */
            idx = flat_config.start_addr;

            /* Get offset in words since all the features are set in words length */
            idx = idx / 2;

            /* Set theta */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_FLAT_THETA, config->theta);

            /* Set blocking */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_FLAT_BLOCK, config->blocking);

            /* Increment offset by 1 more word to set hysteresis and hold-time */
            idx++;

            /* Set hysteresis */
            *(data_p + idx) = BMI2_SET_BIT_POS0(*(data_p + idx), BMI2_FLAT_HYST, config->hysteresis);

            /* Set hold-time */
            *(data_p + idx) = BMI2_SET_BITS(*(data_p + idx), BMI2_FLAT_HOLD_TIME, config->hold_time);

            /* Increment offset by 1 more word to get the total length in words */
            idx++;

            /* Get total length in bytes to copy from local pointer to the array */
            idx = (uint8_t)(idx * 2) - flat_config.start_addr;

            /* Copy the bytes to be set back to the array */
            for (index = 0; index < idx; index++)
            {
                feat_config[flat_config.start_addr + index] = *((uint8_t *) data_p + flat_config.start_addr + index);
            }

            /* Set the configuration back to the page */
            rslt = bmi2_set_regs(BMI2_FEATURES_REG_ADDR, feat_config, BMI2_FEAT_SIZE_IN_BYTES, dev);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets any-motion configurations like axes select,
 * duration, threshold and output-configuration.
 */
static int8_t get_any_motion_config(struct bmi2_any_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb;

    /* Variable to define MSB */
    uint16_t msb;

    /* Variable to define a word */
    uint16_t lsb_msb;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for any-motion */
    struct bmi2_feature_config any_mot_config = { 0, 0, 0 };

    /* Search for any-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&any_mot_config, BMI2_ANY_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where any-motion feature resides */
        rslt = bmi2_get_feat_config(any_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for feature enable for any-motion */
            idx = any_mot_config.start_addr;

            /* Get word to calculate duration, x, y and z select */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get duration */
            config->duration = lsb_msb & BMI2_ANY_NO_MOT_DUR_MASK;

            /* Get x-select */
            config->select_x = (lsb_msb & BMI2_ANY_NO_MOT_X_SEL_MASK) >> BMI2_ANY_NO_MOT_X_SEL_POS;

            /* Get y-select */
            config->select_y = (lsb_msb & BMI2_ANY_NO_MOT_Y_SEL_MASK) >> BMI2_ANY_NO_MOT_Y_SEL_POS;

            /* Get z-select */
            config->select_z = (lsb_msb & BMI2_ANY_NO_MOT_Z_SEL_MASK) >> BMI2_ANY_NO_MOT_Z_SEL_POS;

            /* Get word to calculate threshold, output configuration from the same word */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get threshold */
            config->threshold = lsb_msb & BMI2_ANY_NO_MOT_THRES_MASK;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets no-motion configurations like axes select,
 * duration, threshold and output-configuration.
 */
static int8_t get_no_motion_config(struct bmi2_no_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for no-motion */
    struct bmi2_feature_config no_mot_config = { 0, 0, 0 };

    /* Search for no-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&no_mot_config, BMI2_NO_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where no-motion feature resides */
        rslt = bmi2_get_feat_config(no_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for feature enable for no-motion */
            idx = no_mot_config.start_addr;

            /* Get word to calculate duration, x, y and z select */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get duration */
            config->duration = lsb_msb & BMI2_ANY_NO_MOT_DUR_MASK;

            /* Get x-select */
            config->select_x = (lsb_msb & BMI2_ANY_NO_MOT_X_SEL_MASK) >> BMI2_ANY_NO_MOT_X_SEL_POS;

            /* Get y-select */
            config->select_y = (lsb_msb & BMI2_ANY_NO_MOT_Y_SEL_MASK) >> BMI2_ANY_NO_MOT_Y_SEL_POS;

            /* Get z-select */
            config->select_z = (lsb_msb & BMI2_ANY_NO_MOT_Z_SEL_MASK) >> BMI2_ANY_NO_MOT_Z_SEL_POS;

            /* Get word to calculate threshold, output configuration from the same word */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get threshold */
            config->threshold = lsb_msb & BMI2_ANY_NO_MOT_THRES_MASK;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets sig-motion configurations like block-size,
 * output-configuration and other parameters.
 */
static int8_t get_sig_motion_config(struct bmi2_sig_motion_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration sig-motion */
    struct bmi2_feature_config sig_mot_config = { 0, 0, 0 };

    /* Search for sig-motion feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&sig_mot_config, BMI2_SIG_MOTION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where sig-motion feature resides */
        rslt = bmi2_get_feat_config(sig_mot_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for feature enable for sig-motion */
            idx = sig_mot_config.start_addr;

            /* Get word to calculate parameter 1 */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get parameter 1  */
            config->block_size = lsb_msb & BMI2_SIG_MOT_PARAM_1_MASK;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets step counter/detector/activity configurations.
 */
static int8_t get_step_config(struct bmi2_step_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for step counter */
    struct bmi2_feature_config step_count_config = { 0, 0, 0 };

    /* Search for step counter 4 feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&step_count_config, BMI2_STEP_COUNTER, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where step counter 4 parameter resides */
        rslt = bmi2_get_feat_config(step_count_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset for feature enable for step counter/detector/activity */
            idx = step_count_config.start_addr;

            /* Get word to calculate water-mark level and reset counter */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get water-mark level */
            config->watermark_level = lsb_msb & BMI2_STEP_COUNT_WM_LEVEL_MASK;

            /* Get reset counter */
            config->reset_counter = (lsb_msb & BMI2_STEP_COUNT_RST_CNT_MASK) >> BMI2_STEP_COUNT_RST_CNT_POS;

            /* Get word to calculate output configuration of step detector and activity */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            config->step_buffer_size = (lsb_msb & BMI2_STEP_BUFFER_SIZE_MASK) >> BMI2_STEP_BUFFER_SIZE_POS;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets tap configurations like sensitivity,
 * single, double and triple tap enable and output-configuration.
 */
static int8_t get_tap_detector_config(struct bmi2_tap_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    rslt = dev->get_tap_config(config, dev);

    return rslt;
}

/*!
 * @brief This internal API gets orientation configurations like upside/down
 * detection, symmetrical modes, blocking mode, theta, hysteresis and output
 * configuration.
 */
static int8_t get_orient_config(struct bmi2_orient_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for orient */
    struct bmi2_feature_config orient_config = { 0, 0, 0 };

    /* Search for orient feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&orient_config, BMI2_ORIENTATION, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where orient feature resides */
        rslt = bmi2_get_feat_config(orient_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for orient select */
            idx = orient_config.start_addr;

            /* Get word to calculate upside/down detection,
             * symmetrical modes, blocking mode and theta
             */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get upside/down detection */
            config->ud_en = (lsb_msb & BMI2_ORIENT_UP_DOWN_MASK) >> BMI2_ORIENT_UP_DOWN_POS;

            /* Get symmetrical modes */
            config->mode = (lsb_msb & BMI2_ORIENT_SYMM_MODE_MASK) >> BMI2_ORIENT_SYMM_MODE_POS;

            /* Get blocking mode */
            config->blocking = (lsb_msb & BMI2_ORIENT_BLOCK_MODE_MASK) >> BMI2_ORIENT_BLOCK_MODE_POS;

            /* Get theta */
            config->theta = (lsb_msb & BMI2_ORIENT_THETA_MASK) >> BMI2_ORIENT_THETA_POS;

            /* Get the next word to calculate hysteresis and output configuration */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get hysteresis */
            config->hysteresis = lsb_msb & BMI2_ORIENT_HYST_MASK;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets high-g configurations like threshold,
 * hysteresis, duration, and output configuration.
 */
static int8_t get_high_g_config(struct bmi2_high_g_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for high-g */
    struct bmi2_feature_config high_g_config = { 0, 0, 0 };

    /* Search for high-g feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&high_g_config, BMI2_HIGH_G, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where high-g feature resides */
        rslt = bmi2_get_feat_config(high_g_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for high-g select */
            idx = high_g_config.start_addr;

            /* Get word to calculate threshold */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get threshold */
            config->threshold = lsb_msb & BMI2_HIGH_G_THRES_MASK;

            /* Get word to calculate hysteresis */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get hysteresis */
            config->hysteresis = lsb_msb & BMI2_HIGH_G_HYST_MASK;

            /* Get x_select */
            config->select_x = (lsb_msb & BMI2_HIGH_G_X_SEL_MASK) >> BMI2_HIGH_G_X_SEL_POS;

            /* Get y_select */
            config->select_y = (lsb_msb & BMI2_HIGH_G_Y_SEL_MASK) >> BMI2_HIGH_G_Y_SEL_POS;

            /* Get z_select */
            config->select_z = (lsb_msb & BMI2_HIGH_G_Z_SEL_MASK) >> BMI2_HIGH_G_Z_SEL_POS;

            /* Get word to calculate duration and output configuration */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get duration */
            config->duration = lsb_msb & BMI2_HIGH_G_DUR_MASK;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets low-g configurations like threshold,
 * hysteresis, duration, and output configuration.
 */
static int8_t get_low_g_config(struct bmi2_low_g_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for low-g */
    struct bmi2_feature_config low_g_config = { 0, 0, 0 };

    /* Search for low-g feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&low_g_config, BMI2_LOW_G, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where low-g feature resides */
        rslt = bmi2_get_feat_config(low_g_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for low-g select */
            idx = low_g_config.start_addr;

            /* Get word to calculate threshold */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get threshold */
            config->threshold = lsb_msb & BMI2_LOW_G_THRES_MASK;

            /* Get word to calculate hysteresis */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get hysteresis */
            config->hysteresis = lsb_msb & BMI2_LOW_G_HYST_MASK;

            /* Get word to calculate duration and output configuration */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get duration */
            config->duration = lsb_msb & BMI2_LOW_G_DUR_MASK;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets flat configurations like theta, blocking,
 * hold-time, hysteresis, and output configuration.
 */
static int8_t get_flat_config(struct bmi2_flat_config *config, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to define LSB */
    uint16_t lsb = 0;

    /* Variable to define MSB */
    uint16_t msb = 0;

    /* Variable to define a word */
    uint16_t lsb_msb = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature configuration for flat */
    struct bmi2_feature_config flat_config = { 0, 0, 0 };

    /* Search for flat feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&flat_config, BMI2_FLAT, dev);
    if (feat_found)
    {
        /* Get the configuration from the page where flat feature resides */
        rslt = bmi2_get_feat_config(flat_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for flat select */
            idx = flat_config.start_addr;

            /* Get word to calculate theta, blocking mode and output configuration */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get theta */
            config->theta = (lsb_msb & BMI2_FLAT_THETA_MASK) >> BMI2_FLAT_THETA_POS;

            /* Get blocking mode */
            config->blocking = (lsb_msb & BMI2_FLAT_BLOCK_MASK) >> BMI2_FLAT_BLOCK_POS;

            /* Get word to calculate hysteresis and hold-time */
            lsb = (uint16_t) feat_config[idx++];
            msb = ((uint16_t) feat_config[idx++] << 8);
            lsb_msb = lsb | msb;

            /* Get hysteresis */
            config->hysteresis = lsb_msb & BMI2_FLAT_HYST_MASK;

            /* Get hold-time */
            config->hold_time = (lsb_msb & BMI2_FLAT_HOLD_TIME_MASK) >> BMI2_FLAT_HOLD_TIME_POS;
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets the output values of high-g.
 */
static int8_t get_high_g_output(uint8_t *high_g_out, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for high-g */
    struct bmi2_feature_config high_g_out_config = { 0, 0, 0 };

    /* Search for high-g output feature and extract its configuration details */
    feat_found = extract_output_feat_config(&high_g_out_config, BMI2_HIGH_G, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for high-g */
        rslt = bmi2_get_feat_config(high_g_out_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for high-g output */
            idx = high_g_out_config.start_addr;

            /* Get the high-g output byte */
            *high_g_out = feat_config[idx];
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets the output values of step counter.
 */
static int8_t get_step_counter_output(uint32_t *step_count, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for step counter */
    struct bmi2_feature_config step_cnt_out_config = { 0, 0, 0 };

    /* Search for step counter output feature and extract its configuration details */
    feat_found = extract_output_feat_config(&step_cnt_out_config, BMI2_STEP_COUNTER, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for step-counter */
        rslt = bmi2_get_feat_config(step_cnt_out_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for step counter output */
            idx = step_cnt_out_config.start_addr;

            /* Get the step counter output in 4 bytes */
            *step_count = (uint32_t) feat_config[idx++];
            *step_count |= ((uint32_t) feat_config[idx++] << 8);
            *step_count |= ((uint32_t) feat_config[idx++] << 16);
            *step_count |= ((uint32_t) feat_config[idx++] << 24);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets the error status related to NVM.
 */
static int8_t get_nvm_error_status(struct bmi2_nvm_err_status *nvm_err_stat, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for NVM error status */
    struct bmi2_feature_config nvm_err_cfg = { 0, 0, 0 };

    /* Search for NVM error status feature and extract its configuration details */
    feat_found = extract_output_feat_config(&nvm_err_cfg, BMI2_NVM_STATUS, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for NVM error status */
        rslt = bmi2_get_feat_config(nvm_err_cfg.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for NVM error status */
            idx = nvm_err_cfg.start_addr;

            /* Increment index to get the error status */
            idx++;

            /* Error when NVM load action fails */
            nvm_err_stat->load_error = BMI2_GET_BIT_POS0(feat_config[idx], BMI2_NVM_LOAD_ERR_STATUS);

            /* Error when NVM program action fails */
            nvm_err_stat->prog_error = BMI2_GET_BITS(feat_config[idx], BMI2_NVM_PROG_ERR_STATUS);

            /* Error when NVM erase action fails */
            nvm_err_stat->erase_error = BMI2_GET_BITS(feat_config[idx], BMI2_NVM_ERASE_ERR_STATUS);

            /* Error when NVM program limit is exceeded */
            nvm_err_stat->exceed_error = BMI2_GET_BITS(feat_config[idx], BMI2_NVM_END_EXCEED_STATUS);

            /* Error when NVM privilege mode is not acquired */
            nvm_err_stat->privil_error = BMI2_GET_BITS(feat_config[idx], BMI2_NVM_PRIV_ERR_STATUS);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to get enable status of gyroscope user gain
 * update.
 */
static int8_t get_user_gain_upd_status(uint8_t *status, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variable to define the array offset */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Variable to check APS status */
    uint8_t aps_stat = 0;

    /* Initialize feature configuration for gyroscope user gain */
    struct bmi2_feature_config gyr_user_gain_cfg = { 0, 0, 0 };

    /* Search for user gain feature and extract its configuration details */
    feat_found = bmi2_extract_input_feat_config(&gyr_user_gain_cfg, BMI2_GYRO_GAIN_UPDATE, dev);
    if (feat_found)
    {
        /* Disable advance power save */
        aps_stat = dev->aps_status;
        if (aps_stat == BMI2_ENABLE)
        {
            rslt = bmi2_set_adv_power_save(BMI2_DISABLE, dev);
        }

        if (rslt == BMI2_OK)
        {
            /* Get the configuration from the page where user gain feature resides */
            rslt = bmi2_get_feat_config(gyr_user_gain_cfg.page, feat_config, dev);
            if (rslt == BMI2_OK)
            {
                /* Define the offset for enable/disable of user gain */
                idx = gyr_user_gain_cfg.start_addr + BMI2_GYR_USER_GAIN_FEAT_EN_OFFSET;

                /* Set the feature enable status */
                *status = BMI2_GET_BITS(feat_config[idx], BMI2_GYR_USER_GAIN_FEAT_EN);
            }
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    /* Enable Advance power save if disabled while configuring and not when already disabled */
    if ((rslt == BMI2_OK) && (aps_stat == BMI2_ENABLE))
    {
        rslt = bmi2_set_adv_power_save(BMI2_ENABLE, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API gets the output values of step activity.
 */
static int8_t get_step_activity_output(uint8_t *step_act, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for step activity */
    struct bmi2_feature_config step_act_out_config = { 0, 0, 0 };

    /* Search for step activity output feature and extract its configuration details */
    feat_found = extract_output_feat_config(&step_act_out_config, BMI2_STEP_ACTIVITY, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for step-activity */
        rslt = bmi2_get_feat_config(step_act_out_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for step activity output */
            idx = step_act_out_config.start_addr;

            /* Get the step activity output */
            *step_act = feat_config[idx];
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API gets the output values of orientation: portrait-
 * landscape and face up-down.
 */
static int8_t get_orient_output(struct bmi2_orientation_output *orient_out, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for orientation */
    struct bmi2_feature_config orient_out_config = { 0, 0, 0 };

    /* Search for orientation output feature and extract its configuration details */
    feat_found = extract_output_feat_config(&orient_out_config, BMI2_ORIENTATION, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for orientation */
        rslt = bmi2_get_feat_config(orient_out_config.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for orientation output */
            idx = orient_out_config.start_addr;

            /* Get the output value of the orientation detection feature */
            orient_out->portrait_landscape = BMI2_GET_BIT_POS0(feat_config[idx], BMI2_ORIENT_DETECT);

            /* Get the output value of the orientation face up-down feature */
            orient_out->faceup_down = BMI2_GET_BITS(feat_config[idx], BMI2_ORIENT_FACE_UP_DWN);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API enables/disables compensation of the gain defined
 * in the GAIN register.
 */
static int8_t enable_gyro_gain(uint8_t enable, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to define register data */
    uint8_t reg_data = 0;

    rslt = bmi2_get_regs(BMI2_GYR_OFF_COMP_6_ADDR, &reg_data, 1, dev);
    if (rslt == BMI2_OK)
    {
        reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_GAIN_EN, enable);
        rslt = bmi2_set_regs(BMI2_GYR_OFF_COMP_6_ADDR, &reg_data, 1, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API is used to extract the output feature configuration
 * details from the look-up table.
 */
static uint8_t extract_output_feat_config(struct bmi2_feature_config *feat_output,
                                          uint8_t type,
                                          const struct bmi2_dev *dev)
{
    /* Variable to define loop */
    uint8_t loop = 0;

    /* Variable to set flag */
    uint8_t feat_found = BMI2_FALSE;

    /* Search for the output feature from the output configuration array */
    while (loop < dev->out_sens)
    {
        if (dev->feat_output[loop].type == type)
        {
            *feat_output = dev->feat_output[loop];
            feat_found = BMI2_TRUE;
            break;
        }

        loop++;
    }

    /* Return flag */
    return feat_found;
}

/*!
 * @brief This internal API gets the error status related to virtual frames.
 */
static int8_t get_vfrm_error_status(struct bmi2_vfrm_err_status *vfrm_err_stat, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Array to define the feature configuration */
    uint8_t feat_config[BMI2_FEAT_SIZE_IN_BYTES] = { 0 };

    /* Variables to define index */
    uint8_t idx = 0;

    /* Variable to set flag */
    uint8_t feat_found;

    /* Initialize feature output for VFRM error status */
    struct bmi2_feature_config vfrm_err_cfg = { 0, 0, 0 };

    /* Search for VFRM error status feature and extract its configuration details */
    feat_found = extract_output_feat_config(&vfrm_err_cfg, BMI2_VFRM_STATUS, dev);
    if (feat_found)
    {
        /* Get the feature output configuration for VFRM error status */
        rslt = bmi2_get_feat_config(vfrm_err_cfg.page, feat_config, dev);
        if (rslt == BMI2_OK)
        {
            /* Define the offset in bytes for VFRM error status */
            idx = vfrm_err_cfg.start_addr;

            /* Increment index to get the error status */
            idx++;

            /* Internal error while acquiring lock for FIFO */
            vfrm_err_stat->lock_error = BMI2_GET_BITS(feat_config[idx], BMI2_VFRM_LOCK_ERR_STATUS);

            /* Internal error while writing byte into FIFO */
            vfrm_err_stat->write_error = BMI2_GET_BITS(feat_config[idx], BMI2_VFRM_WRITE_ERR_STATUS);

            /* Internal error while writing into FIFO */
            vfrm_err_stat->fatal_error = BMI2_GET_BITS(feat_config[idx], BMI2_VFRM_FATAL_ERR_STATUS);
        }
    }
    else
    {
        rslt = BMI2_E_INVALID_SENSOR;
    }

    return rslt;
}

/*!
 * @brief This internal API sets feature configuration to the sensor.
 */
static int8_t set_feat_config(struct bmi2_sens_config *sens_cfg, uint8_t loop, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    switch (sens_cfg[loop].type)
    {
        /* Set any-motion configuration */
        case BMI2_ANY_MOTION:
            rslt = set_any_motion_config(&sens_cfg[loop].cfg.any_motion, dev);
            break;

        /* Set no-motion configuration */
        case BMI2_NO_MOTION:
            rslt = set_no_motion_config(&sens_cfg[loop].cfg.no_motion, dev);
            break;

        /* Set sig-motion configuration */
        case BMI2_SIG_MOTION:
            rslt = set_sig_motion_config(&sens_cfg[loop].cfg.sig_motion, dev);
            break;

        /* Set step counter/detector/activity configuration */
        case BMI2_STEP_DETECTOR:
        case BMI2_STEP_COUNTER:
        case BMI2_STEP_ACTIVITY:
            rslt = set_step_config(&sens_cfg[loop].cfg.step_counter, dev);
            break;

        /* Set tap configuration */
        case BMI2_TAP:
            rslt = set_tap_detector_config(&sens_cfg[loop].cfg.tap_conf, dev);
            break;

        /* Set orientation configuration */
        case BMI2_ORIENTATION:
            rslt = set_orient_config(&sens_cfg[loop].cfg.orientation, dev);
            break;

        /* Set high-g configuration */
        case BMI2_HIGH_G:
            rslt = set_high_g_config(&sens_cfg[loop].cfg.high_g, dev);
            break;

        /* Set low-g configuration */
        case BMI2_LOW_G:
            rslt = set_low_g_config(&sens_cfg[loop].cfg.low_g, dev);
            break;

        /* Set flat configuration */
        case BMI2_FLAT:
            rslt = set_flat_config(&sens_cfg[loop].cfg.flat, dev);
            break;

        default:
            rslt = BMI2_E_INVALID_SENSOR;
            break;
    }

    return rslt;
}

/*!
 * @brief This internal API gets feature configuration from the sensor.
 */
static int8_t get_feat_config(struct bmi2_sens_config *sens_cfg, uint8_t loop, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    switch (sens_cfg[loop].type)
    {
        /* Get sig-motion configuration */
        case BMI2_SIG_MOTION:
            rslt = get_sig_motion_config(&sens_cfg[loop].cfg.sig_motion, dev);
            break;

        /* Get any motion configuration */
        case BMI2_ANY_MOTION:
            rslt = get_any_motion_config(&sens_cfg[loop].cfg.any_motion, dev);
            break;

        /* Get no motion configuration */
        case BMI2_NO_MOTION:
            rslt = get_no_motion_config(&sens_cfg[loop].cfg.no_motion, dev);
            break;

        /* Get step counter/detector/activity configuration */
        case BMI2_STEP_DETECTOR:
        case BMI2_STEP_COUNTER:
        case BMI2_STEP_ACTIVITY:
            rslt = get_step_config(&sens_cfg[loop].cfg.step_counter, dev);
            break;

        /* Get tap configuration */
        case BMI2_TAP:
            rslt = get_tap_detector_config(&sens_cfg[loop].cfg.tap_conf, dev);
            break;

        /* Get orientation configuration */
        case BMI2_ORIENTATION:
            rslt = get_orient_config(&sens_cfg[loop].cfg.orientation, dev);
            break;

        /* Get high-g configuration */
        case BMI2_HIGH_G:
            rslt = get_high_g_config(&sens_cfg[loop].cfg.high_g, dev);
            break;

        /* Get low-g configuration */
        case BMI2_LOW_G:
            rslt = get_low_g_config(&sens_cfg[loop].cfg.low_g, dev);
            break;

        /* Get flat configuration */
        case BMI2_FLAT:
            rslt = get_flat_config(&sens_cfg[loop].cfg.flat, dev);
            break;

        default:
            rslt = BMI2_E_INVALID_SENSOR;
            break;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to select the sensor feature.
 */
static int8_t sens_select(const uint8_t *sens_list, uint8_t count, uint64_t *sensor_sel)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    switch (sens_list[count])
    {
        case BMI2_SIG_MOTION:
            *sensor_sel |= BMI2_SIG_MOTION_SEL;
            break;
        case BMI2_ANY_MOTION:
            *sensor_sel |= BMI2_ANY_MOT_SEL;
            break;
        case BMI2_NO_MOTION:
            *sensor_sel |= BMI2_NO_MOT_SEL;
            break;
        case BMI2_STEP_DETECTOR:
            *sensor_sel |= BMI2_STEP_DETECT_SEL;
            break;
        case BMI2_STEP_COUNTER:
            *sensor_sel |= BMI2_STEP_COUNT_SEL;
            break;
        case BMI2_STEP_ACTIVITY:
            *sensor_sel |= BMI2_STEP_ACT_SEL;
            break;
        case BMI2_GYRO_GAIN_UPDATE:
            *sensor_sel |= BMI2_GYRO_GAIN_UPDATE_SEL;
            break;
        case BMI2_SINGLE_TAP:
            *sensor_sel |= BMI2_SINGLE_TAP_SEL;
            break;
        case BMI2_DOUBLE_TAP:
            *sensor_sel |= BMI2_DOUBLE_TAP_SEL;
            break;
        case BMI2_TRIPLE_TAP:
            *sensor_sel |= BMI2_TRIPLE_TAP_SEL;
            break;
        case BMI2_ORIENTATION:
            *sensor_sel |= BMI2_ORIENT_SEL;
            break;
        case BMI2_HIGH_G:
            *sensor_sel |= BMI2_HIGH_G_SEL;
            break;
        case BMI2_LOW_G:
            *sensor_sel |= BMI2_LOW_G_SEL;
            break;
        case BMI2_FLAT:
            *sensor_sel |= BMI2_FLAT_SEL;
            break;
        case BMI2_GYRO_SELF_OFF:
            *sensor_sel |= BMI2_GYRO_SELF_OFF_SEL;
            break;
        default:
            rslt = BMI2_E_INVALID_SENSOR;
            break;
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable main sensors like accel, gyro, aux and temperature.
 */
static int8_t enable_main_sensors(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to store register values */
    uint8_t reg_data;

    rslt = bmi2_get_regs(BMI2_PWR_CTRL_ADDR, &reg_data, 1, dev);

    if (rslt == BMI2_OK)
    {
        /* Enable accelerometer */
        if (sensor_sel & BMI2_ACCEL_SENS_SEL)
        {
            reg_data = BMI2_SET_BITS(reg_data, BMI2_ACC_EN, BMI2_ENABLE);
        }

        /* Enable gyroscope */
        if (sensor_sel & BMI2_GYRO_SENS_SEL)
        {
            reg_data = BMI2_SET_BITS(reg_data, BMI2_GYR_EN, BMI2_ENABLE);
        }

        /* Enable auxiliary sensor */
        if (sensor_sel & BMI2_AUX_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_POS0(reg_data, BMI2_AUX_EN, BMI2_ENABLE);
        }

        /* Enable temperature sensor */
        if (sensor_sel & BMI2_TEMP_SENS_SEL)
        {
            reg_data = BMI2_SET_BITS(reg_data, BMI2_TEMP_EN, BMI2_ENABLE);
        }

        /* Enable the sensors that are set in the power control register */
        if (sensor_sel & BMI2_MAIN_SENSORS)
        {
            rslt = bmi2_set_regs(BMI2_PWR_CTRL_ADDR, &reg_data, 1, dev);
        }
    }

    return rslt;
}

/*!
 * @brief This internal API is used to enable sensor features.
 */
static int8_t enable_sensor_features(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Enable sig-motion feature */
    if (sensor_sel & BMI2_SIG_MOTION_SEL)
    {
        rslt = set_sig_motion(BMI2_ENABLE, dev);
    }

    /* Enable any motion feature */
    if (sensor_sel & BMI2_ANY_MOT_SEL)
    {
        rslt = set_any_motion(BMI2_ENABLE, dev);
    }

    /* Enable no motion feature */
    if (sensor_sel & BMI2_NO_MOT_SEL)
    {
        rslt = set_no_motion(BMI2_ENABLE, dev);
    }

    /* Enable step detector feature */
    if (sensor_sel & BMI2_STEP_DETECT_SEL)
    {
        rslt = set_step_detector(BMI2_ENABLE, dev);
    }

    /* Enable step counter feature */
    if (sensor_sel & BMI2_STEP_COUNT_SEL)
    {
        rslt = set_step_counter(BMI2_ENABLE, dev);
    }

    /* Enable step activity feature */
    if (sensor_sel & BMI2_STEP_ACT_SEL)
    {
        rslt = set_step_activity(BMI2_ENABLE, dev);
    }

    /* Enable gyroscope user gain */
    if (sensor_sel & BMI2_GYRO_GAIN_UPDATE_SEL)
    {
        rslt = set_gyro_user_gain(BMI2_ENABLE, dev);
    }

    /* Enable single tap feature */
    if (sensor_sel & BMI2_SINGLE_TAP_SEL)
    {
        rslt = set_single_tap(BMI2_ENABLE, dev);
    }

    /* Enable double tap feature */
    if (sensor_sel & BMI2_DOUBLE_TAP_SEL)
    {
        rslt = set_double_tap(BMI2_ENABLE, dev);
    }

    /* Enable triple tap feature */
    if (sensor_sel & BMI2_TRIPLE_TAP_SEL)
    {
        rslt = set_triple_tap(BMI2_ENABLE, dev);
    }

    /* Enable orientation feature */
    if (sensor_sel & BMI2_ORIENT_SEL)
    {
        rslt = set_orientation(BMI2_ENABLE, dev);
    }

    /* Enable high-g feature */
    if (sensor_sel & BMI2_HIGH_G_SEL)
    {
        rslt = set_high_g(BMI2_ENABLE, dev);
    }

    /* Enable low-g feature */
    if (sensor_sel & BMI2_LOW_G_SEL)
    {
        rslt = set_low_g(BMI2_ENABLE, dev);
    }

    /* Enable flat feature */
    if (sensor_sel & BMI2_FLAT_SEL)
    {
        rslt = set_flat(BMI2_ENABLE, dev);
    }

    /* Enable gyroscope self-offset correction feature */
    if (sensor_sel & BMI2_GYRO_SELF_OFF_SEL)
    {
        rslt = set_gyro_self_offset_corr(BMI2_ENABLE, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API is used to disable main sensors like accel, gyro, aux and temperature.
 */
static int8_t disable_main_sensors(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt;

    /* Variable to store register values */
    uint8_t reg_data;

    rslt = bmi2_get_regs(BMI2_PWR_CTRL_ADDR, &reg_data, 1, dev);

    if (rslt == BMI2_OK)
    {
        /* Disable accelerometer */
        if (sensor_sel & BMI2_ACCEL_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_ACC_EN);
        }

        /* Disable gyroscope */
        if (sensor_sel & BMI2_GYRO_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_GYR_EN);
        }

        /* Disable auxiliary sensor */
        if (sensor_sel & BMI2_AUX_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_AUX_EN);
        }

        /* Disable temperature sensor */
        if (sensor_sel & BMI2_TEMP_SENS_SEL)
        {
            reg_data = BMI2_SET_BIT_VAL0(reg_data, BMI2_TEMP_EN);
        }

        /* Disable the sensors that are set in the power control register */
        if (sensor_sel & BMI2_MAIN_SENSORS)
        {
            rslt = bmi2_set_regs(BMI2_PWR_CTRL_ADDR, &reg_data, 1, dev);
        }
    }

    return rslt;
}

/*!
 * @brief This internal API is used to disable sensor features.
 */
static int8_t disable_sensor_features(uint64_t sensor_sel, struct bmi2_dev *dev)
{
    /* Variable to define error */
    int8_t rslt = BMI2_OK;

    /* Disable sig-motion feature */
    if (sensor_sel & BMI2_SIG_MOTION_SEL)
    {
        rslt = set_sig_motion(BMI2_DISABLE, dev);
    }

    /* Disable any-motion feature */
    if (sensor_sel & BMI2_ANY_MOT_SEL)
    {
        rslt = set_any_motion(BMI2_DISABLE, dev);
    }

    /* Disable no-motion feature */
    if (sensor_sel & BMI2_NO_MOT_SEL)
    {
        rslt = set_no_motion(BMI2_DISABLE, dev);
    }

    /* Disable step detector feature */
    if (sensor_sel & BMI2_STEP_DETECT_SEL)
    {
        rslt = set_step_detector(BMI2_DISABLE, dev);
    }

    /* Disable step counter feature */
    if (sensor_sel & BMI2_STEP_COUNT_SEL)
    {
        rslt = set_step_counter(BMI2_DISABLE, dev);
    }

    /* Disable step activity feature */
    if (sensor_sel & BMI2_STEP_ACT_SEL)
    {
        rslt = set_step_activity(BMI2_DISABLE, dev);
    }

    /* Disable gyroscope user gain */
    if (sensor_sel & BMI2_GYRO_GAIN_UPDATE_SEL)
    {
        rslt = set_gyro_user_gain(BMI2_DISABLE, dev);
    }

    /* Disable single tap feature */
    if (sensor_sel & BMI2_SINGLE_TAP_SEL)
    {
        rslt = set_single_tap(BMI2_DISABLE, dev);
    }

    /* Disable double tap feature */
    if (sensor_sel & BMI2_DOUBLE_TAP_SEL)
    {
        rslt = set_double_tap(BMI2_DISABLE, dev);
    }

    /* Disable triple tap feature */
    if (sensor_sel & BMI2_TRIPLE_TAP_SEL)
    {
        rslt = set_triple_tap(BMI2_DISABLE, dev);
    }

    /* Disable orientation feature */
    if (sensor_sel & BMI2_ORIENT_SEL)
    {
        rslt = set_orientation(BMI2_DISABLE, dev);
    }

    /* Disable high-g feature */
    if (sensor_sel & BMI2_HIGH_G_SEL)
    {
        rslt = set_high_g(BMI2_DISABLE, dev);
    }

    /* Disable low-g feature */
    if (sensor_sel & BMI2_LOW_G_SEL)
    {
        rslt = set_low_g(BMI2_DISABLE, dev);
    }

    /* Disable flat feature */
    if (sensor_sel & BMI2_FLAT_SEL)
    {
        rslt = set_flat(BMI2_DISABLE, dev);
    }

    /* Disable gyroscope self-offset correction feature */
    if (sensor_sel & BMI2_GYRO_SELF_OFF_SEL)
    {
        rslt = set_gyro_self_offset_corr(BMI2_DISABLE, dev);
    }

    return rslt;
}
