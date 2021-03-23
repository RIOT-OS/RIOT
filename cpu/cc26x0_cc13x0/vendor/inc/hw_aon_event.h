/******************************************************************************
*  Filename:       hw_aon_event_h
*  Revised:        2017-01-31 09:37:48 +0100 (Tue, 31 Jan 2017)
*  Revision:       48345
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

#ifndef __HW_AON_EVENT_H__
#define __HW_AON_EVENT_H__

//*****************************************************************************
//
// This section defines the register offsets of
// AON_EVENT component
//
//*****************************************************************************
// Wake-up Selector For MCU
#define AON_EVENT_O_MCUWUSEL                                        0x00000000

// Wake-up Selector For AUX
#define AON_EVENT_O_AUXWUSEL                                        0x00000004

// Event Selector For MCU Event Fabric
#define AON_EVENT_O_EVTOMCUSEL                                      0x00000008

// RTC Capture Event Selector For AON_RTC
#define AON_EVENT_O_RTCSEL                                          0x0000000C

//*****************************************************************************
//
// Register: AON_EVENT_O_MCUWUSEL
//
//*****************************************************************************
// Field: [29:24] WU3_EV
//
// MCU Wakeup Source #3
//
// AON Event Source selecting 1 of 4 events routed to AON_WUC for waking up the
// MCU domain from Power Off or Power Down.
// Note:
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_MCUWUSEL_WU3_EV_W                                          6
#define AON_EVENT_MCUWUSEL_WU3_EV_M                                 0x3F000000
#define AON_EVENT_MCUWUSEL_WU3_EV_S                                         24
#define AON_EVENT_MCUWUSEL_WU3_EV_NONE                              0x3F000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_COMPB_ASYNC_N                 0x38000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_COMPB_ASYNC                   0x37000000
#define AON_EVENT_MCUWUSEL_WU3_EV_BATMON_VOLT                       0x36000000
#define AON_EVENT_MCUWUSEL_WU3_EV_BATMON_TEMP                       0x35000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_TIMER1_EV                     0x34000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_TIMER0_EV                     0x33000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_TDC_DONE                      0x32000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_ADC_DONE                      0x31000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_COMPB                         0x30000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_COMPA                         0x2F000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_SWEV2                         0x2E000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_SWEV1                         0x2D000000
#define AON_EVENT_MCUWUSEL_WU3_EV_AUX_SWEV0                         0x2C000000
#define AON_EVENT_MCUWUSEL_WU3_EV_JTAG                              0x2B000000
#define AON_EVENT_MCUWUSEL_WU3_EV_RTC_UPD                           0x2A000000
#define AON_EVENT_MCUWUSEL_WU3_EV_RTC_COMB_DLY                      0x29000000
#define AON_EVENT_MCUWUSEL_WU3_EV_RTC_CH2_DLY                       0x28000000
#define AON_EVENT_MCUWUSEL_WU3_EV_RTC_CH1_DLY                       0x27000000
#define AON_EVENT_MCUWUSEL_WU3_EV_RTC_CH0_DLY                       0x26000000
#define AON_EVENT_MCUWUSEL_WU3_EV_RTC_CH2                           0x25000000
#define AON_EVENT_MCUWUSEL_WU3_EV_RTC_CH1                           0x24000000
#define AON_EVENT_MCUWUSEL_WU3_EV_RTC_CH0                           0x23000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD                               0x20000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD31                             0x1F000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD30                             0x1E000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD29                             0x1D000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD28                             0x1C000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD27                             0x1B000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD26                             0x1A000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD25                             0x19000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD24                             0x18000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD23                             0x17000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD22                             0x16000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD21                             0x15000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD20                             0x14000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD19                             0x13000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD18                             0x12000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD17                             0x11000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD16                             0x10000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD15                             0x0F000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD14                             0x0E000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD13                             0x0D000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD12                             0x0C000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD11                             0x0B000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD10                             0x0A000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD9                              0x09000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD8                              0x08000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD7                              0x07000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD6                              0x06000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD5                              0x05000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD4                              0x04000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD3                              0x03000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD2                              0x02000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD1                              0x01000000
#define AON_EVENT_MCUWUSEL_WU3_EV_PAD0                              0x00000000

// Field: [21:16] WU2_EV
//
// MCU Wakeup Source #2
//
// AON Event Source selecting 1 of 4 events routed to AON_WUC for waking up the
// MCU domain from Power Off or Power Down.
// Note:
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_MCUWUSEL_WU2_EV_W                                          6
#define AON_EVENT_MCUWUSEL_WU2_EV_M                                 0x003F0000
#define AON_EVENT_MCUWUSEL_WU2_EV_S                                         16
#define AON_EVENT_MCUWUSEL_WU2_EV_NONE                              0x003F0000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_COMPB_ASYNC_N                 0x00380000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_COMPB_ASYNC                   0x00370000
#define AON_EVENT_MCUWUSEL_WU2_EV_BATMON_VOLT                       0x00360000
#define AON_EVENT_MCUWUSEL_WU2_EV_BATMON_TEMP                       0x00350000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_TIMER1_EV                     0x00340000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_TIMER0_EV                     0x00330000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_TDC_DONE                      0x00320000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_ADC_DONE                      0x00310000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_COMPB                         0x00300000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_COMPA                         0x002F0000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_SWEV2                         0x002E0000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_SWEV1                         0x002D0000
#define AON_EVENT_MCUWUSEL_WU2_EV_AUX_SWEV0                         0x002C0000
#define AON_EVENT_MCUWUSEL_WU2_EV_JTAG                              0x002B0000
#define AON_EVENT_MCUWUSEL_WU2_EV_RTC_UPD                           0x002A0000
#define AON_EVENT_MCUWUSEL_WU2_EV_RTC_COMB_DLY                      0x00290000
#define AON_EVENT_MCUWUSEL_WU2_EV_RTC_CH2_DLY                       0x00280000
#define AON_EVENT_MCUWUSEL_WU2_EV_RTC_CH1_DLY                       0x00270000
#define AON_EVENT_MCUWUSEL_WU2_EV_RTC_CH0_DLY                       0x00260000
#define AON_EVENT_MCUWUSEL_WU2_EV_RTC_CH2                           0x00250000
#define AON_EVENT_MCUWUSEL_WU2_EV_RTC_CH1                           0x00240000
#define AON_EVENT_MCUWUSEL_WU2_EV_RTC_CH0                           0x00230000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD                               0x00200000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD31                             0x001F0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD30                             0x001E0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD29                             0x001D0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD28                             0x001C0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD27                             0x001B0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD26                             0x001A0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD25                             0x00190000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD24                             0x00180000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD23                             0x00170000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD22                             0x00160000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD21                             0x00150000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD20                             0x00140000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD19                             0x00130000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD18                             0x00120000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD17                             0x00110000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD16                             0x00100000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD15                             0x000F0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD14                             0x000E0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD13                             0x000D0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD12                             0x000C0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD11                             0x000B0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD10                             0x000A0000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD9                              0x00090000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD8                              0x00080000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD7                              0x00070000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD6                              0x00060000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD5                              0x00050000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD4                              0x00040000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD3                              0x00030000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD2                              0x00020000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD1                              0x00010000
#define AON_EVENT_MCUWUSEL_WU2_EV_PAD0                              0x00000000

// Field:  [13:8] WU1_EV
//
// MCU Wakeup Source #1
//
// AON Event Source selecting 1 of 4 events routed to AON_WUC for waking up the
// MCU domain from Power Off or Power Down.
// Note:
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_MCUWUSEL_WU1_EV_W                                          6
#define AON_EVENT_MCUWUSEL_WU1_EV_M                                 0x00003F00
#define AON_EVENT_MCUWUSEL_WU1_EV_S                                          8
#define AON_EVENT_MCUWUSEL_WU1_EV_NONE                              0x00003F00
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_COMPB_ASYNC_N                 0x00003800
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_COMPB_ASYNC                   0x00003700
#define AON_EVENT_MCUWUSEL_WU1_EV_BATMON_VOLT                       0x00003600
#define AON_EVENT_MCUWUSEL_WU1_EV_BATMON_TEMP                       0x00003500
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_TIMER1_EV                     0x00003400
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_TIMER0_EV                     0x00003300
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_TDC_DONE                      0x00003200
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_ADC_DONE                      0x00003100
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_COMPB                         0x00003000
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_COMPA                         0x00002F00
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_SWEV2                         0x00002E00
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_SWEV1                         0x00002D00
#define AON_EVENT_MCUWUSEL_WU1_EV_AUX_SWEV0                         0x00002C00
#define AON_EVENT_MCUWUSEL_WU1_EV_JTAG                              0x00002B00
#define AON_EVENT_MCUWUSEL_WU1_EV_RTC_UPD                           0x00002A00
#define AON_EVENT_MCUWUSEL_WU1_EV_RTC_COMB_DLY                      0x00002900
#define AON_EVENT_MCUWUSEL_WU1_EV_RTC_CH2_DLY                       0x00002800
#define AON_EVENT_MCUWUSEL_WU1_EV_RTC_CH1_DLY                       0x00002700
#define AON_EVENT_MCUWUSEL_WU1_EV_RTC_CH0_DLY                       0x00002600
#define AON_EVENT_MCUWUSEL_WU1_EV_RTC_CH2                           0x00002500
#define AON_EVENT_MCUWUSEL_WU1_EV_RTC_CH1                           0x00002400
#define AON_EVENT_MCUWUSEL_WU1_EV_RTC_CH0                           0x00002300
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD                               0x00002000
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD31                             0x00001F00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD30                             0x00001E00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD29                             0x00001D00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD28                             0x00001C00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD27                             0x00001B00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD26                             0x00001A00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD25                             0x00001900
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD24                             0x00001800
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD23                             0x00001700
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD22                             0x00001600
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD21                             0x00001500
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD20                             0x00001400
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD19                             0x00001300
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD18                             0x00001200
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD17                             0x00001100
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD16                             0x00001000
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD15                             0x00000F00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD14                             0x00000E00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD13                             0x00000D00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD12                             0x00000C00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD11                             0x00000B00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD10                             0x00000A00
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD9                              0x00000900
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD8                              0x00000800
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD7                              0x00000700
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD6                              0x00000600
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD5                              0x00000500
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD4                              0x00000400
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD3                              0x00000300
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD2                              0x00000200
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD1                              0x00000100
#define AON_EVENT_MCUWUSEL_WU1_EV_PAD0                              0x00000000

// Field:   [5:0] WU0_EV
//
// MCU Wakeup Source #0
//
// AON Event Source selecting 1 of 4 events routed to AON_WUC for waking up the
// MCU domain from Power Off or Power Down.
// Note:
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_MCUWUSEL_WU0_EV_W                                          6
#define AON_EVENT_MCUWUSEL_WU0_EV_M                                 0x0000003F
#define AON_EVENT_MCUWUSEL_WU0_EV_S                                          0
#define AON_EVENT_MCUWUSEL_WU0_EV_NONE                              0x0000003F
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_COMPB_ASYNC_N                 0x00000038
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_COMPB_ASYNC                   0x00000037
#define AON_EVENT_MCUWUSEL_WU0_EV_BATMON_VOLT                       0x00000036
#define AON_EVENT_MCUWUSEL_WU0_EV_BATMON_TEMP                       0x00000035
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_TIMER1_EV                     0x00000034
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_TIMER0_EV                     0x00000033
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_TDC_DONE                      0x00000032
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_ADC_DONE                      0x00000031
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_COMPB                         0x00000030
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_COMPA                         0x0000002F
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_SWEV2                         0x0000002E
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_SWEV1                         0x0000002D
#define AON_EVENT_MCUWUSEL_WU0_EV_AUX_SWEV0                         0x0000002C
#define AON_EVENT_MCUWUSEL_WU0_EV_JTAG                              0x0000002B
#define AON_EVENT_MCUWUSEL_WU0_EV_RTC_UPD                           0x0000002A
#define AON_EVENT_MCUWUSEL_WU0_EV_RTC_COMB_DLY                      0x00000029
#define AON_EVENT_MCUWUSEL_WU0_EV_RTC_CH2_DLY                       0x00000028
#define AON_EVENT_MCUWUSEL_WU0_EV_RTC_CH1_DLY                       0x00000027
#define AON_EVENT_MCUWUSEL_WU0_EV_RTC_CH0_DLY                       0x00000026
#define AON_EVENT_MCUWUSEL_WU0_EV_RTC_CH2                           0x00000025
#define AON_EVENT_MCUWUSEL_WU0_EV_RTC_CH1                           0x00000024
#define AON_EVENT_MCUWUSEL_WU0_EV_RTC_CH0                           0x00000023
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD                               0x00000020
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD31                             0x0000001F
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD30                             0x0000001E
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD29                             0x0000001D
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD28                             0x0000001C
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD27                             0x0000001B
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD26                             0x0000001A
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD25                             0x00000019
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD24                             0x00000018
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD23                             0x00000017
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD22                             0x00000016
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD21                             0x00000015
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD20                             0x00000014
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD19                             0x00000013
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD18                             0x00000012
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD17                             0x00000011
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD16                             0x00000010
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD15                             0x0000000F
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD14                             0x0000000E
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD13                             0x0000000D
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD12                             0x0000000C
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD11                             0x0000000B
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD10                             0x0000000A
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD9                              0x00000009
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD8                              0x00000008
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD7                              0x00000007
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD6                              0x00000006
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD5                              0x00000005
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD4                              0x00000004
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD3                              0x00000003
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD2                              0x00000002
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD1                              0x00000001
#define AON_EVENT_MCUWUSEL_WU0_EV_PAD0                              0x00000000

//*****************************************************************************
//
// Register: AON_EVENT_O_AUXWUSEL
//
//*****************************************************************************
// Field: [21:16] WU2_EV
//
// AUX Wakeup Source #2
//
// AON Event Source selecting 1 of 3 events routed to AON_WUC for waking up the
// AUX domain from Power Off or Power Down.
// Note:
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_AUXWUSEL_WU2_EV_W                                          6
#define AON_EVENT_AUXWUSEL_WU2_EV_M                                 0x003F0000
#define AON_EVENT_AUXWUSEL_WU2_EV_S                                         16
#define AON_EVENT_AUXWUSEL_WU2_EV_NONE                              0x003F0000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_COMPB_ASYNC_N                 0x00380000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_COMPB_ASYNC                   0x00370000
#define AON_EVENT_AUXWUSEL_WU2_EV_BATMON_VOLT                       0x00360000
#define AON_EVENT_AUXWUSEL_WU2_EV_BATMON_TEMP                       0x00350000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_TIMER1_EV                     0x00340000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_TIMER0_EV                     0x00330000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_TDC_DONE                      0x00320000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_ADC_DONE                      0x00310000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_COMPB                         0x00300000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_COMPA                         0x002F0000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_SWEV2                         0x002E0000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_SWEV1                         0x002D0000
#define AON_EVENT_AUXWUSEL_WU2_EV_AUX_SWEV0                         0x002C0000
#define AON_EVENT_AUXWUSEL_WU2_EV_JTAG                              0x002B0000
#define AON_EVENT_AUXWUSEL_WU2_EV_RTC_UPD                           0x002A0000
#define AON_EVENT_AUXWUSEL_WU2_EV_RTC_COMB_DLY                      0x00290000
#define AON_EVENT_AUXWUSEL_WU2_EV_RTC_CH2_DLY                       0x00280000
#define AON_EVENT_AUXWUSEL_WU2_EV_RTC_CH1_DLY                       0x00270000
#define AON_EVENT_AUXWUSEL_WU2_EV_RTC_CH0_DLY                       0x00260000
#define AON_EVENT_AUXWUSEL_WU2_EV_RTC_CH2                           0x00250000
#define AON_EVENT_AUXWUSEL_WU2_EV_RTC_CH1                           0x00240000
#define AON_EVENT_AUXWUSEL_WU2_EV_RTC_CH0                           0x00230000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD                               0x00200000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD31                             0x001F0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD30                             0x001E0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD29                             0x001D0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD28                             0x001C0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD27                             0x001B0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD26                             0x001A0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD25                             0x00190000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD24                             0x00180000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD23                             0x00170000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD22                             0x00160000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD21                             0x00150000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD20                             0x00140000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD19                             0x00130000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD18                             0x00120000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD17                             0x00110000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD16                             0x00100000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD15                             0x000F0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD14                             0x000E0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD13                             0x000D0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD12                             0x000C0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD11                             0x000B0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD10                             0x000A0000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD9                              0x00090000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD8                              0x00080000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD7                              0x00070000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD6                              0x00060000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD5                              0x00050000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD4                              0x00040000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD3                              0x00030000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD2                              0x00020000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD1                              0x00010000
#define AON_EVENT_AUXWUSEL_WU2_EV_PAD0                              0x00000000

// Field:  [13:8] WU1_EV
//
// AUX Wakeup Source #1
//
// AON Event Source selecting 1 of 3 events routed to AON_WUC for waking up the
// AUX domain from Power Off or Power Down.
// Note:
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_AUXWUSEL_WU1_EV_W                                          6
#define AON_EVENT_AUXWUSEL_WU1_EV_M                                 0x00003F00
#define AON_EVENT_AUXWUSEL_WU1_EV_S                                          8
#define AON_EVENT_AUXWUSEL_WU1_EV_NONE                              0x00003F00
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_COMPB_ASYNC_N                 0x00003800
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_COMPB_ASYNC                   0x00003700
#define AON_EVENT_AUXWUSEL_WU1_EV_BATMON_VOLT                       0x00003600
#define AON_EVENT_AUXWUSEL_WU1_EV_BATMON_TEMP                       0x00003500
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_TIMER1_EV                     0x00003400
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_TIMER0_EV                     0x00003300
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_TDC_DONE                      0x00003200
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_ADC_DONE                      0x00003100
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_COMPB                         0x00003000
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_COMPA                         0x00002F00
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_SWEV2                         0x00002E00
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_SWEV1                         0x00002D00
#define AON_EVENT_AUXWUSEL_WU1_EV_AUX_SWEV0                         0x00002C00
#define AON_EVENT_AUXWUSEL_WU1_EV_JTAG                              0x00002B00
#define AON_EVENT_AUXWUSEL_WU1_EV_RTC_UPD                           0x00002A00
#define AON_EVENT_AUXWUSEL_WU1_EV_RTC_COMB_DLY                      0x00002900
#define AON_EVENT_AUXWUSEL_WU1_EV_RTC_CH2_DLY                       0x00002800
#define AON_EVENT_AUXWUSEL_WU1_EV_RTC_CH1_DLY                       0x00002700
#define AON_EVENT_AUXWUSEL_WU1_EV_RTC_CH0_DLY                       0x00002600
#define AON_EVENT_AUXWUSEL_WU1_EV_RTC_CH2                           0x00002500
#define AON_EVENT_AUXWUSEL_WU1_EV_RTC_CH1                           0x00002400
#define AON_EVENT_AUXWUSEL_WU1_EV_RTC_CH0                           0x00002300
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD                               0x00002000
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD31                             0x00001F00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD30                             0x00001E00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD29                             0x00001D00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD28                             0x00001C00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD27                             0x00001B00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD26                             0x00001A00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD25                             0x00001900
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD24                             0x00001800
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD23                             0x00001700
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD22                             0x00001600
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD21                             0x00001500
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD20                             0x00001400
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD19                             0x00001300
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD18                             0x00001200
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD17                             0x00001100
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD16                             0x00001000
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD15                             0x00000F00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD14                             0x00000E00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD13                             0x00000D00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD12                             0x00000C00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD11                             0x00000B00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD10                             0x00000A00
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD9                              0x00000900
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD8                              0x00000800
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD7                              0x00000700
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD6                              0x00000600
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD5                              0x00000500
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD4                              0x00000400
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD3                              0x00000300
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD2                              0x00000200
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD1                              0x00000100
#define AON_EVENT_AUXWUSEL_WU1_EV_PAD0                              0x00000000

// Field:   [5:0] WU0_EV
//
// AUX Wakeup Source #0
//
// AON Event Source selecting 1 of 3 events routed to AON_WUC for waking up the
// AUX domain from Power Off or Power Down.
// Note:
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_AUXWUSEL_WU0_EV_W                                          6
#define AON_EVENT_AUXWUSEL_WU0_EV_M                                 0x0000003F
#define AON_EVENT_AUXWUSEL_WU0_EV_S                                          0
#define AON_EVENT_AUXWUSEL_WU0_EV_NONE                              0x0000003F
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_COMPB_ASYNC_N                 0x00000038
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_COMPB_ASYNC                   0x00000037
#define AON_EVENT_AUXWUSEL_WU0_EV_BATMON_VOLT                       0x00000036
#define AON_EVENT_AUXWUSEL_WU0_EV_BATMON_TEMP                       0x00000035
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_TIMER1_EV                     0x00000034
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_TIMER0_EV                     0x00000033
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_TDC_DONE                      0x00000032
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_ADC_DONE                      0x00000031
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_COMPB                         0x00000030
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_COMPA                         0x0000002F
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_SWEV2                         0x0000002E
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_SWEV1                         0x0000002D
#define AON_EVENT_AUXWUSEL_WU0_EV_AUX_SWEV0                         0x0000002C
#define AON_EVENT_AUXWUSEL_WU0_EV_JTAG                              0x0000002B
#define AON_EVENT_AUXWUSEL_WU0_EV_RTC_UPD                           0x0000002A
#define AON_EVENT_AUXWUSEL_WU0_EV_RTC_COMB_DLY                      0x00000029
#define AON_EVENT_AUXWUSEL_WU0_EV_RTC_CH2_DLY                       0x00000028
#define AON_EVENT_AUXWUSEL_WU0_EV_RTC_CH1_DLY                       0x00000027
#define AON_EVENT_AUXWUSEL_WU0_EV_RTC_CH0_DLY                       0x00000026
#define AON_EVENT_AUXWUSEL_WU0_EV_RTC_CH2                           0x00000025
#define AON_EVENT_AUXWUSEL_WU0_EV_RTC_CH1                           0x00000024
#define AON_EVENT_AUXWUSEL_WU0_EV_RTC_CH0                           0x00000023
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD                               0x00000020
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD31                             0x0000001F
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD30                             0x0000001E
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD29                             0x0000001D
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD28                             0x0000001C
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD27                             0x0000001B
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD26                             0x0000001A
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD25                             0x00000019
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD24                             0x00000018
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD23                             0x00000017
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD22                             0x00000016
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD21                             0x00000015
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD20                             0x00000014
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD19                             0x00000013
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD18                             0x00000012
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD17                             0x00000011
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD16                             0x00000010
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD15                             0x0000000F
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD14                             0x0000000E
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD13                             0x0000000D
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD12                             0x0000000C
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD11                             0x0000000B
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD10                             0x0000000A
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD9                              0x00000009
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD8                              0x00000008
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD7                              0x00000007
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD6                              0x00000006
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD5                              0x00000005
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD4                              0x00000004
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD3                              0x00000003
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD2                              0x00000002
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD1                              0x00000001
#define AON_EVENT_AUXWUSEL_WU0_EV_PAD0                              0x00000000

//*****************************************************************************
//
// Register: AON_EVENT_O_EVTOMCUSEL
//
//*****************************************************************************
// Field: [21:16] AON_PROG2_EV
//
// Event selector for AON_PROG2 event.
//
// AON Event Source id# selecting event routed to EVENT as AON_PROG2 event.
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_W                                  6
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_M                         0x003F0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_S                                 16
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_NONE                      0x003F0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_COMPB_ASYNC_N         0x00380000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_COMPB_ASYNC           0x00370000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_BATMON_VOLT               0x00360000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_BATMON_TEMP               0x00350000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_TIMER1_EV             0x00340000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_TIMER0_EV             0x00330000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_TDC_DONE              0x00320000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_ADC_DONE              0x00310000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_COMPB                 0x00300000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_COMPA                 0x002F0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_SWEV2                 0x002E0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_SWEV1                 0x002D0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_AUX_SWEV0                 0x002C0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_JTAG                      0x002B0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_RTC_UPD                   0x002A0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_RTC_COMB_DLY              0x00290000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_RTC_CH2_DLY               0x00280000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_RTC_CH1_DLY               0x00270000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_RTC_CH0_DLY               0x00260000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_RTC_CH2                   0x00250000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_RTC_CH1                   0x00240000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_RTC_CH0                   0x00230000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD                       0x00200000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD31                     0x001F0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD30                     0x001E0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD29                     0x001D0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD28                     0x001C0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD27                     0x001B0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD26                     0x001A0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD25                     0x00190000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD24                     0x00180000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD23                     0x00170000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD22                     0x00160000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD21                     0x00150000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD20                     0x00140000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD19                     0x00130000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD18                     0x00120000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD17                     0x00110000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD16                     0x00100000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD15                     0x000F0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD14                     0x000E0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD13                     0x000D0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD12                     0x000C0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD11                     0x000B0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD10                     0x000A0000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD9                      0x00090000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD8                      0x00080000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD7                      0x00070000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD6                      0x00060000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD5                      0x00050000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD4                      0x00040000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD3                      0x00030000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD2                      0x00020000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD1                      0x00010000
#define AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_PAD0                      0x00000000

// Field:  [13:8] AON_PROG1_EV
//
// Event selector for AON_PROG1 event.
//
// AON Event Source id# selecting event routed to EVENT as AON_PROG1 event.
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_W                                  6
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_M                         0x00003F00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_S                                  8
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_NONE                      0x00003F00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_COMPB_ASYNC_N         0x00003800
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_COMPB_ASYNC           0x00003700
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_BATMON_VOLT               0x00003600
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_BATMON_TEMP               0x00003500
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_TIMER1_EV             0x00003400
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_TIMER0_EV             0x00003300
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_TDC_DONE              0x00003200
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_ADC_DONE              0x00003100
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_COMPB                 0x00003000
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_COMPA                 0x00002F00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_SWEV2                 0x00002E00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_SWEV1                 0x00002D00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_AUX_SWEV0                 0x00002C00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_JTAG                      0x00002B00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_RTC_UPD                   0x00002A00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_RTC_COMB_DLY              0x00002900
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_RTC_CH2_DLY               0x00002800
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_RTC_CH1_DLY               0x00002700
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_RTC_CH0_DLY               0x00002600
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_RTC_CH2                   0x00002500
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_RTC_CH1                   0x00002400
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_RTC_CH0                   0x00002300
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD                       0x00002000
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD31                     0x00001F00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD30                     0x00001E00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD29                     0x00001D00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD28                     0x00001C00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD27                     0x00001B00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD26                     0x00001A00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD25                     0x00001900
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD24                     0x00001800
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD23                     0x00001700
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD22                     0x00001600
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD21                     0x00001500
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD20                     0x00001400
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD19                     0x00001300
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD18                     0x00001200
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD17                     0x00001100
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD16                     0x00001000
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD15                     0x00000F00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD14                     0x00000E00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD13                     0x00000D00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD12                     0x00000C00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD11                     0x00000B00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD10                     0x00000A00
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD9                      0x00000900
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD8                      0x00000800
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD7                      0x00000700
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD6                      0x00000600
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD5                      0x00000500
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD4                      0x00000400
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD3                      0x00000300
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD2                      0x00000200
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD1                      0x00000100
#define AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_PAD0                      0x00000000

// Field:   [5:0] AON_PROG0_EV
//
// Event selector for AON_PROG0 event.
//
// AON Event Source id# selecting event routed to EVENT as AON_PROG0 event.
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_W                                  6
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_M                         0x0000003F
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_S                                  0
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_NONE                      0x0000003F
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_COMPB_ASYNC_N         0x00000038
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_COMPB_ASYNC           0x00000037
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_BATMON_VOLT               0x00000036
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_BATMON_TEMP               0x00000035
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_TIMER1_EV             0x00000034
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_TIMER0_EV             0x00000033
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_TDC_DONE              0x00000032
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_ADC_DONE              0x00000031
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_COMPB                 0x00000030
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_COMPA                 0x0000002F
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_SWEV2                 0x0000002E
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_SWEV1                 0x0000002D
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_AUX_SWEV0                 0x0000002C
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_JTAG                      0x0000002B
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_RTC_UPD                   0x0000002A
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_RTC_COMB_DLY              0x00000029
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_RTC_CH2_DLY               0x00000028
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_RTC_CH1_DLY               0x00000027
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_RTC_CH0_DLY               0x00000026
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_RTC_CH2                   0x00000025
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_RTC_CH1                   0x00000024
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_RTC_CH0                   0x00000023
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD                       0x00000020
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD31                     0x0000001F
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD30                     0x0000001E
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD29                     0x0000001D
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD28                     0x0000001C
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD27                     0x0000001B
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD26                     0x0000001A
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD25                     0x00000019
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD24                     0x00000018
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD23                     0x00000017
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD22                     0x00000016
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD21                     0x00000015
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD20                     0x00000014
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD19                     0x00000013
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD18                     0x00000012
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD17                     0x00000011
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD16                     0x00000010
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD15                     0x0000000F
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD14                     0x0000000E
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD13                     0x0000000D
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD12                     0x0000000C
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD11                     0x0000000B
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD10                     0x0000000A
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD9                      0x00000009
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD8                      0x00000008
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD7                      0x00000007
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD6                      0x00000006
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD5                      0x00000005
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD4                      0x00000004
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD3                      0x00000003
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD2                      0x00000002
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD1                      0x00000001
#define AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_PAD0                      0x00000000

//*****************************************************************************
//
// Register: AON_EVENT_O_RTCSEL
//
//*****************************************************************************
// Field:   [5:0] RTC_CH1_CAPT_EV
//
// AON Event Source id# for RTCSEL event which is fed to AON_RTC. Please refer
// to AON_RTC:CH1CAPT
// ENUMs:
// NONE                     No event, always low
// AUX_COMPB_ASYNC_N        Comparator B not triggered. Asynchronous signal
//                          directly from AUX Comparator B (inverted) as
//                          opposed to AUX_COMPB which is synchronized in
//                          AUX
// AUX_COMPB_ASYNC          Comparator B triggered. Asynchronous signal
//                          directly from the AUX Comparator B as opposed
//                          to AUX_COMPB which is synchronized in AUX
// BATMON_VOLT              BATMON voltage update event
// BATMON_TEMP              BATMON temperature update event
// AUX_TIMER1_EV            AUX Timer 1 Event
// AUX_TIMER0_EV            AUX Timer 0 Event
// AUX_TDC_DONE             TDC completed or timed out
// AUX_ADC_DONE             ADC conversion completed
// AUX_COMPB                Comparator B triggered
// AUX_COMPA                Comparator A triggered
// AUX_SWEV2                AUX Software triggered event #2. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV2
// AUX_SWEV1                AUX Software triggered event #1. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV1
// AUX_SWEV0                AUX Software triggered event #0. Triggered by
//                          AUX_EVCTL:SWEVSET.SWEV0
// JTAG                     JTAG generated event
// RTC_UPD                  RTC Update Tick (16 kHz signal, i.e. event line
//                          toggles value every 32 kHz clock period)
// RTC_COMB_DLY             RTC combined delayed event
// RTC_CH2_DLY              RTC channel 2 - delayed event
// RTC_CH1_DLY              RTC channel 1 - delayed event
// RTC_CH0_DLY              RTC channel 0 - delayed event
// RTC_CH2                  RTC channel 2 event
// RTC_CH1                  RTC channel 1 event
// RTC_CH0                  RTC channel 0 event
// PAD                      Edge detect on any PAD
// PAD31                    Edge detect on PAD31
// PAD30                    Edge detect on PAD30
// PAD29                    Edge detect on PAD29
// PAD28                    Edge detect on PAD28
// PAD27                    Edge detect on PAD27
// PAD26                    Edge detect on PAD26
// PAD25                    Edge detect on PAD25
// PAD24                    Edge detect on PAD24
// PAD23                    Edge detect on PAD23
// PAD22                    Edge detect on PAD22
// PAD21                    Edge detect on PAD21
// PAD20                    Edge detect on PAD20
// PAD19                    Edge detect on PAD19
// PAD18                    Edge detect on PAD18
// PAD17                    Edge detect on PAD17
// PAD16                    Edge detect on PAD16
// PAD15                    Edge detect on PAD15
// PAD14                    Edge detect on PAD14
// PAD13                    Edge detect on PAD13
// PAD12                    Edge detect on PAD12
// PAD11                    Edge detect on PAD11
// PAD10                    Edge detect on PAD10
// PAD9                     Edge detect on PAD9
// PAD8                     Edge detect on PAD8
// PAD7                     Edge detect on PAD7
// PAD6                     Edge detect on PAD6
// PAD5                     Edge detect on PAD5
// PAD4                     Edge detect on PAD4
// PAD3                     Edge detect on PAD3
// PAD2                     Edge detect on PAD2
// PAD1                     Edge detect on PAD1
// PAD0                     Edge detect on PAD0
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_W                                   6
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_M                          0x0000003F
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_S                                   0
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_NONE                       0x0000003F
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_COMPB_ASYNC_N          0x00000038
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_COMPB_ASYNC            0x00000037
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_BATMON_VOLT                0x00000036
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_BATMON_TEMP                0x00000035
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_TIMER1_EV              0x00000034
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_TIMER0_EV              0x00000033
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_TDC_DONE               0x00000032
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_ADC_DONE               0x00000031
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_COMPB                  0x00000030
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_COMPA                  0x0000002F
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_SWEV2                  0x0000002E
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_SWEV1                  0x0000002D
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_AUX_SWEV0                  0x0000002C
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_JTAG                       0x0000002B
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_RTC_UPD                    0x0000002A
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_RTC_COMB_DLY               0x00000029
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_RTC_CH2_DLY                0x00000028
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_RTC_CH1_DLY                0x00000027
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_RTC_CH0_DLY                0x00000026
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_RTC_CH2                    0x00000025
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_RTC_CH1                    0x00000024
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_RTC_CH0                    0x00000023
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD                        0x00000020
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD31                      0x0000001F
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD30                      0x0000001E
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD29                      0x0000001D
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD28                      0x0000001C
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD27                      0x0000001B
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD26                      0x0000001A
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD25                      0x00000019
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD24                      0x00000018
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD23                      0x00000017
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD22                      0x00000016
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD21                      0x00000015
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD20                      0x00000014
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD19                      0x00000013
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD18                      0x00000012
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD17                      0x00000011
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD16                      0x00000010
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD15                      0x0000000F
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD14                      0x0000000E
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD13                      0x0000000D
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD12                      0x0000000C
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD11                      0x0000000B
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD10                      0x0000000A
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD9                       0x00000009
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD8                       0x00000008
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD7                       0x00000007
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD6                       0x00000006
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD5                       0x00000005
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD4                       0x00000004
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD3                       0x00000003
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD2                       0x00000002
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD1                       0x00000001
#define AON_EVENT_RTCSEL_RTC_CH1_CAPT_EV_PAD0                       0x00000000


#endif // __AON_EVENT__
