/******************************************************************************
*  Filename:       hw_udmachctl.h
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

#ifndef __HW_UDMACHCTL_H__
#define __HW_UDMACHCTL_H__

//*****************************************************************************
//
// The following are defines for the UDMACHCTL register offsets.
//
//*****************************************************************************
#define UDMACHCTL_O_SRCENDP     0x00000000  
#define UDMACHCTL_O_DSTENDP     0x00000004  
#define UDMACHCTL_O_CHCTL       0x00000008  


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UDMACHCTL_O_SRCENDP register.
//
//*****************************************************************************
#define UDMACHCTL_SRCENDP_ADDR_M \
                                0xFFFFFFFF  // Source address end pointer This 
                                            // field points to the last address 
                                            // of the uDMA transfer source 
                                            // (inclusive). If the source 
                                            // address is not incrementing (the 
                                            // SRCINC field in the DMACHCTL 
                                            // register is 0x3), then this 
                                            // field points at the source 
                                            // location itself (such as a 
                                            // peripheral control register). 

#define UDMACHCTL_SRCENDP_ADDR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UDMACHCTL_O_DSTENDP register.
//
//*****************************************************************************
#define UDMACHCTL_DSTENDP_ADDR_M \
                                0xFFFFFFFF  // Destination address end pointer 
                                            // This field points to the last 
                                            // address of the uDMA transfer 
                                            // destination (inclusive). If the 
                                            // destination address is not 
                                            // incrementing (the DSTINC field 
                                            // in the DMACHCTL register is 
                                            // 0x3), then this field points at 
                                            // the destination location itself 
                                            // (such as a peripheral control 
                                            // register). 

#define UDMACHCTL_DSTENDP_ADDR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UDMACHCTL_O_CHCTL register.
//
//*****************************************************************************
#define UDMACHCTL_CHCTL_DSTINC_M \
                                0xC0000000  // Destination address increment 
                                            // This field configures the 
                                            // destination address increment. 
                                            // The address increment value must 
                                            // be equal or greater than the 
                                            // value of the destination size 
                                            // (DSTSIZE). 0x0: Byte - Increment 
                                            // by 8-bit locations 0x1: 
                                            // Half-word - Increment by 16-bit 
                                            // locations 0x2: Word - Increment 
                                            // by 32-bit locations 0x3: No 
                                            // increment - Address remains set 
                                            // to the value of the Destination 
                                            // address end pointer (DMADSTENDP) 
                                            // for the channel. 

#define UDMACHCTL_CHCTL_DSTINC_S 30
#define UDMACHCTL_CHCTL_DSTSIZE_M \
                                0x30000000  // Destination data size This 
                                            // field configures the destination 
                                            // item data size. Note: DSTSIZE 
                                            // must be the same as SRCSIZE. 
                                            // 0x0: Byte - 8-bit data size 0x1: 
                                            // Half-word - 16-bit data size 
                                            // 0x2: Word - 32-bit data size 
                                            // 0x3: Reserved 

#define UDMACHCTL_CHCTL_DSTSIZE_S 28
#define UDMACHCTL_CHCTL_SRCINC_M \
                                0x0C000000  // Source address increment This 
                                            // field configures the source 
                                            // address increment. The address 
                                            // increment value must be equal or 
                                            // greater than the value of the 
                                            // source size (SRCSIZE). 0x0: Byte 
                                            // - Increment by 8-bit locations 
                                            // 0x1: Half-word - Increment by 
                                            // 16-bit locations 0x2: Word - 
                                            // Increment by 32-bit locations 
                                            // 0x3: No increment - Address 
                                            // remains set to the value of the 
                                            // Source address end pointer 
                                            // (DMASRCENDP) for the channel. 

#define UDMACHCTL_CHCTL_SRCINC_S 26
#define UDMACHCTL_CHCTL_SRCSIZE_M \
                                0x03000000  // Source data size This field 
                                            // configures the source item data 
                                            // size. Note: SRCSIZE must be the 
                                            // same as DSTSIZE. 0x0: Byte - 
                                            // 8-bit data size 0x1: Half-word - 
                                            // 16-bit data size 0x2: Word - 
                                            // 32-bit data size 0x3: Reserved 

#define UDMACHCTL_CHCTL_SRCSIZE_S 24
#define UDMACHCTL_CHCTL_ARBSIZE_M \
                                0x0003C000  // Arbitration size This field 
                                            // configures the number of 
                                            // transfers that can occur before 
                                            // the uDMA controller 
                                            // re-arbitrates. The possible 
                                            // arbitration rate configurations 
                                            // represent powers of 2 and are 
                                            // shown below. 0x0: 1 Transfer - 
                                            // Arbitrates after each uDMA 
                                            // transfer 0x1: 2 Transfers 0x2: 4 
                                            // Transfers 0x3: 8 Transfers 0x4: 
                                            // 16 Transfers 0x5: 32 Transfers 
                                            // 0x6: 64 Transfers 0x7: 128 
                                            // Transfers 0x8: 256 Transfers 
                                            // 0x9: 512 Transfers 0xA-0xF: 1024 
                                            // Transfers - In this 
                                            // configuration, no arbitration 
                                            // occurs during the uDMA transfer 
                                            // because the maximum transfer 
                                            // size is 1024. 

#define UDMACHCTL_CHCTL_ARBSIZE_S 14
#define UDMACHCTL_CHCTL_XFERSIZE_M \
                                0x00003FF0  // Transfer size (minus 1) This 
                                            // field configures the total 
                                            // number of items to transfer. The 
                                            // value of this field is 1 less 
                                            // than the number to transfer 
                                            // (value 0 means transfer 1 item). 
                                            // The maximum value for this 
                                            // 10-bit field is 1023which 
                                            // represents a transfer size of 
                                            // 1024 items. The transfer size is 
                                            // the number of items, not the 
                                            // number of bytes, If the data 
                                            // size is 32 bits, then this value 
                                            // is the number of 32-bit words to 
                                            // transfer. The uDMA controller 
                                            // updates this field immediately 
                                            // before entering the arbitration 
                                            // process, so it contrains the 
                                            // number of outstanding items that 
                                            // is necessary to complete the 
                                            // uDMA cycle. 

#define UDMACHCTL_CHCTL_XFERSIZE_S 4
#define UDMACHCTL_CHCTL_NXTUSEBURST \
                                0x00000008  // Next useburst This field 
                                            // controls whether the Useburst 
                                            // SET[n] bit is automatically set 
                                            // for the last transfer of a 
                                            // peripheral scatter-gather 
                                            // operation. Normally, for the 
                                            // last transfer, if the number of 
                                            // remaining items to transfer is 
                                            // less than the arbitration size, 
                                            // the uDMA controller uses single 
                                            // transfers to complete the 
                                            // transaction. If this bit is set, 
                                            // then the controller uses a burst 
                                            // transfer to complete the last 
                                            // transfer. 

#define UDMACHCTL_CHCTL_NXTUSEBURST_M \
                                0x00000008
#define UDMACHCTL_CHCTL_NXTUSEBURST_S 3
#define UDMACHCTL_CHCTL_XFERMODE_M \
                                0x00000007  // uDMA transfer mode This field 
                                            // configures the operating mode of 
                                            // the uDMA cycle. Refer to "Micro 
                                            // Direct Memory Access - Transfer 
                                            // Modes" for a detailed 
                                            // explanation of transfer modes. 
                                            // Because this register is in 
                                            // system RAM, it has no reset 
                                            // value. Therefore, this field 
                                            // should be initialized to 0 
                                            // before the channel is enabled. 
                                            // 0x0: Stop 0x1: Basic 0x2: 
                                            // Auto-request 0x3: Ping-pong 0x4: 
                                            // Memory scatter-gather 0x5: 
                                            // Alternate memory scatter-gather 
                                            // 0x6: Peripheral scatter-gather 
                                            // 0x7: Alternate peripheral 
                                            // scatter-gather 

#define UDMACHCTL_CHCTL_XFERMODE_S 0


#endif // __HW_UDMACHCTL_H__

