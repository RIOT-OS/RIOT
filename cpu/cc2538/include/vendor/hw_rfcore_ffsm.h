/******************************************************************************
*  Filename:       hw_rfcore_ffsm.h
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

#ifndef __HW_RFCORE_FFSM_H__
#define __HW_RFCORE_FFSM_H__

//*****************************************************************************
//
// The following are defines for the RFCORE_FFSM register offsets.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCRESMASK0 \
                                0x40088580  // Source address matching result 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCRESMASK1 \
                                0x40088584  // Source address matching result 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCRESMASK2 \
                                0x40088588  // Source address matching result 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCRESINDEX \
                                0x4008858C  // Source address matching result 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCEXTPENDEN0 \
                                0x40088590  // Source address matching control 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCEXTPENDEN1 \
                                0x40088594  // Source address matching control 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCEXTPENDEN2 \
                                0x40088598  // Source address matching control 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCSHORTPENDEN0 \
                                0x4008859C  // Source address matching control 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCSHORTPENDEN1 \
                                0x400885A0  // Source address matching control 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_SRCSHORTPENDEN2 \
                                0x400885A4  // Source address matching control 
                                            // This register is stored in RAM; 
                                            // the reset value is undefined. 

#define RFCORE_FFSM_EXT_ADDR0   0x400885A8  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_EXT_ADDR1   0x400885AC  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_EXT_ADDR2   0x400885B0  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_EXT_ADDR3   0x400885B4  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_EXT_ADDR4   0x400885B8  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_EXT_ADDR5   0x400885BC  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_EXT_ADDR6   0x400885C0  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_EXT_ADDR7   0x400885C4  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_PAN_ID0     0x400885C8  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_PAN_ID1     0x400885CC  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 
#define RFCORE_FFSM_SHORT_ADDR0 \
                                0x400885D0  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 

#define RFCORE_FFSM_SHORT_ADDR1 \
                                0x400885D4  // Local address information This 
                                            // register is stored in RAM; the 
                                            // reset value is undefined. 



//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCRESMASK0 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCRESMASK0_SRCRESMASK0_M \
                                0x000000FF  // Extended address matching When 
                                            // there is a match on entry ext_n, 
                                            // bits 2n and 2n + 1 are set in 
                                            // SRCRESMASK. 

#define RFCORE_FFSM_SRCRESMASK0_SRCRESMASK0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCRESMASK1 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCRESMASK1_SRCRESMASK1_M \
                                0x000000FF  // Short address matching When 
                                            // there is a match on entry 
                                            // panid_n + short_n, bit n is set 
                                            // in SRCRESMASK. 

#define RFCORE_FFSM_SRCRESMASK1_SRCRESMASK1_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCRESMASK2 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCRESMASK2_SRCRESMASK2_M \
                                0x000000FF  // 24-bit mask that indicates 
                                            // source address match for each 
                                            // individual entry in the source 
                                            // address table 

#define RFCORE_FFSM_SRCRESMASK2_SRCRESMASK2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCRESINDEX register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCRESINDEX_SRCRESINDEX_M \
                                0x000000FF  // The bit index of the 
                                            // least-significant entry (0-23 
                                            // for short addresses or 0-11 for 
                                            // extended addresses) in 
                                            // SRCRESMASK, or 0x3F when there 
                                            // is no source match On a match, 
                                            // bit 5 is 0 when the match is on 
                                            // a short address and 1 when it is 
                                            // on an extended address. On a 
                                            // match, bit 6 is 1 when the 
                                            // conditions for automatic pending 
                                            // bit in acknowledgment have been 
                                            // met (see the description of 
                                            // SRCMATCH.AUTOPEND). The bit does 
                                            // not indicate if the 
                                            // acknowledgment is actually 
                                            // transmitted, and does not 
                                            // consider the PENDING_OR register 
                                            // bit and the SACK/SACKPEND/SNACK 
                                            // strobes. 

#define RFCORE_FFSM_SRCRESINDEX_SRCRESINDEX_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCEXTPENDEN0 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCEXTPENDEN0_SRCEXTPENDEN0_M \
                                0x000000FF  // 8 LSBs of the 24-bit mask that 
                                            // enables or disables automatic 
                                            // pending for each of the 12 
                                            // extended addresses. Entry n is 
                                            // mapped to SRCEXTPENDEN[2n]. All 
                                            // SRCEXTPENDEN[2n + 1] bits are 
                                            // don't care. 

#define RFCORE_FFSM_SRCEXTPENDEN0_SRCEXTPENDEN0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCEXTPENDEN1 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCEXTPENDEN1_SRCEXTPENDEN1_M \
                                0x000000FF  // 8 middle bits of the 24-bit 
                                            // mask that enables or disables 
                                            // automatic pending for each of 
                                            // the 12 extended addresses Entry 
                                            // n is mapped to SRCEXTPENDEN[2n]. 
                                            // All SRCEXTPENDEN[2n + 1] bits 
                                            // are don't care. 

#define RFCORE_FFSM_SRCEXTPENDEN1_SRCEXTPENDEN1_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCEXTPENDEN2 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCEXTPENDEN2_SRCEXTPENDEN2_M \
                                0x000000FF  // 8 MSBs of the 24-bit mask that 
                                            // enables or disables automatic 
                                            // pending for each of the 12 
                                            // extended addresses Entry n is 
                                            // mapped to SRCEXTPENDEN[2n]. All 
                                            // SRCEXTPENDEN[2n + 1] bits are 
                                            // don't care. 

#define RFCORE_FFSM_SRCEXTPENDEN2_SRCEXTPENDEN2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCSHORTPENDEN0 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCSHORTPENDEN0_SRCSHORTPENDEN0_M \
                                0x000000FF  // 8 LSBs of the 24-bit mask that 
                                            // enables or disables automatic 
                                            // pending for each of the 24 short 
                                            // addresses 

#define RFCORE_FFSM_SRCSHORTPENDEN0_SRCSHORTPENDEN0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCSHORTPENDEN1 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCSHORTPENDEN1_SRCSHORTPENDEN1_M \
                                0x000000FF  // 8 middle bits of the 24-bit 
                                            // mask that enables or disables 
                                            // automatic pending for each of 
                                            // the 24 short addresses 

#define RFCORE_FFSM_SRCSHORTPENDEN1_SRCSHORTPENDEN1_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SRCSHORTPENDEN2 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SRCSHORTPENDEN2_SRCSHORTPENDEN2_M \
                                0x000000FF  // 8 MSBs of the 24-bit mask that 
                                            // enables or disables automatic 
                                            // pending for each of the 24 short 
                                            // addresses 

#define RFCORE_FFSM_SRCSHORTPENDEN2_SRCSHORTPENDEN2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_EXT_ADDR0 register.
//
//*****************************************************************************
#define RFCORE_FFSM_EXT_ADDR0_EXT_ADDR0_M \
                                0x000000FF  // EXT_ADDR[7:0] The IEEE extended 
                                            // address used during destination 
                                            // address filtering 

#define RFCORE_FFSM_EXT_ADDR0_EXT_ADDR0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_EXT_ADDR1 register.
//
//*****************************************************************************
#define RFCORE_FFSM_EXT_ADDR1_EXT_ADDR1_M \
                                0x000000FF  // EXT_ADDR[15:8] The IEEE 
                                            // extended address used during 
                                            // destination address filtering 

#define RFCORE_FFSM_EXT_ADDR1_EXT_ADDR1_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_EXT_ADDR2 register.
//
//*****************************************************************************
#define RFCORE_FFSM_EXT_ADDR2_EXT_ADDR2_M \
                                0x000000FF  // EXT_ADDR[23:16] The IEEE 
                                            // extended address used during 
                                            // destination address filtering 

#define RFCORE_FFSM_EXT_ADDR2_EXT_ADDR2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_EXT_ADDR3 register.
//
//*****************************************************************************
#define RFCORE_FFSM_EXT_ADDR3_EXT_ADDR3_M \
                                0x000000FF  // EXT_ADDR[31:24] The IEEE 
                                            // extended address used during 
                                            // destination address filtering 

#define RFCORE_FFSM_EXT_ADDR3_EXT_ADDR3_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_EXT_ADDR4 register.
//
//*****************************************************************************
#define RFCORE_FFSM_EXT_ADDR4_EXT_ADDR4_M \
                                0x000000FF  // EXT_ADDR[39:32] The IEEE 
                                            // extended address used during 
                                            // destination address filtering 

#define RFCORE_FFSM_EXT_ADDR4_EXT_ADDR4_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_EXT_ADDR5 register.
//
//*****************************************************************************
#define RFCORE_FFSM_EXT_ADDR5_EXT_ADDR5_M \
                                0x000000FF  // EXT_ADDR[47:40] The IEEE 
                                            // extended address used during 
                                            // destination address filtering 

#define RFCORE_FFSM_EXT_ADDR5_EXT_ADDR5_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_EXT_ADDR6 register.
//
//*****************************************************************************
#define RFCORE_FFSM_EXT_ADDR6_EXT_ADDR6_M \
                                0x000000FF  // EXT_ADDR[55:48] The IEEE 
                                            // extended address used during 
                                            // destination address filtering 

#define RFCORE_FFSM_EXT_ADDR6_EXT_ADDR6_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_EXT_ADDR7 register.
//
//*****************************************************************************
#define RFCORE_FFSM_EXT_ADDR7_EXT_ADDR7_M \
                                0x000000FF  // EXT_ADDR[63:56] The IEEE 
                                            // extended address used during 
                                            // destination address filtering 

#define RFCORE_FFSM_EXT_ADDR7_EXT_ADDR7_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_PAN_ID0 register.
//
//*****************************************************************************
#define RFCORE_FFSM_PAN_ID0_PAN_ID0_M \
                                0x000000FF  // PAN_ID[7:0] The PAN ID used 
                                            // during destination address 
                                            // filtering 

#define RFCORE_FFSM_PAN_ID0_PAN_ID0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_PAN_ID1 register.
//
//*****************************************************************************
#define RFCORE_FFSM_PAN_ID1_PAN_ID1_M \
                                0x000000FF  // PAN_ID[15:8] The PAN ID used 
                                            // during destination address 
                                            // filtering 

#define RFCORE_FFSM_PAN_ID1_PAN_ID1_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SHORT_ADDR0 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SHORT_ADDR0_SHORT_ADDR0_M \
                                0x000000FF  // SHORT_ADDR[7:0] The short 
                                            // address used during destination 
                                            // address filtering 

#define RFCORE_FFSM_SHORT_ADDR0_SHORT_ADDR0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_FFSM_SHORT_ADDR1 register.
//
//*****************************************************************************
#define RFCORE_FFSM_SHORT_ADDR1_SHORT_ADDR1_M \
                                0x000000FF  // SHORT_ADDR[15:8] The short 
                                            // address used during destination 
                                            // address filtering 

#define RFCORE_FFSM_SHORT_ADDR1_SHORT_ADDR1_S 0


#endif // __HW_RFCORE_FFSM_H__

