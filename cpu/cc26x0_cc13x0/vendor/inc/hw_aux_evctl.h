/******************************************************************************
*  Filename:       hw_aux_evctl_h
*  Revised:        2017-05-16 19:35:21 +0200 (Tue, 16 May 2017)
*  Revision:       49005
*
* Copyright (c) 2015 - 2017, Texas Instruments Incorporated
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1) Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2) Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3) Neither the name of the ORGANIZATION nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef __HW_AUX_EVCTL_H__
#define __HW_AUX_EVCTL_H__

//*****************************************************************************
//
// This section defines the register offsets of
// AUX_EVCTL component
//
//*****************************************************************************
// Vector Configuration 0
#define AUX_EVCTL_O_VECCFG0                                         0x00000000

// Vector Configuration 1
#define AUX_EVCTL_O_VECCFG1                                         0x00000004

// Sensor Controller Engine Wait Event Selection
#define AUX_EVCTL_O_SCEWEVSEL                                       0x00000008

// Events To AON Flags
#define AUX_EVCTL_O_EVTOAONFLAGS                                    0x0000000C

// Events To AON Polarity
#define AUX_EVCTL_O_EVTOAONPOL                                      0x00000010

// Direct Memory Access Control
#define AUX_EVCTL_O_DMACTL                                          0x00000014

// Software Event Set
#define AUX_EVCTL_O_SWEVSET                                         0x00000018

// Event Status 0
#define AUX_EVCTL_O_EVSTAT0                                         0x0000001C

// Event Status 1
#define AUX_EVCTL_O_EVSTAT1                                         0x00000020

// Event To MCU Polarity
#define AUX_EVCTL_O_EVTOMCUPOL                                      0x00000024

// Events to MCU Flags
#define AUX_EVCTL_O_EVTOMCUFLAGS                                    0x00000028

// Combined Event To MCU Mask
#define AUX_EVCTL_O_COMBEVTOMCUMASK                                 0x0000002C

// Vector Flags
#define AUX_EVCTL_O_VECFLAGS                                        0x00000034

// Events To MCU Flags Clear
#define AUX_EVCTL_O_EVTOMCUFLAGSCLR                                 0x00000038

// Events To AON Clear
#define AUX_EVCTL_O_EVTOAONFLAGSCLR                                 0x0000003C

// Vector Flags Clear
#define AUX_EVCTL_O_VECFLAGSCLR                                     0x00000040

//*****************************************************************************
//
// Register: AUX_EVCTL_O_VECCFG0
//
//*****************************************************************************
// Field:    [14] VEC1_POL
//
// Vector 1 trigger event polarity.
//
// To manually trigger vector 1 execution:
// - AUX_SCE must sleep.
// - Set VEC1_EV to a known static value.
// - Toggle VEC1_POL twice.
// ENUMs:
// FALL                     Falling edge triggers vector 1 execution.
// RISE                     Rising edge triggers vector 1 execution.
#define AUX_EVCTL_VECCFG0_VEC1_POL                                  0x00004000
#define AUX_EVCTL_VECCFG0_VEC1_POL_BITN                                     14
#define AUX_EVCTL_VECCFG0_VEC1_POL_M                                0x00004000
#define AUX_EVCTL_VECCFG0_VEC1_POL_S                                        14
#define AUX_EVCTL_VECCFG0_VEC1_POL_FALL                             0x00004000
#define AUX_EVCTL_VECCFG0_VEC1_POL_RISE                             0x00000000

// Field:    [13] VEC1_EN
//
// Vector 1 trigger enable.
//
// When enabled, VEC1_EV event with VEC1_POL polarity triggers a jump to vector
// # 1 when AUX_SCE sleeps.
//
// Lower vectors (0) have priority.
// ENUMs:
// EN                       Enable vector 1 trigger.
// DIS                      Disable vector 1 trigger.
#define AUX_EVCTL_VECCFG0_VEC1_EN                                   0x00002000
#define AUX_EVCTL_VECCFG0_VEC1_EN_BITN                                      13
#define AUX_EVCTL_VECCFG0_VEC1_EN_M                                 0x00002000
#define AUX_EVCTL_VECCFG0_VEC1_EN_S                                         13
#define AUX_EVCTL_VECCFG0_VEC1_EN_EN                                0x00002000
#define AUX_EVCTL_VECCFG0_VEC1_EN_DIS                               0x00000000

// Field:  [12:8] VEC1_EV
//
// Select vector 1 trigger source event.
// ENUMs:
// ADC_IRQ                  EVSTAT1.ADC_IRQ
// MCU_EV                   EVSTAT1.MCU_EV
// ACLK_REF                 EVSTAT1.ACLK_REF
// AUXIO15                  EVSTAT1.AUXIO15
// AUXIO14                  EVSTAT1.AUXIO14
// AUXIO13                  EVSTAT1.AUXIO13
// AUXIO12                  EVSTAT1.AUXIO12
// AUXIO11                  EVSTAT1.AUXIO11
// AUXIO10                  EVSTAT1.AUXIO10
// AUXIO9                   EVSTAT1.AUXIO9
// AUXIO8                   EVSTAT1.AUXIO8
// AUXIO7                   EVSTAT1.AUXIO7
// AUXIO6                   EVSTAT1.AUXIO6
// AUXIO5                   EVSTAT1.AUXIO5
// AUXIO4                   EVSTAT1.AUXIO4
// AUXIO3                   EVSTAT1.AUXIO3
// AUXIO2                   EVSTAT0.AUXIO2
// AUXIO1                   EVSTAT0.AUXIO1
// AUXIO0                   EVSTAT0.AUXIO0
// AON_PROG_WU              EVSTAT0.AON_PROG_WU
// AON_SW                   EVSTAT0.AON_SW
// OBSMUX1                  EVSTAT0.OBSMUX1
// OBSMUX0                  EVSTAT0.OBSMUX0
// ADC_FIFO_ALMOST_FULL     EVSTAT0.ADC_FIFO_ALMOST_FULL
// ADC_DONE                 EVSTAT0.ADC_DONE
// SMPH_AUTOTAKE_DONE       EVSTAT0.SMPH_AUTOTAKE_DONE
// TIMER1_EV                EVSTAT0.TIMER1_EV
// TIMER0_EV                EVSTAT0.TIMER0_EV
// TDC_DONE                 EVSTAT0.TDC_DONE
// AUX_COMPB                EVSTAT0.AUX_COMPB
// AUX_COMPA                EVSTAT0.AUX_COMPA
// AON_RTC_CH2              EVSTAT0.AON_RTC_CH2
#define AUX_EVCTL_VECCFG0_VEC1_EV_W                                          5
#define AUX_EVCTL_VECCFG0_VEC1_EV_M                                 0x00001F00
#define AUX_EVCTL_VECCFG0_VEC1_EV_S                                          8
#define AUX_EVCTL_VECCFG0_VEC1_EV_ADC_IRQ                           0x00001F00
#define AUX_EVCTL_VECCFG0_VEC1_EV_MCU_EV                            0x00001E00
#define AUX_EVCTL_VECCFG0_VEC1_EV_ACLK_REF                          0x00001D00
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO15                           0x00001C00
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO14                           0x00001B00
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO13                           0x00001A00
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO12                           0x00001900
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO11                           0x00001800
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO10                           0x00001700
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO9                            0x00001600
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO8                            0x00001500
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO7                            0x00001400
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO6                            0x00001300
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO5                            0x00001200
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO4                            0x00001100
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO3                            0x00001000
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO2                            0x00000F00
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO1                            0x00000E00
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUXIO0                            0x00000D00
#define AUX_EVCTL_VECCFG0_VEC1_EV_AON_PROG_WU                       0x00000C00
#define AUX_EVCTL_VECCFG0_VEC1_EV_AON_SW                            0x00000B00
#define AUX_EVCTL_VECCFG0_VEC1_EV_OBSMUX1                           0x00000A00
#define AUX_EVCTL_VECCFG0_VEC1_EV_OBSMUX0                           0x00000900
#define AUX_EVCTL_VECCFG0_VEC1_EV_ADC_FIFO_ALMOST_FULL              0x00000800
#define AUX_EVCTL_VECCFG0_VEC1_EV_ADC_DONE                          0x00000700
#define AUX_EVCTL_VECCFG0_VEC1_EV_SMPH_AUTOTAKE_DONE                0x00000600
#define AUX_EVCTL_VECCFG0_VEC1_EV_TIMER1_EV                         0x00000500
#define AUX_EVCTL_VECCFG0_VEC1_EV_TIMER0_EV                         0x00000400
#define AUX_EVCTL_VECCFG0_VEC1_EV_TDC_DONE                          0x00000300
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUX_COMPB                         0x00000200
#define AUX_EVCTL_VECCFG0_VEC1_EV_AUX_COMPA                         0x00000100
#define AUX_EVCTL_VECCFG0_VEC1_EV_AON_RTC_CH2                       0x00000000

// Field:     [6] VEC0_POL
//
// Vector 0 trigger event polarity.
//
// To manually trigger vector 0 execution:
// - AUX_SCE must sleep.
// - Set VEC0_EV to a known static value.
// - Toggle VEC0_POL twice.
// ENUMs:
// FALL                     Falling edge triggers vector 0 execution.
// RISE                     Rising edge triggers vector 0 execution.
#define AUX_EVCTL_VECCFG0_VEC0_POL                                  0x00000040
#define AUX_EVCTL_VECCFG0_VEC0_POL_BITN                                      6
#define AUX_EVCTL_VECCFG0_VEC0_POL_M                                0x00000040
#define AUX_EVCTL_VECCFG0_VEC0_POL_S                                         6
#define AUX_EVCTL_VECCFG0_VEC0_POL_FALL                             0x00000040
#define AUX_EVCTL_VECCFG0_VEC0_POL_RISE                             0x00000000

// Field:     [5] VEC0_EN
//
// Vector 0 trigger enable.
//
// When enabled, VEC0_EV event with VEC0_POL polarity triggers a jump to vector
// # 0 when AUX_SCE sleeps.
// ENUMs:
// EN                       Enable vector 0 trigger.
// DIS                      Disable vector 0 trigger.
#define AUX_EVCTL_VECCFG0_VEC0_EN                                   0x00000020
#define AUX_EVCTL_VECCFG0_VEC0_EN_BITN                                       5
#define AUX_EVCTL_VECCFG0_VEC0_EN_M                                 0x00000020
#define AUX_EVCTL_VECCFG0_VEC0_EN_S                                          5
#define AUX_EVCTL_VECCFG0_VEC0_EN_EN                                0x00000020
#define AUX_EVCTL_VECCFG0_VEC0_EN_DIS                               0x00000000

// Field:   [4:0] VEC0_EV
//
// Select vector 0 trigger source event.
// ENUMs:
// ADC_IRQ                  EVSTAT1.ADC_IRQ
// MCU_EV                   EVSTAT1.MCU_EV
// ACLK_REF                 EVSTAT1.ACLK_REF
// AUXIO15                  EVSTAT1.AUXIO15
// AUXIO14                  EVSTAT1.AUXIO14
// AUXIO13                  EVSTAT1.AUXIO13
// AUXIO12                  EVSTAT1.AUXIO12
// AUXIO11                  EVSTAT1.AUXIO11
// AUXIO10                  EVSTAT1.AUXIO10
// AUXIO9                   EVSTAT1.AUXIO9
// AUXIO8                   EVSTAT1.AUXIO8
// AUXIO7                   EVSTAT1.AUXIO7
// AUXIO6                   EVSTAT1.AUXIO6
// AUXIO5                   EVSTAT1.AUXIO5
// AUXIO4                   EVSTAT1.AUXIO4
// AUXIO3                   EVSTAT1.AUXIO3
// AUXIO2                   EVSTAT0.AUXIO2
// AUXIO1                   EVSTAT0.AUXIO1
// AUXIO0                   EVSTAT0.AUXIO0
// AON_PROG_WU              EVSTAT0.AON_PROG_WU
// AON_SW                   EVSTAT0.AON_SW
// OBSMUX1                  EVSTAT0.OBSMUX1
// OBSMUX0                  EVSTAT0.OBSMUX0
// ADC_FIFO_ALMOST_FULL     EVSTAT0.ADC_FIFO_ALMOST_FULL
// ADC_DONE                 EVSTAT0.ADC_DONE
// SMPH_AUTOTAKE_DONE       EVSTAT0.SMPH_AUTOTAKE_DONE
// TIMER1_EV                EVSTAT0.TIMER1_EV
// TIMER0_EV                EVSTAT0.TIMER0_EV
// TDC_DONE                 EVSTAT0.TDC_DONE
// AUX_COMPB                EVSTAT0.AUX_COMPB
// AUX_COMPA                EVSTAT0.AUX_COMPA
// AON_RTC_CH2              EVSTAT0.AON_RTC_CH2
#define AUX_EVCTL_VECCFG0_VEC0_EV_W                                          5
#define AUX_EVCTL_VECCFG0_VEC0_EV_M                                 0x0000001F
#define AUX_EVCTL_VECCFG0_VEC0_EV_S                                          0
#define AUX_EVCTL_VECCFG0_VEC0_EV_ADC_IRQ                           0x0000001F
#define AUX_EVCTL_VECCFG0_VEC0_EV_MCU_EV                            0x0000001E
#define AUX_EVCTL_VECCFG0_VEC0_EV_ACLK_REF                          0x0000001D
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO15                           0x0000001C
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO14                           0x0000001B
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO13                           0x0000001A
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO12                           0x00000019
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO11                           0x00000018
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO10                           0x00000017
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO9                            0x00000016
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO8                            0x00000015
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO7                            0x00000014
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO6                            0x00000013
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO5                            0x00000012
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO4                            0x00000011
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO3                            0x00000010
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO2                            0x0000000F
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO1                            0x0000000E
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUXIO0                            0x0000000D
#define AUX_EVCTL_VECCFG0_VEC0_EV_AON_PROG_WU                       0x0000000C
#define AUX_EVCTL_VECCFG0_VEC0_EV_AON_SW                            0x0000000B
#define AUX_EVCTL_VECCFG0_VEC0_EV_OBSMUX1                           0x0000000A
#define AUX_EVCTL_VECCFG0_VEC0_EV_OBSMUX0                           0x00000009
#define AUX_EVCTL_VECCFG0_VEC0_EV_ADC_FIFO_ALMOST_FULL              0x00000008
#define AUX_EVCTL_VECCFG0_VEC0_EV_ADC_DONE                          0x00000007
#define AUX_EVCTL_VECCFG0_VEC0_EV_SMPH_AUTOTAKE_DONE                0x00000006
#define AUX_EVCTL_VECCFG0_VEC0_EV_TIMER1_EV                         0x00000005
#define AUX_EVCTL_VECCFG0_VEC0_EV_TIMER0_EV                         0x00000004
#define AUX_EVCTL_VECCFG0_VEC0_EV_TDC_DONE                          0x00000003
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUX_COMPB                         0x00000002
#define AUX_EVCTL_VECCFG0_VEC0_EV_AUX_COMPA                         0x00000001
#define AUX_EVCTL_VECCFG0_VEC0_EV_AON_RTC_CH2                       0x00000000

//*****************************************************************************
//
// Register: AUX_EVCTL_O_VECCFG1
//
//*****************************************************************************
// Field:    [14] VEC3_POL
//
// Vector 3 trigger event polarity.
//
// To manually trigger vector 3 execution:
// - AUX_SCE must sleep.
// - Set VEC3_EV to a known static value.
// - Toggle VEC3_POL twice.
// ENUMs:
// FALL                     Falling edge triggers vector 3 execution.
// RISE                     Rising edge triggers vector 3 execution.
#define AUX_EVCTL_VECCFG1_VEC3_POL                                  0x00004000
#define AUX_EVCTL_VECCFG1_VEC3_POL_BITN                                     14
#define AUX_EVCTL_VECCFG1_VEC3_POL_M                                0x00004000
#define AUX_EVCTL_VECCFG1_VEC3_POL_S                                        14
#define AUX_EVCTL_VECCFG1_VEC3_POL_FALL                             0x00004000
#define AUX_EVCTL_VECCFG1_VEC3_POL_RISE                             0x00000000

// Field:    [13] VEC3_EN
//
// Vector 3 trigger enable.
//
// When enabled, VEC3_EV event with VEC3_POL polarity triggers a jump to vector
// # 3 when AUX_SCE sleeps.
//
// Lower vectors (0, 1, and 2) have priority.
// ENUMs:
// EN                       Enable vector 3 trigger.
// DIS                      Disable vector 3 trigger.
#define AUX_EVCTL_VECCFG1_VEC3_EN                                   0x00002000
#define AUX_EVCTL_VECCFG1_VEC3_EN_BITN                                      13
#define AUX_EVCTL_VECCFG1_VEC3_EN_M                                 0x00002000
#define AUX_EVCTL_VECCFG1_VEC3_EN_S                                         13
#define AUX_EVCTL_VECCFG1_VEC3_EN_EN                                0x00002000
#define AUX_EVCTL_VECCFG1_VEC3_EN_DIS                               0x00000000

// Field:  [12:8] VEC3_EV
//
// Select vector 3 trigger source event.
// ENUMs:
// ADC_IRQ                  EVSTAT1.ADC_IRQ
// MCU_EV                   EVSTAT1.MCU_EV
// ACLK_REF                 EVSTAT1.ACLK_REF
// AUXIO15                  EVSTAT1.AUXIO15
// AUXIO14                  EVSTAT1.AUXIO14
// AUXIO13                  EVSTAT1.AUXIO13
// AUXIO12                  EVSTAT1.AUXIO12
// AUXIO11                  EVSTAT1.AUXIO11
// AUXIO10                  EVSTAT1.AUXIO10
// AUXIO9                   EVSTAT1.AUXIO9
// AUXIO8                   EVSTAT1.AUXIO8
// AUXIO7                   EVSTAT1.AUXIO7
// AUXIO6                   EVSTAT1.AUXIO6
// AUXIO5                   EVSTAT1.AUXIO5
// AUXIO4                   EVSTAT1.AUXIO4
// AUXIO3                   EVSTAT1.AUXIO3
// AUXIO2                   EVSTAT0.AUXIO2
// AUXIO1                   EVSTAT0.AUXIO1
// AUXIO0                   EVSTAT0.AUXIO0
// AON_PROG_WU              EVSTAT0.AON_PROG_WU
// AON_SW                   EVSTAT0.AON_SW
// OBSMUX1                  EVSTAT0.OBSMUX1
// OBSMUX0                  EVSTAT0.OBSMUX0
// ADC_FIFO_ALMOST_FULL     EVSTAT0.ADC_FIFO_ALMOST_FULL
// ADC_DONE                 EVSTAT0.ADC_DONE
// SMPH_AUTOTAKE_DONE       EVSTAT0.SMPH_AUTOTAKE_DONE
// TIMER1_EV                EVSTAT0.TIMER1_EV
// TIMER0_EV                EVSTAT0.TIMER0_EV
// TDC_DONE                 EVSTAT0.TDC_DONE
// AUX_COMPB                EVSTAT0.AUX_COMPB
// AUX_COMPA                EVSTAT0.AUX_COMPA
// AON_RTC_CH2              EVSTAT0.AON_RTC_CH2
#define AUX_EVCTL_VECCFG1_VEC3_EV_W                                          5
#define AUX_EVCTL_VECCFG1_VEC3_EV_M                                 0x00001F00
#define AUX_EVCTL_VECCFG1_VEC3_EV_S                                          8
#define AUX_EVCTL_VECCFG1_VEC3_EV_ADC_IRQ                           0x00001F00
#define AUX_EVCTL_VECCFG1_VEC3_EV_MCU_EV                            0x00001E00
#define AUX_EVCTL_VECCFG1_VEC3_EV_ACLK_REF                          0x00001D00
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO15                           0x00001C00
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO14                           0x00001B00
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO13                           0x00001A00
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO12                           0x00001900
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO11                           0x00001800
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO10                           0x00001700
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO9                            0x00001600
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO8                            0x00001500
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO7                            0x00001400
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO6                            0x00001300
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO5                            0x00001200
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO4                            0x00001100
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO3                            0x00001000
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO2                            0x00000F00
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO1                            0x00000E00
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUXIO0                            0x00000D00
#define AUX_EVCTL_VECCFG1_VEC3_EV_AON_PROG_WU                       0x00000C00
#define AUX_EVCTL_VECCFG1_VEC3_EV_AON_SW                            0x00000B00
#define AUX_EVCTL_VECCFG1_VEC3_EV_OBSMUX1                           0x00000A00
#define AUX_EVCTL_VECCFG1_VEC3_EV_OBSMUX0                           0x00000900
#define AUX_EVCTL_VECCFG1_VEC3_EV_ADC_FIFO_ALMOST_FULL              0x00000800
#define AUX_EVCTL_VECCFG1_VEC3_EV_ADC_DONE                          0x00000700
#define AUX_EVCTL_VECCFG1_VEC3_EV_SMPH_AUTOTAKE_DONE                0x00000600
#define AUX_EVCTL_VECCFG1_VEC3_EV_TIMER1_EV                         0x00000500
#define AUX_EVCTL_VECCFG1_VEC3_EV_TIMER0_EV                         0x00000400
#define AUX_EVCTL_VECCFG1_VEC3_EV_TDC_DONE                          0x00000300
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUX_COMPB                         0x00000200
#define AUX_EVCTL_VECCFG1_VEC3_EV_AUX_COMPA                         0x00000100
#define AUX_EVCTL_VECCFG1_VEC3_EV_AON_RTC_CH2                       0x00000000

// Field:     [6] VEC2_POL
//
// Vector 2 trigger event polarity.
//
// To manually trigger vector 2 execution:
// - AUX_SCE must sleep.
// - Set VEC2_EV to a known static value.
// - Toggle VEC2_POL twice.
// ENUMs:
// FALL                     Falling edge triggers vector 2 execution.
// RISE                     Rising edge triggers vector 2 execution.
#define AUX_EVCTL_VECCFG1_VEC2_POL                                  0x00000040
#define AUX_EVCTL_VECCFG1_VEC2_POL_BITN                                      6
#define AUX_EVCTL_VECCFG1_VEC2_POL_M                                0x00000040
#define AUX_EVCTL_VECCFG1_VEC2_POL_S                                         6
#define AUX_EVCTL_VECCFG1_VEC2_POL_FALL                             0x00000040
#define AUX_EVCTL_VECCFG1_VEC2_POL_RISE                             0x00000000

// Field:     [5] VEC2_EN
//
// Vector 2 trigger enable.
//
// When enabled, VEC2_EV event with VEC2_POL polarity triggers a jump to vector
// # 2 when AUX_SCE sleeps.
//
// Lower vectors (0 and 1) have priority.
// ENUMs:
// EN                       Enable vector 2 trigger.
// DIS                      Disable vector 2 trigger.
#define AUX_EVCTL_VECCFG1_VEC2_EN                                   0x00000020
#define AUX_EVCTL_VECCFG1_VEC2_EN_BITN                                       5
#define AUX_EVCTL_VECCFG1_VEC2_EN_M                                 0x00000020
#define AUX_EVCTL_VECCFG1_VEC2_EN_S                                          5
#define AUX_EVCTL_VECCFG1_VEC2_EN_EN                                0x00000020
#define AUX_EVCTL_VECCFG1_VEC2_EN_DIS                               0x00000000

// Field:   [4:0] VEC2_EV
//
// Select vector 2 trigger source event.
// ENUMs:
// ADC_IRQ                  EVSTAT1.ADC_IRQ
// MCU_EV                   EVSTAT1.MCU_EV
// ACLK_REF                 EVSTAT1.ACLK_REF
// AUXIO15                  EVSTAT1.AUXIO15
// AUXIO14                  EVSTAT1.AUXIO14
// AUXIO13                  EVSTAT1.AUXIO13
// AUXIO12                  EVSTAT1.AUXIO12
// AUXIO11                  EVSTAT1.AUXIO11
// AUXIO10                  EVSTAT1.AUXIO10
// AUXIO9                   EVSTAT1.AUXIO9
// AUXIO8                   EVSTAT1.AUXIO8
// AUXIO7                   EVSTAT1.AUXIO7
// AUXIO6                   EVSTAT1.AUXIO6
// AUXIO5                   EVSTAT1.AUXIO5
// AUXIO4                   EVSTAT1.AUXIO4
// AUXIO3                   EVSTAT1.AUXIO3
// AUXIO2                   EVSTAT0.AUXIO2
// AUXIO1                   EVSTAT0.AUXIO1
// AUXIO0                   EVSTAT0.AUXIO0
// AON_PROG_WU              EVSTAT0.AON_PROG_WU
// AON_SW                   EVSTAT0.AON_SW
// OBSMUX1                  EVSTAT0.OBSMUX1
// OBSMUX0                  EVSTAT0.OBSMUX0
// ADC_FIFO_ALMOST_FULL     EVSTAT0.ADC_FIFO_ALMOST_FULL
// ADC_DONE                 EVSTAT0.ADC_DONE
// SMPH_AUTOTAKE_DONE       EVSTAT0.SMPH_AUTOTAKE_DONE
// TIMER1_EV                EVSTAT0.TIMER1_EV
// TIMER0_EV                EVSTAT0.TIMER0_EV
// TDC_DONE                 EVSTAT0.TDC_DONE
// AUX_COMPB                EVSTAT0.AUX_COMPB
// AUX_COMPA                EVSTAT0.AUX_COMPA
// AON_RTC_CH2              EVSTAT0.AON_RTC_CH2
#define AUX_EVCTL_VECCFG1_VEC2_EV_W                                          5
#define AUX_EVCTL_VECCFG1_VEC2_EV_M                                 0x0000001F
#define AUX_EVCTL_VECCFG1_VEC2_EV_S                                          0
#define AUX_EVCTL_VECCFG1_VEC2_EV_ADC_IRQ                           0x0000001F
#define AUX_EVCTL_VECCFG1_VEC2_EV_MCU_EV                            0x0000001E
#define AUX_EVCTL_VECCFG1_VEC2_EV_ACLK_REF                          0x0000001D
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO15                           0x0000001C
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO14                           0x0000001B
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO13                           0x0000001A
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO12                           0x00000019
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO11                           0x00000018
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO10                           0x00000017
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO9                            0x00000016
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO8                            0x00000015
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO7                            0x00000014
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO6                            0x00000013
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO5                            0x00000012
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO4                            0x00000011
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO3                            0x00000010
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO2                            0x0000000F
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO1                            0x0000000E
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUXIO0                            0x0000000D
#define AUX_EVCTL_VECCFG1_VEC2_EV_AON_PROG_WU                       0x0000000C
#define AUX_EVCTL_VECCFG1_VEC2_EV_AON_SW                            0x0000000B
#define AUX_EVCTL_VECCFG1_VEC2_EV_OBSMUX1                           0x0000000A
#define AUX_EVCTL_VECCFG1_VEC2_EV_OBSMUX0                           0x00000009
#define AUX_EVCTL_VECCFG1_VEC2_EV_ADC_FIFO_ALMOST_FULL              0x00000008
#define AUX_EVCTL_VECCFG1_VEC2_EV_ADC_DONE                          0x00000007
#define AUX_EVCTL_VECCFG1_VEC2_EV_SMPH_AUTOTAKE_DONE                0x00000006
#define AUX_EVCTL_VECCFG1_VEC2_EV_TIMER1_EV                         0x00000005
#define AUX_EVCTL_VECCFG1_VEC2_EV_TIMER0_EV                         0x00000004
#define AUX_EVCTL_VECCFG1_VEC2_EV_TDC_DONE                          0x00000003
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUX_COMPB                         0x00000002
#define AUX_EVCTL_VECCFG1_VEC2_EV_AUX_COMPA                         0x00000001
#define AUX_EVCTL_VECCFG1_VEC2_EV_AON_RTC_CH2                       0x00000000

//*****************************************************************************
//
// Register: AUX_EVCTL_O_SCEWEVSEL
//
//*****************************************************************************
// Field:   [4:0] WEV7_EV
//
// Select event source to connect to AUX_SCE:WUSTAT.EV_SIGNALS bit 7.
// ENUMs:
// ADC_IRQ                  EVSTAT1.ADC_IRQ
// MCU_EV                   EVSTAT1.MCU_EV
// ACLK_REF                 EVSTAT1.ACLK_REF
// AUXIO15                  EVSTAT1.AUXIO15
// AUXIO14                  EVSTAT1.AUXIO14
// AUXIO13                  EVSTAT1.AUXIO13
// AUXIO12                  EVSTAT1.AUXIO12
// AUXIO11                  EVSTAT1.AUXIO11
// AUXIO10                  EVSTAT1.AUXIO10
// AUXIO9                   EVSTAT1.AUXIO9
// AUXIO8                   EVSTAT1.AUXIO8
// AUXIO7                   EVSTAT1.AUXIO7
// AUXIO6                   EVSTAT1.AUXIO6
// AUXIO5                   EVSTAT1.AUXIO5
// AUXIO4                   EVSTAT1.AUXIO4
// AUXIO3                   EVSTAT1.AUXIO3
// AUXIO2                   EVSTAT0.AUXIO2
// AUXIO1                   EVSTAT0.AUXIO1
// AUXIO0                   EVSTAT0.AUXIO0
// AON_PROG_WU              EVSTAT0.AON_PROG_WU
// AON_SW                   EVSTAT0.AON_SW
// OBSMUX1                  EVSTAT0.OBSMUX1
// OBSMUX0                  EVSTAT0.OBSMUX0
// ADC_FIFO_ALMOST_FULL     EVSTAT0.ADC_FIFO_ALMOST_FULL
// ADC_DONE                 EVSTAT0.ADC_DONE
// SMPH_AUTOTAKE_DONE       EVSTAT0.SMPH_AUTOTAKE_DONE
// TIMER1_EV                EVSTAT0.TIMER1_EV
// TIMER0_EV                EVSTAT0.TIMER0_EV
// TDC_DONE                 EVSTAT0.TDC_DONE
// AUX_COMPB                EVSTAT0.AUX_COMPB
// AUX_COMPA                EVSTAT0.AUX_COMPA
// AON_RTC_CH2              EVSTAT0.AON_RTC_CH2
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_W                                        5
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_M                               0x0000001F
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_S                                        0
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_ADC_IRQ                         0x0000001F
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_MCU_EV                          0x0000001E
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_ACLK_REF                        0x0000001D
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO15                         0x0000001C
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO14                         0x0000001B
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO13                         0x0000001A
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO12                         0x00000019
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO11                         0x00000018
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO10                         0x00000017
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO9                          0x00000016
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO8                          0x00000015
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO7                          0x00000014
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO6                          0x00000013
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO5                          0x00000012
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO4                          0x00000011
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO3                          0x00000010
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO2                          0x0000000F
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO1                          0x0000000E
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUXIO0                          0x0000000D
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AON_PROG_WU                     0x0000000C
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AON_SW                          0x0000000B
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_OBSMUX1                         0x0000000A
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_OBSMUX0                         0x00000009
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_ADC_FIFO_ALMOST_FULL            0x00000008
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_ADC_DONE                        0x00000007
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_SMPH_AUTOTAKE_DONE              0x00000006
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_TIMER1_EV                       0x00000005
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_TIMER0_EV                       0x00000004
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_TDC_DONE                        0x00000003
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUX_COMPB                       0x00000002
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AUX_COMPA                       0x00000001
#define AUX_EVCTL_SCEWEVSEL_WEV7_EV_AON_RTC_CH2                     0x00000000

//*****************************************************************************
//
// Register: AUX_EVCTL_O_EVTOAONFLAGS
//
//*****************************************************************************
// Field:     [8] TIMER1_EV
//
// This event flag is set when level selected by EVTOAONPOL.TIMER1_EV occurs on
// EVSTAT0.TIMER1_EV.
#define AUX_EVCTL_EVTOAONFLAGS_TIMER1_EV                            0x00000100
#define AUX_EVCTL_EVTOAONFLAGS_TIMER1_EV_BITN                                8
#define AUX_EVCTL_EVTOAONFLAGS_TIMER1_EV_M                          0x00000100
#define AUX_EVCTL_EVTOAONFLAGS_TIMER1_EV_S                                   8

// Field:     [7] TIMER0_EV
//
// This event flag is set when level selected by EVTOAONPOL.TIMER0_EV occurs on
// EVSTAT0.TIMER0_EV.
#define AUX_EVCTL_EVTOAONFLAGS_TIMER0_EV                            0x00000080
#define AUX_EVCTL_EVTOAONFLAGS_TIMER0_EV_BITN                                7
#define AUX_EVCTL_EVTOAONFLAGS_TIMER0_EV_M                          0x00000080
#define AUX_EVCTL_EVTOAONFLAGS_TIMER0_EV_S                                   7

// Field:     [6] TDC_DONE
//
// This event flag is set when level selected by EVTOAONPOL.TDC_DONE occurs on
// EVSTAT0.TDC_DONE.
#define AUX_EVCTL_EVTOAONFLAGS_TDC_DONE                             0x00000040
#define AUX_EVCTL_EVTOAONFLAGS_TDC_DONE_BITN                                 6
#define AUX_EVCTL_EVTOAONFLAGS_TDC_DONE_M                           0x00000040
#define AUX_EVCTL_EVTOAONFLAGS_TDC_DONE_S                                    6

// Field:     [5] ADC_DONE
//
// This event flag is set when level selected by EVTOAONPOL.ADC_DONE occurs on
// EVSTAT0.ADC_DONE.
#define AUX_EVCTL_EVTOAONFLAGS_ADC_DONE                             0x00000020
#define AUX_EVCTL_EVTOAONFLAGS_ADC_DONE_BITN                                 5
#define AUX_EVCTL_EVTOAONFLAGS_ADC_DONE_M                           0x00000020
#define AUX_EVCTL_EVTOAONFLAGS_ADC_DONE_S                                    5

// Field:     [4] AUX_COMPB
//
// This event flag is set when edge selected by EVTOAONPOL.AUX_COMPB occurs on
// EVSTAT0.AUX_COMPB.
#define AUX_EVCTL_EVTOAONFLAGS_AUX_COMPB                            0x00000010
#define AUX_EVCTL_EVTOAONFLAGS_AUX_COMPB_BITN                                4
#define AUX_EVCTL_EVTOAONFLAGS_AUX_COMPB_M                          0x00000010
#define AUX_EVCTL_EVTOAONFLAGS_AUX_COMPB_S                                   4

// Field:     [3] AUX_COMPA
//
// This event flag is set when edge selected by EVTOAONPOL.AUX_COMPA occurs on
// EVSTAT0.AUX_COMPA.
#define AUX_EVCTL_EVTOAONFLAGS_AUX_COMPA                            0x00000008
#define AUX_EVCTL_EVTOAONFLAGS_AUX_COMPA_BITN                                3
#define AUX_EVCTL_EVTOAONFLAGS_AUX_COMPA_M                          0x00000008
#define AUX_EVCTL_EVTOAONFLAGS_AUX_COMPA_S                                   3

// Field:     [2] SWEV2
//
// This event flag is set when software writes a 1 to SWEVSET.SWEV2.
#define AUX_EVCTL_EVTOAONFLAGS_SWEV2                                0x00000004
#define AUX_EVCTL_EVTOAONFLAGS_SWEV2_BITN                                    2
#define AUX_EVCTL_EVTOAONFLAGS_SWEV2_M                              0x00000004
#define AUX_EVCTL_EVTOAONFLAGS_SWEV2_S                                       2

// Field:     [1] SWEV1
//
// This event flag is set when software writes a 1 to SWEVSET.SWEV1.
#define AUX_EVCTL_EVTOAONFLAGS_SWEV1                                0x00000002
#define AUX_EVCTL_EVTOAONFLAGS_SWEV1_BITN                                    1
#define AUX_EVCTL_EVTOAONFLAGS_SWEV1_M                              0x00000002
#define AUX_EVCTL_EVTOAONFLAGS_SWEV1_S                                       1

// Field:     [0] SWEV0
//
// This event flag is set when software writes a 1 to SWEVSET.SWEV0.
#define AUX_EVCTL_EVTOAONFLAGS_SWEV0                                0x00000001
#define AUX_EVCTL_EVTOAONFLAGS_SWEV0_BITN                                    0
#define AUX_EVCTL_EVTOAONFLAGS_SWEV0_M                              0x00000001
#define AUX_EVCTL_EVTOAONFLAGS_SWEV0_S                                       0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_EVTOAONPOL
//
//*****************************************************************************
// Field:     [8] TIMER1_EV
//
// Select the level of EVSTAT0.TIMER1_EV that sets EVTOAONFLAGS.TIMER1_EV.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOAONPOL_TIMER1_EV                              0x00000100
#define AUX_EVCTL_EVTOAONPOL_TIMER1_EV_BITN                                  8
#define AUX_EVCTL_EVTOAONPOL_TIMER1_EV_M                            0x00000100
#define AUX_EVCTL_EVTOAONPOL_TIMER1_EV_S                                     8
#define AUX_EVCTL_EVTOAONPOL_TIMER1_EV_LOW                          0x00000100
#define AUX_EVCTL_EVTOAONPOL_TIMER1_EV_HIGH                         0x00000000

// Field:     [7] TIMER0_EV
//
// Select the level of EVSTAT0.TIMER0_EV that sets EVTOAONFLAGS.TIMER0_EV.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOAONPOL_TIMER0_EV                              0x00000080
#define AUX_EVCTL_EVTOAONPOL_TIMER0_EV_BITN                                  7
#define AUX_EVCTL_EVTOAONPOL_TIMER0_EV_M                            0x00000080
#define AUX_EVCTL_EVTOAONPOL_TIMER0_EV_S                                     7
#define AUX_EVCTL_EVTOAONPOL_TIMER0_EV_LOW                          0x00000080
#define AUX_EVCTL_EVTOAONPOL_TIMER0_EV_HIGH                         0x00000000

// Field:     [6] TDC_DONE
//
// Select level of EVSTAT0.TDC_DONE that sets EVTOAONFLAGS.TDC_DONE.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOAONPOL_TDC_DONE                               0x00000040
#define AUX_EVCTL_EVTOAONPOL_TDC_DONE_BITN                                   6
#define AUX_EVCTL_EVTOAONPOL_TDC_DONE_M                             0x00000040
#define AUX_EVCTL_EVTOAONPOL_TDC_DONE_S                                      6
#define AUX_EVCTL_EVTOAONPOL_TDC_DONE_LOW                           0x00000040
#define AUX_EVCTL_EVTOAONPOL_TDC_DONE_HIGH                          0x00000000

// Field:     [5] ADC_DONE
//
// Select the level of  EVSTAT0.ADC_DONE that sets EVTOAONFLAGS.ADC_DONE.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOAONPOL_ADC_DONE                               0x00000020
#define AUX_EVCTL_EVTOAONPOL_ADC_DONE_BITN                                   5
#define AUX_EVCTL_EVTOAONPOL_ADC_DONE_M                             0x00000020
#define AUX_EVCTL_EVTOAONPOL_ADC_DONE_S                                      5
#define AUX_EVCTL_EVTOAONPOL_ADC_DONE_LOW                           0x00000020
#define AUX_EVCTL_EVTOAONPOL_ADC_DONE_HIGH                          0x00000000

// Field:     [4] AUX_COMPB
//
// Select the edge of  EVSTAT0.AUX_COMPB that sets EVTOAONFLAGS.AUX_COMPB.
// ENUMs:
// LOW                      Falling edge
// HIGH                     Rising edge
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPB                              0x00000010
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPB_BITN                                  4
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPB_M                            0x00000010
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPB_S                                     4
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPB_LOW                          0x00000010
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPB_HIGH                         0x00000000

// Field:     [3] AUX_COMPA
//
// Select the edge of  EVSTAT0.AUX_COMPA that sets EVTOAONFLAGS.AUX_COMPA.
// ENUMs:
// LOW                      Falling edge
// HIGH                     Rising edge
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPA                              0x00000008
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPA_BITN                                  3
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPA_M                            0x00000008
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPA_S                                     3
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPA_LOW                          0x00000008
#define AUX_EVCTL_EVTOAONPOL_AUX_COMPA_HIGH                         0x00000000

//*****************************************************************************
//
// Register: AUX_EVCTL_O_DMACTL
//
//*****************************************************************************
// Field:     [2] REQ_MODE
//
// UDMA0 Request mode
// ENUMs:
// SINGLE                   Single requests are generated on UDMA0 channel 7
//                          when the condition configured in SEL is met.
// BURST                    Burst requests are generated on UDMA0 channel 7
//                          when the condition configured in SEL is met.
#define AUX_EVCTL_DMACTL_REQ_MODE                                   0x00000004
#define AUX_EVCTL_DMACTL_REQ_MODE_BITN                                       2
#define AUX_EVCTL_DMACTL_REQ_MODE_M                                 0x00000004
#define AUX_EVCTL_DMACTL_REQ_MODE_S                                          2
#define AUX_EVCTL_DMACTL_REQ_MODE_SINGLE                            0x00000004
#define AUX_EVCTL_DMACTL_REQ_MODE_BURST                             0x00000000

// Field:     [1] EN
//
// uDMA ADC interface enable.
//
// 0: Disable UDMA0 interface to ADC.
// 1: Enable UDMA0 interface to ADC.
#define AUX_EVCTL_DMACTL_EN                                         0x00000002
#define AUX_EVCTL_DMACTL_EN_BITN                                             1
#define AUX_EVCTL_DMACTL_EN_M                                       0x00000002
#define AUX_EVCTL_DMACTL_EN_S                                                1

// Field:     [0] SEL
//
// Select FIFO watermark level required to trigger a UDMA0 transfer of ADC FIFO
// data.
// ENUMs:
// FIFO_ALMOST_FULL         UDMA0 trigger event will be generated when the ADC
//                          FIFO is almost full (3/4 full).
// FIFO_NOT_EMPTY           UDMA0 trigger event will be generated when there
//                          are samples in the ADC FIFO.
#define AUX_EVCTL_DMACTL_SEL                                        0x00000001
#define AUX_EVCTL_DMACTL_SEL_BITN                                            0
#define AUX_EVCTL_DMACTL_SEL_M                                      0x00000001
#define AUX_EVCTL_DMACTL_SEL_S                                               0
#define AUX_EVCTL_DMACTL_SEL_FIFO_ALMOST_FULL                       0x00000001
#define AUX_EVCTL_DMACTL_SEL_FIFO_NOT_EMPTY                         0x00000000

//*****************************************************************************
//
// Register: AUX_EVCTL_O_SWEVSET
//
//*****************************************************************************
// Field:     [2] SWEV2
//
// Software event flag 2.
//
// 0: No effect.
// 1: Set software event flag 2.
#define AUX_EVCTL_SWEVSET_SWEV2                                     0x00000004
#define AUX_EVCTL_SWEVSET_SWEV2_BITN                                         2
#define AUX_EVCTL_SWEVSET_SWEV2_M                                   0x00000004
#define AUX_EVCTL_SWEVSET_SWEV2_S                                            2

// Field:     [1] SWEV1
//
// Software event flag 1.
//
// 0: No effect.
// 1: Set software event flag 1.
#define AUX_EVCTL_SWEVSET_SWEV1                                     0x00000002
#define AUX_EVCTL_SWEVSET_SWEV1_BITN                                         1
#define AUX_EVCTL_SWEVSET_SWEV1_M                                   0x00000002
#define AUX_EVCTL_SWEVSET_SWEV1_S                                            1

// Field:     [0] SWEV0
//
// Software event flag 0.
//
// 0: No effect.
// 1: Set software event flag 0.
#define AUX_EVCTL_SWEVSET_SWEV0                                     0x00000001
#define AUX_EVCTL_SWEVSET_SWEV0_BITN                                         0
#define AUX_EVCTL_SWEVSET_SWEV0_M                                   0x00000001
#define AUX_EVCTL_SWEVSET_SWEV0_S                                            0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_EVSTAT0
//
//*****************************************************************************
// Field:    [15] AUXIO2
//
// AUXIO2   pin level, read value corresponds to AUX_AIODIO0:GPIODIN bit 2.
#define AUX_EVCTL_EVSTAT0_AUXIO2                                    0x00008000
#define AUX_EVCTL_EVSTAT0_AUXIO2_BITN                                       15
#define AUX_EVCTL_EVSTAT0_AUXIO2_M                                  0x00008000
#define AUX_EVCTL_EVSTAT0_AUXIO2_S                                          15

// Field:    [14] AUXIO1
//
// AUXIO1   pin level, read value corresponds to AUX_AIODIO0:GPIODIN bit 1.
#define AUX_EVCTL_EVSTAT0_AUXIO1                                    0x00004000
#define AUX_EVCTL_EVSTAT0_AUXIO1_BITN                                       14
#define AUX_EVCTL_EVSTAT0_AUXIO1_M                                  0x00004000
#define AUX_EVCTL_EVSTAT0_AUXIO1_S                                          14

// Field:    [13] AUXIO0
//
// AUXIO0   pin level, read value corresponds to AUX_AIODIO0:GPIODIN bit 0.
#define AUX_EVCTL_EVSTAT0_AUXIO0                                    0x00002000
#define AUX_EVCTL_EVSTAT0_AUXIO0_BITN                                       13
#define AUX_EVCTL_EVSTAT0_AUXIO0_M                                  0x00002000
#define AUX_EVCTL_EVSTAT0_AUXIO0_S                                          13

// Field:    [12] AON_PROG_WU
//
// AON_EVENT:AUXWUSEL.WU2_EV OR AON_EVENT:AUXWUSEL.WU1_EV OR
// AON_EVENT:AUXWUSEL.WU0_EV
#define AUX_EVCTL_EVSTAT0_AON_PROG_WU                               0x00001000
#define AUX_EVCTL_EVSTAT0_AON_PROG_WU_BITN                                  12
#define AUX_EVCTL_EVSTAT0_AON_PROG_WU_M                             0x00001000
#define AUX_EVCTL_EVSTAT0_AON_PROG_WU_S                                     12

// Field:    [11] AON_SW
//
// AON_WUC:AUXCTL.SWEV
#define AUX_EVCTL_EVSTAT0_AON_SW                                    0x00000800
#define AUX_EVCTL_EVSTAT0_AON_SW_BITN                                       11
#define AUX_EVCTL_EVSTAT0_AON_SW_M                                  0x00000800
#define AUX_EVCTL_EVSTAT0_AON_SW_S                                          11

// Field:    [10] OBSMUX1
//
// Observation input 1 from IOC.
// This event is configured by IOC:OBSAUXOUTPUT.SEL1.
#define AUX_EVCTL_EVSTAT0_OBSMUX1                                   0x00000400
#define AUX_EVCTL_EVSTAT0_OBSMUX1_BITN                                      10
#define AUX_EVCTL_EVSTAT0_OBSMUX1_M                                 0x00000400
#define AUX_EVCTL_EVSTAT0_OBSMUX1_S                                         10

// Field:     [9] OBSMUX0
//
// Observation input 0 from IOC.
// This event is configured by IOC:OBSAUXOUTPUT.SEL0 and can be overridden by
// IOC:OBSAUXOUTPUT.SEL_MISC.
#define AUX_EVCTL_EVSTAT0_OBSMUX0                                   0x00000200
#define AUX_EVCTL_EVSTAT0_OBSMUX0_BITN                                       9
#define AUX_EVCTL_EVSTAT0_OBSMUX0_M                                 0x00000200
#define AUX_EVCTL_EVSTAT0_OBSMUX0_S                                          9

// Field:     [8] ADC_FIFO_ALMOST_FULL
//
// AUX_ANAIF:ADCFIFOSTAT.ALMOST_FULL
#define AUX_EVCTL_EVSTAT0_ADC_FIFO_ALMOST_FULL                      0x00000100
#define AUX_EVCTL_EVSTAT0_ADC_FIFO_ALMOST_FULL_BITN                          8
#define AUX_EVCTL_EVSTAT0_ADC_FIFO_ALMOST_FULL_M                    0x00000100
#define AUX_EVCTL_EVSTAT0_ADC_FIFO_ALMOST_FULL_S                             8

// Field:     [7] ADC_DONE
//
// AUX_ANAIF ADC conversion done event.
#define AUX_EVCTL_EVSTAT0_ADC_DONE                                  0x00000080
#define AUX_EVCTL_EVSTAT0_ADC_DONE_BITN                                      7
#define AUX_EVCTL_EVSTAT0_ADC_DONE_M                                0x00000080
#define AUX_EVCTL_EVSTAT0_ADC_DONE_S                                         7

// Field:     [6] SMPH_AUTOTAKE_DONE
//
// See AUX_SMPH:AUTOTAKE.SMPH_ID for description.
#define AUX_EVCTL_EVSTAT0_SMPH_AUTOTAKE_DONE                        0x00000040
#define AUX_EVCTL_EVSTAT0_SMPH_AUTOTAKE_DONE_BITN                            6
#define AUX_EVCTL_EVSTAT0_SMPH_AUTOTAKE_DONE_M                      0x00000040
#define AUX_EVCTL_EVSTAT0_SMPH_AUTOTAKE_DONE_S                               6

// Field:     [5] TIMER1_EV
//
// AUX_TIMER1_EV event, see AUX_TIMER:T1TARGET for description.
#define AUX_EVCTL_EVSTAT0_TIMER1_EV                                 0x00000020
#define AUX_EVCTL_EVSTAT0_TIMER1_EV_BITN                                     5
#define AUX_EVCTL_EVSTAT0_TIMER1_EV_M                               0x00000020
#define AUX_EVCTL_EVSTAT0_TIMER1_EV_S                                        5

// Field:     [4] TIMER0_EV
//
// AUX_TIMER0_EV event, see AUX_TIMER:T0TARGET for description.
#define AUX_EVCTL_EVSTAT0_TIMER0_EV                                 0x00000010
#define AUX_EVCTL_EVSTAT0_TIMER0_EV_BITN                                     4
#define AUX_EVCTL_EVSTAT0_TIMER0_EV_M                               0x00000010
#define AUX_EVCTL_EVSTAT0_TIMER0_EV_S                                        4

// Field:     [3] TDC_DONE
//
// AUX_TDC:STAT.DONE
#define AUX_EVCTL_EVSTAT0_TDC_DONE                                  0x00000008
#define AUX_EVCTL_EVSTAT0_TDC_DONE_BITN                                      3
#define AUX_EVCTL_EVSTAT0_TDC_DONE_M                                0x00000008
#define AUX_EVCTL_EVSTAT0_TDC_DONE_S                                         3

// Field:     [2] AUX_COMPB
//
// Comparator B output
#define AUX_EVCTL_EVSTAT0_AUX_COMPB                                 0x00000004
#define AUX_EVCTL_EVSTAT0_AUX_COMPB_BITN                                     2
#define AUX_EVCTL_EVSTAT0_AUX_COMPB_M                               0x00000004
#define AUX_EVCTL_EVSTAT0_AUX_COMPB_S                                        2

// Field:     [1] AUX_COMPA
//
// Comparator A output
#define AUX_EVCTL_EVSTAT0_AUX_COMPA                                 0x00000002
#define AUX_EVCTL_EVSTAT0_AUX_COMPA_BITN                                     1
#define AUX_EVCTL_EVSTAT0_AUX_COMPA_M                               0x00000002
#define AUX_EVCTL_EVSTAT0_AUX_COMPA_S                                        1

// Field:     [0] AON_RTC_CH2
//
// AON_RTC:EVFLAGS.CH2
#define AUX_EVCTL_EVSTAT0_AON_RTC_CH2                               0x00000001
#define AUX_EVCTL_EVSTAT0_AON_RTC_CH2_BITN                                   0
#define AUX_EVCTL_EVSTAT0_AON_RTC_CH2_M                             0x00000001
#define AUX_EVCTL_EVSTAT0_AON_RTC_CH2_S                                      0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_EVSTAT1
//
//*****************************************************************************
// Field:    [15] ADC_IRQ
//
// The logical function for this event is configurable.
//
// When DMACTL.EN = 1 :
//    Event = UDMA0 Channel 7 done event     OR
// AUX_ANAIF:ADCFIFOSTAT.OVERFLOW      OR      AUX_ANAIF:ADCFIFOSTAT.UNDERFLOW
//
// When DMACTL.EN = 0 :
//    Event = (NOT AUX_ANAIF:ADCFIFOSTAT.EMPTY)      OR
// AUX_ANAIF:ADCFIFOSTAT.OVERFLOW      OR      AUX_ANAIF:ADCFIFOSTAT.UNDERFLOW
//
// Bit 7 in UDMA0:DONEMASK must be 0.
#define AUX_EVCTL_EVSTAT1_ADC_IRQ                                   0x00008000
#define AUX_EVCTL_EVSTAT1_ADC_IRQ_BITN                                      15
#define AUX_EVCTL_EVSTAT1_ADC_IRQ_M                                 0x00008000
#define AUX_EVCTL_EVSTAT1_ADC_IRQ_S                                         15

// Field:    [14] MCU_EV
//
// Event from EVENT configured by EVENT:AUXSEL0.
#define AUX_EVCTL_EVSTAT1_MCU_EV                                    0x00004000
#define AUX_EVCTL_EVSTAT1_MCU_EV_BITN                                       14
#define AUX_EVCTL_EVSTAT1_MCU_EV_M                                  0x00004000
#define AUX_EVCTL_EVSTAT1_MCU_EV_S                                          14

// Field:    [13] ACLK_REF
//
// TDC reference clock.
// It is configured by DDI_0_OSC:CTL0.ACLK_REF_SRC_SEL and enabled by
// AUX_WUC:REFCLKCTL.REQ.
#define AUX_EVCTL_EVSTAT1_ACLK_REF                                  0x00002000
#define AUX_EVCTL_EVSTAT1_ACLK_REF_BITN                                     13
#define AUX_EVCTL_EVSTAT1_ACLK_REF_M                                0x00002000
#define AUX_EVCTL_EVSTAT1_ACLK_REF_S                                        13

// Field:    [12] AUXIO15
//
// AUXIO15 pin level, read value corresponds to AUX_AIODIO1:GPIODIN bit 7.
#define AUX_EVCTL_EVSTAT1_AUXIO15                                   0x00001000
#define AUX_EVCTL_EVSTAT1_AUXIO15_BITN                                      12
#define AUX_EVCTL_EVSTAT1_AUXIO15_M                                 0x00001000
#define AUX_EVCTL_EVSTAT1_AUXIO15_S                                         12

// Field:    [11] AUXIO14
//
// AUXIO14 pin level, read value corresponds to AUX_AIODIO1:GPIODIN bit 6.
#define AUX_EVCTL_EVSTAT1_AUXIO14                                   0x00000800
#define AUX_EVCTL_EVSTAT1_AUXIO14_BITN                                      11
#define AUX_EVCTL_EVSTAT1_AUXIO14_M                                 0x00000800
#define AUX_EVCTL_EVSTAT1_AUXIO14_S                                         11

// Field:    [10] AUXIO13
//
// AUXIO13 pin level, read value corresponds to AUX_AIODIO1:GPIODIN bit 5.
#define AUX_EVCTL_EVSTAT1_AUXIO13                                   0x00000400
#define AUX_EVCTL_EVSTAT1_AUXIO13_BITN                                      10
#define AUX_EVCTL_EVSTAT1_AUXIO13_M                                 0x00000400
#define AUX_EVCTL_EVSTAT1_AUXIO13_S                                         10

// Field:     [9] AUXIO12
//
// AUXIO12 pin level, read value corresponds to AUX_AIODIO1:GPIODIN bit 4.
#define AUX_EVCTL_EVSTAT1_AUXIO12                                   0x00000200
#define AUX_EVCTL_EVSTAT1_AUXIO12_BITN                                       9
#define AUX_EVCTL_EVSTAT1_AUXIO12_M                                 0x00000200
#define AUX_EVCTL_EVSTAT1_AUXIO12_S                                          9

// Field:     [8] AUXIO11
//
// AUXIO11 pin level, read value corresponds to AUX_AIODIO1:GPIODIN bit 3.
#define AUX_EVCTL_EVSTAT1_AUXIO11                                   0x00000100
#define AUX_EVCTL_EVSTAT1_AUXIO11_BITN                                       8
#define AUX_EVCTL_EVSTAT1_AUXIO11_M                                 0x00000100
#define AUX_EVCTL_EVSTAT1_AUXIO11_S                                          8

// Field:     [7] AUXIO10
//
// AUXIO10 pin level, read value corresponds to AUX_AIODIO1:GPIODIN bit 2.
#define AUX_EVCTL_EVSTAT1_AUXIO10                                   0x00000080
#define AUX_EVCTL_EVSTAT1_AUXIO10_BITN                                       7
#define AUX_EVCTL_EVSTAT1_AUXIO10_M                                 0x00000080
#define AUX_EVCTL_EVSTAT1_AUXIO10_S                                          7

// Field:     [6] AUXIO9
//
// AUXIO9   pin level, read value corresponds to AUX_AIODIO1:GPIODIN bit 1.
#define AUX_EVCTL_EVSTAT1_AUXIO9                                    0x00000040
#define AUX_EVCTL_EVSTAT1_AUXIO9_BITN                                        6
#define AUX_EVCTL_EVSTAT1_AUXIO9_M                                  0x00000040
#define AUX_EVCTL_EVSTAT1_AUXIO9_S                                           6

// Field:     [5] AUXIO8
//
// AUXIO8   pin level, read value corresponds to AUX_AIODIO1:GPIODIN bit 0.
#define AUX_EVCTL_EVSTAT1_AUXIO8                                    0x00000020
#define AUX_EVCTL_EVSTAT1_AUXIO8_BITN                                        5
#define AUX_EVCTL_EVSTAT1_AUXIO8_M                                  0x00000020
#define AUX_EVCTL_EVSTAT1_AUXIO8_S                                           5

// Field:     [4] AUXIO7
//
// AUXIO7   pin level, read value corresponds to AUX_AIODIO0:GPIODIN bit 7.
#define AUX_EVCTL_EVSTAT1_AUXIO7                                    0x00000010
#define AUX_EVCTL_EVSTAT1_AUXIO7_BITN                                        4
#define AUX_EVCTL_EVSTAT1_AUXIO7_M                                  0x00000010
#define AUX_EVCTL_EVSTAT1_AUXIO7_S                                           4

// Field:     [3] AUXIO6
//
// AUXIO6   pin level, read value corresponds to AUX_AIODIO0:GPIODIN bit 6.
#define AUX_EVCTL_EVSTAT1_AUXIO6                                    0x00000008
#define AUX_EVCTL_EVSTAT1_AUXIO6_BITN                                        3
#define AUX_EVCTL_EVSTAT1_AUXIO6_M                                  0x00000008
#define AUX_EVCTL_EVSTAT1_AUXIO6_S                                           3

// Field:     [2] AUXIO5
//
// AUXIO5   pin level, read value corresponds to AUX_AIODIO0:GPIODIN bit 5.
#define AUX_EVCTL_EVSTAT1_AUXIO5                                    0x00000004
#define AUX_EVCTL_EVSTAT1_AUXIO5_BITN                                        2
#define AUX_EVCTL_EVSTAT1_AUXIO5_M                                  0x00000004
#define AUX_EVCTL_EVSTAT1_AUXIO5_S                                           2

// Field:     [1] AUXIO4
//
// AUXIO4   pin level, read value corresponds to AUX_AIODIO0:GPIODIN bit 4.
#define AUX_EVCTL_EVSTAT1_AUXIO4                                    0x00000002
#define AUX_EVCTL_EVSTAT1_AUXIO4_BITN                                        1
#define AUX_EVCTL_EVSTAT1_AUXIO4_M                                  0x00000002
#define AUX_EVCTL_EVSTAT1_AUXIO4_S                                           1

// Field:     [0] AUXIO3
//
// AUXIO3   pin level, read value corresponds to AUX_AIODIO0:GPIODIN bit 3.
#define AUX_EVCTL_EVSTAT1_AUXIO3                                    0x00000001
#define AUX_EVCTL_EVSTAT1_AUXIO3_BITN                                        0
#define AUX_EVCTL_EVSTAT1_AUXIO3_M                                  0x00000001
#define AUX_EVCTL_EVSTAT1_AUXIO3_S                                           0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_EVTOMCUPOL
//
//*****************************************************************************
// Field:    [10] ADC_IRQ
//
// Select the event source level that sets EVTOMCUFLAGS.ADC_IRQ.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_ADC_IRQ                                0x00000400
#define AUX_EVCTL_EVTOMCUPOL_ADC_IRQ_BITN                                   10
#define AUX_EVCTL_EVTOMCUPOL_ADC_IRQ_M                              0x00000400
#define AUX_EVCTL_EVTOMCUPOL_ADC_IRQ_S                                      10
#define AUX_EVCTL_EVTOMCUPOL_ADC_IRQ_LOW                            0x00000400
#define AUX_EVCTL_EVTOMCUPOL_ADC_IRQ_HIGH                           0x00000000

// Field:     [9] OBSMUX0
//
// Select the event source level that sets EVTOMCUFLAGS.OBSMUX0.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_OBSMUX0                                0x00000200
#define AUX_EVCTL_EVTOMCUPOL_OBSMUX0_BITN                                    9
#define AUX_EVCTL_EVTOMCUPOL_OBSMUX0_M                              0x00000200
#define AUX_EVCTL_EVTOMCUPOL_OBSMUX0_S                                       9
#define AUX_EVCTL_EVTOMCUPOL_OBSMUX0_LOW                            0x00000200
#define AUX_EVCTL_EVTOMCUPOL_OBSMUX0_HIGH                           0x00000000

// Field:     [8] ADC_FIFO_ALMOST_FULL
//
// Select the event source level that sets EVTOMCUFLAGS.ADC_FIFO_ALMOST_FULL.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_ADC_FIFO_ALMOST_FULL                   0x00000100
#define AUX_EVCTL_EVTOMCUPOL_ADC_FIFO_ALMOST_FULL_BITN                       8
#define AUX_EVCTL_EVTOMCUPOL_ADC_FIFO_ALMOST_FULL_M                 0x00000100
#define AUX_EVCTL_EVTOMCUPOL_ADC_FIFO_ALMOST_FULL_S                          8
#define AUX_EVCTL_EVTOMCUPOL_ADC_FIFO_ALMOST_FULL_LOW               0x00000100
#define AUX_EVCTL_EVTOMCUPOL_ADC_FIFO_ALMOST_FULL_HIGH              0x00000000

// Field:     [7] ADC_DONE
//
// Select the event source level that sets EVTOMCUFLAGS.ADC_DONE.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_ADC_DONE                               0x00000080
#define AUX_EVCTL_EVTOMCUPOL_ADC_DONE_BITN                                   7
#define AUX_EVCTL_EVTOMCUPOL_ADC_DONE_M                             0x00000080
#define AUX_EVCTL_EVTOMCUPOL_ADC_DONE_S                                      7
#define AUX_EVCTL_EVTOMCUPOL_ADC_DONE_LOW                           0x00000080
#define AUX_EVCTL_EVTOMCUPOL_ADC_DONE_HIGH                          0x00000000

// Field:     [6] SMPH_AUTOTAKE_DONE
//
// Select the event source level that sets EVTOMCUFLAGS.SMPH_AUTOTAKE_DONE.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_SMPH_AUTOTAKE_DONE                     0x00000040
#define AUX_EVCTL_EVTOMCUPOL_SMPH_AUTOTAKE_DONE_BITN                         6
#define AUX_EVCTL_EVTOMCUPOL_SMPH_AUTOTAKE_DONE_M                   0x00000040
#define AUX_EVCTL_EVTOMCUPOL_SMPH_AUTOTAKE_DONE_S                            6
#define AUX_EVCTL_EVTOMCUPOL_SMPH_AUTOTAKE_DONE_LOW                 0x00000040
#define AUX_EVCTL_EVTOMCUPOL_SMPH_AUTOTAKE_DONE_HIGH                0x00000000

// Field:     [5] TIMER1_EV
//
// Select the event source level that sets EVTOMCUFLAGS.TIMER1_EV.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_TIMER1_EV                              0x00000020
#define AUX_EVCTL_EVTOMCUPOL_TIMER1_EV_BITN                                  5
#define AUX_EVCTL_EVTOMCUPOL_TIMER1_EV_M                            0x00000020
#define AUX_EVCTL_EVTOMCUPOL_TIMER1_EV_S                                     5
#define AUX_EVCTL_EVTOMCUPOL_TIMER1_EV_LOW                          0x00000020
#define AUX_EVCTL_EVTOMCUPOL_TIMER1_EV_HIGH                         0x00000000

// Field:     [4] TIMER0_EV
//
// Select the event source level that sets EVTOMCUFLAGS.TIMER0_EV.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_TIMER0_EV                              0x00000010
#define AUX_EVCTL_EVTOMCUPOL_TIMER0_EV_BITN                                  4
#define AUX_EVCTL_EVTOMCUPOL_TIMER0_EV_M                            0x00000010
#define AUX_EVCTL_EVTOMCUPOL_TIMER0_EV_S                                     4
#define AUX_EVCTL_EVTOMCUPOL_TIMER0_EV_LOW                          0x00000010
#define AUX_EVCTL_EVTOMCUPOL_TIMER0_EV_HIGH                         0x00000000

// Field:     [3] TDC_DONE
//
// Select the event source level that sets EVTOMCUFLAGS.TDC_DONE.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_TDC_DONE                               0x00000008
#define AUX_EVCTL_EVTOMCUPOL_TDC_DONE_BITN                                   3
#define AUX_EVCTL_EVTOMCUPOL_TDC_DONE_M                             0x00000008
#define AUX_EVCTL_EVTOMCUPOL_TDC_DONE_S                                      3
#define AUX_EVCTL_EVTOMCUPOL_TDC_DONE_LOW                           0x00000008
#define AUX_EVCTL_EVTOMCUPOL_TDC_DONE_HIGH                          0x00000000

// Field:     [2] AUX_COMPB
//
// Select the event source level that sets EVTOMCUFLAGS.AUX_COMPB.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPB                              0x00000004
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPB_BITN                                  2
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPB_M                            0x00000004
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPB_S                                     2
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPB_LOW                          0x00000004
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPB_HIGH                         0x00000000

// Field:     [1] AUX_COMPA
//
// Select the event source level that sets EVTOMCUFLAGS.AUX_COMPA.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPA                              0x00000002
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPA_BITN                                  1
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPA_M                            0x00000002
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPA_S                                     1
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPA_LOW                          0x00000002
#define AUX_EVCTL_EVTOMCUPOL_AUX_COMPA_HIGH                         0x00000000

// Field:     [0] AON_WU_EV
//
// Select the event source level that sets EVTOMCUFLAGS.AON_WU_EV.
// ENUMs:
// LOW                      Low level
// HIGH                     High level
#define AUX_EVCTL_EVTOMCUPOL_AON_WU_EV                              0x00000001
#define AUX_EVCTL_EVTOMCUPOL_AON_WU_EV_BITN                                  0
#define AUX_EVCTL_EVTOMCUPOL_AON_WU_EV_M                            0x00000001
#define AUX_EVCTL_EVTOMCUPOL_AON_WU_EV_S                                     0
#define AUX_EVCTL_EVTOMCUPOL_AON_WU_EV_LOW                          0x00000001
#define AUX_EVCTL_EVTOMCUPOL_AON_WU_EV_HIGH                         0x00000000

//*****************************************************************************
//
// Register: AUX_EVCTL_O_EVTOMCUFLAGS
//
//*****************************************************************************
// Field:    [10] ADC_IRQ
//
// This event flag is set when level selected by EVTOMCUPOL.ADC_IRQ occurs on
// EVSTAT0.ADC_IRQ.
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ                              0x00000400
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ_BITN                                 10
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ_M                            0x00000400
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ_S                                    10

// Field:     [9] OBSMUX0
//
// This event flag is set when level selected by EVTOMCUPOL.MCU_OBSMUX0 occurs
// on EVSTAT0.MCU_OBSMUX0.
#define AUX_EVCTL_EVTOMCUFLAGS_OBSMUX0                              0x00000200
#define AUX_EVCTL_EVTOMCUFLAGS_OBSMUX0_BITN                                  9
#define AUX_EVCTL_EVTOMCUFLAGS_OBSMUX0_M                            0x00000200
#define AUX_EVCTL_EVTOMCUFLAGS_OBSMUX0_S                                     9

// Field:     [8] ADC_FIFO_ALMOST_FULL
//
// This event flag is set when level selected by
// EVTOMCUPOL.ADC_FIFO_ALMOST_FULL occurs on EVSTAT0.ADC_FIFO_ALMOST_FULL.
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_FIFO_ALMOST_FULL                 0x00000100
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_FIFO_ALMOST_FULL_BITN                     8
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_FIFO_ALMOST_FULL_M               0x00000100
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_FIFO_ALMOST_FULL_S                        8

// Field:     [7] ADC_DONE
//
// This event flag is set when level selected by EVTOMCUPOL.ADC_DONE occurs on
// EVSTAT0.ADC_DONE.
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE                             0x00000080
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE_BITN                                 7
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE_M                           0x00000080
#define AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE_S                                    7

// Field:     [6] SMPH_AUTOTAKE_DONE
//
// This event flag is set when level selected by EVTOMCUPOL.SMPH_AUTOTAKE_DONE
// occurs on EVSTAT0.SMPH_AUTOTAKE_DONE.
#define AUX_EVCTL_EVTOMCUFLAGS_SMPH_AUTOTAKE_DONE                   0x00000040
#define AUX_EVCTL_EVTOMCUFLAGS_SMPH_AUTOTAKE_DONE_BITN                       6
#define AUX_EVCTL_EVTOMCUFLAGS_SMPH_AUTOTAKE_DONE_M                 0x00000040
#define AUX_EVCTL_EVTOMCUFLAGS_SMPH_AUTOTAKE_DONE_S                          6

// Field:     [5] TIMER1_EV
//
// This event flag is set when level selected by EVTOMCUPOL.TIMER1_EV occurs on
// EVSTAT0.TIMER1_EV.
#define AUX_EVCTL_EVTOMCUFLAGS_TIMER1_EV                            0x00000020
#define AUX_EVCTL_EVTOMCUFLAGS_TIMER1_EV_BITN                                5
#define AUX_EVCTL_EVTOMCUFLAGS_TIMER1_EV_M                          0x00000020
#define AUX_EVCTL_EVTOMCUFLAGS_TIMER1_EV_S                                   5

// Field:     [4] TIMER0_EV
//
// This event flag is set when level selected by EVTOMCUPOL.TIMER0_EV occurs on
// EVSTAT0.TIMER0_EV.
#define AUX_EVCTL_EVTOMCUFLAGS_TIMER0_EV                            0x00000010
#define AUX_EVCTL_EVTOMCUFLAGS_TIMER0_EV_BITN                                4
#define AUX_EVCTL_EVTOMCUFLAGS_TIMER0_EV_M                          0x00000010
#define AUX_EVCTL_EVTOMCUFLAGS_TIMER0_EV_S                                   4

// Field:     [3] TDC_DONE
//
// This event flag is set when level selected by EVTOMCUPOL.TDC_DONE occurs on
// EVSTAT0.TDC_DONE.
#define AUX_EVCTL_EVTOMCUFLAGS_TDC_DONE                             0x00000008
#define AUX_EVCTL_EVTOMCUFLAGS_TDC_DONE_BITN                                 3
#define AUX_EVCTL_EVTOMCUFLAGS_TDC_DONE_M                           0x00000008
#define AUX_EVCTL_EVTOMCUFLAGS_TDC_DONE_S                                    3

// Field:     [2] AUX_COMPB
//
// This event flag is set when edge selected by EVTOMCUPOL.AUX_COMPB occurs on
// EVSTAT0.AUX_COMPB.
#define AUX_EVCTL_EVTOMCUFLAGS_AUX_COMPB                            0x00000004
#define AUX_EVCTL_EVTOMCUFLAGS_AUX_COMPB_BITN                                2
#define AUX_EVCTL_EVTOMCUFLAGS_AUX_COMPB_M                          0x00000004
#define AUX_EVCTL_EVTOMCUFLAGS_AUX_COMPB_S                                   2

// Field:     [1] AUX_COMPA
//
// This event flag is set when edge selected by EVTOMCUPOL.AUX_COMPA occurs on
// EVSTAT0.AUX_COMPA.
#define AUX_EVCTL_EVTOMCUFLAGS_AUX_COMPA                            0x00000002
#define AUX_EVCTL_EVTOMCUFLAGS_AUX_COMPA_BITN                                1
#define AUX_EVCTL_EVTOMCUFLAGS_AUX_COMPA_M                          0x00000002
#define AUX_EVCTL_EVTOMCUFLAGS_AUX_COMPA_S                                   1

// Field:     [0] AON_WU_EV
//
// This event flag is set when level selected by EVTOMCUPOL.AON_WU_EV occurs on
// the reduction-OR of the AUX_EVCTL:EVSTAT0.RTC_CH2_EV,
// AUX_EVCTL:EVSTAT0.AON_SW, and AUX_EVCTL:EVSTAT0.AON_PROG_WU events.
#define AUX_EVCTL_EVTOMCUFLAGS_AON_WU_EV                            0x00000001
#define AUX_EVCTL_EVTOMCUFLAGS_AON_WU_EV_BITN                                0
#define AUX_EVCTL_EVTOMCUFLAGS_AON_WU_EV_M                          0x00000001
#define AUX_EVCTL_EVTOMCUFLAGS_AON_WU_EV_S                                   0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_COMBEVTOMCUMASK
//
//*****************************************************************************
// Field:    [10] ADC_IRQ
//
// EVTOMCUFLAGS.ADC_IRQ contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_IRQ                           0x00000400
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_IRQ_BITN                              10
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_IRQ_M                         0x00000400
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_IRQ_S                                 10

// Field:     [9] OBSMUX0
//
// EVTOMCUFLAGS.MCU_OBSMUX0 contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_OBSMUX0                           0x00000200
#define AUX_EVCTL_COMBEVTOMCUMASK_OBSMUX0_BITN                               9
#define AUX_EVCTL_COMBEVTOMCUMASK_OBSMUX0_M                         0x00000200
#define AUX_EVCTL_COMBEVTOMCUMASK_OBSMUX0_S                                  9

// Field:     [8] ADC_FIFO_ALMOST_FULL
//
// EVTOMCUFLAGS.ADC_FIFO_ALMOST_FULL contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_FIFO_ALMOST_FULL              0x00000100
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_FIFO_ALMOST_FULL_BITN                  8
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_FIFO_ALMOST_FULL_M            0x00000100
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_FIFO_ALMOST_FULL_S                     8

// Field:     [7] ADC_DONE
//
// EVTOMCUFLAGS.ADC_DONE contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_DONE                          0x00000080
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_DONE_BITN                              7
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_DONE_M                        0x00000080
#define AUX_EVCTL_COMBEVTOMCUMASK_ADC_DONE_S                                 7

// Field:     [6] SMPH_AUTOTAKE_DONE
//
// EVTOMCUFLAGS.SMPH_AUTOTAKE_DONE contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_SMPH_AUTOTAKE_DONE                0x00000040
#define AUX_EVCTL_COMBEVTOMCUMASK_SMPH_AUTOTAKE_DONE_BITN                    6
#define AUX_EVCTL_COMBEVTOMCUMASK_SMPH_AUTOTAKE_DONE_M              0x00000040
#define AUX_EVCTL_COMBEVTOMCUMASK_SMPH_AUTOTAKE_DONE_S                       6

// Field:     [5] TIMER1_EV
//
// EVTOMCUFLAGS.TIMER1_EV contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_TIMER1_EV                         0x00000020
#define AUX_EVCTL_COMBEVTOMCUMASK_TIMER1_EV_BITN                             5
#define AUX_EVCTL_COMBEVTOMCUMASK_TIMER1_EV_M                       0x00000020
#define AUX_EVCTL_COMBEVTOMCUMASK_TIMER1_EV_S                                5

// Field:     [4] TIMER0_EV
//
// EVTOMCUFLAGS.TIMER0_EV contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_TIMER0_EV                         0x00000010
#define AUX_EVCTL_COMBEVTOMCUMASK_TIMER0_EV_BITN                             4
#define AUX_EVCTL_COMBEVTOMCUMASK_TIMER0_EV_M                       0x00000010
#define AUX_EVCTL_COMBEVTOMCUMASK_TIMER0_EV_S                                4

// Field:     [3] TDC_DONE
//
// EVTOMCUFLAGS.TDC_DONE contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_TDC_DONE                          0x00000008
#define AUX_EVCTL_COMBEVTOMCUMASK_TDC_DONE_BITN                              3
#define AUX_EVCTL_COMBEVTOMCUMASK_TDC_DONE_M                        0x00000008
#define AUX_EVCTL_COMBEVTOMCUMASK_TDC_DONE_S                                 3

// Field:     [2] AUX_COMPB
//
// EVTOMCUFLAGS.AUX_COMPB contribution to the AUX_COMB event.
//
// 0: Exclude
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_AUX_COMPB                         0x00000004
#define AUX_EVCTL_COMBEVTOMCUMASK_AUX_COMPB_BITN                             2
#define AUX_EVCTL_COMBEVTOMCUMASK_AUX_COMPB_M                       0x00000004
#define AUX_EVCTL_COMBEVTOMCUMASK_AUX_COMPB_S                                2

// Field:     [1] AUX_COMPA
//
// EVTOMCUFLAGS.AUX_COMPA contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_AUX_COMPA                         0x00000002
#define AUX_EVCTL_COMBEVTOMCUMASK_AUX_COMPA_BITN                             1
#define AUX_EVCTL_COMBEVTOMCUMASK_AUX_COMPA_M                       0x00000002
#define AUX_EVCTL_COMBEVTOMCUMASK_AUX_COMPA_S                                1

// Field:     [0] AON_WU_EV
//
// EVTOMCUFLAGS.AON_WU_EV contribution to the AUX_COMB event.
//
// 0: Exclude.
// 1: Include.
#define AUX_EVCTL_COMBEVTOMCUMASK_AON_WU_EV                         0x00000001
#define AUX_EVCTL_COMBEVTOMCUMASK_AON_WU_EV_BITN                             0
#define AUX_EVCTL_COMBEVTOMCUMASK_AON_WU_EV_M                       0x00000001
#define AUX_EVCTL_COMBEVTOMCUMASK_AON_WU_EV_S                                0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_VECFLAGS
//
//*****************************************************************************
// Field:     [3] VEC3
//
// Vector flag 3.
//
// The vector flag is set if the edge selected VECCFG1.VEC3_POL occurs on the
// event selected in VECCFG1.VEC3_EV.
//
// The flag is cleared by writing a 0 to this bit, or (preferably) a 1 to
// VECFLAGSCLR.VEC3.
#define AUX_EVCTL_VECFLAGS_VEC3                                     0x00000008
#define AUX_EVCTL_VECFLAGS_VEC3_BITN                                         3
#define AUX_EVCTL_VECFLAGS_VEC3_M                                   0x00000008
#define AUX_EVCTL_VECFLAGS_VEC3_S                                            3

// Field:     [2] VEC2
//
// Vector flag 2.
//
// The vector flag is set if the edge selected VECCFG1.VEC2_POL occurs on the
// event selected in VECCFG1.VEC2_EV.
//
// The flag is cleared by writing a 0 to this bit, or (preferably) a 1 to
// VECFLAGSCLR.VEC2.
#define AUX_EVCTL_VECFLAGS_VEC2                                     0x00000004
#define AUX_EVCTL_VECFLAGS_VEC2_BITN                                         2
#define AUX_EVCTL_VECFLAGS_VEC2_M                                   0x00000004
#define AUX_EVCTL_VECFLAGS_VEC2_S                                            2

// Field:     [1] VEC1
//
// Vector flag 1.
//
// The vector flag is set if the edge selected VECCFG0.VEC1_POL occurs on the
// event selected in VECCFG0.VEC1_EV.
//
// The flag is cleared by writing a 0 to this bit, or (preferably) a 1 to
// VECFLAGSCLR.VEC1.
#define AUX_EVCTL_VECFLAGS_VEC1                                     0x00000002
#define AUX_EVCTL_VECFLAGS_VEC1_BITN                                         1
#define AUX_EVCTL_VECFLAGS_VEC1_M                                   0x00000002
#define AUX_EVCTL_VECFLAGS_VEC1_S                                            1

// Field:     [0] VEC0
//
// Vector flag 0.
//
// The vector flag is set if the edge selected VECCFG0.VEC0_POL occurs on the
// event selected in VECCFG0.VEC0_EV.
//
// The flag is cleared by writing a 0 to this bit, or (preferably) a 1 to
// VECFLAGSCLR.VEC0.
#define AUX_EVCTL_VECFLAGS_VEC0                                     0x00000001
#define AUX_EVCTL_VECFLAGS_VEC0_BITN                                         0
#define AUX_EVCTL_VECFLAGS_VEC0_M                                   0x00000001
#define AUX_EVCTL_VECFLAGS_VEC0_S                                            0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_EVTOMCUFLAGSCLR
//
//*****************************************************************************
// Field:    [10] ADC_IRQ
//
// Write 1 to clear EVTOMCUFLAGS.ADC_IRQ.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_IRQ                           0x00000400
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_IRQ_BITN                              10
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_IRQ_M                         0x00000400
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_IRQ_S                                 10

// Field:     [9] OBSMUX0
//
// Write 1 to clear EVTOMCUFLAGS.MCU_OBSMUX0.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_OBSMUX0                           0x00000200
#define AUX_EVCTL_EVTOMCUFLAGSCLR_OBSMUX0_BITN                               9
#define AUX_EVCTL_EVTOMCUFLAGSCLR_OBSMUX0_M                         0x00000200
#define AUX_EVCTL_EVTOMCUFLAGSCLR_OBSMUX0_S                                  9

// Field:     [8] ADC_FIFO_ALMOST_FULL
//
// Write 1 to clear EVTOMCUFLAGS.ADC_FIFO_ALMOST_FULL.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_FIFO_ALMOST_FULL              0x00000100
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_FIFO_ALMOST_FULL_BITN                  8
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_FIFO_ALMOST_FULL_M            0x00000100
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_FIFO_ALMOST_FULL_S                     8

// Field:     [7] ADC_DONE
//
// Write 1 to clear EVTOMCUFLAGS.ADC_DONE.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_DONE                          0x00000080
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_DONE_BITN                              7
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_DONE_M                        0x00000080
#define AUX_EVCTL_EVTOMCUFLAGSCLR_ADC_DONE_S                                 7

// Field:     [6] SMPH_AUTOTAKE_DONE
//
// Write 1 to clear EVTOMCUFLAGS.SMPH_AUTOTAKE_DONE.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_SMPH_AUTOTAKE_DONE                0x00000040
#define AUX_EVCTL_EVTOMCUFLAGSCLR_SMPH_AUTOTAKE_DONE_BITN                    6
#define AUX_EVCTL_EVTOMCUFLAGSCLR_SMPH_AUTOTAKE_DONE_M              0x00000040
#define AUX_EVCTL_EVTOMCUFLAGSCLR_SMPH_AUTOTAKE_DONE_S                       6

// Field:     [5] TIMER1_EV
//
// Write 1 to clear EVTOMCUFLAGS.TIMER1_EV.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TIMER1_EV                         0x00000020
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TIMER1_EV_BITN                             5
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TIMER1_EV_M                       0x00000020
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TIMER1_EV_S                                5

// Field:     [4] TIMER0_EV
//
// Write 1 to clear EVTOMCUFLAGS.TIMER0_EV.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TIMER0_EV                         0x00000010
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TIMER0_EV_BITN                             4
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TIMER0_EV_M                       0x00000010
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TIMER0_EV_S                                4

// Field:     [3] TDC_DONE
//
// Write 1 to clear EVTOMCUFLAGS.TDC_DONE.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TDC_DONE                          0x00000008
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TDC_DONE_BITN                              3
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TDC_DONE_M                        0x00000008
#define AUX_EVCTL_EVTOMCUFLAGSCLR_TDC_DONE_S                                 3

// Field:     [2] AUX_COMPB
//
// Write 1 to clear EVTOMCUFLAGS.AUX_COMPB.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AUX_COMPB                         0x00000004
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AUX_COMPB_BITN                             2
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AUX_COMPB_M                       0x00000004
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AUX_COMPB_S                                2

// Field:     [1] AUX_COMPA
//
// Write 1 to clear EVTOMCUFLAGS.AUX_COMPA.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AUX_COMPA                         0x00000002
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AUX_COMPA_BITN                             1
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AUX_COMPA_M                       0x00000002
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AUX_COMPA_S                                1

// Field:     [0] AON_WU_EV
//
// Write 1 to clear EVTOMCUFLAGS.AON_WU_EV.
//
// Read value is 0.
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AON_WU_EV                         0x00000001
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AON_WU_EV_BITN                             0
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AON_WU_EV_M                       0x00000001
#define AUX_EVCTL_EVTOMCUFLAGSCLR_AON_WU_EV_S                                0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_EVTOAONFLAGSCLR
//
//*****************************************************************************
// Field:     [8] TIMER1_EV
//
// Write 1 to clear EVTOAONFLAGS.TIMER1_EV.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_TIMER1_EV                         0x00000100
#define AUX_EVCTL_EVTOAONFLAGSCLR_TIMER1_EV_BITN                             8
#define AUX_EVCTL_EVTOAONFLAGSCLR_TIMER1_EV_M                       0x00000100
#define AUX_EVCTL_EVTOAONFLAGSCLR_TIMER1_EV_S                                8

// Field:     [7] TIMER0_EV
//
// Write 1 to clear EVTOAONFLAGS.TIMER0_EV.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_TIMER0_EV                         0x00000080
#define AUX_EVCTL_EVTOAONFLAGSCLR_TIMER0_EV_BITN                             7
#define AUX_EVCTL_EVTOAONFLAGSCLR_TIMER0_EV_M                       0x00000080
#define AUX_EVCTL_EVTOAONFLAGSCLR_TIMER0_EV_S                                7

// Field:     [6] TDC_DONE
//
// Write 1 to clear EVTOAONFLAGS.TDC_DONE.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_TDC_DONE                          0x00000040
#define AUX_EVCTL_EVTOAONFLAGSCLR_TDC_DONE_BITN                              6
#define AUX_EVCTL_EVTOAONFLAGSCLR_TDC_DONE_M                        0x00000040
#define AUX_EVCTL_EVTOAONFLAGSCLR_TDC_DONE_S                                 6

// Field:     [5] ADC_DONE
//
// Write 1 to clear EVTOAONFLAGS.ADC_DONE.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_ADC_DONE                          0x00000020
#define AUX_EVCTL_EVTOAONFLAGSCLR_ADC_DONE_BITN                              5
#define AUX_EVCTL_EVTOAONFLAGSCLR_ADC_DONE_M                        0x00000020
#define AUX_EVCTL_EVTOAONFLAGSCLR_ADC_DONE_S                                 5

// Field:     [4] AUX_COMPB
//
// Write 1 to clear EVTOAONFLAGS.AUX_COMPB.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_AUX_COMPB                         0x00000010
#define AUX_EVCTL_EVTOAONFLAGSCLR_AUX_COMPB_BITN                             4
#define AUX_EVCTL_EVTOAONFLAGSCLR_AUX_COMPB_M                       0x00000010
#define AUX_EVCTL_EVTOAONFLAGSCLR_AUX_COMPB_S                                4

// Field:     [3] AUX_COMPA
//
// Write 1 to clear EVTOAONFLAGS.AUX_COMPA.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_AUX_COMPA                         0x00000008
#define AUX_EVCTL_EVTOAONFLAGSCLR_AUX_COMPA_BITN                             3
#define AUX_EVCTL_EVTOAONFLAGSCLR_AUX_COMPA_M                       0x00000008
#define AUX_EVCTL_EVTOAONFLAGSCLR_AUX_COMPA_S                                3

// Field:     [2] SWEV2
//
// Write 1 to clear EVTOAONFLAGS.SWEV2.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV2                             0x00000004
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV2_BITN                                 2
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV2_M                           0x00000004
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV2_S                                    2

// Field:     [1] SWEV1
//
// Write 1 to clear EVTOAONFLAGS.SWEV1.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV1                             0x00000002
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV1_BITN                                 1
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV1_M                           0x00000002
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV1_S                                    1

// Field:     [0] SWEV0
//
// Write 1 to clear EVTOAONFLAGS.SWEV0.
//
// Read value is 0.
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV0                             0x00000001
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV0_BITN                                 0
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV0_M                           0x00000001
#define AUX_EVCTL_EVTOAONFLAGSCLR_SWEV0_S                                    0

//*****************************************************************************
//
// Register: AUX_EVCTL_O_VECFLAGSCLR
//
//*****************************************************************************
// Field:     [3] VEC3
//
// Clear vector flag 3.
//
// 0: No effect.
// 1: Clear VECFLAGS.VEC3.
//
// Read value is 0.
#define AUX_EVCTL_VECFLAGSCLR_VEC3                                  0x00000008
#define AUX_EVCTL_VECFLAGSCLR_VEC3_BITN                                      3
#define AUX_EVCTL_VECFLAGSCLR_VEC3_M                                0x00000008
#define AUX_EVCTL_VECFLAGSCLR_VEC3_S                                         3

// Field:     [2] VEC2
//
// Clear vector flag 2.
//
// 0: No effect.
// 1: Clear VECFLAGS.VEC2.
//
// Read value is 0.
#define AUX_EVCTL_VECFLAGSCLR_VEC2                                  0x00000004
#define AUX_EVCTL_VECFLAGSCLR_VEC2_BITN                                      2
#define AUX_EVCTL_VECFLAGSCLR_VEC2_M                                0x00000004
#define AUX_EVCTL_VECFLAGSCLR_VEC2_S                                         2

// Field:     [1] VEC1
//
// Clear vector flag 1.
//
// 0: No effect.
// 1: Clear VECFLAGS.VEC1.
//
// Read value is 0.
#define AUX_EVCTL_VECFLAGSCLR_VEC1                                  0x00000002
#define AUX_EVCTL_VECFLAGSCLR_VEC1_BITN                                      1
#define AUX_EVCTL_VECFLAGSCLR_VEC1_M                                0x00000002
#define AUX_EVCTL_VECFLAGSCLR_VEC1_S                                         1

// Field:     [0] VEC0
//
// Clear vector flag 0.
//
// 0: No effect.
// 1: Clear VECFLAGS.VEC0.
//
// Read value is 0.
#define AUX_EVCTL_VECFLAGSCLR_VEC0                                  0x00000001
#define AUX_EVCTL_VECFLAGSCLR_VEC0_BITN                                      0
#define AUX_EVCTL_VECFLAGSCLR_VEC0_M                                0x00000001
#define AUX_EVCTL_VECFLAGSCLR_VEC0_S                                         0


#endif // __AUX_EVCTL__
