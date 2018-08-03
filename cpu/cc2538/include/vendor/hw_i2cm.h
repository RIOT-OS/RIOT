/******************************************************************************
*  Filename:       hw_i2cm.h
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

#ifndef __HW_I2CM_H__
#define __HW_I2CM_H__

//*****************************************************************************
//
// The following are defines for the I2CM register offsets.
//
//*****************************************************************************
#define I2CM_SA                 0x40020000  // I2C master slave address This 
                                            // register consists of eight bits, 
                                            // seven address bits (A6-A0), and 
                                            // a receive and send bit, which 
                                            // determines if the next operation 
                                            // is a receive (high) or transmit 
                                            // (low). 
#define I2CM_CTRL               0x40020004  // I2C master control and status 
                                            // This register accesses status 
                                            // bits when read and control bits 
                                            // when written. When read, the 
                                            // status register indicates the 
                                            // state of the I2C bus controller. 
                                            // When written, the control 
                                            // register configures the I2C 
                                            // controller operation. The START 
                                            // bit generates the START or 
                                            // REPEATED START condition. The 
                                            // STOP bit determines if the cycle 
                                            // stops at the end of the data 
                                            // cycle or continues on to a 
                                            // repeated START condition. To 
                                            // generate a single transmit 
                                            // cycle, the I2C master slave 
                                            // address (I2CMSA) register is 
                                            // written with the desired 
                                            // address, the R/S bit is cleared, 
                                            // and this register is written 
                                            // with ACK = X (0 or 1), STOP = 1, 
                                            // START = 1, and RUN = 1 to 
                                            // perform the operation and stop. 
                                            // When the operation is completed 
                                            // (or aborted due an error), an 
                                            // interrupt becomes active and the 
                                            // data may be read from the I2CMDR 
                                            // register. When the I2C module 
                                            // operates in master receiver 
                                            // mode, the ACK bit is normally 
                                            // set, causing the I2C bus 
                                            // controller to automatically 
                                            // transmit an acknowledge after 
                                            // each byte. This bit must be 
                                            // cleared when the I2C bus 
                                            // controller requires no further 
                                            // data to be transmitted from the 
                                            // slave transmitter. 
#define I2CM_STAT               0x40020004  // I2C master control and status 
                                            // This register accesses status 
                                            // bits when read and control bits 
                                            // when written. When read, the 
                                            // status register indicates the 
                                            // state of the I2C bus controller. 
                                            // When written, the control 
                                            // register configures the I2C 
                                            // controller operation. The START 
                                            // bit generates the START or 
                                            // REPEATED START condition. The 
                                            // STOP bit determines if the cycle 
                                            // stops at the end of the data 
                                            // cycle or continues on to a 
                                            // repeated START condition. To 
                                            // generate a single transmit 
                                            // cycle, the I2C master slave 
                                            // address (I2CMSA) register is 
                                            // written with the desired 
                                            // address, the R/S bit is cleared, 
                                            // and this register is written 
                                            // with ACK = X (0 or 1), STOP = 1, 
                                            // START = 1, and RUN = 1 to 
                                            // perform the operation and stop. 
                                            // When the operation is completed 
                                            // (or aborted due an error), an 
                                            // interrupt becomes active and the 
                                            // data may be read from the I2CMDR 
                                            // register. When the I2C module 
                                            // operates in master receiver 
                                            // mode, the ACK bit is normally 
                                            // set, causing the I2C bus 
                                            // controller to automatically 
                                            // transmit an acknowledge after 
                                            // each byte. This bit must be 
                                            // cleared when the I2C bus 
                                            // controller requires no further 
                                            // data to be transmitted from the 
                                            // slave transmitter. 
#define I2CM_DR                 0x40020008  // I2C master data This register 
                                            // contains the data to be 
                                            // transmitted when in the master 
                                            // transmit state and the data 
                                            // received when in the master 
                                            // receive state. 
#define I2CM_TPR                0x4002000C  // I2C master timer period This 
                                            // register specifies the period of 
                                            // the SCL clock. 
#define I2CM_IMR                0x40020010  // I2C master interrupt mask This 
                                            // register controls whether a raw 
                                            // interrupt is promoted to a 
                                            // controller interrupt. 
#define I2CM_RIS                0x40020014  // I2C master raw interrupt status 
                                            // This register specifies whether 
                                            // an interrupt is pending. 
#define I2CM_MIS                0x40020018  // I2C master masked interrupt 
                                            // status This register specifies 
                                            // whether an interrupt was 
                                            // signaled. 
#define I2CM_ICR                0x4002001C  // I2C master interrupt clear This 
                                            // register clears the raw and 
                                            // masked interrupts. 
#define I2CM_CR                 0x40020020  // I2C master configuration This 
                                            // register configures the mode 
                                            // (master or slave) and sets the 
                                            // interface for test mode 
                                            // loopback. 


//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_SA register.
//
//*****************************************************************************
#define I2CM_SA_SA_M            0x000000FE  // I2C slave address 
#define I2CM_SA_SA_S            1
#define I2CM_SA_RS              0x00000001  // Receive and send The R/S bit 
                                            // specifies if the next operation 
                                            // is a receive (high) or transmit 
                                            // (low). 0: Transmit 1: Receive 
#define I2CM_SA_RS_M            0x00000001
#define I2CM_SA_RS_S            0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_CTRL register.
//
//*****************************************************************************
#define I2CM_CTRL_ACK           0x00000008  // Data acknowledge enable 0: The 
                                            // received data byte is not 
                                            // acknowledged automatically by 
                                            // the master. 1: The received data 
                                            // byte is acknowledged 
                                            // automatically by the master. 
#define I2CM_CTRL_ACK_M         0x00000008
#define I2CM_CTRL_ACK_S         3
#define I2CM_CTRL_STOP          0x00000004  // Generate STOP 0: The controller 
                                            // does not generate the STOP 
                                            // condition. 1: The controller 
                                            // generates the STOP condition. 
#define I2CM_CTRL_STOP_M        0x00000004
#define I2CM_CTRL_STOP_S        2
#define I2CM_CTRL_START         0x00000002  // Generate START 0: The 
                                            // controller does not generate the 
                                            // START condition. 1: The 
                                            // controller generates the START 
                                            // condition. 
#define I2CM_CTRL_START_M       0x00000002
#define I2CM_CTRL_START_S       1
#define I2CM_CTRL_RUN           0x00000001  // I2C master enable 0: The master 
                                            // is disabled. 1: The master is 
                                            // enabled to transmit or receive 
                                            // data. When the BUSY bit is set, 
                                            // the other status bits are not 
                                            // valid. 
#define I2CM_CTRL_RUN_M         0x00000001
#define I2CM_CTRL_RUN_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_STAT register.
//
//*****************************************************************************
#define I2CM_STAT_BUSBSY        0x00000040  // Bus busy 0: The I2C bus is 
                                            // idle. 1: The I2C bus is busy. 
                                            // The bit changes based on the 
                                            // START and STOP conditions. 
#define I2CM_STAT_BUSBSY_M      0x00000040
#define I2CM_STAT_BUSBSY_S      6
#define I2CM_STAT_IDLE          0x00000020  // I2C idle 0: The I2C controller 
                                            // is not idle. 1: The I2C 
                                            // controller is idle. 
#define I2CM_STAT_IDLE_M        0x00000020
#define I2CM_STAT_IDLE_S        5
#define I2CM_STAT_ARBLST        0x00000010  // Arbitration lost 0: The I2C 
                                            // controller won arbitration. 1: 
                                            // The I2C controller lost 
                                            // arbitration. 
#define I2CM_STAT_ARBLST_M      0x00000010
#define I2CM_STAT_ARBLST_S      4
#define I2CM_STAT_DATACK        0x00000008  // Acknowledge data 0: The 
                                            // transmited data was 
                                            // acknowledged. 1: The transmited 
                                            // data was not acknowledged. 
#define I2CM_STAT_DATACK_M      0x00000008
#define I2CM_STAT_DATACK_S      3
#define I2CM_STAT_ADRACK        0x00000004  // Acknowledge address 0: The 
                                            // transmited address was 
                                            // acknowledged. 1: The transmited 
                                            // address was not acknowledged. 
#define I2CM_STAT_ADRACK_M      0x00000004
#define I2CM_STAT_ADRACK_S      2
#define I2CM_STAT_ERROR         0x00000002  // Error 0: No error was detected 
                                            // on the last operation. 1: An 
                                            // error occurred on the last 
                                            // operation. 
#define I2CM_STAT_ERROR_M       0x00000002
#define I2CM_STAT_ERROR_S       1
#define I2CM_STAT_BUSY          0x00000001  // I2C busy 0: The controller is 
                                            // idle. 1: The controller is busy. 
                                            // When the BUSY bit is set, the 
                                            // other status bits are not valid. 
#define I2CM_STAT_BUSY_M        0x00000001
#define I2CM_STAT_BUSY_S        0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_DR register.
//
//*****************************************************************************
#define I2CM_DR_DATA_M          0x000000FF  // Data transferred Data 
                                            // transferred during transaction 
#define I2CM_DR_DATA_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_TPR register.
//
//*****************************************************************************
#define I2CM_TPR_TPR_M          0x0000007F  // SCL clock period This field 
                                            // specifies the period of the SCL 
                                            // clock. SCL_PRD = 2 * 
                                            // (1+TPR)*(SCL_LP + 
                                            // SCL_HP)*CLK_PRD where: SCL_PRD 
                                            // is the SCL line period (I2C 
                                            // clock). TPR is the timer period 
                                            // register value (range of 1 to 
                                            // 127) SCL_LP is the SCL low 
                                            // period (fixed at 6). SCL_HP is 
                                            // the SCL high period (fixed at 
                                            // 4). CLK_PRD is the system clock 
                                            // period in ns. 
#define I2CM_TPR_TPR_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_IMR register.
//
//*****************************************************************************
#define I2CM_IMR_IM             0x00000001  // Interrupt mask 1: The master 
                                            // interrupt is sent to the 
                                            // interrupt controller when the 
                                            // RIS bit in the I2CMRIS register 
                                            // is set. 0: The RIS interrupt is 
                                            // suppressed and not sent to the 
                                            // interrupt controller. 
#define I2CM_IMR_IM_M           0x00000001
#define I2CM_IMR_IM_S           0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_RIS register.
//
//*****************************************************************************
#define I2CM_RIS_RIS            0x00000001  // Raw interrupt status 1: A 
                                            // master interrupt is pending. 0: 
                                            // No interrupt This bit is cleared 
                                            // by writing 1 to the IC bit in 
                                            // the I2CMICR register. 
#define I2CM_RIS_RIS_M          0x00000001
#define I2CM_RIS_RIS_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_MIS register.
//
//*****************************************************************************
#define I2CM_MIS_MIS            0x00000001  // Masked interrupt status 1: An 
                                            // unmasked master interrupt is 
                                            // pending. 0: An interrupt has not 
                                            // occurred or is masked. This bit 
                                            // is cleared by writing 1 to the 
                                            // IC bit in the I2CMICR register. 
#define I2CM_MIS_MIS_M          0x00000001
#define I2CM_MIS_MIS_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_ICR register.
//
//*****************************************************************************
#define I2CM_ICR_IC             0x00000001  // Interrupt clear Writing 1 to 
                                            // this bit clears the RIS bit in 
                                            // the I2CMRIS register and the MIS 
                                            // bit in the I2CMMIS register. 
                                            // Reading this register returns no 
                                            // meaningful data. 
#define I2CM_ICR_IC_M           0x00000001
#define I2CM_ICR_IC_S           0
//*****************************************************************************
//
// The following are defines for the bit fields in the I2CM_CR register.
//
//*****************************************************************************
#define I2CM_CR_SFE             0x00000020  // I2C slave function enable 1: 
                                            // Slave mode is enabled. 0: Slave 
                                            // mode is disabled. 
#define I2CM_CR_SFE_M           0x00000020
#define I2CM_CR_SFE_S           5
#define I2CM_CR_MFE             0x00000010  // I2C master function enable 1: 
                                            // Master mode is enabled. 0: 
                                            // Master mode is disabled. 
#define I2CM_CR_MFE_M           0x00000010
#define I2CM_CR_MFE_S           4
#define I2CM_CR_LPBK            0x00000001  // I2C loopback 1: The controller 
                                            // in a test mode loopback 
                                            // configuration. 0: Normal 
                                            // operation 
#define I2CM_CR_LPBK_M          0x00000001
#define I2CM_CR_LPBK_S          0


#endif // __HW_I2CM_H__

