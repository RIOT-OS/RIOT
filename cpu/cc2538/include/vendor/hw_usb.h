/******************************************************************************
*  Filename:       hw_usb.h
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

#ifndef __HW_USB_H__
#define __HW_USB_H__

//*****************************************************************************
//
// The following are defines for the USB register offsets.
//
//*****************************************************************************
#define USB_ADDR                0x40089000  // Function address 
#define USB_POW                 0x40089004  // Power management and control 
                                            // register 
#define USB_IIF                 0x40089008  // Interrupt flags for endpoint 0 
                                            // and IN endpoints 1-5 
#define USB_OIF                 0x40089010  // Interrupt flags for OUT 
                                            // endpoints 1-5 
#define USB_CIF                 0x40089018  // Common USB interrupt flags 
#define USB_IIE                 0x4008901C  // Interrupt enable mask for IN 
                                            // endpoints 1-5 and endpoint 0 
#define USB_OIE                 0x40089024  // Interrupt enable mask for OUT 
                                            // endpoints 1-5 
#define USB_CIE                 0x4008902C  // Common USB interrupt enable 
                                            // mask 
#define USB_FRML                0x40089030  // Frame number (low byte) 
#define USB_FRMH                0x40089034  // Frame number (high byte) 
#define USB_INDEX               0x40089038  // Index register for selecting 
                                            // the endpoint status and control 
                                            // registers 
#define USB_CTRL                0x4008903C  // USB peripheral control register 
#define USB_MAXI                0x40089040  // Indexed register: For USB_INDEX 
                                            // = 1-5: Maximum packet size for 
                                            // IN endpoint {1-5} 
#define USB_CS0_CSIL            0x40089044  // Indexed register: For USB_INDEX 
                                            // = 0: Endpoint 0 control and 
                                            // status For USB_INDEX = 1-5: IN 
                                            // endpoint {1-5} control and 
                                            // status (low byte) 
#define USB_CSIH                0x40089048  // Indexed register: For USB_INDEX 
                                            // = 1-5: IN endpoint {1-5} control 
                                            // and status (high byte) 
#define USB_MAXO                0x4008904C  // Indexed register: For USB_INDEX 
                                            // = 1-5: Maximum packet size for 
                                            // OUT endpoint {1-5} 
#define USB_CSOL                0x40089050  // Indexed register: For USB_INDEX 
                                            // = 1-5: OUT endpoint {1-5} 
                                            // control and status (low byte) 
#define USB_CSOH                0x40089054  // Indexed register: For USB_INDEX 
                                            // = 1-5: OUT endpoint {1-5} 
                                            // control and status (high byte) 
#define USB_CNT0_CNTL           0x40089058  // Indexed register: For USB_INDEX 
                                            // = 0: Number of received bytes in 
                                            // the endpoint 0 FIFO For 
                                            // USB_INDEX = 1-5: Number of 
                                            // received bytes in the OUT 
                                            // endpoint {1-5} FIFO (low byte) 
#define USB_CNTH                0x4008905C  // Indexed register: For USB_INDEX 
                                            // = 1-5: Number of received in the 
                                            // OUT endpoint {1-5} FIFO (high 
                                            // byte) 
#define USB_F0                  0x40089080  // Endpoint 0 FIFO 
#define USB_F1                  0x40089088  // IN/OUT endpoint 1 FIFO 
#define USB_F2                  0x40089090  // IN/OUT endpoint 2 FIFO 
#define USB_F3                  0x40089098  // IN/OUT endpoint 3 FIFO 
#define USB_F4                  0x400890A0  // IN/OUT endpoint 4 FIFO 
#define USB_F5                  0x400890A8  // IN/OUT endpoint 5 FIFO 


//*****************************************************************************
//
// The following are defines for the bit fields in the USB_ADDR register.
//
//*****************************************************************************
#define USB_ADDR_UPDATE         0x00000080  // This bit is set by hardware 
                                            // when writing to this register, 
                                            // and is cleared by hardware when 
                                            // the new address becomes 
                                            // effective. 
#define USB_ADDR_UPDATE_M       0x00000080
#define USB_ADDR_UPDATE_S       7
#define USB_ADDR_USBADDR_M      0x0000007F  // Device address. The address 
                                            // shall be updated upon successful 
                                            // completion of the status stage 
                                            // of the SET_ADDRESS request. 
#define USB_ADDR_USBADDR_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_POW register.
//
//*****************************************************************************
#define USB_POW_ISOWAITSOF      0x00000080  // For isochronous mode IN 
                                            // endpoints: When set, the USB 
                                            // controller will wait for an SOF 
                                            // token from the time 
                                            // USB_CSIL.INPKTRDY is set before 
                                            // sending the packet. If an IN 
                                            // token is received before an SOF 
                                            // token, then a zero length data 
                                            // packet will be sent. 
#define USB_POW_ISOWAITSOF_M    0x00000080
#define USB_POW_ISOWAITSOF_S    7
#define USB_POW_RST             0x00000008  // Indicates that reset signaling 
                                            // is present on the bus 
#define USB_POW_RST_M           0x00000008
#define USB_POW_RST_S           3
#define USB_POW_RESUME          0x00000004  // Drives resume signaling for 
                                            // remote wakeup According to the 
                                            // USB Specification, the resume 
                                            // signal must be held active for 
                                            // at least 1 ms and no more than 
                                            // 15 ms. It is recommended to keep 
                                            // this bit set for approximately 
                                            // 10 ms. 
#define USB_POW_RESUME_M        0x00000004
#define USB_POW_RESUME_S        2
#define USB_POW_SUSPEND         0x00000002  // Indicates entry into suspend 
                                            // mode Suspend mode must be 
                                            // enabled by setting 
                                            // USB_POW.SUSPENDEN Software 
                                            // clears this bit by reading the 
                                            // USB_CIF register or by asserting 
                                            // USB_POW.RESUME 
#define USB_POW_SUSPEND_M       0x00000002
#define USB_POW_SUSPEND_S       1
#define USB_POW_SUSPENDEN       0x00000001  // Enables detection of and entry 
                                            // into suspend mode. 
#define USB_POW_SUSPENDEN_M     0x00000001
#define USB_POW_SUSPENDEN_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_IIF register.
//
//*****************************************************************************
#define USB_IIF_INEP5IF         0x00000020  // Interrupt flag for IN endpoint 
                                            // 5 Cleared by hardware when read 
#define USB_IIF_INEP5IF_M       0x00000020
#define USB_IIF_INEP5IF_S       5
#define USB_IIF_INEP4IF         0x00000010  // Interrupt flag for IN endpoint 
                                            // 4 Cleared by hardware when read 
#define USB_IIF_INEP4IF_M       0x00000010
#define USB_IIF_INEP4IF_S       4
#define USB_IIF_INEP3IF         0x00000008  // Interrupt flag for IN endpoint 
                                            // 3 Cleared by hardware when read 
#define USB_IIF_INEP3IF_M       0x00000008
#define USB_IIF_INEP3IF_S       3
#define USB_IIF_INEP2IF         0x00000004  // Interrupt flag for IN endpoint 
                                            // 2 Cleared by hardware when read 
#define USB_IIF_INEP2IF_M       0x00000004
#define USB_IIF_INEP2IF_S       2
#define USB_IIF_INEP1IF         0x00000002  // Interrupt flag for IN endpoint 
                                            // 1 Cleared by hardware when read 
#define USB_IIF_INEP1IF_M       0x00000002
#define USB_IIF_INEP1IF_S       1
#define USB_IIF_EP0IF           0x00000001  // Interrupt flag for endpoint 0 
                                            // Cleared by hardware when read 
#define USB_IIF_EP0IF_M         0x00000001
#define USB_IIF_EP0IF_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_OIF register.
//
//*****************************************************************************
#define USB_OIF_OUTEP5IF        0x00000020  // Interrupt flag for OUT endpoint 
                                            // 5 Cleared by hardware when read 
#define USB_OIF_OUTEP5IF_M      0x00000020
#define USB_OIF_OUTEP5IF_S      5
#define USB_OIF_OUTEP4IF        0x00000010  // Interrupt flag for OUT endpoint 
                                            // 4 Cleared by hardware when read 
#define USB_OIF_OUTEP4IF_M      0x00000010
#define USB_OIF_OUTEP4IF_S      4
#define USB_OIF_OUTEP3IF        0x00000008  // Interrupt flag for OUT endpoint 
                                            // 3 Cleared by hardware when read 
#define USB_OIF_OUTEP3IF_M      0x00000008
#define USB_OIF_OUTEP3IF_S      3
#define USB_OIF_OUTEP2IF        0x00000004  // Interrupt flag for OUT endpoint 
                                            // 2 Cleared by hardware when read 
#define USB_OIF_OUTEP2IF_M      0x00000004
#define USB_OIF_OUTEP2IF_S      2
#define USB_OIF_OUTEP1IF        0x00000002  // Interrupt flag for OUT endpoint 
                                            // 1 Cleared by hardware when read 
#define USB_OIF_OUTEP1IF_M      0x00000002
#define USB_OIF_OUTEP1IF_S      1
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_CIF register.
//
//*****************************************************************************
#define USB_CIF_SOFIF           0x00000008  // Start-of-frame interrupt flag 
                                            // Cleared by hardware when read 
#define USB_CIF_SOFIF_M         0x00000008
#define USB_CIF_SOFIF_S         3
#define USB_CIF_RSTIF           0x00000004  // Reset interrupt flag Cleared by 
                                            // hardware when read 
#define USB_CIF_RSTIF_M         0x00000004
#define USB_CIF_RSTIF_S         2
#define USB_CIF_RESUMEIF        0x00000002  // Resume interrupt flag Cleared 
                                            // by hardware when read 
#define USB_CIF_RESUMEIF_M      0x00000002
#define USB_CIF_RESUMEIF_S      1
#define USB_CIF_SUSPENDIF       0x00000001  // Suspend interrupt flag Cleared 
                                            // by hardware when read 
#define USB_CIF_SUSPENDIF_M     0x00000001
#define USB_CIF_SUSPENDIF_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_IIE register.
//
//*****************************************************************************
#define USB_IIE_INEP5IE         0x00000020  // Interrupt enable for IN 
                                            // endpoint 5 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_IIE_INEP5IE_M       0x00000020
#define USB_IIE_INEP5IE_S       5
#define USB_IIE_INEP4IE         0x00000010  // Interrupt enable for IN 
                                            // endpoint 4 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_IIE_INEP4IE_M       0x00000010
#define USB_IIE_INEP4IE_S       4
#define USB_IIE_INEP3IE         0x00000008  // Interrupt enable for IN 
                                            // endpoint 3 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_IIE_INEP3IE_M       0x00000008
#define USB_IIE_INEP3IE_S       3
#define USB_IIE_INEP2IE         0x00000004  // Interrupt enable for IN 
                                            // endpoint 2 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_IIE_INEP2IE_M       0x00000004
#define USB_IIE_INEP2IE_S       2
#define USB_IIE_INEP1IE         0x00000002  // Interrupt enable for IN 
                                            // endpoint 1 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_IIE_INEP1IE_M       0x00000002
#define USB_IIE_INEP1IE_S       1
#define USB_IIE_EP0IE           0x00000001  // Interrupt enable for endpoint 0 
                                            // 0: Interrupt disabled 1: 
                                            // Interrupt enabled 
#define USB_IIE_EP0IE_M         0x00000001
#define USB_IIE_EP0IE_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_OIE register.
//
//*****************************************************************************
#define USB_OIE_reserved8_M     0x000000C0  // Reserved 
#define USB_OIE_reserved8_S     6
#define USB_OIE_OUTEP5IE        0x00000020  // Interrupt enable for OUT 
                                            // endpoint 5 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_OIE_OUTEP5IE_M      0x00000020
#define USB_OIE_OUTEP5IE_S      5
#define USB_OIE_OUTEP4IE        0x00000010  // Interrupt enable for OUT 
                                            // endpoint 4 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_OIE_OUTEP4IE_M      0x00000010
#define USB_OIE_OUTEP4IE_S      4
#define USB_OIE_OUTEP3IE        0x00000008  // Interrupt enable for OUT 
                                            // endpoint 3 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_OIE_OUTEP3IE_M      0x00000008
#define USB_OIE_OUTEP3IE_S      3
#define USB_OIE_OUTEP2IE        0x00000004  // Interrupt enable for OUT 
                                            // endpoint 2 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_OIE_OUTEP2IE_M      0x00000004
#define USB_OIE_OUTEP2IE_S      2
#define USB_OIE_OUTEP1IE        0x00000002  // Interrupt enable for OUT 
                                            // endpoint 1 0: Interrupt disabled 
                                            // 1: Interrupt enabled 
#define USB_OIE_OUTEP1IE_M      0x00000002
#define USB_OIE_OUTEP1IE_S      1
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_CIE register.
//
//*****************************************************************************
#define USB_CIE_SOFIE           0x00000008  // Start-of-frame interrupt enable 
                                            // 0: Interrupt disabled 1: 
                                            // Interrupt enabled 
#define USB_CIE_SOFIE_M         0x00000008
#define USB_CIE_SOFIE_S         3
#define USB_CIE_RSTIE           0x00000004  // Reset interrupt enable 0: 
                                            // Interrupt disabled 1: Interrupt 
                                            // enabled 
#define USB_CIE_RSTIE_M         0x00000004
#define USB_CIE_RSTIE_S         2
#define USB_CIE_RESUMEIE        0x00000002  // Resume interrupt enable 0: 
                                            // Interrupt disabled 1: Interrupt 
                                            // enabled 
#define USB_CIE_RESUMEIE_M      0x00000002
#define USB_CIE_RESUMEIE_S      1
#define USB_CIE_SUSPENDIE       0x00000001  // Suspend interrupt enable 0: 
                                            // Interrupt disabled 1: Interrupt 
                                            // enabled 
#define USB_CIE_SUSPENDIE_M     0x00000001
#define USB_CIE_SUSPENDIE_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_FRML register.
//
//*****************************************************************************
#define USB_FRML_FRAMEL_M       0x000000FF  // Bits 7:0 of the 11-bit frame 
                                            // number The frame number is only 
                                            // updated upon successful 
                                            // reception of SOF tokens 
#define USB_FRML_FRAMEL_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_FRMH register.
//
//*****************************************************************************
#define USB_FRMH_FRAMEH_M       0x00000007  // Bits 10:8 of the 11-bit frame 
                                            // number The frame number is only 
                                            // updated upon successful 
                                            // reception of SOF tokens 
#define USB_FRMH_FRAMEH_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_INDEX register.
//
//*****************************************************************************
#define USB_INDEX_USBINDEX_M    0x0000000F  // Index of the currently selected 
                                            // endpoint The index is set to 0 
                                            // to enable access to endpoint 0 
                                            // control and status registers The 
                                            // index is set to 1, 2, 3, 4 or 5 
                                            // to enable access to IN/OUT 
                                            // endpoint 1, 2, 3, 4 or 5 control 
                                            // and status registers, 
                                            // respectively 
#define USB_INDEX_USBINDEX_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_CTRL register.
//
//*****************************************************************************
#define USB_CTRL_PLLLOCKED      0x00000080  // PLL lock status. The PLL is 
                                            // locked when USB_CTRL.PLLLOCKED 
                                            // is 1. 
#define USB_CTRL_PLLLOCKED_M    0x00000080
#define USB_CTRL_PLLLOCKED_S    7
#define USB_CTRL_PLLEN          0x00000002  // 48 MHz USB PLL enable When this 
                                            // bit is set, the 48 MHz PLL is 
                                            // started. Software must avoid 
                                            // access to other USB registers 
                                            // before the PLL has locked; that 
                                            // is, USB_CTRL.PLLLOCKED is 1. 
                                            // This bit can be set only when 
                                            // USB_CTRL.USBEN is 1. The PLL 
                                            // must be disabled before entering 
                                            // PM1 when suspended, and must be 
                                            // re-enabled when resuming 
                                            // operation. 
#define USB_CTRL_PLLEN_M        0x00000002
#define USB_CTRL_PLLEN_S        1
#define USB_CTRL_USBEN          0x00000001  // USB enable The USB controller 
                                            // is reset when this bit is 
                                            // cleared 
#define USB_CTRL_USBEN_M        0x00000001
#define USB_CTRL_USBEN_S        0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_MAXI register.
//
//*****************************************************************************
#define USB_MAXI_USBMAXI_M      0x000000FF  // Maximum packet size, in units 
                                            // of 8 bytes, for the selected IN 
                                            // endpoint The value of this 
                                            // register should match the 
                                            // wMaxPacketSize field in the 
                                            // standard endpoint descriptor for 
                                            // the endpoint. The value must not 
                                            // exceed the available memory. 
#define USB_MAXI_USBMAXI_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_CS0_CSIL register.
//
//*****************************************************************************
#define USB_CS0_CSIL_CLROUTPKTRDY_or_CLRDATATOG \
                                0x00000040  // USB_CS0.CLROUTPKTRDY [RW]: 
                                            // Software sets this bit to clear 
                                            // the USB_CS0.OUTPKTRDY bit. It is 
                                            // cleared automatically. 
                                            // USB_CSIL.CLRDATATOG [RW]: 
                                            // Software sets this bit to reset 
                                            // the IN endpoint data toggle to 
                                            // 0. 

#define USB_CS0_CSIL_CLROUTPKTRDY_or_CLRDATATOG_M \
                                0x00000040
#define USB_CS0_CSIL_CLROUTPKTRDY_or_CLRDATATOG_S 6
#define USB_CS0_CSIL_SENDSTALL_or_SENTSTALL \
                                0x00000020  // USB_CS0.SENDSTALL [RW]: 
                                            // Software sets this bit to 
                                            // terminate the current 
                                            // transaction with a STALL 
                                            // handshake. The bit is cleared 
                                            // automatically when the STALL 
                                            // handshake has been transmitted. 
                                            // USB_CSIL.SENTSTALL [RW]: For 
                                            // bulk/interrupt mode IN 
                                            // endpoints: This bit is set when 
                                            // a STALL handshake is 
                                            // transmitted. The FIFO is flushed 
                                            // and the USB_CSIL.INPKTRDY bit 
                                            // cleared. Software should clear 
                                            // this bit. 

#define USB_CS0_CSIL_SENDSTALL_or_SENTSTALL_M \
                                0x00000020
#define USB_CS0_CSIL_SENDSTALL_or_SENTSTALL_S 5
#define USB_CS0_CSIL_SETUPEND_or_SENDSTALL \
                                0x00000010  // USB_CS0.SETUPEND [RO]: This bit 
                                            // is set when a control 
                                            // transaction ends before the 
                                            // USB_CS0.DATAEND bit has been 
                                            // set. An interrupt is generated 
                                            // and the FIFO flushed at this 
                                            // time. Software clears this bit 
                                            // by setting USB_CS0.CLRSETUPEND. 
                                            // CSIL.SENDSTALL [RW]: For 
                                            // bulk/interrupt mode IN 
                                            // endpoints: Software sets this 
                                            // bit to issue a STALL handshake. 
                                            // Software clears this bit to 
                                            // terminate the stall condition. 

#define USB_CS0_CSIL_SETUPEND_or_SENDSTALL_M \
                                0x00000010
#define USB_CS0_CSIL_SETUPEND_or_SENDSTALL_S 4
#define USB_CS0_CSIL_DATAEND_or_FLUSHPACKET \
                                0x00000008  // USB_CS0.DATAEND [RW]: This bit 
                                            // is used to signal the end of the 
                                            // data stage, and must be set: 1. 
                                            // When the last data packet is 
                                            // loaded and USB_CS0.INPKTRDY is 
                                            // set. 2. When the last data 
                                            // packet is unloaded and 
                                            // USB_CS0.CLROUTPKTRDY is set. 3. 
                                            // When USB_CS0.INPKTRDY is set to 
                                            // send a zero-length packet. The 
                                            // USB controller clears this bit 
                                            // automatically. 
                                            // USB_CSIL.FLUSHPACKET [RW]: 
                                            // Software sets this bit to flush 
                                            // the next packet to be 
                                            // transmitted from the IN endpoint 
                                            // FIFO. The FIFO pointer is reset 
                                            // and the USB_CSIL.INPKTRDY bit is 
                                            // cleared. Note: If the FIFO 
                                            // contains two packets, 
                                            // USB_CSIL.FLUSHPACKET will need 
                                            // to be set twice to completely 
                                            // clear the FIFO. 

#define USB_CS0_CSIL_DATAEND_or_FLUSHPACKET_M \
                                0x00000008
#define USB_CS0_CSIL_DATAEND_or_FLUSHPACKET_S 3
#define USB_CS0_CSIL_SENTSTALL_or_UNDERRUN \
                                0x00000004  // USB_CS0.SENTSTALL [RW]: This 
                                            // bit is set when a STALL 
                                            // handshake is sent. An interrupt 
                                            // is generated is generated when 
                                            // this bit is set. Software must 
                                            // clear this bit. 
                                            // USB_CSIL.UNDERRUN [RW]: In 
                                            // isochronous mode, this bit is 
                                            // set when a zero length data 
                                            // packet is sent after receiving 
                                            // an IN token with 
                                            // USB_CSIL.INPKTRDY not set. In 
                                            // bulk/interrupt mode, this bit is 
                                            // set when a NAK is returned in 
                                            // response to an IN token. 
                                            // Software should clear this bit. 

#define USB_CS0_CSIL_SENTSTALL_or_UNDERRUN_M \
                                0x00000004
#define USB_CS0_CSIL_SENTSTALL_or_UNDERRUN_S 2
#define USB_CS0_CSIL_INPKTRDY_or_PKTPRESENT \
                                0x00000002  // USB_CS0. INPKTRDY [RW]: 
                                            // Software sets this bit after 
                                            // loading a data packet into the 
                                            // endpoint 0 FIFO. It is cleared 
                                            // automatically when the data 
                                            // packet has been transmitted. An 
                                            // interrupt is generated when the 
                                            // bit is cleared. 
                                            // USB_CSIL.PKTPRESENT [RO]: This 
                                            // bit is set when there is at 
                                            // least one packet in the IN 
                                            // endpoint FIFO. 

#define USB_CS0_CSIL_INPKTRDY_or_PKTPRESENT_M \
                                0x00000002
#define USB_CS0_CSIL_INPKTRDY_or_PKTPRESENT_S 1
#define USB_CS0_CSIL_OUTPKTRDY_or_INPKTRDY \
                                0x00000001  // USB_CS0.OUTPKTRDY [RO]: 
                                            // Endpoint 0 data packet received 
                                            // An interrupt request (EP0) is 
                                            // generated if the interrupt is 
                                            // enabled. Software must read the 
                                            // endpoint 0 FIFO empty, and clear 
                                            // this bit by setting 
                                            // USB_CS0.CLROUTPKTRDY 
                                            // USB_CSIL.INPKTRDY [RW]: IN 
                                            // endpoint {1-5} packet transfer 
                                            // pending Software sets this bit 
                                            // after loading a data packet into 
                                            // the FIFO. It is cleared 
                                            // automatically when a data packet 
                                            // has been transmitted. An 
                                            // interrupt is generated (if 
                                            // enabled) when the bit is 
                                            // cleared. When using 
                                            // double-buffering, the bit is 
                                            // cleared immediately if the other 
                                            // FIFO is empty. 

#define USB_CS0_CSIL_OUTPKTRDY_or_INPKTRDY_M \
                                0x00000001
#define USB_CS0_CSIL_OUTPKTRDY_or_INPKTRDY_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_CSIH register.
//
//*****************************************************************************
#define USB_CSIH_AUTISET        0x00000080  // If set by software, the 
                                            // USB_CSIL.INPKTRDY bit is 
                                            // automatically set when a data 
                                            // packet of maximum size 
                                            // (specified by USBMAXI) is loaded 
                                            // into the IN endpoint FIFO. If a 
                                            // packet of less than the maximum 
                                            // packet size is loaded, then 
                                            // USB_CSIL.INPKTRDY will have to 
                                            // be set manually. 
#define USB_CSIH_AUTISET_M      0x00000080
#define USB_CSIH_AUTISET_S      7
#define USB_CSIH_ISO            0x00000040  // Selects IN endpoint type: 0: 
                                            // Bulk/interrupt 1: Isochronous 
#define USB_CSIH_ISO_M          0x00000040
#define USB_CSIH_ISO_S          6
#define USB_CSIH_FORCEDATATOG   0x00000008  // Software sets this bit to force 
                                            // the IN endpoint's data toggle to 
                                            // switch after each data packet is 
                                            // sent regardless of whether an 
                                            // ACK was received. This can be 
                                            // used by interrupt IN endpoints 
                                            // which are used to communicate 
                                            // rate feedback for isochronous 
                                            // endpoints. 
#define USB_CSIH_FORCEDATATOG_M 0x00000008
#define USB_CSIH_FORCEDATATOG_S 3
#define USB_CSIH_INDBLBUF       0x00000001  // IN endpoint FIFO 
                                            // double-buffering enable: 0: 
                                            // Double buffering disabled 1: 
                                            // Double buffering enabled 
#define USB_CSIH_INDBLBUF_M     0x00000001
#define USB_CSIH_INDBLBUF_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_MAXO register.
//
//*****************************************************************************
#define USB_MAXO_USBMAXO_M      0x000000FF  // Maximum packet size, in units 
                                            // of 8 bytes, for the selected OUT 
                                            // endpoint The value of this 
                                            // register should match the 
                                            // wMaxPacketSize field in the 
                                            // standard endpoint descriptor for 
                                            // the endpoint. The value must not 
                                            // exceed the available memory. 
#define USB_MAXO_USBMAXO_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_CSOL register.
//
//*****************************************************************************
#define USB_CSOL_CLRDATATOG     0x00000080  // Software sets this bit to reset 
                                            // the endpoint data toggle to 0. 
#define USB_CSOL_CLRDATATOG_M   0x00000080
#define USB_CSOL_CLRDATATOG_S   7
#define USB_CSOL_SENTSTALL      0x00000040  // This bit is set when a STALL 
                                            // handshake is transmitted. An 
                                            // interrupt is generated when this 
                                            // bit is set. Software should 
                                            // clear this bit. 
#define USB_CSOL_SENTSTALL_M    0x00000040
#define USB_CSOL_SENTSTALL_S    6
#define USB_CSOL_SENDSTALL      0x00000020  // For bulk/interrupt mode OUT 
                                            // endpoints: Software sets this 
                                            // bit to issue a STALL handshake. 
                                            // Software clears this bit to 
                                            // terminate the stall condition. 
#define USB_CSOL_SENDSTALL_M    0x00000020
#define USB_CSOL_SENDSTALL_S    5
#define USB_CSOL_FLUSHPACKET    0x00000010  // Software sets this bit to flush 
                                            // the next packet to be read from 
                                            // the endpoint OUT FIFO. Note: If 
                                            // the FIFO contains two packets, 
                                            // USB_CSOL.FLUSHPACKET will need 
                                            // to be set twice to completely 
                                            // clear the FIFO. 
#define USB_CSOL_FLUSHPACKET_M  0x00000010
#define USB_CSOL_FLUSHPACKET_S  4
#define USB_CSOL_DATAERROR      0x00000008  // For isochronous mode OUT 
                                            // endpoints: This bit is set when 
                                            // USB_CSOL.OUTPKTRDY is set if the 
                                            // data packet has a CRC or 
                                            // bit-stuff error. It is cleared 
                                            // automatically when 
                                            // USB_CSOL.OUTPKTRDY is cleared. 
#define USB_CSOL_DATAERROR_M    0x00000008
#define USB_CSOL_DATAERROR_S    3
#define USB_CSOL_OVERRUN        0x00000004  // For isochronous mode OUT 
                                            // endpoints: This bit is set when 
                                            // an OUT packet cannot be loaded 
                                            // into the OUT endpoint FIFO. 
                                            // Firmware should clear this bit. 
#define USB_CSOL_OVERRUN_M      0x00000004
#define USB_CSOL_OVERRUN_S      2
#define USB_CSOL_FIFOFULL       0x00000002  // This bit is set when no more 
                                            // packets can be loaded into the 
                                            // OUT endpoint FIFO. 
#define USB_CSOL_FIFOFULL_M     0x00000002
#define USB_CSOL_FIFOFULL_S     1
#define USB_CSOL_OUTPKTRDY      0x00000001  // This bit is set when a data 
                                            // packet has been received. 
                                            // Software should clear this bit 
                                            // when the packet has been 
                                            // unloaded from the OUT endpoint 
                                            // FIFO. An interrupt is generated 
                                            // when the bit is set. 
#define USB_CSOL_OUTPKTRDY_M    0x00000001
#define USB_CSOL_OUTPKTRDY_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_CSOH register.
//
//*****************************************************************************
#define USB_CSOH_AUTOCLEAR      0x00000080  // If software sets this bit, the 
                                            // USB_CSOL.OUTPKTRDY bit will be 
                                            // automatically cleared when a 
                                            // packet of maximum size 
                                            // (specified by USB_MAXO) has been 
                                            // unloaded from the OUT FIFO. When 
                                            // packets of less than the maximum 
                                            // packet size are unloaded, 
                                            // USB_CSOL.OUTPKTRDY will have to 
                                            // be cleared manually. 
#define USB_CSOH_AUTOCLEAR_M    0x00000080
#define USB_CSOH_AUTOCLEAR_S    7
#define USB_CSOH_ISO            0x00000040  // Selects OUT endpoint type: 0: 
                                            // Bulk/interrupt 1: Isochronous 
#define USB_CSOH_ISO_M          0x00000040
#define USB_CSOH_ISO_S          6
#define USB_CSOH_OUTDBLBUF      0x00000001  // OUT endpoint FIFO 
                                            // double-buffering enable: 0: 
                                            // Double buffering disabled 1: 
                                            // Double buffering enabled 
#define USB_CSOH_OUTDBLBUF_M    0x00000001
#define USB_CSOH_OUTDBLBUF_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// USB_CNT0_CNTL register.
//
//*****************************************************************************
#define USB_CNT0_CNTL_FIFOCNT_or_FIFOCNTL_M \
                                0x000000FF  // USB_CS0.FIFOCNT (USBINDEX = 0) 
                                            // [RO]: Number of bytes received 
                                            // in the packet in the endpoint 0 
                                            // FIFO Valid only when 
                                            // USB_CS0.OUTPKTRDY is set 
                                            // USB_CSIL.FIFOCNTL (USBINDEX = 1 
                                            // to 5) [RW]: Bits 7:0 of the of 
                                            // the number of bytes received in 
                                            // the packet in the OUT endpoint 
                                            // {1-5} FIFO Valid only when 
                                            // USB_CSOL.OUTPKTRDY is set 

#define USB_CNT0_CNTL_FIFOCNT_or_FIFOCNTL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_CNTH register.
//
//*****************************************************************************
#define USB_CNTH_FIFOCNTH_M     0x00000007  // Bits 10:8 of the of the number 
                                            // of bytes received in the packet 
                                            // in the OUT endpoint {1-5} FIFO 
                                            // Valid only when 
                                            // USB_CSOL.OUTPKTRDY is set 
#define USB_CNTH_FIFOCNTH_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_F0 register.
//
//*****************************************************************************
#define USB_F0_USBF0_M          0x000000FF  // Endpoint 0 FIFO Reading this 
                                            // register unloads one byte from 
                                            // the endpoint 0 FIFO. Writing to 
                                            // this register loads one byte 
                                            // into the endpoint 0 FIFO. The 
                                            // FIFO memory for EP0 is used for 
                                            // incoming and outgoing data 
                                            // packets. 
#define USB_F0_USBF0_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_F1 register.
//
//*****************************************************************************
#define USB_F1_USBF1_M          0x000000FF  // Endpoint 1 FIFO register 
                                            // Reading this register unloads 
                                            // one byte from the EP1 OUT FIFO. 
                                            // Writing to this register loads 
                                            // one byte into the EP1 IN FIFO. 
#define USB_F1_USBF1_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_F2 register.
//
//*****************************************************************************
#define USB_F2_USBF2_M          0x000000FF  // Endpoint 2 FIFO register 
                                            // Reading this register unloads 
                                            // one byte from the EP2 OUT FIFO. 
                                            // Writing to this register loads 
                                            // one byte into the EP2 IN FIFO. 
#define USB_F2_USBF2_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_F3 register.
//
//*****************************************************************************
#define USB_F3_USBF3_M          0x000000FF  // Endpoint 3 FIFO register 
                                            // Reading this register unloads 
                                            // one byte from the EP3 OUT FIFO. 
                                            // Writing to this register loads 
                                            // one byte into the EP3 IN FIFO. 
#define USB_F3_USBF3_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_F4 register.
//
//*****************************************************************************
#define USB_F4_USBF4_M          0x000000FF  // Endpoint 4 FIFO register 
                                            // Reading this register unloads 
                                            // one byte from the EP4 OUT FIFO. 
                                            // Writing to this register loads 
                                            // one byte into the EP4 IN FIFO. 
#define USB_F4_USBF4_S          0
//*****************************************************************************
//
// The following are defines for the bit fields in the USB_F5 register.
//
//*****************************************************************************
#define USB_F5_USBF5_M          0x000000FF  // Endpoint 5 FIFO register 
                                            // Reading this register unloads 
                                            // one byte from the EP5 OUT FIFO. 
                                            // Writing to this register loads 
                                            // one byte into the EP5 IN FIFO. 
#define USB_F5_USBF5_S          0


#endif // __HW_USB_H__

