/******************************************************************************
*  Filename:       hw_ssi.h
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

#ifndef __HW_SSI_H__
#define __HW_SSI_H__

//*****************************************************************************
//
// The following are defines for the SSI register offsets.
//
//*****************************************************************************
#define SSI_O_CR0               0x00000000  // The CR0 register contains bit 
                                            // fields that control various 
                                            // functions within the SSI module. 
                                            // Functionality such as protocol 
                                            // mode, clock rate, and data size 
                                            // are configured in this register. 
#define SSI_O_CR1               0x00000004  // The CR1 register contains bit 
                                            // fields that control various 
                                            // functions within the SSI module. 
                                            // Master and slave mode 
                                            // functionality is controlled by 
                                            // this register. 
#define SSI_O_DR                0x00000008  // The DR register is 16 bits 
                                            // wide. When the SSIDR register is 
                                            // read, the entry in the receive 
                                            // FIFO that is pointed to by the 
                                            // current FIFO read pointer is 
                                            // accessed. When a data value is 
                                            // removed by the SSI receive logic 
                                            // from the incoming data frame, it 
                                            // is placed into the entry in the 
                                            // receive FIFO pointed to by the 
                                            // current FIFO write pointer. When 
                                            // the DR register is written to, 
                                            // the entry in the transmit FIFO 
                                            // that is pointed to by the write 
                                            // pointer is written to. Data 
                                            // values are removed from the 
                                            // transmit FIFO one value at a 
                                            // time by the transmit logic. Each 
                                            // data value is loaded into the 
                                            // transmit serial shifter, then 
                                            // serially shifted out onto the 
                                            // SSITx pin at the programmed bit 
                                            // rate. When a data size of less 
                                            // than 16 bits is selected, the 
                                            // user must right-justify data 
                                            // written to the transmit FIFO. 
                                            // The transmit logic ignores the 
                                            // unused bits. Received data less 
                                            // than 16 bits is automatically 
                                            // right-justified in the receive 
                                            // buffer. When the SSI is 
                                            // programmed for MICROWIRE frame 
                                            // format, the default size for 
                                            // transmit data is eight bits (the 
                                            // most significant byte is 
                                            // ignored). The receive data size 
                                            // is controlled by the programmer. 
                                            // The transmit FIFO and the 
                                            // receive FIFO are not cleared 
                                            // even when the SSE bit in the 
                                            // SSICR1 register is cleared, 
                                            // allowing the software to fill 
                                            // the transmit FIFO before 
                                            // enabling the SSI. 
#define SSI_O_SR                0x0000000C  // The SR register contains bits 
                                            // that indicate the FIFO fill 
                                            // status and the SSI busy status. 
#define SSI_O_CPSR              0x00000010  // The CPSR register specifies the 
                                            // division factor which is used to 
                                            // derive the SSIClk from the 
                                            // system clock. The clock is 
                                            // further divided by a value from 
                                            // 1 to 256, which is 1 + SCR. SCR 
                                            // is programmed in the SSICR0 
                                            // register. The frequency of the 
                                            // SSIClk is defined by: SSIClk = 
                                            // SysClk / (CPSDVSR x (1 + SCR)) 
                                            // The value programmed into this 
                                            // register must be an even number 
                                            // between 2 and 254. The 
                                            // least-significant bit of the 
                                            // programmed number is hard-coded 
                                            // to zero. If an odd number is 
                                            // written to this register, data 
                                            // read back from this register has 
                                            // the least-significant bit as 
                                            // zero. 
#define SSI_O_IM                0x00000014  // The IM register is the 
                                            // interrupt mask set or clear 
                                            // register. It is a read/write 
                                            // register and all bits are 
                                            // cleared on reset. On a read, 
                                            // this register gives the current 
                                            // value of the mask on the 
                                            // corresponding interrupt. Setting 
                                            // a bit sets the mask, preventing 
                                            // the interrupt from being 
                                            // signaled to the interrupt 
                                            // controller. Clearing a bit 
                                            // clears the corresponding mask, 
                                            // enabling the interrupt to be 
                                            // sent to the interrupt 
                                            // controller. 
#define SSI_O_RIS               0x00000018  // The RIS register is the raw 
                                            // interrupt status register. On a 
                                            // read, this register gives the 
                                            // current raw status value of the 
                                            // corresponding interrupt before 
                                            // masking. A write has no effect. 
#define SSI_O_MIS               0x0000001C  // The MIS register is the masked 
                                            // interrupt status register. On a 
                                            // read, this register gives the 
                                            // current masked status value of 
                                            // the corresponding interrupt. A 
                                            // write has no effect. 
#define SSI_O_ICR               0x00000020  // The ICR register is the 
                                            // interrupt clear register. On a 
                                            // write of 1, the corresponding 
                                            // interrupt is cleared. A write of 
                                            // 0 has no effect. 
#define SSI_O_DMACTL            0x00000024  // The DMACTL register is the uDMA 
                                            // control register. 
#define SSI_O_CC                0x00000FC8  // SSI clock configuration The CC 
                                            // register controls the baud clock 
                                            // and system clocks sources for 
                                            // the SSI module. Note: If the 
                                            // PIOSC is used for the SSI baud 
                                            // clock, the system clock 
                                            // frequency must be at least 16 
                                            // MHz in run mode. 


//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CR0 register.
//
//*****************************************************************************
#define SSI_CR0_SCR_M           0x0000FF00  // SSI serial clock rate (R/W) 
                                            // Reset value: 0x0 The value SCR 
                                            // is used to generate the transmit 
                                            // and receive bit rate of the SSI. 
                                            // Where the bit rate is: BR = 
                                            // FSSICLK/(CPSDVR * (1 + SCR)) 
                                            // where CPSDVR is an even value 
                                            // from 2-254, programmed in the 
                                            // SSICPSR register and SCR is a 
                                            // value from 0-255. 
#define SSI_CR0_SCR_S           8
#define SSI_CR0_SPH             0x00000080  // SSI serial clock phase (R/W) 
                                            // Reset value: 0x0 This bit is 
                                            // only applicable to the Motorola 
                                            // SPI Format. 
#define SSI_CR0_SPH_M           0x00000080
#define SSI_CR0_SPH_S           7
#define SSI_CR0_SPO             0x00000040  // SSI serial clock phase (R/W) 
                                            // Reset value: 0x0 This bit is 
                                            // only applicable to the Motorola 
                                            // SPI Format. 
#define SSI_CR0_SPO_M           0x00000040
#define SSI_CR0_SPO_S           6
#define SSI_CR0_FRF_M           0x00000030  // SSI frame format select (R/W) 
                                            // Reset value: 0x0 00: Motorola 
                                            // SPI frame format 01: TI 
                                            // synchronous serial frame format 
                                            // 10: National Microwire frame 
                                            // format 11: Reserved 
#define SSI_CR0_FRF_S           4
#define SSI_CR0_DSS_M           0x0000000F  // SSI data size select (R/W) 
                                            // Reset value: 0x0 0000-0010: 
                                            // Reserved 0011: 4-bit data 0100: 
                                            // 5-bit data 0101: 6-bit data 
                                            // 0110: 7-bit data 0111: 8-bit 
                                            // data 1000: 9-bit data 1001: 
                                            // 10-bit data 1010: 11-bit data 
                                            // 1011: 12-bit data 1100: 13-bit 
                                            // data 1101: 14-bit data 1110: 
                                            // 15-bit data 1111: 16-bit data 
#define SSI_CR0_DSS_S           0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CR1 register.
//
//*****************************************************************************
#define SSI_CR1_SOD             0x00000008  // SSI slave mode output disable 
                                            // (R/W) Reset value: 0x0 This bit 
                                            // is relevant only in the slave 
                                            // mode (MS = 1). In multiple-slave 
                                            // systems, it is possible for the 
                                            // SSI master to broadcast a 
                                            // message to all slaves in the 
                                            // system while ensuring that only 
                                            // one slave drives data onto the 
                                            // serial output line. In such 
                                            // systems, the RXD lines from 
                                            // multiple slaves could be tied 
                                            // together. To operate in such a 
                                            // system, the SOD bit can be set 
                                            // if the SSI slave is not suppose 
                                            // to drive the SSITXD line. 0: SSI 
                                            // can drive SSITXD in slave output 
                                            // mode 1: SSI must not drive the 
                                            // SSITXD output in slave mode 
#define SSI_CR1_SOD_M           0x00000008
#define SSI_CR1_SOD_S           3
#define SSI_CR1_MS              0x00000004  // SSI master and slave select 
                                            // (R/W) Reset value: 0x0 This bit 
                                            // can be modified only when the 
                                            // SSI is disabled (SSE = 0). 0: 
                                            // Device configured as a master 
                                            // (default) 1: Device configured 
                                            // as a slave 
#define SSI_CR1_MS_M            0x00000004
#define SSI_CR1_MS_S            2
#define SSI_CR1_SSE             0x00000002  // SSI synchronous serial port 
                                            // enable (R/W) Reset value: 0x0 0: 
                                            // SSI operation is disabled. 1: 
                                            // SSI operation is enabled. 
#define SSI_CR1_SSE_M           0x00000002
#define SSI_CR1_SSE_S           1
#define SSI_CR1_LBM             0x00000001  // SSI loop-back mode (R/W) Reset 
                                            // value: 0x0 0: Normal serial port 
                                            // operation is enabled. 1: The 
                                            // output of the transmit serial 
                                            // shifter is connected to the 
                                            // input of the receive serial 
                                            // shift register internally. 
#define SSI_CR1_LBM_M           0x00000001
#define SSI_CR1_LBM_S           0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_DR register.
//
//*****************************************************************************
#define SSI_DR_DATA_M           0x0000FFFF  // SSI receive/transmit data 
                                            // register (R/W) Reset value: 
                                            // 0xXXXX A read operation reads 
                                            // the receive FIFO. A write 
                                            // operation writes the transmit 
                                            // FIFO. Software must 
                                            // right-justify data when the SSI 
                                            // is programmed for a data size 
                                            // that is less than 16 bits. 
                                            // Unused bits at the top are 
                                            // ignored by the transmit logic. 
                                            // The receive logic automatically 
                                            // right-justified the data. 
#define SSI_DR_DATA_S           0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_SR register.
//
//*****************************************************************************
#define SSI_SR_BSY              0x00000010  // SSI busy bit (RO) Reset value: 
                                            // 0x0 0: SSI is idle. 1: SSI is 
                                            // currently transmitting and/or 
                                            // receiving a frame or the 
                                            // transmit FIFO is not empty. 
#define SSI_SR_BSY_M            0x00000010
#define SSI_SR_BSY_S            4
#define SSI_SR_RFF              0x00000008  // SSI receive FIFO full (RO) 
                                            // Reset value: 0x0 0: Receive FIFO 
                                            // is not full. 1: Receive FIFO is 
                                            // full. 
#define SSI_SR_RFF_M            0x00000008
#define SSI_SR_RFF_S            3
#define SSI_SR_RNE              0x00000004  // SSI receive FIFO not empty (RO) 
                                            // Reset value: 0x0 0: Receive FIFO 
                                            // is empty. 1: Receive FIFO is not 
                                            // empty. 
#define SSI_SR_RNE_M            0x00000004
#define SSI_SR_RNE_S            2
#define SSI_SR_TNF              0x00000002  // SSI transmit FIFO not full (RO) 
                                            // Reset value: 0x1 0: Transmit 
                                            // FIFO is full. 1: Transmit FIFO 
                                            // is not full. 
#define SSI_SR_TNF_M            0x00000002
#define SSI_SR_TNF_S            1
#define SSI_SR_TFE              0x00000001  // SSI transmit FIFO empty (RO) 
                                            // Reset value: 0x1 0: Transmit 
                                            // FIFO is not empty. 1: Transmit 
                                            // FIFO is empty. 
#define SSI_SR_TFE_M            0x00000001
#define SSI_SR_TFE_S            0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CPSR register.
//
//*****************************************************************************
#define SSI_CPSR_CPSDVSR_M      0x000000FF  // SSI clock prescale divisor 
                                            // (R/W) Reset value: 0x0 This 
                                            // value must be an even number 
                                            // from 2 to 254, depending on the 
                                            // frequency of SSICLK. The LSB 
                                            // always returns zero on reads. 
#define SSI_CPSR_CPSDVSR_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_IM register.
//
//*****************************************************************************
#define SSI_IM_TXIM             0x00000008  // SSI transmit FIFO interrupt 
                                            // mask (R/W) Reset value: 0x0 0: 
                                            // TX FIFO half empty or condition 
                                            // interrupt is masked. 1: TX FIFO 
                                            // half empty or less condition 
                                            // interrupt is not masked. 
#define SSI_IM_TXIM_M           0x00000008
#define SSI_IM_TXIM_S           3
#define SSI_IM_RXIM             0x00000004  // SSI receive FIFO interrupt mask 
                                            // (R/W) Reset value: 0x0 0: RX 
                                            // FIFO half empty or condition 
                                            // interrupt is masked. 1: RX FIFO 
                                            // half empty or less condition 
                                            // interrupt is not masked. 
#define SSI_IM_RXIM_M           0x00000004
#define SSI_IM_RXIM_S           2
#define SSI_IM_RTIM             0x00000002  // SSI receive time-out interrupt 
                                            // mask (R/W) Reset value: 0x0 0: 
                                            // RX FIFO time-out interrupt is 
                                            // masked. 1: RX FIFO time-out 
                                            // interrupt is not masked 
#define SSI_IM_RTIM_M           0x00000002
#define SSI_IM_RTIM_S           1
#define SSI_IM_RORIM            0x00000001  // SSI receive overrun interrupt 
                                            // mask (R/W) Reset value: 0x0 0: 
                                            // RX FIFO Overrun interrupt is 
                                            // masked. 1: RX FIFO Overrun 
                                            // interrupt is not masked 
#define SSI_IM_RORIM_M          0x00000001
#define SSI_IM_RORIM_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_RIS register.
//
//*****************************************************************************
#define SSI_RIS_TXRIS           0x00000008  // SSI SSITXINTR raw state (RO) 
                                            // Reset value: 0x1 Gives the raw 
                                            // interrupt state (before masking) 
                                            // of SSITXINTR 
#define SSI_RIS_TXRIS_M         0x00000008
#define SSI_RIS_TXRIS_S         3
#define SSI_RIS_RXRIS           0x00000004  // SSI SSIRXINTR raw state (RO) 
                                            // Reset value: 0x0 Gives the raw 
                                            // interrupt state (before masking) 
                                            // of SSIRXINTR 
#define SSI_RIS_RXRIS_M         0x00000004
#define SSI_RIS_RXRIS_S         2
#define SSI_RIS_RTRIS           0x00000002  // SSI SSIRTINTR raw state (RO) 
                                            // Reset value: 0x0 Gives the raw 
                                            // interrupt state (before masking) 
                                            // of SSIRTINTR 
#define SSI_RIS_RTRIS_M         0x00000002
#define SSI_RIS_RTRIS_S         1
#define SSI_RIS_RORRIS          0x00000001  // SSI SSIRORINTR raw state (RO) 
                                            // Reset value: 0x0 Gives the raw 
                                            // interrupt state (before masking) 
                                            // of SSIRORINTR 
#define SSI_RIS_RORRIS_M        0x00000001
#define SSI_RIS_RORRIS_S        0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_MIS register.
//
//*****************************************************************************
#define SSI_MIS_TXMIS           0x00000008  // SSI SSITXINTR masked state (RO) 
                                            // Reset value: 0x0 Gives the 
                                            // interrupt state (after masking) 
                                            // of SSITXINTR 
#define SSI_MIS_TXMIS_M         0x00000008
#define SSI_MIS_TXMIS_S         3
#define SSI_MIS_RXMIS           0x00000004  // SSI SSIRXINTR masked state (RO) 
                                            // Reset value: 0x0 Gives the 
                                            // interrupt state (after masking) 
                                            // of SSIRXINTR 
#define SSI_MIS_RXMIS_M         0x00000004
#define SSI_MIS_RXMIS_S         2
#define SSI_MIS_RTMIS           0x00000002  // SSI SSIRTINTR masked state (RO) 
                                            // Reset value: 0x0 Gives the 
                                            // interrupt state (after masking) 
                                            // of SSIRTINTR 
#define SSI_MIS_RTMIS_M         0x00000002
#define SSI_MIS_RTMIS_S         1
#define SSI_MIS_RORMIS          0x00000001  // SSI SSIRORINTR masked state 
                                            // (RO) Reset value: 0x0 Gives the 
                                            // interrupt state (after masking) 
                                            // of SSIRORINTR 
#define SSI_MIS_RORMIS_M        0x00000001
#define SSI_MIS_RORMIS_S        0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_ICR register.
//
//*****************************************************************************
#define SSI_ICR_RTIC            0x00000002  // SSI receive time-out interrupt 
                                            // clear (W1C) Reset value: 0x0 0: 
                                            // No effect on interrupt 1: Clears 
                                            // interrupt 
#define SSI_ICR_RTIC_M          0x00000002
#define SSI_ICR_RTIC_S          1
#define SSI_ICR_RORIC           0x00000001  // SSI receive overrun interrupt 
                                            // clear (W1C) Reset value: 0x0 0: 
                                            // No effect on interrupt 1: Clears 
                                            // interrupt 
#define SSI_ICR_RORIC_M         0x00000001
#define SSI_ICR_RORIC_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_DMACTL register.
//
//*****************************************************************************
#define SSI_DMACTL_TXDMAE       0x00000002  // Transmit DMA enable 0: uDMA for 
                                            // the transmit FIFO is disabled. 
                                            // 1: uDMA for the transmit FIFO is 
                                            // enabled. 
#define SSI_DMACTL_TXDMAE_M     0x00000002
#define SSI_DMACTL_TXDMAE_S     1
#define SSI_DMACTL_RXDMAE       0x00000001  // Receive DMA enable 0: uDMA for 
                                            // the receive FIFO is disabled. 1: 
                                            // uDMA for the receive FIFO is 
                                            // enabled. 
#define SSI_DMACTL_RXDMAE_M     0x00000001
#define SSI_DMACTL_RXDMAE_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the SSI_O_CC register.
//
//*****************************************************************************
#define SSI_CC_CS_M             0x00000007  // SSI baud and system clock 
                                            // source The following bits 
                                            // determine the clock source that 
                                            // generates the baud and system 
                                            // clocks for the SSI. bit0 
                                            // (PIOSC): 1: The SSI baud clock 
                                            // is determined by the IO DIV 
                                            // setting in the system 
                                            // controller. 0: The SSI baud 
                                            // clock is determined by the SYS 
                                            // DIV setting in the system 
                                            // controller. bit1: Unused bit2: 
                                            // (DSEN) Only meaningful when the 
                                            // system is in deep sleep mode. 
                                            // This bit is a don't care when 
                                            // not in sleep mode. 1: The SSI 
                                            // system clock is running on the 
                                            // same clock as the baud clock, as 
                                            // per PIOSC setting above. 0: The 
                                            // SSI system clock is determined 
                                            // by the SYS DIV setting in the 
                                            // system controller. 
#define SSI_CC_CS_S             0


#endif // __HW_SSI_H__

