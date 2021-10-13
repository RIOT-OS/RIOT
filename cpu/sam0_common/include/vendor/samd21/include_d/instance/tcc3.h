/**
 * \file
 *
 * \brief Instance description for TCC3
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

#ifndef _SAMD21_TCC3_INSTANCE_
#define _SAMD21_TCC3_INSTANCE_

/* ========== Register definition for TCC3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC3_CTRLA             (0x42006000) /**< \brief (TCC3) Control A */
#define REG_TCC3_CTRLBCLR          (0x42006004) /**< \brief (TCC3) Control B Clear */
#define REG_TCC3_CTRLBSET          (0x42006005) /**< \brief (TCC3) Control B Set */
#define REG_TCC3_SYNCBUSY          (0x42006008) /**< \brief (TCC3) Synchronization Busy */
#define REG_TCC3_FCTRLA            (0x4200600C) /**< \brief (TCC3) Recoverable Fault A Configuration */
#define REG_TCC3_FCTRLB            (0x42006010) /**< \brief (TCC3) Recoverable Fault B Configuration */
#define REG_TCC3_WEXCTRL           (0x42006014) /**< \brief (TCC3) Waveform Extension Configuration */
#define REG_TCC3_DRVCTRL           (0x42006018) /**< \brief (TCC3) Driver Control */
#define REG_TCC3_DBGCTRL           (0x4200601E) /**< \brief (TCC3) Debug Control */
#define REG_TCC3_EVCTRL            (0x42006020) /**< \brief (TCC3) Event Control */
#define REG_TCC3_INTENCLR          (0x42006024) /**< \brief (TCC3) Interrupt Enable Clear */
#define REG_TCC3_INTENSET          (0x42006028) /**< \brief (TCC3) Interrupt Enable Set */
#define REG_TCC3_INTFLAG           (0x4200602C) /**< \brief (TCC3) Interrupt Flag Status and Clear */
#define REG_TCC3_STATUS            (0x42006030) /**< \brief (TCC3) Status */
#define REG_TCC3_COUNT             (0x42006034) /**< \brief (TCC3) Count */
#define REG_TCC3_PATT              (0x42006038) /**< \brief (TCC3) Pattern */
#define REG_TCC3_WAVE              (0x4200603C) /**< \brief (TCC3) Waveform Control */
#define REG_TCC3_PER               (0x42006040) /**< \brief (TCC3) Period */
#define REG_TCC3_CC0               (0x42006044) /**< \brief (TCC3) Compare and Capture 0 */
#define REG_TCC3_CC1               (0x42006048) /**< \brief (TCC3) Compare and Capture 1 */
#define REG_TCC3_CC2               (0x4200604C) /**< \brief (TCC3) Compare and Capture 2 */
#define REG_TCC3_CC3               (0x42006050) /**< \brief (TCC3) Compare and Capture 3 */
#define REG_TCC3_PATTB             (0x42006064) /**< \brief (TCC3) Pattern Buffer */
#define REG_TCC3_WAVEB             (0x42006068) /**< \brief (TCC3) Waveform Control Buffer */
#define REG_TCC3_PERB              (0x4200606C) /**< \brief (TCC3) Period Buffer */
#define REG_TCC3_CCB0              (0x42006070) /**< \brief (TCC3) Compare and Capture Buffer 0 */
#define REG_TCC3_CCB1              (0x42006074) /**< \brief (TCC3) Compare and Capture Buffer 1 */
#define REG_TCC3_CCB2              (0x42006078) /**< \brief (TCC3) Compare and Capture Buffer 2 */
#define REG_TCC3_CCB3              (0x4200607C) /**< \brief (TCC3) Compare and Capture Buffer 3 */
#else
#define REG_TCC3_CTRLA             (*(RwReg  *)0x42006000UL) /**< \brief (TCC3) Control A */
#define REG_TCC3_CTRLBCLR          (*(RwReg8 *)0x42006004UL) /**< \brief (TCC3) Control B Clear */
#define REG_TCC3_CTRLBSET          (*(RwReg8 *)0x42006005UL) /**< \brief (TCC3) Control B Set */
#define REG_TCC3_SYNCBUSY          (*(RoReg  *)0x42006008UL) /**< \brief (TCC3) Synchronization Busy */
#define REG_TCC3_FCTRLA            (*(RwReg  *)0x4200600CUL) /**< \brief (TCC3) Recoverable Fault A Configuration */
#define REG_TCC3_FCTRLB            (*(RwReg  *)0x42006010UL) /**< \brief (TCC3) Recoverable Fault B Configuration */
#define REG_TCC3_WEXCTRL           (*(RwReg  *)0x42006014UL) /**< \brief (TCC3) Waveform Extension Configuration */
#define REG_TCC3_DRVCTRL           (*(RwReg  *)0x42006018UL) /**< \brief (TCC3) Driver Control */
#define REG_TCC3_DBGCTRL           (*(RwReg8 *)0x4200601EUL) /**< \brief (TCC3) Debug Control */
#define REG_TCC3_EVCTRL            (*(RwReg  *)0x42006020UL) /**< \brief (TCC3) Event Control */
#define REG_TCC3_INTENCLR          (*(RwReg  *)0x42006024UL) /**< \brief (TCC3) Interrupt Enable Clear */
#define REG_TCC3_INTENSET          (*(RwReg  *)0x42006028UL) /**< \brief (TCC3) Interrupt Enable Set */
#define REG_TCC3_INTFLAG           (*(RwReg  *)0x4200602CUL) /**< \brief (TCC3) Interrupt Flag Status and Clear */
#define REG_TCC3_STATUS            (*(RwReg  *)0x42006030UL) /**< \brief (TCC3) Status */
#define REG_TCC3_COUNT             (*(RwReg  *)0x42006034UL) /**< \brief (TCC3) Count */
#define REG_TCC3_PATT              (*(RwReg16*)0x42006038UL) /**< \brief (TCC3) Pattern */
#define REG_TCC3_WAVE              (*(RwReg  *)0x4200603CUL) /**< \brief (TCC3) Waveform Control */
#define REG_TCC3_PER               (*(RwReg  *)0x42006040UL) /**< \brief (TCC3) Period */
#define REG_TCC3_CC0               (*(RwReg  *)0x42006044UL) /**< \brief (TCC3) Compare and Capture 0 */
#define REG_TCC3_CC1               (*(RwReg  *)0x42006048UL) /**< \brief (TCC3) Compare and Capture 1 */
#define REG_TCC3_CC2               (*(RwReg  *)0x4200604CUL) /**< \brief (TCC3) Compare and Capture 2 */
#define REG_TCC3_CC3               (*(RwReg  *)0x42006050UL) /**< \brief (TCC3) Compare and Capture 3 */
#define REG_TCC3_PATTB             (*(RwReg16*)0x42006064UL) /**< \brief (TCC3) Pattern Buffer */
#define REG_TCC3_WAVEB             (*(RwReg  *)0x42006068UL) /**< \brief (TCC3) Waveform Control Buffer */
#define REG_TCC3_PERB              (*(RwReg  *)0x4200606CUL) /**< \brief (TCC3) Period Buffer */
#define REG_TCC3_CCB0              (*(RwReg  *)0x42006070UL) /**< \brief (TCC3) Compare and Capture Buffer 0 */
#define REG_TCC3_CCB1              (*(RwReg  *)0x42006074UL) /**< \brief (TCC3) Compare and Capture Buffer 1 */
#define REG_TCC3_CCB2              (*(RwReg  *)0x42006078UL) /**< \brief (TCC3) Compare and Capture Buffer 2 */
#define REG_TCC3_CCB3              (*(RwReg  *)0x4200607CUL) /**< \brief (TCC3) Compare and Capture Buffer 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC3 peripheral ========== */
#define TCC3_CC_NUM                 4        // Number of Compare/Capture units
#define TCC3_DITHERING              1        // Dithering feature implemented
#define TCC3_DMAC_ID_MC_0           46
#define TCC3_DMAC_ID_MC_1           47
#define TCC3_DMAC_ID_MC_2           48
#define TCC3_DMAC_ID_MC_3           49
#define TCC3_DMAC_ID_MC_LSB         46
#define TCC3_DMAC_ID_MC_MSB         49
#define TCC3_DMAC_ID_MC_SIZE        4
#define TCC3_DMAC_ID_OVF            45       // DMA overflow/underflow/retrigger trigger
#define TCC3_DTI                    1        // Dead-Time-Insertion feature implemented
#define TCC3_EXT                    31       // Coding of implemented extended features
#define TCC3_GCLK_ID                37       // Index of Generic Clock
#define TCC3_MASTER                 0       
#define TCC3_OTMX                   1        // Output Matrix feature implemented
#define TCC3_OW_NUM                 8        // Number of Output Waveforms
#define TCC3_PG                     1        // Pattern Generation feature implemented
#define TCC3_SIZE                   24      
#define TCC3_SWAP                   1        // DTI outputs swap feature implemented
#define TCC3_TYPE                   0        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAMD21_TCC3_INSTANCE_ */
