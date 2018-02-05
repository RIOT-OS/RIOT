/******************************************************************************
*  Filename:       hw_sys_ctrl.h
*  Revised:        $Date: 2013-04-12 15:10:54 +0200 (Fri, 12 Apr 2013) $
*  Revision:       $Revision: 9735 $
*
*  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef __HW_SYS_CTRL_H__
#define __HW_SYS_CTRL_H__

//*****************************************************************************
//
// The following are defines for the SYS_CTRL register offsets.
//
//*****************************************************************************
#define SYS_CTRL_CLOCK_CTRL     0x400D2000  // The clock control register 
                                            // handels clock settings in the 
                                            // CC2538. The settings in 
                                            // CLOCK_CTRL do not always reflect 
                                            // the current chip status which is 
                                            // found in CLOCK_STA register. 
#define SYS_CTRL_CLOCK_STA      0x400D2004  // Clock status register This 
                                            // register reflects the current 
                                            // chip status. 
#define SYS_CTRL_RCGCGPT        0x400D2008  // This register defines the 
                                            // module clocks for GPT[3:0] when 
                                            // the CPU is in active (run) mode. 
                                            // This register setting is don't 
                                            // care for PM1-3, because the 
                                            // system clock is powered down in 
                                            // these modes. 
#define SYS_CTRL_SCGCGPT        0x400D200C  // This register defines the 
                                            // module clocks for GPT[3:0] when 
                                            // the CPU is in sleep mode. This 
                                            // register setting is don't care 
                                            // for PM1-3, because the system 
                                            // clock is powered down in these 
                                            // modes. 
#define SYS_CTRL_DCGCGPT        0x400D2010  // This register defines the 
                                            // module clocks for GPT[3:0] when 
                                            // the CPU is in PM0. This register 
                                            // setting is don't care for PM1-3, 
                                            // because the system clock is 
                                            // powered down in these modes. 
#define SYS_CTRL_SRGPT          0x400D2014  // This register controls the 
                                            // reset for GPT[3:0]. 
#define SYS_CTRL_RCGCSSI        0x400D2018  // This register defines the 
                                            // module clocks for SSI[1:0] when 
                                            // the CPU is in active (run) mode. 
                                            // This register setting is don't 
                                            // care for PM1-3, because the 
                                            // system clock is powered down in 
                                            // these modes. 
#define SYS_CTRL_SCGCSSI        0x400D201C  // This register defines the 
                                            // module clocks for SSI[1:0] when 
                                            // the CPU is insSleep mode. This 
                                            // register setting is don't care 
                                            // for PM1-3, because the system 
                                            // clock is powered down in these 
                                            // modes. 
#define SYS_CTRL_DCGCSSI        0x400D2020  // This register defines the 
                                            // module clocks for SSI[1:0] when 
                                            // the CPU is in PM0. This register 
                                            // setting is don't care for PM1-3, 
                                            // because the system clock is 
                                            // powered down in these modes. 
#define SYS_CTRL_SRSSI          0x400D2024  // This register controls the 
                                            // reset for SSI[1:0]. 
#define SYS_CTRL_RCGCUART       0x400D2028  // This register defines the 
                                            // module clocks for UART[1:0] when 
                                            // the CPU is in active (run) mode. 
                                            // This register setting is don't 
                                            // care for PM1-3, because the 
                                            // system clock is powered down in 
                                            // these modes. 
#define SYS_CTRL_SCGCUART       0x400D202C  // This register defines the 
                                            // module clocks for UART[1:0] when 
                                            // the CPU is in sleep mode. This 
                                            // register setting is don't care 
                                            // for PM1-3, because the system 
                                            // clock is powered down in these 
                                            // modes. 
#define SYS_CTRL_DCGCUART       0x400D2030  // This register defines the 
                                            // module clocks for UART[1:0] when 
                                            // the CPU is in PM0. This register 
                                            // setting is don't care for PM1-3, 
                                            // because the system clock is 
                                            // powered down in these modes. 
#define SYS_CTRL_SRUART         0x400D2034  // This register controls the 
                                            // reset for UART[1:0]. 
#define SYS_CTRL_RCGCI2C        0x400D2038  // This register defines the 
                                            // module clocks for I2C when the 
                                            // CPU is in active (run) mode. 
                                            // This register setting is don't 
                                            // care for PM1-3, because the 
                                            // system clock is powered down in 
                                            // these modes. 
#define SYS_CTRL_SCGCI2C        0x400D203C  // This register defines the 
                                            // module clocks for I2C when the 
                                            // CPU is in sleep mode. This 
                                            // register setting is don't care 
                                            // for PM1-3, because the system 
                                            // clock is powered down in these 
                                            // modes. 
#define SYS_CTRL_DCGCI2C        0x400D2040  // This register defines the 
                                            // module clocks for I2C when the 
                                            // CPU is in PM0. This register 
                                            // setting is don't care for PM1-3, 
                                            // because the system clock is 
                                            // powered down in these modes. 
#define SYS_CTRL_SRI2C          0x400D2044  // This register controls the 
                                            // reset for I2C. 
#define SYS_CTRL_RCGCSEC        0x400D2048  // This register defines the 
                                            // module clocks for the security 
                                            // module when the CPU is in active 
                                            // (run) mode. This register 
                                            // setting is don't care for PM1-3, 
                                            // because the system clock is 
                                            // powered down in these modes. 
#define SYS_CTRL_SCGCSEC        0x400D204C  // This register defines the 
                                            // module clocks for the security 
                                            // module when the CPU is in sleep 
                                            // mode. This register setting is 
                                            // don't care for PM1-3, because 
                                            // the system clock is powered down 
                                            // in these modes. 
#define SYS_CTRL_DCGCSEC        0x400D2050  // This register defines the 
                                            // module clocks for the security 
                                            // module when the CPU is in PM0. 
                                            // This register setting is don't 
                                            // care for PM1-3, because the 
                                            // system clock is powered down in 
                                            // these modes. 
#define SYS_CTRL_SRSEC          0x400D2054  // This register controls the 
                                            // reset for the security module. 
#define SYS_CTRL_PMCTL          0x400D2058  // This register controls the 
                                            // power mode. Note: The 
                                            // Corresponding PM is not entered 
                                            // before the WFI instruction is 
                                            // asserted. To enter PM1-3 the 
                                            // DEEPSLEEP bit in SYSCTRL must be 
                                            // 1. 
#define SYS_CTRL_SRCRC          0x400D205C  // This register controls CRC on 
                                            // state retention. 
#define SYS_CTRL_PWRDBG         0x400D2074  // Power debug register 
#define SYS_CTRL_CLD            0x400D2080  // This register controls the 
                                            // clock loss detection feature. 
#define SYS_CTRL_IWE            0x400D2094  // This register controls 
                                            // interrupt wake-up. 
#define SYS_CTRL_I_MAP          0x400D2098  // This register selects which 
                                            // interrupt map to be used. 
#define SYS_CTRL_RCGCRFC        0x400D20A8  // This register defines the 
                                            // module clocks for RF CORE when 
                                            // the CPU is in active (run) mode. 
                                            // This register setting is don't 
                                            // care for PM1-3, because the 
                                            // system clock is powered down in 
                                            // these modes. 
#define SYS_CTRL_SCGCRFC        0x400D20AC  // This register defines the 
                                            // module clocks for RF CORE when 
                                            // the CPU is in sleep mode. This 
                                            // register setting is don't care 
                                            // for PM1-3, because the system 
                                            // clock is powered down in these 
                                            // modes. 
#define SYS_CTRL_DCGCRFC        0x400D20B0  // This register defines the 
                                            // module clocks for RF CORE when 
                                            // the CPU is in PM0. This register 
                                            // setting is don't care for PM1-3, 
                                            // because the system clock is 
                                            // powered down in these modes. 
#define SYS_CTRL_EMUOVR         0x400D20B4  // This register defines the 
                                            // emulator override controls for 
                                            // power mode and peripheral clock 
                                            // gate. 


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_CLOCK_CTRL register.
//
//*****************************************************************************
#define SYS_CTRL_CLOCK_CTRL_OSC32K_CALDIS \
                                0x02000000  // Disable calibration 32-kHz RC 
                                            // oscillator. 0: Enable 
                                            // calibration 1: Disable 
                                            // calibration 

#define SYS_CTRL_CLOCK_CTRL_OSC32K_CALDIS_M \
                                0x02000000
#define SYS_CTRL_CLOCK_CTRL_OSC32K_CALDIS_S 25
#define SYS_CTRL_CLOCK_CTRL_OSC32K \
                                0x01000000  // 32-kHz clock oscillator 
                                            // selection 0: 32-kHz crystal 
                                            // oscillator 1: 32-kHz RC 
                                            // oscillator 

#define SYS_CTRL_CLOCK_CTRL_OSC32K_M \
                                0x01000000
#define SYS_CTRL_CLOCK_CTRL_OSC32K_S 24
#define SYS_CTRL_CLOCK_CTRL_AMP_DET \
                                0x00200000  // Amplitude detector of XOSC 
                                            // during power up 0: No action 1: 
                                            // Delay qualification of XOSC 
                                            // until amplitude is greater than 
                                            // the threshold. 

#define SYS_CTRL_CLOCK_CTRL_AMP_DET_M \
                                0x00200000
#define SYS_CTRL_CLOCK_CTRL_AMP_DET_S 21
#define SYS_CTRL_CLOCK_CTRL_OSC_PD \
                                0x00020000  // 0: Power up both oscillators 1: 
                                            // Power down oscillator not 
                                            // selected by OSC bit 
                                            // (hardware-controlled when 
                                            // selected). 

#define SYS_CTRL_CLOCK_CTRL_OSC_PD_M \
                                0x00020000
#define SYS_CTRL_CLOCK_CTRL_OSC_PD_S 17
#define SYS_CTRL_CLOCK_CTRL_OSC 0x00010000  // System clock oscillator 
                                            // selection 0: 32-MHz crystal 
                                            // oscillator 1: 16-MHz HF-RC 
                                            // oscillator 
#define SYS_CTRL_CLOCK_CTRL_OSC_M \
                                0x00010000
#define SYS_CTRL_CLOCK_CTRL_OSC_S 16
#define SYS_CTRL_CLOCK_CTRL_IO_DIV_M \
                                0x00000700  // I/O clock rate setting Cannot 
                                            // be higher than OSC setting 000: 
                                            // 32 MHz 001: 16 MHz 010: 8 MHz 
                                            // 011: 4 MHz 100: 2 MHz 101: 1 MHz 
                                            // 110: 0.5 MHz 111: 0.25 MHz 

#define SYS_CTRL_CLOCK_CTRL_IO_DIV_S 8
#define SYS_CTRL_CLOCK_CTRL_SYS_DIV_M \
                                0x00000007  // System clock rate setting 
                                            // Cannot be higher than OSC 
                                            // setting 000: 32 MHz 001: 16 MHz 
                                            // 010: 8 MHz 011: 4 MHz 100: 2 MHz 
                                            // 101: 1 MHz 110: 0.5 MHz 111: 
                                            // 0.25 MHz 

#define SYS_CTRL_CLOCK_CTRL_SYS_DIV_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_CLOCK_STA register.
//
//*****************************************************************************
#define SYS_CTRL_CLOCK_STA_SYNC_32K \
                                0x04000000  // 32-kHz clock source synced to 
                                            // undivided system clock (16 or 32 
                                            // MHz). 

#define SYS_CTRL_CLOCK_STA_SYNC_32K_M \
                                0x04000000
#define SYS_CTRL_CLOCK_STA_SYNC_32K_S 26
#define SYS_CTRL_CLOCK_STA_OSC32K_CALDIS \
                                0x02000000  // Disable calibration 32-kHz RC 
                                            // oscillator. 0: Calibration 
                                            // enabled 1: Calibration disabled 

#define SYS_CTRL_CLOCK_STA_OSC32K_CALDIS_M \
                                0x02000000
#define SYS_CTRL_CLOCK_STA_OSC32K_CALDIS_S 25
#define SYS_CTRL_CLOCK_STA_OSC32K \
                                0x01000000  // Current 32-kHz clock oscillator 
                                            // selected. 0: 32-kHz crystal 
                                            // oscillator 1: 32-kHz RC 
                                            // oscillator 

#define SYS_CTRL_CLOCK_STA_OSC32K_M \
                                0x01000000
#define SYS_CTRL_CLOCK_STA_OSC32K_S 24
#define SYS_CTRL_CLOCK_STA_RST_M \
                                0x00C00000  // Returns last source of reset 
                                            // 00: POR 01: External reset 10: 
                                            // WDT 11: CLD or software reset 

#define SYS_CTRL_CLOCK_STA_RST_S 22
#define SYS_CTRL_CLOCK_STA_SOURCE_CHANGE \
                                0x00100000  // 0: System clock is not 
                                            // requested to change. 1: A change 
                                            // of system clock source has been 
                                            // initiated and is not finished. 
                                            // Same as when OSC bit in 
                                            // CLOCK_STA and CLOCK_CTRL 
                                            // register are not equal 

#define SYS_CTRL_CLOCK_STA_SOURCE_CHANGE_M \
                                0x00100000
#define SYS_CTRL_CLOCK_STA_SOURCE_CHANGE_S 20
#define SYS_CTRL_CLOCK_STA_XOSC_STB \
                                0x00080000  // XOSC stable status 0: XOSC is 
                                            // not powered up or not yet 
                                            // stable. 1: XOSC is powered up 
                                            // and stable. 

#define SYS_CTRL_CLOCK_STA_XOSC_STB_M \
                                0x00080000
#define SYS_CTRL_CLOCK_STA_XOSC_STB_S 19
#define SYS_CTRL_CLOCK_STA_HSOSC_STB \
                                0x00040000  // HSOSC stable status 0: HSOSC is 
                                            // not powered up or not yet 
                                            // stable. 1: HSOSC is powered up 
                                            // and stable. 

#define SYS_CTRL_CLOCK_STA_HSOSC_STB_M \
                                0x00040000
#define SYS_CTRL_CLOCK_STA_HSOSC_STB_S 18
#define SYS_CTRL_CLOCK_STA_OSC_PD \
                                0x00020000  // 0: Both oscillators powered up 
                                            // and stable and OSC_PD_CMD = 0. 
                                            // 1: Oscillator not selected by 
                                            // CLOCK_CTRL.OSC bit is powered 
                                            // down. 

#define SYS_CTRL_CLOCK_STA_OSC_PD_M \
                                0x00020000
#define SYS_CTRL_CLOCK_STA_OSC_PD_S 17
#define SYS_CTRL_CLOCK_STA_OSC  0x00010000  // Current clock source selected 
                                            // 0: 32-MHz crystal oscillator 1: 
                                            // 16-MHz HF-RC oscillator 
#define SYS_CTRL_CLOCK_STA_OSC_M \
                                0x00010000
#define SYS_CTRL_CLOCK_STA_OSC_S 16
#define SYS_CTRL_CLOCK_STA_IO_DIV_M \
                                0x00000700  // Returns current functional 
                                            // frequency for IO_CLK (may differ 
                                            // from setting in the CLOCK_CTRL 
                                            // register) 000: 32 MHz 001: 16 
                                            // MHz 010: 8 MHz 011: 4 MHz 100: 2 
                                            // MHz 101: 1 MHz 110: 0.5 MHz 111: 
                                            // 0.25 MHz 

#define SYS_CTRL_CLOCK_STA_IO_DIV_S 8
#define SYS_CTRL_CLOCK_STA_RTCLK_FREQ_M \
                                0x00000018  // Returns current functional 
                                            // frequency for real-time clock. 
                                            // (may differ from setting in the 
                                            // CLOCK_CTRL register) 1x : 8 MHz 
                                            // 01: 2 MHz 00: 62.5 kHz 

#define SYS_CTRL_CLOCK_STA_RTCLK_FREQ_S 3
#define SYS_CTRL_CLOCK_STA_SYS_DIV_M \
                                0x00000007  // Returns current functional 
                                            // frequency for system clock (may 
                                            // differ from setting in the 
                                            // CLOCK_CTRL register) 000: 32 MHz 
                                            // 001: 16 MHz 010: 8 MHz 011: 4 
                                            // MHz 100: 2 MHz 101: 1 MHz 110: 
                                            // 0.5 MHz 111: 0.25 MHz 

#define SYS_CTRL_CLOCK_STA_SYS_DIV_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_RCGCGPT register.
//
//*****************************************************************************
#define SYS_CTRL_RCGCGPT_GPT3   0x00000008  // 0: Clock for GPT3 is gated. 1: 
                                            // Clock for GPT3 is enabled. 
#define SYS_CTRL_RCGCGPT_GPT3_M 0x00000008
#define SYS_CTRL_RCGCGPT_GPT3_S 3
#define SYS_CTRL_RCGCGPT_GPT2   0x00000004  // 0: Clock for GPT2 is gated. 1: 
                                            // Clock for GPT2 is enabled. 
#define SYS_CTRL_RCGCGPT_GPT2_M 0x00000004
#define SYS_CTRL_RCGCGPT_GPT2_S 2
#define SYS_CTRL_RCGCGPT_GPT1   0x00000002  // 0: Clock for GPT1 is gated. 1: 
                                            // Clock for GPT1 is enabled. 
#define SYS_CTRL_RCGCGPT_GPT1_M 0x00000002
#define SYS_CTRL_RCGCGPT_GPT1_S 1
#define SYS_CTRL_RCGCGPT_GPT0   0x00000001  // 0: Clock for GPT0 is gated. 1: 
                                            // Clock for GPT0 is enabled. 
#define SYS_CTRL_RCGCGPT_GPT0_M 0x00000001
#define SYS_CTRL_RCGCGPT_GPT0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SCGCGPT register.
//
//*****************************************************************************
#define SYS_CTRL_SCGCGPT_GPT3   0x00000008  // 0: Clock for GPT3 is gated. 1: 
                                            // Clock for GPT3 is enabled. 
#define SYS_CTRL_SCGCGPT_GPT3_M 0x00000008
#define SYS_CTRL_SCGCGPT_GPT3_S 3
#define SYS_CTRL_SCGCGPT_GPT2   0x00000004  // 0: Clock for GPT2 is gated. 1: 
                                            // Clock for GPT2 is enabled. 
#define SYS_CTRL_SCGCGPT_GPT2_M 0x00000004
#define SYS_CTRL_SCGCGPT_GPT2_S 2
#define SYS_CTRL_SCGCGPT_GPT1   0x00000002  // 0: Clock for GPT1 is gated. 1: 
                                            // Clock for GPT1 is enabled. 
#define SYS_CTRL_SCGCGPT_GPT1_M 0x00000002
#define SYS_CTRL_SCGCGPT_GPT1_S 1
#define SYS_CTRL_SCGCGPT_GPT0   0x00000001  // 0: Clock for GPT0 is gated. 1: 
                                            // Clock for GPT0 is enabled. 
#define SYS_CTRL_SCGCGPT_GPT0_M 0x00000001
#define SYS_CTRL_SCGCGPT_GPT0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_DCGCGPT register.
//
//*****************************************************************************
#define SYS_CTRL_DCGCGPT_GPT3   0x00000008  // 0: Clock for GPT3 is gated. 1: 
                                            // Clock for GPT3 is enabled. 
#define SYS_CTRL_DCGCGPT_GPT3_M 0x00000008
#define SYS_CTRL_DCGCGPT_GPT3_S 3
#define SYS_CTRL_DCGCGPT_GPT2   0x00000004  // 0: Clock for GPT2 is gated. 1: 
                                            // Clock for GPT2 is enabled. 
#define SYS_CTRL_DCGCGPT_GPT2_M 0x00000004
#define SYS_CTRL_DCGCGPT_GPT2_S 2
#define SYS_CTRL_DCGCGPT_GPT1   0x00000002  // 0: Clock for GPT1 is gated. 1: 
                                            // Clock for GPT1 is enabled. 
#define SYS_CTRL_DCGCGPT_GPT1_M 0x00000002
#define SYS_CTRL_DCGCGPT_GPT1_S 1
#define SYS_CTRL_DCGCGPT_GPT0   0x00000001  // 0: Clock for GPT0 is gated. 1: 
                                            // Clock for GPT0 is enabled. 
#define SYS_CTRL_DCGCGPT_GPT0_M 0x00000001
#define SYS_CTRL_DCGCGPT_GPT0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SRGPT register.
//
//*****************************************************************************
#define SYS_CTRL_SRGPT_GPT3     0x00000008  // 0: GPT3 module is not reset 1: 
                                            // GPT3 module is reset 
#define SYS_CTRL_SRGPT_GPT3_M   0x00000008
#define SYS_CTRL_SRGPT_GPT3_S   3
#define SYS_CTRL_SRGPT_GPT2     0x00000004  // 0: GPT2 module is not reset 1: 
                                            // GPT2 module is reset 
#define SYS_CTRL_SRGPT_GPT2_M   0x00000004
#define SYS_CTRL_SRGPT_GPT2_S   2
#define SYS_CTRL_SRGPT_GPT1     0x00000002  // 0: GPT1 module is not reset 1: 
                                            // GPT1 module is reset 
#define SYS_CTRL_SRGPT_GPT1_M   0x00000002
#define SYS_CTRL_SRGPT_GPT1_S   1
#define SYS_CTRL_SRGPT_GPT0     0x00000001  // 0: GPT0 module is not reset 1: 
                                            // GPT0 module is reset 
#define SYS_CTRL_SRGPT_GPT0_M   0x00000001
#define SYS_CTRL_SRGPT_GPT0_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_RCGCSSI register.
//
//*****************************************************************************
#define SYS_CTRL_RCGCSSI_SSI1   0x00000002  // 0: Clock for SSI1 is gated. 1: 
                                            // Clock for SSI1 is enabled. 
#define SYS_CTRL_RCGCSSI_SSI1_M 0x00000002
#define SYS_CTRL_RCGCSSI_SSI1_S 1
#define SYS_CTRL_RCGCSSI_SSI0   0x00000001  // 0: Clock for SSI0 is gated. 1: 
                                            // Clock for SSI0 is enabled. 
#define SYS_CTRL_RCGCSSI_SSI0_M 0x00000001
#define SYS_CTRL_RCGCSSI_SSI0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SCGCSSI register.
//
//*****************************************************************************
#define SYS_CTRL_SCGCSSI_SSI1   0x00000002  // 0: Clock for SSI1 is gated. 1: 
                                            // Clock for SSI1 is enabled. 
#define SYS_CTRL_SCGCSSI_SSI1_M 0x00000002
#define SYS_CTRL_SCGCSSI_SSI1_S 1
#define SYS_CTRL_SCGCSSI_SSI0   0x00000001  // 0: Clock for SSI0 is gated. 1: 
                                            // Clock for SSI0 is enabled. 
#define SYS_CTRL_SCGCSSI_SSI0_M 0x00000001
#define SYS_CTRL_SCGCSSI_SSI0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_DCGCSSI register.
//
//*****************************************************************************
#define SYS_CTRL_DCGCSSI_SSI1   0x00000002  // 0: Clock for SSI1 is gated. 1: 
                                            // Clock for SSI1 is enabled. 
#define SYS_CTRL_DCGCSSI_SSI1_M 0x00000002
#define SYS_CTRL_DCGCSSI_SSI1_S 1
#define SYS_CTRL_DCGCSSI_SSI0   0x00000001  // 0: Clock for SSI0 is gated. 1: 
                                            // Clock for SSI0 is enabled. 
#define SYS_CTRL_DCGCSSI_SSI0_M 0x00000001
#define SYS_CTRL_DCGCSSI_SSI0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SRSSI register.
//
//*****************************************************************************
#define SYS_CTRL_SRSSI_SSI1     0x00000002  // 0: SSI1 module is not reset 1: 
                                            // SSI1 module is reset 
#define SYS_CTRL_SRSSI_SSI1_M   0x00000002
#define SYS_CTRL_SRSSI_SSI1_S   1
#define SYS_CTRL_SRSSI_SSI0     0x00000001  // 0: SSI0 module is not reset 1: 
                                            // SSI0 module is reset 
#define SYS_CTRL_SRSSI_SSI0_M   0x00000001
#define SYS_CTRL_SRSSI_SSI0_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_RCGCUART register.
//
//*****************************************************************************
#define SYS_CTRL_RCGCUART_UART1 0x00000002  // 0: Clock for UART1 is gated. 1: 
                                            // Clock for UART1 is enabled. 
#define SYS_CTRL_RCGCUART_UART1_M \
                                0x00000002
#define SYS_CTRL_RCGCUART_UART1_S 1
#define SYS_CTRL_RCGCUART_UART0 0x00000001  // 0: Clock for UART0 is gated. 1: 
                                            // Clock for UART0 is enabled. 
#define SYS_CTRL_RCGCUART_UART0_M \
                                0x00000001
#define SYS_CTRL_RCGCUART_UART0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SCGCUART register.
//
//*****************************************************************************
#define SYS_CTRL_SCGCUART_UART1 0x00000002  // 0: Clock for UART1 is gated. 1: 
                                            // Clock for UART1 is enabled. 
#define SYS_CTRL_SCGCUART_UART1_M \
                                0x00000002
#define SYS_CTRL_SCGCUART_UART1_S 1
#define SYS_CTRL_SCGCUART_UART0 0x00000001  // 0: Clock for UART0 is gated. 1: 
                                            // Clock for UART0 is enabled. 
#define SYS_CTRL_SCGCUART_UART0_M \
                                0x00000001
#define SYS_CTRL_SCGCUART_UART0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_DCGCUART register.
//
//*****************************************************************************
#define SYS_CTRL_DCGCUART_UART1 0x00000002  // 0: Clock for UART1 is gated. 1: 
                                            // Clock for UART1 is enabled. 
#define SYS_CTRL_DCGCUART_UART1_M \
                                0x00000002
#define SYS_CTRL_DCGCUART_UART1_S 1
#define SYS_CTRL_DCGCUART_UART0 0x00000001  // 0: Clock for UART0 is gated. 1: 
                                            // Clock for UART0 is enabled. 
#define SYS_CTRL_DCGCUART_UART0_M \
                                0x00000001
#define SYS_CTRL_DCGCUART_UART0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SRUART register.
//
//*****************************************************************************
#define SYS_CTRL_SRUART_UART1   0x00000002  // 0: UART1 module is not reset 1: 
                                            // UART1 module is reset 
#define SYS_CTRL_SRUART_UART1_M 0x00000002
#define SYS_CTRL_SRUART_UART1_S 1
#define SYS_CTRL_SRUART_UART0   0x00000001  // 0: UART0 module is not reset 1: 
                                            // UART0 module is reset 
#define SYS_CTRL_SRUART_UART0_M 0x00000001
#define SYS_CTRL_SRUART_UART0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_RCGCI2C register.
//
//*****************************************************************************
#define SYS_CTRL_RCGCI2C_I2C0   0x00000001  // 0: Clock for I2C0 is gated. 1: 
                                            // Clock for I2C0 is enabled. 
#define SYS_CTRL_RCGCI2C_I2C0_M 0x00000001
#define SYS_CTRL_RCGCI2C_I2C0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SCGCI2C register.
//
//*****************************************************************************
#define SYS_CTRL_SCGCI2C_I2C0   0x00000001  // 0: Clock for I2C0 is gated. 1: 
                                            // Clock for I2C0 is enabled. 
#define SYS_CTRL_SCGCI2C_I2C0_M 0x00000001
#define SYS_CTRL_SCGCI2C_I2C0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_DCGCI2C register.
//
//*****************************************************************************
#define SYS_CTRL_DCGCI2C_I2C0   0x00000001  // 0: Clock for I2C0 is gated. 1: 
                                            // Clock for I2C0 is enabled. 
#define SYS_CTRL_DCGCI2C_I2C0_M 0x00000001
#define SYS_CTRL_DCGCI2C_I2C0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SRI2C register.
//
//*****************************************************************************
#define SYS_CTRL_SRI2C_I2C0     0x00000001  // 0: I2C0 module is not reset 1: 
                                            // I2C0 module is reset 
#define SYS_CTRL_SRI2C_I2C0_M   0x00000001
#define SYS_CTRL_SRI2C_I2C0_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_RCGCSEC register.
//
//*****************************************************************************
#define SYS_CTRL_RCGCSEC_AES    0x00000002  // 0: Clock for AES is gated. 1: 
                                            // Clock for AES is enabled. 
#define SYS_CTRL_RCGCSEC_AES_M  0x00000002
#define SYS_CTRL_RCGCSEC_AES_S  1
#define SYS_CTRL_RCGCSEC_PKA    0x00000001  // 0: Clock for PKA is gated. 1: 
                                            // Clock for PKA is enabled. 
#define SYS_CTRL_RCGCSEC_PKA_M  0x00000001
#define SYS_CTRL_RCGCSEC_PKA_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SCGCSEC register.
//
//*****************************************************************************
#define SYS_CTRL_SCGCSEC_AES    0x00000002  // 0: Clock for AES is gated. 1: 
                                            // Clock for AES is enabled. 
#define SYS_CTRL_SCGCSEC_AES_M  0x00000002
#define SYS_CTRL_SCGCSEC_AES_S  1
#define SYS_CTRL_SCGCSEC_PKA    0x00000001  // 0: Clock for PKA is gated. 1: 
                                            // Clock for PKA is enabled. 
#define SYS_CTRL_SCGCSEC_PKA_M  0x00000001
#define SYS_CTRL_SCGCSEC_PKA_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_DCGCSEC register.
//
//*****************************************************************************
#define SYS_CTRL_DCGCSEC_AES    0x00000002  // 0: Clock for AES is gated. 1: 
                                            // Clock for AES is enabled. 
#define SYS_CTRL_DCGCSEC_AES_M  0x00000002
#define SYS_CTRL_DCGCSEC_AES_S  1
#define SYS_CTRL_DCGCSEC_PKA    0x00000001  // 0: Clock for PKA is gated. 1: 
                                            // Clock for PKA is enabled. 
#define SYS_CTRL_DCGCSEC_PKA_M  0x00000001
#define SYS_CTRL_DCGCSEC_PKA_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SRSEC register.
//
//*****************************************************************************
#define SYS_CTRL_SRSEC_AES      0x00000002  // 0: AES module is not reset 1: 
                                            // AES module is reset 
#define SYS_CTRL_SRSEC_AES_M    0x00000002
#define SYS_CTRL_SRSEC_AES_S    1
#define SYS_CTRL_SRSEC_PKA      0x00000001  // 0: PKA module is not reset 1: 
                                            // PKA module is reset 
#define SYS_CTRL_SRSEC_PKA_M    0x00000001
#define SYS_CTRL_SRSEC_PKA_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_PMCTL register.
//
//*****************************************************************************
#define SYS_CTRL_PMCTL_PM_M     0x00000003  // 00: No action 01: PM1 10: PM2 
                                            // 11: PM3 
#define SYS_CTRL_PMCTL_PM_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SRCRC register.
//
//*****************************************************************************
#define SYS_CTRL_SRCRC_CRC_REN_USB \
                                0x00000100  // 1: Enable reset of chip if CRC 
                                            // fails. 0: Disable reset feature 
                                            // of chip due to CRC. 

#define SYS_CTRL_SRCRC_CRC_REN_USB_M \
                                0x00000100
#define SYS_CTRL_SRCRC_CRC_REN_USB_S 8
#define SYS_CTRL_SRCRC_CRC_REN_RF \
                                0x00000001  // 1: Enable reset of chip if CRC 
                                            // fails. 0: Disable reset feature 
                                            // of chip due to CRC. 

#define SYS_CTRL_SRCRC_CRC_REN_RF_M \
                                0x00000001
#define SYS_CTRL_SRCRC_CRC_REN_RF_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_PWRDBG register.
//
//*****************************************************************************
#define SYS_CTRL_PWRDBG_FORCE_WARM_RESET \
                                0x00000008  // 0: No action 1: When written 
                                            // high, the chip is reset in the 
                                            // same manner as a CLD event and 
                                            // is readable from the RST field 
                                            // in the CLOCK_STA register. 

#define SYS_CTRL_PWRDBG_FORCE_WARM_RESET_M \
                                0x00000008
#define SYS_CTRL_PWRDBG_FORCE_WARM_RESET_S 3
//*****************************************************************************
//
// The following are defines for the bit fields in the SYS_CTRL_CLD register.
//
//*****************************************************************************
#define SYS_CTRL_CLD_VALID      0x00000100  // 0: CLD status in always-on 
                                            // domain is not equal to status in 
                                            // the EN register. 1: CLD status 
                                            // in always-on domain and EN 
                                            // register are equal. 
#define SYS_CTRL_CLD_VALID_M    0x00000100
#define SYS_CTRL_CLD_VALID_S    8
#define SYS_CTRL_CLD_EN         0x00000001  // 0: CLD is disabled. 1: CLD is 
                                            // enabled. Writing to this 
                                            // register shall be ignored if 
                                            // VALID = 0 
#define SYS_CTRL_CLD_EN_M       0x00000001
#define SYS_CTRL_CLD_EN_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the SYS_CTRL_IWE register.
//
//*****************************************************************************
#define SYS_CTRL_IWE_SM_TIMER_IWE \
                                0x00000020  // 1: Enable SM Timer wake-up 
                                            // interrupt. 0: Disable SM Timer 
                                            // wake-up interrupt. 

#define SYS_CTRL_IWE_SM_TIMER_IWE_M \
                                0x00000020
#define SYS_CTRL_IWE_SM_TIMER_IWE_S 5
#define SYS_CTRL_IWE_USB_IWE    0x00000010  // 1: Enable USB wake-up 
                                            // interrupt. 0: Disable USB 
                                            // wake-up interrupt. 
#define SYS_CTRL_IWE_USB_IWE_M  0x00000010
#define SYS_CTRL_IWE_USB_IWE_S  4
#define SYS_CTRL_IWE_PORT_D_IWE 0x00000008  // 1: Enable port D wake-up 
                                            // interrupt. 0: Disable port D 
                                            // wake-up interrupt. 
#define SYS_CTRL_IWE_PORT_D_IWE_M \
                                0x00000008
#define SYS_CTRL_IWE_PORT_D_IWE_S 3
#define SYS_CTRL_IWE_PORT_C_IWE 0x00000004  // 1: Enable port C wake-up 
                                            // interrupt. 0: Disable port C 
                                            // wake-up interrupt. 
#define SYS_CTRL_IWE_PORT_C_IWE_M \
                                0x00000004
#define SYS_CTRL_IWE_PORT_C_IWE_S 2
#define SYS_CTRL_IWE_PORT_B_IWE 0x00000002  // 1: Enable port B wake-up 
                                            // interrupt. 0: Disable port B 
                                            // wake-up interrupt. 
#define SYS_CTRL_IWE_PORT_B_IWE_M \
                                0x00000002
#define SYS_CTRL_IWE_PORT_B_IWE_S 1
#define SYS_CTRL_IWE_PORT_A_IWE 0x00000001  // 1: Enable port A wake-up 
                                            // interrupt. 0: Disable port A 
                                            // wake-up interrupt. 
#define SYS_CTRL_IWE_PORT_A_IWE_M \
                                0x00000001
#define SYS_CTRL_IWE_PORT_A_IWE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_I_MAP register.
//
//*****************************************************************************
#define SYS_CTRL_I_MAP_ALTMAP   0x00000001  // 1: Select alternate interrupt 
                                            // map. 0: Select regular interrupt 
                                            // map. (See the ASD document for 
                                            // details.) 
#define SYS_CTRL_I_MAP_ALTMAP_M 0x00000001
#define SYS_CTRL_I_MAP_ALTMAP_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_RCGCRFC register.
//
//*****************************************************************************
#define SYS_CTRL_RCGCRFC_RFC0   0x00000001  // 0: Clock for RF CORE is gated. 
                                            // 1: Clock for RF CORE is enabled. 
#define SYS_CTRL_RCGCRFC_RFC0_M 0x00000001
#define SYS_CTRL_RCGCRFC_RFC0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_SCGCRFC register.
//
//*****************************************************************************
#define SYS_CTRL_SCGCRFC_RFC0   0x00000001  // 0: Clock for RF CORE is gated. 
                                            // 1: Clock for RF CORE is enabled. 
#define SYS_CTRL_SCGCRFC_RFC0_M 0x00000001
#define SYS_CTRL_SCGCRFC_RFC0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_DCGCRFC register.
//
//*****************************************************************************
#define SYS_CTRL_DCGCRFC_RFC0   0x00000001  // 0: Clock for RF CORE is gated. 
                                            // 1: Clock for RF CORE is enabled. 
#define SYS_CTRL_DCGCRFC_RFC0_M 0x00000001
#define SYS_CTRL_DCGCRFC_RFC0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SYS_CTRL_EMUOVR register.
//
//*****************************************************************************
#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_CLOCK_CG \
                                0x00000080  // ICEPick 'Force Active' clock 
                                            // gate override bit. 'Force 
                                            // Active' is an ICEPick command. 1 
                                            // --> In non-sleep power mode, 
                                            // peripherals clocks are forced to 
                                            // follow RCG* register settings. 
                                            // It forces CM3 clocks on. 0 --> 
                                            // Does not affect the peripheral 
                                            // clock settings. 

#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_CLOCK_CG_M \
                                0x00000080
#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_CLOCK_CG_S 7
#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_POWER_CG \
                                0x00000040  // ICEPick 'Force Power' clock 
                                            // gate override bit. 'Force Power' 
                                            // is an ICEPick command. 1 --> In 
                                            // non-sleep power mode, 
                                            // peripherals clocks are forced to 
                                            // follow RCG* register settings. 
                                            // It forces CM3 clocks on. 0 --> 
                                            // Does not affect the peripheral 
                                            // clock settings. 

#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_POWER_CG_M \
                                0x00000040
#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_POWER_CG_S 6
#define SYS_CTRL_EMUOVR_ICEPICK_INHIBIT_SLEEP_CG \
                                0x00000020  // ICEPick 'Inhibit Sleep' clock 
                                            // gate override bit. 'Inhibit 
                                            // Sleep' is an ICEPick command. 1 
                                            // --> In non-sleep power mode, 
                                            // peripherals clocks are forced to 
                                            // follow RCG* register settings. 
                                            // It forces CM3 clocks on. 0 --> 
                                            // Does not affect the peripheral 
                                            // clock settings. 

#define SYS_CTRL_EMUOVR_ICEPICK_INHIBIT_SLEEP_CG_M \
                                0x00000020
#define SYS_CTRL_EMUOVR_ICEPICK_INHIBIT_SLEEP_CG_S 5
#define SYS_CTRL_EMUOVR_ICEMELTER_WKUP_CG \
                                0x00000010  // ICEMelter 'WAKEUPEMU' clock 
                                            // gate override bit. 1 --> In 
                                            // non-sleep power mode, 
                                            // peripherals clocks are forced to 
                                            // follow RCG* register settings. 
                                            // It forces CM3 clocks on. 0 --> 
                                            // Does not affect the peripheral 
                                            // clock settings 

#define SYS_CTRL_EMUOVR_ICEMELTER_WKUP_CG_M \
                                0x00000010
#define SYS_CTRL_EMUOVR_ICEMELTER_WKUP_CG_S 4
#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_CLOCK_PM \
                                0x00000008  // ICEPick 'Force Active' power 
                                            // mode override bit. 'Force 
                                            // Active' is an ICEPick command. 1 
                                            // --> Prohibit the system to go 
                                            // into any power down modes. Keeps 
                                            // the emulator attached. 0 --> 
                                            // Does not override any power mode 
                                            // settings from SYSREGS and does 
                                            // not prohibit system to go into 
                                            // any power down modes. 

#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_CLOCK_PM_M \
                                0x00000008
#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_CLOCK_PM_S 3
#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_POWER_PM \
                                0x00000004  // ICEPick 'Force Power' power 
                                            // mode override bit. 'Force Power' 
                                            // is an ICEPick command. 1 --> 
                                            // Prohibit the system to go into 
                                            // any power down modes. Keeps the 
                                            // emulator attached. 0 --> Does 
                                            // not override any power mode 
                                            // settings from SYSREGS and does 
                                            // not prohibit system to go into 
                                            // any power down modes. 

#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_POWER_PM_M \
                                0x00000004
#define SYS_CTRL_EMUOVR_ICEPICK_FORCE_POWER_PM_S 2
#define SYS_CTRL_EMUOVR_ICEPICK_INHIBIT_SLEEP_PM \
                                0x00000002  // ICEPick 'Inhibit Sleep' power 
                                            // mode override bit. 'Inhibit 
                                            // Sleep' is an ICEPick command. 1 
                                            // --> Prohibit the system to go 
                                            // into any power down modes. Keeps 
                                            // the emulator attached. 0 --> 
                                            // Does not override any power mode 
                                            // settings from SYSREGS and does 
                                            // not prohibit system to go into 
                                            // any power down modes. 

#define SYS_CTRL_EMUOVR_ICEPICK_INHIBIT_SLEEP_PM_M \
                                0x00000002
#define SYS_CTRL_EMUOVR_ICEPICK_INHIBIT_SLEEP_PM_S 1
#define SYS_CTRL_EMUOVR_ICEMELTER_WKUP_PM \
                                0x00000001  // ICEMelter 'WAKEUPEMU' power 
                                            // mode override bit. 1 --> 
                                            // Prohibit the system to go into 
                                            // any power down modes. Keeps the 
                                            // emulator attached. 0 --> Does 
                                            // not override any power mode 
                                            // settings from SYSREGS and does 
                                            // not prohibit system to go into 
                                            // any power down modes. 

#define SYS_CTRL_EMUOVR_ICEMELTER_WKUP_PM_M \
                                0x00000001
#define SYS_CTRL_EMUOVR_ICEMELTER_WKUP_PM_S 0


#endif // __HW_SYS_CTRL_H__

