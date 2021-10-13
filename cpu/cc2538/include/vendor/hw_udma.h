/******************************************************************************
*  Filename:       hw_udma.h
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

#ifndef __HW_UDMA_H__
#define __HW_UDMA_H__

//*****************************************************************************
//
// The following are defines for the UDMA register offsets.
//
//*****************************************************************************
#define UDMA_STAT               0x400FF000  // DMA status The STAT register 
                                            // returns the status of the uDMA 
                                            // controller. This register cannot 
                                            // be read when the uDMA controller 
                                            // is in the reset state. 
#define UDMA_CFG                0x400FF004  // DMA configuration The CFG 
                                            // register controls the 
                                            // configuration of the uDMA 
                                            // controller. 
#define UDMA_CTLBASE            0x400FF008  // DMA channel control base 
                                            // pointer The CTLBASE register 
                                            // must be configured so that the 
                                            // base pointer points to a 
                                            // location in system memory. The 
                                            // amount of system memory that 
                                            // must be assigned to the uDMA 
                                            // controller depends on the number 
                                            // of uDMA channels used and 
                                            // whether the alternate channel 
                                            // control data structure is used. 
                                            // See Section 10.2.5 for details 
                                            // about the Channel Control Table. 
                                            // The base address must be aligned 
                                            // on a 1024-byte boundary. This 
                                            // register cannot be read when the 
                                            // uDMA controller is in the reset 
                                            // state. 
#define UDMA_ALTBASE            0x400FF00C  // DMA alternate channel control 
                                            // base pointer The ALTBASE 
                                            // register returns the base 
                                            // address of the alternate channel 
                                            // control data. This register 
                                            // removes the necessity for 
                                            // application software to 
                                            // calculate the base address of 
                                            // the alternate channel control 
                                            // structures. This register cannot 
                                            // be read when the uDMA controller 
                                            // is in the reset state. 
#define UDMA_WAITSTAT           0x400FF010  // DMA channel wait-on-request 
                                            // status This read-only register 
                                            // indicates that the uDMA channel 
                                            // is waiting on a request. A 
                                            // peripheral can hold off the uDMA 
                                            // from performing a single request 
                                            // until the peripheral is ready 
                                            // for a burst request to enhance 
                                            // the uDMA performance. The use of 
                                            // this feature is dependent on the 
                                            // design of the peripheral and is 
                                            // not controllable by software in 
                                            // any way. This register cannot be 
                                            // read when the uDMA controller is 
                                            // in the reset state. 
#define UDMA_SWREQ              0x400FF014  // DMA channel software request 
                                            // Each bit of the SWREQ register 
                                            // represents the corresponding 
                                            // uDMA channel. Setting a bit 
                                            // generates a request for the 
                                            // specified uDMA channel. 
#define UDMA_USEBURSTSET        0x400FF018  // DMA channel useburst set Each 
                                            // bit of the USEBURSTSET register 
                                            // represents the corresponding 
                                            // uDMA channel. Setting a bit 
                                            // disables the channel single 
                                            // request input from generating 
                                            // requests, configuring the 
                                            // channel to only accept burst 
                                            // requests. Reading the register 
                                            // returns the status of USEBURST. 
                                            // If the amount of data to 
                                            // transfer is a multiple of the 
                                            // arbitration (burst) size, the 
                                            // corresponding SET[n] bit is 
                                            // cleared after completing the 
                                            // final transfer. If there are 
                                            // fewer items remaining to 
                                            // transfer than the arbitration 
                                            // (burst) size, the uDMA 
                                            // controller automatically clears 
                                            // the corresponding SET[n] bit, 
                                            // allowing the remaining items to 
                                            // transfer using single requests. 
                                            // To resume transfers using burst 
                                            // requests, the corresponding bit 
                                            // must be set again. A bit must 
                                            // not be set if the corresponding 
                                            // peripheral does not support the 
                                            // burst request model. 
#define UDMA_USEBURSTCLR        0x400FF01C  // DMA channel useburst clear Each 
                                            // bit of the USEBURSTCLR register 
                                            // represents the corresponding 
                                            // uDMA channel. Setting a bit 
                                            // clears the corresponding SET[n] 
                                            // bit in the USEBURSTSET register. 
#define UDMA_REQMASKSET         0x400FF020  // DMA channel request mask set 
                                            // Each bit of the REQMASKSET 
                                            // register represents the 
                                            // corresponding uDMA channel. 
                                            // Setting a bit disables uDMA 
                                            // requests for the channel. 
                                            // Reading the register returns the 
                                            // request mask status. When a uDMA 
                                            // channel request is masked, that 
                                            // means the peripheral can no 
                                            // longer request uDMA transfers. 
                                            // The channel can then be used for 
                                            // software-initiated transfers. 
#define UDMA_REQMASKCLR         0x400FF024  // DMA channel request mask clear 
                                            // Each bit of the REQMASKCLR 
                                            // register represents the 
                                            // corresponding uDMA channel. 
                                            // Setting a bit clears the 
                                            // corresponding SET[n] bit in the 
                                            // REQMASKSET register. 
#define UDMA_ENASET             0x400FF028  // DMA channel enable set Each bit 
                                            // of the ENASET register 
                                            // represents the corresponding 
                                            // uDMA channel. Setting a bit 
                                            // enables the corresponding uDMA 
                                            // channel. Reading the register 
                                            // returns the enable status of the 
                                            // channels. If a channel is 
                                            // enabled but the request mask is 
                                            // set (REQMASKSET), then the 
                                            // channel can be used for 
                                            // software-initiated transfers. 
#define UDMA_ENACLR             0x400FF02C  // DMA channel enable clear Each 
                                            // bit of the ENACLR register 
                                            // represents the corresponding 
                                            // uDMA channel. Setting a bit 
                                            // clears the corresponding SET[n] 
                                            // bit in the ENASET register. 
#define UDMA_ALTSET             0x400FF030  // DMA channel primary alternate 
                                            // set Each bit of the ALTSET 
                                            // register represents the 
                                            // corresponding uDMA channel. 
                                            // Setting a bit configures the 
                                            // uDMA channel to use the 
                                            // alternate control data 
                                            // structure. Reading the register 
                                            // returns the status of which 
                                            // control data structure is in use 
                                            // for the corresponding uDMA 
                                            // channel. 
#define UDMA_ALTCLR             0x400FF034  // DMA channel primary alternate 
                                            // clear Each bit of the ALTCLR 
                                            // register represents the 
                                            // corresponding uDMA channel. 
                                            // Setting a bit clears the 
                                            // corresponding SET[n] bit in the 
                                            // ALTSET register. 
#define UDMA_PRIOSET            0x400FF038  // DMA channel priority set Each 
                                            // bit of the PRIOSET register 
                                            // represents the corresponding 
                                            // uDMA channel. Setting a bit 
                                            // configures the uDMA channel to 
                                            // have a high priority level. 
                                            // Reading the register returns the 
                                            // status of the channel priority 
                                            // mask. 
#define UDMA_PRIOCLR            0x400FF03C  // DMA channel priority clear Each 
                                            // bit of the DMAPRIOCLR register 
                                            // represents the corresponding 
                                            // uDMA channel. Setting a bit 
                                            // clears the corresponding SET[n] 
                                            // bit in the PRIOSET register. 
#define UDMA_ERRCLR             0x400FF04C  // DMA bus error clear The ERRCLR 
                                            // register is used to read and 
                                            // clear the uDMA bus error status. 
                                            // The error status is set if the 
                                            // uDMA controller encountered a 
                                            // bus error while performing a 
                                            // transfer. If a bus error occurs 
                                            // on a channel, that channel is 
                                            // automatically disabled by the 
                                            // uDMA controller. The other 
                                            // channels are unaffected. 
#define UDMA_CHASGN             0x400FF500  // DMA channel assignment Each bit 
                                            // of the CHASGN register 
                                            // represents the corresponding 
                                            // uDMA channel. Setting a bit 
                                            // selects the secondary channel 
                                            // assignment as specified in the 
                                            // section "Channel Assignments" 
#define UDMA_CHIS               0x400FF504  // DMA channel interrupt status 
                                            // Each bit of the CHIS register 
                                            // represents the corresponding 
                                            // uDMA channel. A bit is set when 
                                            // that uDMA channel causes a 
                                            // completion interrupt. The bits 
                                            // are cleared by writing 1. 
#define UDMA_CHMAP0             0x400FF510  // DMA channel map select 0 Each 
                                            // 4-bit field of the CHMAP0 
                                            // register configures the uDMA 
                                            // channel assignment as specified 
                                            // in the uDMA channel assignment 
                                            // table in the "Channel 
                                            // Assignments" section. 
#define UDMA_CHMAP1             0x400FF514  // DMA channel map select 1 Each 
                                            // 4-bit field of the CHMAP1 
                                            // register configures the uDMA 
                                            // channel assignment as specified 
                                            // in the uDMA channel assignment 
                                            // table in the "Channel 
                                            // Assignments" section. 
#define UDMA_CHMAP2             0x400FF518  // DMA channel map select 2 Each 
                                            // 4-bit field of the CHMAP2 
                                            // register configures the uDMA 
                                            // channel assignment as specified 
                                            // in the uDMA channel assignment 
                                            // table in the "Channel 
                                            // Assignments" section. 
#define UDMA_CHMAP3             0x400FF51C  // DMA channel map select 3 Each 
                                            // 4-bit field of the CHMAP3 
                                            // register configures the uDMA 
                                            // channel assignment as specified 
                                            // in the uDMA channel assignment 
                                            // table in the "Channel 
                                            // Assignments" section. 


//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_STAT register.
//
//*****************************************************************************
#define UDMA_STAT_DMACHANS_M    0x001F0000  // Available uDMA channels minus 1 
                                            // This field contains a value 
                                            // equal to the number of uDMA 
                                            // channels the uDMA controller is 
                                            // configured to use, minus one. 
                                            // The value of 0x1F corresponds to 
                                            // 32 uDMA channels. 
#define UDMA_STAT_DMACHANS_S    16
#define UDMA_STAT_STATE_M       0x000000F0  // Control state machine status 
                                            // This field shows the current 
                                            // status of the control 
                                            // state-machine. Status can be one 
                                            // of the following: 0x0: Idle 0x1: 
                                            // Reading channel controller data 
                                            // 0x2: Reading source end pointer 
                                            // 0x3: Reading destination end 
                                            // pointer 0x4: Reading source data 
                                            // 0x5: Writing destination data 
                                            // 0x6: Waiting for uDMA request to 
                                            // clear 0x7: Writing channel 
                                            // controller data 0x8: Stalled 
                                            // 0x9: Done 0xA-0xF: Undefined 
#define UDMA_STAT_STATE_S       4
#define UDMA_STAT_MASTEN        0x00000001  // Master enable status 0: The 
                                            // uDMA controller is disabled. 1: 
                                            // The uDMA controller is enabled. 
#define UDMA_STAT_MASTEN_M      0x00000001
#define UDMA_STAT_MASTEN_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CFG register.
//
//*****************************************************************************
#define UDMA_CFG_MASTEN         0x00000001  // Controller master enable 0: 
                                            // Disables the uDMA controller. 1: 
                                            // Enables the uDMA controller. 
#define UDMA_CFG_MASTEN_M       0x00000001
#define UDMA_CFG_MASTEN_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CTLBASE register.
//
//*****************************************************************************
#define UDMA_CTLBASE_ADDR_M     0xFFFFFC00  // Channel control base address 
                                            // This field contains the pointer 
                                            // to the base address of the 
                                            // channel control table. The base 
                                            // address must be 1024-byte 
                                            // alligned. 
#define UDMA_CTLBASE_ADDR_S     10
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ALTBASE register.
//
//*****************************************************************************
#define UDMA_ALTBASE_ADDR_M     0xFFFFFFFF  // Alternate channel address 
                                            // pointer This field provides the 
                                            // base address of the alternate 
                                            // channel control structures. 
#define UDMA_ALTBASE_ADDR_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UDMA_WAITSTAT register.
//
//*****************************************************************************
#define UDMA_WAITSTAT_WAITREQ_M 0xFFFFFFFF  // Channel [n] wait status These 
                                            // bits provide the tchannel 
                                            // wait-on-request status. Bit 0 
                                            // corresponds to channel 0. 1: The 
                                            // corresponding channel is waiting 
                                            // on a request. 0: The 
                                            // corresponding channel is not 
                                            // waiting on a request. 
#define UDMA_WAITSTAT_WAITREQ_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_SWREQ register.
//
//*****************************************************************************
#define UDMA_SWREQ_SWREQ_M      0xFFFFFFFF  // Channel [n] software request 
                                            // These bits generate software 
                                            // requests. Bit 0 corresponds to 
                                            // channel 0. 1: Generate a 
                                            // software request for the 
                                            // corresponding channel 0: No 
                                            // request generated These bits are 
                                            // automatically cleared when the 
                                            // software request has been 
                                            // completed. 
#define UDMA_SWREQ_SWREQ_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UDMA_USEBURSTSET register.
//
//*****************************************************************************
#define UDMA_USEBURSTSET_SET_M  0xFFFFFFFF  // Channel [n] useburst set 0: 
                                            // uDMA channel [n] responds to 
                                            // single or burst requests. 1: 
                                            // uDMA channel [n] responds only 
                                            // to burst requests. Bit 0 
                                            // corresponds to channel 0. This 
                                            // bit is automatically cleared as 
                                            // described above. A bit can also 
                                            // be manually cleared by setting 
                                            // the corresponding CLR[n] bit in 
                                            // the DMAUSEBURSTCLR register. 
#define UDMA_USEBURSTSET_SET_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UDMA_USEBURSTCLR register.
//
//*****************************************************************************
#define UDMA_USEBURSTCLR_CLR_M  0xFFFFFFFF  // Channel [n] useburst clear 0: 
                                            // No effect 1: Setting a bit 
                                            // clears the corresponding SET[n] 
                                            // bit in the DMAUSEBURSTSET 
                                            // register meaning that uDMA 
                                            // channel [n] responds to single 
                                            // and burst requests. 
#define UDMA_USEBURSTCLR_CLR_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UDMA_REQMASKSET register.
//
//*****************************************************************************
#define UDMA_REQMASKSET_SET_M   0xFFFFFFFF  // Channel [n] request mask set 0: 
                                            // The peripheral associated with 
                                            // channel [n] is enabled to 
                                            // request uDMA transfers 1: The 
                                            // peripheral associated with 
                                            // channel [n] is not able to 
                                            // request uDMA transfers. Channel 
                                            // [n] may be used for 
                                            // software-initiated transfers. 
                                            // Bit 0 corresponds to channel 0. 
                                            // A bit can only be cleared by 
                                            // setting the corresponding CLR[n] 
                                            // bit in the DMAREQMASKCLR 
                                            // register. 
#define UDMA_REQMASKSET_SET_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UDMA_REQMASKCLR register.
//
//*****************************************************************************
#define UDMA_REQMASKCLR_CLR_M   0xFFFFFFFF  // Channel [n] request mask clear 
                                            // 0: No effect 1: Setting a bit 
                                            // clears the corresponding SET[n] 
                                            // bit in the DMAREQMASKSET 
                                            // register meaning that the 
                                            // peripheral associated with 
                                            // channel [n] is enabled to 
                                            // request uDMA transfers. 
#define UDMA_REQMASKCLR_CLR_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ENASET register.
//
//*****************************************************************************
#define UDMA_ENASET_SET_M       0xFFFFFFFF  // Channel [n] enable set 0: uDMA 
                                            // channel [n] is disabled 1: uDMA 
                                            // channel [n] is enabled Bit 0 
                                            // corresponds to channel 0. A bit 
                                            // can only be cleared by setting 
                                            // the corresponding CLR[n] bit in 
                                            // the DMAENACLR register. 
#define UDMA_ENASET_SET_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ENACLR register.
//
//*****************************************************************************
#define UDMA_ENACLR_CLR_M       0xFFFFFFFF  // Channel [n] enable clear 0: No 
                                            // effect 1: Setting a bit clears 
                                            // the corresponding SET[n] bit in 
                                            // the DMAENASET register meaning 
                                            // that channel [n] is disabled for 
                                            // uDMA transfers. Note: The 
                                            // controller disables a channel 
                                            // when it completes the uDMA 
                                            // cycle. 
#define UDMA_ENACLR_CLR_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ALTSET register.
//
//*****************************************************************************
#define UDMA_ALTSET_SET_M       0xFFFFFFFF  // Channel [n] alternate set 0: 
                                            // uDMA channel [n] is using the 
                                            // primary control structure 1: 
                                            // uDMA channel [n] is using the 
                                            // alternate control structure Bit 
                                            // 0 corresponds to channel 0. A 
                                            // bit can only be cleared by 
                                            // setting the corresponding CLR[n] 
                                            // bit in the DMAALTCLR register. 
                                            // Note: For Ping-Pong and 
                                            // Scatter-Gather cycle types, the 
                                            // uDMA controller automatically 
                                            // sets these bits to select the 
                                            // alternate channel control data 
                                            // structure. 
#define UDMA_ALTSET_SET_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ALTCLR register.
//
//*****************************************************************************
#define UDMA_ALTCLR_CLR_M       0xFFFFFFFF  // Channel [n] alternate clear 0: 
                                            // No effect 1: Setting a bit 
                                            // clears the corresponding SET[n] 
                                            // bit in the DMAALTSET register 
                                            // meaning that channel [n] is 
                                            // using the primary control 
                                            // structure. Note: For Ping-Pong 
                                            // and Scatter-Gather cycle types, 
                                            // the uDMA controller 
                                            // automatically sets these bits to 
                                            // select the alternate channel 
                                            // control data structure. 
#define UDMA_ALTCLR_CLR_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_PRIOSET register.
//
//*****************************************************************************
#define UDMA_PRIOSET_SET_M      0xFFFFFFFF  // Channel [n] priority set 0: 
                                            // uDMA channel [n] is using the 
                                            // default priority level 1: uDMA 
                                            // channel [n] is using a high 
                                            // priority level Bit 0 corresponds 
                                            // to channel 0. A bit can only be 
                                            // cleared by setting the 
                                            // corresponding CLR[n] bit in the 
                                            // DMAPRIOCLR register. 
#define UDMA_PRIOSET_SET_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_PRIOCLR register.
//
//*****************************************************************************
#define UDMA_PRIOCLR_CLR_M      0xFFFFFFFF  // Channel [n] priority clear 0: 
                                            // No effect 1: Setting a bit 
                                            // clears the corresponding SET[n] 
                                            // bit in the DMAPRIOSET register 
                                            // meaning that channel [n] is 
                                            // using the default priority 
                                            // level. 
#define UDMA_PRIOCLR_CLR_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_ERRCLR register.
//
//*****************************************************************************
#define UDMA_ERRCLR_ERRCLR      0x00000001  // uDMA bus error status 0: No bus 
                                            // error is pending 1: A bus error 
                                            // is pending This bit is cleared 
                                            // by writing 1 to it. 
#define UDMA_ERRCLR_ERRCLR_M    0x00000001
#define UDMA_ERRCLR_ERRCLR_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHASGN register.
//
//*****************************************************************************
#define UDMA_CHASGN_CHASGN_M    0xFFFFFFFF  // Channel [n] assignment select 
                                            // 0: Use the primary channel 
                                            // assignment 1: Use the secondary 
                                            // channel assignment 
#define UDMA_CHASGN_CHASGN_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHIS register.
//
//*****************************************************************************
#define UDMA_CHIS_CHIS_M        0xFFFFFFFF  // Channel [n] interrupt status 0: 
                                            // The corresponding uDMA channel 
                                            // has not caused an interrupt. 1: 
                                            // The corresponding uDMA channel 
                                            // has caused an interrupt. This 
                                            // bit is cleared by writing 1 to 
                                            // it. 
#define UDMA_CHIS_CHIS_S        0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHMAP0 register.
//
//*****************************************************************************
#define UDMA_CHMAP0_CH7SEL_M    0xF0000000  // uDMA channel 7 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP0_CH7SEL_S    28
#define UDMA_CHMAP0_CH6SEL_M    0x0F000000  // uDMA channel 6 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP0_CH6SEL_S    24
#define UDMA_CHMAP0_CH5SEL_M    0x00F00000  // uDMA channel 5 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP0_CH5SEL_S    20
#define UDMA_CHMAP0_CH4SEL_M    0x000F0000  // uDMA channel 4 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP0_CH4SEL_S    16
#define UDMA_CHMAP0_CH3SEL_M    0x0000F000  // uDMA channel 3 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP0_CH3SEL_S    12
#define UDMA_CHMAP0_CH2SEL_M    0x00000F00  // uDMA channel 2 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP0_CH2SEL_S    8
#define UDMA_CHMAP0_CH1SEL_M    0x000000F0  // uDMA channel 1 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP0_CH1SEL_S    4
#define UDMA_CHMAP0_CH0SEL_M    0x0000000F  // uDMA channel 0 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP0_CH0SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHMAP1 register.
//
//*****************************************************************************
#define UDMA_CHMAP1_CH15SEL_M   0xF0000000  // uDMA channel 15 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP1_CH15SEL_S   28
#define UDMA_CHMAP1_CH14SEL_M   0x0F000000  // uDMA channel 14 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP1_CH14SEL_S   24
#define UDMA_CHMAP1_CH13SEL_M   0x00F00000  // uDMA channel 13 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP1_CH13SEL_S   20
#define UDMA_CHMAP1_CH12SEL_M   0x000F0000  // uDMA channel 12 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP1_CH12SEL_S   16
#define UDMA_CHMAP1_CH11SEL_M   0x0000F000  // uDMA channel 11 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP1_CH11SEL_S   12
#define UDMA_CHMAP1_CH10SEL_M   0x00000F00  // uDMA channel 10 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP1_CH10SEL_S   8
#define UDMA_CHMAP1_CH9SEL_M    0x000000F0  // uDMA channel 9 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP1_CH9SEL_S    4
#define UDMA_CHMAP1_CH8SEL_M    0x0000000F  // uDMA channel 8 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP1_CH8SEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHMAP2 register.
//
//*****************************************************************************
#define UDMA_CHMAP2_CH23SEL_M   0xF0000000  // uDMA channel 23 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP2_CH23SEL_S   28
#define UDMA_CHMAP2_CH22SEL_M   0x0F000000  // uDMA channel 22 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP2_CH22SEL_S   24
#define UDMA_CHMAP2_CH21SEL_M   0x00F00000  // uDMA channel 21 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP2_CH21SEL_S   20
#define UDMA_CHMAP2_CH20SEL_M   0x000F0000  // uDMA channel 20 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP2_CH20SEL_S   16
#define UDMA_CHMAP2_CH19SEL_M   0x0000F000  // uDMA channel 19 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP2_CH19SEL_S   12
#define UDMA_CHMAP2_CH18SEL_M   0x00000F00  // uDMA channel 18 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP2_CH18SEL_S   8
#define UDMA_CHMAP2_CH17SEL_M   0x000000F0  // uDMA channel 17 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP2_CH17SEL_S   4
#define UDMA_CHMAP2_CH16SEL_M   0x0000000F  // uDMA channel 16 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP2_CH16SEL_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_CHMAP3 register.
//
//*****************************************************************************
#define UDMA_CHMAP3_CH31SEL_M   0xF0000000  // uDMA channel 31 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP3_CH31SEL_S   28
#define UDMA_CHMAP3_CH30SEL_M   0x0F000000  // uDMA channel 30 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP3_CH30SEL_S   24
#define UDMA_CHMAP3_CH29SEL_M   0x00F00000  // uDMA channel 29 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP3_CH29SEL_S   20
#define UDMA_CHMAP3_CH28SEL_M   0x000F0000  // uDMA channel 28 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP3_CH28SEL_S   16
#define UDMA_CHMAP3_CH27SEL_M   0x0000F000  // uDMA channel 27 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP3_CH27SEL_S   12
#define UDMA_CHMAP3_CH26SEL_M   0x00000F00  // uDMA channel 26 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP3_CH26SEL_S   8
#define UDMA_CHMAP3_CH25SEL_M   0x000000F0  // uDMA channel 25 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP3_CH25SEL_S   4
#define UDMA_CHMAP3_CH24SEL_M   0x0000000F  // uDMA channel 24 source select 
                                            // See section titled "Channel 
                                            // Assignments" in Micro Direct 
                                            // Memory Access chapter. 
#define UDMA_CHMAP3_CH24SEL_S   0


#endif // __HW_UDMA_H__

