/**
 * \file
 *
 * \brief Instance description for USB
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

#ifndef _SAML21_USB_INSTANCE_
#define _SAML21_USB_INSTANCE_

/* ========== Register definition for USB peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_USB_CTRLA              (0x41000000U) /**< \brief (USB) Control A */
#define REG_USB_SYNCBUSY           (0x41000002U) /**< \brief (USB) Synchronization Busy */
#define REG_USB_FSMSTATUS          (0x4100000DU) /**< \brief (USB) Finite State Machine Status */
#define REG_USB_DESCADD            (0x41000024U) /**< \brief (USB) Descriptor Address */
#define REG_USB_PADCAL             (0x41000028U) /**< \brief (USB) USB PAD Calibration */
#define REG_USB_DEVICE_CTRLB       (0x41000008U) /**< \brief (USB) DEVICE Control B */
#define REG_USB_DEVICE_DADD        (0x4100000AU) /**< \brief (USB) DEVICE Device Address */
#define REG_USB_DEVICE_STATUS      (0x4100000CU) /**< \brief (USB) DEVICE Status */
#define REG_USB_DEVICE_FNUM        (0x41000010U) /**< \brief (USB) DEVICE Device Frame Number */
#define REG_USB_DEVICE_INTENCLR    (0x41000014U) /**< \brief (USB) DEVICE Device Interrupt Enable Clear */
#define REG_USB_DEVICE_INTENSET    (0x41000018U) /**< \brief (USB) DEVICE Device Interrupt Enable Set */
#define REG_USB_DEVICE_INTFLAG     (0x4100001CU) /**< \brief (USB) DEVICE Device Interrupt Flag */
#define REG_USB_DEVICE_EPINTSMRY   (0x41000020U) /**< \brief (USB) DEVICE End Point Interrupt Summary */
#define REG_USB_DEVICE_ENDPOINT_EPCFG0 (0x41000100U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR0 (0x41000104U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET0 (0x41000105U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS0 (0x41000106U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG0 (0x41000107U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR0 (0x41000108U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET0 (0x41000109U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG1 (0x41000120U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR1 (0x41000124U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET1 (0x41000125U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS1 (0x41000126U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG1 (0x41000127U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR1 (0x41000128U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET1 (0x41000129U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG2 (0x41000140U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR2 (0x41000144U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET2 (0x41000145U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS2 (0x41000146U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG2 (0x41000147U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR2 (0x41000148U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET2 (0x41000149U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG3 (0x41000160U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR3 (0x41000164U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET3 (0x41000165U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS3 (0x41000166U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG3 (0x41000167U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR3 (0x41000168U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET3 (0x41000169U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG4 (0x41000180U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR4 (0x41000184U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET4 (0x41000185U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS4 (0x41000186U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG4 (0x41000187U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR4 (0x41000188U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET4 (0x41000189U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG5 (0x410001A0U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR5 (0x410001A4U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET5 (0x410001A5U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS5 (0x410001A6U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG5 (0x410001A7U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR5 (0x410001A8U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET5 (0x410001A9U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG6 (0x410001C0U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR6 (0x410001C4U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET6 (0x410001C5U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS6 (0x410001C6U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG6 (0x410001C7U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR6 (0x410001C8U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET6 (0x410001C9U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG7 (0x410001E0U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR7 (0x410001E4U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET7 (0x410001E5U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS7 (0x410001E6U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG7 (0x410001E7U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR7 (0x410001E8U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET7 (0x410001E9U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 7 */
#define REG_USB_HOST_CTRLB         (0x41000008U) /**< \brief (USB) HOST Control B */
#define REG_USB_HOST_HSOFC         (0x4100000AU) /**< \brief (USB) HOST Host Start Of Frame Control */
#define REG_USB_HOST_STATUS        (0x4100000CU) /**< \brief (USB) HOST Status */
#define REG_USB_HOST_FNUM          (0x41000010U) /**< \brief (USB) HOST Host Frame Number */
#define REG_USB_HOST_FLENHIGH      (0x41000012U) /**< \brief (USB) HOST Host Frame Length */
#define REG_USB_HOST_INTENCLR      (0x41000014U) /**< \brief (USB) HOST Host Interrupt Enable Clear */
#define REG_USB_HOST_INTENSET      (0x41000018U) /**< \brief (USB) HOST Host Interrupt Enable Set */
#define REG_USB_HOST_INTFLAG       (0x4100001CU) /**< \brief (USB) HOST Host Interrupt Flag */
#define REG_USB_HOST_PINTSMRY      (0x41000020U) /**< \brief (USB) HOST Pipe Interrupt Summary */
#define REG_USB_HOST_PIPE_PCFG0    (0x41000100U) /**< \brief (USB) HOST_PIPE End Point Configuration 0 */
#define REG_USB_HOST_PIPE_BINTERVAL0 (0x41000103U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 0 */
#define REG_USB_HOST_PIPE_PSTATUSCLR0 (0x41000104U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 0 */
#define REG_USB_HOST_PIPE_PSTATUSSET0 (0x41000105U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 0 */
#define REG_USB_HOST_PIPE_PSTATUS0 (0x41000106U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 0 */
#define REG_USB_HOST_PIPE_PINTFLAG0 (0x41000107U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 0 */
#define REG_USB_HOST_PIPE_PINTENCLR0 (0x41000108U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 0 */
#define REG_USB_HOST_PIPE_PINTENSET0 (0x41000109U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 0 */
#define REG_USB_HOST_PIPE_PCFG1    (0x41000120U) /**< \brief (USB) HOST_PIPE End Point Configuration 1 */
#define REG_USB_HOST_PIPE_BINTERVAL1 (0x41000123U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 1 */
#define REG_USB_HOST_PIPE_PSTATUSCLR1 (0x41000124U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 1 */
#define REG_USB_HOST_PIPE_PSTATUSSET1 (0x41000125U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 1 */
#define REG_USB_HOST_PIPE_PSTATUS1 (0x41000126U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 1 */
#define REG_USB_HOST_PIPE_PINTFLAG1 (0x41000127U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 1 */
#define REG_USB_HOST_PIPE_PINTENCLR1 (0x41000128U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 1 */
#define REG_USB_HOST_PIPE_PINTENSET1 (0x41000129U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 1 */
#define REG_USB_HOST_PIPE_PCFG2    (0x41000140U) /**< \brief (USB) HOST_PIPE End Point Configuration 2 */
#define REG_USB_HOST_PIPE_BINTERVAL2 (0x41000143U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 2 */
#define REG_USB_HOST_PIPE_PSTATUSCLR2 (0x41000144U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 2 */
#define REG_USB_HOST_PIPE_PSTATUSSET2 (0x41000145U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 2 */
#define REG_USB_HOST_PIPE_PSTATUS2 (0x41000146U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 2 */
#define REG_USB_HOST_PIPE_PINTFLAG2 (0x41000147U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 2 */
#define REG_USB_HOST_PIPE_PINTENCLR2 (0x41000148U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 2 */
#define REG_USB_HOST_PIPE_PINTENSET2 (0x41000149U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 2 */
#define REG_USB_HOST_PIPE_PCFG3    (0x41000160U) /**< \brief (USB) HOST_PIPE End Point Configuration 3 */
#define REG_USB_HOST_PIPE_BINTERVAL3 (0x41000163U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 3 */
#define REG_USB_HOST_PIPE_PSTATUSCLR3 (0x41000164U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 3 */
#define REG_USB_HOST_PIPE_PSTATUSSET3 (0x41000165U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 3 */
#define REG_USB_HOST_PIPE_PSTATUS3 (0x41000166U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 3 */
#define REG_USB_HOST_PIPE_PINTFLAG3 (0x41000167U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 3 */
#define REG_USB_HOST_PIPE_PINTENCLR3 (0x41000168U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 3 */
#define REG_USB_HOST_PIPE_PINTENSET3 (0x41000169U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 3 */
#define REG_USB_HOST_PIPE_PCFG4    (0x41000180U) /**< \brief (USB) HOST_PIPE End Point Configuration 4 */
#define REG_USB_HOST_PIPE_BINTERVAL4 (0x41000183U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 4 */
#define REG_USB_HOST_PIPE_PSTATUSCLR4 (0x41000184U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 4 */
#define REG_USB_HOST_PIPE_PSTATUSSET4 (0x41000185U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 4 */
#define REG_USB_HOST_PIPE_PSTATUS4 (0x41000186U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 4 */
#define REG_USB_HOST_PIPE_PINTFLAG4 (0x41000187U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 4 */
#define REG_USB_HOST_PIPE_PINTENCLR4 (0x41000188U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 4 */
#define REG_USB_HOST_PIPE_PINTENSET4 (0x41000189U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 4 */
#define REG_USB_HOST_PIPE_PCFG5    (0x410001A0U) /**< \brief (USB) HOST_PIPE End Point Configuration 5 */
#define REG_USB_HOST_PIPE_BINTERVAL5 (0x410001A3U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 5 */
#define REG_USB_HOST_PIPE_PSTATUSCLR5 (0x410001A4U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 5 */
#define REG_USB_HOST_PIPE_PSTATUSSET5 (0x410001A5U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 5 */
#define REG_USB_HOST_PIPE_PSTATUS5 (0x410001A6U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 5 */
#define REG_USB_HOST_PIPE_PINTFLAG5 (0x410001A7U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 5 */
#define REG_USB_HOST_PIPE_PINTENCLR5 (0x410001A8U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 5 */
#define REG_USB_HOST_PIPE_PINTENSET5 (0x410001A9U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 5 */
#define REG_USB_HOST_PIPE_PCFG6    (0x410001C0U) /**< \brief (USB) HOST_PIPE End Point Configuration 6 */
#define REG_USB_HOST_PIPE_BINTERVAL6 (0x410001C3U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 6 */
#define REG_USB_HOST_PIPE_PSTATUSCLR6 (0x410001C4U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 6 */
#define REG_USB_HOST_PIPE_PSTATUSSET6 (0x410001C5U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 6 */
#define REG_USB_HOST_PIPE_PSTATUS6 (0x410001C6U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 6 */
#define REG_USB_HOST_PIPE_PINTFLAG6 (0x410001C7U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 6 */
#define REG_USB_HOST_PIPE_PINTENCLR6 (0x410001C8U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 6 */
#define REG_USB_HOST_PIPE_PINTENSET6 (0x410001C9U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 6 */
#define REG_USB_HOST_PIPE_PCFG7    (0x410001E0U) /**< \brief (USB) HOST_PIPE End Point Configuration 7 */
#define REG_USB_HOST_PIPE_BINTERVAL7 (0x410001E3U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 7 */
#define REG_USB_HOST_PIPE_PSTATUSCLR7 (0x410001E4U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 7 */
#define REG_USB_HOST_PIPE_PSTATUSSET7 (0x410001E5U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 7 */
#define REG_USB_HOST_PIPE_PSTATUS7 (0x410001E6U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 7 */
#define REG_USB_HOST_PIPE_PINTFLAG7 (0x410001E7U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 7 */
#define REG_USB_HOST_PIPE_PINTENCLR7 (0x410001E8U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 7 */
#define REG_USB_HOST_PIPE_PINTENSET7 (0x410001E9U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 7 */
#else
#define REG_USB_CTRLA              (*(RwReg8 *)0x41000000U) /**< \brief (USB) Control A */
#define REG_USB_SYNCBUSY           (*(RoReg8 *)0x41000002U) /**< \brief (USB) Synchronization Busy */
#define REG_USB_FSMSTATUS          (*(RoReg8 *)0x4100000DU) /**< \brief (USB) Finite State Machine Status */
#define REG_USB_DESCADD            (*(RwReg  *)0x41000024U) /**< \brief (USB) Descriptor Address */
#define REG_USB_PADCAL             (*(RwReg16*)0x41000028U) /**< \brief (USB) USB PAD Calibration */
#define REG_USB_DEVICE_CTRLB       (*(RwReg16*)0x41000008U) /**< \brief (USB) DEVICE Control B */
#define REG_USB_DEVICE_DADD        (*(RwReg8 *)0x4100000AU) /**< \brief (USB) DEVICE Device Address */
#define REG_USB_DEVICE_STATUS      (*(RoReg8 *)0x4100000CU) /**< \brief (USB) DEVICE Status */
#define REG_USB_DEVICE_FNUM        (*(RoReg16*)0x41000010U) /**< \brief (USB) DEVICE Device Frame Number */
#define REG_USB_DEVICE_INTENCLR    (*(RwReg16*)0x41000014U) /**< \brief (USB) DEVICE Device Interrupt Enable Clear */
#define REG_USB_DEVICE_INTENSET    (*(RwReg16*)0x41000018U) /**< \brief (USB) DEVICE Device Interrupt Enable Set */
#define REG_USB_DEVICE_INTFLAG     (*(RwReg16*)0x4100001CU) /**< \brief (USB) DEVICE Device Interrupt Flag */
#define REG_USB_DEVICE_EPINTSMRY   (*(RoReg16*)0x41000020U) /**< \brief (USB) DEVICE End Point Interrupt Summary */
#define REG_USB_DEVICE_ENDPOINT_EPCFG0 (*(RwReg8 *)0x41000100U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR0 (*(WoReg8 *)0x41000104U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET0 (*(WoReg8 *)0x41000105U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS0 (*(RoReg8 *)0x41000106U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG0 (*(RwReg8 *)0x41000107U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR0 (*(RwReg8 *)0x41000108U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET0 (*(RwReg8 *)0x41000109U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG1 (*(RwReg8 *)0x41000120U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR1 (*(WoReg8 *)0x41000124U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET1 (*(WoReg8 *)0x41000125U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS1 (*(RoReg8 *)0x41000126U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG1 (*(RwReg8 *)0x41000127U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR1 (*(RwReg8 *)0x41000128U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET1 (*(RwReg8 *)0x41000129U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG2 (*(RwReg8 *)0x41000140U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR2 (*(WoReg8 *)0x41000144U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET2 (*(WoReg8 *)0x41000145U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS2 (*(RoReg8 *)0x41000146U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG2 (*(RwReg8 *)0x41000147U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR2 (*(RwReg8 *)0x41000148U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET2 (*(RwReg8 *)0x41000149U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG3 (*(RwReg8 *)0x41000160U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR3 (*(WoReg8 *)0x41000164U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET3 (*(WoReg8 *)0x41000165U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS3 (*(RoReg8 *)0x41000166U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG3 (*(RwReg8 *)0x41000167U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR3 (*(RwReg8 *)0x41000168U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET3 (*(RwReg8 *)0x41000169U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG4 (*(RwReg8 *)0x41000180U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR4 (*(WoReg8 *)0x41000184U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET4 (*(WoReg8 *)0x41000185U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS4 (*(RoReg8 *)0x41000186U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG4 (*(RwReg8 *)0x41000187U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR4 (*(RwReg8 *)0x41000188U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET4 (*(RwReg8 *)0x41000189U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG5 (*(RwReg8 *)0x410001A0U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR5 (*(WoReg8 *)0x410001A4U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET5 (*(WoReg8 *)0x410001A5U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS5 (*(RoReg8 *)0x410001A6U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG5 (*(RwReg8 *)0x410001A7U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR5 (*(RwReg8 *)0x410001A8U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET5 (*(RwReg8 *)0x410001A9U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG6 (*(RwReg8 *)0x410001C0U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR6 (*(WoReg8 *)0x410001C4U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET6 (*(WoReg8 *)0x410001C5U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS6 (*(RoReg8 *)0x410001C6U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG6 (*(RwReg8 *)0x410001C7U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR6 (*(RwReg8 *)0x410001C8U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET6 (*(RwReg8 *)0x410001C9U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG7 (*(RwReg8 *)0x410001E0U) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR7 (*(WoReg8 *)0x410001E4U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET7 (*(WoReg8 *)0x410001E5U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS7 (*(RoReg8 *)0x410001E6U) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG7 (*(RwReg8 *)0x410001E7U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR7 (*(RwReg8 *)0x410001E8U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET7 (*(RwReg8 *)0x410001E9U) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 7 */
#define REG_USB_HOST_CTRLB         (*(RwReg16*)0x41000008U) /**< \brief (USB) HOST Control B */
#define REG_USB_HOST_HSOFC         (*(RwReg8 *)0x4100000AU) /**< \brief (USB) HOST Host Start Of Frame Control */
#define REG_USB_HOST_STATUS        (*(RwReg8 *)0x4100000CU) /**< \brief (USB) HOST Status */
#define REG_USB_HOST_FNUM          (*(RwReg16*)0x41000010U) /**< \brief (USB) HOST Host Frame Number */
#define REG_USB_HOST_FLENHIGH      (*(RoReg8 *)0x41000012U) /**< \brief (USB) HOST Host Frame Length */
#define REG_USB_HOST_INTENCLR      (*(RwReg16*)0x41000014U) /**< \brief (USB) HOST Host Interrupt Enable Clear */
#define REG_USB_HOST_INTENSET      (*(RwReg16*)0x41000018U) /**< \brief (USB) HOST Host Interrupt Enable Set */
#define REG_USB_HOST_INTFLAG       (*(RwReg16*)0x4100001CU) /**< \brief (USB) HOST Host Interrupt Flag */
#define REG_USB_HOST_PINTSMRY      (*(RoReg16*)0x41000020U) /**< \brief (USB) HOST Pipe Interrupt Summary */
#define REG_USB_HOST_PIPE_PCFG0    (*(RwReg8 *)0x41000100U) /**< \brief (USB) HOST_PIPE End Point Configuration 0 */
#define REG_USB_HOST_PIPE_BINTERVAL0 (*(RwReg8 *)0x41000103U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 0 */
#define REG_USB_HOST_PIPE_PSTATUSCLR0 (*(WoReg8 *)0x41000104U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 0 */
#define REG_USB_HOST_PIPE_PSTATUSSET0 (*(WoReg8 *)0x41000105U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 0 */
#define REG_USB_HOST_PIPE_PSTATUS0 (*(RoReg8 *)0x41000106U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 0 */
#define REG_USB_HOST_PIPE_PINTFLAG0 (*(RwReg8 *)0x41000107U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 0 */
#define REG_USB_HOST_PIPE_PINTENCLR0 (*(RwReg8 *)0x41000108U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 0 */
#define REG_USB_HOST_PIPE_PINTENSET0 (*(RwReg8 *)0x41000109U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 0 */
#define REG_USB_HOST_PIPE_PCFG1    (*(RwReg8 *)0x41000120U) /**< \brief (USB) HOST_PIPE End Point Configuration 1 */
#define REG_USB_HOST_PIPE_BINTERVAL1 (*(RwReg8 *)0x41000123U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 1 */
#define REG_USB_HOST_PIPE_PSTATUSCLR1 (*(WoReg8 *)0x41000124U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 1 */
#define REG_USB_HOST_PIPE_PSTATUSSET1 (*(WoReg8 *)0x41000125U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 1 */
#define REG_USB_HOST_PIPE_PSTATUS1 (*(RoReg8 *)0x41000126U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 1 */
#define REG_USB_HOST_PIPE_PINTFLAG1 (*(RwReg8 *)0x41000127U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 1 */
#define REG_USB_HOST_PIPE_PINTENCLR1 (*(RwReg8 *)0x41000128U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 1 */
#define REG_USB_HOST_PIPE_PINTENSET1 (*(RwReg8 *)0x41000129U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 1 */
#define REG_USB_HOST_PIPE_PCFG2    (*(RwReg8 *)0x41000140U) /**< \brief (USB) HOST_PIPE End Point Configuration 2 */
#define REG_USB_HOST_PIPE_BINTERVAL2 (*(RwReg8 *)0x41000143U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 2 */
#define REG_USB_HOST_PIPE_PSTATUSCLR2 (*(WoReg8 *)0x41000144U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 2 */
#define REG_USB_HOST_PIPE_PSTATUSSET2 (*(WoReg8 *)0x41000145U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 2 */
#define REG_USB_HOST_PIPE_PSTATUS2 (*(RoReg8 *)0x41000146U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 2 */
#define REG_USB_HOST_PIPE_PINTFLAG2 (*(RwReg8 *)0x41000147U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 2 */
#define REG_USB_HOST_PIPE_PINTENCLR2 (*(RwReg8 *)0x41000148U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 2 */
#define REG_USB_HOST_PIPE_PINTENSET2 (*(RwReg8 *)0x41000149U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 2 */
#define REG_USB_HOST_PIPE_PCFG3    (*(RwReg8 *)0x41000160U) /**< \brief (USB) HOST_PIPE End Point Configuration 3 */
#define REG_USB_HOST_PIPE_BINTERVAL3 (*(RwReg8 *)0x41000163U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 3 */
#define REG_USB_HOST_PIPE_PSTATUSCLR3 (*(WoReg8 *)0x41000164U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 3 */
#define REG_USB_HOST_PIPE_PSTATUSSET3 (*(WoReg8 *)0x41000165U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 3 */
#define REG_USB_HOST_PIPE_PSTATUS3 (*(RoReg8 *)0x41000166U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 3 */
#define REG_USB_HOST_PIPE_PINTFLAG3 (*(RwReg8 *)0x41000167U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 3 */
#define REG_USB_HOST_PIPE_PINTENCLR3 (*(RwReg8 *)0x41000168U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 3 */
#define REG_USB_HOST_PIPE_PINTENSET3 (*(RwReg8 *)0x41000169U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 3 */
#define REG_USB_HOST_PIPE_PCFG4    (*(RwReg8 *)0x41000180U) /**< \brief (USB) HOST_PIPE End Point Configuration 4 */
#define REG_USB_HOST_PIPE_BINTERVAL4 (*(RwReg8 *)0x41000183U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 4 */
#define REG_USB_HOST_PIPE_PSTATUSCLR4 (*(WoReg8 *)0x41000184U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 4 */
#define REG_USB_HOST_PIPE_PSTATUSSET4 (*(WoReg8 *)0x41000185U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 4 */
#define REG_USB_HOST_PIPE_PSTATUS4 (*(RoReg8 *)0x41000186U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 4 */
#define REG_USB_HOST_PIPE_PINTFLAG4 (*(RwReg8 *)0x41000187U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 4 */
#define REG_USB_HOST_PIPE_PINTENCLR4 (*(RwReg8 *)0x41000188U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 4 */
#define REG_USB_HOST_PIPE_PINTENSET4 (*(RwReg8 *)0x41000189U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 4 */
#define REG_USB_HOST_PIPE_PCFG5    (*(RwReg8 *)0x410001A0U) /**< \brief (USB) HOST_PIPE End Point Configuration 5 */
#define REG_USB_HOST_PIPE_BINTERVAL5 (*(RwReg8 *)0x410001A3U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 5 */
#define REG_USB_HOST_PIPE_PSTATUSCLR5 (*(WoReg8 *)0x410001A4U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 5 */
#define REG_USB_HOST_PIPE_PSTATUSSET5 (*(WoReg8 *)0x410001A5U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 5 */
#define REG_USB_HOST_PIPE_PSTATUS5 (*(RoReg8 *)0x410001A6U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 5 */
#define REG_USB_HOST_PIPE_PINTFLAG5 (*(RwReg8 *)0x410001A7U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 5 */
#define REG_USB_HOST_PIPE_PINTENCLR5 (*(RwReg8 *)0x410001A8U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 5 */
#define REG_USB_HOST_PIPE_PINTENSET5 (*(RwReg8 *)0x410001A9U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 5 */
#define REG_USB_HOST_PIPE_PCFG6    (*(RwReg8 *)0x410001C0U) /**< \brief (USB) HOST_PIPE End Point Configuration 6 */
#define REG_USB_HOST_PIPE_BINTERVAL6 (*(RwReg8 *)0x410001C3U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 6 */
#define REG_USB_HOST_PIPE_PSTATUSCLR6 (*(WoReg8 *)0x410001C4U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 6 */
#define REG_USB_HOST_PIPE_PSTATUSSET6 (*(WoReg8 *)0x410001C5U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 6 */
#define REG_USB_HOST_PIPE_PSTATUS6 (*(RoReg8 *)0x410001C6U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 6 */
#define REG_USB_HOST_PIPE_PINTFLAG6 (*(RwReg8 *)0x410001C7U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 6 */
#define REG_USB_HOST_PIPE_PINTENCLR6 (*(RwReg8 *)0x410001C8U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 6 */
#define REG_USB_HOST_PIPE_PINTENSET6 (*(RwReg8 *)0x410001C9U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 6 */
#define REG_USB_HOST_PIPE_PCFG7    (*(RwReg8 *)0x410001E0U) /**< \brief (USB) HOST_PIPE End Point Configuration 7 */
#define REG_USB_HOST_PIPE_BINTERVAL7 (*(RwReg8 *)0x410001E3U) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 7 */
#define REG_USB_HOST_PIPE_PSTATUSCLR7 (*(WoReg8 *)0x410001E4U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 7 */
#define REG_USB_HOST_PIPE_PSTATUSSET7 (*(WoReg8 *)0x410001E5U) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 7 */
#define REG_USB_HOST_PIPE_PSTATUS7 (*(RoReg8 *)0x410001E6U) /**< \brief (USB) HOST_PIPE End Point Pipe Status 7 */
#define REG_USB_HOST_PIPE_PINTFLAG7 (*(RwReg8 *)0x410001E7U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 7 */
#define REG_USB_HOST_PIPE_PINTENCLR7 (*(RwReg8 *)0x410001E8U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 7 */
#define REG_USB_HOST_PIPE_PINTENSET7 (*(RwReg8 *)0x410001E9U) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 7 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for USB peripheral ========== */
#define USB_EPT_NBR                 8        // Number of USB end points (obsolete)
#define USB_EPT_NUM                 8        // Number of USB end points
#define USB_GCLK_ID                 4        // Index of Generic Clock
#define USB_PIPE_NUM                8        // Number of USB pipes

#endif /* _SAML21_USB_INSTANCE_ */
