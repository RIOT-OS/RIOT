/**
 * \file
 *
 * \brief Instance description for TCC1
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAML21_TCC1_INSTANCE_
#define _SAML21_TCC1_INSTANCE_

/* ========== Register definition for TCC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC1_CTRLA             (0x42001800U) /**< \brief (TCC1) Control A */
#define REG_TCC1_CTRLBCLR          (0x42001804U) /**< \brief (TCC1) Control B Clear */
#define REG_TCC1_CTRLBSET          (0x42001805U) /**< \brief (TCC1) Control B Set */
#define REG_TCC1_SYNCBUSY          (0x42001808U) /**< \brief (TCC1) Synchronization Busy */
#define REG_TCC1_FCTRLA            (0x4200180CU) /**< \brief (TCC1) Recoverable Fault A Configuration */
#define REG_TCC1_FCTRLB            (0x42001810U) /**< \brief (TCC1) Recoverable Fault B Configuration */
#define REG_TCC1_DRVCTRL           (0x42001818U) /**< \brief (TCC1) Driver Control */
#define REG_TCC1_DBGCTRL           (0x4200181EU) /**< \brief (TCC1) Debug Control */
#define REG_TCC1_EVCTRL            (0x42001820U) /**< \brief (TCC1) Event Control */
#define REG_TCC1_INTENCLR          (0x42001824U) /**< \brief (TCC1) Interrupt Enable Clear */
#define REG_TCC1_INTENSET          (0x42001828U) /**< \brief (TCC1) Interrupt Enable Set */
#define REG_TCC1_INTFLAG           (0x4200182CU) /**< \brief (TCC1) Interrupt Flag Status and Clear */
#define REG_TCC1_STATUS            (0x42001830U) /**< \brief (TCC1) Status */
#define REG_TCC1_COUNT             (0x42001834U) /**< \brief (TCC1) Count */
#define REG_TCC1_PATT              (0x42001838U) /**< \brief (TCC1) Pattern */
#define REG_TCC1_WAVE              (0x4200183CU) /**< \brief (TCC1) Waveform Control */
#define REG_TCC1_PER               (0x42001840U) /**< \brief (TCC1) Period */
#define REG_TCC1_CC0               (0x42001844U) /**< \brief (TCC1) Compare and Capture 0 */
#define REG_TCC1_CC1               (0x42001848U) /**< \brief (TCC1) Compare and Capture 1 */
#define REG_TCC1_PATTBUF           (0x42001864U) /**< \brief (TCC1) Pattern Buffer */
#define REG_TCC1_PERBUF            (0x4200186CU) /**< \brief (TCC1) Period Buffer */
#define REG_TCC1_CCBUF0            (0x42001870U) /**< \brief (TCC1) Compare and Capture Buffer 0 */
#define REG_TCC1_CCBUF1            (0x42001874U) /**< \brief (TCC1) Compare and Capture Buffer 1 */
#else
#define REG_TCC1_CTRLA             (*(RwReg  *)0x42001800U) /**< \brief (TCC1) Control A */
#define REG_TCC1_CTRLBCLR          (*(RwReg8 *)0x42001804U) /**< \brief (TCC1) Control B Clear */
#define REG_TCC1_CTRLBSET          (*(RwReg8 *)0x42001805U) /**< \brief (TCC1) Control B Set */
#define REG_TCC1_SYNCBUSY          (*(RoReg  *)0x42001808U) /**< \brief (TCC1) Synchronization Busy */
#define REG_TCC1_FCTRLA            (*(RwReg  *)0x4200180CU) /**< \brief (TCC1) Recoverable Fault A Configuration */
#define REG_TCC1_FCTRLB            (*(RwReg  *)0x42001810U) /**< \brief (TCC1) Recoverable Fault B Configuration */
#define REG_TCC1_DRVCTRL           (*(RwReg  *)0x42001818U) /**< \brief (TCC1) Driver Control */
#define REG_TCC1_DBGCTRL           (*(RwReg8 *)0x4200181EU) /**< \brief (TCC1) Debug Control */
#define REG_TCC1_EVCTRL            (*(RwReg  *)0x42001820U) /**< \brief (TCC1) Event Control */
#define REG_TCC1_INTENCLR          (*(RwReg  *)0x42001824U) /**< \brief (TCC1) Interrupt Enable Clear */
#define REG_TCC1_INTENSET          (*(RwReg  *)0x42001828U) /**< \brief (TCC1) Interrupt Enable Set */
#define REG_TCC1_INTFLAG           (*(RwReg  *)0x4200182CU) /**< \brief (TCC1) Interrupt Flag Status and Clear */
#define REG_TCC1_STATUS            (*(RwReg  *)0x42001830U) /**< \brief (TCC1) Status */
#define REG_TCC1_COUNT             (*(RwReg  *)0x42001834U) /**< \brief (TCC1) Count */
#define REG_TCC1_PATT              (*(RwReg16*)0x42001838U) /**< \brief (TCC1) Pattern */
#define REG_TCC1_WAVE              (*(RwReg  *)0x4200183CU) /**< \brief (TCC1) Waveform Control */
#define REG_TCC1_PER               (*(RwReg  *)0x42001840U) /**< \brief (TCC1) Period */
#define REG_TCC1_CC0               (*(RwReg  *)0x42001844U) /**< \brief (TCC1) Compare and Capture 0 */
#define REG_TCC1_CC1               (*(RwReg  *)0x42001848U) /**< \brief (TCC1) Compare and Capture 1 */
#define REG_TCC1_PATTBUF           (*(RwReg16*)0x42001864U) /**< \brief (TCC1) Pattern Buffer */
#define REG_TCC1_PERBUF            (*(RwReg  *)0x4200186CU) /**< \brief (TCC1) Period Buffer */
#define REG_TCC1_CCBUF0            (*(RwReg  *)0x42001870U) /**< \brief (TCC1) Compare and Capture Buffer 0 */
#define REG_TCC1_CCBUF1            (*(RwReg  *)0x42001874U) /**< \brief (TCC1) Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC1 peripheral ========== */
#define TCC1_CC_NUM                 2        // Number of Compare/Capture units
#define TCC1_DITHERING              1        // Dithering feature implemented
#define TCC1_DMAC_ID_MC_0           17
#define TCC1_DMAC_ID_MC_1           18
#define TCC1_DMAC_ID_MC_LSB         17
#define TCC1_DMAC_ID_MC_MSB         18
#define TCC1_DMAC_ID_MC_SIZE        2
#define TCC1_DMAC_ID_OVF            16       // DMA overflow/underflow/retrigger trigger
#define TCC1_DTI                    0        // Dead-Time-Insertion feature implemented
#define TCC1_EXT                    24       // Coding of implemented extended features
#define TCC1_GCLK_ID                25       // Index of Generic Clock
#define TCC1_OTMX                   0        // Output Matrix feature implemented
#define TCC1_OW_NUM                 4        // Number of Output Waveforms
#define TCC1_PG                     1        // Pattern Generation feature implemented
#define TCC1_SIZE                   24
#define TCC1_SWAP                   0        // DTI outputs swap feature implemented
#define TCC1_TYPE                   2        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAML21_TCC1_INSTANCE_ */
