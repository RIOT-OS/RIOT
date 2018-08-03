/******************************************************************************
*  Filename:       hw_memmap.h
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

#ifndef __HW_MEMMAP_H__
#define __HW_MEMMAP_H__

//*****************************************************************************
//
// The following are defines for the base address of the memories and
// peripherals on the top_s interface.
//
//*****************************************************************************
#define ROM_BASE                0x00000000  // ROM 
#define FLASH_BASE              0x00200000  // Flash 
#define SRAM_BASE               0x20000000  // SRAM 
#define SRAM_LL_BASE            0x20004000  // SRAM_LL 
#define SSI0_BASE               0x40008000  // SSI 
#define SSI1_BASE               0x40009000  // SSI 
#define UART0_BASE              0x4000C000  // UART 
#define UART1_BASE              0x4000D000  // UART 
#define I2C_M0_BASE             0x40020000  // I2CM 
#define I2C_S0_BASE             0x40020800  // I2CS 
#define GPTIMER0_BASE           0x40030000  // GPTIMER 
#define GPTIMER1_BASE           0x40031000  // GPTIMER 
#define GPTIMER2_BASE           0x40032000  // GPTIMER 
#define GPTIMER3_BASE           0x40033000  // GPTIMER 
#define RFCORE_RAM_BASE         0x40088000  // SRAM_RFCORE 
#define FRMF_SRCM_RAM_BASE      0x40088400  // SRAM_FRMF_SRCM 
#define RFCORE_FFSM_BASE        0x40088500  // RFCORE_FFSM 
#define RFCORE_XREG_BASE        0x40088600  // RFCORE_XREG 
#define RFCORE_SFR_BASE         0x40088800  // RFCORE_SFR 
#define USB_BASE                0x40089000  // USB 
#define AES_BASE                0x4008B000  // AES 
#define SYS_CTRL_BASE           0x400D2000  // SYS_CTRL 
#define FLASH_CTRL_BASE         0x400D3000  // FLASH_CTRL 
#define IOC_BASE                0x400D4000  // IOC 
#define SMWDTHROSC_BASE         0x400D5000  // SMWDTHROSC 
#define ANA_REGS_BASE           0x400D6000  // ANA_REGS 
#define SOC_ADC_BASE            0x400D7000  // SOC_ADC 
#define GPIO_A_BASE             0x400D9000  // GPIO 
#define GPIO_B_BASE             0x400DA000  // GPIO 
#define GPIO_C_BASE             0x400DB000  // GPIO 
#define GPIO_D_BASE             0x400DC000  // GPIO 
#define uDMA_BASE               0x400FF000  // UDMA 
#define ST_TESTCTRL_BASE        0x40110000  // STTEST 
#define PKA_BASE                0x44004000  // PKA 
#define PKA_RAM_BASE            0x44006000  // SRAM_PKA 
#define CC_TESTCTRL_BASE        0x44010000  // CCTEST 

#endif // __HW_MEMMAP_H__

