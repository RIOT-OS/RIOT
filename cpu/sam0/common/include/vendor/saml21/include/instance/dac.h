/**
 * \file
 *
 * \brief Instance description for DAC
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAML21_DAC_INSTANCE_
#define _SAML21_DAC_INSTANCE_

/* ========== Register definition for DAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DAC_CTRLA              (0x42003000) /**< \brief (DAC) Control A */
#define REG_DAC_CTRLB              (0x42003001) /**< \brief (DAC) Control B */
#define REG_DAC_EVCTRL             (0x42003002) /**< \brief (DAC) Event Control */
#define REG_DAC_INTENCLR           (0x42003004) /**< \brief (DAC) Interrupt Enable Clear */
#define REG_DAC_INTENSET           (0x42003005) /**< \brief (DAC) Interrupt Enable Set */
#define REG_DAC_INTFLAG            (0x42003006) /**< \brief (DAC) Interrupt Flag Status and Clear */
#define REG_DAC_STATUS             (0x42003007) /**< \brief (DAC) Status */
#define REG_DAC_SYNCBUSY           (0x42003008) /**< \brief (DAC) Synchronization Busy */
#define REG_DAC_DACCTRL0           (0x4200300C) /**< \brief (DAC) DACx Control 0 */
#define REG_DAC_DACCTRL1           (0x4200300E) /**< \brief (DAC) DACx Control 1 */
#define REG_DAC_DATA0              (0x42003010) /**< \brief (DAC) Data DAC0 0 */
#define REG_DAC_DATA1              (0x42003012) /**< \brief (DAC) Data DAC0 1 */
#define REG_DAC_DATABUF0           (0x42003014) /**< \brief (DAC) Data Buffer DAC0 0 */
#define REG_DAC_DATABUF1           (0x42003016) /**< \brief (DAC) Data Buffer DAC0 1 */
#define REG_DAC_DBGCTRL            (0x42003018) /**< \brief (DAC) Debug Control */
#else
#define REG_DAC_CTRLA              (*(RwReg8 *)0x42003000UL) /**< \brief (DAC) Control A */
#define REG_DAC_CTRLB              (*(RwReg8 *)0x42003001UL) /**< \brief (DAC) Control B */
#define REG_DAC_EVCTRL             (*(RwReg8 *)0x42003002UL) /**< \brief (DAC) Event Control */
#define REG_DAC_INTENCLR           (*(RwReg8 *)0x42003004UL) /**< \brief (DAC) Interrupt Enable Clear */
#define REG_DAC_INTENSET           (*(RwReg8 *)0x42003005UL) /**< \brief (DAC) Interrupt Enable Set */
#define REG_DAC_INTFLAG            (*(RwReg8 *)0x42003006UL) /**< \brief (DAC) Interrupt Flag Status and Clear */
#define REG_DAC_STATUS             (*(RoReg8 *)0x42003007UL) /**< \brief (DAC) Status */
#define REG_DAC_SYNCBUSY           (*(RoReg  *)0x42003008UL) /**< \brief (DAC) Synchronization Busy */
#define REG_DAC_DACCTRL0           (*(RwReg16*)0x4200300CUL) /**< \brief (DAC) DACx Control 0 */
#define REG_DAC_DACCTRL1           (*(RwReg16*)0x4200300EUL) /**< \brief (DAC) DACx Control 1 */
#define REG_DAC_DATA0              (*(WoReg16*)0x42003010UL) /**< \brief (DAC) Data DAC0 0 */
#define REG_DAC_DATA1              (*(WoReg16*)0x42003012UL) /**< \brief (DAC) Data DAC0 1 */
#define REG_DAC_DATABUF0           (*(WoReg16*)0x42003014UL) /**< \brief (DAC) Data Buffer DAC0 0 */
#define REG_DAC_DATABUF1           (*(WoReg16*)0x42003016UL) /**< \brief (DAC) Data Buffer DAC0 1 */
#define REG_DAC_DBGCTRL            (*(RwReg8 *)0x42003018UL) /**< \brief (DAC) Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DAC peripheral ========== */
#define DAC_DAC_NUM                 2        // Number of DACs
#define DAC_DATA_SIZE               12       // Number of bits in data
#define DAC_DMAC_ID_EMPTY_0         38
#define DAC_DMAC_ID_EMPTY_1         39
#define DAC_DMAC_ID_EMPTY_LSB       38
#define DAC_DMAC_ID_EMPTY_MSB       39
#define DAC_DMAC_ID_EMPTY_SIZE      2
#define DAC_GCLK_ID                 32       // Index of Generic Clock

#endif /* _SAML21_DAC_INSTANCE_ */
