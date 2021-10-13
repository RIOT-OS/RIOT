/******************************************************************************
*  Filename:       hw_ioc.h
*  Revised:        $Date: 2013-04-30 17:13:44 +0200 (Tue, 30 Apr 2013) $
*  Revision:       $Revision: 9943 $
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

#ifndef __HW_IOC_H__
#define __HW_IOC_H__

//*****************************************************************************
//
// The following are defines for the IOC register offsets.
//
//*****************************************************************************
#define IOC_PA0_SEL             0x400D4000  // Peripheral select control for 
                                            // PA0 
#define IOC_PA1_SEL             0x400D4004  // Peripheral select control for 
                                            // PA1 
#define IOC_PA2_SEL             0x400D4008  // Peripheral select control for 
                                            // PA2 
#define IOC_PA3_SEL             0x400D400C  // Peripheral select control for 
                                            // PA3 
#define IOC_PA4_SEL             0x400D4010  // Peripheral select control for 
                                            // PA4 
#define IOC_PA5_SEL             0x400D4014  // Peripheral select control for 
                                            // PA5 
#define IOC_PA6_SEL             0x400D4018  // Peripheral select control for 
                                            // PA6 
#define IOC_PA7_SEL             0x400D401C  // Peripheral select control for 
                                            // PA7 
#define IOC_PB0_SEL             0x400D4020  // Peripheral select control for 
                                            // PB0 
#define IOC_PB1_SEL             0x400D4024  // Peripheral select control for 
                                            // PB1 
#define IOC_PB2_SEL             0x400D4028  // Peripheral select control for 
                                            // PB2 
#define IOC_PB3_SEL             0x400D402C  // Peripheral select control for 
                                            // PB3 
#define IOC_PB4_SEL             0x400D4030  // Peripheral select control for 
                                            // PB4 
#define IOC_PB5_SEL             0x400D4034  // Peripheral select control for 
                                            // PB5 
#define IOC_PB6_SEL             0x400D4038  // Peripheral select control for 
                                            // PB6 
#define IOC_PB7_SEL             0x400D403C  // Peripheral select control for 
                                            // PB7 
#define IOC_PC0_SEL             0x400D4040  // Peripheral select control for 
                                            // PC0 
#define IOC_PC1_SEL             0x400D4044  // Peripheral select control for 
                                            // PC1 
#define IOC_PC2_SEL             0x400D4048  // Peripheral select control for 
                                            // PC2 
#define IOC_PC3_SEL             0x400D404C  // Peripheral select control for 
                                            // PC3 
#define IOC_PC4_SEL             0x400D4050  // Peripheral select control for 
                                            // PC4 
#define IOC_PC5_SEL             0x400D4054  // Peripheral select control for 
                                            // PC5 
#define IOC_PC6_SEL             0x400D4058  // Peripheral select control for 
                                            // PC6 
#define IOC_PC7_SEL             0x400D405C  // Peripheral select control for 
                                            // PC7 
#define IOC_PD0_SEL             0x400D4060  // Peripheral select control for 
                                            // PD0 
#define IOC_PD1_SEL             0x400D4064  // Peripheral select control for 
                                            // PD1 
#define IOC_PD2_SEL             0x400D4068  // Peripheral select control for 
                                            // PD2 
#define IOC_PD3_SEL             0x400D406C  // Peripheral select control for 
                                            // PD3 
#define IOC_PD4_SEL             0x400D4070  // Peripheral select control for 
                                            // PD4 
#define IOC_PD5_SEL             0x400D4074  // Peripheral select control for 
                                            // PD5 
#define IOC_PD6_SEL             0x400D4078  // Peripheral select control for 
                                            // PD6 
#define IOC_PD7_SEL             0x400D407C  // Peripheral select control for 
                                            // PD7 
#define IOC_PA0_OVER            0x400D4080  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PA1_OVER            0x400D4084  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PA2_OVER            0x400D4088  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PA3_OVER            0x400D408C  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PA4_OVER            0x400D4090  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PA5_OVER            0x400D4094  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PA6_OVER            0x400D4098  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PA7_OVER            0x400D409C  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PB0_OVER            0x400D40A0  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PB1_OVER            0x400D40A4  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PB2_OVER            0x400D40A8  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PB3_OVER            0x400D40AC  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PB4_OVER            0x400D40B0  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PB5_OVER            0x400D40B4  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PB6_OVER            0x400D40B8  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PB7_OVER            0x400D40BC  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PC0_OVER            0x400D40C0  // This is the overide 
                                            // configuration register for each 
                                            // pad. PC0 has high drive 
                                            // capability. 
#define IOC_PC1_OVER            0x400D40C4  // This is the overide 
                                            // configuration register for each 
                                            // pad. PC1 has high drive 
                                            // capability. 
#define IOC_PC2_OVER            0x400D40C8  // This is the overide 
                                            // configuration register for each 
                                            // pad. PC2 has high drive 
                                            // capability. 
#define IOC_PC3_OVER            0x400D40CC  // This is the overide 
                                            // configuration register for each 
                                            // pad. PC3 has high drive 
                                            // capability. 
#define IOC_PC4_OVER            0x400D40D0  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PC5_OVER            0x400D40D4  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PC6_OVER            0x400D40D8  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PC7_OVER            0x400D40DC  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PD0_OVER            0x400D40E0  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PD1_OVER            0x400D40E4  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PD2_OVER            0x400D40E8  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PD3_OVER            0x400D40EC  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PD4_OVER            0x400D40F0  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PD5_OVER            0x400D40F4  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PD6_OVER            0x400D40F8  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_PD7_OVER            0x400D40FC  // This is the overide 
                                            // configuration register for each 
                                            // pad. 
#define IOC_UARTRXD_UART0       0x400D4100  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the UART0 RX. 
#define IOC_UARTCTS_UART1       0x400D4104  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the UART1 CTS. 
#define IOC_UARTRXD_UART1       0x400D4108  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the UART1 RX. 
#define IOC_CLK_SSI_SSI0        0x400D410C  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the SSI0 CLK. 
#define IOC_SSIRXD_SSI0         0x400D4110  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the SSI0 RX. 
#define IOC_SSIFSSIN_SSI0       0x400D4114  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the SSI0 FSSIN. 
#define IOC_CLK_SSIIN_SSI0      0x400D4118  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the SSI0 CLK_SSIN. 
#define IOC_CLK_SSI_SSI1        0x400D411C  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the SSI1 CLK. 
#define IOC_SSIRXD_SSI1         0x400D4120  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the SSI1 RX. 
#define IOC_SSIFSSIN_SSI1       0x400D4124  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the SSI1 FSSIN. 
#define IOC_CLK_SSIIN_SSI1      0x400D4128  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the SSI1 CLK_SSIN. 
#define IOC_I2CMSSDA            0x400D412C  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the I2C SDA. 
#define IOC_I2CMSSCL            0x400D4130  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the I2C SCL. 
#define IOC_GPT0OCP1            0x400D4134  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the GPT0OCP1. 
#define IOC_GPT0OCP2            0x400D4138  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the GPT0OCP2. 
#define IOC_GPT1OCP1            0x400D413C  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the GPT1OCP1. 
#define IOC_GPT1OCP2            0x400D4140  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the GPT1OCP2. 
#define IOC_GPT2OCP1            0x400D4144  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the GPT2OCP1. 
#define IOC_GPT2OCP2            0x400D4148  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the GPT2OCP2. 
#define IOC_GPT3OCP1            0x400D414C  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the GPT3OCP1. 
#define IOC_GPT3OCP2            0x400D4150  // Selects one of the 32 pins on 
                                            // the four 8-pin I/O-ports (port 
                                            // A, port B, port C, and port D) 
                                            // to be the GPT3OCP2. 


//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA0_SEL register.
//
//*****************************************************************************
#define IOC_PA0_SEL_PA0_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PA0. 
#define IOC_PA0_SEL_PA0_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA1_SEL register.
//
//*****************************************************************************
#define IOC_PA1_SEL_PA1_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PA1. 
#define IOC_PA1_SEL_PA1_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA2_SEL register.
//
//*****************************************************************************
#define IOC_PA2_SEL_PA2_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PA2. 
#define IOC_PA2_SEL_PA2_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA3_SEL register.
//
//*****************************************************************************
#define IOC_PA3_SEL_PA3_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PA3. 
#define IOC_PA3_SEL_PA3_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA4_SEL register.
//
//*****************************************************************************
#define IOC_PA4_SEL_PA4_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PA4. 
#define IOC_PA4_SEL_PA4_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA5_SEL register.
//
//*****************************************************************************
#define IOC_PA5_SEL_PA5_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PA5. 
#define IOC_PA5_SEL_PA5_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA6_SEL register.
//
//*****************************************************************************
#define IOC_PA6_SEL_PA6_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PA6. 
#define IOC_PA6_SEL_PA6_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA7_SEL register.
//
//*****************************************************************************
#define IOC_PA7_SEL_PA7_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PA7. 
#define IOC_PA7_SEL_PA7_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB0_SEL register.
//
//*****************************************************************************
#define IOC_PB0_SEL_PB0_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PB0. 
#define IOC_PB0_SEL_PB0_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB1_SEL register.
//
//*****************************************************************************
#define IOC_PB1_SEL_PB1_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PB1. 
#define IOC_PB1_SEL_PB1_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB2_SEL register.
//
//*****************************************************************************
#define IOC_PB2_SEL_PB2_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PB2. 
#define IOC_PB2_SEL_PB2_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB3_SEL register.
//
//*****************************************************************************
#define IOC_PB3_SEL_PB3_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PB3. 
#define IOC_PB3_SEL_PB3_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB4_SEL register.
//
//*****************************************************************************
#define IOC_PB4_SEL_PB4_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PB4. 
#define IOC_PB4_SEL_PB4_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB5_SEL register.
//
//*****************************************************************************
#define IOC_PB5_SEL_PB5_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PB5. 
#define IOC_PB5_SEL_PB5_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB6_SEL register.
//
//*****************************************************************************
#define IOC_PB6_SEL_PB6_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PB6. 
#define IOC_PB6_SEL_PB6_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB7_SEL register.
//
//*****************************************************************************
#define IOC_PB7_SEL_PB7_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PB7. 
#define IOC_PB7_SEL_PB7_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC0_SEL register.
//
//*****************************************************************************
#define IOC_PC0_SEL_PC0_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PC0. 
#define IOC_PC0_SEL_PC0_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC1_SEL register.
//
//*****************************************************************************
#define IOC_PC1_SEL_PC1_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PC1. 
#define IOC_PC1_SEL_PC1_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC2_SEL register.
//
//*****************************************************************************
#define IOC_PC2_SEL_PC2_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PC2. 
#define IOC_PC2_SEL_PC2_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC3_SEL register.
//
//*****************************************************************************
#define IOC_PC3_SEL_PC3_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PC3. 
#define IOC_PC3_SEL_PC3_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC4_SEL register.
//
//*****************************************************************************
#define IOC_PC4_SEL_PC4_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PC4. 
#define IOC_PC4_SEL_PC4_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC5_SEL register.
//
//*****************************************************************************
#define IOC_PC5_SEL_PC5_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PC5. 
#define IOC_PC5_SEL_PC5_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC6_SEL register.
//
//*****************************************************************************
#define IOC_PC6_SEL_PC6_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PC6. 
#define IOC_PC6_SEL_PC6_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC7_SEL register.
//
//*****************************************************************************
#define IOC_PC7_SEL_PC7_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PC7. 
#define IOC_PC7_SEL_PC7_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD0_SEL register.
//
//*****************************************************************************
#define IOC_PD0_SEL_PD0_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PD0. 
#define IOC_PD0_SEL_PD0_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD1_SEL register.
//
//*****************************************************************************
#define IOC_PD1_SEL_PD1_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PD1. 
#define IOC_PD1_SEL_PD1_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD2_SEL register.
//
//*****************************************************************************
#define IOC_PD2_SEL_PD2_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PD2. 
#define IOC_PD2_SEL_PD2_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD3_SEL register.
//
//*****************************************************************************
#define IOC_PD3_SEL_PD3_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PD3. 
#define IOC_PD3_SEL_PD3_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD4_SEL register.
//
//*****************************************************************************
#define IOC_PD4_SEL_PD4_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PD4. 
#define IOC_PD4_SEL_PD4_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD5_SEL register.
//
//*****************************************************************************
#define IOC_PD5_SEL_PD5_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PD5. 
#define IOC_PD5_SEL_PD5_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD6_SEL register.
//
//*****************************************************************************
#define IOC_PD6_SEL_PD6_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PD6. 
#define IOC_PD6_SEL_PD6_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD7_SEL register.
//
//*****************************************************************************
#define IOC_PD7_SEL_PD7_sel_M   0x0000001F  // Select one peripheral signal 
                                            // output for PD7. 
#define IOC_PD7_SEL_PD7_sel_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA0_OVER register.
//
//*****************************************************************************
#define IOC_PA0_OVER_PA0_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PA0_OVER_PA0_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA1_OVER register.
//
//*****************************************************************************
#define IOC_PA1_OVER_PA1_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PA1_OVER_PA1_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA2_OVER register.
//
//*****************************************************************************
#define IOC_PA2_OVER_PA2_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PA2_OVER_PA2_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA3_OVER register.
//
//*****************************************************************************
#define IOC_PA3_OVER_PA3_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PA3_OVER_PA3_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA4_OVER register.
//
//*****************************************************************************
#define IOC_PA4_OVER_PA4_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PA4_OVER_PA4_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA5_OVER register.
//
//*****************************************************************************
#define IOC_PA5_OVER_PA5_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PA5_OVER_PA5_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA6_OVER register.
//
//*****************************************************************************
#define IOC_PA6_OVER_PA6_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PA6_OVER_PA6_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PA7_OVER register.
//
//*****************************************************************************
#define IOC_PA7_OVER_PA7_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PA7_OVER_PA7_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB0_OVER register.
//
//*****************************************************************************
#define IOC_PB0_OVER_PB0_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PB0_OVER_PB0_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB1_OVER register.
//
//*****************************************************************************
#define IOC_PB1_OVER_PB1_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PB1_OVER_PB1_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB2_OVER register.
//
//*****************************************************************************
#define IOC_PB2_OVER_PB2_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PB2_OVER_PB2_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB3_OVER register.
//
//*****************************************************************************
#define IOC_PB3_OVER_PB3_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PB3_OVER_PB3_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB4_OVER register.
//
//*****************************************************************************
#define IOC_PB4_OVER_PB4_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PB4_OVER_PB4_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB5_OVER register.
//
//*****************************************************************************
#define IOC_PB5_OVER_PB5_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PB5_OVER_PB5_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB6_OVER register.
//
//*****************************************************************************
#define IOC_PB6_OVER_PB6_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PB6_OVER_PB6_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PB7_OVER register.
//
//*****************************************************************************
#define IOC_PB7_OVER_PB7_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PB7_OVER_PB7_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC0_OVER register.
//
//*****************************************************************************
#define IOC_PC0_OVER_PC0_over   0x00000008  // 0: output disable 1: oe - 
                                            // output enable 
#define IOC_PC0_OVER_PC0_over_M 0x00000008
#define IOC_PC0_OVER_PC0_over_S 3
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC1_OVER register.
//
//*****************************************************************************
#define IOC_PC1_OVER_PC1_over   0x00000008  // 0: output disable 1: oe - 
                                            // output enable 
#define IOC_PC1_OVER_PC1_over_M 0x00000008
#define IOC_PC1_OVER_PC1_over_S 3
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC2_OVER register.
//
//*****************************************************************************
#define IOC_PC2_OVER_PC2_over   0x00000008  // 0: output disable 1: oe - 
                                            // output enable 
#define IOC_PC2_OVER_PC2_over_M 0x00000008
#define IOC_PC2_OVER_PC2_over_S 3
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC3_OVER register.
//
//*****************************************************************************
#define IOC_PC3_OVER_PC3_over   0x00000008  // 0: output disable 1: oe - 
                                            // output enable 
#define IOC_PC3_OVER_PC3_over_M 0x00000008
#define IOC_PC3_OVER_PC3_over_S 3
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC4_OVER register.
//
//*****************************************************************************
#define IOC_PC4_OVER_PC4_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PC4_OVER_PC4_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC5_OVER register.
//
//*****************************************************************************
#define IOC_PC5_OVER_PC5_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PC5_OVER_PC5_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC6_OVER register.
//
//*****************************************************************************
#define IOC_PC6_OVER_PC6_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PC6_OVER_PC6_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PC7_OVER register.
//
//*****************************************************************************
#define IOC_PC7_OVER_PC7_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PC7_OVER_PC7_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD0_OVER register.
//
//*****************************************************************************
#define IOC_PD0_OVER_PD0_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PD0_OVER_PD0_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD1_OVER register.
//
//*****************************************************************************
#define IOC_PD1_OVER_PD1_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PD1_OVER_PD1_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD2_OVER register.
//
//*****************************************************************************
#define IOC_PD2_OVER_PD2_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PD2_OVER_PD2_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD3_OVER register.
//
//*****************************************************************************
#define IOC_PD3_OVER_PD3_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PD3_OVER_PD3_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD4_OVER register.
//
//*****************************************************************************
#define IOC_PD4_OVER_PD4_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PD4_OVER_PD4_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD5_OVER register.
//
//*****************************************************************************
#define IOC_PD5_OVER_PD5_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PD5_OVER_PD5_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD6_OVER register.
//
//*****************************************************************************
#define IOC_PD6_OVER_PD6_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PD6_OVER_PD6_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_PD7_OVER register.
//
//*****************************************************************************
#define IOC_PD7_OVER_PD7_over_M 0x0000000F  // 0x8: oe - output enable 0x4: 
                                            // pue - pullup enable 0x2: pde - 
                                            // pulldown enable 0x1: ana - 
                                            // analog enable 
#define IOC_PD7_OVER_PD7_over_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_UARTRXD_UART0 register.
//
//*****************************************************************************
#define IOC_UARTRXD_UART0_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as UART0 RX 1: 
                                            // PA1 selected as UART0 RX ... 31: 
                                            // PD7 selected as UART0 RX 

#define IOC_UARTRXD_UART0_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_UARTCTS_UART1 register.
//
//*****************************************************************************
#define IOC_UARTCTS_UART1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as UART1 CTS 1: 
                                            // PA1 selected as UART1 CTS ... 
                                            // 31: PD7 selected as UART1 CTS 

#define IOC_UARTCTS_UART1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_UARTRXD_UART1 register.
//
//*****************************************************************************
#define IOC_UARTRXD_UART1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as UART1 RX 1: 
                                            // PA1 selected as UART1 RX ... 31: 
                                            // PD7 selected as UART1 RX 

#define IOC_UARTRXD_UART1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_CLK_SSI_SSI0 register.
//
//*****************************************************************************
#define IOC_CLK_SSI_SSI0_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as SSI0 CLK 1: 
                                            // PA1 selected as SSI0 CLK ... 31: 
                                            // PD7 selected as SSI0 CLK 

#define IOC_CLK_SSI_SSI0_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_SSIRXD_SSI0 register.
//
//*****************************************************************************
#define IOC_SSIRXD_SSI0_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as SSI0 RX 1: 
                                            // PA1 selected as SSI0 RX ... 31: 
                                            // PD7 selected as SSI0 RX 

#define IOC_SSIRXD_SSI0_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_SSIFSSIN_SSI0 register.
//
//*****************************************************************************
#define IOC_SSIFSSIN_SSI0_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as SSI0 FSSIN 
                                            // 1: PA1 selected as SSI0 FSSIN 
                                            // ... 31: PD7 selected as SSI0 
                                            // FSSIN 

#define IOC_SSIFSSIN_SSI0_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_CLK_SSIIN_SSI0 register.
//
//*****************************************************************************
#define IOC_CLK_SSIIN_SSI0_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as SSI0 
                                            // CLK_SSIN 1: PA1 selected as SSI0 
                                            // CLK_SSIN ... 31: PD7 selected as 
                                            // SSI0 CLK_SSIN 

#define IOC_CLK_SSIIN_SSI0_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_CLK_SSI_SSI1 register.
//
//*****************************************************************************
#define IOC_CLK_SSI_SSI1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as SSI1 CLK 1: 
                                            // PA1 selected as SSI1 CLK ... 31: 
                                            // PD7 selected as SSI1 CLK 

#define IOC_CLK_SSI_SSI1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_SSIRXD_SSI1 register.
//
//*****************************************************************************
#define IOC_SSIRXD_SSI1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as SSI1 RX 1: 
                                            // PA1 selected as SSI1 RX ... 31: 
                                            // PD7 selected as SSI1 RX 

#define IOC_SSIRXD_SSI1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_SSIFSSIN_SSI1 register.
//
//*****************************************************************************
#define IOC_SSIFSSIN_SSI1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as SSI1 FSSIN 
                                            // 1: PA1 selected as SSI1 FSSIN 
                                            // ... 31: PD7 selected as SSI1 
                                            // FSSIN 

#define IOC_SSIFSSIN_SSI1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// IOC_CLK_SSIIN_SSI1 register.
//
//*****************************************************************************
#define IOC_CLK_SSIIN_SSI1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as SSI1 
                                            // CLK_SSIN 1: PA1 selected as SSI1 
                                            // CLK_SSIN ... 31: PD7 selected as 
                                            // SSI1 CLK_SSIN 

#define IOC_CLK_SSIIN_SSI1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_I2CMSSDA register.
//
//*****************************************************************************
#define IOC_I2CMSSDA_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as I2C SDA 1: 
                                            // PA1 selected as I2C SDA ... 31: 
                                            // PD7 selected as I2C SDA 

#define IOC_I2CMSSDA_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_I2CMSSCL register.
//
//*****************************************************************************
#define IOC_I2CMSSCL_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as I2C SCL 1: 
                                            // PA1 selected as I2C SCL ... 31: 
                                            // PD7 selected as I2C SCL 

#define IOC_I2CMSSCL_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_GPT0OCP1 register.
//
//*****************************************************************************
#define IOC_GPT0OCP1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as GPT0OCP1 1: 
                                            // PA1 selected as GPT0OCP1 ... 31: 
                                            // PD7 selected as GPT0OCP1 

#define IOC_GPT0OCP1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_GPT0OCP2 register.
//
//*****************************************************************************
#define IOC_GPT0OCP2_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as GPT0OCP2 1: 
                                            // PA1 selected as GPT0OCP2 ... 31: 
                                            // PD7 selected as GPT0OCP2 

#define IOC_GPT0OCP2_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_GPT1OCP1 register.
//
//*****************************************************************************
#define IOC_GPT1OCP1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as GPT1OCP1 1: 
                                            // PA1 selected as GPT1OCP1 ... 31: 
                                            // PD7 selected as GPT1OCP1 

#define IOC_GPT1OCP1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_GPT1OCP2 register.
//
//*****************************************************************************
#define IOC_GPT1OCP2_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as GPT1OCP2 1: 
                                            // PA1 selected as GPT1OCP2 ... 31: 
                                            // PD7 selected as GPT1OCP2 

#define IOC_GPT1OCP2_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_GPT2OCP1 register.
//
//*****************************************************************************
#define IOC_GPT2OCP1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as GPT2OCP1 1: 
                                            // PA1 selected as GPT2OCP1 ... 31: 
                                            // PD7 selected as GPT2OCP1 

#define IOC_GPT2OCP1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_GPT2OCP2 register.
//
//*****************************************************************************
#define IOC_GPT2OCP2_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as GPT2OCP2 1: 
                                            // PA1 selected as GPT2OCP2 ... 31: 
                                            // PD7 selected as GPT2OCP2 

#define IOC_GPT2OCP2_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_GPT3OCP1 register.
//
//*****************************************************************************
#define IOC_GPT3OCP1_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as GPT3OCP1 1: 
                                            // PA1 selected as GPT3OCP1 ... 31: 
                                            // PD7 selected as GPT3OCP1 

#define IOC_GPT3OCP1_INPUT_SEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the IOC_GPT3OCP2 register.
//
//*****************************************************************************
#define IOC_GPT3OCP2_INPUT_SEL_M \
                                0x0000001F  // 0: PA0 selected as GPT3OCP2 1: 
                                            // PA1 selected as GPT3OCP2 ... 31: 
                                            // PD7 selected as GPT3OCP2 

#define IOC_GPT3OCP2_INPUT_SEL_S 0


#endif // __HW_IOC_H__

