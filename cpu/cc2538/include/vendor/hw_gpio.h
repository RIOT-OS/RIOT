/******************************************************************************
*  Filename:       hw_gpio.h
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

#ifndef __HW_GPIO_H__
#define __HW_GPIO_H__

//*****************************************************************************
//
// The following are defines for the GPIO register offsets.
//
//*****************************************************************************
#define GPIO_O_DATA             0x00000000  // This is the data register. In 
                                            // software control mode, values 
                                            // written in the GPIODATA register 
                                            // are transferred onto the GPOUT 
                                            // pins if the respective pins have 
                                            // been configured as outputs 
                                            // through the GPIODIR register. A 
                                            // read from GPIODATA returns the 
                                            // last bit value written if the 
                                            // respective pins are configured 
                                            // as output, or it returns the 
                                            // value on the corresponding input 
                                            // GPIN bit when these are 
                                            // configured as inputs. 
#define GPIO_O_DIR              0x00000400  // The DIR register is the data 
                                            // direction register. All bits are 
                                            // cleared by a reset; therefore, 
                                            // the GPIO pins are input by 
                                            // default. 
#define GPIO_O_IS               0x00000404  // The IS register is the 
                                            // interrupt sense register. 
#define GPIO_O_IBE              0x00000408  // The IBE register is the 
                                            // interrupt both-edges register. 
                                            // When the corresponding bit in IS 
                                            // is set to detect edges, bits set 
                                            // to high in IBE configure the 
                                            // corresponding pin to detect both 
                                            // rising and falling edges, 
                                            // regardless of the corresponding 
                                            // bit in the IEV (interrupt event 
                                            // register). Clearing a bit 
                                            // configures the pin to be 
                                            // controlled by IEV. 
#define GPIO_O_IEV              0x0000040C  // The IEV register is the 
                                            // interrupt event register. Bits 
                                            // set to high in IEV configure the 
                                            // corresponding pin to detect 
                                            // rising edges or high levels, 
                                            // depending on the corresponding 
                                            // bit value in IS. Clearing a bit 
                                            // configures the pin to detect 
                                            // falling edges or low levels, 
                                            // depending on the corresponding 
                                            // bit value in IS. 
#define GPIO_O_IE               0x00000410  // The IE register is the 
                                            // interrupt mask register. Bits 
                                            // set to high in IE allow the 
                                            // corresponding pins to trigger 
                                            // their individual interrupts and 
                                            // the combined GPIOINTR line. 
                                            // Clearing a bit disables 
                                            // interrupt triggering on that 
                                            // pin. 
#define GPIO_O_RIS              0x00000414  // The RIS register is the raw 
                                            // interrupt status register. Bits 
                                            // read high in RIS reflect the 
                                            // status of interrupts trigger 
                                            // conditions detected (raw, before 
                                            // masking), indicating that all 
                                            // the requirements are met, before 
                                            // they are finally allowed to 
                                            // trigger by IE. Bits read as 0 
                                            // indicate that corresponding 
                                            // input pins have not initiated an 
                                            // interrupt. 
#define GPIO_O_MIS              0x00000418  // The MIS register is the masked 
                                            // interrupt status register. Bits 
                                            // read high in MIS reflect the 
                                            // status of input lines triggering 
                                            // an interrupt. Bits read as low 
                                            // indicate that either no 
                                            // interrupt has been generated, or 
                                            // the interrupt is masked. MIS is 
                                            // the state of the interrupt after 
                                            // masking. 
#define GPIO_O_IC               0x0000041C  // The IC register is the 
                                            // interrupt clear register. 
                                            // Writing 1 to a bit in this 
                                            // register clears the 
                                            // corresponding interrupt edge 
                                            // detection logic register. 
                                            // Writing 0 has no effect. 
#define GPIO_O_AFSEL            0x00000420  // The AFSEL register is the mode 
                                            // control select register. Writing 
                                            // 1 to any bit in this register 
                                            // selects the hardware 
                                            // (peripheral) control for the 
                                            // corresponding GPIO line. All 
                                            // bits are cleared by a reset, 
                                            // therefore no GPIO line is set to 
                                            // hardware control by default. 
#define GPIO_O_GPIOLOCK         0x00000520  // A write of the value 0x4C4F434B 
                                            // to the GPIOLOCK register unlocks 
                                            // the GPIO commit register 
                                            // (GPIOCR) for write access. A 
                                            // write of any other value 
                                            // reapplies the lock, preventing 
                                            // any register updates. Any write 
                                            // to the commit register (GPIOCR) 
                                            // causes the lock register to be 
                                            // locked. 
#define GPIO_O_GPIOCR           0x00000524  // The GPIOCR register is the 
                                            // commit register. The value of 
                                            // the GPIOCR register determines 
                                            // which bits of the AFSEL register 
                                            // is committed when a write to the 
                                            // AFSEL register is performed. If 
                                            // a bit in the GPIOCR register is 
                                            // 0, the data being written to the 
                                            // corresponding bit in the AFSEL 
                                            // register is not committed and 
                                            // retains its previous value. If a 
                                            // bit in the GPIOCR register is 
                                            // set to 1, the data being written 
                                            // to the corresponding bit of the 
                                            // AFSEL register is committed to 
                                            // the register and will reflect 
                                            // the new value. The contents of 
                                            // the GPIOCR register can only be 
                                            // modified if the GPIOLOCK 
                                            // register is unlocked. Writes to 
                                            // the GPIOCR register will be 
                                            // ignored if the GPIOLOCK register 
                                            // is locked. Any write to the 
                                            // commit register causes the lock 
                                            // register to be locked. 
#define GPIO_O_PMUX             0x00000700  // The PMUX register can be used 
                                            // to output external decouple 
                                            // control and clock_32k on I/O 
                                            // pins. Decouple control can be 
                                            // output on specific PB pins and 
                                            // clock_32k can be output on a 
                                            // specific PA or PB pin. These 
                                            // features override the current 
                                            // setting of the selected pin when 
                                            // enabled. The pin is set to 
                                            // output, pull-up and -down 
                                            // disabled, and analog mode 
                                            // disabled. 
#define GPIO_O_P_EDGE_CTRL      0x00000704  // The port edge control register 
                                            // is used to control which edge of 
                                            // each port input causes that port 
                                            // to generate a power-up interrupt 
                                            // to the system. 
#define GPIO_O_USB_CTRL         0x00000708  // This register is used to 
                                            // control which edge of the USB 
                                            // controller input generates a 
                                            // power-up interrupt to the 
                                            // system. 
#define GPIO_O_PI_IEN           0x00000710  // The power-up interrupt enable 
                                            // register selects, for its 
                                            // corresponding port A-D pin, 
                                            // whether interrupts are enabled 
                                            // or disabled. 
#define GPIO_O_IRQ_DETECT_ACK   0x00000718  // If the IRQ detect ACK register 
                                            // is read, the value returned can 
                                            // be used to determine which 
                                            // enabled I/O port is responsible 
                                            // for creating a power-up 
                                            // interrupt to the system. Writing 
                                            // the IRQ detect ACK register is 
                                            // used to clear any number of 
                                            // individual port bits that may be 
                                            // signaling that an edge was 
                                            // detected as configured by the 
                                            // port edge control register and 
                                            // the interrupt control register. 
                                            // There is a self-clearing 
                                            // function to this register that 
                                            // generates a reset pulse to clear 
                                            // any interrupt which has its 
                                            // corresponding bit set to 1. 
#define GPIO_O_USB_IRQ_ACK      0x0000071C  // Same functionality as 
                                            // IRQ_DETECT_ACK, but for USB 
#define GPIO_O_IRQ_DETECT_UNMASK \
                                0x00000720  // Same functionality as 
                                            // IRQ_DETECT_ACK, but this 
                                            // register handles masked 
                                            // interrupts 



//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_DATA register.
//
//*****************************************************************************
#define GPIO_DATA_DATA_M        0x000000FF  // Input and output data 
#define GPIO_DATA_DATA_S        0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_DIR register.
//
//*****************************************************************************
#define GPIO_DIR_DIR_M          0x000000FF  // Bits set: Corresponding pin is 
                                            // output Bits cleared: 
                                            // Corresponding pin is input 
#define GPIO_DIR_DIR_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_IS register.
//
//*****************************************************************************
#define GPIO_IS_IS_M            0x000000FF  // Bits set: Level on 
                                            // corresponding pin is detected 
                                            // Bits cleared: Edge on 
                                            // corresponding pin is detected 
#define GPIO_IS_IS_S            0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_IBE register.
//
//*****************************************************************************
#define GPIO_IBE_IBE_M          0x000000FF  // Bits set: Both edges on 
                                            // corresponding pin trigger an 
                                            // interrupt Bits cleared: 
                                            // Interrupt generation event is 
                                            // controlled by GPIOIEV Single 
                                            // edge: Determined by 
                                            // corresponding bit in GPIOIEV 
                                            // register 
#define GPIO_IBE_IBE_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_IEV register.
//
//*****************************************************************************
#define GPIO_IEV_IEV_M          0x000000FF  // Bits set: Rising edges or high 
                                            // levels on corresponding pin 
                                            // trigger interrupts Bits cleared: 
                                            // Falling edges or low levels on 
                                            // corresponding pin trigger 
                                            // interrupts 
#define GPIO_IEV_IEV_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_IE register.
//
//*****************************************************************************
#define GPIO_IE_IE_M            0x000000FF  // Bits set: Corresponding pin is 
                                            // not masked Bits cleared: 
                                            // Corresponding pin is masked 
#define GPIO_IE_IE_S            0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_RIS register.
//
//*****************************************************************************
#define GPIO_RIS_RIS_M          0x000000FF  // Reflects the status of 
                                            // interrupts trigger conditions 
                                            // detected on pins (raw, before 
                                            // masking) Bits set: Requirements 
                                            // met by corresponding pins Bits 
                                            // clear: Requirements not met 
#define GPIO_RIS_RIS_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_MIS register.
//
//*****************************************************************************
#define GPIO_MIS_MIS_M          0x000000FF  // Masked value of interrupt due 
                                            // to corresponding pin Bits clear: 
                                            // GPIO line interrupt not active 
                                            // Bits set: GPIO line asserting 
                                            // interrupt 
#define GPIO_MIS_MIS_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_IC register.
//
//*****************************************************************************
#define GPIO_IC_IC_M            0x000000FF  // Bit written as 1: Clears edge 
                                            // detection logic Bit written as 
                                            // 0: Has no effect 
#define GPIO_IC_IC_S            0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_AFSEL register.
//
//*****************************************************************************
#define GPIO_AFSEL_AFSEL_M      0x000000FF  // Bit set: Enables hardware 
                                            // (peripheral) control mode Bit 
                                            // cleared: Enables software 
                                            // control mode 
#define GPIO_AFSEL_AFSEL_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPIO_O_GPIOLOCK register.
//
//*****************************************************************************
#define GPIO_GPIOLOCK_LOCK_M    0xFFFFFFFF  // A read of this register returns 
                                            // the following values: Locked: 
                                            // 0x00000001 Unlocked: 0x00000000 
#define GPIO_GPIOLOCK_LOCK_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_GPIOCR register.
//
//*****************************************************************************
#define GPIO_GPIOCR_CR_M        0x000000FF  // On a bit-wise basis, any bit 
                                            // set allows the corresponding 
                                            // GPIOAFSEL bit to be set to its 
                                            // alternate function. 
#define GPIO_GPIOCR_CR_S        0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_PMUX register.
//
//*****************************************************************************
#define GPIO_PMUX_CKOEN         0x00000080  // Clock out enable When this bit 
                                            // is set, the 32-kHz clock is 
                                            // routed to either PA[0] or PB[7] 
                                            // pins. PMUX.CKOPIN selects the 
                                            // pin to use. This overrides the 
                                            // current configuration setting 
                                            // for this pin. The pullup or 
                                            // pulldown is disabled and the 
                                            // direction is set to output for 
                                            // this pin. 
#define GPIO_PMUX_CKOEN_M       0x00000080
#define GPIO_PMUX_CKOEN_S       7
#define GPIO_PMUX_CKOPIN        0x00000010  // Decouple control pin select 
                                            // This control only has relevance 
                                            // when CKOEN is set. When 0, PA[0] 
                                            // becomes the 32-kHz clock output. 
                                            // When 1, PB[7] becomes the 32-kHz 
                                            // clock output. 
#define GPIO_PMUX_CKOPIN_M      0x00000010
#define GPIO_PMUX_CKOPIN_S      4
#define GPIO_PMUX_DCEN          0x00000008  // Decouple control enable When 
                                            // this bit is set, the on-die 
                                            // digital regulator status is 
                                            // routed to either PB[1] or PB[0] 
                                            // pins. PMUX.DCPIN selects the pin 
                                            // to use. This overrides the 
                                            // current configuration setting 
                                            // for this pin. The pullup or 
                                            // pulldown is disabled and the 
                                            // direction is set to output for 
                                            // this pin. 
#define GPIO_PMUX_DCEN_M        0x00000008
#define GPIO_PMUX_DCEN_S        3
#define GPIO_PMUX_DCPIN         0x00000001  // Decouple control pin select 
                                            // This control has relevance only 
                                            // when DCEN is set. When 0, PB[1] 
                                            // becomes the on-die digital 
                                            // regulator status (1 indicates 
                                            // the on-die digital regulator is 
                                            // active); when 1, PB[0] becomes 
                                            // the on-die digital regulator 
                                            // status. NOTE: PB[1] and PB[0] 
                                            // can also be controlled with 
                                            // other override features. In 
                                            // priority order for PB[1]: When 
                                            // POR/BOD test mode is active, 
                                            // PB[1] becomes the active low 
                                            // brown-out detected indicator. 
                                            // When DCEN is set and DCPIN is 
                                            // not set, PB[1] becomes the 
                                            // on-dir digital regulator status. 
                                            // In priority order for PB[0]: 
                                            // When POR/BOD test mode is 
                                            // active, PB[0] becomes the 
                                            // power-on-reset indicator. When 
                                            // DCEN and DCPIN are set, PB[0] 
                                            // becomes the on-die digital 
                                            // regulator status. 
#define GPIO_PMUX_DCPIN_M       0x00000001
#define GPIO_PMUX_DCPIN_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPIO_O_P_EDGE_CTRL register.
//
//*****************************************************************************
#define GPIO_P_EDGE_CTRL_PDIRC7 0x80000000  // Port D bit 7 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PDIRC7_M \
                                0x80000000
#define GPIO_P_EDGE_CTRL_PDIRC7_S 31
#define GPIO_P_EDGE_CTRL_PDIRC6 0x40000000  // Port D bit 6 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PDIRC6_M \
                                0x40000000
#define GPIO_P_EDGE_CTRL_PDIRC6_S 30
#define GPIO_P_EDGE_CTRL_PDIRC5 0x20000000  // Port D bit 5 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PDIRC5_M \
                                0x20000000
#define GPIO_P_EDGE_CTRL_PDIRC5_S 29
#define GPIO_P_EDGE_CTRL_PDIRC4 0x10000000  // Port D bit 4 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PDIRC4_M \
                                0x10000000
#define GPIO_P_EDGE_CTRL_PDIRC4_S 28
#define GPIO_P_EDGE_CTRL_PDIRC3 0x08000000  // Port D bit 3 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PDIRC3_M \
                                0x08000000
#define GPIO_P_EDGE_CTRL_PDIRC3_S 27
#define GPIO_P_EDGE_CTRL_PDIRC2 0x04000000  // Port D bit 2 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PDIRC2_M \
                                0x04000000
#define GPIO_P_EDGE_CTRL_PDIRC2_S 26
#define GPIO_P_EDGE_CTRL_PDIRC1 0x02000000  // Port D bit 1 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PDIRC1_M \
                                0x02000000
#define GPIO_P_EDGE_CTRL_PDIRC1_S 25
#define GPIO_P_EDGE_CTRL_PDIRC0 0x01000000  // Port D bit 0 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PDIRC0_M \
                                0x01000000
#define GPIO_P_EDGE_CTRL_PDIRC0_S 24
#define GPIO_P_EDGE_CTRL_PCIRC7 0x00800000  // Port C bit 7 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PCIRC7_M \
                                0x00800000
#define GPIO_P_EDGE_CTRL_PCIRC7_S 23
#define GPIO_P_EDGE_CTRL_PCIRC6 0x00400000  // Port C bit 6 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PCIRC6_M \
                                0x00400000
#define GPIO_P_EDGE_CTRL_PCIRC6_S 22
#define GPIO_P_EDGE_CTRL_PCIRC5 0x00200000  // Port C bit 5 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PCIRC5_M \
                                0x00200000
#define GPIO_P_EDGE_CTRL_PCIRC5_S 21
#define GPIO_P_EDGE_CTRL_PCIRC4 0x00100000  // Port C bit 4 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PCIRC4_M \
                                0x00100000
#define GPIO_P_EDGE_CTRL_PCIRC4_S 20
#define GPIO_P_EDGE_CTRL_PCIRC3 0x00080000  // Port C bit 3 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PCIRC3_M \
                                0x00080000
#define GPIO_P_EDGE_CTRL_PCIRC3_S 19
#define GPIO_P_EDGE_CTRL_PCIRC2 0x00040000  // Port C bit 2 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PCIRC2_M \
                                0x00040000
#define GPIO_P_EDGE_CTRL_PCIRC2_S 18
#define GPIO_P_EDGE_CTRL_PCIRC1 0x00020000  // Port C bit 1 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PCIRC1_M \
                                0x00020000
#define GPIO_P_EDGE_CTRL_PCIRC1_S 17
#define GPIO_P_EDGE_CTRL_PCIRC0 0x00010000  // Port C bit 0 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PCIRC0_M \
                                0x00010000
#define GPIO_P_EDGE_CTRL_PCIRC0_S 16
#define GPIO_P_EDGE_CTRL_PBIRC7 0x00008000  // Port B bit 7 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PBIRC7_M \
                                0x00008000
#define GPIO_P_EDGE_CTRL_PBIRC7_S 15
#define GPIO_P_EDGE_CTRL_PBIRC6 0x00004000  // Port B bit 6 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PBIRC6_M \
                                0x00004000
#define GPIO_P_EDGE_CTRL_PBIRC6_S 14
#define GPIO_P_EDGE_CTRL_PBIRC5 0x00002000  // Port B bit 5 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PBIRC5_M \
                                0x00002000
#define GPIO_P_EDGE_CTRL_PBIRC5_S 13
#define GPIO_P_EDGE_CTRL_PBIRC4 0x00001000  // Port B bit 4 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PBIRC4_M \
                                0x00001000
#define GPIO_P_EDGE_CTRL_PBIRC4_S 12
#define GPIO_P_EDGE_CTRL_PBIRC3 0x00000800  // Port B bit 3 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PBIRC3_M \
                                0x00000800
#define GPIO_P_EDGE_CTRL_PBIRC3_S 11
#define GPIO_P_EDGE_CTRL_PBIRC2 0x00000400  // Port B bit 2 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PBIRC2_M \
                                0x00000400
#define GPIO_P_EDGE_CTRL_PBIRC2_S 10
#define GPIO_P_EDGE_CTRL_PBIRC1 0x00000200  // Port B bit 1 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PBIRC1_M \
                                0x00000200
#define GPIO_P_EDGE_CTRL_PBIRC1_S 9
#define GPIO_P_EDGE_CTRL_PBIRC0 0x00000100  // Port B bit 0 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PBIRC0_M \
                                0x00000100
#define GPIO_P_EDGE_CTRL_PBIRC0_S 8
#define GPIO_P_EDGE_CTRL_PAIRC7 0x00000080  // Port A bit 7 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PAIRC7_M \
                                0x00000080
#define GPIO_P_EDGE_CTRL_PAIRC7_S 7
#define GPIO_P_EDGE_CTRL_PAIRC6 0x00000040  // Port A bit 6 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PAIRC6_M \
                                0x00000040
#define GPIO_P_EDGE_CTRL_PAIRC6_S 6
#define GPIO_P_EDGE_CTRL_PAIRC5 0x00000020  // Port A bit 5 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PAIRC5_M \
                                0x00000020
#define GPIO_P_EDGE_CTRL_PAIRC5_S 5
#define GPIO_P_EDGE_CTRL_PAIRC4 0x00000010  // Port A bit 4 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PAIRC4_M \
                                0x00000010
#define GPIO_P_EDGE_CTRL_PAIRC4_S 4
#define GPIO_P_EDGE_CTRL_PAIRC3 0x00000008  // Port A bit 3 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PAIRC3_M \
                                0x00000008
#define GPIO_P_EDGE_CTRL_PAIRC3_S 3
#define GPIO_P_EDGE_CTRL_PAIRC2 0x00000004  // Port A bit 2 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PAIRC2_M \
                                0x00000004
#define GPIO_P_EDGE_CTRL_PAIRC2_S 2
#define GPIO_P_EDGE_CTRL_PAIRC1 0x00000002  // Port A bit 1 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PAIRC1_M \
                                0x00000002
#define GPIO_P_EDGE_CTRL_PAIRC1_S 1
#define GPIO_P_EDGE_CTRL_PAIRC0 0x00000001  // Port A bit 0 interrupt request 
                                            // condition: 0: Rising 1: Falling 
                                            // edge 
#define GPIO_P_EDGE_CTRL_PAIRC0_M \
                                0x00000001
#define GPIO_P_EDGE_CTRL_PAIRC0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPIO_O_USB_CTRL register.
//
//*****************************************************************************
#define GPIO_USB_CTRL_USB_EDGE_CTL \
                                0x00000001  // Used to set the edge which 
                                            // triggers the USB power up 
                                            // interrupt 0: Rising 1: Falling 

#define GPIO_USB_CTRL_USB_EDGE_CTL_M \
                                0x00000001
#define GPIO_USB_CTRL_USB_EDGE_CTL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPIO_O_PI_IEN register.
//
//*****************************************************************************
#define GPIO_PI_IEN_PDIEN7      0x80000000  // Port D bit 7 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PDIEN7_M    0x80000000
#define GPIO_PI_IEN_PDIEN7_S    31
#define GPIO_PI_IEN_PDIEN6      0x40000000  // Port D bit 6 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PDIEN6_M    0x40000000
#define GPIO_PI_IEN_PDIEN6_S    30
#define GPIO_PI_IEN_PDIEN5      0x20000000  // Port D bit 5 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PDIEN5_M    0x20000000
#define GPIO_PI_IEN_PDIEN5_S    29
#define GPIO_PI_IEN_PDIEN4      0x10000000  // Port D bit 4 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PDIEN4_M    0x10000000
#define GPIO_PI_IEN_PDIEN4_S    28
#define GPIO_PI_IEN_PDIEN3      0x08000000  // Port D bit 3 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PDIEN3_M    0x08000000
#define GPIO_PI_IEN_PDIEN3_S    27
#define GPIO_PI_IEN_PDIEN2      0x04000000  // Port D bit 2 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PDIEN2_M    0x04000000
#define GPIO_PI_IEN_PDIEN2_S    26
#define GPIO_PI_IEN_PDIEN1      0x02000000  // Port D bit 1 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PDIEN1_M    0x02000000
#define GPIO_PI_IEN_PDIEN1_S    25
#define GPIO_PI_IEN_PDIEN0      0x01000000  // Port D bit 0 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PDIEN0_M    0x01000000
#define GPIO_PI_IEN_PDIEN0_S    24
#define GPIO_PI_IEN_PCIEN7      0x00800000  // Port C bit 7 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PCIEN7_M    0x00800000
#define GPIO_PI_IEN_PCIEN7_S    23
#define GPIO_PI_IEN_PCIEN6      0x00400000  // Port C bit 6 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PCIEN6_M    0x00400000
#define GPIO_PI_IEN_PCIEN6_S    22
#define GPIO_PI_IEN_PCIEN5      0x00200000  // Port C bit 5 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PCIEN5_M    0x00200000
#define GPIO_PI_IEN_PCIEN5_S    21
#define GPIO_PI_IEN_PCIEN4      0x00100000  // Port C bit 4 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PCIEN4_M    0x00100000
#define GPIO_PI_IEN_PCIEN4_S    20
#define GPIO_PI_IEN_PCIEN3      0x00080000  // Port C bit 3 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PCIEN3_M    0x00080000
#define GPIO_PI_IEN_PCIEN3_S    19
#define GPIO_PI_IEN_PCIEN2      0x00040000  // Port C bit 2 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PCIEN2_M    0x00040000
#define GPIO_PI_IEN_PCIEN2_S    18
#define GPIO_PI_IEN_PCIEN1      0x00020000  // Port C bit 1 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PCIEN1_M    0x00020000
#define GPIO_PI_IEN_PCIEN1_S    17
#define GPIO_PI_IEN_PCIEN0      0x00010000  // Port C bit 0 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PCIEN0_M    0x00010000
#define GPIO_PI_IEN_PCIEN0_S    16
#define GPIO_PI_IEN_PBIEN7      0x00008000  // Port B bit 7 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PBIEN7_M    0x00008000
#define GPIO_PI_IEN_PBIEN7_S    15
#define GPIO_PI_IEN_PBIEN6      0x00004000  // Port B bit 6 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PBIEN6_M    0x00004000
#define GPIO_PI_IEN_PBIEN6_S    14
#define GPIO_PI_IEN_PBIEN5      0x00002000  // Port B bit 5 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PBIEN5_M    0x00002000
#define GPIO_PI_IEN_PBIEN5_S    13
#define GPIO_PI_IEN_PBIEN4      0x00001000  // Port B bit 4 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PBIEN4_M    0x00001000
#define GPIO_PI_IEN_PBIEN4_S    12
#define GPIO_PI_IEN_PBIEN3      0x00000800  // Port B bit 3 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PBIEN3_M    0x00000800
#define GPIO_PI_IEN_PBIEN3_S    11
#define GPIO_PI_IEN_PBIEN2      0x00000400  // Port B bit 2 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PBIEN2_M    0x00000400
#define GPIO_PI_IEN_PBIEN2_S    10
#define GPIO_PI_IEN_PBIEN1      0x00000200  // Port B bit 1 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PBIEN1_M    0x00000200
#define GPIO_PI_IEN_PBIEN1_S    9
#define GPIO_PI_IEN_PBIEN0      0x00000100  // Port B bit 0 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PBIEN0_M    0x00000100
#define GPIO_PI_IEN_PBIEN0_S    8
#define GPIO_PI_IEN_PAIEN7      0x00000080  // Port A bit 7 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PAIEN7_M    0x00000080
#define GPIO_PI_IEN_PAIEN7_S    7
#define GPIO_PI_IEN_PAIEN6      0x00000040  // Port A bit 6 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PAIEN6_M    0x00000040
#define GPIO_PI_IEN_PAIEN6_S    6
#define GPIO_PI_IEN_PAIEN5      0x00000020  // Port A bit 5 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PAIEN5_M    0x00000020
#define GPIO_PI_IEN_PAIEN5_S    5
#define GPIO_PI_IEN_PAIEN4      0x00000010  // Port A bit 4 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PAIEN4_M    0x00000010
#define GPIO_PI_IEN_PAIEN4_S    4
#define GPIO_PI_IEN_PAIEN3      0x00000008  // Port A bit 3 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PAIEN3_M    0x00000008
#define GPIO_PI_IEN_PAIEN3_S    3
#define GPIO_PI_IEN_PAIEN2      0x00000004  // Port A bit 2 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PAIEN2_M    0x00000004
#define GPIO_PI_IEN_PAIEN2_S    2
#define GPIO_PI_IEN_PAIEN1      0x00000002  // Port A bit 1 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PAIEN1_M    0x00000002
#define GPIO_PI_IEN_PAIEN1_S    1
#define GPIO_PI_IEN_PAIEN0      0x00000001  // Port A bit 0 interrupt enable: 
                                            // 1: Enabled 2: Disabled 
#define GPIO_PI_IEN_PAIEN0_M    0x00000001
#define GPIO_PI_IEN_PAIEN0_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPIO_O_IRQ_DETECT_ACK register.
//
//*****************************************************************************
#define GPIO_IRQ_DETECT_ACK_PDIACK7 \
                                0x80000000  // Port D bit 7 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PDIACK7_M \
                                0x80000000
#define GPIO_IRQ_DETECT_ACK_PDIACK7_S 31
#define GPIO_IRQ_DETECT_ACK_PDIACK6 \
                                0x40000000  // Port D bit 6 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PDIACK6_M \
                                0x40000000
#define GPIO_IRQ_DETECT_ACK_PDIACK6_S 30
#define GPIO_IRQ_DETECT_ACK_PDIACK5 \
                                0x20000000  // Port D bit 5 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PDIACK5_M \
                                0x20000000
#define GPIO_IRQ_DETECT_ACK_PDIACK5_S 29
#define GPIO_IRQ_DETECT_ACK_PDIACK4 \
                                0x10000000  // Port D bit 4 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PDIACK4_M \
                                0x10000000
#define GPIO_IRQ_DETECT_ACK_PDIACK4_S 28
#define GPIO_IRQ_DETECT_ACK_PDIACK3 \
                                0x08000000  // Port D bit 3 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PDIACK3_M \
                                0x08000000
#define GPIO_IRQ_DETECT_ACK_PDIACK3_S 27
#define GPIO_IRQ_DETECT_ACK_PDIACK2 \
                                0x04000000  // Port D bit 2 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PDIACK2_M \
                                0x04000000
#define GPIO_IRQ_DETECT_ACK_PDIACK2_S 26
#define GPIO_IRQ_DETECT_ACK_PDIACK1 \
                                0x02000000  // Port D bit 1 masked interrupt 
                                            // status: 1: Detected0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PDIACK1_M \
                                0x02000000
#define GPIO_IRQ_DETECT_ACK_PDIACK1_S 25
#define GPIO_IRQ_DETECT_ACK_PDIACK0 \
                                0x01000000  // Port D bit 0 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PDIACK0_M \
                                0x01000000
#define GPIO_IRQ_DETECT_ACK_PDIACK0_S 24
#define GPIO_IRQ_DETECT_ACK_PCIACK7 \
                                0x00800000  // Port C bit 7 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PCIACK7_M \
                                0x00800000
#define GPIO_IRQ_DETECT_ACK_PCIACK7_S 23
#define GPIO_IRQ_DETECT_ACK_PCIACK6 \
                                0x00400000  // Port C bit 6 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PCIACK6_M \
                                0x00400000
#define GPIO_IRQ_DETECT_ACK_PCIACK6_S 22
#define GPIO_IRQ_DETECT_ACK_PCIACK5 \
                                0x00200000  // Port C bit 5 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PCIACK5_M \
                                0x00200000
#define GPIO_IRQ_DETECT_ACK_PCIACK5_S 21
#define GPIO_IRQ_DETECT_ACK_PCIACK4 \
                                0x00100000  // Port C bit 4 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PCIACK4_M \
                                0x00100000
#define GPIO_IRQ_DETECT_ACK_PCIACK4_S 20
#define GPIO_IRQ_DETECT_ACK_PCIACK3 \
                                0x00080000  // Port C bit 3 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PCIACK3_M \
                                0x00080000
#define GPIO_IRQ_DETECT_ACK_PCIACK3_S 19
#define GPIO_IRQ_DETECT_ACK_PCIACK2 \
                                0x00040000  // Port C bit 2 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PCIACK2_M \
                                0x00040000
#define GPIO_IRQ_DETECT_ACK_PCIACK2_S 18
#define GPIO_IRQ_DETECT_ACK_PCIACK1 \
                                0x00020000  // Port C bit 1 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PCIACK1_M \
                                0x00020000
#define GPIO_IRQ_DETECT_ACK_PCIACK1_S 17
#define GPIO_IRQ_DETECT_ACK_PCIACK0 \
                                0x00010000  // Port C bit 0 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PCIACK0_M \
                                0x00010000
#define GPIO_IRQ_DETECT_ACK_PCIACK0_S 16
#define GPIO_IRQ_DETECT_ACK_PBIACK7 \
                                0x00008000  // Port B bit 7 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PBIACK7_M \
                                0x00008000
#define GPIO_IRQ_DETECT_ACK_PBIACK7_S 15
#define GPIO_IRQ_DETECT_ACK_PBIACK6 \
                                0x00004000  // Port B bit 6 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PBIACK6_M \
                                0x00004000
#define GPIO_IRQ_DETECT_ACK_PBIACK6_S 14
#define GPIO_IRQ_DETECT_ACK_PBIACK5 \
                                0x00002000  // Port B bit 5 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PBIACK5_M \
                                0x00002000
#define GPIO_IRQ_DETECT_ACK_PBIACK5_S 13
#define GPIO_IRQ_DETECT_ACK_PBIACK4 \
                                0x00001000  // Port B bit 4 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PBIACK4_M \
                                0x00001000
#define GPIO_IRQ_DETECT_ACK_PBIACK4_S 12
#define GPIO_IRQ_DETECT_ACK_PBIACK3 \
                                0x00000800  // Port B bit 3 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PBIACK3_M \
                                0x00000800
#define GPIO_IRQ_DETECT_ACK_PBIACK3_S 11
#define GPIO_IRQ_DETECT_ACK_PBIACK2 \
                                0x00000400  // Port B bit 2 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PBIACK2_M \
                                0x00000400
#define GPIO_IRQ_DETECT_ACK_PBIACK2_S 10
#define GPIO_IRQ_DETECT_ACK_PBIACK1 \
                                0x00000200  // Port B bit 1 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PBIACK1_M \
                                0x00000200
#define GPIO_IRQ_DETECT_ACK_PBIACK1_S 9
#define GPIO_IRQ_DETECT_ACK_PBIACK0 \
                                0x00000100  // Port B bit 0 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PBIACK0_M \
                                0x00000100
#define GPIO_IRQ_DETECT_ACK_PBIACK0_S 8
#define GPIO_IRQ_DETECT_ACK_PAIACK7 \
                                0x00000080  // Port A bit 7 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PAIACK7_M \
                                0x00000080
#define GPIO_IRQ_DETECT_ACK_PAIACK7_S 7
#define GPIO_IRQ_DETECT_ACK_PAIACK6 \
                                0x00000040  // Port A bit 6 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PAIACK6_M \
                                0x00000040
#define GPIO_IRQ_DETECT_ACK_PAIACK6_S 6
#define GPIO_IRQ_DETECT_ACK_PAIACK5 \
                                0x00000020  // Port A bit 5 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PAIACK5_M \
                                0x00000020
#define GPIO_IRQ_DETECT_ACK_PAIACK5_S 5
#define GPIO_IRQ_DETECT_ACK_PAIACK4 \
                                0x00000010  // Port A bit 4 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PAIACK4_M \
                                0x00000010
#define GPIO_IRQ_DETECT_ACK_PAIACK4_S 4
#define GPIO_IRQ_DETECT_ACK_PAIACK3 \
                                0x00000008  // Port A bit 3 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PAIACK3_M \
                                0x00000008
#define GPIO_IRQ_DETECT_ACK_PAIACK3_S 3
#define GPIO_IRQ_DETECT_ACK_PAIACK2 \
                                0x00000004  // Port A bit 2 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PAIACK2_M \
                                0x00000004
#define GPIO_IRQ_DETECT_ACK_PAIACK2_S 2
#define GPIO_IRQ_DETECT_ACK_PAIACK1 \
                                0x00000002  // Port A bit 1 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PAIACK1_M \
                                0x00000002
#define GPIO_IRQ_DETECT_ACK_PAIACK1_S 1
#define GPIO_IRQ_DETECT_ACK_PAIACK0 \
                                0x00000001  // Port A bit 0 masked interrupt 
                                            // status: 1: Detected 0: Not 
                                            // detected 

#define GPIO_IRQ_DETECT_ACK_PAIACK0_M \
                                0x00000001
#define GPIO_IRQ_DETECT_ACK_PAIACK0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPIO_O_USB_IRQ_ACK register.
//
//*****************************************************************************
#define GPIO_USB_IRQ_ACK_USBACK 0x00000001  // USB masked interrupt status: 1: 
                                            // Detected 0: Not detected 
#define GPIO_USB_IRQ_ACK_USBACK_M \
                                0x00000001
#define GPIO_USB_IRQ_ACK_USBACK_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPIO_O_IRQ_DETECT_UNMASK register.
//
//*****************************************************************************
#define GPIO_IRQ_DETECT_UNMASK_PDIACK7 \
                                0x80000000  // Port D bit 7 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PDIACK7_M \
                                0x80000000
#define GPIO_IRQ_DETECT_UNMASK_PDIACK7_S 31
#define GPIO_IRQ_DETECT_UNMASK_PDIACK6 \
                                0x40000000  // Port D bit 6 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PDIACK6_M \
                                0x40000000
#define GPIO_IRQ_DETECT_UNMASK_PDIACK6_S 30
#define GPIO_IRQ_DETECT_UNMASK_PDIACK5 \
                                0x20000000  // Port D bit 5 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PDIACK5_M \
                                0x20000000
#define GPIO_IRQ_DETECT_UNMASK_PDIACK5_S 29
#define GPIO_IRQ_DETECT_UNMASK_PDIACK4 \
                                0x10000000  // Port D bit 4 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PDIACK4_M \
                                0x10000000
#define GPIO_IRQ_DETECT_UNMASK_PDIACK4_S 28
#define GPIO_IRQ_DETECT_UNMASK_PDIACK3 \
                                0x08000000  // Port D bit 3 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PDIACK3_M \
                                0x08000000
#define GPIO_IRQ_DETECT_UNMASK_PDIACK3_S 27
#define GPIO_IRQ_DETECT_UNMASK_PDIACK2 \
                                0x04000000  // Port D bit 2 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PDIACK2_M \
                                0x04000000
#define GPIO_IRQ_DETECT_UNMASK_PDIACK2_S 26
#define GPIO_IRQ_DETECT_UNMASK_PDIACK1 \
                                0x02000000  // Port D bit 1 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PDIACK1_M \
                                0x02000000
#define GPIO_IRQ_DETECT_UNMASK_PDIACK1_S 25
#define GPIO_IRQ_DETECT_UNMASK_PDIACK0 \
                                0x01000000  // Port D bit 0 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PDIACK0_M \
                                0x01000000
#define GPIO_IRQ_DETECT_UNMASK_PDIACK0_S 24
#define GPIO_IRQ_DETECT_UNMASK_PCIACK7 \
                                0x00800000  // Port C bit 7 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PCIACK7_M \
                                0x00800000
#define GPIO_IRQ_DETECT_UNMASK_PCIACK7_S 23
#define GPIO_IRQ_DETECT_UNMASK_PCIACK6 \
                                0x00400000  // Port C bit 6 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PCIACK6_M \
                                0x00400000
#define GPIO_IRQ_DETECT_UNMASK_PCIACK6_S 22
#define GPIO_IRQ_DETECT_UNMASK_PCIACK5 \
                                0x00200000  // Port C bit 5 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PCIACK5_M \
                                0x00200000
#define GPIO_IRQ_DETECT_UNMASK_PCIACK5_S 21
#define GPIO_IRQ_DETECT_UNMASK_PCIACK4 \
                                0x00100000  // Port C bit 4 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PCIACK4_M \
                                0x00100000
#define GPIO_IRQ_DETECT_UNMASK_PCIACK4_S 20
#define GPIO_IRQ_DETECT_UNMASK_PCIACK3 \
                                0x00080000  // Port C bit 3 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PCIACK3_M \
                                0x00080000
#define GPIO_IRQ_DETECT_UNMASK_PCIACK3_S 19
#define GPIO_IRQ_DETECT_UNMASK_PCIACK2 \
                                0x00040000  // Port C bit 2 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PCIACK2_M \
                                0x00040000
#define GPIO_IRQ_DETECT_UNMASK_PCIACK2_S 18
#define GPIO_IRQ_DETECT_UNMASK_PCIACK1 \
                                0x00020000  // Port C bit 1 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PCIACK1_M \
                                0x00020000
#define GPIO_IRQ_DETECT_UNMASK_PCIACK1_S 17
#define GPIO_IRQ_DETECT_UNMASK_PCIACK0 \
                                0x00010000  // Port C bit 0 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PCIACK0_M \
                                0x00010000
#define GPIO_IRQ_DETECT_UNMASK_PCIACK0_S 16
#define GPIO_IRQ_DETECT_UNMASK_PBIACK7 \
                                0x00008000  // Port B bit 7 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PBIACK7_M \
                                0x00008000
#define GPIO_IRQ_DETECT_UNMASK_PBIACK7_S 15
#define GPIO_IRQ_DETECT_UNMASK_PBIACK6 \
                                0x00004000  // Port B bit 6 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PBIACK6_M \
                                0x00004000
#define GPIO_IRQ_DETECT_UNMASK_PBIACK6_S 14
#define GPIO_IRQ_DETECT_UNMASK_PBIACK5 \
                                0x00002000  // Port B bit 5 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PBIACK5_M \
                                0x00002000
#define GPIO_IRQ_DETECT_UNMASK_PBIACK5_S 13
#define GPIO_IRQ_DETECT_UNMASK_PBIACK4 \
                                0x00001000  // Port B bit 4 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PBIACK4_M \
                                0x00001000
#define GPIO_IRQ_DETECT_UNMASK_PBIACK4_S 12
#define GPIO_IRQ_DETECT_UNMASK_PBIACK3 \
                                0x00000800  // Port B bit 3 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PBIACK3_M \
                                0x00000800
#define GPIO_IRQ_DETECT_UNMASK_PBIACK3_S 11
#define GPIO_IRQ_DETECT_UNMASK_PBIACK2 \
                                0x00000400  // Port B bit 2 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PBIACK2_M \
                                0x00000400
#define GPIO_IRQ_DETECT_UNMASK_PBIACK2_S 10
#define GPIO_IRQ_DETECT_UNMASK_PBIACK1 \
                                0x00000200  // Port B bit 1 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PBIACK1_M \
                                0x00000200
#define GPIO_IRQ_DETECT_UNMASK_PBIACK1_S 9
#define GPIO_IRQ_DETECT_UNMASK_PBIACK0 \
                                0x00000100  // Port B bit 0 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PBIACK0_M \
                                0x00000100
#define GPIO_IRQ_DETECT_UNMASK_PBIACK0_S 8
#define GPIO_IRQ_DETECT_UNMASK_PAIACK7 \
                                0x00000080  // Port A bit 7 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PAIACK7_M \
                                0x00000080
#define GPIO_IRQ_DETECT_UNMASK_PAIACK7_S 7
#define GPIO_IRQ_DETECT_UNMASK_PAIACK6 \
                                0x00000040  // Port A bit 6 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PAIACK6_M \
                                0x00000040
#define GPIO_IRQ_DETECT_UNMASK_PAIACK6_S 6
#define GPIO_IRQ_DETECT_UNMASK_PAIACK5 \
                                0x00000020  // Port A bit 5 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PAIACK5_M \
                                0x00000020
#define GPIO_IRQ_DETECT_UNMASK_PAIACK5_S 5
#define GPIO_IRQ_DETECT_UNMASK_PAIACK4 \
                                0x00000010  // Port A bit 4 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PAIACK4_M \
                                0x00000010
#define GPIO_IRQ_DETECT_UNMASK_PAIACK4_S 4
#define GPIO_IRQ_DETECT_UNMASK_PAIACK3 \
                                0x00000008  // Port A bit 3 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PAIACK3_M \
                                0x00000008
#define GPIO_IRQ_DETECT_UNMASK_PAIACK3_S 3
#define GPIO_IRQ_DETECT_UNMASK_PAIACK2 \
                                0x00000004  // Port A bit 2 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PAIACK2_M \
                                0x00000004
#define GPIO_IRQ_DETECT_UNMASK_PAIACK2_S 2
#define GPIO_IRQ_DETECT_UNMASK_PAIACK1 \
                                0x00000002  // Port A bit 1 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PAIACK1_M \
                                0x00000002
#define GPIO_IRQ_DETECT_UNMASK_PAIACK1_S 1
#define GPIO_IRQ_DETECT_UNMASK_PAIACK0 \
                                0x00000001  // Port A bit 0 unmasked interrupt 
                                            // status: 1: Detected 0: 
                                            // Undetected 

#define GPIO_IRQ_DETECT_UNMASK_PAIACK0_M \
                                0x00000001
#define GPIO_IRQ_DETECT_UNMASK_PAIACK0_S 0


#endif // __HW_GPIO_H__

