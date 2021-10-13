/**
 * \file
 *
 * \brief Instance description for TCC2
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

#ifndef _SAML21_TCC2_INSTANCE_
#define _SAML21_TCC2_INSTANCE_

/* ========== Register definition for TCC2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC2_CTRLA             (0x42001C00) /**< \brief (TCC2) Control A */
#define REG_TCC2_CTRLBCLR          (0x42001C04) /**< \brief (TCC2) Control B Clear */
#define REG_TCC2_CTRLBSET          (0x42001C05) /**< \brief (TCC2) Control B Set */
#define REG_TCC2_SYNCBUSY          (0x42001C08) /**< \brief (TCC2) Synchronization Busy */
#define REG_TCC2_FCTRLA            (0x42001C0C) /**< \brief (TCC2) Recoverable Fault A Configuration */
#define REG_TCC2_FCTRLB            (0x42001C10) /**< \brief (TCC2) Recoverable Fault B Configuration */
#define REG_TCC2_DRVCTRL           (0x42001C18) /**< \brief (TCC2) Driver Control */
#define REG_TCC2_DBGCTRL           (0x42001C1E) /**< \brief (TCC2) Debug Control */
#define REG_TCC2_EVCTRL            (0x42001C20) /**< \brief (TCC2) Event Control */
#define REG_TCC2_INTENCLR          (0x42001C24) /**< \brief (TCC2) Interrupt Enable Clear */
#define REG_TCC2_INTENSET          (0x42001C28) /**< \brief (TCC2) Interrupt Enable Set */
#define REG_TCC2_INTFLAG           (0x42001C2C) /**< \brief (TCC2) Interrupt Flag Status and Clear */
#define REG_TCC2_STATUS            (0x42001C30) /**< \brief (TCC2) Status */
#define REG_TCC2_COUNT             (0x42001C34) /**< \brief (TCC2) Count */
#define REG_TCC2_WAVE              (0x42001C3C) /**< \brief (TCC2) Waveform Control */
#define REG_TCC2_PER               (0x42001C40) /**< \brief (TCC2) Period */
#define REG_TCC2_CC0               (0x42001C44) /**< \brief (TCC2) Compare and Capture 0 */
#define REG_TCC2_CC1               (0x42001C48) /**< \brief (TCC2) Compare and Capture 1 */
#define REG_TCC2_PERBUF            (0x42001C6C) /**< \brief (TCC2) Period Buffer */
#define REG_TCC2_CCBUF0            (0x42001C70) /**< \brief (TCC2) Compare and Capture Buffer 0 */
#define REG_TCC2_CCBUF1            (0x42001C74) /**< \brief (TCC2) Compare and Capture Buffer 1 */
#else
#define REG_TCC2_CTRLA             (*(RwReg  *)0x42001C00UL) /**< \brief (TCC2) Control A */
#define REG_TCC2_CTRLBCLR          (*(RwReg8 *)0x42001C04UL) /**< \brief (TCC2) Control B Clear */
#define REG_TCC2_CTRLBSET          (*(RwReg8 *)0x42001C05UL) /**< \brief (TCC2) Control B Set */
#define REG_TCC2_SYNCBUSY          (*(RoReg  *)0x42001C08UL) /**< \brief (TCC2) Synchronization Busy */
#define REG_TCC2_FCTRLA            (*(RwReg  *)0x42001C0CUL) /**< \brief (TCC2) Recoverable Fault A Configuration */
#define REG_TCC2_FCTRLB            (*(RwReg  *)0x42001C10UL) /**< \brief (TCC2) Recoverable Fault B Configuration */
#define REG_TCC2_DRVCTRL           (*(RwReg  *)0x42001C18UL) /**< \brief (TCC2) Driver Control */
#define REG_TCC2_DBGCTRL           (*(RwReg8 *)0x42001C1EUL) /**< \brief (TCC2) Debug Control */
#define REG_TCC2_EVCTRL            (*(RwReg  *)0x42001C20UL) /**< \brief (TCC2) Event Control */
#define REG_TCC2_INTENCLR          (*(RwReg  *)0x42001C24UL) /**< \brief (TCC2) Interrupt Enable Clear */
#define REG_TCC2_INTENSET          (*(RwReg  *)0x42001C28UL) /**< \brief (TCC2) Interrupt Enable Set */
#define REG_TCC2_INTFLAG           (*(RwReg  *)0x42001C2CUL) /**< \brief (TCC2) Interrupt Flag Status and Clear */
#define REG_TCC2_STATUS            (*(RwReg  *)0x42001C30UL) /**< \brief (TCC2) Status */
#define REG_TCC2_COUNT             (*(RwReg  *)0x42001C34UL) /**< \brief (TCC2) Count */
#define REG_TCC2_WAVE              (*(RwReg  *)0x42001C3CUL) /**< \brief (TCC2) Waveform Control */
#define REG_TCC2_PER               (*(RwReg  *)0x42001C40UL) /**< \brief (TCC2) Period */
#define REG_TCC2_CC0               (*(RwReg  *)0x42001C44UL) /**< \brief (TCC2) Compare and Capture 0 */
#define REG_TCC2_CC1               (*(RwReg  *)0x42001C48UL) /**< \brief (TCC2) Compare and Capture 1 */
#define REG_TCC2_PERBUF            (*(RwReg  *)0x42001C6CUL) /**< \brief (TCC2) Period Buffer */
#define REG_TCC2_CCBUF0            (*(RwReg  *)0x42001C70UL) /**< \brief (TCC2) Compare and Capture Buffer 0 */
#define REG_TCC2_CCBUF1            (*(RwReg  *)0x42001C74UL) /**< \brief (TCC2) Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC2 peripheral ========== */
#define TCC2_CC_NUM                 2        // Number of Compare/Capture units
#define TCC2_DITHERING              0        // Dithering feature implemented
#define TCC2_DMAC_ID_MC_0           20
#define TCC2_DMAC_ID_MC_1           21
#define TCC2_DMAC_ID_MC_LSB         20
#define TCC2_DMAC_ID_MC_MSB         21
#define TCC2_DMAC_ID_MC_SIZE        2
#define TCC2_DMAC_ID_OVF            19       // DMA overflow/underflow/retrigger trigger
#define TCC2_DTI                    0        // Dead-Time-Insertion feature implemented
#define TCC2_EXT                    0        // Coding of implemented extended features
#define TCC2_GCLK_ID                26       // Index of Generic Clock
#define TCC2_OTMX                   0        // Output Matrix feature implemented
#define TCC2_OW_NUM                 2        // Number of Output Waveforms
#define TCC2_PG                     0        // Pattern Generation feature implemented
#define TCC2_SIZE                   16      
#define TCC2_SWAP                   0        // DTI outputs swap feature implemented
#define TCC2_TYPE                   0        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAML21_TCC2_INSTANCE_ */
