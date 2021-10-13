/******************************************************************************
*  Filename:       hw_aux_aiodio_h
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

#ifndef __HW_AUX_AIODIO_H__
#define __HW_AUX_AIODIO_H__

//*****************************************************************************
//
// This section defines the register offsets of
// AUX_AIODIO component
//
//*****************************************************************************
// General Purpose Input Output Data Out
#define AUX_AIODIO_O_GPIODOUT                                       0x00000000

// Input Output Mode
#define AUX_AIODIO_O_IOMODE                                         0x00000004

// General Purpose Input Output Data In
#define AUX_AIODIO_O_GPIODIN                                        0x00000008

// General Purpose Input Output Data Out Set
#define AUX_AIODIO_O_GPIODOUTSET                                    0x0000000C

// General Purpose Input Output Data Out Clear
#define AUX_AIODIO_O_GPIODOUTCLR                                    0x00000010

// General Purpose Input Output Data Out Toggle
#define AUX_AIODIO_O_GPIODOUTTGL                                    0x00000014

// General Purpose Input Output Digital Input Enable
#define AUX_AIODIO_O_GPIODIE                                        0x00000018

//*****************************************************************************
//
// Register: AUX_AIODIO_O_GPIODOUT
//
//*****************************************************************************
// Field:   [7:0] IO7_0
//
// Write 1 to bit index n in this bit vector to set AUXIO[8i+n].
// Write 0 to bit index n in this bit vector to clear AUXIO[8i+n].
#define AUX_AIODIO_GPIODOUT_IO7_0_W                                          8
#define AUX_AIODIO_GPIODOUT_IO7_0_M                                 0x000000FF
#define AUX_AIODIO_GPIODOUT_IO7_0_S                                          0

//*****************************************************************************
//
// Register: AUX_AIODIO_O_IOMODE
//
//*****************************************************************************
// Field: [15:14] IO7
//
// Select mode for AUXIO[8i+7].
// ENUMs:
// OPEN_SOURCE              Open-Source Mode:
//
//                          When GPIODOUT bit 7 is 0:
//                          AUXIO[8i+7] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
//
//                          When GPIODOUT bit 7 is 1:
//                          AUXIO[8i+7] is driven high.
// OPEN_DRAIN               Open-Drain Mode:
//
//                          When GPIODOUT bit 7 is 0:
//                          AUXIO[8i+7] is driven low.
//
//                          When GPIODOUT bit 7 is 1:
//                          AUXIO[8i+7] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
// IN                       Input Mode:
//
//                          When GPIODIE bit 7 is 0:
//                          AUXIO[8i+7] is enabled for analog signal
//                          transfer.
//
//                          When GPIODIE bit 7 is 1:
//                          AUXIO[8i+7] is enabled for digital input.
// OUT                      Output Mode:
//
//                          GPIODOUT bit 7 drives
//                          AUXIO[8i+7].
#define AUX_AIODIO_IOMODE_IO7_W                                              2
#define AUX_AIODIO_IOMODE_IO7_M                                     0x0000C000
#define AUX_AIODIO_IOMODE_IO7_S                                             14
#define AUX_AIODIO_IOMODE_IO7_OPEN_SOURCE                           0x0000C000
#define AUX_AIODIO_IOMODE_IO7_OPEN_DRAIN                            0x00008000
#define AUX_AIODIO_IOMODE_IO7_IN                                    0x00004000
#define AUX_AIODIO_IOMODE_IO7_OUT                                   0x00000000

// Field: [13:12] IO6
//
// Select mode for AUXIO[8i+6].
// ENUMs:
// OPEN_SOURCE              Open-Source Mode:
//
//                          When GPIODOUT bit 6 is 0:
//                          AUXIO[8i+6] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
//
//                          When GPIODOUT bit 6 is 1:
//                          AUXIO[8i+6] is driven high.
// OPEN_DRAIN               Open-Drain Mode:
//
//                          When GPIODOUT bit 6 is 0:
//                          AUXIO[8i+6] is driven low.
//
//                          When GPIODOUT bit 6 is 1:
//                          AUXIO[8i+6] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
// IN                       Input Mode:
//
//                          When GPIODIE bit 6 is 0:
//                          AUXIO[8i+6] is enabled for analog signal
//                          transfer.
//
//                          When GPIODIE bit 6 is 1:
//                          AUXIO[8i+6] is enabled for digital input.
// OUT                      Output Mode:
//
//                          GPIODOUT bit 6 drives
//                          AUXIO[8i+6].
#define AUX_AIODIO_IOMODE_IO6_W                                              2
#define AUX_AIODIO_IOMODE_IO6_M                                     0x00003000
#define AUX_AIODIO_IOMODE_IO6_S                                             12
#define AUX_AIODIO_IOMODE_IO6_OPEN_SOURCE                           0x00003000
#define AUX_AIODIO_IOMODE_IO6_OPEN_DRAIN                            0x00002000
#define AUX_AIODIO_IOMODE_IO6_IN                                    0x00001000
#define AUX_AIODIO_IOMODE_IO6_OUT                                   0x00000000

// Field: [11:10] IO5
//
// Select mode for AUXIO[8i+5].
// ENUMs:
// OPEN_SOURCE              Open-Source Mode:
//
//                          When GPIODOUT bit 5 is 0:
//                          AUXIO[8i+5] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
//
//                          When GPIODOUT bit 5 is 1:
//                          AUXIO[8i+5] is driven high.
// OPEN_DRAIN               Open-Drain Mode:
//
//                          When GPIODOUT bit 5 is 0:
//                          AUXIO[8i+5] is driven low.
//
//                          When GPIODOUT bit 5 is 1:
//                          AUXIO[8i+5] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
// IN                       Input Mode:
//
//                          When GPIODIE bit 5 is 0:
//                          AUXIO[8i+5] is enabled for analog signal
//                          transfer.
//
//                          When GPIODIE bit 5 is 1:
//                          AUXIO[8i+5] is enabled for digital input.
// OUT                      Output Mode:
//
//                          GPIODOUT bit 5 drives
//                          AUXIO[8i+5].
#define AUX_AIODIO_IOMODE_IO5_W                                              2
#define AUX_AIODIO_IOMODE_IO5_M                                     0x00000C00
#define AUX_AIODIO_IOMODE_IO5_S                                             10
#define AUX_AIODIO_IOMODE_IO5_OPEN_SOURCE                           0x00000C00
#define AUX_AIODIO_IOMODE_IO5_OPEN_DRAIN                            0x00000800
#define AUX_AIODIO_IOMODE_IO5_IN                                    0x00000400
#define AUX_AIODIO_IOMODE_IO5_OUT                                   0x00000000

// Field:   [9:8] IO4
//
// Select mode for AUXIO[8i+4].
// ENUMs:
// OPEN_SOURCE              Open-Source Mode:
//
//                          When GPIODOUT bit 4 is 0:
//                          AUXIO[8i+4] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
//
//                          When GPIODOUT bit 4 is 1:
//                          AUXIO[8i+4] is driven high.
// OPEN_DRAIN               Open-Drain Mode:
//
//                          When GPIODOUT bit 4 is 0:
//                          AUXIO[8i+4] is driven low.
//
//                          When GPIODOUT bit 4 is 1:
//                          AUXIO[8i+4] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
// IN                       Input Mode:
//
//                          When GPIODIE bit 4 is 0:
//                          AUXIO[8i+4] is enabled for analog signal
//                          transfer.
//
//                          When GPIODIE bit 4 is 1:
//                          AUXIO[8i+4] is enabled for digital input.
// OUT                      Output Mode:
//
//                          GPIODOUT bit 4 drives
//                          AUXIO[8i+4].
#define AUX_AIODIO_IOMODE_IO4_W                                              2
#define AUX_AIODIO_IOMODE_IO4_M                                     0x00000300
#define AUX_AIODIO_IOMODE_IO4_S                                              8
#define AUX_AIODIO_IOMODE_IO4_OPEN_SOURCE                           0x00000300
#define AUX_AIODIO_IOMODE_IO4_OPEN_DRAIN                            0x00000200
#define AUX_AIODIO_IOMODE_IO4_IN                                    0x00000100
#define AUX_AIODIO_IOMODE_IO4_OUT                                   0x00000000

// Field:   [7:6] IO3
//
// Select mode for AUXIO[8i+3].
// ENUMs:
// OPEN_SOURCE              Open-Source Mode:
//
//                          When GPIODOUT bit 3 is 0:
//                          AUXIO[8i+3] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
//
//                          When GPIODOUT bit 3 is 1:
//                          AUXIO[8i+3] is driven high.
// OPEN_DRAIN               Open-Drain Mode:
//
//                          When GPIODOUT bit 3 is 0:
//                          AUXIO[8i+3] is driven low.
//
//                          When GPIODOUT bit 3 is 1:
//                          AUXIO[8i+3] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
// IN                       Input Mode:
//
//                          When GPIODIE bit 3 is 0:
//                          AUXIO[8i+3] is enabled for analog signal
//                          transfer.
//
//                          When GPIODIE bit 3 is 1:
//                          AUXIO[8i+3] is enabled for digital input.
// OUT                      Output Mode:
//
//                          GPIODOUT bit 3 drives
//                          AUXIO[8i+3].
#define AUX_AIODIO_IOMODE_IO3_W                                              2
#define AUX_AIODIO_IOMODE_IO3_M                                     0x000000C0
#define AUX_AIODIO_IOMODE_IO3_S                                              6
#define AUX_AIODIO_IOMODE_IO3_OPEN_SOURCE                           0x000000C0
#define AUX_AIODIO_IOMODE_IO3_OPEN_DRAIN                            0x00000080
#define AUX_AIODIO_IOMODE_IO3_IN                                    0x00000040
#define AUX_AIODIO_IOMODE_IO3_OUT                                   0x00000000

// Field:   [5:4] IO2
//
// Select mode for AUXIO[8i+2].
// ENUMs:
// OPEN_SOURCE              Open-Source Mode:
//
//                          When GPIODOUT bit 2 is 0:
//                          AUXIO[8i+2] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
//
//                          When GPIODOUT bit 2 is 1:
//                          AUXIO[8i+2] is driven high.
// OPEN_DRAIN               Open-Drain Mode:
//
//                          When GPIODOUT bit 2 is 0:
//                          AUXIO[8i+2] is driven low.
//
//                          When GPIODOUT bit 2 is 1:
//                          AUXIO[8i+2] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
// IN                       Input Mode:
//
//                          When GPIODIE bit 2 is 0:
//                          AUXIO[8i+2] is enabled for analog signal
//                          transfer.
//
//                          When GPIODIE bit 2 is 1:
//                          AUXIO[8i+2] is enabled for digital input.
// OUT                      Output Mode:
//
//                          GPIODOUT bit 2 drives
//                          AUXIO[8i+2].
#define AUX_AIODIO_IOMODE_IO2_W                                              2
#define AUX_AIODIO_IOMODE_IO2_M                                     0x00000030
#define AUX_AIODIO_IOMODE_IO2_S                                              4
#define AUX_AIODIO_IOMODE_IO2_OPEN_SOURCE                           0x00000030
#define AUX_AIODIO_IOMODE_IO2_OPEN_DRAIN                            0x00000020
#define AUX_AIODIO_IOMODE_IO2_IN                                    0x00000010
#define AUX_AIODIO_IOMODE_IO2_OUT                                   0x00000000

// Field:   [3:2] IO1
//
// Select mode for AUXIO[8i+1].
// ENUMs:
// OPEN_SOURCE              Open-Source Mode:
//
//                          When GPIODOUT bit 1 is 0:
//                          AUXIO[8i+1] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
//
//                          When GPIODOUT bit 1 is 1:
//                          AUXIO[8i+1] is driven high.
// OPEN_DRAIN               Open-Drain Mode:
//
//                          When GPIODOUT bit 1 is 0:
//                          AUXIO[8i+1] is driven low.
//
//                          When GPIODOUT bit 1 is 1:
//                          AUXIO[8i+1] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
// IN                       Input Mode:
//
//                          When GPIODIE bit 1 is 0:
//                          AUXIO[8i+1] is enabled for analog signal
//                          transfer.
//
//                          When GPIODIE bit 1 is 1:
//                          AUXIO[8i+1] is enabled for digital input.
// OUT                      Output Mode:
//
//                          GPIODOUT bit 1 drives
//                          AUXIO[8i+1].
#define AUX_AIODIO_IOMODE_IO1_W                                              2
#define AUX_AIODIO_IOMODE_IO1_M                                     0x0000000C
#define AUX_AIODIO_IOMODE_IO1_S                                              2
#define AUX_AIODIO_IOMODE_IO1_OPEN_SOURCE                           0x0000000C
#define AUX_AIODIO_IOMODE_IO1_OPEN_DRAIN                            0x00000008
#define AUX_AIODIO_IOMODE_IO1_IN                                    0x00000004
#define AUX_AIODIO_IOMODE_IO1_OUT                                   0x00000000

// Field:   [1:0] IO0
//
// Select mode for AUXIO[8i+0].
// ENUMs:
// OPEN_SOURCE              Open-Source Mode:
//
//                          When GPIODOUT bit 0 is 0:
//                          AUXIO[8i+0] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
//
//                          When GPIODOUT bit 0 is 1:
//                          AUXIO[8i+0] is driven high.
// OPEN_DRAIN               Open-Drain Mode:
//
//                          When GPIODOUT bit 0 is 0:
//                          AUXIO[8i+0] is driven low.
//
//                          When GPIODOUT bit 0 is 1:
//                          AUXIO[8i+0] is tri-stated or pulled. This
//                          depends on IOC:IOCFGn.PULL_CTL.
// IN                       Input Mode:
//
//                          When GPIODIE bit 0 is 0:
//                          AUXIO[8i+0] is enabled for analog signal
//                          transfer.
//
//                          When GPIODIE bit 0 is 1:
//                          AUXIO[8i+0] is enabled for digital input.
// OUT                      Output Mode:
//
//                          GPIODOUT bit 0 drives
//                          AUXIO[8i+0].
#define AUX_AIODIO_IOMODE_IO0_W                                              2
#define AUX_AIODIO_IOMODE_IO0_M                                     0x00000003
#define AUX_AIODIO_IOMODE_IO0_S                                              0
#define AUX_AIODIO_IOMODE_IO0_OPEN_SOURCE                           0x00000003
#define AUX_AIODIO_IOMODE_IO0_OPEN_DRAIN                            0x00000002
#define AUX_AIODIO_IOMODE_IO0_IN                                    0x00000001
#define AUX_AIODIO_IOMODE_IO0_OUT                                   0x00000000

//*****************************************************************************
//
// Register: AUX_AIODIO_O_GPIODIN
//
//*****************************************************************************
// Field:   [7:0] IO7_0
//
// Bit n in this bit vector contains the value for AUXIO[8i+n] when GPIODIE bit
// n is set. Otherwise, bit n value is old.
#define AUX_AIODIO_GPIODIN_IO7_0_W                                           8
#define AUX_AIODIO_GPIODIN_IO7_0_M                                  0x000000FF
#define AUX_AIODIO_GPIODIN_IO7_0_S                                           0

//*****************************************************************************
//
// Register: AUX_AIODIO_O_GPIODOUTSET
//
//*****************************************************************************
// Field:   [7:0] IO7_0
//
// Write 1 to bit index n in this bit vector to set GPIODOUT bit n.
//
// Read value is 0.
#define AUX_AIODIO_GPIODOUTSET_IO7_0_W                                       8
#define AUX_AIODIO_GPIODOUTSET_IO7_0_M                              0x000000FF
#define AUX_AIODIO_GPIODOUTSET_IO7_0_S                                       0

//*****************************************************************************
//
// Register: AUX_AIODIO_O_GPIODOUTCLR
//
//*****************************************************************************
// Field:   [7:0] IO7_0
//
// Write 1 to bit index n in this bit vector to clear GPIODOUT bit n.
//
// Read value is 0.
#define AUX_AIODIO_GPIODOUTCLR_IO7_0_W                                       8
#define AUX_AIODIO_GPIODOUTCLR_IO7_0_M                              0x000000FF
#define AUX_AIODIO_GPIODOUTCLR_IO7_0_S                                       0

//*****************************************************************************
//
// Register: AUX_AIODIO_O_GPIODOUTTGL
//
//*****************************************************************************
// Field:   [7:0] IO7_0
//
// Write 1 to bit index n in this bit vector to toggle GPIODOUT bit n.
//
// Read value is 0.
#define AUX_AIODIO_GPIODOUTTGL_IO7_0_W                                       8
#define AUX_AIODIO_GPIODOUTTGL_IO7_0_M                              0x000000FF
#define AUX_AIODIO_GPIODOUTTGL_IO7_0_S                                       0

//*****************************************************************************
//
// Register: AUX_AIODIO_O_GPIODIE
//
//*****************************************************************************
// Field:   [7:0] IO7_0
//
// Write 1 to bit index n in this bit vector to enable digital input buffer for
// AUXIO[8i+n].
// Write 0 to bit index n in this bit vector to disable digital input buffer
// for AUXIO[8i+n].
//
// You must enable the digital input buffer for AUXIO[8i+n] to read the pin
// value in GPIODIN.
// You must disable the digital input buffer for analog input or pins that
// float to avoid current leakage.
#define AUX_AIODIO_GPIODIE_IO7_0_W                                           8
#define AUX_AIODIO_GPIODIE_IO7_0_M                                  0x000000FF
#define AUX_AIODIO_GPIODIE_IO7_0_S                                           0


#endif // __AUX_AIODIO__
