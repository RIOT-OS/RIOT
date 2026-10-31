/**
 * \file
 *
 * \brief Instance description for DMAC
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

#ifndef _SAMR34_DMAC_INSTANCE_
#define _SAMR34_DMAC_INSTANCE_

/* ========== Register definition for DMAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DMAC_CTRL              (0x44000400) /**< \brief (DMAC) Control */
#define REG_DMAC_CRCCTRL           (0x44000402) /**< \brief (DMAC) CRC Control */
#define REG_DMAC_CRCDATAIN         (0x44000404) /**< \brief (DMAC) CRC Data Input */
#define REG_DMAC_CRCCHKSUM         (0x44000408) /**< \brief (DMAC) CRC Checksum */
#define REG_DMAC_CRCSTATUS         (0x4400040C) /**< \brief (DMAC) CRC Status */
#define REG_DMAC_DBGCTRL           (0x4400040D) /**< \brief (DMAC) Debug Control */
#define REG_DMAC_QOSCTRL           (0x4400040E) /**< \brief (DMAC) QOS Control */
#define REG_DMAC_SWTRIGCTRL        (0x44000410) /**< \brief (DMAC) Software Trigger Control */
#define REG_DMAC_PRICTRL0          (0x44000414) /**< \brief (DMAC) Priority Control 0 */
#define REG_DMAC_INTPEND           (0x44000420) /**< \brief (DMAC) Interrupt Pending */
#define REG_DMAC_INTSTATUS         (0x44000424) /**< \brief (DMAC) Interrupt Status */
#define REG_DMAC_BUSYCH            (0x44000428) /**< \brief (DMAC) Busy Channels */
#define REG_DMAC_PENDCH            (0x4400042C) /**< \brief (DMAC) Pending Channels */
#define REG_DMAC_ACTIVE            (0x44000430) /**< \brief (DMAC) Active Channel and Levels */
#define REG_DMAC_BASEADDR          (0x44000434) /**< \brief (DMAC) Descriptor Memory Section Base Address */
#define REG_DMAC_WRBADDR           (0x44000438) /**< \brief (DMAC) Write-Back Memory Section Base Address */
#define REG_DMAC_CHID              (0x4400043F) /**< \brief (DMAC) Channel ID */
#define REG_DMAC_CHCTRLA           (0x44000440) /**< \brief (DMAC) Channel Control A */
#define REG_DMAC_CHCTRLB           (0x44000444) /**< \brief (DMAC) Channel Control B */
#define REG_DMAC_CHINTENCLR        (0x4400044C) /**< \brief (DMAC) Channel Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET        (0x4400044D) /**< \brief (DMAC) Channel Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG         (0x4400044E) /**< \brief (DMAC) Channel Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS          (0x4400044F) /**< \brief (DMAC) Channel Status */
#else
#define REG_DMAC_CTRL              (*(RwReg16*)0x44000400UL) /**< \brief (DMAC) Control */
#define REG_DMAC_CRCCTRL           (*(RwReg16*)0x44000402UL) /**< \brief (DMAC) CRC Control */
#define REG_DMAC_CRCDATAIN         (*(RwReg  *)0x44000404UL) /**< \brief (DMAC) CRC Data Input */
#define REG_DMAC_CRCCHKSUM         (*(RwReg  *)0x44000408UL) /**< \brief (DMAC) CRC Checksum */
#define REG_DMAC_CRCSTATUS         (*(RwReg8 *)0x4400040CUL) /**< \brief (DMAC) CRC Status */
#define REG_DMAC_DBGCTRL           (*(RwReg8 *)0x4400040DUL) /**< \brief (DMAC) Debug Control */
#define REG_DMAC_QOSCTRL           (*(RwReg8 *)0x4400040EUL) /**< \brief (DMAC) QOS Control */
#define REG_DMAC_SWTRIGCTRL        (*(RwReg  *)0x44000410UL) /**< \brief (DMAC) Software Trigger Control */
#define REG_DMAC_PRICTRL0          (*(RwReg  *)0x44000414UL) /**< \brief (DMAC) Priority Control 0 */
#define REG_DMAC_INTPEND           (*(RwReg16*)0x44000420UL) /**< \brief (DMAC) Interrupt Pending */
#define REG_DMAC_INTSTATUS         (*(RoReg  *)0x44000424UL) /**< \brief (DMAC) Interrupt Status */
#define REG_DMAC_BUSYCH            (*(RoReg  *)0x44000428UL) /**< \brief (DMAC) Busy Channels */
#define REG_DMAC_PENDCH            (*(RoReg  *)0x4400042CUL) /**< \brief (DMAC) Pending Channels */
#define REG_DMAC_ACTIVE            (*(RoReg  *)0x44000430UL) /**< \brief (DMAC) Active Channel and Levels */
#define REG_DMAC_BASEADDR          (*(RwReg  *)0x44000434UL) /**< \brief (DMAC) Descriptor Memory Section Base Address */
#define REG_DMAC_WRBADDR           (*(RwReg  *)0x44000438UL) /**< \brief (DMAC) Write-Back Memory Section Base Address */
#define REG_DMAC_CHID              (*(RwReg8 *)0x4400043FUL) /**< \brief (DMAC) Channel ID */
#define REG_DMAC_CHCTRLA           (*(RwReg8 *)0x44000440UL) /**< \brief (DMAC) Channel Control A */
#define REG_DMAC_CHCTRLB           (*(RwReg  *)0x44000444UL) /**< \brief (DMAC) Channel Control B */
#define REG_DMAC_CHINTENCLR        (*(RwReg8 *)0x4400044CUL) /**< \brief (DMAC) Channel Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET        (*(RwReg8 *)0x4400044DUL) /**< \brief (DMAC) Channel Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG         (*(RwReg8 *)0x4400044EUL) /**< \brief (DMAC) Channel Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS          (*(RoReg8 *)0x4400044FUL) /**< \brief (DMAC) Channel Status */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DMAC peripheral ========== */
#define DMAC_CH_BITS                4        // Number of bits to select channel
#define DMAC_CH_NUM                 16       // Number of channels
#define DMAC_CLK_AHB_ID             11       // AHB clock index
#define DMAC_EVIN_NUM               8        // Number of input events
#define DMAC_EVOUT_NUM              8        // Number of output events
#define DMAC_LVL_BITS               2        // Number of bit to select level priority
#define DMAC_LVL_NUM                4        // Enable priority level number
#define DMAC_QOSCTRL_D_RESETVALUE   2        // QOS dmac ahb interface reset value
#define DMAC_QOSCTRL_F_RESETVALUE   2        // QOS dmac fetch interface reset value
#define DMAC_QOSCTRL_WRB_RESETVALUE 2        // QOS dmac write back interface reset value
#define DMAC_TRIG_BITS              6        // Number of bits to select trigger source
#define DMAC_TRIG_NUM               46       // Number of peripheral triggers

#endif /* _SAMR34_DMAC_INSTANCE_ */
