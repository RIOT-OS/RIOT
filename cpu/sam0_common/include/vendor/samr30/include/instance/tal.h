/**
 * \file
 *
 * \brief Instance description for TAL
 *
 * Copyright (c) 2016 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMR30_TAL_INSTANCE_
#define _SAMR30_TAL_INSTANCE_

/* ========== Register definition for TAL peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TAL_CTRLA              (0x40002C00) /**< \brief (TAL) Control A */
#define REG_TAL_RSTCTRL            (0x40002C04) /**< \brief (TAL) Reset Control */
#define REG_TAL_EXTCTRL            (0x40002C05) /**< \brief (TAL) External Break Control */
#define REG_TAL_EVCTRL             (0x40002C06) /**< \brief (TAL) Event Control */
#define REG_TAL_INTENCLR           (0x40002C08) /**< \brief (TAL) Interrupt Enable Clear */
#define REG_TAL_INTENSET           (0x40002C09) /**< \brief (TAL) Interrupt Enable Set */
#define REG_TAL_INTFLAG            (0x40002C0A) /**< \brief (TAL) Interrupt Flag Status and Clear */
#define REG_TAL_GLOBMASK           (0x40002C0B) /**< \brief (TAL) Global Break Requests Mask */
#define REG_TAL_HALT               (0x40002C0C) /**< \brief (TAL) Debug Halt Request */
#define REG_TAL_RESTART            (0x40002C0D) /**< \brief (TAL) Debug Restart Request */
#define REG_TAL_BRKSTATUS          (0x40002C0E) /**< \brief (TAL) Break Request Status */
#define REG_TAL_CTICTRLA0          (0x40002C10) /**< \brief (TAL) Cross-Trigger Interface 0 Control A */
#define REG_TAL_CTIMASK0           (0x40002C11) /**< \brief (TAL) Cross-Trigger Interface 0 Mask */
#define REG_TAL_CTICTRLA1          (0x40002C12) /**< \brief (TAL) Cross-Trigger Interface 1 Control A */
#define REG_TAL_CTIMASK1           (0x40002C13) /**< \brief (TAL) Cross-Trigger Interface 1 Mask */
#define REG_TAL_CTICTRLA2          (0x40002C14) /**< \brief (TAL) Cross-Trigger Interface 2 Control A */
#define REG_TAL_CTIMASK2           (0x40002C15) /**< \brief (TAL) Cross-Trigger Interface 2 Mask */
#define REG_TAL_CTICTRLA3          (0x40002C16) /**< \brief (TAL) Cross-Trigger Interface 3 Control A */
#define REG_TAL_CTIMASK3           (0x40002C17) /**< \brief (TAL) Cross-Trigger Interface 3 Mask */
#define REG_TAL_INTSTATUS0         (0x40002C20) /**< \brief (TAL) Interrupt 0 Status */
#define REG_TAL_INTSTATUS1         (0x40002C21) /**< \brief (TAL) Interrupt 1 Status */
#define REG_TAL_INTSTATUS2         (0x40002C22) /**< \brief (TAL) Interrupt 2 Status */
#define REG_TAL_INTSTATUS3         (0x40002C23) /**< \brief (TAL) Interrupt 3 Status */
#define REG_TAL_INTSTATUS4         (0x40002C24) /**< \brief (TAL) Interrupt 4 Status */
#define REG_TAL_INTSTATUS5         (0x40002C25) /**< \brief (TAL) Interrupt 5 Status */
#define REG_TAL_INTSTATUS6         (0x40002C26) /**< \brief (TAL) Interrupt 6 Status */
#define REG_TAL_INTSTATUS7         (0x40002C27) /**< \brief (TAL) Interrupt 7 Status */
#define REG_TAL_INTSTATUS8         (0x40002C28) /**< \brief (TAL) Interrupt 8 Status */
#define REG_TAL_INTSTATUS9         (0x40002C29) /**< \brief (TAL) Interrupt 9 Status */
#define REG_TAL_INTSTATUS10        (0x40002C2A) /**< \brief (TAL) Interrupt 10 Status */
#define REG_TAL_INTSTATUS11        (0x40002C2B) /**< \brief (TAL) Interrupt 11 Status */
#define REG_TAL_INTSTATUS12        (0x40002C2C) /**< \brief (TAL) Interrupt 12 Status */
#define REG_TAL_INTSTATUS13        (0x40002C2D) /**< \brief (TAL) Interrupt 13 Status */
#define REG_TAL_INTSTATUS14        (0x40002C2E) /**< \brief (TAL) Interrupt 14 Status */
#define REG_TAL_INTSTATUS15        (0x40002C2F) /**< \brief (TAL) Interrupt 15 Status */
#define REG_TAL_INTSTATUS16        (0x40002C30) /**< \brief (TAL) Interrupt 16 Status */
#define REG_TAL_INTSTATUS17        (0x40002C31) /**< \brief (TAL) Interrupt 17 Status */
#define REG_TAL_INTSTATUS18        (0x40002C32) /**< \brief (TAL) Interrupt 18 Status */
#define REG_TAL_INTSTATUS19        (0x40002C33) /**< \brief (TAL) Interrupt 19 Status */
#define REG_TAL_INTSTATUS20        (0x40002C34) /**< \brief (TAL) Interrupt 20 Status */
#define REG_TAL_INTSTATUS21        (0x40002C35) /**< \brief (TAL) Interrupt 21 Status */
#define REG_TAL_INTSTATUS22        (0x40002C36) /**< \brief (TAL) Interrupt 22 Status */
#define REG_TAL_INTSTATUS23        (0x40002C37) /**< \brief (TAL) Interrupt 23 Status */
#define REG_TAL_INTSTATUS24        (0x40002C38) /**< \brief (TAL) Interrupt 24 Status */
#define REG_TAL_INTSTATUS25        (0x40002C39) /**< \brief (TAL) Interrupt 25 Status */
#define REG_TAL_INTSTATUS26        (0x40002C3A) /**< \brief (TAL) Interrupt 26 Status */
#define REG_TAL_INTSTATUS27        (0x40002C3B) /**< \brief (TAL) Interrupt 27 Status */
#define REG_TAL_INTSTATUS28        (0x40002C3C) /**< \brief (TAL) Interrupt 28 Status */
#define REG_TAL_DMACPUSEL0         (0x40002C40) /**< \brief (TAL) DMA Channel Interrupts CPU Select 0 */
#define REG_TAL_EVCPUSEL0          (0x40002C48) /**< \brief (TAL) EVSYS Channel Interrupts CPU Select 0 */
#define REG_TAL_EICCPUSEL0         (0x40002C50) /**< \brief (TAL) EIC External Interrupts CPU Select 0 */
#define REG_TAL_INTCPUSEL0         (0x40002C58) /**< \brief (TAL) Interrupts CPU Select 0 */
#define REG_TAL_INTCPUSEL1         (0x40002C5C) /**< \brief (TAL) Interrupts CPU Select 1 */
#define REG_TAL_IRQTRIG            (0x40002C60) /**< \brief (TAL) Interrupt Trigger */
#define REG_TAL_CPUIRQS0           (0x40002C64) /**< \brief (TAL) Interrupt Status for CPU 0 */
#define REG_TAL_CPUIRQS1           (0x40002C68) /**< \brief (TAL) Interrupt Status for CPU 1 */
#else
#define REG_TAL_CTRLA              (*(RwReg8 *)0x40002C00UL) /**< \brief (TAL) Control A */
#define REG_TAL_RSTCTRL            (*(RwReg8 *)0x40002C04UL) /**< \brief (TAL) Reset Control */
#define REG_TAL_EXTCTRL            (*(RwReg8 *)0x40002C05UL) /**< \brief (TAL) External Break Control */
#define REG_TAL_EVCTRL             (*(RwReg8 *)0x40002C06UL) /**< \brief (TAL) Event Control */
#define REG_TAL_INTENCLR           (*(RwReg8 *)0x40002C08UL) /**< \brief (TAL) Interrupt Enable Clear */
#define REG_TAL_INTENSET           (*(RwReg8 *)0x40002C09UL) /**< \brief (TAL) Interrupt Enable Set */
#define REG_TAL_INTFLAG            (*(RwReg8 *)0x40002C0AUL) /**< \brief (TAL) Interrupt Flag Status and Clear */
#define REG_TAL_GLOBMASK           (*(RwReg8 *)0x40002C0BUL) /**< \brief (TAL) Global Break Requests Mask */
#define REG_TAL_HALT               (*(WoReg8 *)0x40002C0CUL) /**< \brief (TAL) Debug Halt Request */
#define REG_TAL_RESTART            (*(WoReg8 *)0x40002C0DUL) /**< \brief (TAL) Debug Restart Request */
#define REG_TAL_BRKSTATUS          (*(RoReg16*)0x40002C0EUL) /**< \brief (TAL) Break Request Status */
#define REG_TAL_CTICTRLA0          (*(RwReg8 *)0x40002C10UL) /**< \brief (TAL) Cross-Trigger Interface 0 Control A */
#define REG_TAL_CTIMASK0           (*(RwReg8 *)0x40002C11UL) /**< \brief (TAL) Cross-Trigger Interface 0 Mask */
#define REG_TAL_CTICTRLA1          (*(RwReg8 *)0x40002C12UL) /**< \brief (TAL) Cross-Trigger Interface 1 Control A */
#define REG_TAL_CTIMASK1           (*(RwReg8 *)0x40002C13UL) /**< \brief (TAL) Cross-Trigger Interface 1 Mask */
#define REG_TAL_CTICTRLA2          (*(RwReg8 *)0x40002C14UL) /**< \brief (TAL) Cross-Trigger Interface 2 Control A */
#define REG_TAL_CTIMASK2           (*(RwReg8 *)0x40002C15UL) /**< \brief (TAL) Cross-Trigger Interface 2 Mask */
#define REG_TAL_CTICTRLA3          (*(RwReg8 *)0x40002C16UL) /**< \brief (TAL) Cross-Trigger Interface 3 Control A */
#define REG_TAL_CTIMASK3           (*(RwReg8 *)0x40002C17UL) /**< \brief (TAL) Cross-Trigger Interface 3 Mask */
#define REG_TAL_INTSTATUS0         (*(RoReg8 *)0x40002C20UL) /**< \brief (TAL) Interrupt 0 Status */
#define REG_TAL_INTSTATUS1         (*(RoReg8 *)0x40002C21UL) /**< \brief (TAL) Interrupt 1 Status */
#define REG_TAL_INTSTATUS2         (*(RoReg8 *)0x40002C22UL) /**< \brief (TAL) Interrupt 2 Status */
#define REG_TAL_INTSTATUS3         (*(RoReg8 *)0x40002C23UL) /**< \brief (TAL) Interrupt 3 Status */
#define REG_TAL_INTSTATUS4         (*(RoReg8 *)0x40002C24UL) /**< \brief (TAL) Interrupt 4 Status */
#define REG_TAL_INTSTATUS5         (*(RoReg8 *)0x40002C25UL) /**< \brief (TAL) Interrupt 5 Status */
#define REG_TAL_INTSTATUS6         (*(RoReg8 *)0x40002C26UL) /**< \brief (TAL) Interrupt 6 Status */
#define REG_TAL_INTSTATUS7         (*(RoReg8 *)0x40002C27UL) /**< \brief (TAL) Interrupt 7 Status */
#define REG_TAL_INTSTATUS8         (*(RoReg8 *)0x40002C28UL) /**< \brief (TAL) Interrupt 8 Status */
#define REG_TAL_INTSTATUS9         (*(RoReg8 *)0x40002C29UL) /**< \brief (TAL) Interrupt 9 Status */
#define REG_TAL_INTSTATUS10        (*(RoReg8 *)0x40002C2AUL) /**< \brief (TAL) Interrupt 10 Status */
#define REG_TAL_INTSTATUS11        (*(RoReg8 *)0x40002C2BUL) /**< \brief (TAL) Interrupt 11 Status */
#define REG_TAL_INTSTATUS12        (*(RoReg8 *)0x40002C2CUL) /**< \brief (TAL) Interrupt 12 Status */
#define REG_TAL_INTSTATUS13        (*(RoReg8 *)0x40002C2DUL) /**< \brief (TAL) Interrupt 13 Status */
#define REG_TAL_INTSTATUS14        (*(RoReg8 *)0x40002C2EUL) /**< \brief (TAL) Interrupt 14 Status */
#define REG_TAL_INTSTATUS15        (*(RoReg8 *)0x40002C2FUL) /**< \brief (TAL) Interrupt 15 Status */
#define REG_TAL_INTSTATUS16        (*(RoReg8 *)0x40002C30UL) /**< \brief (TAL) Interrupt 16 Status */
#define REG_TAL_INTSTATUS17        (*(RoReg8 *)0x40002C31UL) /**< \brief (TAL) Interrupt 17 Status */
#define REG_TAL_INTSTATUS18        (*(RoReg8 *)0x40002C32UL) /**< \brief (TAL) Interrupt 18 Status */
#define REG_TAL_INTSTATUS19        (*(RoReg8 *)0x40002C33UL) /**< \brief (TAL) Interrupt 19 Status */
#define REG_TAL_INTSTATUS20        (*(RoReg8 *)0x40002C34UL) /**< \brief (TAL) Interrupt 20 Status */
#define REG_TAL_INTSTATUS21        (*(RoReg8 *)0x40002C35UL) /**< \brief (TAL) Interrupt 21 Status */
#define REG_TAL_INTSTATUS22        (*(RoReg8 *)0x40002C36UL) /**< \brief (TAL) Interrupt 22 Status */
#define REG_TAL_INTSTATUS23        (*(RoReg8 *)0x40002C37UL) /**< \brief (TAL) Interrupt 23 Status */
#define REG_TAL_INTSTATUS24        (*(RoReg8 *)0x40002C38UL) /**< \brief (TAL) Interrupt 24 Status */
#define REG_TAL_INTSTATUS25        (*(RoReg8 *)0x40002C39UL) /**< \brief (TAL) Interrupt 25 Status */
#define REG_TAL_INTSTATUS26        (*(RoReg8 *)0x40002C3AUL) /**< \brief (TAL) Interrupt 26 Status */
#define REG_TAL_INTSTATUS27        (*(RoReg8 *)0x40002C3BUL) /**< \brief (TAL) Interrupt 27 Status */
#define REG_TAL_INTSTATUS28        (*(RoReg8 *)0x40002C3CUL) /**< \brief (TAL) Interrupt 28 Status */
#define REG_TAL_DMACPUSEL0         (*(RwReg  *)0x40002C40UL) /**< \brief (TAL) DMA Channel Interrupts CPU Select 0 */
#define REG_TAL_EVCPUSEL0          (*(RwReg  *)0x40002C48UL) /**< \brief (TAL) EVSYS Channel Interrupts CPU Select 0 */
#define REG_TAL_EICCPUSEL0         (*(RwReg  *)0x40002C50UL) /**< \brief (TAL) EIC External Interrupts CPU Select 0 */
#define REG_TAL_INTCPUSEL0         (*(RwReg  *)0x40002C58UL) /**< \brief (TAL) Interrupts CPU Select 0 */
#define REG_TAL_INTCPUSEL1         (*(RwReg  *)0x40002C5CUL) /**< \brief (TAL) Interrupts CPU Select 1 */
#define REG_TAL_IRQTRIG            (*(RwReg16*)0x40002C60UL) /**< \brief (TAL) Interrupt Trigger */
#define REG_TAL_CPUIRQS0           (*(RoReg  *)0x40002C64UL) /**< \brief (TAL) Interrupt Status for CPU 0 */
#define REG_TAL_CPUIRQS1           (*(RoReg  *)0x40002C68UL) /**< \brief (TAL) Interrupt Status for CPU 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TAL peripheral ========== */
#define TAL_CPU_NUM                 2        // Number of CPUs
#define TAL_CTI_NUM                 4        // Number of Cross-Trigger Interfaces
#define TAL_DMA_CH_NUM              16       // Number of DMAC Channels
#define TAL_EV_CH_NUM               12       // Number of EVSYS Channels
#define TAL_EXTINT_NUM              16       // Number of EIC External Interrrupts
#define TAL_INT_NUM                 29       // Number of Interrupt Requests

#endif /* _SAMR30_TAL_INSTANCE_ */
