/******************************************************************************
*  Filename:       hw_rfcore_xreg.h
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

#ifndef __HW_RFCORE_XREG_H__
#define __HW_RFCORE_XREG_H__

//*****************************************************************************
//
// The following are defines for the RFCORE_XREG register offsets.
//
//*****************************************************************************
#define RFCORE_XREG_FRMFILT0    0x40088600  // The frame filtering function 
                                            // rejects unintended frames as 
                                            // specified by IEEE 802.15.4, 
                                            // section 7.5.6.2, third filtering 
                                            // level. In addition, it provides 
                                            // filtering on: - The eight 
                                            // different frame types (see the 
                                            // FRMFILT1 register) - The 
                                            // reserved bits in the frame 
                                            // control field (FCF) The function 
                                            // is controlled by: - The FRMFILT0 
                                            // and FRMFILT1 registers - The 
                                            // PAN_ID, SHORT_ADDR, and EXT_ADDR 
                                            // values in RAM 
#define RFCORE_XREG_FRMFILT1    0x40088604  // The frame filtering function 
                                            // rejects unintended frames as 
                                            // specified by IEEE 802.15.4, 
                                            // section 7.5.6.2, third filtering 
                                            // level. In addition, it provides 
                                            // filtering on: - The eight 
                                            // different frame types (see the 
                                            // FRMFILT1 register) - The 
                                            // reserved bits in the frame 
                                            // control field (FCF) The function 
                                            // is controlled by: - The FRMFILT0 
                                            // and FRMFILT1 registers - The 
                                            // PAN_ID, SHORT_ADDR, and EXT_ADDR 
                                            // values in RAM 
#define RFCORE_XREG_SRCMATCH    0x40088608  // Source address matching and 
                                            // pending bits 
#define RFCORE_XREG_SRCSHORTEN0 \
                                0x4008860C  // Short address matching 

#define RFCORE_XREG_SRCSHORTEN1 \
                                0x40088610  // Short address matching 

#define RFCORE_XREG_SRCSHORTEN2 \
                                0x40088614  // Short address matching 

#define RFCORE_XREG_SRCEXTEN0   0x40088618  // Extended address matching 
#define RFCORE_XREG_SRCEXTEN1   0x4008861C  // Extended address matching 
#define RFCORE_XREG_SRCEXTEN2   0x40088620  // Extended address matching 
#define RFCORE_XREG_FRMCTRL0    0x40088624  // Frame handling 
#define RFCORE_XREG_FRMCTRL1    0x40088628  // Frame handling 
#define RFCORE_XREG_RXENABLE    0x4008862C  // RX enabling 
#define RFCORE_XREG_RXMASKSET   0x40088630  // RX enabling 
#define RFCORE_XREG_RXMASKCLR   0x40088634  // RX disabling 
#define RFCORE_XREG_FREQTUNE    0x40088638  // Crystal oscillator frequency 
                                            // tuning 
#define RFCORE_XREG_FREQCTRL    0x4008863C  // Controls the RF frequency 
#define RFCORE_XREG_TXPOWER     0x40088640  // Controls the output power 
#define RFCORE_XREG_TXCTRL      0x40088644  // Controls the TX settings 
#define RFCORE_XREG_FSMSTAT0    0x40088648  // Radio status register 
#define RFCORE_XREG_FSMSTAT1    0x4008864C  // Radio status register 
#define RFCORE_XREG_FIFOPCTRL   0x40088650  // FIFOP threshold 
#define RFCORE_XREG_FSMCTRL     0x40088654  // FSM options 
#define RFCORE_XREG_CCACTRL0    0x40088658  // CCA threshold 
#define RFCORE_XREG_CCACTRL1    0x4008865C  // Other CCA Options 
#define RFCORE_XREG_RSSI        0x40088660  // RSSI status register 
#define RFCORE_XREG_RSSISTAT    0x40088664  // RSSI valid status register 
#define RFCORE_XREG_RXFIRST     0x40088668  // First byte in RX FIFO 
#define RFCORE_XREG_RXFIFOCNT   0x4008866C  // Number of bytes in RX FIFO 
#define RFCORE_XREG_TXFIFOCNT   0x40088670  // Number of bytes in TX FIFO 
#define RFCORE_XREG_RXFIRST_PTR \
                                0x40088674  // RX FIFO pointer 

#define RFCORE_XREG_RXLAST_PTR \
                                0x40088678  // RX FIFO pointer 

#define RFCORE_XREG_RXP1_PTR    0x4008867C  // RX FIFO pointer 
#define RFCORE_XREG_TXFIRST_PTR \
                                0x40088684  // TX FIFO pointer 

#define RFCORE_XREG_TXLAST_PTR \
                                0x40088688  // TX FIFO pointer 

#define RFCORE_XREG_RFIRQM0     0x4008868C  // RF interrupt masks 
#define RFCORE_XREG_RFIRQM1     0x40088690  // RF interrupt masks 
#define RFCORE_XREG_RFERRM      0x40088694  // RF error interrupt mask 
#define RFCORE_XREG_RFRND       0x4008869C  // Random data 
#define RFCORE_XREG_MDMCTRL0    0x400886A0  // Controls modem 
#define RFCORE_XREG_MDMCTRL1    0x400886A4  // Controls modem 
#define RFCORE_XREG_FREQEST     0x400886A8  // Estimated RF frequency offset 
#define RFCORE_XREG_RXCTRL      0x400886AC  // Tune receive section 
#define RFCORE_XREG_FSCTRL      0x400886B0  // Tune frequency synthesizer 
#define RFCORE_XREG_FSCAL0      0x400886B4  // Tune frequency calibration 
#define RFCORE_XREG_FSCAL1      0x400886B8  // Tune frequency calibration 
#define RFCORE_XREG_FSCAL2      0x400886BC  // Tune frequency calibration 
#define RFCORE_XREG_FSCAL3      0x400886C0  // Tune frequency calibration 
#define RFCORE_XREG_AGCCTRL0    0x400886C4  // AGC dynamic range control 
#define RFCORE_XREG_AGCCTRL1    0x400886C8  // AGC reference level 
#define RFCORE_XREG_AGCCTRL2    0x400886CC  // AGC gain override 
#define RFCORE_XREG_AGCCTRL3    0x400886D0  // AGC control 
#define RFCORE_XREG_ADCTEST0    0x400886D4  // ADC tuning 
#define RFCORE_XREG_ADCTEST1    0x400886D8  // ADC tuning 
#define RFCORE_XREG_ADCTEST2    0x400886DC  // ADC tuning 
#define RFCORE_XREG_MDMTEST0    0x400886E0  // Test register for modem 
#define RFCORE_XREG_MDMTEST1    0x400886E4  // Test Register for Modem 
#define RFCORE_XREG_DACTEST0    0x400886E8  // DAC override value 
#define RFCORE_XREG_DACTEST1    0x400886EC  // DAC override value 
#define RFCORE_XREG_DACTEST2    0x400886F0  // DAC test setting 
#define RFCORE_XREG_ATEST       0x400886F4  // Analog test control 
#define RFCORE_XREG_PTEST0      0x400886F8  // Override power-down register 
#define RFCORE_XREG_PTEST1      0x400886FC  // Override power-down register 
#define RFCORE_XREG_CSPPROG0    0x40088700  // CSP program 
#define RFCORE_XREG_CSPPROG1    0x40088704  // CSP program 
#define RFCORE_XREG_CSPPROG2    0x40088708  // CSP program 
#define RFCORE_XREG_CSPPROG3    0x4008870C  // CSP program 
#define RFCORE_XREG_CSPPROG4    0x40088710  // CSP program 
#define RFCORE_XREG_CSPPROG5    0x40088714  // CSP program 
#define RFCORE_XREG_CSPPROG6    0x40088718  // CSP program 
#define RFCORE_XREG_CSPPROG7    0x4008871C  // CSP program 
#define RFCORE_XREG_CSPPROG8    0x40088720  // CSP program 
#define RFCORE_XREG_CSPPROG9    0x40088724  // CSP program 
#define RFCORE_XREG_CSPPROG10   0x40088728  // CSP program 
#define RFCORE_XREG_CSPPROG11   0x4008872C  // CSP program 
#define RFCORE_XREG_CSPPROG12   0x40088730  // CSP program 
#define RFCORE_XREG_CSPPROG13   0x40088734  // CSP program 
#define RFCORE_XREG_CSPPROG14   0x40088738  // CSP program 
#define RFCORE_XREG_CSPPROG15   0x4008873C  // CSP program 
#define RFCORE_XREG_CSPPROG16   0x40088740  // CSP program 
#define RFCORE_XREG_CSPPROG17   0x40088744  // CSP program 
#define RFCORE_XREG_CSPPROG18   0x40088748  // CSP program 
#define RFCORE_XREG_CSPPROG19   0x4008874C  // CSP program 
#define RFCORE_XREG_CSPPROG20   0x40088750  // CSP program 
#define RFCORE_XREG_CSPPROG21   0x40088754  // CSP program 
#define RFCORE_XREG_CSPPROG22   0x40088758  // CSP program 
#define RFCORE_XREG_CSPPROG23   0x4008875C  // CSP program 
#define RFCORE_XREG_CSPCTRL     0x40088780  // CSP control bit 
#define RFCORE_XREG_CSPSTAT     0x40088784  // CSP status register 
#define RFCORE_XREG_CSPX        0x40088788  // CSP X data register 
#define RFCORE_XREG_CSPY        0x4008878C  // CSP Y data register 
#define RFCORE_XREG_CSPZ        0x40088790  // CSP Z data register 
#define RFCORE_XREG_CSPT        0x40088794  // CSP T data register 
#define RFCORE_XREG_RFC_OBS_CTRL0 \
                                0x400887AC  // RF observation mux control 

#define RFCORE_XREG_RFC_OBS_CTRL1 \
                                0x400887B0  // RF observation mux control 

#define RFCORE_XREG_RFC_OBS_CTRL2 \
                                0x400887B4  // RF observation mux control 

#define RFCORE_XREG_TXFILTCFG   0x400887E8  // TX filter configuration 


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FRMFILT0 register.
//
//*****************************************************************************
#define RFCORE_XREG_FRMFILT0_MAX_FRAME_VERSION_M \
                                0x0000000C  // Used for filtering on the frame 
                                            // version field of the frame 
                                            // control field (FCF) If 
                                            // FCF[13:12] (the frame version 
                                            // subfield) is higher than 
                                            // MAX_FRAME_VERSION[1:0] and frame 
                                            // filtering is enabled, the frame 
                                            // is rejected. 

#define RFCORE_XREG_FRMFILT0_MAX_FRAME_VERSION_S 2
#define RFCORE_XREG_FRMFILT0_PAN_COORDINATOR \
                                0x00000002  // Should be set high when the 
                                            // device is a PAN coordinator, to 
                                            // accept frames with no 
                                            // destination address (as 
                                            // specified in Section 7.5.6.2 in 
                                            // IEEE 802.15.4) 0: Device is not 
                                            // a PAN coordinator 1: Device is a 
                                            // PAN coordinator 

#define RFCORE_XREG_FRMFILT0_PAN_COORDINATOR_M \
                                0x00000002
#define RFCORE_XREG_FRMFILT0_PAN_COORDINATOR_S 1
#define RFCORE_XREG_FRMFILT0_FRAME_FILTER_EN \
                                0x00000001  // Enables frame filtering When 
                                            // this bit is set, the radio 
                                            // performs frame filtering as 
                                            // specified in section 7.5.6.2 of 
                                            // IEEE 802.15.4(b), third 
                                            // filtering level. FRMFILT0[6:1] 
                                            // and FRMFILT1[7:1], together with 
                                            // the local address information, 
                                            // define the behavior of the 
                                            // filtering algorithm. 0: Frame 
                                            // filtering off. (FRMFILT0[6:1], 
                                            // FRMFILT1[7:1] and SRCMATCH[2:0] 
                                            // are don't care.) 1: Frame 
                                            // filtering on. 

#define RFCORE_XREG_FRMFILT0_FRAME_FILTER_EN_M \
                                0x00000001
#define RFCORE_XREG_FRMFILT0_FRAME_FILTER_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FRMFILT1 register.
//
//*****************************************************************************
#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_3_MAC_CMD \
                                0x00000040  // Defines whether MAC command 
                                            // frames are accepted or not. MAC 
                                            // command frames have frame type = 
                                            // 011. 0: Reject 1: Accept 

#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_3_MAC_CMD_M \
                                0x00000040
#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_3_MAC_CMD_S 6
#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_2_ACK \
                                0x00000020  // Defines whether acknowledgment 
                                            // frames are accepted or not. 
                                            // Acknowledgement frames have 
                                            // frame type = 010. 0: Reject 1: 
                                            // Accept 

#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_2_ACK_M \
                                0x00000020
#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_2_ACK_S 5
#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_1_DATA \
                                0x00000010  // Defines whether data frames are 
                                            // accepted or not. Data frames 
                                            // have frame type = 001. 0: Reject 
                                            // 1: Accept 

#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_1_DATA_M \
                                0x00000010
#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_1_DATA_S 4
#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_0_BEACON \
                                0x00000008  // Defines whether beacon frames 
                                            // are accepted or not. Beacon 
                                            // frames have frame type = 000. 0: 
                                            // Reject 1: Accept 

#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_0_BEACON_M \
                                0x00000008
#define RFCORE_XREG_FRMFILT1_ACCEPT_FT_0_BEACON_S 3
#define RFCORE_XREG_FRMFILT1_MODIFY_FT_FILTER_M \
                                0x00000006  // These bits are used to modify 
                                            // the frame type field of a 
                                            // received frame before frame type 
                                            // filtering is performed. The 
                                            // modification does not influence 
                                            // the frame that is written to the 
                                            // RX FIFO. 00: Leave the frame 
                                            // type as it is. 01: Invert MSB of 
                                            // the frame type. 10: Set MSB of 
                                            // the frame type to 0. 11: Set MSB 
                                            // of the frame type to 1. 

#define RFCORE_XREG_FRMFILT1_MODIFY_FT_FILTER_S 1
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_SRCMATCH register.
//
//*****************************************************************************
#define RFCORE_XREG_SRCMATCH_PEND_DATAREQ_ONLY \
                                0x00000004  // When this bit is set, the 
                                            // AUTOPEND function also requires 
                                            // that the received frame is a 
                                            // DATA REQUEST MAC command frame. 

#define RFCORE_XREG_SRCMATCH_PEND_DATAREQ_ONLY_M \
                                0x00000004
#define RFCORE_XREG_SRCMATCH_PEND_DATAREQ_ONLY_S 2
#define RFCORE_XREG_SRCMATCH_AUTOPEND \
                                0x00000002  // Automatic acknowledgment 
                                            // pending flag enable When a frame 
                                            // is received, the pending bit in 
                                            // the (possibly) returned 
                                            // acknowledgment is set 
                                            // automatically when the following 
                                            // conditions are met: - 
                                            // FRMFILT.FRAME_FILTER_EN is set. 
                                            // - SRCMATCH.SRC_MATCH_EN is set. 
                                            // - SRCMATCH.AUTOPEND is set. - 
                                            // The received frame matches the 
                                            // current 
                                            // SRCMATCH.PEND_DATAREQ_ONLY 
                                            // setting. - The received source 
                                            // address matches at least one 
                                            // source match table entry, which 
                                            // is enabled in SHORT_ADDR_EN and 
                                            // SHORT_PEND_EN or in EXT_ADDR_EN 
                                            // and EXT_PEND_EN. 

#define RFCORE_XREG_SRCMATCH_AUTOPEND_M \
                                0x00000002
#define RFCORE_XREG_SRCMATCH_AUTOPEND_S 1
#define RFCORE_XREG_SRCMATCH_SRC_MATCH_EN \
                                0x00000001  // Source address matching enable 
                                            // (requires that 
                                            // FRMFILT.FRAME_FILTER_EN = 1) 

#define RFCORE_XREG_SRCMATCH_SRC_MATCH_EN_M \
                                0x00000001
#define RFCORE_XREG_SRCMATCH_SRC_MATCH_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_SRCSHORTEN0 register.
//
//*****************************************************************************
#define RFCORE_XREG_SRCSHORTEN0_SHORT_ADDR_EN_M \
                                0x000000FF  // 7:0 part of the 24-bit word 
                                            // SHORT_ADDR_EN that enables or 
                                            // disables source address matching 
                                            // for each of the 24 short address 
                                            // table entries Optional safety 
                                            // feature: To ensure that an entry 
                                            // in the source matching table is 
                                            // not used while it is being 
                                            // updated, set the corresponding 
                                            // SHORT_ADDR_EN bit to 0 while 
                                            // updating. 

#define RFCORE_XREG_SRCSHORTEN0_SHORT_ADDR_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_SRCSHORTEN1 register.
//
//*****************************************************************************
#define RFCORE_XREG_SRCSHORTEN1_SHORT_ADDR_EN_M \
                                0x000000FF  // 15:8 part of the 24-bit word 
                                            // SHORT_ADDR_EN See description of 
                                            // SRCSHORTEN0.SHORT_ADDR_EN. 

#define RFCORE_XREG_SRCSHORTEN1_SHORT_ADDR_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_SRCSHORTEN2 register.
//
//*****************************************************************************
#define RFCORE_XREG_SRCSHORTEN2_SHORT_ADDR_EN_M \
                                0x000000FF  // 23:16 part of the 24-bit word 
                                            // SHORT_ADDR_EN See description of 
                                            // SRCSHORTEN0.SHORT_ADDR_EN. 

#define RFCORE_XREG_SRCSHORTEN2_SHORT_ADDR_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_SRCEXTEN0 register.
//
//*****************************************************************************
#define RFCORE_XREG_SRCEXTEN0_EXT_ADDR_EN_M \
                                0x000000FF  // 7:0 part of the 24-bit word 
                                            // EXT_ADDR_EN that enables or 
                                            // disables source address matching 
                                            // for each of the 12 extended 
                                            // address table entries Write 
                                            // access: Extended address enable 
                                            // for table entry n (0 to 11) is 
                                            // mapped to EXT_ADDR_EN[2n]. All 
                                            // EXT_ADDR_EN[2n + 1] bits are 
                                            // read only. Read access: Extended 
                                            // address enable for table entry n 
                                            // (0 to 11) is mapped to 
                                            // EXT_ADDR_EN[2n] and 
                                            // EXT_ADDR_EN[2n + 1]. Optional 
                                            // safety feature: To ensure that 
                                            // an entry in the source matching 
                                            // table is not used while it is 
                                            // being updated, set the 
                                            // corresponding EXT_ADDR_EN bit to 
                                            // 0 while updating. 

#define RFCORE_XREG_SRCEXTEN0_EXT_ADDR_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_SRCEXTEN1 register.
//
//*****************************************************************************
#define RFCORE_XREG_SRCEXTEN1_EXT_ADDR_EN_M \
                                0x000000FF  // 15:8 part of the 24-bit word 
                                            // EXT_ADDR_EN See description of 
                                            // SRCEXTEN0.EXT_ADDR_EN. 

#define RFCORE_XREG_SRCEXTEN1_EXT_ADDR_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_SRCEXTEN2 register.
//
//*****************************************************************************
#define RFCORE_XREG_SRCEXTEN2_EXT_ADDR_EN_M \
                                0x000000FF  // 23:16 part of the 24-bit word 
                                            // EXT_ADDR_EN See description of 
                                            // SRCEXTEN0.EXT_ADDR_EN. 

#define RFCORE_XREG_SRCEXTEN2_EXT_ADDR_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FRMCTRL0 register.
//
//*****************************************************************************
#define RFCORE_XREG_FRMCTRL0_APPEND_DATA_MODE \
                                0x00000080  // When AUTOCRC = 0: Don't care 
                                            // When AUTOCRC = 1: 0: RSSI + The 
                                            // CRC_OK bit and the 7-bit 
                                            // correlation value are appended 
                                            // at the end of each received 
                                            // frame 1: RSSI + The CRC_OK bit 
                                            // and the 7-bit SRCRESINDEX are 
                                            // appended at the end of each 
                                            // received frame. 

#define RFCORE_XREG_FRMCTRL0_APPEND_DATA_MODE_M \
                                0x00000080
#define RFCORE_XREG_FRMCTRL0_APPEND_DATA_MODE_S 7
#define RFCORE_XREG_FRMCTRL0_AUTOCRC \
                                0x00000040  // In TX 1: A CRC-16 (ITU-T) is 
                                            // generated in hardware and 
                                            // appended to the transmitted 
                                            // frame. There is no need to write 
                                            // the last 2 bytes to TXBUF. 0: No 
                                            // CRC-16 is appended to the frame. 
                                            // The last 2 bytes of the frame 
                                            // must be generated manually and 
                                            // written to TXBUF (if not, 
                                            // TX_UNDERFLOW occurs). In RX 1: 
                                            // The CRC-16 is checked in 
                                            // hardware, and replaced in the 
                                            // RXFIFO by a 16-bit status word 
                                            // which contains a CRC OK bit. The 
                                            // status word is controllable 
                                            // through APPEND_DATA_MODE. 0: The 
                                            // last 2 bytes of the frame 
                                            // (CRC-16 field) are stored in the 
                                            // RX FIFO. The CRC (if any) must 
                                            // be done manually. This setting 
                                            // does not influence 
                                            // acknowledgment transmission 
                                            // (including AUTOACK). 

#define RFCORE_XREG_FRMCTRL0_AUTOCRC_M \
                                0x00000040
#define RFCORE_XREG_FRMCTRL0_AUTOCRC_S 6
#define RFCORE_XREG_FRMCTRL0_AUTOACK \
                                0x00000020  // Defines whether the radio 
                                            // automatically transmits 
                                            // acknowledge frames or not. When 
                                            // autoack is enabled, all frames 
                                            // that are accepted by address 
                                            // filtering, have the acknowledge 
                                            // request flag set, and have a 
                                            // valid CRC are automatically 
                                            // acknowledged 12 symbol periods 
                                            // after being received. 0: Autoack 
                                            // disabled 1: Autoack enabled 

#define RFCORE_XREG_FRMCTRL0_AUTOACK_M \
                                0x00000020
#define RFCORE_XREG_FRMCTRL0_AUTOACK_S 5
#define RFCORE_XREG_FRMCTRL0_ENERGY_SCAN \
                                0x00000010  // Defines whether the RSSI 
                                            // register contains the 
                                            // most-recent signal strength or 
                                            // the peak signal strength since 
                                            // the energy scan was enabled. 0: 
                                            // Most-recent signal strength 1: 
                                            // Peak signal strength 

#define RFCORE_XREG_FRMCTRL0_ENERGY_SCAN_M \
                                0x00000010
#define RFCORE_XREG_FRMCTRL0_ENERGY_SCAN_S 4
#define RFCORE_XREG_FRMCTRL0_RX_MODE_M \
                                0x0000000C  // Set RX modes. 00: Normal 
                                            // operation, use RX FIFO 01: 
                                            // Receive serial mode, output 
                                            // received data on to IOC; 
                                            // infinite RX 10: RX FIFO looping 
                                            // ignore overflow in RX FIFO; 
                                            // infinite reception 11: Same as 
                                            // normal operation except that 
                                            // symbol search is disabled. Can 
                                            // be used for RSSI or CCA 
                                            // measurements when finding symbol 
                                            // is not desired. 

#define RFCORE_XREG_FRMCTRL0_RX_MODE_S 2
#define RFCORE_XREG_FRMCTRL0_TX_MODE_M \
                                0x00000003  // Set test modes for TX. 00: 
                                            // Normal operation, transmit TX 
                                            // FIFO 01: Reserved, should not be 
                                            // used 10: TX FIFO looping ignore 
                                            // underflow in TX FIFO and read 
                                            // cyclic; infinite transmission 
                                            // 11: Send random data from CRC; 
                                            // infinite transmission 

#define RFCORE_XREG_FRMCTRL0_TX_MODE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FRMCTRL1 register.
//
//*****************************************************************************
#define RFCORE_XREG_FRMCTRL1_PENDING_OR \
                                0x00000004  // Defines whether the pending 
                                            // data bit in outgoing 
                                            // acknowledgment frames is always 
                                            // set to 1 or controlled by the 
                                            // main FSM and the address 
                                            // filtering 0: Pending data bit is 
                                            // controlled by main FSM and 
                                            // address filtering. 1: Pending 
                                            // data bit is always 1. 

#define RFCORE_XREG_FRMCTRL1_PENDING_OR_M \
                                0x00000004
#define RFCORE_XREG_FRMCTRL1_PENDING_OR_S 2
#define RFCORE_XREG_FRMCTRL1_IGNORE_TX_UNDERF \
                                0x00000002  // Defines whether or not TX 
                                            // underflow should be ignored 0: 
                                            // Normal TX operation. TX 
                                            // underflow is detected and TX is 
                                            // aborted if underflow occurs. 1: 
                                            // Ignore TX underflow. Transmit 
                                            // the number of bytes given by the 
                                            // frame-length field. 

#define RFCORE_XREG_FRMCTRL1_IGNORE_TX_UNDERF_M \
                                0x00000002
#define RFCORE_XREG_FRMCTRL1_IGNORE_TX_UNDERF_S 1
#define RFCORE_XREG_FRMCTRL1_SET_RXENMASK_ON_TX \
                                0x00000001  // Defines whether STXON sets bit 
                                            // 6 in the RXENABLE register or 
                                            // leaves it unchanged 0: Does not 
                                            // affect RXENABLE 1: Sets bit 6 in 
                                            // RXENABLE. Used for backward 
                                            // compatibility with the CC2420. 

#define RFCORE_XREG_FRMCTRL1_SET_RXENMASK_ON_TX_M \
                                0x00000001
#define RFCORE_XREG_FRMCTRL1_SET_RXENMASK_ON_TX_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXENABLE register.
//
//*****************************************************************************
#define RFCORE_XREG_RXENABLE_RXENMASK_M \
                                0x000000FF  // RXENABLE enables the receiver. 
                                            // A nonzero value in this register 
                                            // causes FFCTRL to enable the 
                                            // receiver when in idle, after 
                                            // transmission and after 
                                            // acknowledgement transmission. 
                                            // The following strobes can modify 
                                            // RXENMASK: SRXON: Set bit 7 in 
                                            // RXENMASK. STXON: Set bit 6 in 
                                            // RXENMASK if SET_RXENMASK_ON_TX = 
                                            // 1. SRFOFF: Clears all bits in 
                                            // RXENMASK. SRXMASKBITSET: Set bit 
                                            // 5 in RXENMASK. SRXMASKBITCLR: 
                                            // Clear bit 5 in RXENMASK. There 
                                            // could be conflicts between the 
                                            // CSP and xreg_bus write 
                                            // operations if both operations 
                                            // try to modify RXENMASK 
                                            // simultaneously. To handle the 
                                            // case of simultaneous access to 
                                            // RXENMASK the following rules 
                                            // apply: - If the two sources 
                                            // agree (they modify different 
                                            // parts of the register) both of 
                                            // their requests to modify 
                                            // RXENMASK are processed. - If 
                                            // both operations try to modify 
                                            // the mask simultaneously, bus 
                                            // write operations to RXMASKSET 
                                            // and RXMASKCLR have priority over 
                                            // the CSP. This situation must be 
                                            // avoided. 

#define RFCORE_XREG_RXENABLE_RXENMASK_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXMASKSET register.
//
//*****************************************************************************
#define RFCORE_XREG_RXMASKSET_RXENMASKSET_M \
                                0x000000FF  // When written, the written data 
                                            // is ORed with the RXENMASK and 
                                            // stored in RXENMASK. 

#define RFCORE_XREG_RXMASKSET_RXENMASKSET_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXMASKCLR register.
//
//*****************************************************************************
#define RFCORE_XREG_RXMASKCLR_RXENMASKCLR_M \
                                0x000000FF  // When written, the written data 
                                            // is inverted and ANDed with the 
                                            // RXENMASK and stored in RXENMASK. 
                                            // For example, if 1 is written to 
                                            // one or more bit positions in 
                                            // this register, the corresponding 
                                            // bits are cleared in RXENMASK. 

#define RFCORE_XREG_RXMASKCLR_RXENMASKCLR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FREQTUNE register.
//
//*****************************************************************************
#define RFCORE_XREG_FREQTUNE_XOSC32M_TUNE_M \
                                0x0000000F  // Tune crystal oscillator The 
                                            // default setting 1111 leaves the 
                                            // XOSC untuned. Changing the 
                                            // setting from the default setting 
                                            // (1111) switches in extra 
                                            // capacitance to the oscillator, 
                                            // effectively lowering the XOSC 
                                            // frequency. Hence, a higher 
                                            // setting gives a higher 
                                            // frequency. 

#define RFCORE_XREG_FREQTUNE_XOSC32M_TUNE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FREQCTRL register.
//
//*****************************************************************************
#define RFCORE_XREG_FREQCTRL_FREQ_M \
                                0x0000007F  // Frequency control word The 
                                            // frequency word in FREQ[6:0] is 
                                            // an offset value from 2394 (fRF = 
                                            // FREQ[6 0] + 2394). The 
                                            // RF-frequency is specified from 
                                            // 2405 to 2480 MHz in 1-MHz steps; 
                                            // hence, the only valid settings 
                                            // for FREQ[6:0] are 11 to 86 (11 + 
                                            // 2394 = 2405 and 86 + 2394 = 
                                            // 2480). The device supports the 
                                            // frequency range from 2394 to 
                                            // 2507 MHz. Consequently, the 
                                            // usable settings for FREQ[6:0] 
                                            // are 0 to 113. Settings outside 
                                            // of the usable range (114 to 127) 
                                            // give a frequency of 2507 MHz. 
                                            // IEEE 802.15.4-2006 specifies a 
                                            // frequency range from 2405 MHz to 
                                            // 2480 MHz with 16 channels 5 MHz 
                                            // apart. The channels are numbered 
                                            // 11 through 26. For an IEEE 
                                            // 802.15.4-2006 compliant system, 
                                            // the only valid settings are thus 
                                            // FREQ[6:0] = 11 + 5 (channel 
                                            // number - 11). 

#define RFCORE_XREG_FREQCTRL_FREQ_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_TXPOWER register.
//
//*****************************************************************************
#define RFCORE_XREG_TXPOWER_PA_POWER_M \
                                0x000000F0  // PA power control 

#define RFCORE_XREG_TXPOWER_PA_POWER_S 4
#define RFCORE_XREG_TXPOWER_PA_BIAS_M \
                                0x0000000F  // PA bias control 

#define RFCORE_XREG_TXPOWER_PA_BIAS_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_TXCTRL register.
//
//*****************************************************************************
#define RFCORE_XREG_TXCTRL_DAC_CURR_M \
                                0x00000070  // Change the current in the DAC. 

#define RFCORE_XREG_TXCTRL_DAC_CURR_S 4
#define RFCORE_XREG_TXCTRL_DAC_DC_M \
                                0x0000000C  // Adjusts the DC level to the TX 
                                            // mixer. 

#define RFCORE_XREG_TXCTRL_DAC_DC_S 2
#define RFCORE_XREG_TXCTRL_TXMIX_CURRENT_M \
                                0x00000003  // Transmit mixers core current 
                                            // Current increases with 
                                            // increasing setting. 

#define RFCORE_XREG_TXCTRL_TXMIX_CURRENT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FSMSTAT0 register.
//
//*****************************************************************************
#define RFCORE_XREG_FSMSTAT0_CAL_DONE \
                                0x00000080  // Frequency synthesis calibration 
                                            // has been performed since the 
                                            // last time the FS was turned on. 

#define RFCORE_XREG_FSMSTAT0_CAL_DONE_M \
                                0x00000080
#define RFCORE_XREG_FSMSTAT0_CAL_DONE_S 7
#define RFCORE_XREG_FSMSTAT0_CAL_RUNNING \
                                0x00000040  // Frequency synthesis calibration 
                                            // status 0: Calibration is 
                                            // complete or not started. 1: 
                                            // Calibration is in progress. 

#define RFCORE_XREG_FSMSTAT0_CAL_RUNNING_M \
                                0x00000040
#define RFCORE_XREG_FSMSTAT0_CAL_RUNNING_S 6
#define RFCORE_XREG_FSMSTAT0_FSM_FFCTRL_STATE_M \
                                0x0000003F  // Gives the current state of the 
                                            // FIFO and frame control (FFCTRL) 
                                            // finite state-machine. 

#define RFCORE_XREG_FSMSTAT0_FSM_FFCTRL_STATE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FSMSTAT1 register.
//
//*****************************************************************************
#define RFCORE_XREG_FSMSTAT1_FIFO \
                                0x00000080  // FIFO is high when there is data 
                                            // in the RX FIFO. FIFO is low 
                                            // during RX FIFO overflow. 

#define RFCORE_XREG_FSMSTAT1_FIFO_M \
                                0x00000080
#define RFCORE_XREG_FSMSTAT1_FIFO_S 7
#define RFCORE_XREG_FSMSTAT1_FIFOP \
                                0x00000040  // FIFOP is set high when there 
                                            // are at more than FIFOP_THR bytes 
                                            // of data in the RX FIFO that has 
                                            // passed frame filtering. FIFOP is 
                                            // set high when there is at least 
                                            // one complete frame in the RX 
                                            // FIFO. FIFOP is high during RX 
                                            // FIFO overflow. 

#define RFCORE_XREG_FSMSTAT1_FIFOP_M \
                                0x00000040
#define RFCORE_XREG_FSMSTAT1_FIFOP_S 6
#define RFCORE_XREG_FSMSTAT1_SFD \
                                0x00000020  // In TX 0: When a complete frame 
                                            // with SFD was sent or no SFD was 
                                            // sent 1: SFD was sent. In RX 0: 
                                            // When a complete frame was 
                                            // received or no SFD was received 
                                            // 1: SFD was received. 

#define RFCORE_XREG_FSMSTAT1_SFD_M \
                                0x00000020
#define RFCORE_XREG_FSMSTAT1_SFD_S 5
#define RFCORE_XREG_FSMSTAT1_CCA \
                                0x00000010  // Clear channel assessment 
                                            // Dependent on CCA_MODE settings. 
                                            // See CCACTRL1 for details. 

#define RFCORE_XREG_FSMSTAT1_CCA_M \
                                0x00000010
#define RFCORE_XREG_FSMSTAT1_CCA_S 4
#define RFCORE_XREG_FSMSTAT1_SAMPLED_CCA \
                                0x00000008  // Contains a sampled value of the 
                                            // CCA The value is updated when a 
                                            // SSAMPLECCA or STXONCCA strobe is 
                                            // issued. 

#define RFCORE_XREG_FSMSTAT1_SAMPLED_CCA_M \
                                0x00000008
#define RFCORE_XREG_FSMSTAT1_SAMPLED_CCA_S 3
#define RFCORE_XREG_FSMSTAT1_LOCK_STATUS \
                                0x00000004  // 1 when PLL is in lock; 
                                            // otherwise 0 

#define RFCORE_XREG_FSMSTAT1_LOCK_STATUS_M \
                                0x00000004
#define RFCORE_XREG_FSMSTAT1_LOCK_STATUS_S 2
#define RFCORE_XREG_FSMSTAT1_TX_ACTIVE \
                                0x00000002  // Status signal Active when FFC 
                                            // is in one of the transmit states 

#define RFCORE_XREG_FSMSTAT1_TX_ACTIVE_M \
                                0x00000002
#define RFCORE_XREG_FSMSTAT1_TX_ACTIVE_S 1
#define RFCORE_XREG_FSMSTAT1_RX_ACTIVE \
                                0x00000001  // Status signal Active when FFC 
                                            // is in one of the receive states 

#define RFCORE_XREG_FSMSTAT1_RX_ACTIVE_M \
                                0x00000001
#define RFCORE_XREG_FSMSTAT1_RX_ACTIVE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FIFOPCTRL register.
//
//*****************************************************************************
#define RFCORE_XREG_FIFOPCTRL_FIFOP_THR_M \
                                0x0000007F  // Threshold used when generating 
                                            // FIFOP signal 

#define RFCORE_XREG_FIFOPCTRL_FIFOP_THR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FSMCTRL register.
//
//*****************************************************************************
#define RFCORE_XREG_FSMCTRL_SLOTTED_ACK \
                                0x00000002  // Controls timing of transmission 
                                            // of acknowledge frames 0: The 
                                            // acknowledge frame is sent 12 
                                            // symbol periods after the end of 
                                            // the received frame which 
                                            // requests the aknowledge. 1: The 
                                            // acknowledge frame is sent at the 
                                            // first backoff-slot boundary more 
                                            // than 12 symbol periods after the 
                                            // end of the received frame which 
                                            // requests the aknowledge. 

#define RFCORE_XREG_FSMCTRL_SLOTTED_ACK_M \
                                0x00000002
#define RFCORE_XREG_FSMCTRL_SLOTTED_ACK_S 1
#define RFCORE_XREG_FSMCTRL_RX2RX_TIME_OFF \
                                0x00000001  // Defines whether or not a 
                                            // 12-symbol time-out should be 
                                            // used after frame reception has 
                                            // ended. 0: No time-out 1: 
                                            // 12-symbol-period time-out 

#define RFCORE_XREG_FSMCTRL_RX2RX_TIME_OFF_M \
                                0x00000001
#define RFCORE_XREG_FSMCTRL_RX2RX_TIME_OFF_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CCACTRL0 register.
//
//*****************************************************************************
#define RFCORE_XREG_CCACTRL0_CCA_THR_M \
                                0x000000FF  // Clear-channel-assessment 
                                            // threshold value, signed 
                                            // 2's-complement number for 
                                            // comparison with the RSSI. The 
                                            // unit is 1 dB, offset is 73dB The 
                                            // CCA signal goes high when the 
                                            // received signal is below this 
                                            // value. The CCA signal is 
                                            // available on the CCA pin and in 
                                            // the FSMSTAT1 register. The value 
                                            // must never be set lower than 
                                            // CCA_HYST - 128 to avoid 
                                            // erroneous behavior of the CCA 
                                            // signal. Note: The reset value 
                                            // translates to an input level of 
                                            // approximately -32 - 73 = -105 
                                            // dBm, which is well below the 
                                            // sensitivity limit. This means 
                                            // that the CCA signal never 
                                            // indicates a clear channel. This 
                                            // register should be updated to 
                                            // 0xF8, which translates to an 
                                            // input level of about -8 - 73 = 
                                            // -81 dBm. 

#define RFCORE_XREG_CCACTRL0_CCA_THR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CCACTRL1 register.
//
//*****************************************************************************
#define RFCORE_XREG_CCACTRL1_CCA_MODE_M \
                                0x00000018  // 00: CCA always set to 1 01: CCA 
                                            // = 1 when RSSI < CCA_THR - 
                                            // CCA_HYST; CCA = 0 when RSSI >= 
                                            // CCA_THR 10: CCA = 1 when not 
                                            // receiving a frame, else CCA = 0 
                                            // 11: CCA = 1 when RSSI < CCA_THR 
                                            // - CCA_HYST and not receiving a 
                                            // frame; CCA = 0 when RSSI >= 
                                            // CCA_THR or when receiving a 
                                            // frame 

#define RFCORE_XREG_CCACTRL1_CCA_MODE_S 3
#define RFCORE_XREG_CCACTRL1_CCA_HYST_M \
                                0x00000007  // Sets the level of CCA 
                                            // hysteresis. Unsigned values 
                                            // given in dB 

#define RFCORE_XREG_CCACTRL1_CCA_HYST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RSSI register.
//
//*****************************************************************************
#define RFCORE_XREG_RSSI_RSSI_VAL_M \
                                0x000000FF  // RSSI estimate on a logarithmic 
                                            // scale, signed number on 2's 
                                            // complement Unit is 1 dB, offset 
                                            // is 73dB. The RSSI value is 
                                            // averaged over eight symbol 
                                            // periods. The RSSI_VALID status 
                                            // bit should be checked before 
                                            // reading RSSI_VAL for the first 
                                            // time. The reset value of -128 
                                            // also indicates that the RSSI 
                                            // value is invalid. 

#define RFCORE_XREG_RSSI_RSSI_VAL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RSSISTAT register.
//
//*****************************************************************************
#define RFCORE_XREG_RSSISTAT_RSSI_VALID \
                                0x00000001  // RSSI value is valid. Occurs 
                                            // eight symbol periods after 
                                            // entering RX. 

#define RFCORE_XREG_RSSISTAT_RSSI_VALID_M \
                                0x00000001
#define RFCORE_XREG_RSSISTAT_RSSI_VALID_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXFIRST register.
//
//*****************************************************************************
#define RFCORE_XREG_RXFIRST_DATA_M \
                                0x000000FF  // First byte of the RX FIFO Note: 
                                            // Reading this register does not 
                                            // modify the contents of the FIFO. 

#define RFCORE_XREG_RXFIRST_DATA_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXFIFOCNT register.
//
//*****************************************************************************
#define RFCORE_XREG_RXFIFOCNT_RXFIFOCNT_M \
                                0x000000FF  // Number of bytes in the RX FIFO 
                                            // (unsigned integer) 

#define RFCORE_XREG_RXFIFOCNT_RXFIFOCNT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_TXFIFOCNT register.
//
//*****************************************************************************
#define RFCORE_XREG_TXFIFOCNT_TXFIFOCNT_M \
                                0x000000FF  // Number of bytes in the TX FIFO 
                                            // (unsigned integer) 

#define RFCORE_XREG_TXFIFOCNT_TXFIFOCNT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXFIRST_PTR register.
//
//*****************************************************************************
#define RFCORE_XREG_RXFIRST_PTR_RXFIRST_PTR_M \
                                0x000000FF  // RAM address offset of the first 
                                            // byte in the RX FIFO 

#define RFCORE_XREG_RXFIRST_PTR_RXFIRST_PTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXLAST_PTR register.
//
//*****************************************************************************
#define RFCORE_XREG_RXLAST_PTR_RXLAST_PTR_M \
                                0x000000FF  // RAM address offset of the last 
                                            // byte + 1 byte in the RX FIFO 

#define RFCORE_XREG_RXLAST_PTR_RXLAST_PTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXP1_PTR register.
//
//*****************************************************************************
#define RFCORE_XREG_RXP1_PTR_RXP1_PTR_M \
                                0x000000FF  // RAM address offset of the first 
                                            // byte of the first frame in the 
                                            // RX FIFO 

#define RFCORE_XREG_RXP1_PTR_RXP1_PTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_TXFIRST_PTR register.
//
//*****************************************************************************
#define RFCORE_XREG_TXFIRST_PTR_TXFIRST_PTR_M \
                                0x000000FF  // RAM address offset of the next 
                                            // byte to be transmitted from the 
                                            // TX FIFO 

#define RFCORE_XREG_TXFIRST_PTR_TXFIRST_PTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_TXLAST_PTR register.
//
//*****************************************************************************
#define RFCORE_XREG_TXLAST_PTR_TXLAST_PTR_M \
                                0x000000FF  // RAM address offset of the last 
                                            // byte + 1 byte of the TX FIFO 

#define RFCORE_XREG_TXLAST_PTR_TXLAST_PTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RFIRQM0 register.
//
//*****************************************************************************
#define RFCORE_XREG_RFIRQM0_RFIRQM_M \
                                0x000000FF  // Bit mask is masking out 
                                            // interrupt sources. Bit position: 
                                            // 7: RXMASKZERO 6: RXPKTDONE 5: 
                                            // FRAME_ACCEPTED 4: 
                                            // SRC_MATCH_FOUND 3: 
                                            // SRC_MATCH_DONE 2: FIFOP 1: SFD 
                                            // 0: ACT_UNUSED 

#define RFCORE_XREG_RFIRQM0_RFIRQM_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RFIRQM1 register.
//
//*****************************************************************************
#define RFCORE_XREG_RFIRQM1_RFIRQM_M \
                                0x0000003F  // Bit mask is masking out 
                                            // interrupt sources. Bit position: 
                                            // 5: CSP_WAIT 4: CSP_STOP 3: 
                                            // CSP_MANINT 2: RF_IDLE 1: TXDONE 
                                            // 0: TXACKDONE 

#define RFCORE_XREG_RFIRQM1_RFIRQM_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RFERRM register.
//
//*****************************************************************************
#define RFCORE_XREG_RFERRM_RFERRM_M \
                                0x0000007F  // Bit mask is masking out 
                                            // interrupt sources. Bit position: 
                                            // 6: STROBE_ERR 5: TXUNDERF 4: 
                                            // TXOVERF 3: RXUNDERF 2: RXOVERF 
                                            // 1: RXABO 0: NLOCK 

#define RFCORE_XREG_RFERRM_RFERRM_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RFRND register.
//
//*****************************************************************************
#define RFCORE_XREG_RFRND_QRND  0x00000002  // Random bit from the Q channel 
                                            // of the receiver 
#define RFCORE_XREG_RFRND_QRND_M \
                                0x00000002
#define RFCORE_XREG_RFRND_QRND_S 1
#define RFCORE_XREG_RFRND_IRND  0x00000001  // Random bit from the I channel 
                                            // of the receiver 
#define RFCORE_XREG_RFRND_IRND_M \
                                0x00000001
#define RFCORE_XREG_RFRND_IRND_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_MDMCTRL0 register.
//
//*****************************************************************************
#define RFCORE_XREG_MDMCTRL0_DEM_NUM_ZEROS_M \
                                0x000000C0  // Sets how many zero symbols must 
                                            // be detected before the sync word 
                                            // when searching for sync. Only 
                                            // one zero symbol is required to 
                                            // have a correlation value above 
                                            // the correlation threshold set in 
                                            // the MDMCTRL1 register. 00: 
                                            // Reserved 01: 1 zero symbol 10: 2 
                                            // zero symbols 11: 3 zero symbols 

#define RFCORE_XREG_MDMCTRL0_DEM_NUM_ZEROS_S 6
#define RFCORE_XREG_MDMCTRL0_DEMOD_AVG_MODE \
                                0x00000020  // Defines the behavior or the 
                                            // frequency offset averaging 
                                            // filter. 0: Lock average level 
                                            // after preamble match. Restart 
                                            // frequency offset calibration 
                                            // when searching for the next 
                                            // frame. 1: Continuously update 
                                            // average level. 

#define RFCORE_XREG_MDMCTRL0_DEMOD_AVG_MODE_M \
                                0x00000020
#define RFCORE_XREG_MDMCTRL0_DEMOD_AVG_MODE_S 5
#define RFCORE_XREG_MDMCTRL0_PREAMBLE_LENGTH_M \
                                0x0000001E  // The number of preamble bytes 
                                            // (two zero-symbols) to be sent in 
                                            // TX mode before the SFD, encoded 
                                            // in steps of 2 symbols (1 byte). 
                                            // The reset value of 2 is 
                                            // compliant with IEEE 802.15.4. 
                                            // 0000: 2 leading-zero bytes 0001: 
                                            // 3 leading-zero bytes 0010: 4 
                                            // leading-zero bytes ... 1111: 17 
                                            // leading-zero bytes 

#define RFCORE_XREG_MDMCTRL0_PREAMBLE_LENGTH_S 1
#define RFCORE_XREG_MDMCTRL0_TX_FILTER \
                                0x00000001  // Defines the kind of TX filter 
                                            // that is used. The normal TX 
                                            // filter is as defined by the IEEE 
                                            // 802.15.4 standard. Extra 
                                            // filtering may be applied to 
                                            // lower the out-of-band emissions. 
                                            // 0: Normal TX filtering 1: Enable 
                                            // extra filtering 

#define RFCORE_XREG_MDMCTRL0_TX_FILTER_M \
                                0x00000001
#define RFCORE_XREG_MDMCTRL0_TX_FILTER_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_MDMCTRL1 register.
//
//*****************************************************************************
#define RFCORE_XREG_MDMCTRL1_CORR_THR_SFD \
                                0x00000020  // Defines requirements for SFD 
                                            // detection: 0: The correlation 
                                            // value of one of the zero symbols 
                                            // of the preamble must be above 
                                            // the correlation threshold. 1: 
                                            // The correlation value of one 
                                            // zero symbol of the preamble and 
                                            // both symbols in the SFD must be 
                                            // above the correlation threshold. 

#define RFCORE_XREG_MDMCTRL1_CORR_THR_SFD_M \
                                0x00000020
#define RFCORE_XREG_MDMCTRL1_CORR_THR_SFD_S 5
#define RFCORE_XREG_MDMCTRL1_CORR_THR_M \
                                0x0000001F  // Demodulator correlator 
                                            // threshold value, required before 
                                            // SFD search. Threshold value 
                                            // adjusts how the receiver 
                                            // synchronizes to data from the 
                                            // radio. If the threshold is set 
                                            // too low, sync can more easily be 
                                            // found on noise. If set too high, 
                                            // the sensitivity is reduced, but 
                                            // sync is not likely to be found 
                                            // on noise. In combination with 
                                            // DEM_NUM_ZEROS, the system can be 
                                            // tuned so sensitivity is high 
                                            // with less sync found on noise. 

#define RFCORE_XREG_MDMCTRL1_CORR_THR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FREQEST register.
//
//*****************************************************************************
#define RFCORE_XREG_FREQEST_FREQEST_M \
                                0x000000FF  // Signed 2's-complement value. 
                                            // Contains an estimate of the 
                                            // frequency offset between carrier 
                                            // and the receiver LO. The offset 
                                            // frequency is FREQEST x 7800 Hz. 
                                            // DEM_AVG_MODE controls when this 
                                            // estimate is updated. If 
                                            // DEM_AVG_MODE = 0, it is updated 
                                            // until sync is found. Then the 
                                            // frequency offset estimate is 
                                            // frozen until the end of the 
                                            // received frame. If DEM_AVG_MODE 
                                            // = 1, it is updated as long as 
                                            // the demodulator is enabled. To 
                                            // calculate the correct value, one 
                                            // must use an offset 
                                            // (FREQEST_offset), which can be 
                                            // found in the device data sheet. 
                                            // Real FREQEST value = FREQEST - 
                                            // FREQEST_offset. 

#define RFCORE_XREG_FREQEST_FREQEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RXCTRL register.
//
//*****************************************************************************
#define RFCORE_XREG_RXCTRL_GBIAS_LNA2_REF_M \
                                0x00000030  // Adjusts front-end LNA2/mixer 
                                            // PTAT current output (from M = 3 
                                            // to M = 6), default: M = 5 

#define RFCORE_XREG_RXCTRL_GBIAS_LNA2_REF_S 4
#define RFCORE_XREG_RXCTRL_GBIAS_LNA_REF_M \
                                0x0000000C  // Adjusts front-end LNA PTAT 
                                            // current output (from M = 3 to M 
                                            // = 6), default: M = 5 

#define RFCORE_XREG_RXCTRL_GBIAS_LNA_REF_S 2
#define RFCORE_XREG_RXCTRL_MIX_CURRENT_M \
                                0x00000003  // Control of the output current 
                                            // from the receiver mixers The 
                                            // current increases with 
                                            // increasing setting set. 

#define RFCORE_XREG_RXCTRL_MIX_CURRENT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FSCTRL register.
//
//*****************************************************************************
#define RFCORE_XREG_FSCTRL_PRE_CURRENT_M \
                                0x000000C0  // Prescaler current setting 

#define RFCORE_XREG_FSCTRL_PRE_CURRENT_S 6
#define RFCORE_XREG_FSCTRL_LODIV_BUF_CURRENT_TX_M \
                                0x00000030  // Adjusts current in mixer and PA 
                                            // buffers Used when TX_ACTIVE = 1 

#define RFCORE_XREG_FSCTRL_LODIV_BUF_CURRENT_TX_S 4
#define RFCORE_XREG_FSCTRL_LODIV_BUF_CURRENT_RX_M \
                                0x0000000C  // Adjusts current in mixer and PA 
                                            // buffers Used when TX_ACTIVE = 0 

#define RFCORE_XREG_FSCTRL_LODIV_BUF_CURRENT_RX_S 2
#define RFCORE_XREG_FSCTRL_LODIV_CURRENT_M \
                                0x00000003  // Adjusts divider currents, 
                                            // except mixer and PA buffers 

#define RFCORE_XREG_FSCTRL_LODIV_CURRENT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FSCAL0 register.
//
//*****************************************************************************
#define RFCORE_XREG_FSCAL0_VCO_CURR_COMP_EN_OV \
                                0x00000080  // Force on the current comparator 
                                            // in the VCO. This signal is ORed 
                                            // with the signal coming from the 
                                            // calibration module. 

#define RFCORE_XREG_FSCAL0_VCO_CURR_COMP_EN_OV_M \
                                0x00000080
#define RFCORE_XREG_FSCAL0_VCO_CURR_COMP_EN_OV_S 7
#define RFCORE_XREG_FSCAL0_CHP_DISABLE \
                                0x00000040  // Set this bit to manually 
                                            // disable charge pump by masking 
                                            // the up and down pulses from the 
                                            // phase detector. 

#define RFCORE_XREG_FSCAL0_CHP_DISABLE_M \
                                0x00000040
#define RFCORE_XREG_FSCAL0_CHP_DISABLE_S 6
#define RFCORE_XREG_FSCAL0_CHP_CURRENT_M \
                                0x0000003C  // Digital bit vector defining the 
                                            // charge-pump output current on an 
                                            // exponential scale If 
                                            // FFC_BW_BOOST = 0, the read value 
                                            // is the value stored in 
                                            // CHP_CURRENT. If FFC_BW_BOOST = 
                                            // 1, the read value is CHP_CURRENT 
                                            // + 4. If the addition causes 
                                            // overflow, the signal is 
                                            // saturated. 

#define RFCORE_XREG_FSCAL0_CHP_CURRENT_S 2
#define RFCORE_XREG_FSCAL0_BW_BOOST_MODE_M \
                                0x00000003  // Control signal Defines the 
                                            // synthesizer boost mode 00: No 
                                            // BW_BOOST 01: BW_BOOST is high 
                                            // during calibration and 
                                            // approximately 30 us into the 
                                            // settling. 10: BW_BOOST is always 
                                            // on (or high). 11: Reserved 

#define RFCORE_XREG_FSCAL0_BW_BOOST_MODE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FSCAL1 register.
//
//*****************************************************************************
#define RFCORE_XREG_FSCAL1_VCO_CURR_CAL_OE \
                                0x00000080  // Override current calibration 

#define RFCORE_XREG_FSCAL1_VCO_CURR_CAL_OE_M \
                                0x00000080
#define RFCORE_XREG_FSCAL1_VCO_CURR_CAL_OE_S 7
#define RFCORE_XREG_FSCAL1_VCO_CURR_CAL_M \
                                0x0000007C  // Calibration result Override 
                                            // value if VCO_CURR_CAL_OE = 1 

#define RFCORE_XREG_FSCAL1_VCO_CURR_CAL_S 2
#define RFCORE_XREG_FSCAL1_VCO_CURR_M \
                                0x00000003  // Defines current in VCO core 
                                            // Sets the multiplier between 
                                            // calibrated current and VCO 
                                            // current. 

#define RFCORE_XREG_FSCAL1_VCO_CURR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FSCAL2 register.
//
//*****************************************************************************
#define RFCORE_XREG_FSCAL2_VCO_CAPARR_OE \
                                0x00000040  // Override the calibration result 
                                            // with the value from 
                                            // VCO_CAPARR[5:0]. 

#define RFCORE_XREG_FSCAL2_VCO_CAPARR_OE_M \
                                0x00000040
#define RFCORE_XREG_FSCAL2_VCO_CAPARR_OE_S 6
#define RFCORE_XREG_FSCAL2_VCO_CAPARR_M \
                                0x0000003F  // VCO capacitor array setting 
                                            // Programmed during calibration 
                                            // Override value when 
                                            // VCO_CAPARR_OE = 1 

#define RFCORE_XREG_FSCAL2_VCO_CAPARR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_FSCAL3 register.
//
//*****************************************************************************
#define RFCORE_XREG_FSCAL3_VCO_DAC_EN_OV \
                                0x00000040  // Enables the VCO DAC when 1 

#define RFCORE_XREG_FSCAL3_VCO_DAC_EN_OV_M \
                                0x00000040
#define RFCORE_XREG_FSCAL3_VCO_DAC_EN_OV_S 6
#define RFCORE_XREG_FSCAL3_VCO_VC_DAC_M \
                                0x0000003C  // Bit vector for programming 
                                            // varactor control voltage from VC 
                                            // DAC 

#define RFCORE_XREG_FSCAL3_VCO_VC_DAC_S 2
#define RFCORE_XREG_FSCAL3_VCO_CAPARR_CAL_CTRL_M \
                                0x00000003  // Calibration accuracy setting 
                                            // for the cap_array calibration 
                                            // part of the calibration 00: 80 
                                            // XOSC periods 01: 100 XOSC 
                                            // periods 10: 125 XOSC periods 11: 
                                            // 250 XOSC periods 

#define RFCORE_XREG_FSCAL3_VCO_CAPARR_CAL_CTRL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_AGCCTRL0 register.
//
//*****************************************************************************
#define RFCORE_XREG_AGCCTRL0_AGC_DR_XTND_EN \
                                0x00000040  // 0: The AGC performs no 
                                            // adjustment of attenuation in the 
                                            // AAF. 1: The AGC adjusts the gain 
                                            // in the AAF to achieve extra 
                                            // dynamic range for the receiver. 

#define RFCORE_XREG_AGCCTRL0_AGC_DR_XTND_EN_M \
                                0x00000040
#define RFCORE_XREG_AGCCTRL0_AGC_DR_XTND_EN_S 6
#define RFCORE_XREG_AGCCTRL0_AGC_DR_XTND_THR_M \
                                0x0000003F  // If the measured error between 
                                            // the AGC reference magnitude and 
                                            // the actual magnitude in dB is 
                                            // larger than this threshold, the 
                                            // extra attenuation is enabled in 
                                            // the front end. This threshold 
                                            // must be set higher than 0x0C. 
                                            // This feature is enabled by 
                                            // AGC_DR_XTND_EN. 

#define RFCORE_XREG_AGCCTRL0_AGC_DR_XTND_THR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_AGCCTRL1 register.
//
//*****************************************************************************
#define RFCORE_XREG_AGCCTRL1_AGC_REF_M \
                                0x0000003F  // Target value for the AGC 
                                            // control loop, given in 1-dB 
                                            // steps 

#define RFCORE_XREG_AGCCTRL1_AGC_REF_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_AGCCTRL2 register.
//
//*****************************************************************************
#define RFCORE_XREG_AGCCTRL2_LNA1_CURRENT_M \
                                0x000000C0  // Overrride value for LNA 1 Used 
                                            // only when LNA_CURRENT_OE = 1 
                                            // When read, this register returns 
                                            // the current applied gain 
                                            // setting. 00: 0-dB gain 
                                            // (reference level) 01: 3-dB gain 
                                            // 10: Reserved 11: 6-dB gain 

#define RFCORE_XREG_AGCCTRL2_LNA1_CURRENT_S 6
#define RFCORE_XREG_AGCCTRL2_LNA2_CURRENT_M \
                                0x00000038  // Overrride value for LNA 2 Used 
                                            // only when LNA_CURRENT_OE = 1 
                                            // When read, this register returns 
                                            // the current applied gain 
                                            // setting. 000: 0-dB gain 
                                            // (reference level) 001: 3-dB gain 
                                            // 010: 6-dB gain 011: 9-dB gain 
                                            // 100: 12-dB gain 101: 15-dB gain 
                                            // 110: 18-dB gain 111: 21-dB gain 

#define RFCORE_XREG_AGCCTRL2_LNA2_CURRENT_S 3
#define RFCORE_XREG_AGCCTRL2_LNA3_CURRENT_M \
                                0x00000006  // Overrride value for LNA 3 Used 
                                            // only when LNA_CURRENT_OE = 1 
                                            // When read, this register returns 
                                            // the current applied gain 
                                            // setting. 00: 0-dB gain 
                                            // (reference level) 01: 3-dB gain 
                                            // 10: 6-dB gain 11: 9-dB gain 

#define RFCORE_XREG_AGCCTRL2_LNA3_CURRENT_S 1
#define RFCORE_XREG_AGCCTRL2_LNA_CURRENT_OE \
                                0x00000001  // Write 1 to override the AGC LNA 
                                            // current setting with the values 
                                            // above (LNA1_CURRENT, 
                                            // LNA2_CURRENT, and LNA3_CURRENT). 

#define RFCORE_XREG_AGCCTRL2_LNA_CURRENT_OE_M \
                                0x00000001
#define RFCORE_XREG_AGCCTRL2_LNA_CURRENT_OE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_AGCCTRL3 register.
//
//*****************************************************************************
#define RFCORE_XREG_AGCCTRL3_AGC_SETTLE_WAIT_M \
                                0x00000060  // Timing for AGC to wait for 
                                            // analog gain to settle after a 
                                            // gain change. During this period, 
                                            // the energy measurement in the 
                                            // AGC is paused. 00: 15 periods 
                                            // 01: 20 periods 10: 25 periods 
                                            // 11: 30 periods 

#define RFCORE_XREG_AGCCTRL3_AGC_SETTLE_WAIT_S 5
#define RFCORE_XREG_AGCCTRL3_AGC_WIN_SIZE_M \
                                0x00000018  // Window size for the 
                                            // accumulate-and-dump function in 
                                            // the AGC. 00: 16 samples 01: 32 
                                            // samples 10: 64 samples 11: 128 
                                            // samples 

#define RFCORE_XREG_AGCCTRL3_AGC_WIN_SIZE_S 3
#define RFCORE_XREG_AGCCTRL3_AAF_RP_M \
                                0x00000006  // Overrides the control signals 
                                            // of the AGC to AAF when AAF_RP_OE 
                                            // = 1. When read, it returns the 
                                            // applied signal to the AAF. 00: 
                                            // 9-dB attenuation in AAF 01: 6-dB 
                                            // attenuation in AAF 10: 3-dB 
                                            // attenuation in AAF 11: 0-dB 
                                            // attenuation in AAF (reference 
                                            // level) 

#define RFCORE_XREG_AGCCTRL3_AAF_RP_S 1
#define RFCORE_XREG_AGCCTRL3_AAF_RP_OE \
                                0x00000001  // Override the AAF control 
                                            // signals of the AGC with the 
                                            // values stored in AAF_RP. 

#define RFCORE_XREG_AGCCTRL3_AAF_RP_OE_M \
                                0x00000001
#define RFCORE_XREG_AGCCTRL3_AAF_RP_OE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_ADCTEST0 register.
//
//*****************************************************************************
#define RFCORE_XREG_ADCTEST0_ADC_VREF_ADJ_M \
                                0x000000C0  // Quantizer threshold control for 
                                            // test and debug 

#define RFCORE_XREG_ADCTEST0_ADC_VREF_ADJ_S 6
#define RFCORE_XREG_ADCTEST0_ADC_QUANT_ADJ_M \
                                0x00000030  // Quantizer threshold control for 
                                            // test and debug 

#define RFCORE_XREG_ADCTEST0_ADC_QUANT_ADJ_S 4
#define RFCORE_XREG_ADCTEST0_ADC_GM_ADJ_M \
                                0x0000000E  // Gm-control for test and debug 

#define RFCORE_XREG_ADCTEST0_ADC_GM_ADJ_S 1
#define RFCORE_XREG_ADCTEST0_ADC_DAC2_EN \
                                0x00000001  // Enables DAC2 for enhanced ADC 
                                            // stability 

#define RFCORE_XREG_ADCTEST0_ADC_DAC2_EN_M \
                                0x00000001
#define RFCORE_XREG_ADCTEST0_ADC_DAC2_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_ADCTEST1 register.
//
//*****************************************************************************
#define RFCORE_XREG_ADCTEST1_ADC_TEST_CTRL_M \
                                0x000000F0  // ADC test mode selector 

#define RFCORE_XREG_ADCTEST1_ADC_TEST_CTRL_S 4
#define RFCORE_XREG_ADCTEST1_ADC_C2_ADJ_M \
                                0x0000000C  // Used to adjust capacitor values 
                                            // in ADC 

#define RFCORE_XREG_ADCTEST1_ADC_C2_ADJ_S 2
#define RFCORE_XREG_ADCTEST1_ADC_C3_ADJ_M \
                                0x00000003  // Used to adjust capacitor values 
                                            // in ADC 

#define RFCORE_XREG_ADCTEST1_ADC_C3_ADJ_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_ADCTEST2 register.
//
//*****************************************************************************
#define RFCORE_XREG_ADCTEST2_ADC_TEST_MODE_M \
                                0x00000060  // Test mode to enable output of 
                                            // ADC data from demodulator. When 
                                            // enabled, raw ADC data is clocked 
                                            // out on the GPIO pins. 00: Test 
                                            // mode disabled 01: Data from the 
                                            // I and Q ADCs are output (data 
                                            // rate 76 MHz) 10: Data from the I 
                                            // ADC is output. Two and two ADC 
                                            // samples grouped (data rate 38 
                                            // MHz) 11: Data from the Q ADC is 
                                            // output. Two and two ADC samples 
                                            // grouped (data rate 38 MHz) 

#define RFCORE_XREG_ADCTEST2_ADC_TEST_MODE_S 5
#define RFCORE_XREG_ADCTEST2_AAF_RS_M \
                                0x00000018  // Controls series resistance of 
                                            // AAF 

#define RFCORE_XREG_ADCTEST2_AAF_RS_S 3
#define RFCORE_XREG_ADCTEST2_ADC_FF_ADJ_M \
                                0x00000006  // Adjust feed forward 

#define RFCORE_XREG_ADCTEST2_ADC_FF_ADJ_S 1
#define RFCORE_XREG_ADCTEST2_ADC_DAC_ROT \
                                0x00000001  // Control of DAC DWA scheme 0 = 
                                            // DWA (scrambling) disabled 1 = 
                                            // DWA enabled 

#define RFCORE_XREG_ADCTEST2_ADC_DAC_ROT_M \
                                0x00000001
#define RFCORE_XREG_ADCTEST2_ADC_DAC_ROT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_MDMTEST0 register.
//
//*****************************************************************************
#define RFCORE_XREG_MDMTEST0_TX_TONE_M \
                                0x000000F0  // Enables the possibility to 
                                            // transmit a baseband tone by 
                                            // picking samples from the sine 
                                            // tables with a controllable phase 
                                            // step between the samples. The 
                                            // step size is controlled by 
                                            // TX_TONE. If MDMTEST1.MOD_IF is 
                                            // 0, the tone is superpositioned 
                                            // on the modulated data, 
                                            // effectively giving modulation 
                                            // with an IF. If MDMTEST1.MOD_IF 
                                            // is 1, only the tone is 
                                            // transmitted. 0000: -6 MHz 0001: 
                                            // -4 MHz 0010: -3 MHz 0011: -2 MHz 
                                            // 0100: -1 MHz 0101: -500 kHz 
                                            // 0110: -4 kHz 0111: 0 1000: 4 kHz 
                                            // 1001: 500 kHz 1010: 1 MHz 1011: 
                                            // 2 MHz 1100: 3 MHz 1101: 4 MHz 
                                            // 1110: 6 MHz Others: Reserved 

#define RFCORE_XREG_MDMTEST0_TX_TONE_S 4
#define RFCORE_XREG_MDMTEST0_DC_WIN_SIZE_M \
                                0x0000000C  // Controls the numbers of samples 
                                            // to be accumulated between each 
                                            // dump of the accumulate-and-dump 
                                            // filter used in DC removal 00: 32 
                                            // samples 01: 64 samples 10: 128 
                                            // samples 11: 256 samples 

#define RFCORE_XREG_MDMTEST0_DC_WIN_SIZE_S 2
#define RFCORE_XREG_MDMTEST0_DC_BLOCK_MODE_M \
                                0x00000003  // Selects the mode of operation 
                                            // 00: The input signal to the DC 
                                            // blocker is passed to the output 
                                            // without any attempt to remove 
                                            // DC. 01: Enable DC cancellation. 
                                            // Normal operation 10: Freeze 
                                            // estimates of DC when sync is 
                                            // found. Resume estimating DC when 
                                            // searching for the next frame. 
                                            // 11: Reserved 

#define RFCORE_XREG_MDMTEST0_DC_BLOCK_MODE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_MDMTEST1 register.
//
//*****************************************************************************
#define RFCORE_XREG_MDMTEST1_USEMIRROR_IF \
                                0x00000020  // 0: Use the normal IF frequency 
                                            // (MDMTEST0.TX_TONE) for automatic 
                                            // IF compensation of channel 
                                            // frequency on TX. 1: Use mirror 
                                            // IF frequency for automatic 
                                            // compensation of channel 
                                            // frequency on TX. 

#define RFCORE_XREG_MDMTEST1_USEMIRROR_IF_M \
                                0x00000020
#define RFCORE_XREG_MDMTEST1_USEMIRROR_IF_S 5
#define RFCORE_XREG_MDMTEST1_MOD_IF \
                                0x00000010  // 0: Modulation is performed at 
                                            // an IF set by MDMTEST0.TX_TONE. 
                                            // The tone set by MDMTEST0.TX_TONE 
                                            // is superimposed on the data. 1: 
                                            // Modulate a tone set by 
                                            // MDMTEST0.TX_TONE. A tone is 
                                            // transmitted with frequency set 
                                            // by MDMTEST0.TX_TONE. 

#define RFCORE_XREG_MDMTEST1_MOD_IF_M \
                                0x00000010
#define RFCORE_XREG_MDMTEST1_MOD_IF_S 4
#define RFCORE_XREG_MDMTEST1_RAMP_AMP \
                                0x00000008  // 1: Enable ramping of DAC output 
                                            // amplitude during startup and 
                                            // finish. 0: Disable ramping of 
                                            // DAC output amplitude. 

#define RFCORE_XREG_MDMTEST1_RAMP_AMP_M \
                                0x00000008
#define RFCORE_XREG_MDMTEST1_RAMP_AMP_S 3
#define RFCORE_XREG_MDMTEST1_RFC_SNIFF_EN \
                                0x00000004  // 0: Packet sniffer module 
                                            // disabled 1: Packet sniffer 
                                            // module enabled. The received and 
                                            // transmitted data can be observed 
                                            // on GPIO pins. 

#define RFCORE_XREG_MDMTEST1_RFC_SNIFF_EN_M \
                                0x00000004
#define RFCORE_XREG_MDMTEST1_RFC_SNIFF_EN_S 2
#define RFCORE_XREG_MDMTEST1_LOOPBACK_EN \
                                0x00000001  // Enables loopback of modulated 
                                            // data into the receiver chain 0: 
                                            // An STXCAL instruction calibrates 
                                            // for TX. Use STXON to continue to 
                                            // active TX. 1: An STXCAL 
                                            // instruction enables the loopback 
                                            // mode. 

#define RFCORE_XREG_MDMTEST1_LOOPBACK_EN_M \
                                0x00000001
#define RFCORE_XREG_MDMTEST1_LOOPBACK_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_DACTEST0 register.
//
//*****************************************************************************
#define RFCORE_XREG_DACTEST0_DAC_Q_M \
                                0x400886FF  // Q-branch DAC override value 
                                            // when DAC_SRC = 001 If DAC_SRC is 
                                            // set to be ADC data, CORDIC 
                                            // magnitude, or channel filtered 
                                            // data, then DAC_Q_O controls the 
                                            // part of the word in question 
                                            // that is actually multiplexed to 
                                            // the DAC, as described below. 
                                            // 000111: Bits 7:0 001000: Bits 
                                            // 8:1 001001: Bits 9:2 ... If an 
                                            // invalid setting is chosen, the 
                                            // DAC outputs only zeros (minimum 
                                            // value). 

#define RFCORE_XREG_DACTEST0_DAC_Q_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_DACTEST1 register.
//
//*****************************************************************************
#define RFCORE_XREG_DACTEST1_DAC_I_M \
                                0x400886FF  // I-branch DAC override value 
                                            // when DAC_SRC = 001 If DAC_SRC is 
                                            // set to be ADC data, CORDIC 
                                            // magnitude, channel filtered 
                                            // data, or DC filtered data, then 
                                            // DAC_I_O controls the part of the 
                                            // word in question that is 
                                            // actually multiplexed to the DAC 
                                            // as described below. 000111: Bits 
                                            // 7:0 001000: Bits 8:1 001001: 
                                            // Bits 9:2 ... If an invalid 
                                            // setting is chosen, then the DAC 
                                            // outputs only zeros (minimum 
                                            // value). 

#define RFCORE_XREG_DACTEST1_DAC_I_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_DACTEST2 register.
//
//*****************************************************************************
#define RFCORE_XREG_DACTEST2_DAC_DEM_EN \
                                0x00000020  // Enable and disable dynamic 
                                            // element matching Drives 
                                            // RFR_DAC_DEM_EN 

#define RFCORE_XREG_DACTEST2_DAC_DEM_EN_M \
                                0x00000020
#define RFCORE_XREG_DACTEST2_DAC_DEM_EN_S 5
#define RFCORE_XREG_DACTEST2_DAC_CASC_CTRL_M \
                                0x00000018  // Adjustment of output stage 
                                            // Drives RFR_DAC_CASC_CTRL 

#define RFCORE_XREG_DACTEST2_DAC_CASC_CTRL_S 3
#define RFCORE_XREG_DACTEST2_DAC_SRC_M \
                                0x00000007  // The TX DACs data source is 
                                            // selected by DAC_SRC according 
                                            // to: 000: Normal operation (from 
                                            // modulator) 001: The DAC_I_O and 
                                            // DAC_Q_O override values 010: ADC 
                                            // data after decimation, magnitude 
                                            // controlled by DAC_I_O and 
                                            // DAC_Q_O 011: I/Q after 
                                            // decimation, channel and DC 
                                            // filtering, magnitude controlled 
                                            // by DAC_I_O and DAC_Q_O 100: 
                                            // CORDIC magnitude output and 
                                            // front-end gain is output, 
                                            // magnitude controlled by DAC_I_O 
                                            // and DAC_Q_O 101: RSSI I output 
                                            // on the I DAC 111: Reserved 

#define RFCORE_XREG_DACTEST2_DAC_SRC_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_ATEST register.
//
//*****************************************************************************
#define RFCORE_XREG_ATEST_ATEST_CTRL_M \
                                0x0000003F  // Controls the analog test mode: 
                                            // 00 0000: Disabled 00 0001: 
                                            // Enables the temperature sensor 
                                            // (see also the CCTEST_TR0 
                                            // register description). Other 
                                            // values reserved. 

#define RFCORE_XREG_ATEST_ATEST_CTRL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_PTEST0 register.
//
//*****************************************************************************
#define RFCORE_XREG_PTEST0_PRE_PD \
                                0x00000080  // Prescaler power-down signal 
                                            // When PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST0_PRE_PD_M \
                                0x00000080
#define RFCORE_XREG_PTEST0_PRE_PD_S 7
#define RFCORE_XREG_PTEST0_CHP_PD \
                                0x00000040  // Charge pump power-down signal 
                                            // When PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST0_CHP_PD_M \
                                0x00000040
#define RFCORE_XREG_PTEST0_CHP_PD_S 6
#define RFCORE_XREG_PTEST0_ADC_PD \
                                0x00000020  // ADC power-down signal When 
                                            // PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST0_ADC_PD_M \
                                0x00000020
#define RFCORE_XREG_PTEST0_ADC_PD_S 5
#define RFCORE_XREG_PTEST0_DAC_PD \
                                0x00000010  // DAC power-down signal When 
                                            // PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST0_DAC_PD_M \
                                0x00000010
#define RFCORE_XREG_PTEST0_DAC_PD_S 4
#define RFCORE_XREG_PTEST0_LNA_PD_M \
                                0x0000000C  // Low-noise amplifier power-down 
                                            // signal Defines LNA/mixer 
                                            // power-down modes: 00: Power up 
                                            // 01: LNA off, mixer/regulator on 
                                            // 10: LNA/mixer off, regulator on 
                                            // 11: PD When PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST0_LNA_PD_S 2
#define RFCORE_XREG_PTEST0_TXMIX_PD \
                                0x00000002  // Transmit mixer power-down 
                                            // signal When PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST0_TXMIX_PD_M \
                                0x00000002
#define RFCORE_XREG_PTEST0_TXMIX_PD_S 1
#define RFCORE_XREG_PTEST0_AAF_PD \
                                0x00000001  // Antialiasing filter power-down 
                                            // signal When PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST0_AAF_PD_M \
                                0x00000001
#define RFCORE_XREG_PTEST0_AAF_PD_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_PTEST1 register.
//
//*****************************************************************************
#define RFCORE_XREG_PTEST1_PD_OVERRIDE \
                                0x00000008  // Override enabling and disabling 
                                            // of various modules (for debug 
                                            // and testing only) It is 
                                            // impossible to override 
                                            // hard-coded BIAS_PD[1:0] 
                                            // depenancy. 

#define RFCORE_XREG_PTEST1_PD_OVERRIDE_M \
                                0x00000008
#define RFCORE_XREG_PTEST1_PD_OVERRIDE_S 3
#define RFCORE_XREG_PTEST1_PA_PD \
                                0x00000004  // Power amplifier power-down 
                                            // signal When PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST1_PA_PD_M \
                                0x00000004
#define RFCORE_XREG_PTEST1_PA_PD_S 2
#define RFCORE_XREG_PTEST1_VCO_PD \
                                0x00000002  // VCO power-down signal When 
                                            // PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST1_VCO_PD_M \
                                0x00000002
#define RFCORE_XREG_PTEST1_VCO_PD_S 1
#define RFCORE_XREG_PTEST1_LODIV_PD \
                                0x00000001  // LO power-down signal When 
                                            // PD_OVERRIDE = 1 

#define RFCORE_XREG_PTEST1_LODIV_PD_M \
                                0x00000001
#define RFCORE_XREG_PTEST1_LODIV_PD_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG0 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG0_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG0_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG1 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG1_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG1_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG2 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG2_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG2_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG3 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG3_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG3_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG4 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG4_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG4_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG5 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG5_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG5_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG6 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG6_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG6_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG7 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG7_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG7_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG8 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG8_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG8_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG9 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG9_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG9_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG10 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG10_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG10_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG11 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG11_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG11_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG12 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG12_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG12_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG13 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG13_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG13_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG14 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG14_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG14_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG15 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG15_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG15_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG16 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG16_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG16_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG17 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG17_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG17_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG18 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG18_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG18_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG19 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG19_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG19_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG20 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG20_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG20_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG21 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG21_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG21_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG22 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG22_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG22_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPPROG23 register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPPROG23_CSP_INSTR_M \
                                0x000000FF  // Byte N of the CSP program 
                                            // memory 

#define RFCORE_XREG_CSPPROG23_CSP_INSTR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPCTRL register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPCTRL_MCU_CTRL \
                                0x00000001  // CSP MCU control input 

#define RFCORE_XREG_CSPCTRL_MCU_CTRL_M \
                                0x00000001
#define RFCORE_XREG_CSPCTRL_MCU_CTRL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPSTAT register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPSTAT_CSP_RUNNING \
                                0x00000020  // 1: CSP is running. 0: CSP is 
                                            // idle. 

#define RFCORE_XREG_CSPSTAT_CSP_RUNNING_M \
                                0x00000020
#define RFCORE_XREG_CSPSTAT_CSP_RUNNING_S 5
#define RFCORE_XREG_CSPSTAT_CSP_PC_M \
                                0x0000001F  // CSP program counter 

#define RFCORE_XREG_CSPSTAT_CSP_PC_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPX register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPX_CSPX_M 0x000000FF  // Used by CSP instructions WAITX, 
                                            // RANDXY, INCX, DECX, and 
                                            // conditional instructions. 
#define RFCORE_XREG_CSPX_CSPX_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPY register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPY_CSPY_M 0x000000FF  // Used by CSP instructions 
                                            // RANDXY, INCY, DECY, and 
                                            // conditional instructions. 
#define RFCORE_XREG_CSPY_CSPY_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPZ register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPZ_CSPZ_M 0x000000FF  // Used by CSP instructions INCZ, 
                                            // DECZ, and conditional 
                                            // instructions. 
#define RFCORE_XREG_CSPZ_CSPZ_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_CSPT register.
//
//*****************************************************************************
#define RFCORE_XREG_CSPT_CSPT_M 0x000000FF  // Content is decremented each 
                                            // time the MAC Timer overflows 
                                            // while the CSP program is 
                                            // running. The SCP program stops 
                                            // when decremented to 0. Setting 
                                            // CSPT = 0xFF prevents the 
                                            // register from being decremented. 
#define RFCORE_XREG_CSPT_CSPT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RFC_OBS_CTRL0 register.
//
//*****************************************************************************
#define RFCORE_XREG_RFC_OBS_CTRL0_RFC_OBS_POL0 \
                                0x00000040  // The signal chosen by 
                                            // RFC_OBS_MUX0 is XORed with this 
                                            // bit. 

#define RFCORE_XREG_RFC_OBS_CTRL0_RFC_OBS_POL0_M \
                                0x00000040
#define RFCORE_XREG_RFC_OBS_CTRL0_RFC_OBS_POL0_S 6
#define RFCORE_XREG_RFC_OBS_CTRL0_RFC_OBS_MUX0_M \
                                0x0000003F  // Controls which observable 
                                            // signal from RF Core is to be 
                                            // muxed out to rfc_obs_sigs[0]. 00 
                                            // 0000: 0 - Constant value 00 
                                            // 0001: 1 - Constant value 00 
                                            // 1000: rfc_sniff_data - Data from 
                                            // packet sniffer. Sample data on 
                                            // rising edges of sniff_clk. 00 
                                            // 1001: rfc_sniff_clk - 250kHz 
                                            // clock for packet sniffer data. 
                                            // 00 1100: rssi_valid - Pin is 
                                            // high when the RSSI value has 
                                            // been updated at least once since 
                                            // RX was started. Cleared when 
                                            // leaving RX. 00 1101: demod_cca - 
                                            // Clear channel assessment. See 
                                            // FSMSTAT1 register for details on 
                                            // how to configure the behavior of 
                                            // this signal. 00 1110: 
                                            // sampled_cca - A sampled version 
                                            // of the CCA bit from demodulator. 
                                            // The value is updated whenever a 
                                            // SSAMPLECCA or STXONCCA strobe is 
                                            // issued. 00 1111: sfd_sync - Pin 
                                            // is high when a SFD has been 
                                            // received or transmitted. Cleared 
                                            // when leaving RX/TX respectively. 
                                            // Not to be confused with the SFD 
                                            // exception. 01 0000: tx_active - 
                                            // Indicates that FFCTRL is in one 
                                            // of the TX states. Active-high. 
                                            // Note: This signal might have 
                                            // glitches, because it has no 
                                            // output flip-flop and is based on 
                                            // the current state register of 
                                            // the FFCTRL FSM. 01 0001: 
                                            // rx_active - Indicates that 
                                            // FFCTRL is in one of the RX 
                                            // states. Active-high. Note: This 
                                            // signal might have glitches, 
                                            // because it has no output 
                                            // flip-flop and is based on the 
                                            // current state register of the 
                                            // FFCTRL FSM. 01 0010: ffctrl_fifo 
                                            // - Pin is high when one or more 
                                            // bytes are in the RXFIFO. Low 
                                            // during RXFIFO overflow. 01 0011: 
                                            // ffctrl_fifop - Pin is high when 
                                            // the number of bytes in the 
                                            // RXFIFO exceeds the programmable 
                                            // threshold or at least one 
                                            // complete frame is in the RXFIFO. 
                                            // Also high during RXFIFO 
                                            // overflow. Not to be confused 
                                            // with the FIFOP exception. 01 
                                            // 0100: packet_done - A complete 
                                            // frame has been received. I.e., 
                                            // the number of bytes set by the 
                                            // frame-length field has been 
                                            // received. 01 0110: 
                                            // rfc_xor_rand_i_q - XOR between I 
                                            // and Q random outputs. Updated at 
                                            // 8 MHz. 01 0111: rfc_rand_q - 
                                            // Random data output from the Q 
                                            // channel of the receiver. Updated 
                                            // at 8 MHz. 01 1000: rfc_rand_i - 
                                            // Random data output from the I 
                                            // channel of the receiver. Updated 
                                            // at 8 MHz 01 1001: lock_status - 
                                            // 1 when PLL is in lock, otherwise 
                                            // 0 10 1000: pa_pd - Power 
                                            // amplifier power-down signal 10 
                                            // 1010: lna_pd - LNA power-down 
                                            // signal Others: Reserved 

#define RFCORE_XREG_RFC_OBS_CTRL0_RFC_OBS_MUX0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RFC_OBS_CTRL1 register.
//
//*****************************************************************************
#define RFCORE_XREG_RFC_OBS_CTRL1_RFC_OBS_POL1 \
                                0x00000040  // The signal chosen by 
                                            // RFC_OBS_MUX1 is XORed with this 
                                            // bit. 

#define RFCORE_XREG_RFC_OBS_CTRL1_RFC_OBS_POL1_M \
                                0x00000040
#define RFCORE_XREG_RFC_OBS_CTRL1_RFC_OBS_POL1_S 6
#define RFCORE_XREG_RFC_OBS_CTRL1_RFC_OBS_MUX1_M \
                                0x0000003F  // Controls which observable 
                                            // signal from RF Core is to be 
                                            // muxed out to rfc_obs_sigs[1]. 
                                            // See description of RFC_OBS_CTRL0 
                                            // for details. 

#define RFCORE_XREG_RFC_OBS_CTRL1_RFC_OBS_MUX1_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_RFC_OBS_CTRL2 register.
//
//*****************************************************************************
#define RFCORE_XREG_RFC_OBS_CTRL2_RFC_OBS_POL2 \
                                0x00000040  // The signal chosen by 
                                            // RFC_OBS_MUX2 is XORed with this 
                                            // bit. 

#define RFCORE_XREG_RFC_OBS_CTRL2_RFC_OBS_POL2_M \
                                0x00000040
#define RFCORE_XREG_RFC_OBS_CTRL2_RFC_OBS_POL2_S 6
#define RFCORE_XREG_RFC_OBS_CTRL2_RFC_OBS_MUX2_M \
                                0x0000003F  // Controls which observable 
                                            // signal from RF Core is to be 
                                            // muxed out to rfc_obs_sigs[2]. 
                                            // See description of RFC_OBS_CTRL0 
                                            // for details. 

#define RFCORE_XREG_RFC_OBS_CTRL2_RFC_OBS_MUX2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_XREG_TXFILTCFG register.
//
//*****************************************************************************
#define RFCORE_XREG_TXFILTCFG_FC_M \
                                0x0000000F  // Drives signal rfr_txfilt_fc 

#define RFCORE_XREG_TXFILTCFG_FC_S 0


#endif // __HW_RFCORE_XREG_H__

