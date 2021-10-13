/******************************************************************************
*  Filename:       hw_aux_anaif_h
*  Revised:        2017-05-30 11:42:02 +0200 (Tue, 30 May 2017)
*  Revision:       49074
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

#ifndef __HW_AUX_ANAIF_H__
#define __HW_AUX_ANAIF_H__

//*****************************************************************************
//
// This section defines the register offsets of
// AUX_ANAIF component
//
//*****************************************************************************
// ADC Control
#define AUX_ANAIF_O_ADCCTL                                          0x00000010

// ADC FIFO Status
#define AUX_ANAIF_O_ADCFIFOSTAT                                     0x00000014

// ADC FIFO
#define AUX_ANAIF_O_ADCFIFO                                         0x00000018

// ADC Trigger
#define AUX_ANAIF_O_ADCTRIG                                         0x0000001C

// Current Source Control
#define AUX_ANAIF_O_ISRCCTL                                         0x00000020

//*****************************************************************************
//
// Register: AUX_ANAIF_O_ADCCTL
//
//*****************************************************************************
// Field:    [13] START_POL
//
// Select active polarity for START_SRC event.
// ENUMs:
// FALL                     Set ADC trigger on falling edge of event source.
// RISE                     Set ADC trigger on rising edge of event source.
#define AUX_ANAIF_ADCCTL_START_POL                                  0x00002000
#define AUX_ANAIF_ADCCTL_START_POL_BITN                                     13
#define AUX_ANAIF_ADCCTL_START_POL_M                                0x00002000
#define AUX_ANAIF_ADCCTL_START_POL_S                                        13
#define AUX_ANAIF_ADCCTL_START_POL_FALL                             0x00002000
#define AUX_ANAIF_ADCCTL_START_POL_RISE                             0x00000000

// Field:  [12:8] START_SRC
//
// Select ADC trigger event source from the asynchronous AUX event bus.
//
// Set START_SRC to NO_EVENT<n> if you want to trigger the ADC manually through
// ADCTRIG.START.
// ENUMs:
// ADC_IRQ                  AUX_EVCTL:EVSTAT1.ADC_IRQ
// MCU_EV                   AUX_EVCTL:EVSTAT1.MCU_EV
// ACLK_REF                 AUX_EVCTL:EVSTAT1.ACLK_REF
// AUXIO15                  AUX_EVCTL:EVSTAT1.AUXIO15
// AUXIO14                  AUX_EVCTL:EVSTAT1.AUXIO14
// AUXIO13                  AUX_EVCTL:EVSTAT1.AUXIO13
// AUXIO12                  AUX_EVCTL:EVSTAT1.AUXIO12
// AUXIO11                  AUX_EVCTL:EVSTAT1.AUXIO11
// AUXIO10                  AUX_EVCTL:EVSTAT1.AUXIO10
// AUXIO9                   AUX_EVCTL:EVSTAT1.AUXIO9
// AUXIO8                   AUX_EVCTL:EVSTAT1.AUXIO8
// AUXIO7                   AUX_EVCTL:EVSTAT1.AUXIO7
// AUXIO6                   AUX_EVCTL:EVSTAT1.AUXIO6
// AUXIO5                   AUX_EVCTL:EVSTAT1.AUXIO5
// AUXIO4                   AUX_EVCTL:EVSTAT1.AUXIO4
// AUXIO3                   AUX_EVCTL:EVSTAT1.AUXIO3
// AUXIO2                   AUX_EVCTL:EVSTAT0.AUXIO2
// AUXIO1                   AUX_EVCTL:EVSTAT0.AUXIO1
// AUXIO0                   AUX_EVCTL:EVSTAT0.AUXIO0
// AON_PROG_WU              AUX_EVCTL:EVSTAT0.AON_PROG_WU
// AON_SW                   AUX_EVCTL:EVSTAT0.AON_SW
// NO_EVENT1                No event.
// NO_EVENT0                No event.
// RESERVED1                Reserved - Do not use.
// RESERVED0                Reserved - Do not use.
// SMPH_AUTOTAKE_DONE       AUX_EVCTL:EVSTAT0.SMPH_AUTOTAKE_DONE
// TIMER1_EV                AUX_EVCTL:EVSTAT0.TIMER1_EV
// TIMER0_EV                AUX_EVCTL:EVSTAT0.TIMER0_EV
// TDC_DONE                 AUX_EVCTL:EVSTAT0.TDC_DONE
// AUX_COMPB                AUX_EVCTL:EVSTAT0.AUX_COMPB
// AUX_COMPA                AUX_EVCTL:EVSTAT0.AUX_COMPA
// RTC_CH2_EV               AUX_EVCTL:EVSTAT0.AON_RTC_CH2
#define AUX_ANAIF_ADCCTL_START_SRC_W                                         5
#define AUX_ANAIF_ADCCTL_START_SRC_M                                0x00001F00
#define AUX_ANAIF_ADCCTL_START_SRC_S                                         8
#define AUX_ANAIF_ADCCTL_START_SRC_ADC_IRQ                          0x00001F00
#define AUX_ANAIF_ADCCTL_START_SRC_MCU_EV                           0x00001E00
#define AUX_ANAIF_ADCCTL_START_SRC_ACLK_REF                         0x00001D00
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO15                          0x00001C00
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO14                          0x00001B00
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO13                          0x00001A00
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO12                          0x00001900
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO11                          0x00001800
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO10                          0x00001700
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO9                           0x00001600
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO8                           0x00001500
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO7                           0x00001400
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO6                           0x00001300
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO5                           0x00001200
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO4                           0x00001100
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO3                           0x00001000
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO2                           0x00000F00
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO1                           0x00000E00
#define AUX_ANAIF_ADCCTL_START_SRC_AUXIO0                           0x00000D00
#define AUX_ANAIF_ADCCTL_START_SRC_AON_PROG_WU                      0x00000C00
#define AUX_ANAIF_ADCCTL_START_SRC_AON_SW                           0x00000B00
#define AUX_ANAIF_ADCCTL_START_SRC_NO_EVENT1                        0x00000A00
#define AUX_ANAIF_ADCCTL_START_SRC_NO_EVENT0                        0x00000900
#define AUX_ANAIF_ADCCTL_START_SRC_RESERVED1                        0x00000800
#define AUX_ANAIF_ADCCTL_START_SRC_RESERVED0                        0x00000700
#define AUX_ANAIF_ADCCTL_START_SRC_SMPH_AUTOTAKE_DONE               0x00000600
#define AUX_ANAIF_ADCCTL_START_SRC_TIMER1_EV                        0x00000500
#define AUX_ANAIF_ADCCTL_START_SRC_TIMER0_EV                        0x00000400
#define AUX_ANAIF_ADCCTL_START_SRC_TDC_DONE                         0x00000300
#define AUX_ANAIF_ADCCTL_START_SRC_AUX_COMPB                        0x00000200
#define AUX_ANAIF_ADCCTL_START_SRC_AUX_COMPA                        0x00000100
#define AUX_ANAIF_ADCCTL_START_SRC_RTC_CH2_EV                       0x00000000

// Field:   [1:0] CMD
//
// ADC interface command.
//
// Non-enumerated values are not supported. The written value is returned when
// read.
// ENUMs:
// FLUSH                    Flush ADC FIFO.
//
//                          You must set CMD to EN or
//                          DIS after flush.
//
//                          System CPU must wait two
//                          clock cycles before it sets CMD to EN or DIS.
// EN                       Enable ADC interface.
// DIS                      Disable ADC interface.
#define AUX_ANAIF_ADCCTL_CMD_W                                               2
#define AUX_ANAIF_ADCCTL_CMD_M                                      0x00000003
#define AUX_ANAIF_ADCCTL_CMD_S                                               0
#define AUX_ANAIF_ADCCTL_CMD_FLUSH                                  0x00000003
#define AUX_ANAIF_ADCCTL_CMD_EN                                     0x00000001
#define AUX_ANAIF_ADCCTL_CMD_DIS                                    0x00000000

//*****************************************************************************
//
// Register: AUX_ANAIF_O_ADCFIFOSTAT
//
//*****************************************************************************
// Field:     [4] OVERFLOW
//
// FIFO overflow flag.
//
// 0: FIFO has not overflowed.
// 1: FIFO has overflowed, this flag is sticky until you flush the FIFO.
//
// When the flag is set, the ADC FIFO write pointer is static. It is not
// possible to add more samples to the ADC FIFO. Flush FIFO to clear the flag.
#define AUX_ANAIF_ADCFIFOSTAT_OVERFLOW                              0x00000010
#define AUX_ANAIF_ADCFIFOSTAT_OVERFLOW_BITN                                  4
#define AUX_ANAIF_ADCFIFOSTAT_OVERFLOW_M                            0x00000010
#define AUX_ANAIF_ADCFIFOSTAT_OVERFLOW_S                                     4

// Field:     [3] UNDERFLOW
//
// FIFO underflow flag.
//
// 0: FIFO has not underflowed.
// 1: FIFO has underflowed, this flag is sticky until you flush the FIFO.
//
// When the flag is set, the ADC FIFO read pointer is static. Read returns the
// previous sample that was read. Flush FIFO to clear the flag.
#define AUX_ANAIF_ADCFIFOSTAT_UNDERFLOW                             0x00000008
#define AUX_ANAIF_ADCFIFOSTAT_UNDERFLOW_BITN                                 3
#define AUX_ANAIF_ADCFIFOSTAT_UNDERFLOW_M                           0x00000008
#define AUX_ANAIF_ADCFIFOSTAT_UNDERFLOW_S                                    3

// Field:     [2] FULL
//
// FIFO full flag.
//
// 0: FIFO is not full, there is less than 4 samples in the FIFO.
// 1: FIFO is full, there are 4 samples in the FIFO.
//
// When the flag is set, it is not possible to add more samples to the ADC
// FIFO. An attempt to add samples sets the OVERFLOW flag.
#define AUX_ANAIF_ADCFIFOSTAT_FULL                                  0x00000004
#define AUX_ANAIF_ADCFIFOSTAT_FULL_BITN                                      2
#define AUX_ANAIF_ADCFIFOSTAT_FULL_M                                0x00000004
#define AUX_ANAIF_ADCFIFOSTAT_FULL_S                                         2

// Field:     [1] ALMOST_FULL
//
// FIFO almost full flag.
//
// 0: There are less than 3 samples in the FIFO, or the FIFO is full. The FULL
// flag is also asserted in the latter case.
// 1: There are 3 samples in the FIFO, there is room for one more sample.
#define AUX_ANAIF_ADCFIFOSTAT_ALMOST_FULL                           0x00000002
#define AUX_ANAIF_ADCFIFOSTAT_ALMOST_FULL_BITN                               1
#define AUX_ANAIF_ADCFIFOSTAT_ALMOST_FULL_M                         0x00000002
#define AUX_ANAIF_ADCFIFOSTAT_ALMOST_FULL_S                                  1

// Field:     [0] EMPTY
//
// FIFO empty flag.
//
// 0: FIFO contains one or more samples.
// 1: FIFO is empty.
//
// When the flag is set, read returns the previous sample that was read and
// sets the UNDERFLOW flag.
#define AUX_ANAIF_ADCFIFOSTAT_EMPTY                                 0x00000001
#define AUX_ANAIF_ADCFIFOSTAT_EMPTY_BITN                                     0
#define AUX_ANAIF_ADCFIFOSTAT_EMPTY_M                               0x00000001
#define AUX_ANAIF_ADCFIFOSTAT_EMPTY_S                                        0

//*****************************************************************************
//
// Register: AUX_ANAIF_O_ADCFIFO
//
//*****************************************************************************
// Field:  [11:0] DATA
//
// FIFO data.
//
// Read:
// Get oldest ADC sample from FIFO.
//
// Write:
// Write dummy sample to FIFO. This is useful for code development when you do
// not have real ADC samples.
#define AUX_ANAIF_ADCFIFO_DATA_W                                            12
#define AUX_ANAIF_ADCFIFO_DATA_M                                    0x00000FFF
#define AUX_ANAIF_ADCFIFO_DATA_S                                             0

//*****************************************************************************
//
// Register: AUX_ANAIF_O_ADCTRIG
//
//*****************************************************************************
// Field:     [0] START
//
// Manual ADC trigger.
//
// 0: No effect.
// 1: Single ADC trigger.
//
// To manually trigger the ADC, you must set ADCCTL.START_SRC to NO_EVENT<n> to
// avoid conflict with event-driven ADC trigger.
#define AUX_ANAIF_ADCTRIG_START                                     0x00000001
#define AUX_ANAIF_ADCTRIG_START_BITN                                         0
#define AUX_ANAIF_ADCTRIG_START_M                                   0x00000001
#define AUX_ANAIF_ADCTRIG_START_S                                            0

//*****************************************************************************
//
// Register: AUX_ANAIF_O_ISRCCTL
//
//*****************************************************************************
// Field:     [0] RESET_N
//
// ISRC reset control.
//
// 0: ISRC drives 0 uA.
// 1: ISRC drives current ADI_4_AUX:ISRC.TRIM to COMPA_IN.
#define AUX_ANAIF_ISRCCTL_RESET_N                                   0x00000001
#define AUX_ANAIF_ISRCCTL_RESET_N_BITN                                       0
#define AUX_ANAIF_ISRCCTL_RESET_N_M                                 0x00000001
#define AUX_ANAIF_ISRCCTL_RESET_N_S                                          0


#endif // __AUX_ANAIF__
