/******************************************************************************
*  Filename:       hw_smwdthrosc.h
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

#ifndef __HW_SMWDTHROSC_H__
#define __HW_SMWDTHROSC_H__

//*****************************************************************************
//
// The following are defines for the SMWDTHROSC register offsets.
//
//*****************************************************************************
#define SMWDTHROSC_WDCTL        0x400D5000  // Watchdog Timer Control 
#define SMWDTHROSC_ST0          0x400D5040  // Sleep Timer 0 count and compare 
#define SMWDTHROSC_ST1          0x400D5044  // Sleep Timer 1 count and compare 
#define SMWDTHROSC_ST2          0x400D5048  // Sleep Timer 2 count and compare 
#define SMWDTHROSC_ST3          0x400D504C  // Sleep Timer 3 count and compare 
#define SMWDTHROSC_STLOAD       0x400D5050  // Sleep Timer load status 
#define SMWDTHROSC_STCC         0x400D5054  // Sleep Timer Capture control 
#define SMWDTHROSC_STCS         0x400D5058  // Sleep Timer Capture status 
#define SMWDTHROSC_STCV0        0x400D505C  // Sleep Timer Capture value byte 
                                            // 0 
#define SMWDTHROSC_STCV1        0x400D5060  // Sleep Timer Capture value byte 
                                            // 1 
#define SMWDTHROSC_STCV2        0x400D5064  // Sleep Timer Capture value byte 
                                            // 2 
#define SMWDTHROSC_STCV3        0x400D5068  // Sleep Timer Capture value byte 
                                            // 3 


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_WDCTL register.
//
//*****************************************************************************
#define SMWDTHROSC_WDCTL_CLR_M  0x000000F0  // Clear timer When 0xA followed 
                                            // by 0x5 is written to these bits, 
                                            // the timer is loaded with 0x0000. 
                                            // Note that 0x5 must be written 
                                            // within one watchdog clock period 
                                            // Twdt after 0xA was written for 
                                            // the clearing to take effect 
                                            // (ensured). If 0x5 is written 
                                            // between Twdt and 2Twdt after 0xA 
                                            // was written, the clearing may 
                                            // take effect, but there is no 
                                            // guarantee. If 0x5 is written > 
                                            // 2Twdt after 0xA was written, the 
                                            // timer will not be cleared. If a 
                                            // value other than 0x5 is written 
                                            // after 0xA has been written, the 
                                            // clear sequence is aborted. If 
                                            // 0xA is written, this starts a 
                                            // new clear sequence. Writing to 
                                            // these bits when EN = 0 has no 
                                            // effect. 
#define SMWDTHROSC_WDCTL_CLR_S  4
#define SMWDTHROSC_WDCTL_EN     0x00000008  // Enable timer When 1 is written 
                                            // to this bit the timer is enabled 
                                            // and starts incrementing. The 
                                            // interval setting specified by 
                                            // INT[1:0] is used. Writing 0 to 
                                            // this bit have no effect. 
#define SMWDTHROSC_WDCTL_EN_M   0x00000008
#define SMWDTHROSC_WDCTL_EN_S   3
#define SMWDTHROSC_WDCTL_INT_M  0x00000003  // Timer interval select These 
                                            // bits select the timer interval 
                                            // as follows: 00: Twdt x 32768 01: 
                                            // Twdt x 8192 10: Twdt x 512 11: 
                                            // Twdt x 64 Writing these bits 
                                            // when EN = 1 has no effect. 
#define SMWDTHROSC_WDCTL_INT_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_ST0 register.
//
//*****************************************************************************
#define SMWDTHROSC_ST0_ST0_M    0x000000FF  // Sleep Timer count and compare 
                                            // value. When read, this register 
                                            // returns the low bits [7:0] of 
                                            // the Sleep Timer count. When 
                                            // writing this register sets the 
                                            // low bits [7:0] of the compare 
                                            // value. 
#define SMWDTHROSC_ST0_ST0_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_ST1 register.
//
//*****************************************************************************
#define SMWDTHROSC_ST1_ST1_M    0x000000FF  // Sleep Timer count and compare 
                                            // value When read, this register 
                                            // returns the middle bits [15:8] 
                                            // of the Sleep Timer count. When 
                                            // writing this register sets the 
                                            // middle bits [15:8] of the 
                                            // compare value. The value read is 
                                            // latched at the time of reading 
                                            // register ST0. The value written 
                                            // is latched when ST0 is written. 
#define SMWDTHROSC_ST1_ST1_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_ST2 register.
//
//*****************************************************************************
#define SMWDTHROSC_ST2_ST2_M    0x000000FF  // Sleep Timer count and compare 
                                            // value When read, this register 
                                            // returns the high bits [23:16] of 
                                            // the Sleep Timer count. When 
                                            // writing this register sets the 
                                            // high bits [23:16] of the compare 
                                            // value. The value read is latched 
                                            // at the time of reading register 
                                            // ST0. The value written is 
                                            // latched when ST0 is written. 
#define SMWDTHROSC_ST2_ST2_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_ST3 register.
//
//*****************************************************************************
#define SMWDTHROSC_ST3_ST3_M    0x000000FF  // Sleep Timer count and compare 
                                            // value When read, this register 
                                            // returns the high bits [31:24] of 
                                            // the Sleep Timer count. When 
                                            // writing this register sets the 
                                            // high bits [31:24] of the compare 
                                            // value. The value read is latched 
                                            // at the time of reading register 
                                            // ST0. The value written is 
                                            // latched when ST0 is written. 
#define SMWDTHROSC_ST3_ST3_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_STLOAD register.
//
//*****************************************************************************
#define SMWDTHROSC_STLOAD_STLOAD \
                                0x00000001  // Status signal for when STx 
                                            // registers have been uploaded to 
                                            // 32-kHz counter. 1: Load is 
                                            // complete 0: Load is busy and STx 
                                            // regs are blocked for writing 

#define SMWDTHROSC_STLOAD_STLOAD_M \
                                0x00000001
#define SMWDTHROSC_STLOAD_STLOAD_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_STCC register.
//
//*****************************************************************************
#define SMWDTHROSC_STCC_PORT_M  0x00000038  // Port select Valid settings are 
                                            // 0-3, all others inhibit any 
                                            // capture from occurring 000: Port 
                                            // A selected 001: Port B selected 
                                            // 010: Port C selected 011: Port D 
                                            // selected 
#define SMWDTHROSC_STCC_PORT_S  3
#define SMWDTHROSC_STCC_PIN_M   0x00000007  // Pin select Valid settings are 
                                            // 1-7 when either port A, B, C, or 
                                            // D is selected. 
#define SMWDTHROSC_STCC_PIN_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_STCS register.
//
//*****************************************************************************
#define SMWDTHROSC_STCS_VALID   0x00000001  // Capture valid flag Set to 1 
                                            // when capture value in STCV has 
                                            // been updated Clear explicitly to 
                                            // allow new capture 
#define SMWDTHROSC_STCS_VALID_M 0x00000001
#define SMWDTHROSC_STCS_VALID_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_STCV0 register.
//
//*****************************************************************************
#define SMWDTHROSC_STCV0_STCV0_M \
                                0x000000FF  // Bits [7:0] of Sleep Timer 
                                            // capture value 

#define SMWDTHROSC_STCV0_STCV0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_STCV1 register.
//
//*****************************************************************************
#define SMWDTHROSC_STCV1_STCV1_M \
                                0x000000FF  // Bits [15:8] of Sleep Timer 
                                            // capture value 

#define SMWDTHROSC_STCV1_STCV1_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_STCV2 register.
//
//*****************************************************************************
#define SMWDTHROSC_STCV2_STCV2_M \
                                0x000000FF  // Bits [23:16] of Sleep Timer 
                                            // capture value 

#define SMWDTHROSC_STCV2_STCV2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// SMWDTHROSC_STCV3 register.
//
//*****************************************************************************
#define SMWDTHROSC_STCV3_STCV3_M \
                                0x000000FF  // Bits [32:24] of Sleep Timer 
                                            // capture value 

#define SMWDTHROSC_STCV3_STCV3_S 0


#endif // __HW_SMWDTHROSC_H__

