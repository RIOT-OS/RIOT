/**
 * \file
 *
 * \brief Instance description for TC4
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

#ifndef _SAML21_TC4_INSTANCE_
#define _SAML21_TC4_INSTANCE_

/* ========== Register definition for TC4 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC4_CTRLA              (0x43000800) /**< \brief (TC4) Control A */
#define REG_TC4_CTRLBCLR           (0x43000804) /**< \brief (TC4) Control B Clear */
#define REG_TC4_CTRLBSET           (0x43000805) /**< \brief (TC4) Control B Set */
#define REG_TC4_EVCTRL             (0x43000806) /**< \brief (TC4) Event Control */
#define REG_TC4_INTENCLR           (0x43000808) /**< \brief (TC4) Interrupt Enable Clear */
#define REG_TC4_INTENSET           (0x43000809) /**< \brief (TC4) Interrupt Enable Set */
#define REG_TC4_INTFLAG            (0x4300080A) /**< \brief (TC4) Interrupt Flag Status and Clear */
#define REG_TC4_STATUS             (0x4300080B) /**< \brief (TC4) Status */
#define REG_TC4_WAVE               (0x4300080C) /**< \brief (TC4) Waveform Generation Control */
#define REG_TC4_DRVCTRL            (0x4300080D) /**< \brief (TC4) Control C */
#define REG_TC4_DBGCTRL            (0x4300080F) /**< \brief (TC4) Debug Control */
#define REG_TC4_SYNCBUSY           (0x43000810) /**< \brief (TC4) Synchronization Status */
#define REG_TC4_COUNT16_COUNT      (0x43000814) /**< \brief (TC4) COUNT16 Count */
#define REG_TC4_COUNT16_CC0        (0x4300081C) /**< \brief (TC4) COUNT16 Compare and Capture 0 */
#define REG_TC4_COUNT16_CC1        (0x4300081E) /**< \brief (TC4) COUNT16 Compare and Capture 1 */
#define REG_TC4_COUNT16_CCBUF0     (0x43000830) /**< \brief (TC4) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT16_CCBUF1     (0x43000832) /**< \brief (TC4) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC4_COUNT32_COUNT      (0x43000814) /**< \brief (TC4) COUNT32 Count */
#define REG_TC4_COUNT32_CC0        (0x4300081C) /**< \brief (TC4) COUNT32 Compare and Capture 0 */
#define REG_TC4_COUNT32_CC1        (0x43000820) /**< \brief (TC4) COUNT32 Compare and Capture 1 */
#define REG_TC4_COUNT32_CCBUF0     (0x43000830) /**< \brief (TC4) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT32_CCBUF1     (0x43000834) /**< \brief (TC4) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC4_COUNT8_COUNT       (0x43000814) /**< \brief (TC4) COUNT8 Count */
#define REG_TC4_COUNT8_PER         (0x4300081B) /**< \brief (TC4) COUNT8 Period */
#define REG_TC4_COUNT8_CC0         (0x4300081C) /**< \brief (TC4) COUNT8 Compare and Capture 0 */
#define REG_TC4_COUNT8_CC1         (0x4300081D) /**< \brief (TC4) COUNT8 Compare and Capture 1 */
#define REG_TC4_COUNT8_PERBUF      (0x4300082F) /**< \brief (TC4) COUNT8 Period Buffer */
#define REG_TC4_COUNT8_CCBUF0      (0x43000830) /**< \brief (TC4) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT8_CCBUF1      (0x43000831) /**< \brief (TC4) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC4_CTRLA              (*(RwReg  *)0x43000800UL) /**< \brief (TC4) Control A */
#define REG_TC4_CTRLBCLR           (*(RwReg8 *)0x43000804UL) /**< \brief (TC4) Control B Clear */
#define REG_TC4_CTRLBSET           (*(RwReg8 *)0x43000805UL) /**< \brief (TC4) Control B Set */
#define REG_TC4_EVCTRL             (*(RwReg16*)0x43000806UL) /**< \brief (TC4) Event Control */
#define REG_TC4_INTENCLR           (*(RwReg8 *)0x43000808UL) /**< \brief (TC4) Interrupt Enable Clear */
#define REG_TC4_INTENSET           (*(RwReg8 *)0x43000809UL) /**< \brief (TC4) Interrupt Enable Set */
#define REG_TC4_INTFLAG            (*(RwReg8 *)0x4300080AUL) /**< \brief (TC4) Interrupt Flag Status and Clear */
#define REG_TC4_STATUS             (*(RwReg8 *)0x4300080BUL) /**< \brief (TC4) Status */
#define REG_TC4_WAVE               (*(RwReg8 *)0x4300080CUL) /**< \brief (TC4) Waveform Generation Control */
#define REG_TC4_DRVCTRL            (*(RwReg8 *)0x4300080DUL) /**< \brief (TC4) Control C */
#define REG_TC4_DBGCTRL            (*(RwReg8 *)0x4300080FUL) /**< \brief (TC4) Debug Control */
#define REG_TC4_SYNCBUSY           (*(RoReg  *)0x43000810UL) /**< \brief (TC4) Synchronization Status */
#define REG_TC4_COUNT16_COUNT      (*(RwReg16*)0x43000814UL) /**< \brief (TC4) COUNT16 Count */
#define REG_TC4_COUNT16_CC0        (*(RwReg16*)0x4300081CUL) /**< \brief (TC4) COUNT16 Compare and Capture 0 */
#define REG_TC4_COUNT16_CC1        (*(RwReg16*)0x4300081EUL) /**< \brief (TC4) COUNT16 Compare and Capture 1 */
#define REG_TC4_COUNT16_CCBUF0     (*(RwReg16*)0x43000830UL) /**< \brief (TC4) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT16_CCBUF1     (*(RwReg16*)0x43000832UL) /**< \brief (TC4) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC4_COUNT32_COUNT      (*(RwReg  *)0x43000814UL) /**< \brief (TC4) COUNT32 Count */
#define REG_TC4_COUNT32_CC0        (*(RwReg  *)0x4300081CUL) /**< \brief (TC4) COUNT32 Compare and Capture 0 */
#define REG_TC4_COUNT32_CC1        (*(RwReg  *)0x43000820UL) /**< \brief (TC4) COUNT32 Compare and Capture 1 */
#define REG_TC4_COUNT32_CCBUF0     (*(RwReg  *)0x43000830UL) /**< \brief (TC4) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT32_CCBUF1     (*(RwReg  *)0x43000834UL) /**< \brief (TC4) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC4_COUNT8_COUNT       (*(RwReg8 *)0x43000814UL) /**< \brief (TC4) COUNT8 Count */
#define REG_TC4_COUNT8_PER         (*(RwReg8 *)0x4300081BUL) /**< \brief (TC4) COUNT8 Period */
#define REG_TC4_COUNT8_CC0         (*(RwReg8 *)0x4300081CUL) /**< \brief (TC4) COUNT8 Compare and Capture 0 */
#define REG_TC4_COUNT8_CC1         (*(RwReg8 *)0x4300081DUL) /**< \brief (TC4) COUNT8 Compare and Capture 1 */
#define REG_TC4_COUNT8_PERBUF      (*(RwReg8 *)0x4300082FUL) /**< \brief (TC4) COUNT8 Period Buffer */
#define REG_TC4_COUNT8_CCBUF0      (*(RwReg8 *)0x43000830UL) /**< \brief (TC4) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT8_CCBUF1      (*(RwReg8 *)0x43000831UL) /**< \brief (TC4) COUNT8 Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC4 peripheral ========== */
#define TC4_CC_NUM                  2       
#define TC4_DMAC_ID_MC_0            35
#define TC4_DMAC_ID_MC_1            36
#define TC4_DMAC_ID_MC_LSB          35
#define TC4_DMAC_ID_MC_MSB          36
#define TC4_DMAC_ID_MC_SIZE         2
#define TC4_DMAC_ID_OVF             34       // Indexes of DMA Overflow trigger
#define TC4_EXT                     0       
#define TC4_GCLK_ID                 29      
#define TC4_MASTER                  0       
#define TC4_OW_NUM                  2       

#endif /* _SAML21_TC4_INSTANCE_ */
