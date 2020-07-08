/******************************************************************************
*  Filename:       hw_uart.h
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

#ifndef __HW_UART_H__
#define __HW_UART_H__

//*****************************************************************************
//
// The following are defines for the UART register offsets.
//
//*****************************************************************************
#define UART_O_DR               0x00000000  // UART data Important: This 
                                            // register is read-sensitive. See 
                                            // the register description for 
                                            // details. This register is the 
                                            // data register (the interface to 
                                            // the FIFOs). For transmitted 
                                            // data, if the FIFO is enabled, 
                                            // data written to this location is 
                                            // pushed onto the transmit FIFO. 
                                            // If the FIFO is disabled, data is 
                                            // stored in the transmitter 
                                            // holding register (the bottom 
                                            // word of the transmit FIFO). A 
                                            // write to this register initiates 
                                            // a transmission from the UART. 
                                            // For received data, if the FIFO 
                                            // is enabled, the data byte and 
                                            // the 4-bit status (break, frame, 
                                            // parity, and overrun) is pushed 
                                            // onto the 12-bit wide receive 
                                            // FIFO. If the FIFO is disabled, 
                                            // the data byte and status are 
                                            // stored in the receiving holding 
                                            // register (the bottom word of the 
                                            // receive FIFO). The received data 
                                            // can be retrieved by reading this 
                                            // register. 
#define UART_O_RSR              0x00000004  // UART receive status and error 
                                            // clear The RSR/ECR register is 
                                            // the receive status register and 
                                            // error clear register. In 
                                            // addition to the DR register, 
                                            // receive status can also be read 
                                            // from the RSR register. If the 
                                            // status is read from this 
                                            // register, then the status 
                                            // information corresponds to the 
                                            // entry read from DR before 
                                            // reading RSR. The status 
                                            // information for overrun is set 
                                            // immediately when an overrun 
                                            // condition occurs. The RSR 
                                            // register cannot be written. 
                                            // Read-only status register 
#define UART_O_ECR              0x00000004  // UART receive status and error 
                                            // clear The RSR/ECR register is 
                                            // the receive status 
                                            // register/error clear register. A 
                                            // write of any value to the ECR 
                                            // register clears the framing, 
                                            // parity, break, and overrun 
                                            // errors. All the bits are cleared 
                                            // on reset. Write-only error clear 
                                            // register 
#define UART_O_FR               0x00000018  // UART flag The FR register is 
                                            // the flag register. After reset, 
                                            // the TXFF, RXFF, and BUSY bits 
                                            // are 0, and TXFE and RXFE bits 
                                            // are 1. The CTS bit indicate the 
                                            // modem flow control. Note that 
                                            // the modem bits are only 
                                            // implemented on UART1 and are 
                                            // tied inactive on UART0. Due to 
                                            // this difference, the reset state 
                                            // of the UART0 FR register is 
                                            // 0x90, while UART1 FR register 
                                            // reset state 0x197 . 
#define UART_O_ILPR             0x00000020  // UART IrDA low-power register 
                                            // The ILPR register stores the 
                                            // 8-bit low-power counter divisor 
                                            // value used to derive the 
                                            // low-power SIR pulse width clock 
                                            // by dividing down the system 
                                            // clock (SysClk). All the bits are 
                                            // cleared when reset. The internal 
                                            // IrLPBaud16 clock is generated by 
                                            // dividing down SysClk according 
                                            // to the low-power divisor value 
                                            // written to ILPR. The duration of 
                                            // SIR pulses generated when 
                                            // low-power mode is enabled is 
                                            // three times the period of the 
                                            // IrLPBaud16 clock. The low-power 
                                            // divisor value is calculated as 
                                            // follows: ILPDVSR = SysClk / 
                                            // FIrLPBaud16 where FIrLPBaud16 is 
                                            // nominally 1.8432 MHz The divisor 
                                            // must be programmed such that 
                                            // FIrLPBaud16 is in the range 1.42 
                                            // MHz to 2.12 MHz, resulting in a 
                                            // low-power pulse duration of 
                                            // 1.41-2.11 us (three times the 
                                            // period of IrLPBaud16). The 
                                            // minimum frequency of IrLPBaud16 
                                            // ensures that pulses less than 
                                            // one period of IrLPBaud16 are 
                                            // rejected, but pulses greater 
                                            // than 1.4 us are accepted as 
                                            // valid pulses. Note: Zero is an 
                                            // illegal value. Programming a 
                                            // zero value results in no 
                                            // IrLPBaud16 pulses being 
                                            // generated. 
#define UART_O_IBRD             0x00000024  // UART integer baud-rate divisor 
                                            // The IBRD register is the integer 
                                            // part of the baud-rate divisor 
                                            // value. All the bits are cleared 
                                            // on reset. The minimum possible 
                                            // divide ratio is 1 (when IBRD = 
                                            // 0), in which case the FBRD 
                                            // register is ignored. When 
                                            // changing the IBRD register, the 
                                            // new value does not take effect 
                                            // until transmission or reception 
                                            // of the current character is 
                                            // complete. Any changes to the 
                                            // baud-rate divisor must be 
                                            // followed by a write to the LCRH 
                                            // register. 
#define UART_O_FBRD             0x00000028  // UART fractional baud-rate 
                                            // divisor The FBRD register is the 
                                            // fractional part of the baud-rate 
                                            // divisor value. All the bits are 
                                            // cleared on reset. When changing 
                                            // the FBRD register, the new value 
                                            // does not take effect until 
                                            // transmission or reception of the 
                                            // current character is complete. 
                                            // Any changes to the baud-rate 
                                            // divisor must be followed by a 
                                            // write to the LCRH register. 
#define UART_O_LCRH             0x0000002C  // UART line control The LCRH 
                                            // register is the line control 
                                            // register. Serial parameters such 
                                            // as data length, parity, and stop 
                                            // bit selection are implemented in 
                                            // this register. When updating the 
                                            // baud-rate divisor (IBRD and/or 
                                            // IFRD), the LCRH register must 
                                            // also be written. The write 
                                            // strobe for the baud-rate divisor 
                                            // registers is tied to the LCRH 
                                            // register. 
#define UART_O_CTL              0x00000030  // UART control The CTL register 
                                            // is the control register. All the 
                                            // bits are cleared on reset except 
                                            // for the transmit enable (TXE) 
                                            // and receive enable (RXE) bits, 
                                            // which are set. To enable the 
                                            // UART module, the UARTEN bit must 
                                            // be set. If software requires a 
                                            // configuration change in the 
                                            // module, the UARTEN bit must be 
                                            // cleared before the configuration 
                                            // changes are written. If the UART 
                                            // is disabled during a transmit or 
                                            // receive operation, the current 
                                            // transaction is completed before 
                                            // the UART stopping. Note: The 
                                            // UARTCTL register should not be 
                                            // changed while the UART is 
                                            // enabled or else the results are 
                                            // unpredictable. The following 
                                            // sequence is recommended for 
                                            // making changes to the UARTCTL 
                                            // register: 1. Disable the UART. 
                                            // 2. Wait for the end of 
                                            // transmission or reception of the 
                                            // current character. 3. Flush the 
                                            // transmit FIFO by clearing bit 4 
                                            // (FEN) in the line control 
                                            // register (UARTLCRH). 4. 
                                            // Reprogram the control register. 
                                            // 5. Enable the UART. 
#define UART_O_IFLS             0x00000034  // UART interrupt FIFO level 
                                            // select The IFLS register is the 
                                            // interrupt FIFO level select 
                                            // register. This register can be 
                                            // used to define the FIFO level at 
                                            // which the TXRIS and RXRIS bits 
                                            // in the RIS register are 
                                            // triggered. The interrupts are 
                                            // generated based on a transition 
                                            // through a level rather than 
                                            // being based on the level. That 
                                            // is, the interrupts are generated 
                                            // when the fill level progresses 
                                            // through the trigger level. For 
                                            // example, if the receive trigger 
                                            // level is set to the half-way 
                                            // mark, the interrupt is triggered 
                                            // as the module is receiving the 
                                            // 9th character. Out of reset, the 
                                            // TXIFLSEL and RXIFLSEL bits are 
                                            // configured so that the FIFOs 
                                            // trigger an interrupt at the 
                                            // half-way mark. 
#define UART_O_IM               0x00000038  // UART interrupt mask The IM 
                                            // register is the interrupt mask 
                                            // set/clear register. On a read, 
                                            // this register gives the current 
                                            // value of the mask on the 
                                            // relevant interrupt. Setting a 
                                            // bit allows the corresponding raw 
                                            // interrupt signal to be routed to 
                                            // the interrupt controller. 
                                            // Clearing a bit prevents the raw 
                                            // interrupt signal from being sent 
                                            // to the interrupt controller. 
#define UART_O_RIS              0x0000003C  // UART raw interrupt status The 
                                            // RIS register is the raw 
                                            // interrupt status register. On a 
                                            // read, this register gives the 
                                            // current raw status value of the 
                                            // corresponding interrupt. A write 
                                            // has no effect. Note that the HW 
                                            // modem flow control bits are only 
                                            // implemented on UART1 and are 
                                            // tied inactive on UART0. 
#define UART_O_MIS              0x00000040  // UART masked interrupt status 
                                            // The MIS register is the masked 
                                            // interrupt status register. On a 
                                            // read, this register gives the 
                                            // current masked status value of 
                                            // the corresponding interrupt. A 
                                            // write has no effect. 
#define UART_O_ICR              0x00000044  // UART interrupt clear The ICR 
                                            // register is the interrupt clear 
                                            // register. On a write of 1, the 
                                            // corresponding interrupt (both 
                                            // raw interrupt and masked 
                                            // interrupt, if enabled) is 
                                            // cleared. A write of 0 has no 
                                            // effect. 
#define UART_O_DMACTL           0x00000048  // UART DMA control The DMACTL 
                                            // register is the DMA control 
                                            // register. 
#define UART_O_LCTL             0x00000090  // UART LIN control The LCTL 
                                            // register is the configures the 
                                            // operation of the UART when in 
                                            // LIN mode. 
#define UART_O_LSS              0x00000094  // LIN snap shot The LSS register 
                                            // captures the free-running timer 
                                            // value when either the sync edge 
                                            // 1 or the sync edge 5 is detected 
                                            // in LIN mode. 
#define UART_O_LTIM             0x00000098  // UART LIN timer The LTIM 
                                            // register contains the current 
                                            // timer value for the free-running 
                                            // timer that is used to calculate 
                                            // the baud rate when in LIN slave 
                                            // mode. The value in this register 
                                            // is used along with the value in 
                                            // the UART LIN snap shot (LSS) 
                                            // register to adjust the baud rate 
                                            // to match that of the master. 
#define UART_O_NINEBITADDR      0x000000A4  // UART 9-bit self address The 
                                            // NINEBITADDR register is used to 
                                            // write the specific address that 
                                            // should be matched with the 
                                            // receiving byte when the 9-bit 
                                            // address mask (NINEBITAMASK) is 
                                            // set to 0xFF. This register is 
                                            // used in conjunction with 
                                            // NINEBITAMASK to form a match for 
                                            // address-byte received. 
#define UART_O_NINEBITAMASK     0x000000A8  // UART 9-bit self address mask 
                                            // The NINEBITAMASK register is 
                                            // used to enable the address mask 
                                            // for 9-bit mode. The lower 
                                            // address bits are masked to 
                                            // create a range of address to be 
                                            // matched with the received 
                                            // address byte. 
#define UART_O_PP               0x00000FC0  // UART peripheral properties The 
                                            // PP register provides information 
                                            // regarding the properties of the 
                                            // UART module. 
#define UART_O_CC               0x00000FC8  // UART clock configuration The CC 
                                            // register controls the baud and 
                                            // system clocks sources for the 
                                            // UART module. For more 
                                            // information, see the section 
                                            // called "Baud-Rate Generation". 
                                            // Note: If the PIOSC is used for 
                                            // the UART baud clock, the system 
                                            // clock frequency must be at least 
                                            // 9 MHz in run mode. 


//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_DR register.
//
//*****************************************************************************
#define UART_DR_OE              0x00000800  // UART overrun error 1: New data 
                                            // was received when the FIFO was 
                                            // full, resulting in data loss. 0: 
                                            // No data has been lost due to a 
                                            // FIFO overrun. 
#define UART_DR_OE_M            0x00000800
#define UART_DR_OE_S            11
#define UART_DR_BE              0x00000400  // UART break error 1: A break 
                                            // condition has been detected, 
                                            // indicating that the receive data 
                                            // input was held low for longer 
                                            // than a full-word transmission 
                                            // time (defined as start, data, 
                                            // parity, and stop bits). 0: No 
                                            // break condition has occurred. In 
                                            // FIFO mode, this error is 
                                            // associated with the character at 
                                            // the top of the FIFO. When a 
                                            // break occurs, only the one 0 
                                            // character is loaded into the 
                                            // FIFO. The next character is only 
                                            // enabled after the received data 
                                            // input goes to a 1 (marking 
                                            // state), and the next valid start 
                                            // bit is received. 
#define UART_DR_BE_M            0x00000400
#define UART_DR_BE_S            10
#define UART_DR_PE              0x00000200  // UART parity error 1: The parity 
                                            // of the received data character 
                                            // does not match the parity 
                                            // defined by bits 2 and 7 of the 
                                            // UARTLCRH register 0: No parity 
                                            // error has occurred. In FIFO 
                                            // mode, this error is associated 
                                            // with the character at the top of 
                                            // the FIFO. 
#define UART_DR_PE_M            0x00000200
#define UART_DR_PE_S            9
#define UART_DR_FE              0x00000100  // UART framing error 1: The 
                                            // received character does not have 
                                            // a valid stop bit (a valid stop 
                                            // bit is 1). 0: No framing error 
                                            // has occurred. 
#define UART_DR_FE_M            0x00000100
#define UART_DR_FE_S            8
#define UART_DR_DATA_M          0x000000FF  // Data transmitted or received 
                                            // Data that is to be transmitted 
                                            // via the UART is written to this 
                                            // field. When read, this field 
                                            // contains the data that was 
                                            // received by the UART. 
#define UART_DR_DATA_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_RSR register.
//
//*****************************************************************************
#define UART_RSR_OE             0x00000008  // UART overrun error 1: New data 
                                            // was received when the FIFO was 
                                            // full, resulting in data loss. 0: 
                                            // No data has been lost due to a 
                                            // FIFO overrun. This bit is 
                                            // cleared by a write to UARTECR. 
                                            // The FIFO contents remain valid 
                                            // because no further data is 
                                            // written when the FIFO is full, 
                                            // only the contents of the shift 
                                            // register are overwritten. The 
                                            // CPU must read the data in order 
                                            // to empty the FIFO. 
#define UART_RSR_OE_M           0x00000008
#define UART_RSR_OE_S           3
#define UART_RSR_BE             0x00000004  // UART break error 1: A break 
                                            // condition has been detected, 
                                            // indicating that the receive data 
                                            // input was held low for longer 
                                            // than a full-word transmission 
                                            // time (defined as start, data, 
                                            // parity, and stop bits). 0: No 
                                            // break condition has occurred. 
                                            // This bit is cleared to 0 by a 
                                            // write to UARTECR. In FIFO mode, 
                                            // this error is associated with 
                                            // the character at the top of the 
                                            // FIFO. When a break occurs, only 
                                            // one 0 character is loaded into 
                                            // the FIFO. The next character is 
                                            // only enabled after the receive 
                                            // data input goes to a 1 (marking 
                                            // state) and the next valid start 
                                            // bit is received. 
#define UART_RSR_BE_M           0x00000004
#define UART_RSR_BE_S           2
#define UART_RSR_PE             0x00000002  // UART parity error 1: The parity 
                                            // of the received data character 
                                            // does not match the parity 
                                            // defined by bits 2 and 7 of the 
                                            // UARTLCRH register. 0: No parity 
                                            // error has occurred. This bit is 
                                            // cleared to 0 by a write to 
                                            // UARTECR. 
#define UART_RSR_PE_M           0x00000002
#define UART_RSR_PE_S           1
#define UART_RSR_FE             0x00000001  // UART framing error 1: The 
                                            // received character does not have 
                                            // a valid stop bit (a valid stop 
                                            // bit is 1). 0: No framing error 
                                            // has occurred. This bit is 
                                            // cleared to 0 by a write to 
                                            // UARTECR. In FIFO mode, this 
                                            // error is associated with the 
                                            // character at the top of the 
                                            // FIFO. 
#define UART_RSR_FE_M           0x00000001
#define UART_RSR_FE_S           0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_ECR register.
//
//*****************************************************************************
#define UART_ECR_DATA_M         0x000000FF  // Error clear A write to this 
                                            // register of any data clears the 
                                            // framing, parity, break, and 
                                            // overrun flags. 
#define UART_ECR_DATA_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_FR register.
//
//*****************************************************************************
#define UART_FR_TXFE            0x00000080  // UART transmit FIFO empty The 
                                            // meaning of this bit depends on 
                                            // the state of the FEN bit in the 
                                            // UARTLCRH register. 1: If the 
                                            // FIFO is disabled (FEN is 0), the 
                                            // transmit holding register is 
                                            // empty. If the FIFO is enabled 
                                            // (FEN is 1), the transmit FIFO is 
                                            // empty. 0: The transmitter has 
                                            // data to transmit. 
#define UART_FR_TXFE_M          0x00000080
#define UART_FR_TXFE_S          7
#define UART_FR_RXFF            0x00000040  // UART receive FIFO full The 
                                            // meaning of this bit depends on 
                                            // the state of the FEN bit in the 
                                            // UARTLCRH register. 1: If the 
                                            // FIFO is disabled (FEN is 0), the 
                                            // receive holding register is 
                                            // full. If the FIFO is enabled 
                                            // (FEN is 1), the receive FIFO is 
                                            // full. 0: The receiver can 
                                            // receive data. 
#define UART_FR_RXFF_M          0x00000040
#define UART_FR_RXFF_S          6
#define UART_FR_TXFF            0x00000020  // UART transmit FIFO full The 
                                            // meaning of this bit depends on 
                                            // the state of the FEN bit in the 
                                            // UARTLCRH register. 1: If the 
                                            // FIFO is disabled (FEN is 0), the 
                                            // transmit holding register is 
                                            // full. If the FIFO is enabled 
                                            // (FEN is 1), the transmit FIFO is 
                                            // full. 0: The transmitter is not 
                                            // full. 
#define UART_FR_TXFF_M          0x00000020
#define UART_FR_TXFF_S          5
#define UART_FR_RXFE            0x00000010  // UART receive FIFO empty The 
                                            // meaning of this bit depends on 
                                            // the state of the FEN bit in the 
                                            // UARTLCRH register. 1: If the 
                                            // FIFO is disabled (FEN is 0), the 
                                            // receive holding register is 
                                            // empty. If the FIFO is enabled 
                                            // (FEN is 1), the receive FIFO is 
                                            // empty. 0: The receiver is not 
                                            // empty. 
#define UART_FR_RXFE_M          0x00000010
#define UART_FR_RXFE_S          4
#define UART_FR_BUSY            0x00000008  // UART busy 1: The UART is busy 
                                            // transmitting data. This bit 
                                            // remains set until the complete 
                                            // byte, including all stop bits, 
                                            // has been sent from the shift 
                                            // register. 0: The UART is not 
                                            // busy. This bit is set as soon as 
                                            // the transmit FIFO becomes 
                                            // non-empty (regardless of whether 
                                            // UART is enabled). 
#define UART_FR_BUSY_M          0x00000008
#define UART_FR_BUSY_S          3
#define UART_FR_CTS             0x00000001  // Clear to send (UART1 only, 
                                            // reserved for UART0). 1: The 
                                            // U1CTS signal is asserted. 0: The 
                                            // U1CTS signal is not asserted. 
#define UART_FR_CTS_M           0x00000001
#define UART_FR_CTS_S           0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_ILPR register.
//
//*****************************************************************************
#define UART_ILPR_ILPDVSR_M     0x000000FF  // IrDA low-power divisor This 
                                            // field contains the 8-bit 
                                            // low-power divisor value. 
#define UART_ILPR_ILPDVSR_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_IBRD register.
//
//*****************************************************************************
#define UART_IBRD_DIVINT_M      0x0000FFFF  // Integer baud-rate divisor 
#define UART_IBRD_DIVINT_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_FBRD register.
//
//*****************************************************************************
#define UART_FBRD_DIVFRAC_M     0x0000003F  // Fractional baud-rate divisor 
#define UART_FBRD_DIVFRAC_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LCRH register.
//
//*****************************************************************************
#define UART_LCRH_SPS           0x00000080  // UART stick parity select When 
                                            // bits 1, 2, and 7 of UARTLCRH are 
                                            // set, the parity bit is 
                                            // transmitted and checked as a 0. 
                                            // When bits 1 and 7 are set and 2 
                                            // is cleared, the parity bit is 
                                            // transmitted and checked as a 1. 
                                            // When this bit is cleared, stick 
                                            // parity is disabled. 
#define UART_LCRH_SPS_M         0x00000080
#define UART_LCRH_SPS_S         7
#define UART_LCRH_WLEN_M        0x00000060  // UART word length The bits 
                                            // indicate the number of data bits 
                                            // transmitted or received in a 
                                            // frame as follows: 0x0: 5 bits 
                                            // (default) 0x1: 6 bits 0x2: 7 
                                            // bits 0x3: 8 bits 
#define UART_LCRH_WLEN_S        5
#define UART_LCRH_FEN           0x00000010  // UART enable FIFOs 1: The 
                                            // transmit and receive FIFObuffers 
                                            // are enabled (FIFOmode). 0: The 
                                            // FIFOs are disabled (Character 
                                            // mode). The FIFOs become 
                                            // 1-byte-deep holding registers. 
#define UART_LCRH_FEN_M         0x00000010
#define UART_LCRH_FEN_S         4
#define UART_LCRH_STP2          0x00000008  // UART two stop bits select 1: 
                                            // Two stop bits are transmitted at 
                                            // the end of a frame. The receive 
                                            // logic does not check for two 
                                            // stop bits being received. 0: One 
                                            // stop bit is transmitted at the 
                                            // end of a frame. 
#define UART_LCRH_STP2_M        0x00000008
#define UART_LCRH_STP2_S        3
#define UART_LCRH_EPS           0x00000004  // UART even parity select 1: Even 
                                            // parity generation and checking 
                                            // is performed during transmission 
                                            // and reception, which checks for 
                                            // an even number of 1s in data and 
                                            // parity bits. 0: Odd parity is 
                                            // performed, which checks for an 
                                            // odd number of 1s. This bit has 
                                            // no effect when parity is 
                                            // disabled by the PEN bit. 
#define UART_LCRH_EPS_M         0x00000004
#define UART_LCRH_EPS_S         2
#define UART_LCRH_PEN           0x00000002  // UART parity enable 1: Parity 
                                            // checking and generation is 
                                            // enabled. 0: Parity is disabled 
                                            // and no parity bit is added to 
                                            // the data frame. 
#define UART_LCRH_PEN_M         0x00000002
#define UART_LCRH_PEN_S         1
#define UART_LCRH_BRK           0x00000001  // UART send break 1: A low level 
                                            // is continually output on the 
                                            // UnTx signal, after completing 
                                            // transmission of the current 
                                            // character. For the proper 
                                            // execution of the break command, 
                                            // software must set this bit for 
                                            // at least two frames (character 
                                            // periods). 0: Normal use 
#define UART_LCRH_BRK_M         0x00000001
#define UART_LCRH_BRK_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_CTL register.
//
//*****************************************************************************
#define UART_CTL_CTSEN          0x00008000  // U1CTS Hardware Flow control 
                                            // enable 1: When U1CTS input is 
                                            // asserted, UART1 can transmit 
                                            // data. 0: U1CTS does not control 
                                            // UART1 data transmission. Note: 
                                            // Only used for UART1. This bit is 
                                            // reserved RO for UART0. 
#define UART_CTL_CTSEN_M        0x00008000
#define UART_CTL_CTSEN_S        15
#define UART_CTL_RTSEN          0x00004000  // U1RTS Hardware Flow control 
                                            // enable 1: U1RTS indicates the 
                                            // state of UART1 receive FIFO. 
                                            // U1RTS remains asserted until the 
                                            // preprogrammed watermark level is 
                                            // reached, indicating that the 
                                            // UART1 RXFIFO has no space to 
                                            // store additional characters. 0: 
                                            // U1RTS does not indicate state of 
                                            // UART1 RX FIFO. Note: Only used 
                                            // for UART1. This bit is reserved 
                                            // RO for UART0. 
#define UART_CTL_RTSEN_M        0x00004000
#define UART_CTL_RTSEN_S        14
#define UART_CTL_RXE            0x00000200  // UART receive enable 1: The 
                                            // receive section of the UART is 
                                            // enabled. 0: The receive section 
                                            // of the UART is disabled. If the 
                                            // UART is disabled in the middle 
                                            // of a receive, it completes the 
                                            // current character before 
                                            // stopping. Note: To enable 
                                            // reception, the UARTEN bit must 
                                            // also be set. 
#define UART_CTL_RXE_M          0x00000200
#define UART_CTL_RXE_S          9
#define UART_CTL_TXE            0x00000100  // UART transmit enable 1: The 
                                            // transmit section of the UART is 
                                            // enabled. 0: The transmit section 
                                            // of the UART is disabled. If the 
                                            // UART is disabled in the middle 
                                            // of a transmission, it completes 
                                            // the current character before 
                                            // stopping. Note: To enable 
                                            // transmission, the UARTEN bit 
                                            // must also be set. 
#define UART_CTL_TXE_M          0x00000100
#define UART_CTL_TXE_S          8
#define UART_CTL_LBE            0x00000080  // UART loop back enable 1: The 
                                            // UnTx path is fed through the 
                                            // UnRx path. 0: Normal operation 
#define UART_CTL_LBE_M          0x00000080
#define UART_CTL_LBE_S          7
#define UART_CTL_LIN            0x00000040  // LIN mode enable 1: The UART 
                                            // operates in LIN mode. 0: Normal 
                                            // operation 
#define UART_CTL_LIN_M          0x00000040
#define UART_CTL_LIN_S          6
#define UART_CTL_HSE            0x00000020  // High-speed enable 0: The UART 
                                            // is clocked using the system 
                                            // clock divided by 16. 1: The UART 
                                            // is clocked using the system 
                                            // clock divided by 8. Note: System 
                                            // clock used is also dependent on 
                                            // the baud-rate divisor 
                                            // configuration (See Universal 
                                            // Asynchronous 
                                            // Receivers/Transmitters - 
                                            // Baud-Rate Generation). 
#define UART_CTL_HSE_M          0x00000020
#define UART_CTL_HSE_S          5
#define UART_CTL_EOT            0x00000010  // End of transmission This bit 
                                            // determines the behavior of the 
                                            // TXRIS bit in the UARTRIS 
                                            // register. 1: The TXRIS bit is 
                                            // set only after all transmitted 
                                            // data, including stop bits, have 
                                            // cleared the serializer. 0: The 
                                            // TXRIS bit is set when the 
                                            // transmit FIFO condition 
                                            // specified in UARTIFLS is met. 
#define UART_CTL_EOT_M          0x00000010
#define UART_CTL_EOT_S          4
#define UART_CTL_SIRLP          0x00000004  // UART SIR low-power mode This 
                                            // bit selects the IrDA encoding 
                                            // mode. 1: The UART operates in 
                                            // SIR Low-Power mode. Low-level 
                                            // bits are transmitted with a 
                                            // pulse width which is 3 times the 
                                            // period of the IrLPBaud16 input 
                                            // signal, regardless of the 
                                            // selected bit rate. 0: Low-level 
                                            // bits are transmitted as an 
                                            // active high pulse with a width 
                                            // of 3/16th of the bit period. 
                                            // Setting this bit uses less 
                                            // power, but might reduce 
                                            // transmission distances. 
#define UART_CTL_SIRLP_M        0x00000004
#define UART_CTL_SIRLP_S        2
#define UART_CTL_SIREN          0x00000002  // UART SIR enable 1: The IrDA SIR 
                                            // block is enabled, and the UART 
                                            // transmits and receives data 
                                            // using SIR protocol. 0: Normal 
                                            // operation. 
#define UART_CTL_SIREN_M        0x00000002
#define UART_CTL_SIREN_S        1
#define UART_CTL_UARTEN         0x00000001  // UART enable 1: The UART is 
                                            // enabled. 0: The UART is 
                                            // disabled. If the UART is 
                                            // disabled in the middle of 
                                            // transmission or reception, it 
                                            // completes the current character 
                                            // before stopping. 
#define UART_CTL_UARTEN_M       0x00000001
#define UART_CTL_UARTEN_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_IFLS register.
//
//*****************************************************************************
#define UART_IFLS_RXIFLSEL_M    0x00000038  // UART receive interrupt FIFO 
                                            // level select The trigger points 
                                            // for the receive interrupt are as 
                                            // follows: 0x0: RX FIFO >= 1/8 
                                            // full 0x1: RX FIFO >= 1/4 full 
                                            // 0x2: RX FIFO >= 1/2 full 
                                            // (default) 0x3: RX FIFO >= 3/4 
                                            // full 0x4: RX FIFO >= 7/8 full 
                                            // 0x5-0x7: Reserved 
#define UART_IFLS_RXIFLSEL_S    3
#define UART_IFLS_TXIFLSEL_M    0x00000007  // UART Transmit Interrupt FIFO 
                                            // Level Select The trigger points 
                                            // for the transmit interrupt are 
                                            // as follows: 0x0: TX FIFO <= 7/8 
                                            // empty 0x1: TX FIFO <= 3/4 empty 
                                            // 0x2: TX FIFO <= 1/2 empty 
                                            // (default) 0x3: TX FIFO <= 1/4 
                                            // empty 0x4: TX FIFO <= 1/8 empty 
                                            // 0x5-0x7: Reserved Note: If the 
                                            // EOT bit in UARTCTL is set, the 
                                            // transmit interrupt is generated 
                                            // once the FIFO is completely 
                                            // empty and all data including 
                                            // stop bits have left the transmit 
                                            // serializer. In this case, the 
                                            // setting of TXIFLSEL is ignored. 
#define UART_IFLS_TXIFLSEL_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_IM register.
//
//*****************************************************************************
#define UART_IM_LME5IM          0x00008000  // LIN mode edge 5 interrupt mask 
                                            // 1: An interrupt is sent to the 
                                            // interrupt controller when the 
                                            // LME5RIS bit in the UARTRIS 
                                            // register is set. 0: The LME5RIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_LME5IM_M        0x00008000
#define UART_IM_LME5IM_S        15
#define UART_IM_LME1IM          0x00004000  // LIN mode edge 1 interrupt mask 
                                            // 1: An interrupt is sent to the 
                                            // interrupt controller when the 
                                            // LME1RIS bit in the UARTRIS 
                                            // register is set. 0: The LME1RIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_LME1IM_M        0x00004000
#define UART_IM_LME1IM_S        14
#define UART_IM_LMSBIM          0x00002000  // LIN mode sync break interrupt 
                                            // mask 1: An interrupt is sent to 
                                            // the interrupt controller when 
                                            // the LMSBRIS bit in the UARTRIS 
                                            // register is set. 0: The LMSBRIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_LMSBIM_M        0x00002000
#define UART_IM_LMSBIM_S        13
#define UART_IM_NINEBITIM       0x00001000  // 9-bit mode interrupt mask 1: An 
                                            // interrupt is sent to the 
                                            // interrupt controller when the 
                                            // 9BITRIS bit in the UARTRIS 
                                            // register is set. 0: The 9BITRIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_NINEBITIM_M     0x00001000
#define UART_IM_NINEBITIM_S     12
#define UART_IM_OEIM            0x00000400  // UART overrun error interrupt 
                                            // mask 1: An interrupt is sent to 
                                            // the interrupt controller when 
                                            // the OERIS bit in the UARTRIS 
                                            // register is set. 0: The OERIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_OEIM_M          0x00000400
#define UART_IM_OEIM_S          10
#define UART_IM_BEIM            0x00000200  // UART break error interrupt mask 
                                            // 1: An interrupt is sent to the 
                                            // interrupt controller when the 
                                            // BERIS bit in the UARTRIS 
                                            // register is set. 0: The BERIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_BEIM_M          0x00000200
#define UART_IM_BEIM_S          9
#define UART_IM_PEIM            0x00000100  // UART parity error interrupt 
                                            // mask 1: An interrupt is sent to 
                                            // the interrupt controller when 
                                            // the PERIS bit in the UARTRIS 
                                            // register is set. 0: The PERIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_PEIM_M          0x00000100
#define UART_IM_PEIM_S          8
#define UART_IM_FEIM            0x00000080  // UART framing error interrupt 
                                            // mask 1: An interrupt is sent to 
                                            // the interrupt controller when 
                                            // the FERIS bit in the UARTRIS 
                                            // register is set. 0: The FERIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_FEIM_M          0x00000080
#define UART_IM_FEIM_S          7
#define UART_IM_RTIM            0x00000040  // UART receive time-out interrupt 
                                            // mask 1: An interrupt is sent to 
                                            // the interrupt controller when 
                                            // the RTRIS bit in the UARTRIS 
                                            // register is set. 0: The RTRIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_RTIM_M          0x00000040
#define UART_IM_RTIM_S          6
#define UART_IM_TXIM            0x00000020  // UART transmit interrupt mask 1: 
                                            // An interrupt is sent to the 
                                            // interrupt controller when the 
                                            // TXRIS bit in the UARTRIS 
                                            // register is set. 0: The TXRIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_TXIM_M          0x00000020
#define UART_IM_TXIM_S          5
#define UART_IM_RXIM            0x00000010  // UART receive interrupt mask 1: 
                                            // An interrupt is sent to the 
                                            // interrupt controller when the 
                                            // RXRIS bit in the UARTRIS 
                                            // register is set. 0: The RXRIS 
                                            // interrupt is suppressed and not 
                                            // sent to the interrupt 
                                            // controller. 
#define UART_IM_RXIM_M          0x00000010
#define UART_IM_RXIM_S          4
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_RIS register.
//
//*****************************************************************************
#define UART_RIS_LME5RIS        0x00008000  // LIN mode edge 5 raw interrupt 
                                            // status 1: The timer value at the 
                                            // 5th falling edge of the LIN sync 
                                            // field has been captured. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the LME5IC bit in 
                                            // the UARTICR register. 
#define UART_RIS_LME5RIS_M      0x00008000
#define UART_RIS_LME5RIS_S      15
#define UART_RIS_LME1RIS        0x00004000  // LIN mode edge 1 raw interrupt 
                                            // status 1: The timer value at the 
                                            // 1st falling edge of the LIN Sync 
                                            // Field has been captured. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the LME1IC bit in 
                                            // the UARTICR register. 
#define UART_RIS_LME1RIS_M      0x00004000
#define UART_RIS_LME1RIS_S      14
#define UART_RIS_LMSBRIS        0x00002000  // LIN mode sync break raw 
                                            // interrupt status 1: A LIN sync 
                                            // break has been detected. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the LMSBIC bit in 
                                            // the UARTICR register. 
#define UART_RIS_LMSBRIS_M      0x00002000
#define UART_RIS_LMSBRIS_S      13
#define UART_RIS_NINEBITRIS     0x00001000  // 9-mit mode raw interrupt status 
                                            // 1: A receive address match has 
                                            // occurred. 0: No interrupt This 
                                            // bit is cleared by writing 1 to 
                                            // the 9BITIC bit in the UARTICR 
                                            // register. 
#define UART_RIS_NINEBITRIS_M   0x00001000
#define UART_RIS_NINEBITRIS_S   12
#define UART_RIS_OERIS          0x00000400  // UART overrun error raw 
                                            // interrupt status 1: An overrun 
                                            // error has occurred. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the OEIC bit in the 
                                            // UARTICR register. 
#define UART_RIS_OERIS_M        0x00000400
#define UART_RIS_OERIS_S        10
#define UART_RIS_BERIS          0x00000200  // UART break error raw interrupt 
                                            // status 1: A break error has 
                                            // occurred. 0: No interrupt This 
                                            // bit is cleared by writing 1 to 
                                            // the BEIC bit in the UARTICR 
                                            // register. 
#define UART_RIS_BERIS_M        0x00000200
#define UART_RIS_BERIS_S        9
#define UART_RIS_PERIS          0x00000100  // UART parity error raw interrupt 
                                            // status 1: A parity error has 
                                            // occurred. 0: No interrupt This 
                                            // bit is cleared by writing 1 to 
                                            // the PEIC bit in the UARTICR 
                                            // register. 
#define UART_RIS_PERIS_M        0x00000100
#define UART_RIS_PERIS_S        8
#define UART_RIS_FERIS          0x00000080  // UART framing error raw 
                                            // interrupt status 1: A framing 
                                            // error has occurred. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the FEIC bit in the 
                                            // UARTICR register. 
#define UART_RIS_FERIS_M        0x00000080
#define UART_RIS_FERIS_S        7
#define UART_RIS_RTRIS          0x00000040  // UART receive time-out raw 
                                            // interrupt status 1: A receive 
                                            // time out has occurred. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the RTIC bit in the 
                                            // UARTICR register. 
#define UART_RIS_RTRIS_M        0x00000040
#define UART_RIS_RTRIS_S        6
#define UART_RIS_TXRIS          0x00000020  // UART transmit raw interrupt 
                                            // status 1: If the EOT bit in the 
                                            // UARTCTL register is clear, the 
                                            // transmit FIFO level has passed 
                                            // through the condition defined in 
                                            // the UARTIFLS register. If the 
                                            // EOT bit is set, the last bit of 
                                            // all transmitted data and flags 
                                            // has left the serializer. 0: No 
                                            // interrupt This bit is cleared by 
                                            // writing 1 to the TXIC bit in the 
                                            // UARTICR register. 
#define UART_RIS_TXRIS_M        0x00000020
#define UART_RIS_TXRIS_S        5
#define UART_RIS_RXRIS          0x00000010  // UART receive raw interrupt 
                                            // status 1: The receive FIFO level 
                                            // has passed through the condition 
                                            // defined in the UARTIFLS 
                                            // register. 0: No interrupt This 
                                            // bit is cleared by writing 1 to 
                                            // the RXIC bit in the UARTICR 
                                            // register. 
#define UART_RIS_RXRIS_M        0x00000010
#define UART_RIS_RXRIS_S        4
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_MIS register.
//
//*****************************************************************************
#define UART_MIS_LME5MIS        0x00008000  // LIN mode edge 5 masked 
                                            // interrupt status 1: An unmasked 
                                            // interrupt was signaled due to 
                                            // the 5th falling edge of the LIN 
                                            // sync field. 0: An interrupt has 
                                            // not occurred or is masked. This 
                                            // bit is cleared by writing 1 to 
                                            // the LME5IC bit in the UARTICR 
                                            // register. 
#define UART_MIS_LME5MIS_M      0x00008000
#define UART_MIS_LME5MIS_S      15
#define UART_MIS_LME1MIS        0x00004000  // LIN mode edge 1 masked 
                                            // interrupt status 1: An unmasked 
                                            // interrupt was signaled due to 
                                            // the 1st falling edge of the LIN 
                                            // sync field. 0: An interrupt has 
                                            // not occurred or is masked. This 
                                            // bit is cleared by writing 1 to 
                                            // the LME1IC bit in the UARTICR 
                                            // register. 
#define UART_MIS_LME1MIS_M      0x00004000
#define UART_MIS_LME1MIS_S      14
#define UART_MIS_LMSBMIS        0x00002000  // LIN mode sync break masked 
                                            // interrupt status 1: An unmasked 
                                            // interrupt was signaled due to 
                                            // the receipt of a LIN sync break. 
                                            // 0: An interrupt has not occurred 
                                            // or is masked. This bit is 
                                            // cleared by writing 1 to the 
                                            // LMSBIC bit in the UARTICR 
                                            // register. 
#define UART_MIS_LMSBMIS_M      0x00002000
#define UART_MIS_LMSBMIS_S      13
#define UART_MIS_NINEBITMIS     0x00001000  // 9-bit mode masked interrupt 
                                            // status 1: An unmasked interrupt 
                                            // was signaled due to a receive 
                                            // address match. 0: An interrupt 
                                            // has not occurred or is masked. 
                                            // This bit is cleared by writing 1 
                                            // to the 9BITIC bit in the UARTICR 
                                            // register. 
#define UART_MIS_NINEBITMIS_M   0x00001000
#define UART_MIS_NINEBITMIS_S   12
#define UART_MIS_OEMIS          0x00000400  // UART overrun error masked 
                                            // interrupt status 1: An unmasked 
                                            // interrupt was signaled due to an 
                                            // overrun error. 0: An interrupt 
                                            // has not occurred or is masked. 
                                            // This bit is cleared by writing 1 
                                            // to the OEIC bit in the UARTICR 
                                            // register. 
#define UART_MIS_OEMIS_M        0x00000400
#define UART_MIS_OEMIS_S        10
#define UART_MIS_BEMIS          0x00000200  // UART break error masked 
                                            // interrupt status 1: An unmasked 
                                            // interrupt was signaled due to a 
                                            // break error. 0: An interrupt has 
                                            // not occurred or is masked. This 
                                            // bit is cleared by writing 1 to 
                                            // the BEIC bit in the UARTICR 
                                            // register. 
#define UART_MIS_BEMIS_M        0x00000200
#define UART_MIS_BEMIS_S        9
#define UART_MIS_PEMIS          0x00000100  // UART parity error masked 
                                            // interrupt status 1: An unmasked 
                                            // interrupt was signaled due to a 
                                            // parity error. 0: An interrupt 
                                            // has not occurred or is masked. 
                                            // This bit is cleared by writing 1 
                                            // to the PEIC bit in the UARTICR 
                                            // register. 
#define UART_MIS_PEMIS_M        0x00000100
#define UART_MIS_PEMIS_S        8
#define UART_MIS_FEMIS          0x00000080  // UART framing error masked 
                                            // interrupt status 1: An unmasked 
                                            // interrupt was signaled due to a 
                                            // framing error. 0: An interrupt 
                                            // has not occurred or is masked. 
                                            // This bit is cleared by writing 1 
                                            // to the FEIC bit in the UARTICR 
                                            // register. 
#define UART_MIS_FEMIS_M        0x00000080
#define UART_MIS_FEMIS_S        7
#define UART_MIS_RTMIS          0x00000040  // UART receive time-out masked 
                                            // interrupt status 1: An unmasked 
                                            // interrupt was signaled due to a 
                                            // receive time out. 0: An 
                                            // interrupt has not occurred or is 
                                            // masked. This bit is cleared by 
                                            // writing 1 to the RTIC bit in the 
                                            // UARTICR register. 
#define UART_MIS_RTMIS_M        0x00000040
#define UART_MIS_RTMIS_S        6
#define UART_MIS_TXMIS          0x00000020  // UART transmit masked interrupt 
                                            // status 1: An unmasked interrupt 
                                            // was signaled due to passing 
                                            // through the specified transmit 
                                            // FIFO level (if the EOT bit is 
                                            // clear) or due to the 
                                            // transmission of the last data 
                                            // bit (if the EOT bit is set). 0: 
                                            // An interrupt has not occurred or 
                                            // is masked. This bit is cleared 
                                            // by writing 1 to the TXIC bit in 
                                            // the UARTICR register. 
#define UART_MIS_TXMIS_M        0x00000020
#define UART_MIS_TXMIS_S        5
#define UART_MIS_RXMIS          0x00000010  // UART receive masked interrupt 
                                            // status 1: An unmasked interrupt 
                                            // was signaled due to passing 
                                            // through the specified receive 
                                            // FIFO level. 0: An interrupt has 
                                            // not occurred or is masked. This 
                                            // bit is cleared by writing 1 to 
                                            // the RXIC bit in the UARTICR 
                                            // register. 
#define UART_MIS_RXMIS_M        0x00000010
#define UART_MIS_RXMIS_S        4
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_ICR register.
//
//*****************************************************************************
#define UART_ICR_LME5IC         0x00008000  // LIN mode edge 5 interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // LME5RIS bit in the UARTRIS 
                                            // register and the LME5MIS bit in 
                                            // the UARTMIS register. 
#define UART_ICR_LME5IC_M       0x00008000
#define UART_ICR_LME5IC_S       15
#define UART_ICR_LME1IC         0x00004000  // LIN mode edge 1 interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // LME1RIS bit in the UARTRIS 
                                            // register and the LME1MIS bit in 
                                            // the UARTMIS register. 
#define UART_ICR_LME1IC_M       0x00004000
#define UART_ICR_LME1IC_S       14
#define UART_ICR_LMSBIC         0x00002000  // LIN mode sync break interrupt 
                                            // clear Writing 1 to this bit 
                                            // clears the LMSBRIS bit in the 
                                            // UARTRIS register and the LMSBMIS 
                                            // bit in the UARTMIS register. 
#define UART_ICR_LMSBIC_M       0x00002000
#define UART_ICR_LMSBIC_S       13
#define UART_ICR_NINEBITIC      0x00001000  // 9-bit mode interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // 9BITRIS bit in the UARTRIS 
                                            // register and the 9BITMIS bit in 
                                            // the UARTMIS register. 
#define UART_ICR_NINEBITIC_M    0x00001000
#define UART_ICR_NINEBITIC_S    12
#define UART_ICR_OEIC           0x00000400  // Overrun error interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // OERIS bit in the UARTRIS 
                                            // register and the OEMIS bit in 
                                            // the UARTMIS register. 
#define UART_ICR_OEIC_M         0x00000400
#define UART_ICR_OEIC_S         10
#define UART_ICR_BEIC           0x00000200  // Break error interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // BERIS bit in the UARTRIS 
                                            // register and the BEMIS bit in 
                                            // the UARTMIS register. 
#define UART_ICR_BEIC_M         0x00000200
#define UART_ICR_BEIC_S         9
#define UART_ICR_PEIC           0x00000100  // Parity error interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // PERIS bit in the UARTRIS 
                                            // register and the PEMIS bit in 
                                            // the UARTMIS register. 
#define UART_ICR_PEIC_M         0x00000100
#define UART_ICR_PEIC_S         8
#define UART_ICR_FEIC           0x00000080  // Framing error interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // FERIS bit in the UARTRIS 
                                            // register and the FEMIS bit in 
                                            // the UARTMIS register. 
#define UART_ICR_FEIC_M         0x00000080
#define UART_ICR_FEIC_S         7
#define UART_ICR_RTIC           0x00000040  // Receive time-out interrupt 
                                            // clear Writing 1 to this bit 
                                            // clears the RTRIS bit in the 
                                            // UARTRIS register and the RTMIS 
                                            // bit in the UARTMIS register. 
#define UART_ICR_RTIC_M         0x00000040
#define UART_ICR_RTIC_S         6
#define UART_ICR_TXIC           0x00000020  // Transmit interrupt clear 
                                            // Writing 1 to this bit clears the 
                                            // TXRIS bit in the UARTRIS 
                                            // register and the TXMIS bit in 
                                            // the UARTMIS register. 
#define UART_ICR_TXIC_M         0x00000020
#define UART_ICR_TXIC_S         5
#define UART_ICR_RXIC           0x00000010  // Receive interrupt clear Writing 
                                            // 1 to this bit clears the RXRIS 
                                            // bit in the UARTRIS register and 
                                            // the RXMIS bit in the UARTMIS 
                                            // register. 
#define UART_ICR_RXIC_M         0x00000010
#define UART_ICR_RXIC_S         4
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_DMACTL register.
//
//*****************************************************************************
#define UART_DMACTL_DMAERR      0x00000004  // DMA on error 1: uDMA receive 
                                            // requests are automatically 
                                            // disabled when a receive error 
                                            // occurs. 0: uDMA receive requests 
                                            // are unaffected when a receive 
                                            // error occurs. 
#define UART_DMACTL_DMAERR_M    0x00000004
#define UART_DMACTL_DMAERR_S    2
#define UART_DMACTL_TXDMAE      0x00000002  // Transmit DMA enable 1: uDMA for 
                                            // the transmit FIFO is enabled. 0: 
                                            // uDMA for the transmit FIFO is 
                                            // disabled. 
#define UART_DMACTL_TXDMAE_M    0x00000002
#define UART_DMACTL_TXDMAE_S    1
#define UART_DMACTL_RXDMAE      0x00000001  // Receive DMA enable 1: uDMA for 
                                            // the receive FIFO is enabled. 0: 
                                            // uDMA for the receive FIFO is 
                                            // disabled. 
#define UART_DMACTL_RXDMAE_M    0x00000001
#define UART_DMACTL_RXDMAE_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LCTL register.
//
//*****************************************************************************
#define UART_LCTL_BLEN_M        0x00000030  // Sync break length 0x3: Sync 
                                            // break length is 16T bits 0x2: 
                                            // Sync break length is 15T bits 
                                            // 0x1: Sync break length is 14T 
                                            // bits 0x0: Sync break length is 
                                            // 13T bits (default) 
#define UART_LCTL_BLEN_S        4
#define UART_LCTL_MASTER        0x00000001  // LIN master enable 1: The UART 
                                            // operates as a LIN master. 0: The 
                                            // UART operates as a LIN slave. 
#define UART_LCTL_MASTER_M      0x00000001
#define UART_LCTL_MASTER_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LSS register.
//
//*****************************************************************************
#define UART_LSS_TSS_M          0x0000FFFF  // Timer snap shot This field 
                                            // contains the value of the 
                                            // free-running timer when either 
                                            // the sync edge 5 or the sync edge 
                                            // 1 was detected. 
#define UART_LSS_TSS_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LTIM register.
//
//*****************************************************************************
#define UART_LTIM_TIMER_M       0x0000FFFF  // Timer value This field contains 
                                            // the value of the free-running 
                                            // timer. 
#define UART_LTIM_TIMER_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UART_O_NINEBITADDR register.
//
//*****************************************************************************
#define UART_NINEBITADDR_NINEBITEN \
                                0x00008000  // Enable 9-bit mode 1: 9-bit mode 
                                            // is enabled. 0: 9-bit mode is 
                                            // disabled. 

#define UART_NINEBITADDR_NINEBITEN_M \
                                0x00008000
#define UART_NINEBITADDR_NINEBITEN_S 15
#define UART_NINEBITADDR_ADDR_M 0x000000FF  // Self address for 9-bit mode 
                                            // This field contains the address 
                                            // that should be matched when 
                                            // UART9BITAMASK is 0xFF. 
#define UART_NINEBITADDR_ADDR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// UART_O_NINEBITAMASK register.
//
//*****************************************************************************
#define UART_NINEBITAMASK_RANGE_M \
                                0x0000FF00  // Self address range for 9-bit 
                                            // mode Writing to the RANGE field 
                                            // does not have any effect; 
                                            // reading it reflects the ANDed 
                                            // output of the ADDR field in the 
                                            // UART9BITADDR register and the 
                                            // MASK field. 

#define UART_NINEBITAMASK_RANGE_S 8
#define UART_NINEBITAMASK_MASK_M \
                                0x000000FF  // Self Address Mask for 9-Bit 
                                            // Mode This field contains the 
                                            // address mask that creates a 
                                            // range of addresses that should 
                                            // be matched. 

#define UART_NINEBITAMASK_MASK_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_PP register.
//
//*****************************************************************************
#define UART_PP_NB              0x00000002  // 9-bit support 1: The UART 
                                            // module provides support for the 
                                            // transmission of 9-bit data for 
                                            // RS-485 support. 0: The UART 
                                            // module does not provide support 
                                            // for the transmission of 9-bit 
                                            // data for RS-485 support. 
#define UART_PP_NB_M            0x00000002
#define UART_PP_NB_S            1
#define UART_PP_SC              0x00000001  // Smart card support 1: The UART 
                                            // module provides smart card 
                                            // support. 0: The UART module does 
                                            // not provide smart card support. 
#define UART_PP_SC_M            0x00000001
#define UART_PP_SC_S            0
//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_CC register.
//
//*****************************************************************************
#define UART_CC_CS_M            0x00000007  // UART baud and system clock 
                                            // source The following bits 
                                            // determine the clock source that 
                                            // generates the baud and system 
                                            // clocks for the UART. bit0 
                                            // (PIOSC): 1: The UART baud clock 
                                            // is determined by the IO DIV 
                                            // setting in the system 
                                            // controller. 0: The UART baud 
                                            // clock is determined by the SYS 
                                            // DIV setting in the system 
                                            // controller. bit1: Unused bit2: 
                                            // (DSEN) Only meaningful when the 
                                            // system is in deep sleep mode. 
                                            // This bit is a don't care when 
                                            // not in sleep mode. 1: The UART 
                                            // system clock is running on the 
                                            // same clock as the baud clock, as 
                                            // per PIOSC setting above. 0: The 
                                            // UART system clock is determined 
                                            // by the SYS DIV setting in the 
                                            // system controller. 
#define UART_CC_CS_S            0


#endif // __HW_UART_H__

