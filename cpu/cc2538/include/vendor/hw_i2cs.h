/******************************************************************************
*  Filename:       hw_i2cs.h
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

#ifndef __HW_I2CS_H__
#define __HW_I2CS_H__

//*****************************************************************************
//
// The following are defines for the I2CS register offsets.
//
//*****************************************************************************
#define I2CS_OAR                0x40020800  // I2C slave own address This 
                                            // register consists of seven 
                                            // address bits that identify the 
                                            // CC2538 I2C device on the I2C 
                                            // bus. 
#define I2CS_STAT               0x40020804  // I2C slave control and status 
                                            // This register functions as a 
                                            // control register when written, 
                                            // and a status register when read. 
#define I2CS_CTRL               0x40020804  // I2C slave control and status 
                                            // This register functions as a 
                                            // control register when written, 
                                            // and a status register when read. 
#define I2CS_DR                 0x40020808  // I2C slave data This register 
                                            // contains the data to be 
                                            // transmitted when in the slave 
                                            // transmit state, and the data 
                                            // received when in the slave 
                                            // receive state. 
#define I2CS_IMR                0x4002080C  // I2C slave interrupt mask This 
                                            // register controls whether a raw 
                                            // interrupt is promoted to a 
                                            // controller interrupt. 
#define I2CS_RIS                0x40020810  // I2C slave raw interrupt status 
                                            // This register specifies whether 
                                            // an interrupt is pending. 
#define I2CS_MIS                0x40020814  // I2C slave masked interrupt 
                                            // status This register specifies 
                                            // whether an interrupt was 
                                            // signaled. 
#define I2CS_ICR                0x40020818  // I2C slave interrupt clear This 
                                            // register clears the raw 
                                            // interrupt. A read of this 
                                            // register returns no meaningful 
                                            // data. 


//*****************************************************************************
//
// The following are defines for the bit fields in the I2CS_OAR register.
//
//*****************************************************************************
#define I2CS_OAR_OAR_M          0x0000007F  // I2C slave own address This 
                                            // field specifies bits A6 through 
                                            // A0 of the slave address. 
#define I2CS_OAR_OAR_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CS_STAT register.
//
//*****************************************************************************
#define I2CS_STAT_FBR           0x00000004  // First byte received 1: The 
                                            // first byte following the slave's 
                                            // own address has been received. 
                                            // 0: The first byte has not been 
                                            // received. This bit is only valid 
                                            // when the RREQ bit is set and is 
                                            // automatically cleared when data 
                                            // has been read from the I2CSDR 
                                            // register. Note: This bit is not 
                                            // used for slave transmit 
                                            // operations. 
#define I2CS_STAT_FBR_M         0x00000004
#define I2CS_STAT_FBR_S         2
#define I2CS_STAT_TREQ          0x00000002  // Transmit request 1: The I2C 
                                            // controller has been addressed as 
                                            // a slave transmitter and is using 
                                            // clock stretching to delay the 
                                            // master until data has been 
                                            // written to the I2CSDR register. 
                                            // 0: No outstanding transmit 
                                            // request. 
#define I2CS_STAT_TREQ_M        0x00000002
#define I2CS_STAT_TREQ_S        1
#define I2CS_STAT_RREQ          0x00000001  // Receive request 1: The I2C 
                                            // controller has outstanding 
                                            // receive data from the I2C master 
                                            // and is using clock stretching to 
                                            // delay the master until data has 
                                            // been read from the I2CSDR 
                                            // register. 0: No outstanding 
                                            // receive data 
#define I2CS_STAT_RREQ_M        0x00000001
#define I2CS_STAT_RREQ_S        0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CS_CTRL register.
//
//*****************************************************************************
#define I2CS_CTRL_DA            0x00000001  // Device active 0: Disables the 
                                            // I2C slave operation 1: Enables 
                                            // the I2C slave operation 
#define I2CS_CTRL_DA_M          0x00000001
#define I2CS_CTRL_DA_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CS_DR register.
//
//*****************************************************************************
#define I2CS_DR_DATA_M          0x000000FF  // Data for transfer This field 
                                            // contains the data for transfer 
                                            // during a slave receive or 
                                            // transmit operation. 
#define I2CS_DR_DATA_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CS_IMR register.
//
//*****************************************************************************
#define I2CS_IMR_STOPIM         0x00000004  // Stop condition interrupt mask 
                                            // 1: The STOP condition interrupt 
                                            // is sent to the interrupt 
                                            // controller when the STOPRIS bit 
                                            // in the I2CSRIS register is set. 
                                            // 0: The STOPRIS interrupt is 
                                            // supressed and not sent to the 
                                            // interrupt controller. 
#define I2CS_IMR_STOPIM_M       0x00000004
#define I2CS_IMR_STOPIM_S       2
#define I2CS_IMR_STARTIM        0x00000002  // Start condition interrupt mask 
                                            // 1: The START condition interrupt 
                                            // is sent to the interrupt 
                                            // controller when the STARTRIS bit 
                                            // in the I2CSRIS register is set. 
                                            // 0: The STARTRIS interrupt is 
                                            // supressed and not sent to the 
                                            // interrupt controller. 
#define I2CS_IMR_STARTIM_M      0x00000002
#define I2CS_IMR_STARTIM_S      1
#define I2CS_IMR_DATAIM         0x00000001  // Data interrupt mask 1: The data 
                                            // received or data requested 
                                            // interrupt is sent to the 
                                            // interrupt controller when the 
                                            // DATARIS bit in the I2CSRIS 
                                            // register is set. 0: The DATARIS 
                                            // interrupt is surpressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define I2CS_IMR_DATAIM_M       0x00000001
#define I2CS_IMR_DATAIM_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CS_RIS register.
//
//*****************************************************************************
#define I2CS_RIS_STOPRIS        0x00000004  // Stop condition raw interrupt 
                                            // status 1: A STOP condition 
                                            // interrupt is pending. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the STOPIC bit in 
                                            // the I2CSICR register. 
#define I2CS_RIS_STOPRIS_M      0x00000004
#define I2CS_RIS_STOPRIS_S      2
#define I2CS_RIS_STARTRIS       0x00000002  // Start condition raw interrupt 
                                            // status 1: A START condition 
                                            // interrupt is pending. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the STARTIC bit in 
                                            // the I2CSICR register. 
#define I2CS_RIS_STARTRIS_M     0x00000002
#define I2CS_RIS_STARTRIS_S     1
#define I2CS_RIS_DATARIS        0x00000001  // Data raw interrupt status 1: A 
                                            // data received or data requested 
                                            // interrupt is pending. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the DATAIC bit in 
                                            // the I2CSICR register. 
#define I2CS_RIS_DATARIS_M      0x00000001
#define I2CS_RIS_DATARIS_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CS_MIS register.
//
//*****************************************************************************
#define I2CS_MIS_STOPMIS        0x00000004  // Stop condition masked interrupt 
                                            // status 1: An unmasked STOP 
                                            // condition interrupt is pending. 
                                            // 0: An interrupt has not occurred 
                                            // or is masked. This bit is 
                                            // cleared by writing 1 to the 
                                            // STOPIC bit in the I2CSICR 
                                            // register. 
#define I2CS_MIS_STOPMIS_M      0x00000004
#define I2CS_MIS_STOPMIS_S      2
#define I2CS_MIS_STARTMIS       0x00000002  // Start condition masked 
                                            // interrupt status 1: An unmasked 
                                            // START condition interrupt is 
                                            // pending. 0: An interrupt has not 
                                            // occurred or is masked. This bit 
                                            // is cleared by writing 1 to the 
                                            // STARTIC bit in the I2CSICR 
                                            // register. 
#define I2CS_MIS_STARTMIS_M     0x00000002
#define I2CS_MIS_STARTMIS_S     1
#define I2CS_MIS_DATAMIS        0x00000001  // Data masked interrupt status 1: 
                                            // An unmasked data received or 
                                            // data requested interrupt is 
                                            // pending. 0: An interrupt has not 
                                            // occurred or is masked. This bit 
                                            // is cleared by writing 1 to the 
                                            // DATAIC bit in the I2CSICR 
                                            // register. 
#define I2CS_MIS_DATAMIS_M      0x00000001
#define I2CS_MIS_DATAMIS_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CS_ICR register.
//
//*****************************************************************************
#define I2CS_ICR_STOPIC         0x00000004  // Stop condition interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // STOPRIS bit in the I2CSRIS 
                                            // register and the STOPMIS bit in 
                                            // the I2CSMIS register. A read of 
                                            // this register returns no 
                                            // meaningful data. 
#define I2CS_ICR_STOPIC_M       0x00000004
#define I2CS_ICR_STOPIC_S       2
#define I2CS_ICR_STARTIC        0x00000002  // Start condition interrupt vlear 
                                            // Writing 1 to this bit clears the 
                                            // STARTRIS bit in the I2CSRIS 
                                            // register and the STARTMIS bit in 
                                            // the I2CSMIS register. A read of 
                                            // this register returns no 
                                            // meaningful data. 
#define I2CS_ICR_STARTIC_M      0x00000002
#define I2CS_ICR_STARTIC_S      1
#define I2CS_ICR_DATAIC         0x00000001  // Data interrupt clear Writing 1 
                                            // to this bit clears the DATARIS 
                                            // bit in the I2CSRIS register and 
                                            // the DATAMIS bit in the I2CSMIS 
                                            // register. A read of this 
                                            // register returns no meaningful 
                                            // data. 
#define I2CS_ICR_DATAIC_M       0x00000001
#define I2CS_ICR_DATAIC_S       0


#endif // __HW_I2CS_H__

