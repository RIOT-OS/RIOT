/******************************************************************************
*  Filename:       hw_aes.h
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

#ifndef __HW_AES_H__
#define __HW_AES_H__

//*****************************************************************************
//
// The following are defines for the AES register offsets.
//
//*****************************************************************************
#define AES_DMAC_CH0_CTRL       0x4008B000  // Channel control This register 
                                            // is used for channel enabling and 
                                            // priority selection. When a 
                                            // channel is disabled, it becomes 
                                            // inactive only when all ongoing 
                                            // requests are finished. 
#define AES_DMAC_CH0_EXTADDR    0x4008B004  // Channel external address 
#define AES_DMAC_CH0_DMALENGTH \
                                0x4008B00C  // Channel DMA length 

#define AES_DMAC_STATUS         0x4008B018  // DMAC status This register 
                                            // provides the actual state of 
                                            // each DMA channel. It also 
                                            // reports port errors in case 
                                            // these were received by the 
                                            // master interface module during 
                                            // the data transfer. 
#define AES_DMAC_SWRES          0x4008B01C  // DMAC software reset register 
                                            // Software reset is used to reset 
                                            // the DMAC to stop all transfers 
                                            // and clears the port error status 
                                            // register. After the software 
                                            // reset is performed, all the 
                                            // channels are disabled and no new 
                                            // requests are performed by the 
                                            // channels. The DMAC waits for the 
                                            // existing (active) requests to 
                                            // finish and accordingly sets the 
                                            // DMAC status registers. 
#define AES_DMAC_CH1_CTRL       0x4008B020  // Channel control This register 
                                            // is used for channel enabling and 
                                            // priority selection. When a 
                                            // channel is disabled, it becomes 
                                            // inactive only when all ongoing 
                                            // requests are finished. 
#define AES_DMAC_CH1_EXTADDR    0x4008B024  // Channel external address 
#define AES_DMAC_CH1_DMALENGTH \
                                0x4008B02C  // Channel DMA length 

#define AES_DMAC_MST_RUNPARAMS \
                                0x4008B078  // DMAC master run-time parameters 
                                            // This register defines all the 
                                            // run-time parameters for the AHB 
                                            // master interface port. These 
                                            // parameters are required for the 
                                            // proper functioning of the 
                                            // EIP-101m AHB master adapter. 

#define AES_DMAC_PERSR          0x4008B07C  // DMAC port error raw status 
                                            // register This register provides 
                                            // the actual status of individual 
                                            // port errors. It also indicates 
                                            // which channel is serviced by an 
                                            // external AHB port (which is 
                                            // frozen by a port error). A port 
                                            // error aborts operations on all 
                                            // serviced channels (channel 
                                            // enable bit is forced to 0) and 
                                            // prevents further transfers via 
                                            // that port until the error is 
                                            // cleared by writing to the 
                                            // DMAC_SWRES register. 
#define AES_DMAC_OPTIONS        0x4008B0F8  // DMAC options register These 
                                            // registers contain information 
                                            // regarding the different options 
                                            // configured in this DMAC. 
#define AES_DMAC_VERSION        0x4008B0FC  // DMAC version register This 
                                            // register contains an indication 
                                            // (or signature) of the EIP type 
                                            // of this DMAC, as well as the 
                                            // hardware version/patch numbers. 
#define AES_KEY_STORE_WRITE_AREA \
                                0x4008B400  // Key store write area register 
                                            // This register defines where the 
                                            // keys should be written in the 
                                            // key store RAM. After writing 
                                            // this register, the key store 
                                            // module is ready to receive the 
                                            // keys through a DMA operation. In 
                                            // case the key data transfer 
                                            // triggered an error in the key 
                                            // store, the error will be 
                                            // available in the interrupt 
                                            // status register after the DMA is 
                                            // finished. The key store 
                                            // write-error is asserted when the 
                                            // programmed/selected area is not 
                                            // completely written. This error 
                                            // is also asserted when the DMA 
                                            // operation writes to ram areas 
                                            // that are not selected. The key 
                                            // store RAM is divided into 8 
                                            // areas of 128 bits. 192-bit keys 
                                            // written in the key store RAM 
                                            // should start on boundaries of 
                                            // 256 bits. This means that 
                                            // writing a 192-bit key to the key 
                                            // store RAM must be done by 
                                            // writing 256 bits of data with 
                                            // the 64 most-significant bits set 
                                            // to 0. These bits are ignored by 
                                            // the AES engine. 

#define AES_KEY_STORE_WRITTEN_AREA \
                                0x4008B404  // Key store written area register 
                                            // This register shows which areas 
                                            // of the key store RAM contain 
                                            // valid written keys. When a new 
                                            // key needs to be written to the 
                                            // key store, on a location that is 
                                            // already occupied by a valid key, 
                                            // this key area must be cleared 
                                            // first. This can be done by 
                                            // writing this register before the 
                                            // new key is written to the key 
                                            // store memory. Attempting to 
                                            // write to a key area that already 
                                            // contains a valid key is not 
                                            // allowed and results in an error. 

#define AES_KEY_STORE_SIZE      0x4008B408  // Key store size register This 
                                            // register defines the size of the 
                                            // keys that are written with DMA. 
                                            // This register should be 
                                            // configured before writing to the 
                                            // KEY_STORE_WRITE_AREA register. 
#define AES_KEY_STORE_READ_AREA \
                                0x4008B40C  // Key store read area register 
                                            // This register selects the key 
                                            // store RAM area from where the 
                                            // key needs to be read that will 
                                            // be used for an AES operation. 
                                            // The operation directly starts 
                                            // after writing this register. 
                                            // When the operation is finished, 
                                            // the status of the key store read 
                                            // operation is available in the 
                                            // interrupt status register. Key 
                                            // store read error is asserted 
                                            // when a RAM area is selected 
                                            // which does not contain valid 
                                            // written key. 

#define AES_AES_KEY2_0          0x4008B500  // AES_KEY2_0 / AES_GHASH_H_IN_0 
                                            // Second Key / GHASH Key 
                                            // (internal, but clearable) The 
                                            // following registers are not 
                                            // accessible through the host for 
                                            // reading and writing. They are 
                                            // used to store internally 
                                            // calculated key information and 
                                            // intermediate results. However, 
                                            // when the host performs a write 
                                            // to the any of the respective 
                                            // AES_KEY2_n or AES_KEY3_n 
                                            // addresses, respectively the 
                                            // whole 128-bit AES_KEY2_n or 
                                            // AES_KEY3_n register is cleared 
                                            // to 0s. The AES_GHASH_H_IN_n 
                                            // registers (required for GHASH, 
                                            // which is part of GCM) are mapped 
                                            // to the AES_KEY2_n registers. The 
                                            // (intermediate) authentication 
                                            // result for GCM and CCM is stored 
                                            // in the AES_KEY3_n register. 
#define AES_AES_KEY2_1          0x4008B504  // AES_KEY2_1 / AES_GHASH_H_IN_1 
                                            // Second Key / GHASH Key 
                                            // (internal, but clearable) The 
                                            // following registers are not 
                                            // accessible through the host for 
                                            // reading and writing. They are 
                                            // used to store internally 
                                            // calculated key information and 
                                            // intermediate results. However, 
                                            // when the host performs a write 
                                            // to the any of the respective 
                                            // AES_KEY2_n or AES_KEY3_n 
                                            // addresses, respectively the 
                                            // whole 128-bit AES_KEY2_n or 
                                            // AES_KEY3_n register is cleared 
                                            // to 0s. The AES_GHASH_H_IN_n 
                                            // registers (required for GHASH, 
                                            // which is part of GCM) are mapped 
                                            // to the AES_KEY2_n registers. The 
                                            // (intermediate) authentication 
                                            // result for GCM and CCM is stored 
                                            // in the AES_KEY3_n register. 
#define AES_AES_KEY2_2          0x4008B508  // AES_KEY2_2 / AES_GHASH_H_IN_2 
                                            // Second Key / GHASH Key 
                                            // (internal, but clearable) The 
                                            // following registers are not 
                                            // accessible through the host for 
                                            // reading and writing. They are 
                                            // used to store internally 
                                            // calculated key information and 
                                            // intermediate results. However, 
                                            // when the host performs a write 
                                            // to the any of the respective 
                                            // AES_KEY2_n or AES_KEY3_n 
                                            // addresses, respectively the 
                                            // whole 128-bit AES_KEY2_n or 
                                            // AES_KEY3_n register is cleared 
                                            // to 0s. The AES_GHASH_H_IN_n 
                                            // registers (required for GHASH, 
                                            // which is part of GCM) are mapped 
                                            // to the AES_KEY2_n registers. The 
                                            // (intermediate) authentication 
                                            // result for GCM and CCM is stored 
                                            // in the AES_KEY3_n register. 
#define AES_AES_KEY2_3          0x4008B50C  // AES_KEY2_3 / AES_GHASH_H_IN_3 
                                            // Second Key / GHASH Key 
                                            // (internal, but clearable) The 
                                            // following registers are not 
                                            // accessible through the host for 
                                            // reading and writing. They are 
                                            // used to store internally 
                                            // calculated key information and 
                                            // intermediate results. However, 
                                            // when the host performs a write 
                                            // to the any of the respective 
                                            // AES_KEY2_n or AES_KEY3_n 
                                            // addresses, respectively the 
                                            // whole 128-bit AES_KEY2_n or 
                                            // AES_KEY3_n register is cleared 
                                            // to 0s. The AES_GHASH_H_IN_n 
                                            // registers (required for GHASH, 
                                            // which is part of GCM) are mapped 
                                            // to the AES_KEY2_n registers. The 
                                            // (intermediate) authentication 
                                            // result for GCM and CCM is stored 
                                            // in the AES_KEY3_n register. 
#define AES_AES_KEY3_0          0x4008B510  // AES_KEY3_0 / AES_KEY2_4 Third 
                                            // Key / Second Key (internal, but 
                                            // clearable) The following 
                                            // registers are not accessible 
                                            // through the host for reading and 
                                            // writing. They are used to store 
                                            // internally calculated key 
                                            // information and intermediate 
                                            // results. However, when the host 
                                            // performs a write to the any of 
                                            // the respective AES_KEY2_n or 
                                            // AES_KEY3_n addresses, 
                                            // respectively the whole 128-bit 
                                            // AES_KEY2_n or AES_KEY3_n 
                                            // register is cleared to 0s. The 
                                            // AES_GHASH_H_IN_n registers 
                                            // (required for GHASH, which is 
                                            // part of GCM) are mapped to the 
                                            // AES_KEY2_n registers. The 
                                            // (intermediate) authentication 
                                            // result for GCM and CCM is stored 
                                            // in the AES_KEY3_n register. 
#define AES_AES_KEY3_1          0x4008B514  // AES_KEY3_1 / AES_KEY2_5 Third 
                                            // Key / Second Key (internal, but 
                                            // clearable) The following 
                                            // registers are not accessible 
                                            // through the host for reading and 
                                            // writing. They are used to store 
                                            // internally calculated key 
                                            // information and intermediate 
                                            // results. However, when the host 
                                            // performs a write to the any of 
                                            // the respective AES_KEY2_n or 
                                            // AES_KEY3_n addresses, 
                                            // respectively the whole 128-bit 
                                            // AES_KEY2_n or AES_KEY3_n 
                                            // register is cleared to 0s. The 
                                            // AES_GHASH_H_IN_n registers 
                                            // (required for GHASH, which is 
                                            // part of GCM) are mapped to the 
                                            // AES_KEY2_n registers. The 
                                            // (intermediate) authentication 
                                            // result for GCM and CCM is stored 
                                            // in the AES_KEY3_n register. 
#define AES_AES_KEY3_2          0x4008B518  // AES_KEY3_2 / AES_KEY2_6 Third 
                                            // Key / Second Key (internal, but 
                                            // clearable) The following 
                                            // registers are not accessible 
                                            // through the host for reading and 
                                            // writing. They are used to store 
                                            // internally calculated key 
                                            // information and intermediate 
                                            // results. However, when the host 
                                            // performs a write to the any of 
                                            // the respective AES_KEY2_n or 
                                            // AES_KEY3_n addresses, 
                                            // respectively the whole 128-bit 
                                            // AES_KEY2_n or AES_KEY3_n 
                                            // register is cleared to 0s. The 
                                            // AES_GHASH_H_IN_n registers 
                                            // (required for GHASH, which is 
                                            // part of GCM) are mapped to the 
                                            // AES_KEY2_n registers. The 
                                            // (intermediate) authentication 
                                            // result for GCM and CCM is stored 
                                            // in the AES_KEY3_n register. 
#define AES_AES_KEY3_3          0x4008B51C  // AES_KEY3_3 / AES_KEY2_7 Third 
                                            // Key / Second Key (internal, but 
                                            // clearable) The following 
                                            // registers are not accessible 
                                            // through the host for reading and 
                                            // writing. They are used to store 
                                            // internally calculated key 
                                            // information and intermediate 
                                            // results. However, when the host 
                                            // performs a write to the any of 
                                            // the respective AES_KEY2_n or 
                                            // AES_KEY3_n addresses, 
                                            // respectively the whole 128-bit 
                                            // AES_KEY2_n or AES_KEY3_n 
                                            // register is cleared to 0s. The 
                                            // AES_GHASH_H_IN_n registers 
                                            // (required for GHASH, which is 
                                            // part of GCM) are mapped to the 
                                            // AES_KEY2_n registers. The 
                                            // (intermediate) authentication 
                                            // result for GCM and CCM is stored 
                                            // in the AES_KEY3_n register. 
#define AES_AES_IV_0            0x4008B540  // AES initialization vector 
                                            // registers These registers are 
                                            // used to provide and read the IV 
                                            // from the AES engine. 
#define AES_AES_IV_1            0x4008B544  // AES initialization vector 
                                            // registers These registers are 
                                            // used to provide and read the IV 
                                            // from the AES engine. 
#define AES_AES_IV_2            0x4008B548  // AES initialization vector 
                                            // registers These registers are 
                                            // used to provide and read the IV 
                                            // from the AES engine. 
#define AES_AES_IV_3            0x4008B54C  // AES initialization vector 
                                            // registers These registers are 
                                            // used to provide and read the IV 
                                            // from the AES engine. 
#define AES_AES_CTRL            0x4008B550  // AES input/output buffer control 
                                            // and mode register This register 
                                            // specifies the AES mode of 
                                            // operation for the EIP-120t. 
                                            // Electronic codebook (ECB) mode 
                                            // is automatically selected if 
                                            // bits [28:5] of this register are 
                                            // all 0. 
#define AES_AES_C_LENGTH_0      0x4008B554  // AES crypto length registers 
                                            // (LSW) These registers are used 
                                            // to write the Length values to 
                                            // the EIP-120t. While processing, 
                                            // the length values decrement to 
                                            // 0. If both lengths are 0, the 
                                            // data stream is finished and a 
                                            // new context is requested. For 
                                            // basic AES modes (ECB, CBC, and 
                                            // CTR), a crypto length of 0 can 
                                            // be written if multiple streams 
                                            // need to be processed with the 
                                            // same key. Writing 0 length 
                                            // results in continued data 
                                            // requests until a new context is 
                                            // written. For the other modes 
                                            // (CBC-MAC, GCM, and CCM) no (new) 
                                            // data requests are done if the 
                                            // length decrements to or equals 
                                            // 0. It is advised to write a new 
                                            // length per packet. If the length 
                                            // registers decrement to 0, no new 
                                            // data is processed until a new 
                                            // context or length value is 
                                            // written. When writing a new mode 
                                            // without writing the length 
                                            // registers, the length register 
                                            // values from the previous context 
                                            // is reused. 
#define AES_AES_C_LENGTH_1      0x4008B558  // AES crypto length registers 
                                            // (MSW) These registers are used 
                                            // to write the Length values to 
                                            // the EIP-120t. While processing, 
                                            // the length values decrement to 
                                            // 0. If both lengths are 0, the 
                                            // data stream is finished and a 
                                            // new context is requested. For 
                                            // basic AES modes (ECB, CBC, and 
                                            // CTR), a crypto length of 0 can 
                                            // be written if multiple streams 
                                            // need to be processed with the 
                                            // same key. Writing 0 length 
                                            // results in continued data 
                                            // requests until a new context is 
                                            // written. For the other modes 
                                            // (CBC-MAC, GCM and CCM) no (new) 
                                            // data requests are done if the 
                                            // length decrements to or equals 
                                            // 0. It is advised to write a new 
                                            // length per packet. If the length 
                                            // registers decrement to 0, no new 
                                            // data is processed until a new 
                                            // context or length value is 
                                            // written. When writing a new mode 
                                            // without writing the length 
                                            // registers, the length register 
                                            // values from the previous context 
                                            // is reused. 
#define AES_AES_AUTH_LENGTH     0x4008B55C  // Authentication length register 
#define AES_AES_DATA_IN_OUT_0   0x4008B560  // Data input/output registers The 
                                            // data registers are typically 
                                            // accessed through the DMA and not 
                                            // with host writes and/or reads. 
                                            // However, for debugging purposes 
                                            // the data input/output registers 
                                            // can be accessed via host write 
                                            // and read operations. The 
                                            // registers are used to buffer the 
                                            // input/output data blocks to/from 
                                            // the EIP-120t. Note: The data 
                                            // input buffer (AES_DATA_IN_n) and 
                                            // data output buffer 
                                            // (AES_DATA_OUT_n) are mapped to 
                                            // the same address locations. 
                                            // Writes (both DMA and host) to 
                                            // these addresses load the Input 
                                            // Buffer while reads pull from the 
                                            // Output Buffer. Therefore, for 
                                            // write access, the data input 
                                            // buffer is written; for read 
                                            // access, the data output buffer 
                                            // is read. The data input buffer 
                                            // must be written before starting 
                                            // an operation. The data output 
                                            // buffer contains valid data on 
                                            // completion of an operation. 
                                            // Therefore, any 128-bit data 
                                            // block can be split over multiple 
                                            // 32-bit word transfers; these can 
                                            // be mixed with other host 
                                            // transfers over the external 
                                            // interface. 
#define AES_AES_DATA_IN_OUT_1   0x4008B564  // Data Input/Output Registers The 
                                            // data registers are typically 
                                            // accessed via DMA and not with 
                                            // host writes and/or reads. 
                                            // However, for debugging purposes 
                                            // the Data Input/Output Registers 
                                            // can be accessed via host write 
                                            // and read operations. The 
                                            // registers are used to buffer the 
                                            // input/output data blocks to/from 
                                            // the EIP-120t. Note: The data 
                                            // input buffer (AES_DATA_IN_n) and 
                                            // data output buffer 
                                            // (AES_DATA_OUT_n) are mapped to 
                                            // the same address locations. 
                                            // Writes (both DMA and host) to 
                                            // these addresses load the Input 
                                            // Buffer while reads pull from the 
                                            // Output Buffer. Therefore, for 
                                            // write access, the data input 
                                            // buffer is written; for read 
                                            // access, the data output buffer 
                                            // is read. The data input buffer 
                                            // must be written before starting 
                                            // an operation. The data output 
                                            // buffer contains valid data on 
                                            // completion of an operation. 
                                            // Therefore, any 128-bit data 
                                            // block can be split over multiple 
                                            // 32-bit word transfers; these can 
                                            // be mixed with other host 
                                            // transfers over the external 
                                            // interface. 
#define AES_AES_DATA_IN_OUT_2   0x4008B568  // Data Input/Output Registers The 
                                            // data registers are typically 
                                            // accessed via DMA and not with 
                                            // host writes and/or reads. 
                                            // However, for debugging purposes 
                                            // the Data Input/Output Registers 
                                            // can be accessed via host write 
                                            // and read operations. The 
                                            // registers are used to buffer the 
                                            // input/output data blocks to/from 
                                            // the EIP-120t. Note: The data 
                                            // input buffer (AES_DATA_IN_n) and 
                                            // data output buffer 
                                            // (AES_DATA_OUT_n) are mapped to 
                                            // the same address locations. 
                                            // Writes (both DMA and host) to 
                                            // these addresses load the Input 
                                            // Buffer while reads pull from the 
                                            // Output Buffer. Therefore, for 
                                            // write access, the data input 
                                            // buffer is written; for read 
                                            // access, the data output buffer 
                                            // is read. The data input buffer 
                                            // must be written before starting 
                                            // an operation. The data output 
                                            // buffer contains valid data on 
                                            // completion of an operation. 
                                            // Therefore, any 128-bit data 
                                            // block can be split over multiple 
                                            // 32-bit word transfers; these can 
                                            // be mixed with other host 
                                            // transfers over the external 
                                            // interface. 
#define AES_AES_DATA_IN_OUT_3   0x4008B56C  // Data Input/Output Registers The 
                                            // data registers are typically 
                                            // accessed via DMA and not with 
                                            // host writes and/or reads. 
                                            // However, for debugging purposes 
                                            // the Data Input/Output Registers 
                                            // can be accessed via host write 
                                            // and read operations. The 
                                            // registers are used to buffer the 
                                            // input/output data blocks to/from 
                                            // the EIP-120t. Note: The data 
                                            // input buffer (AES_DATA_IN_n) and 
                                            // data output buffer 
                                            // (AES_DATA_OUT_n) are mapped to 
                                            // the same address locations. 
                                            // Writes (both DMA and host) to 
                                            // these addresses load the Input 
                                            // Buffer while reads pull from the 
                                            // Output Buffer. Therefore, for 
                                            // write access, the data input 
                                            // buffer is written; for read 
                                            // access, the data output buffer 
                                            // is read. The data input buffer 
                                            // must be written before starting 
                                            // an operation. The data output 
                                            // buffer contains valid data on 
                                            // completion of an operation. 
                                            // Therefore, any 128-bit data 
                                            // block can be split over multiple 
                                            // 32-bit word transfers; these can 
                                            // be mixed with other host 
                                            // transfers over the external 
                                            // interface. 
#define AES_AES_TAG_OUT_0       0x4008B570  // TAG registers The tag registers 
                                            // can be accessed via DMA or 
                                            // directly with host reads. These 
                                            // registers buffer the TAG from 
                                            // the EIP-120t. The registers are 
                                            // shared with the intermediate 
                                            // authentication result registers, 
                                            // but cannot be read until the 
                                            // processing is finished. While 
                                            // processing, a read from these 
                                            // registers returns 0s. If an 
                                            // operation does not return a TAG, 
                                            // reading from these registers 
                                            // returns an IV. If an operation 
                                            // returns a TAG plus an IV and 
                                            // both need to be read by the 
                                            // host, the host must first read 
                                            // the TAG followed by the IV. 
                                            // Reading these in reverse order 
                                            // will return the IV twice. 
#define AES_AES_TAG_OUT_1       0x4008B574  // TAG registers The tag registers 
                                            // can be accessed via DMA or 
                                            // directly with host reads. These 
                                            // registers buffer the TAG from 
                                            // the EIP-120t. The registers are 
                                            // shared with the intermediate 
                                            // authentication result registers, 
                                            // but cannot be read until the 
                                            // processing is finished. While 
                                            // processing, a read from these 
                                            // registers returns 0s. If an 
                                            // operation does not return a TAG, 
                                            // reading from these registers 
                                            // returns an IV. If an operation 
                                            // returns a TAG plus an IV and 
                                            // both need to be read by the 
                                            // host, the host must first read 
                                            // the TAG followed by the IV. 
                                            // Reading these in reverse order 
                                            // returns the IV twice. 
#define AES_AES_TAG_OUT_2       0x4008B578  // TAG registers The tag registers 
                                            // can be accessed via DMA or 
                                            // directly with host reads. These 
                                            // registers buffer the TAG from 
                                            // the EIP-120t. The registers are 
                                            // shared with the intermediate 
                                            // authentication result registers, 
                                            // but cannot be read until the 
                                            // processing is finished. While 
                                            // processing, a read from these 
                                            // registers returns 0s. If an 
                                            // operation does not return a TAG, 
                                            // reading from these registers 
                                            // returns an IV. If an operation 
                                            // returns a TAG plus an IV and 
                                            // both need to be read by the 
                                            // host, the host must first read 
                                            // the TAG followed by the IV. 
                                            // Reading these in reverse order 
                                            // returns the IV twice. 
#define AES_AES_TAG_OUT_3       0x4008B57C  // TAG registers The tag registers 
                                            // can be accessed via DMA or 
                                            // directly with host reads. These 
                                            // registers buffer the TAG from 
                                            // the EIP-120t. The registers are 
                                            // shared with the intermediate 
                                            // authentication result registers, 
                                            // but cannot be read until the 
                                            // processing is finished. While 
                                            // processing, a read from these 
                                            // registers returns 0s. If an 
                                            // operation does not return a TAG, 
                                            // reading from these registers 
                                            // returns an IV. If an operation 
                                            // returns a TAG plus an IV and 
                                            // both need to be read by the 
                                            // host, the host must first read 
                                            // the TAG followed by the IV. 
                                            // Reading these in reverse order 
                                            // returns the IV twice. 
#define AES_HASH_DATA_IN_0      0x4008B600  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_1      0x4008B604  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_2      0x4008B608  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_3      0x4008B60C  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_4      0x4008B610  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_5      0x4008B614  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_6      0x4008B618  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_7      0x4008B61C  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_8      0x4008B620  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_9      0x4008B624  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_10     0x4008B628  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_11     0x4008B62C  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_12     0x4008B630  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_13     0x4008B634  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_14     0x4008B638  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_DATA_IN_15     0x4008B63C  // HASH data input registers The 
                                            // data input registers should be 
                                            // used to provide input data to 
                                            // the hash module through the 
                                            // slave interface. 
#define AES_HASH_IO_BUF_CTRL    0x4008B640  // Input/output buffer control and 
                                            // status register This register 
                                            // pair shares a single address 
                                            // location and contains bits that 
                                            // control and monitor the data 
                                            // flow between the host and the 
                                            // hash engine. 
#define AES_HASH_MODE_IN        0x4008B644  // Hash mode register 
#define AES_HASH_LENGTH_IN_L    0x4008B648  // Hash length register 
#define AES_HASH_LENGTH_IN_H    0x4008B64C  // Hash length register 
#define AES_HASH_DIGEST_A       0x4008B650  // Hash digest registers The hash 
                                            // digest registers consist of 
                                            // eight 32-bit registers, named 
                                            // HASH_DIGEST_A to HASH_DIGEST_H. 
                                            // After processing a message, the 
                                            // output digest can be read from 
                                            // these registers. These registers 
                                            // can be written with an 
                                            // intermediate hash result for 
                                            // continued hash operations. 
#define AES_HASH_DIGEST_B       0x4008B654  // Hash digest registers The hash 
                                            // digest registers consist of 
                                            // eight 32-bit registers, named 
                                            // HASH_DIGEST_A to HASH_DIGEST_H. 
                                            // After processing a message, the 
                                            // output digest can be read from 
                                            // these registers. These registers 
                                            // can be written with an 
                                            // intermediate hash result for 
                                            // continued hash operations. 
#define AES_HASH_DIGEST_C       0x4008B658  // Hash digest registers The hash 
                                            // digest registers consist of 
                                            // eight 32-bit registers, named 
                                            // HASH_DIGEST_A to HASH_DIGEST_H. 
                                            // After processing a message, the 
                                            // output digest can be read from 
                                            // these registers. These registers 
                                            // can be written with an 
                                            // intermediate hash result for 
                                            // continued hash operations. 
#define AES_HASH_DIGEST_D       0x4008B65C  // Hash digest registers The hash 
                                            // digest registers consist of 
                                            // eight 32-bit registers, named 
                                            // HASH_DIGEST_A to HASH_DIGEST_H. 
                                            // After processing a message, the 
                                            // output digest can be read from 
                                            // these registers. These registers 
                                            // can be written with an 
                                            // intermediate hash result for 
                                            // continued hash operations. 
#define AES_HASH_DIGEST_E       0x4008B660  // Hash digest registers The hash 
                                            // digest registers consist of 
                                            // eight 32-bit registers, named 
                                            // HASH_DIGEST_A to HASH_DIGEST_H. 
                                            // After processing a message, the 
                                            // output digest can be read from 
                                            // these registers. These registers 
                                            // can be written with an 
                                            // intermediate hash result for 
                                            // continued hash operations. 
#define AES_HASH_DIGEST_F       0x4008B664  // Hash digest registers The hash 
                                            // digest registers consist of 
                                            // eight 32-bit registers, named 
                                            // HASH_DIGEST_A to HASH_DIGEST_H. 
                                            // After processing a message, the 
                                            // output digest can be read from 
                                            // these registers. These registers 
                                            // can be written with an 
                                            // intermediate hash result for 
                                            // continued hash operations. 
#define AES_HASH_DIGEST_G       0x4008B668  // Hash digest registers The hash 
                                            // digest registers consist of 
                                            // eight 32-bit registers, named 
                                            // HASH_DIGEST_A to HASH_DIGEST_H. 
                                            // After processing a message, the 
                                            // output digest can be read from 
                                            // these registers. These registers 
                                            // can be written with an 
                                            // intermediate hash result for 
                                            // continued hash operations. 
#define AES_HASH_DIGEST_H       0x4008B66C  // Hash digest registers The hash 
                                            // digest registers consist of 
                                            // eight 32-bit registers, named 
                                            // HASH_DIGEST_A to HASH_DIGEST_H. 
                                            // After processing a message, the 
                                            // output digest can be read from 
                                            // these registers. These registers 
                                            // can be written with an 
                                            // intermediate hash result for 
                                            // continued hash operations. 
#define AES_CTRL_ALG_SEL        0x4008B700  // Algorithm select This algorithm 
                                            // selection register configures 
                                            // the internal destination of the 
                                            // DMA controller. 
#define AES_CTRL_PROT_EN        0x4008B704  // Master PROT privileged access 
                                            // enable This register enables the 
                                            // second bit (bit [1]) of the AHB 
                                            // HPROT bus of the AHB master 
                                            // interface when a read action of 
                                            // key(s) is performed on the AHB 
                                            // master interface for writing 
                                            // keys into the store module. 
#define AES_CTRL_SW_RESET       0x4008B740  // Software reset 
#define AES_CTRL_INT_CFG        0x4008B780  // Interrupt configuration 
#define AES_CTRL_INT_EN         0x4008B784  // Interrupt enable 
#define AES_CTRL_INT_CLR        0x4008B788  // Interrupt clear 
#define AES_CTRL_INT_SET        0x4008B78C  // Interrupt set 
#define AES_CTRL_INT_STAT       0x4008B790  // Interrupt status 
#define AES_CTRL_OPTIONS        0x4008B7F8  // Options register 
#define AES_CTRL_VERSION        0x4008B7FC  // Version register 


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_CH0_CTRL register.
//
//*****************************************************************************
#define AES_DMAC_CH0_CTRL_PRIO  0x00000002  // Channel priority 0: Low 1: High 
                                            // If both channels have the same 
                                            // priority, access of the channels 
                                            // to the external port is 
                                            // arbitrated using the round robin 
                                            // scheme. If one channel has a 
                                            // high priority and another one 
                                            // low, the channel with the high 
                                            // priority is served first, in 
                                            // case of simultaneous access 
                                            // requests. 
#define AES_DMAC_CH0_CTRL_PRIO_M \
                                0x00000002
#define AES_DMAC_CH0_CTRL_PRIO_S 1
#define AES_DMAC_CH0_CTRL_EN    0x00000001  // Channel enable 0: Disabled 1: 
                                            // Enable Note: Disabling an active 
                                            // channel interrupts the DMA 
                                            // operation. The ongoing block 
                                            // transfer completes, but no new 
                                            // transfers are requested. 
#define AES_DMAC_CH0_CTRL_EN_M  0x00000001
#define AES_DMAC_CH0_CTRL_EN_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_CH0_EXTADDR register.
//
//*****************************************************************************
#define AES_DMAC_CH0_EXTADDR_ADDR_M \
                                0xFFFFFFFF  // Channel external address value 
                                            // When read during operation, it 
                                            // holds the last updated external 
                                            // address after being sent to the 
                                            // master interface. 

#define AES_DMAC_CH0_EXTADDR_ADDR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_CH0_DMALENGTH register.
//
//*****************************************************************************
#define AES_DMAC_CH0_DMALENGTH_DMALEN_M \
                                0x0000FFFF  // Channel DMA length in bytes 
                                            // During configuration, this 
                                            // register contains the DMA 
                                            // transfer length in bytes. During 
                                            // operation, it contains the last 
                                            // updated value of the DMA 
                                            // transfer length after being sent 
                                            // to the master interface. Note: 
                                            // Setting this register to a 
                                            // nonzero value starts the 
                                            // transfer if the channel is 
                                            // enabled. Therefore, this 
                                            // register must be written last 
                                            // when setting up a DMA channel. 

#define AES_DMAC_CH0_DMALENGTH_DMALEN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_STATUS register.
//
//*****************************************************************************
#define AES_DMAC_STATUS_PORT_ERR \
                                0x00020000  // Reflects possible transfer 
                                            // errors on the AHB port. 

#define AES_DMAC_STATUS_PORT_ERR_M \
                                0x00020000
#define AES_DMAC_STATUS_PORT_ERR_S 17
#define AES_DMAC_STATUS_CH1_ACT 0x00000002  // A value of 1 indicates that 
                                            // channel 1 is active (DMA 
                                            // transfer on-going). 
#define AES_DMAC_STATUS_CH1_ACT_M \
                                0x00000002
#define AES_DMAC_STATUS_CH1_ACT_S 1
#define AES_DMAC_STATUS_CH0_ACT 0x00000001  // A value of 1 indicates that 
                                            // channel 0 is active (DMA 
                                            // transfer on-going). 
#define AES_DMAC_STATUS_CH0_ACT_M \
                                0x00000001
#define AES_DMAC_STATUS_CH0_ACT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_SWRES register.
//
//*****************************************************************************
#define AES_DMAC_SWRES_SWRES    0x00000001  // Software reset enable 0 = 
                                            // Disabled 1 = Enabled 
                                            // (self-cleared to 0) Completion 
                                            // of the software reset must be 
                                            // checked through the DMAC_STATUS 
                                            // register. 
#define AES_DMAC_SWRES_SWRES_M  0x00000001
#define AES_DMAC_SWRES_SWRES_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_CH1_CTRL register.
//
//*****************************************************************************
#define AES_DMAC_CH1_CTRL_PRIO  0x00000002  // Channel priority 0: Low 1: High 
                                            // If both channels have the same 
                                            // priority, access of the channels 
                                            // to the external port is 
                                            // arbitrated using the round robin 
                                            // scheme. If one channel has a 
                                            // high priority and another one 
                                            // low, the channel with the high 
                                            // priority is served first, in 
                                            // case of simultaneous access 
                                            // requests. 
#define AES_DMAC_CH1_CTRL_PRIO_M \
                                0x00000002
#define AES_DMAC_CH1_CTRL_PRIO_S 1
#define AES_DMAC_CH1_CTRL_EN    0x00000001  // Channel enable 0: Disabled 1: 
                                            // Enable Note: Disabling an active 
                                            // channel interrupts the DMA 
                                            // operation. The ongoing block 
                                            // transfer completes, but no new 
                                            // transfers are requested. 
#define AES_DMAC_CH1_CTRL_EN_M  0x00000001
#define AES_DMAC_CH1_CTRL_EN_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_CH1_EXTADDR register.
//
//*****************************************************************************
#define AES_DMAC_CH1_EXTADDR_ADDR_M \
                                0xFFFFFFFF  // Channel external address value. 
                                            // When read during operation, it 
                                            // holds the last updated external 
                                            // address after being sent to the 
                                            // master interface. 

#define AES_DMAC_CH1_EXTADDR_ADDR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_CH1_DMALENGTH register.
//
//*****************************************************************************
#define AES_DMAC_CH1_DMALENGTH_DMALEN_M \
                                0x0000FFFF  // Channel DMA length in bytes. 
                                            // During configuration, this 
                                            // register contains the DMA 
                                            // transfer length in bytes. During 
                                            // operation, it contains the last 
                                            // updated value of the DMA 
                                            // transfer length after being sent 
                                            // to the master interface. Note: 
                                            // Setting this register to a 
                                            // nonzero value starts the 
                                            // transfer if the channel is 
                                            // enabled. Therefore, this 
                                            // register must be written last 
                                            // when setting up a DMA channel. 

#define AES_DMAC_CH1_DMALENGTH_DMALEN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_MST_RUNPARAMS register.
//
//*****************************************************************************
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_BURST_SIZE_M \
                                0x0000F000  // Maximum burst size that can be 
                                            // performed on the AHB bus 0010b = 
                                            // 4 bytes (default) 0011b = 8 
                                            // bytes 0100b = 16 bytes 0101b = 
                                            // 32 bytes 0110b = 64 bytes Others 
                                            // = Reserved 

#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_BURST_SIZE_S 12
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_IDLE_EN \
                                0x00000800  // Idle insertion between 
                                            // consecutive burst transfers on 
                                            // AHB 0: No Idle insertion 1: Idle 
                                            // insertion 

#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_IDLE_EN_M \
                                0x00000800
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_IDLE_EN_S 11
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_INCR_EN \
                                0x00000400  // Burst length type of AHB 
                                            // transfer 0: Unspecified length 
                                            // burst transfers 1: Fixed length 
                                            // burst or single transfers 

#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_INCR_EN_M \
                                0x00000400
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_INCR_EN_S 10
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_LOCK_EN \
                                0x00000200  // Locked transform on AHB 0: 
                                            // Transfers are not locked 1: 
                                            // Transfers are locked 

#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_LOCK_EN_M \
                                0x00000200
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_LOCK_EN_S 9
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_BIGEND \
                                0x00000100  // Endianess for the AHB master 0: 
                                            // Little endian 1: Big endian 

#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_BIGEND_M \
                                0x00000100
#define AES_DMAC_MST_RUNPARAMS_AHB_MST1_BIGEND_S 8
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_PERSR register.
//
//*****************************************************************************
#define AES_DMAC_PERSR_PORT1_AHB_ERROR \
                                0x00001000  // A value of 1 indicates that the 
                                            // EIP-101 has detected an AHB bus 
                                            // error 

#define AES_DMAC_PERSR_PORT1_AHB_ERROR_M \
                                0x00001000
#define AES_DMAC_PERSR_PORT1_AHB_ERROR_S 12
#define AES_DMAC_PERSR_PORT1_CHANNEL \
                                0x00000200  // Indicates which channel has 
                                            // serviced last (channel 0 or 
                                            // channel 1) by AHB master port. 

#define AES_DMAC_PERSR_PORT1_CHANNEL_M \
                                0x00000200
#define AES_DMAC_PERSR_PORT1_CHANNEL_S 9
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_OPTIONS register.
//
//*****************************************************************************
#define AES_DMAC_OPTIONS_NR_OF_CHANNELS_M \
                                0x00000F00  // Number of channels implemented, 
                                            // value in the range 1-8. 

#define AES_DMAC_OPTIONS_NR_OF_CHANNELS_S 8
#define AES_DMAC_OPTIONS_NR_OF_PORTS_M \
                                0x00000007  // Number of ports implemented, 
                                            // value in range 1-4. 

#define AES_DMAC_OPTIONS_NR_OF_PORTS_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_DMAC_VERSION register.
//
//*****************************************************************************
#define AES_DMAC_VERSION_HW_MAJOR_VERSION_M \
                                0x0F000000  // Major version number 

#define AES_DMAC_VERSION_HW_MAJOR_VERSION_S 24
#define AES_DMAC_VERSION_HW_MINOR_VERSION_M \
                                0x00F00000  // Minor version number 

#define AES_DMAC_VERSION_HW_MINOR_VERSION_S 20
#define AES_DMAC_VERSION_HW_PATCH_LEVEL_M \
                                0x000F0000  // Patch level Starts at 0 at 
                                            // first delivery of this version 

#define AES_DMAC_VERSION_HW_PATCH_LEVEL_S 16
#define AES_DMAC_VERSION_EIP_NUMBER_COMPL_M \
                                0x0000FF00  // Bit-by-bit complement of the 
                                            // EIP_NUMBER field bits. 

#define AES_DMAC_VERSION_EIP_NUMBER_COMPL_S 8
#define AES_DMAC_VERSION_EIP_NUMBER_M \
                                0x000000FF  // Binary encoding of the 
                                            // EIP-number of this DMA 
                                            // controller (209) 

#define AES_DMAC_VERSION_EIP_NUMBER_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_KEY_STORE_WRITE_AREA register.
//
//*****************************************************************************
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA7 \
                                0x00000080  // Each RAM_AREAx represents an 
                                            // area of 128 bits. Select the key 
                                            // store RAM area(s) where the 
                                            // key(s) needs to be written 0: 
                                            // RAM_AREA7 is not selected to be 
                                            // written. 1: RAM_AREA7 is 
                                            // selected to be written. Writing 
                                            // to multiple RAM locations is 
                                            // possible only when the selected 
                                            // RAM areas are sequential. Keys 
                                            // that require more than one RAM 
                                            // locations (key size is 192 or 
                                            // 256 bits), must start at one of 
                                            // the following areas: RAM_AREA0, 
                                            // RAM_AREA2, RAM_AREA4, or 
                                            // RAM_AREA6. 

#define AES_KEY_STORE_WRITE_AREA_RAM_AREA7_M \
                                0x00000080
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA7_S 7
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA6 \
                                0x00000040  // Each RAM_AREAx represents an 
                                            // area of 128 bits. Select the key 
                                            // store RAM area(s) where the 
                                            // key(s) needs to be written 0: 
                                            // RAM_AREA6 is not selected to be 
                                            // written. 1: RAM_AREA6 is 
                                            // selected to be written. Writing 
                                            // to multiple RAM locations is 
                                            // possible only when the selected 
                                            // RAM areas are sequential. Keys 
                                            // that require more than one RAM 
                                            // locations (key size is 192 or 
                                            // 256 bits), must start at one of 
                                            // the following areas: RAM_AREA0, 
                                            // RAM_AREA2, RAM_AREA4, or 
                                            // RAM_AREA6. 

#define AES_KEY_STORE_WRITE_AREA_RAM_AREA6_M \
                                0x00000040
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA6_S 6
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA5 \
                                0x00000020  // Each RAM_AREAx represents an 
                                            // area of 128 bits. Select the key 
                                            // store RAM area(s) where the 
                                            // key(s) needs to be written 0: 
                                            // RAM_AREA5 is not selected to be 
                                            // written. 1: RAM_AREA5 is 
                                            // selected to be written. Writing 
                                            // to multiple RAM locations is 
                                            // possible only when the selected 
                                            // RAM areas are sequential. Keys 
                                            // that require more than one RAM 
                                            // locations (key size is 192 or 
                                            // 256 bits), must start at one of 
                                            // the following areas: RAM_AREA0, 
                                            // RAM_AREA2, RAM_AREA4, or 
                                            // RAM_AREA6. 

#define AES_KEY_STORE_WRITE_AREA_RAM_AREA5_M \
                                0x00000020
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA5_S 5
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA4 \
                                0x00000010  // Each RAM_AREAx represents an 
                                            // area of 128 bits. Select the key 
                                            // store RAM area(s) where the 
                                            // key(s) needs to be written 0: 
                                            // RAM_AREA4 is not selected to be 
                                            // written. 1: RAM_AREA4 is 
                                            // selected to be written. Writing 
                                            // to multiple RAM locations is 
                                            // possible only when the selected 
                                            // RAM areas are sequential. Keys 
                                            // that require more than one RAM 
                                            // locations (key size is 192 or 
                                            // 256 bits), must start at one of 
                                            // the following areas: RAM_AREA0, 
                                            // RAM_AREA2, RAM_AREA4, or 
                                            // RAM_AREA6. 

#define AES_KEY_STORE_WRITE_AREA_RAM_AREA4_M \
                                0x00000010
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA4_S 4
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA3 \
                                0x00000008  // Each RAM_AREAx represents an 
                                            // area of 128 bits. Select the key 
                                            // store RAM area(s) where the 
                                            // key(s) needs to be written 0: 
                                            // RAM_AREA3 is not selected to be 
                                            // written. 1: RAM_AREA3 is 
                                            // selected to be written. Writing 
                                            // to multiple RAM locations is 
                                            // possible only when the selected 
                                            // RAM areas are sequential. Keys 
                                            // that require more than one RAM 
                                            // locations (key size is 192 or 
                                            // 256 bits), must start at one of 
                                            // the following areas: RAM_AREA0, 
                                            // RAM_AREA2, RAM_AREA4, or 
                                            // RAM_AREA6. 

#define AES_KEY_STORE_WRITE_AREA_RAM_AREA3_M \
                                0x00000008
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA3_S 3
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA2 \
                                0x00000004  // Each RAM_AREAx represents an 
                                            // area of 128 bits. Select the key 
                                            // store RAM area(s) where the 
                                            // key(s) needs to be written 0: 
                                            // RAM_AREA2 is not selected to be 
                                            // written. 1: RAM_AREA2 is 
                                            // selected to be written. Writing 
                                            // to multiple RAM locations is 
                                            // possible only when the selected 
                                            // RAM areas are sequential. Keys 
                                            // that require more than one RAM 
                                            // locations (key size is 192 or 
                                            // 256 bits), must start at one of 
                                            // the following areas: RAM_AREA0, 
                                            // RAM_AREA2, RAM_AREA4, or 
                                            // RAM_AREA6. 

#define AES_KEY_STORE_WRITE_AREA_RAM_AREA2_M \
                                0x00000004
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA2_S 2
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA1 \
                                0x00000002  // Each RAM_AREAx represents an 
                                            // area of 128 bits. Select the key 
                                            // store RAM area(s) where the 
                                            // key(s) needs to be written 0: 
                                            // RAM_AREA1 is not selected to be 
                                            // written. 1: RAM_AREA1 is 
                                            // selected to be written. Writing 
                                            // to multiple RAM locations is 
                                            // possible only when the selected 
                                            // RAM areas are sequential. Keys 
                                            // that require more than one RAM 
                                            // locations (key size is 192 or 
                                            // 256 bits), must start at one of 
                                            // the following areas: RAM_AREA0, 
                                            // RAM_AREA2, RAM_AREA4, or 
                                            // RAM_AREA6. 

#define AES_KEY_STORE_WRITE_AREA_RAM_AREA1_M \
                                0x00000002
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA1_S 1
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA0 \
                                0x00000001  // Each RAM_AREAx represents an 
                                            // area of 128 bits. Select the key 
                                            // store RAM area(s) where the 
                                            // key(s) needs to be written 0: 
                                            // RAM_AREA0 is not selected to be 
                                            // written. 1: RAM_AREA0 is 
                                            // selected to be written. Writing 
                                            // to multiple RAM locations is 
                                            // possible only when the selected 
                                            // RAM areas are sequential. Keys 
                                            // that require more than one RAM 
                                            // locations (key size is 192 or 
                                            // 256 bits), must start at one of 
                                            // the following areas: RAM_AREA0, 
                                            // RAM_AREA2, RAM_AREA4, or 
                                            // RAM_AREA6. 

#define AES_KEY_STORE_WRITE_AREA_RAM_AREA0_M \
                                0x00000001
#define AES_KEY_STORE_WRITE_AREA_RAM_AREA0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_KEY_STORE_WRITTEN_AREA register.
//
//*****************************************************************************
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN7 \
                                0x00000080  // Read operation: 0: This RAM 
                                            // area is not written with valid 
                                            // key information. 1: This RAM 
                                            // area is written with valid key 
                                            // information. Each individual 
                                            // ram_area_writtenx bit can be 
                                            // reset by writing 1. Note: This 
                                            // register is reset on a soft 
                                            // reset from the master control 
                                            // module. After a soft reset, all 
                                            // keys must be rewritten to the 
                                            // key store memory. 

#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN7_M \
                                0x00000080
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN7_S 7
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN6 \
                                0x00000040  // Read operation: 0: This RAM 
                                            // area is not written with valid 
                                            // key information. 1: This RAM 
                                            // area is written with valid key 
                                            // information. Each individual 
                                            // ram_area_writtenx bit can be 
                                            // reset by writing 1. Note: This 
                                            // register is reset on a soft 
                                            // reset from the master control 
                                            // module. After a soft reset, all 
                                            // keys must be rewritten to the 
                                            // key store memory. 

#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN6_M \
                                0x00000040
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN6_S 6
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN5 \
                                0x00000020  // Read operation: 0: This RAM 
                                            // area is not written with valid 
                                            // key information. 1: This RAM 
                                            // area is written with valid key 
                                            // information. Each individual 
                                            // ram_area_writtenx bit can be 
                                            // reset by writing 1. Note: This 
                                            // register is reset on a soft 
                                            // reset from the master control 
                                            // module. After a soft reset, all 
                                            // keys must be rewritten to the 
                                            // key store memory. 

#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN5_M \
                                0x00000020
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN5_S 5
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN4 \
                                0x00000010  // Read operation: 0: This RAM 
                                            // area is not written with valid 
                                            // key information. 1: This RAM 
                                            // area is written with valid key 
                                            // information. Each individual 
                                            // ram_area_writtenx bit can be 
                                            // reset by writing 1. Note: This 
                                            // register is reset on a soft 
                                            // reset from the master control 
                                            // module. After a soft reset, all 
                                            // keys must be rewritten to the 
                                            // key store memory. 

#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN4_M \
                                0x00000010
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN4_S 4
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN3 \
                                0x00000008  // Read operation: 0: This RAM 
                                            // area is not written with valid 
                                            // key information. 1: This RAM 
                                            // area is written with valid key 
                                            // information. Each individual 
                                            // ram_area_writtenx bit can be 
                                            // reset by writing 1. Note: This 
                                            // register is reset on a soft 
                                            // reset from the master control 
                                            // module. After a soft reset, all 
                                            // keys must be rewritten to the 
                                            // key store memory. 

#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN3_M \
                                0x00000008
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN3_S 3
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN2 \
                                0x00000004  // Read operation: 0: This RAM 
                                            // area is not written with valid 
                                            // key information. 1: This RAM 
                                            // area is written with valid key 
                                            // information. Each individual 
                                            // ram_area_writtenx bit can be 
                                            // reset by writing 1. Note: This 
                                            // register is reset on a soft 
                                            // reset from the master control 
                                            // module. After a soft reset, all 
                                            // keys must be rewritten to the 
                                            // key store memory. 

#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN2_M \
                                0x00000004
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN2_S 2
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN1 \
                                0x00000002  // Read operation: 0: This RAM 
                                            // area is not written with valid 
                                            // key information. 1: This RAM 
                                            // area is written with valid key 
                                            // information. Each individual 
                                            // ram_area_writtenx bit can be 
                                            // reset by writing 1. Note: This 
                                            // register is reset on a soft 
                                            // reset from the master control 
                                            // module. After a soft reset, all 
                                            // keys must be rewritten to the 
                                            // key store memory. 

#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN1_M \
                                0x00000002
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN1_S 1
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN0 \
                                0x00000001  // Read operation: 0: This RAM 
                                            // area is not written with valid 
                                            // key information. 1: This RAM 
                                            // area is written with valid key 
                                            // information. Each individual 
                                            // ram_area_writtenx bit can be 
                                            // reset by writing 1. Note: This 
                                            // register is reset on a soft 
                                            // reset from the master control 
                                            // module. After a soft reset, all 
                                            // keys must be rewritten to the 
                                            // key store memory. 

#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN0_M \
                                0x00000001
#define AES_KEY_STORE_WRITTEN_AREA_RAM_AREA_WRITTEN0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_KEY_STORE_SIZE register.
//
//*****************************************************************************
#define AES_KEY_STORE_SIZE_KEY_SIZE_M \
                                0x00000003  // Key size: 00: Reserved 01: 128 
                                            // bits 10: 192 bits 11: 256 bits 
                                            // When writing this to this 
                                            // register, the 
                                            // KEY_STORE_WRITTEN_AREA register 
                                            // is reset. 

#define AES_KEY_STORE_SIZE_KEY_SIZE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_KEY_STORE_READ_AREA register.
//
//*****************************************************************************
#define AES_KEY_STORE_READ_AREA_BUSY \
                                0x80000000  // Key store operation busy status 
                                            // flag (read only): 0: Operation 
                                            // is complete. 1: Operation is not 
                                            // completed and the key store is 
                                            // busy. 

#define AES_KEY_STORE_READ_AREA_BUSY_M \
                                0x80000000
#define AES_KEY_STORE_READ_AREA_BUSY_S 31
#define AES_KEY_STORE_READ_AREA_RAM_AREA_M \
                                0x0000000F  // Selects the area of the key 
                                            // store RAM from where the key 
                                            // needs to be read that will be 
                                            // writen to the AES engine 
                                            // RAM_AREA: 0000: RAM_AREA0 0001: 
                                            // RAM_AREA1 0010: RAM_AREA2 0011: 
                                            // RAM_AREA3 0100: RAM_AREA4 0101: 
                                            // RAM_AREA5 0110: RAM_AREA6 0111: 
                                            // RAM_AREA7 1000: no RAM area 
                                            // selected 1001-1111: Reserved RAM 
                                            // areas RAM_AREA0, RAM_AREA2, 
                                            // RAM_AREA4 and RAM_AREA6 are the 
                                            // only valid read areas for 192 
                                            // and 256 bits key sizes. Only RAM 
                                            // areas that contain valid written 
                                            // keys can be selected. 

#define AES_KEY_STORE_READ_AREA_RAM_AREA_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_KEY2_0 register.
//
//*****************************************************************************
#define AES_AES_KEY2_0_AES_KEY2_M \
                                0xFFFFFFFF  // AES_KEY2/AES_GHASH_H[31:0] For 
                                            // GCM: -[127:0] - GHASH_H - The 
                                            // internally calculated GHASH key 
                                            // is stored in these registers. 
                                            // Only used for modes that use the 
                                            // GHASH function (GCM). -[255:128] 
                                            // - This register is used to store 
                                            // intermediate values and is 
                                            // initialized with 0s when loading 
                                            // a new key. For CCM: -[255:0] - 
                                            // This register is used to store 
                                            // intermediate values. For 
                                            // CBC-MAC: -[255:0] - ZEROES - 
                                            // This register must remain 0. 

#define AES_AES_KEY2_0_AES_KEY2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_KEY2_1 register.
//
//*****************************************************************************
#define AES_AES_KEY2_1_AES_KEY2_M \
                                0xFFFFFFFF  // AES_KEY2/AES_GHASH_H[63:32] For 
                                            // GCM: -[127:0] - GHASH_H - The 
                                            // internally calculated GHASH key 
                                            // is stored in these registers. 
                                            // Only used for modes that use the 
                                            // GHASH function (GCM). -[255:128] 
                                            // - This register is used to store 
                                            // intermediate values and is 
                                            // initialized with 0s when loading 
                                            // a new key. For CCM: -[255:0] - 
                                            // This register is used to store 
                                            // intermediate values. For 
                                            // CBC-MAC: -[255:0] - ZEROES - 
                                            // This register must remain 0. 

#define AES_AES_KEY2_1_AES_KEY2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_KEY2_2 register.
//
//*****************************************************************************
#define AES_AES_KEY2_2_AES_KEY2_M \
                                0xFFFFFFFF  // AES_KEY2/AES_GHASH_H[95:64] For 
                                            // GCM: -[127:0] - GHASH_H - The 
                                            // internally calculated GHASH key 
                                            // is stored in these registers. 
                                            // Only used for modes that use the 
                                            // GHASH function (GCM). -[255:128] 
                                            // - This register is used to store 
                                            // intermediate values and is 
                                            // initialized with 0s when loading 
                                            // a new key. For CCM: -[255:0] - 
                                            // This register is used to store 
                                            // intermediate values. For 
                                            // CBC-MAC: -[255:0] - ZEROES - 
                                            // This register must remain 0. 

#define AES_AES_KEY2_2_AES_KEY2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_KEY2_3 register.
//
//*****************************************************************************
#define AES_AES_KEY2_3_AES_KEY2_M \
                                0xFFFFFFFF  // AES_KEY2/AES_GHASH_H[127:96] 
                                            // For GCM: -[127:0] - GHASH_H - 
                                            // The internally calculated GHASH 
                                            // key is stored in these 
                                            // registers. Only used for modes 
                                            // that use the GHASH function 
                                            // (GCM). -[255:128] - This 
                                            // register is used to store 
                                            // intermediate values and is 
                                            // initialized with 0s when loading 
                                            // a new key. For CCM: -[255:0] - 
                                            // This register is used to store 
                                            // intermediate values. For 
                                            // CBC-MAC: -[255:0] - ZEROES - 
                                            // This register must remain 0. 

#define AES_AES_KEY2_3_AES_KEY2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_KEY3_0 register.
//
//*****************************************************************************
#define AES_AES_KEY3_0_AES_KEY3_M \
                                0xFFFFFFFF  // 
                                            // AES_KEY3[31:0]/AES_KEY2[159:128] 
                                            // For GCM: -[127:0] - GHASH_H - 
                                            // The internally calculated GHASH 
                                            // key is stored in these 
                                            // registers. Only used for modes 
                                            // that use the GHASH function 
                                            // (GCM). -[255:128] - This 
                                            // register is used to store 
                                            // intermediate values and is 
                                            // initialized with 0s when loading 
                                            // a new key. For CCM: -[255:0] - 
                                            // This register is used to store 
                                            // intermediate values. For 
                                            // CBC-MAC: -[255:0] - ZEROES - 
                                            // This register must remain 0. 

#define AES_AES_KEY3_0_AES_KEY3_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_KEY3_1 register.
//
//*****************************************************************************
#define AES_AES_KEY3_1_AES_KEY3_M \
                                0xFFFFFFFF  // 
                                            // AES_KEY3[63:32]/AES_KEY2[191:160] 
                                            // For GCM: -[127:0] - GHASH_H - 
                                            // The internally calculated GHASH 
                                            // key is stored in these 
                                            // registers. Only used for modes 
                                            // that use the GHASH function 
                                            // (GCM). -[255:128] - This 
                                            // register is used to store 
                                            // intermediate values and is 
                                            // initialized with 0s when loading 
                                            // a new key. For CCM: -[255:0] - 
                                            // This register is used to store 
                                            // intermediate values. For 
                                            // CBC-MAC: -[255:0] - ZEROES - 
                                            // This register must remain 0. 

#define AES_AES_KEY3_1_AES_KEY3_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_KEY3_2 register.
//
//*****************************************************************************
#define AES_AES_KEY3_2_AES_KEY3_M \
                                0xFFFFFFFF  // 
                                            // AES_KEY3[95:64]/AES_KEY2[223:192] 
                                            // For GCM: -[127:0] - GHASH_H - 
                                            // The internally calculated GHASH 
                                            // key is stored in these 
                                            // registers. Only used for modes 
                                            // that use the GHASH function 
                                            // (GCM). -[255:128] - This 
                                            // register is used to store 
                                            // intermediate values and is 
                                            // initialized with 0s when loading 
                                            // a new key. For CCM: -[255:0] - 
                                            // This register is used to store 
                                            // intermediate values. For 
                                            // CBC-MAC: -[255:0] - ZEROES - 
                                            // This register must remain 0. 

#define AES_AES_KEY3_2_AES_KEY3_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_KEY3_3 register.
//
//*****************************************************************************
#define AES_AES_KEY3_3_AES_KEY3_M \
                                0xFFFFFFFF  // 
                                            // AES_KEY3[127:96]/AES_KEY2[255:224] 
                                            // For GCM: -[127:0] - GHASH_H - 
                                            // The internally calculated GHASH 
                                            // key is stored in these 
                                            // registers. Only used for modes 
                                            // that use the GHASH function 
                                            // (GCM). -[255:128] - This 
                                            // register is used to store 
                                            // intermediate values and is 
                                            // initialized with 0s when loading 
                                            // a new key. For CCM: -[255:0] - 
                                            // This register is used to store 
                                            // intermediate values. For 
                                            // CBC-MAC: -[255:0] - ZEROES - 
                                            // This register must remain 0. 

#define AES_AES_KEY3_3_AES_KEY3_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the AES_AES_IV_0 register.
//
//*****************************************************************************
#define AES_AES_IV_0_AES_IV_M   0xFFFFFFFF  // AES_IV[31:0] Initialization 
                                            // vector Used for regular non-ECB 
                                            // modes (CBC/CTR): -[127:0] - 
                                            // AES_IV - For regular AES 
                                            // operations (CBC and CTR) these 
                                            // registers must be written with a 
                                            // new 128-bit IV. After an 
                                            // operation, these registers 
                                            // contain the latest 128-bit 
                                            // result IV, generated by the 
                                            // EIP-120t. If CTR mode is 
                                            // selected, this value is 
                                            // incremented with 0x1: After 
                                            // first use - When a new data 
                                            // block is submitted to the engine 
                                            // For GCM: -[127:0] - AES_IV - For 
                                            // GCM operations, these registers 
                                            // must be written with a new 
                                            // 128-bit IV. After an operation, 
                                            // these registers contain the 
                                            // updated 128-bit result IV, 
                                            // generated by the EIP-120t. Note 
                                            // that bits [127:96] of the IV 
                                            // represent the initial counter 
                                            // value (which is 1 for GCM) and 
                                            // must therefore be initialized to 
                                            // 0x01000000. This value is 
                                            // incremented with 0x1: After 
                                            // first use - When a new data 
                                            // block is submitted to the 
                                            // engine. For CCM: -[127:0] - A0: 
                                            // For CCM this field must be 
                                            // written with value A0, this 
                                            // value is the concatenation of: 
                                            // A0-flags (5-bits of 0 and 3-bits 
                                            // 'L'), Nonce and counter value. 
                                            // 'L' must be a copy from the 'L' 
                                            // value of the AES_CTRL register. 
                                            // This 'L' indicates the width of 
                                            // the Nonce and counter. The 
                                            // loaded counter must be 
                                            // initialized to 0. The total 
                                            // width of A0 is 128-bit. For 
                                            // CBC-MAC: -[127:0] - Zeroes - For 
                                            // CBC-MAC this register must be 
                                            // written with 0s at the start of 
                                            // each operation. After an 
                                            // operation, these registers 
                                            // contain the 128-bit TAG output, 
                                            // generated by the EIP-120t. 
#define AES_AES_IV_0_AES_IV_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the AES_AES_IV_1 register.
//
//*****************************************************************************
#define AES_AES_IV_1_AES_IV_M   0xFFFFFFFF  // AES_IV[63:32] Initialization 
                                            // vector Used for regular non-ECB 
                                            // modes (CBC/CTR): -[127:0] - 
                                            // AES_IV - For regular AES 
                                            // operations (CBC and CTR) these 
                                            // registers must be written with a 
                                            // new 128-bit IV. After an 
                                            // operation, these registers 
                                            // contain the latest 128-bit 
                                            // result IV, generated by the 
                                            // EIP-120t. If CTR mode is 
                                            // selected, this value is 
                                            // incremented with 0x1: After 
                                            // first use - When a new data 
                                            // block is submitted to the engine 
                                            // For GCM: -[127:0] - AES_IV - For 
                                            // GCM operations, these registers 
                                            // must be written with a new 
                                            // 128-bit IV. After an operation, 
                                            // these registers contain the 
                                            // updated 128-bit result IV, 
                                            // generated by the EIP-120t. Note 
                                            // that bits [127:96] of the IV 
                                            // represent the initial counter 
                                            // value (which is 1 for GCM) and 
                                            // must therefore be initialized to 
                                            // 0x01000000. This value is 
                                            // incremented with 0x1: After 
                                            // first use - When a new data 
                                            // block is submitted to the 
                                            // engine. For CCM: -[127:0] - A0: 
                                            // For CCM this field must be 
                                            // written with value A0, this 
                                            // value is the concatenation of: 
                                            // A0-flags (5-bits of 0 and 3-bits 
                                            // 'L'), Nonce and counter value. 
                                            // 'L' must be a copy from the 'L' 
                                            // value of the AES_CTRL register. 
                                            // This 'L' indicates the width of 
                                            // the Nonce and counter. The 
                                            // loaded counter must be 
                                            // initialized to 0. The total 
                                            // width of A0 is 128-bit. For 
                                            // CBC-MAC: -[127:0] - Zeroes - For 
                                            // CBC-MAC this register must be 
                                            // written with 0s at the start of 
                                            // each operation. After an 
                                            // operation, these registers 
                                            // contain the 128-bit TAG output, 
                                            // generated by the EIP-120t. 
#define AES_AES_IV_1_AES_IV_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the AES_AES_IV_2 register.
//
//*****************************************************************************
#define AES_AES_IV_2_AES_IV_M   0xFFFFFFFF  // AES_IV[95:64] Initialization 
                                            // vector Used for regular non-ECB 
                                            // modes (CBC/CTR): -[127:0] - 
                                            // AES_IV - For regular AES 
                                            // operations (CBC and CTR) these 
                                            // registers must be written with a 
                                            // new 128-bit IV. After an 
                                            // operation, these registers 
                                            // contain the latest 128-bit 
                                            // result IV, generated by the 
                                            // EIP-120t. If CTR mode is 
                                            // selected, this value is 
                                            // incremented with 0x1: After 
                                            // first use - When a new data 
                                            // block is submitted to the engine 
                                            // For GCM: -[127:0] - AES_IV - For 
                                            // GCM operations, these registers 
                                            // must be written with a new 
                                            // 128-bit IV. After an operation, 
                                            // these registers contain the 
                                            // updated 128-bit result IV, 
                                            // generated by the EIP-120t. Note 
                                            // that bits [127:96] of the IV 
                                            // represent the initial counter 
                                            // value (which is 1 for GCM) and 
                                            // must therefore be initialized to 
                                            // 0x01000000. This value is 
                                            // incremented with 0x1: After 
                                            // first use - When a new data 
                                            // block is submitted to the 
                                            // engine. For CCM: -[127:0] - A0: 
                                            // For CCM this field must be 
                                            // written with value A0, this 
                                            // value is the concatenation of: 
                                            // A0-flags (5-bits of 0 and 3-bits 
                                            // 'L'), Nonce and counter value. 
                                            // 'L' must be a copy from the 'L' 
                                            // value of the AES_CTRL register. 
                                            // This 'L' indicates the width of 
                                            // the Nonce and counter. The 
                                            // loaded counter must be 
                                            // initialized to 0. The total 
                                            // width of A0 is 128-bit. For 
                                            // CBC-MAC: -[127:0] - Zeroes - For 
                                            // CBC-MAC this register must be 
                                            // written with 0s at the start of 
                                            // each operation. After an 
                                            // operation, these registers 
                                            // contain the 128-bit TAG output, 
                                            // generated by the EIP-120t. 
#define AES_AES_IV_2_AES_IV_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the AES_AES_IV_3 register.
//
//*****************************************************************************
#define AES_AES_IV_3_AES_IV_M   0xFFFFFFFF  // AES_IV[127:96] Initialization 
                                            // vector Used for regular non-ECB 
                                            // modes (CBC/CTR): -[127:0] - 
                                            // AES_IV - For regular AES 
                                            // operations (CBC and CTR) these 
                                            // registers must be written with a 
                                            // new 128-bit IV. After an 
                                            // operation, these registers 
                                            // contain the latest 128-bit 
                                            // result IV, generated by the 
                                            // EIP-120t. If CTR mode is 
                                            // selected, this value is 
                                            // incremented with 0x1: After 
                                            // first use - When a new data 
                                            // block is submitted to the engine 
                                            // For GCM: -[127:0] - AES_IV - For 
                                            // GCM operations, these registers 
                                            // must be written with a new 
                                            // 128-bit IV. After an operation, 
                                            // these registers contain the 
                                            // updated 128-bit result IV, 
                                            // generated by the EIP-120t. Note 
                                            // that bits [127:96] of the IV 
                                            // represent the initial counter 
                                            // value (which is 1 for GCM) and 
                                            // must therefore be initialized to 
                                            // 0x01000000. This value is 
                                            // incremented with 0x1: After 
                                            // first use - When a new data 
                                            // block is submitted to the 
                                            // engine. For CCM: -[127:0] - A0: 
                                            // For CCM this field must be 
                                            // written with value A0, this 
                                            // value is the concatenation of: 
                                            // A0-flags (5-bits of 0 and 3-bits 
                                            // 'L'), Nonce and counter value. 
                                            // 'L' must be a copy from the 'L' 
                                            // value of the AES_CTRL register. 
                                            // This 'L' indicates the width of 
                                            // the Nonce and counter. The 
                                            // loaded counter must be 
                                            // initialized to 0. The total 
                                            // width of A0 is 128-bit. For 
                                            // CBC-MAC: -[127:0] - Zeroes - For 
                                            // CBC-MAC this register must be 
                                            // written with 0s at the start of 
                                            // each operation. After an 
                                            // operation, these registers 
                                            // contain the 128-bit TAG output, 
                                            // generated by the EIP-120t. 
#define AES_AES_IV_3_AES_IV_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the AES_AES_CTRL register.
//
//*****************************************************************************
#define AES_AES_CTRL_context_ready \
                                0x80000000  // If 1, this read-only status bit 
                                            // indicates that the context data 
                                            // registers can be overwritten and 
                                            // the host is permitted to write 
                                            // the next context. 

#define AES_AES_CTRL_context_ready_M \
                                0x80000000
#define AES_AES_CTRL_context_ready_S 31
#define AES_AES_CTRL_saved_context_ready \
                                0x40000000  // If 1, this status bit indicates 
                                            // that an AES authentication TAG 
                                            // and/or IV block(s) is/are 
                                            // available for the host to 
                                            // retrieve. This bit is only 
                                            // asserted if the save_context bit 
                                            // is set to 1. The bit is mutual 
                                            // exclusive with the context_ready 
                                            // bit. Writing one clears the bit 
                                            // to 0, indicating the AES core 
                                            // can start its next operation. 
                                            // This bit is also cleared when 
                                            // the 4th word of the output TAG 
                                            // and/or IV is read. Note: All 
                                            // other mode bit writes are 
                                            // ignored when this mode bit is 
                                            // written with 1. Note: This bit 
                                            // is controlled automatically by 
                                            // the EIP-120t for TAG read DMA 
                                            // operations. 

#define AES_AES_CTRL_saved_context_ready_M \
                                0x40000000
#define AES_AES_CTRL_saved_context_ready_S 30
#define AES_AES_CTRL_save_context \
                                0x20000000  // This bit indicates that an 
                                            // authentication TAG or result IV 
                                            // needs to be stored as a result 
                                            // context. Typically this bit must 
                                            // be set for authentication modes 
                                            // returning a TAG (CBC-MAC, GCM 
                                            // and CCM), or for basic 
                                            // encryption modes that require 
                                            // future continuation with the 
                                            // current result IV. If this bit 
                                            // is set, the engine retains its 
                                            // full context until the TAG 
                                            // and/or IV registers are read. 
                                            // The TAG or IV must be read 
                                            // before the AES engine can start 
                                            // a new operation. 

#define AES_AES_CTRL_save_context_M \
                                0x20000000
#define AES_AES_CTRL_save_context_S 29
#define AES_AES_CTRL_CCM_M_M    0x01C00000  // Defines M, which indicates the 
                                            // length of the authentication 
                                            // field for CCM operations; the 
                                            // authentication field length 
                                            // equals two times (the value of 
                                            // CCM-M plus one). Note: The 
                                            // EIP-120t always returns a 
                                            // 128-bit authentication field, of 
                                            // which the M least significant 
                                            // bytes are valid. All values are 
                                            // supported. 
#define AES_AES_CTRL_CCM_M_S    22
#define AES_AES_CTRL_CCM_L_M    0x00380000  // Defines L, which indicates the 
                                            // width of the length field for 
                                            // CCM operations; the length field 
                                            // in bytes equals the value of 
                                            // CMM-L plus one. All values are 
                                            // supported. 
#define AES_AES_CTRL_CCM_L_S    19
#define AES_AES_CTRL_CCM        0x00040000  // If set to 1, AES-CCM is 
                                            // selected AES-CCM is a combined 
                                            // mode, using AES for 
                                            // authentication and encryption. 
                                            // Note: Selecting AES-CCM mode 
                                            // requires writing of the AAD 
                                            // length register after all other 
                                            // registers. Note: The CTR mode 
                                            // bit in this register must also 
                                            // be set to 1 to enable AES-CTR; 
                                            // selecting other AES modes than 
                                            // CTR mode is invalid. 
#define AES_AES_CTRL_CCM_M      0x00040000
#define AES_AES_CTRL_CCM_S      18
#define AES_AES_CTRL_GCM_M      0x00030000  // Set these bits to 11 to select 
                                            // AES-GCM mode. AES-GCM is a 
                                            // combined mode, using the Galois 
                                            // field multiplier GF(2 to the 
                                            // power of 128) for authentication 
                                            // and AES-CTR mode for encryption. 
                                            // Note: The CTR mode bit in this 
                                            // register must also be set to 1 
                                            // to enable AES-CTR Bit 
                                            // combination description: 00 = No 
                                            // GCM mode 01 = Reserved, do not 
                                            // select 10 = Reserved, do not 
                                            // select 11 = Autonomous GHASH 
                                            // (both H- and Y0-encrypted 
                                            // calculated internally) Note: The 
                                            // EIP-120t-1 configuration only 
                                            // supports mode 11 (autonomous 
                                            // GHASH), other GCM modes are not 
                                            // allowed. 
#define AES_AES_CTRL_GCM_S      16
#define AES_AES_CTRL_CBC_MAC    0x00008000  // Set to 1 to select AES-CBC MAC 
                                            // mode. The direction bit must be 
                                            // set to 1 for this mode. 
                                            // Selecting this mode requires 
                                            // writing the length register 
                                            // after all other registers. 
#define AES_AES_CTRL_CBC_MAC_M  0x00008000
#define AES_AES_CTRL_CBC_MAC_S  15
#define AES_AES_CTRL_ctr_width_M \
                                0x00000180  // Specifies the counter width for 
                                            // AES-CTR mode 00 = 32-bit counter 
                                            // 01 = 64-bit counter 10 = 96-bit 
                                            // counter 11 = 128-bit counter 

#define AES_AES_CTRL_ctr_width_S 7
#define AES_AES_CTRL_CTR        0x00000040  // If set to 1, AES counter mode 
                                            // (CTR) is selected. Note: This 
                                            // bit must also be set for GCM and 
                                            // CCM, when encryption/decryption 
                                            // is required. 
#define AES_AES_CTRL_CTR_M      0x00000040
#define AES_AES_CTRL_CTR_S      6
#define AES_AES_CTRL_CBC        0x00000020  // If set to 1, 
                                            // cipher-block-chaining (CBC) mode 
                                            // is selected. 
#define AES_AES_CTRL_CBC_M      0x00000020
#define AES_AES_CTRL_CBC_S      5
#define AES_AES_CTRL_key_size_M 0x00000018  // This read-only field specifies 
                                            // the key size. The key size is 
                                            // automatically configured when a 
                                            // new key is loaded through the 
                                            // key store module. 00 = N/A - 
                                            // Reserved 01 = 128-bit 10 = 
                                            // 192-bit 11 = 256-bit 
#define AES_AES_CTRL_key_size_S 3
#define AES_AES_CTRL_direction  0x00000004  // If set to 1 an encrypt 
                                            // operation is performed. If set 
                                            // to 0 a decrypt operation is 
                                            // performed. This bit must be 
                                            // written with a 1 when CBC-MAC is 
                                            // selected. 
#define AES_AES_CTRL_direction_M \
                                0x00000004
#define AES_AES_CTRL_direction_S 2
#define AES_AES_CTRL_input_ready \
                                0x00000002  // If 1, this status bit indicates 
                                            // that the 16-byte AES input 
                                            // buffer is empty. The host is 
                                            // permitted to write the next 
                                            // block of data. Writing 0 clears 
                                            // the bit to 0 and indicates that 
                                            // the AES core can use the 
                                            // provided input data block. 
                                            // Writing 1 to this bit is 
                                            // ignored. Note: For DMA 
                                            // operations, this bit is 
                                            // automatically controlled by the 
                                            // EIP-120t. After reset, this bit 
                                            // is 0. After writing a context, 
                                            // this bit becomes 1. 

#define AES_AES_CTRL_input_ready_M \
                                0x00000002
#define AES_AES_CTRL_input_ready_S 1
#define AES_AES_CTRL_output_ready \
                                0x00000001  // If 1, this status bit indicates 
                                            // that an AES output block is 
                                            // available to be retrieved by the 
                                            // host. Writing 0 clears the bit 
                                            // to 0 and indicates that output 
                                            // data is read by the host. The 
                                            // AES core can provide a next 
                                            // output data block. Writing 1 to 
                                            // this bit is ignored. Note: For 
                                            // DMA operations, this bit is 
                                            // automatically controlled by the 
                                            // EIP-120t. 

#define AES_AES_CTRL_output_ready_M \
                                0x00000001
#define AES_AES_CTRL_output_ready_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_C_LENGTH_0 register.
//
//*****************************************************************************
#define AES_AES_C_LENGTH_0_C_LENGTH_M \
                                0xFFFFFFFF  // C_LENGTH[31:0] Bits [60:0] of 
                                            // the crypto length registers (LSW 
                                            // and MSW) store the cryptographic 
                                            // data length in bytes for all 
                                            // modes. Once processing with this 
                                            // context is started, this length 
                                            // decrements to 0. Data lengths up 
                                            // to (261: 1) bytes are allowed. 
                                            // For GCM, any value up to 236 - 
                                            // 32 bytes can be used. This is 
                                            // because a 32-bit counter mode is 
                                            // used; the maximum number of 
                                            // 128-bit blocks is 232 - 2, 
                                            // resulting in a maximum number of 
                                            // bytes of 236 - 32. A write to 
                                            // this register triggers the 
                                            // engine to start using this 
                                            // context. This is valid for all 
                                            // modes except GCM and CCM. Note: 
                                            // For the combined modes (GCM and 
                                            // CCM), this length does not 
                                            // include the authentication only 
                                            // data; the authentication length 
                                            // is specified in the 
                                            // AES_AUTH_LENGTH register below. 
                                            // All modes must have a length 
                                            // greater than 0. For the combined 
                                            // modes, it is allowed to have one 
                                            // of the lengths equal to 0. For 
                                            // the basic encryption modes (ECB, 
                                            // CBC, and CTR) it is allowed to 
                                            // program zero to the length 
                                            // field; in that case the length 
                                            // is assumed infinite. All data 
                                            // must be byte (8-bit) aligned for 
                                            // stream cipher modes; bit aligned 
                                            // data streams are not supported 
                                            // by the EIP-120t. For block 
                                            // cipher modes, the data length 
                                            // must be programmed in multiples 
                                            // of the block cipher size, 16 
                                            // bytes. For a host read 
                                            // operation, these registers 
                                            // return all-0s. 

#define AES_AES_C_LENGTH_0_C_LENGTH_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_C_LENGTH_1 register.
//
//*****************************************************************************
#define AES_AES_C_LENGTH_1_C_LENGTH_M \
                                0x1FFFFFFF  // C_LENGTH[60:32] Bits [60:0] of 
                                            // the crypto length registers (LSW 
                                            // and MSW) store the cryptographic 
                                            // data length in bytes for all 
                                            // modes. Once processing with this 
                                            // context is started, this length 
                                            // decrements to 0. Data lengths up 
                                            // to (261: 1) bytes are allowed. 
                                            // For GCM, any value up to 236 - 
                                            // 32 bytes can be used. This is 
                                            // because a 32-bit counter mode is 
                                            // used; the maximum number of 
                                            // 128-bit blocks is 232 - 2, 
                                            // resulting in a maximum number of 
                                            // bytes of 236 - 32. A write to 
                                            // this register triggers the 
                                            // engine to start using this 
                                            // context. This is valid for all 
                                            // modes except GCM and CCM. Note: 
                                            // For the combined modes (GCM and 
                                            // CCM), this length does not 
                                            // include the authentication only 
                                            // data; the authentication length 
                                            // is specified in the 
                                            // AES_AUTH_LENGTH register below. 
                                            // All modes must have a length 
                                            // greater than 0. For the combined 
                                            // modes, it is allowed to have one 
                                            // of the lengths equal to 0. For 
                                            // the basic encryption modes (ECB, 
                                            // CBC, and CTR) it is allowed to 
                                            // program zero to the length 
                                            // field; in that case the length 
                                            // is assumed infinite. All data 
                                            // must be byte (8-bit) aligned for 
                                            // stream cipher modes; bit aligned 
                                            // data streams are not supported 
                                            // by the EIP-120t. For block 
                                            // cipher modes, the data length 
                                            // must be programmed in multiples 
                                            // of the block cipher size, 16 
                                            // bytes. For a host read 
                                            // operation, these registers 
                                            // return all-0s. 

#define AES_AES_C_LENGTH_1_C_LENGTH_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_AUTH_LENGTH register.
//
//*****************************************************************************
#define AES_AES_AUTH_LENGTH_AUTH_LENGTH_M \
                                0xFFFFFFFF  // Bits [31:0] of the 
                                            // authentication length register 
                                            // store the authentication data 
                                            // length in bytes for combined 
                                            // modes only (GCM or CCM). 
                                            // Supported AAD-lengths for CCM 
                                            // are from 0 to (2^16 - 2^8) 
                                            // bytes. For GCM any value up to 
                                            // (2^32 - 1) bytes can be used. 
                                            // Once processing with this 
                                            // context is started, this length 
                                            // decrements to 0. A write to this 
                                            // register triggers the engine to 
                                            // start using this context for GCM 
                                            // and CCM. For a host read 
                                            // operation, these registers 
                                            // return all-0s. 

#define AES_AES_AUTH_LENGTH_AUTH_LENGTH_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_DATA_IN_OUT_0 register.
//
//*****************************************************************************
#define AES_AES_DATA_IN_OUT_0_AES_DATA_IN_OUT_M \
                                0xFFFFFFFF  // AES input data[31:0] / AES 
                                            // output data[31:0] Data registers 
                                            // for input/output block data 
                                            // to/from the EIP-120t. For normal 
                                            // operations, this register is not 
                                            // used, since data input and 
                                            // output is transferred from and 
                                            // to the AES core via DMA. For a 
                                            // host write operation, these 
                                            // registers must be written with 
                                            // the 128-bit input block for the 
                                            // next AES operation. Writing at a 
                                            // word-aligned offset within this 
                                            // address range stores the word (4 
                                            // bytes) of data into the 
                                            // corresponding position of 4-word 
                                            // deep (16 bytes = 128-bit AES 
                                            // block) data input buffer. This 
                                            // buffer is used for the next AES 
                                            // operation. If the last data 
                                            // block is not completely filled 
                                            // with valid data (see notes 
                                            // below), it is allowed to write 
                                            // only the words with valid data. 
                                            // Next AES operation is triggered 
                                            // by writing to the input_ready 
                                            // flag of the AES_CTRL register. 
                                            // For a host read operation, these 
                                            // registers contain the 128-bit 
                                            // output block from the latest AES 
                                            // operation. Reading from a 
                                            // word-aligned offset within this 
                                            // address range reads one word (4 
                                            // bytes) of data out the 4-word 
                                            // deep (16 bytes = 128-bits AES 
                                            // block) data output buffer. The 
                                            // words (4 words, one full block) 
                                            // should be read before the core 
                                            // will move the next block to the 
                                            // data output buffer. To empty the 
                                            // data output buffer, the 
                                            // output_ready flag of the 
                                            // AES_CTRL register must be 
                                            // written. For the modes with 
                                            // authentication (CBC-MAC, GCM and 
                                            // CCM), the invalid (message) 
                                            // bytes/words can be written with 
                                            // any data. Note: AES typically 
                                            // operates on 128 bits block 
                                            // multiple input data. The CTR, 
                                            // GCM and CCM modes form an 
                                            // exception. The last block of a 
                                            // CTR-mode message may contain 
                                            // less than 128 bits (refer to 
                                            // [NIST 800-38A]). For GCM/CCM, 
                                            // the last block of both AAD and 
                                            // message data may contain less 
                                            // than 128 bits (refer to [NIST 
                                            // 800-38D]). The EIP-120t 
                                            // automatically pads or masks 
                                            // misaligned ending data blocks 
                                            // with 0s for GCM, CCM and 
                                            // CBC-MAC. For CTR mode, the 
                                            // remaining data in an unaligned 
                                            // data block is ignored. Note: The 
                                            // AAD / authentication only data 
                                            // is not copied to the output 
                                            // buffer but only used for 
                                            // authentication. 

#define AES_AES_DATA_IN_OUT_0_AES_DATA_IN_OUT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_DATA_IN_OUT_1 register.
//
//*****************************************************************************
#define AES_AES_DATA_IN_OUT_1_AES_DATA_IN_OUT_M \
                                0xFFFFFFFF  // AES input data[63:32] / AES 
                                            // output data[63:32] Data 
                                            // registers for input/output block 
                                            // data to/from the EIP-120t. For 
                                            // normal operations, this register 
                                            // is not used, since data input 
                                            // and output is transferred from 
                                            // and to the AES core via DMA. For 
                                            // a host write operation, these 
                                            // registers must be written with 
                                            // the 128-bit input block for the 
                                            // next AES operation. Writing at a 
                                            // word-aligned offset within this 
                                            // address range stores the word (4 
                                            // bytes) of data into the 
                                            // corresponding position of 4-word 
                                            // deep (16 bytes = 128-bit AES 
                                            // block) data input buffer. This 
                                            // buffer is used for the next AES 
                                            // operation. If the last data 
                                            // block is not completely filled 
                                            // with valid data (see notes 
                                            // below), it is allowed to write 
                                            // only the words with valid data. 
                                            // Next AES operation is triggered 
                                            // by writing to the input_ready 
                                            // flag of the AES_CTRL register. 
                                            // For a host read operation, these 
                                            // registers contain the 128-bit 
                                            // output block from the latest AES 
                                            // operation. Reading from a 
                                            // word-aligned offset within this 
                                            // address range reads one word (4 
                                            // bytes) of data out the 4-word 
                                            // deep (16 bytes = 128-bits AES 
                                            // block) data output buffer. The 
                                            // words (4 words, one full block) 
                                            // should be read before the core 
                                            // will move the next block to the 
                                            // data output buffer. To empty the 
                                            // data output buffer, the 
                                            // output_ready flag of the 
                                            // AES_CTRL register must be 
                                            // written. For the modes with 
                                            // authentication (CBC-MAC, GCM and 
                                            // CCM), the invalid (message) 
                                            // bytes/words can be written with 
                                            // any data. Note: AES typically 
                                            // operates on 128 bits block 
                                            // multiple input data. The CTR, 
                                            // GCM and CCM modes form an 
                                            // exception. The last block of a 
                                            // CTR-mode message may contain 
                                            // less than 128 bits (refer to 
                                            // [NIST 800-38A]). For GCM/CCM, 
                                            // the last block of both AAD and 
                                            // message data may contain less 
                                            // than 128 bits (refer to [NIST 
                                            // 800-38D]). The EIP-120t 
                                            // automatically pads or masks 
                                            // misaligned ending data blocks 
                                            // with 0s for GCM, CCM and 
                                            // CBC-MAC. For CTR mode, the 
                                            // remaining data in an unaligned 
                                            // data block is ignored. Note: The 
                                            // AAD / authentication only data 
                                            // is not copied to the output 
                                            // buffer but only used for 
                                            // authentication. 

#define AES_AES_DATA_IN_OUT_1_AES_DATA_IN_OUT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_DATA_IN_OUT_2 register.
//
//*****************************************************************************
#define AES_AES_DATA_IN_OUT_2_AES_DATA_IN_OUT_M \
                                0xFFFFFFFF  // AES input data[95:64] / AES 
                                            // output data[95:64] Data 
                                            // registers for input/output block 
                                            // data to/from the EIP-120t. For 
                                            // normal operations, this register 
                                            // is not used, since data input 
                                            // and output is transferred from 
                                            // and to the AES core via DMA. For 
                                            // a host write operation, these 
                                            // registers must be written with 
                                            // the 128-bit input block for the 
                                            // next AES operation. Writing at a 
                                            // word-aligned offset within this 
                                            // address range stores the word (4 
                                            // bytes) of data into the 
                                            // corresponding position of 4-word 
                                            // deep (16 bytes = 128-bit AES 
                                            // block) data input buffer. This 
                                            // buffer is used for the next AES 
                                            // operation. If the last data 
                                            // block is not completely filled 
                                            // with valid data (see notes 
                                            // below), it is allowed to write 
                                            // only the words with valid data. 
                                            // Next AES operation is triggered 
                                            // by writing to the input_ready 
                                            // flag of the AES_CTRL register. 
                                            // For a host read operation, these 
                                            // registers contain the 128-bit 
                                            // output block from the latest AES 
                                            // operation. Reading from a 
                                            // word-aligned offset within this 
                                            // address range reads one word (4 
                                            // bytes) of data out the 4-word 
                                            // deep (16 bytes = 128-bits AES 
                                            // block) data output buffer. The 
                                            // words (4 words, one full block) 
                                            // should be read before the core 
                                            // will move the next block to the 
                                            // data output buffer. To empty the 
                                            // data output buffer, the 
                                            // output_ready flag of the 
                                            // AES_CTRL register must be 
                                            // written. For the modes with 
                                            // authentication (CBC-MAC, GCM and 
                                            // CCM), the invalid (message) 
                                            // bytes/words can be written with 
                                            // any data. Note: AES typically 
                                            // operates on 128 bits block 
                                            // multiple input data. The CTR, 
                                            // GCM and CCM modes form an 
                                            // exception. The last block of a 
                                            // CTR-mode message may contain 
                                            // less than 128 bits (refer to 
                                            // [NIST 800-38A]). For GCM/CCM, 
                                            // the last block of both AAD and 
                                            // message data may contain less 
                                            // than 128 bits (refer to [NIST 
                                            // 800-38D]). The EIP-120t 
                                            // automatically pads or masks 
                                            // misaligned ending data blocks 
                                            // with 0s for GCM, CCM and 
                                            // CBC-MAC. For CTR mode, the 
                                            // remaining data in an unaligned 
                                            // data block is ignored. Note: The 
                                            // AAD / authentication only data 
                                            // is not copied to the output 
                                            // buffer but only used for 
                                            // authentication. 

#define AES_AES_DATA_IN_OUT_2_AES_DATA_IN_OUT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_DATA_IN_OUT_3 register.
//
//*****************************************************************************
#define AES_AES_DATA_IN_OUT_3_AES_DATA_IN_OUT_M \
                                0xFFFFFFFF  // AES input data[127:96] / AES 
                                            // output data[127:96] Data 
                                            // registers for input/output block 
                                            // data to/from the EIP-120t. For 
                                            // normal operations, this register 
                                            // is not used, since data input 
                                            // and output is transferred from 
                                            // and to the AES core via DMA. For 
                                            // a host write operation, these 
                                            // registers must be written with 
                                            // the 128-bit input block for the 
                                            // next AES operation. Writing at a 
                                            // word-aligned offset within this 
                                            // address range stores the word (4 
                                            // bytes) of data into the 
                                            // corresponding position of 4-word 
                                            // deep (16 bytes = 128-bit AES 
                                            // block) data input buffer. This 
                                            // buffer is used for the next AES 
                                            // operation. If the last data 
                                            // block is not completely filled 
                                            // with valid data (see notes 
                                            // below), it is allowed to write 
                                            // only the words with valid data. 
                                            // Next AES operation is triggered 
                                            // by writing to the input_ready 
                                            // flag of the AES_CTRL register. 
                                            // For a host read operation, these 
                                            // registers contain the 128-bit 
                                            // output block from the latest AES 
                                            // operation. Reading from a 
                                            // word-aligned offset within this 
                                            // address range reads one word (4 
                                            // bytes) of data out the 4-word 
                                            // deep (16 bytes = 128-bits AES 
                                            // block) data output buffer. The 
                                            // words (4 words, one full block) 
                                            // should be read before the core 
                                            // will move the next block to the 
                                            // data output buffer. To empty the 
                                            // data output buffer, the 
                                            // output_ready flag of the 
                                            // AES_CTRL register must be 
                                            // written. For the modes with 
                                            // authentication (CBC-MAC, GCM and 
                                            // CCM), the invalid (message) 
                                            // bytes/words can be written with 
                                            // any data. Note: AES typically 
                                            // operates on 128 bits block 
                                            // multiple input data. The CTR, 
                                            // GCM and CCM modes form an 
                                            // exception. The last block of a 
                                            // CTR-mode message may contain 
                                            // less than 128 bits (refer to 
                                            // [NIST 800-38A]). For GCM/CCM, 
                                            // the last block of both AAD and 
                                            // message data may contain less 
                                            // than 128 bits (refer to [NIST 
                                            // 800-38D]). The EIP-120t 
                                            // automatically pads or masks 
                                            // misaligned ending data blocks 
                                            // with 0s for GCM, CCM and 
                                            // CBC-MAC. For CTR mode, the 
                                            // remaining data in an unaligned 
                                            // data block is ignored. Note: The 
                                            // AAD / authentication only data 
                                            // is not copied to the output 
                                            // buffer but only used for 
                                            // authentication. 

#define AES_AES_DATA_IN_OUT_3_AES_DATA_IN_OUT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_TAG_OUT_0 register.
//
//*****************************************************************************
#define AES_AES_TAG_OUT_0_AES_TAG_M \
                                0xFFFFFFFF  // AES_TAG[31:0] Bits [31:0] of 
                                            // the AES_TAG registers store the 
                                            // authentication value for the 
                                            // combined and authentication only 
                                            // modes. For a host read 
                                            // operation, these registers 
                                            // contain the last 128-bit TAG 
                                            // output of the EIP-120t; the TAG 
                                            // is available until the next 
                                            // context is written. This 
                                            // register will only contain valid 
                                            // data if the TAG is available and 
                                            // when the store_ready bit from 
                                            // AES_CTRL register is set. During 
                                            // processing or for 
                                            // operations/modes that do not 
                                            // return a TAG, reads from this 
                                            // register return data from the IV 
                                            // register. 

#define AES_AES_TAG_OUT_0_AES_TAG_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_TAG_OUT_1 register.
//
//*****************************************************************************
#define AES_AES_TAG_OUT_1_AES_TAG_M \
                                0xFFFFFFFF  // AES_TAG[63:32] For a host read 
                                            // operation, these registers 
                                            // contain the last 128-bit TAG 
                                            // output of the EIP-120t; the TAG 
                                            // is available until the next 
                                            // context is written. This 
                                            // register contains valid data 
                                            // only if the TAG is available and 
                                            // when the store_ready bit from 
                                            // AES_CTRL register is set. During 
                                            // processing or for 
                                            // operations/modes that do not 
                                            // return a TAG, reads from this 
                                            // register return data from the IV 
                                            // register. 

#define AES_AES_TAG_OUT_1_AES_TAG_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_TAG_OUT_2 register.
//
//*****************************************************************************
#define AES_AES_TAG_OUT_2_AES_TAG_M \
                                0xFFFFFFFF  // AES_TAG[95:64] For a host read 
                                            // operation, these registers 
                                            // contain the last 128-bit TAG 
                                            // output of the EIP-120t; the TAG 
                                            // is available until the next 
                                            // context is written. This 
                                            // register contains valid data 
                                            // only if the TAG is available and 
                                            // when the store_ready bit from 
                                            // AES_CTRL register is set. During 
                                            // processing or for 
                                            // operations/modes that do not 
                                            // return a TAG, reads from this 
                                            // register return data from the IV 
                                            // register. 

#define AES_AES_TAG_OUT_2_AES_TAG_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_AES_TAG_OUT_3 register.
//
//*****************************************************************************
#define AES_AES_TAG_OUT_3_AES_TAG_M \
                                0xFFFFFFFF  // AES_TAG[127:96] For a host read 
                                            // operation, these registers 
                                            // contain the last 128-bit TAG 
                                            // output of the EIP-120t; the TAG 
                                            // is available until the next 
                                            // context is written. This 
                                            // register contains valid data 
                                            // only if the TAG is available and 
                                            // when the store_ready bit from 
                                            // AES_CTRL register is set. During 
                                            // processing or for 
                                            // operations/modes that do not 
                                            // return a TAG, reads from this 
                                            // register return data from the IV 
                                            // register. 

#define AES_AES_TAG_OUT_3_AES_TAG_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_0 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_0_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[31:0] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_0_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_1 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_1_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[63:32] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_1_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_2 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_2_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[95:64] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_2_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_3 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_3_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[127:96] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_3_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_4 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_4_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[159:128] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_4_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_5 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_5_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[191:160] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_5_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_6 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_6_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[223:192] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_6_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_7 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_7_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[255:224] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_7_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_8 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_8_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[287:256] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_8_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_9 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_9_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[319:288] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_9_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_10 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_10_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[351:320] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_10_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_11 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_11_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[383:352] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_11_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_12 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_12_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[415:384] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_12_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_13 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_13_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[447:416] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_13_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_14 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_14_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[479:448] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_14_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DATA_IN_15 register.
//
//*****************************************************************************
#define AES_HASH_DATA_IN_15_HASH_DATA_IN_M \
                                0xFFFFFFFF  // HASH_DATA_IN[511:480] These 
                                            // registers must be written with 
                                            // the 512-bit input data. The data 
                                            // lines are connected directly to 
                                            // the data input of the hash 
                                            // module and hence into the 
                                            // engine's internal data buffer. 
                                            // Writing to each of the registers 
                                            // triggers a corresponding 32-bit 
                                            // write enable to the internal 
                                            // buffer. Note: The host may only 
                                            // write the input data buffer when 
                                            // the rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL register is 
                                            // high. If the rfd_in bit is 0, 
                                            // the engine is busy with 
                                            // processing. During processing, 
                                            // it is not allowed to write new 
                                            // input data. For message lengths 
                                            // larger than 64 bytes, multiple 
                                            // blocks of data are written to 
                                            // this input buffer using a 
                                            // handshake through flags of the 
                                            // HASH_IO_BUF_CTRL register. All 
                                            // blocks except the last are 
                                            // required to be 512 bits in size. 
                                            // If the last block is not 512 
                                            // bits long, only the least 
                                            // significant bits of data must be 
                                            // written, but they must be padded 
                                            // with 0s to the next 32-bit 
                                            // boundary. Host read operations 
                                            // from these register addresses 
                                            // return 0s. 

#define AES_HASH_DATA_IN_15_HASH_DATA_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_IO_BUF_CTRL register.
//
//*****************************************************************************
#define AES_HASH_IO_BUF_CTRL_PAD_DMA_MESSAGE \
                                0x00000080  // Note: This bit must only be 
                                            // used when data is supplied 
                                            // through the DMA. It should not 
                                            // be used when data is supplied 
                                            // through the slave interface. 
                                            // This bit indicates whether the 
                                            // hash engine has to pad the 
                                            // message, received through the 
                                            // DMA and finalize the hash. When 
                                            // set to 1, the hash engine pads 
                                            // the last block using the 
                                            // programmed length. After 
                                            // padding, the final hash result 
                                            // is calculated. When set to 0, 
                                            // the hash engine treats the last 
                                            // written block as block-size 
                                            // aligned and calculates the 
                                            // intermediate digest. This bit is 
                                            // automatically cleared when the 
                                            // last DMA data block is arrived 
                                            // in the hash engine. 

#define AES_HASH_IO_BUF_CTRL_PAD_DMA_MESSAGE_M \
                                0x00000080
#define AES_HASH_IO_BUF_CTRL_PAD_DMA_MESSAGE_S 7
#define AES_HASH_IO_BUF_CTRL_GET_DIGEST \
                                0x00000040  // Note: The bit description below 
                                            // is only applicable when data is 
                                            // sent through the slave 
                                            // interface. This bit must be set 
                                            // to 0 when data is received 
                                            // through the DMA. This bit 
                                            // indicates whether the hash 
                                            // engine should provide the hash 
                                            // digest. When provided 
                                            // simultaneously with data_in_av, 
                                            // the hash digest is provided 
                                            // after processing the data that 
                                            // is currently in the HASH_DATA_IN 
                                            // register. When provided without 
                                            // data_in_av, the current internal 
                                            // digest buffer value is copied to 
                                            // the HASH_DIGEST_n registers. The 
                                            // host must write a 1 to this bit 
                                            // to make the intermediate hash 
                                            // digest available. Writing 0 to 
                                            // this bit has no effect. This bit 
                                            // is automatically cleared (that 
                                            // is, reads 0) when the hash 
                                            // engine has processed the 
                                            // contents of the HASH_DATA_IN 
                                            // register. In the period between 
                                            // this bit is set by the host and 
                                            // the actual HASH_DATA_IN 
                                            // processing, this bit reads 1. 

#define AES_HASH_IO_BUF_CTRL_GET_DIGEST_M \
                                0x00000040
#define AES_HASH_IO_BUF_CTRL_GET_DIGEST_S 6
#define AES_HASH_IO_BUF_CTRL_PAD_MESSAGE \
                                0x00000020  // Note: The bit description below 
                                            // is only applicable when data is 
                                            // sent through the slave 
                                            // interface. This bit must be set 
                                            // to 0 when data is received 
                                            // through the DMA. This bit 
                                            // indicates that the HASH_DATA_IN 
                                            // registers hold the last data of 
                                            // the message and hash padding 
                                            // must be applied. The host must 
                                            // write this bit to 1 in order to 
                                            // indicate to the hash engine that 
                                            // the HASH_DATA_IN register 
                                            // currently holds the last data of 
                                            // the message. When pad_message is 
                                            // set to 1, the hash engine will 
                                            // add padding bits to the data 
                                            // currently in the HASH_DATA_IN 
                                            // register. When the last message 
                                            // block is smaller than 512 bits, 
                                            // the pad_message bit must be set 
                                            // to 1 together with the 
                                            // data_in_av bit. When the last 
                                            // message block is equal to 512 
                                            // bits, pad_message may be set 
                                            // together with data_in_av. In 
                                            // this case the pad_message bit 
                                            // may also be set after the last 
                                            // data block has been written to 
                                            // the hash engine (so when the 
                                            // rfd_in bit has become 1 again 
                                            // after writing the last data 
                                            // block). Writing 0 to this bit 
                                            // has no effect. This bit is 
                                            // automatically cleared (i.e. 
                                            // reads 0) by the hash engine. 
                                            // This bit reads 1 between the 
                                            // time it was set by the host and 
                                            // the hash engine interpreted its 
                                            // value. 

#define AES_HASH_IO_BUF_CTRL_PAD_MESSAGE_M \
                                0x00000020
#define AES_HASH_IO_BUF_CTRL_PAD_MESSAGE_S 5
#define AES_HASH_IO_BUF_CTRL_RFD_IN \
                                0x00000004  // Note: The bit description below 
                                            // is only applicable when data is 
                                            // sent through the slave 
                                            // interface. This bit can be 
                                            // ignored when data is received 
                                            // through the DMA. Read-only 
                                            // status of the input buffer of 
                                            // the hash engine. When 1, the 
                                            // input buffer of the hash engine 
                                            // can accept new data; the 
                                            // HASH_DATA_IN registers can 
                                            // safely be populated with new 
                                            // data. When 0, the input buffer 
                                            // of the hash engine is processing 
                                            // the data that is currently in 
                                            // HASH_DATA_IN; writing new data 
                                            // to these registers is not 
                                            // allowed. 

#define AES_HASH_IO_BUF_CTRL_RFD_IN_M \
                                0x00000004
#define AES_HASH_IO_BUF_CTRL_RFD_IN_S 2
#define AES_HASH_IO_BUF_CTRL_DATA_IN_AV \
                                0x00000002  // Note: The bit description below 
                                            // is only applicable when data is 
                                            // sent through the slave 
                                            // interface. This bit must be set 
                                            // to 0 when data is received 
                                            // through the DMA. This bit 
                                            // indicates that the HASH_DATA_IN 
                                            // registers contain new input data 
                                            // for processing. The host must 
                                            // write a 1 to this bit to start 
                                            // processing the data in 
                                            // HASH_DATA_IN; the hash engine 
                                            // will process the new data as 
                                            // soon as it is ready for it 
                                            // (rfd_in bit is 1). Writing 0 to 
                                            // this bit has no effect. This bit 
                                            // is automatically cleared (i.e. 
                                            // reads as 0) when the hash engine 
                                            // starts processing the 
                                            // HASH_DATA_IN contents. This bit 
                                            // reads 1 between the time it was 
                                            // set by the host and the hash 
                                            // engine actually starts 
                                            // processing the input data block. 

#define AES_HASH_IO_BUF_CTRL_DATA_IN_AV_M \
                                0x00000002
#define AES_HASH_IO_BUF_CTRL_DATA_IN_AV_S 1
#define AES_HASH_IO_BUF_CTRL_OUTPUT_FULL \
                                0x00000001  // Indicates that the output 
                                            // buffer registers (HASH_DIGEST_n) 
                                            // are available for reading by the 
                                            // host. When this bit reads 0, the 
                                            // output buffer registers are 
                                            // released; the hash engine is 
                                            // allowed to write new data to it. 
                                            // In this case, the registers 
                                            // should not be read by the host. 
                                            // When this bit reads 1, the hash 
                                            // engine has stored the result of 
                                            // the latest hash operation in the 
                                            // output buffer registers. As long 
                                            // as this bit reads 1, the host 
                                            // may read output buffer registers 
                                            // and the hash engine is prevented 
                                            // from writing new data to the 
                                            // output buffer. After retrieving 
                                            // the hash result data from the 
                                            // output buffer, the host must 
                                            // write a 1 to this bit to clear 
                                            // it. This makes the digest output 
                                            // buffer available for the hash 
                                            // engine to store new hash 
                                            // results. Writing 0 to this bit 
                                            // has no effect. Note: If this bit 
                                            // is asserted (1) no new operation 
                                            // should be started before the 
                                            // digest is retrieved from the 
                                            // hash engine and this bit is 
                                            // cleared (0). 

#define AES_HASH_IO_BUF_CTRL_OUTPUT_FULL_M \
                                0x00000001
#define AES_HASH_IO_BUF_CTRL_OUTPUT_FULL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_MODE_IN register.
//
//*****************************************************************************
#define AES_HASH_MODE_IN_SHA256_MODE \
                                0x00000008  // The host must write this bit 
                                            // with 1 before processing a hash 
                                            // session. 

#define AES_HASH_MODE_IN_SHA256_MODE_M \
                                0x00000008
#define AES_HASH_MODE_IN_SHA256_MODE_S 3
#define AES_HASH_MODE_IN_NEW_HASH \
                                0x00000001  // When set to 1, it indicates 
                                            // that the hash engine must start 
                                            // processing a new hash session. 
                                            // The HASH_DIGEST_n registers will 
                                            // automatically be loaded with the 
                                            // initial hash algorithm constants 
                                            // of the selected hash algorithm. 
                                            // When this bit is 0 while the 
                                            // hash processing is started, the 
                                            // initial hash algorithm constants 
                                            // are not loaded in the 
                                            // HASH_DIGEST_n registers. The 
                                            // hash engine will start 
                                            // processing with the digest that 
                                            // is currently in its internal 
                                            // HASH_DIGEST_n registers. This 
                                            // bit is automatically cleared 
                                            // when hash processing is started. 

#define AES_HASH_MODE_IN_NEW_HASH_M \
                                0x00000001
#define AES_HASH_MODE_IN_NEW_HASH_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_LENGTH_IN_L register.
//
//*****************************************************************************
#define AES_HASH_LENGTH_IN_L_LENGTH_IN_M \
                                0xFFFFFFFF  // LENGTH_IN[31:0] Message length 
                                            // registers. The content of these 
                                            // registers is used by the hash 
                                            // engine during the message 
                                            // padding phase of the hash 
                                            // session. The data lines of this 
                                            // registers are directly connected 
                                            // to the interface of the hash 
                                            // engine. For a write operation by 
                                            // the host, these registers should 
                                            // be written with the message 
                                            // length in bits. Final hash 
                                            // operations: The total input data 
                                            // length must be programmed for 
                                            // new hash operations that require 
                                            // finalization (padding). The 
                                            // input data must be provided 
                                            // through the slave or DMA 
                                            // interface. Continued hash 
                                            // operations (finalized): For 
                                            // continued hash operations that 
                                            // require finalization, the total 
                                            // message length must be 
                                            // programmed, including the length 
                                            // of previously hashed data that 
                                            // corresponds to the written input 
                                            // digest. Non-final hash 
                                            // operations: For hash operations 
                                            // that do not require finalization 
                                            // (input data length is multiple 
                                            // of 512-bits which is SHA-256 
                                            // data block size), the length 
                                            // field does not need to be 
                                            // programmed since not used by the 
                                            // operation. If the message length 
                                            // in bits is below (2^32-1), then 
                                            // only HASH_LENGTH_IN_L needs to 
                                            // be written. The hardware 
                                            // automatically sets 
                                            // HASH_LENGTH_IN_H to 0s in this 
                                            // case. The host may write the 
                                            // length register at any time 
                                            // during the hash session when the 
                                            // rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL is high. The 
                                            // length register must be written 
                                            // before the last data of the 
                                            // active hash session is written 
                                            // into the hash engine. host read 
                                            // operations from these register 
                                            // locations will return 0s. Note: 
                                            // When getting data from DMA, this 
                                            // register must be programmed 
                                            // before DMA is programmed to 
                                            // start. 

#define AES_HASH_LENGTH_IN_L_LENGTH_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_LENGTH_IN_H register.
//
//*****************************************************************************
#define AES_HASH_LENGTH_IN_H_LENGTH_IN_M \
                                0xFFFFFFFF  // LENGTH_IN[63:32] Message length 
                                            // registers. The content of these 
                                            // registers is used by the hash 
                                            // engine during the message 
                                            // padding phase of the hash 
                                            // session. The data lines of this 
                                            // registers are directly connected 
                                            // to the interface of the hash 
                                            // engine. For a write operation by 
                                            // the host, these registers should 
                                            // be written with the message 
                                            // length in bits. Final hash 
                                            // operations: The total input data 
                                            // length must be programmed for 
                                            // new hash operations that require 
                                            // finalization (padding). The 
                                            // input data must be provided 
                                            // through the slave or DMA 
                                            // interface. Continued hash 
                                            // operations (finalized): For 
                                            // continued hash operations that 
                                            // require finalization, the total 
                                            // message length must be 
                                            // programmed, including the length 
                                            // of previously hashed data that 
                                            // corresponds to the written input 
                                            // digest. Non-final hash 
                                            // operations: For hash operations 
                                            // that do not require finalization 
                                            // (input data length is multiple 
                                            // of 512-bits which is SHA-256 
                                            // data block size), the length 
                                            // field does not need to be 
                                            // programmed since not used by the 
                                            // operation. If the message length 
                                            // in bits is below (2^32-1), then 
                                            // only HASH_LENGTH_IN_L needs to 
                                            // be written. The hardware 
                                            // automatically sets 
                                            // HASH_LENGTH_IN_H to 0s in this 
                                            // case. The host may write the 
                                            // length register at any time 
                                            // during the hash session when the 
                                            // rfd_in bit of the 
                                            // HASH_IO_BUF_CTRL is high. The 
                                            // length register must be written 
                                            // before the last data of the 
                                            // active hash session is written 
                                            // into the hash engine. host read 
                                            // operations from these register 
                                            // locations will return 0s. Note: 
                                            // When getting data from DMA, this 
                                            // register must be programmed 
                                            // before DMA is programmed to 
                                            // start. 

#define AES_HASH_LENGTH_IN_H_LENGTH_IN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DIGEST_A register.
//
//*****************************************************************************
#define AES_HASH_DIGEST_A_HASH_DIGEST_M \
                                0xFFFFFFFF  // HASH_DIGEST[31:0] Hash digest 
                                            // registers Write operation: 
                                            // Continued hash: These registers 
                                            // should be written with the 
                                            // context data, before the start 
                                            // of a resumed hash session (the 
                                            // new_hash bit in the HASH_MODE 
                                            // register is 0 when starting a 
                                            // hash session). New hash: When 
                                            // initiating a new hash session 
                                            // (the new_hash bit in the 
                                            // HASH_MODE register is high), the 
                                            // internal digest registers are 
                                            // automatically set to the SHA-256 
                                            // algorithm constant and these 
                                            // register should not be written. 
                                            // Reading from these registers 
                                            // provides the intermediate hash 
                                            // result (non-final hash 
                                            // operation) or the final hash 
                                            // result (final hash operation) 
                                            // after data processing. 

#define AES_HASH_DIGEST_A_HASH_DIGEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DIGEST_B register.
//
//*****************************************************************************
#define AES_HASH_DIGEST_B_HASH_DIGEST_M \
                                0xFFFFFFFF  // HASH_DIGEST[63:32] Hash digest 
                                            // registers Write operation: 
                                            // Continued hash: These registers 
                                            // should be written with the 
                                            // context data, before the start 
                                            // of a resumed hash session (the 
                                            // new_hash bit in the HASH_MODE 
                                            // register is 0 when starting a 
                                            // hash session). New hash: When 
                                            // initiating a new hash session 
                                            // (the new_hash bit in the 
                                            // HASH_MODE register is high), the 
                                            // internal digest registers are 
                                            // automatically set to the SHA-256 
                                            // algorithm constant and these 
                                            // register should not be written. 
                                            // Reading from these registers 
                                            // provides the intermediate hash 
                                            // result (non-final hash 
                                            // operation) or the final hash 
                                            // result (final hash operation) 
                                            // after data processing. 

#define AES_HASH_DIGEST_B_HASH_DIGEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DIGEST_C register.
//
//*****************************************************************************
#define AES_HASH_DIGEST_C_HASH_DIGEST_M \
                                0xFFFFFFFF  // HASH_DIGEST[95:64] Hash digest 
                                            // registers Write operation: 
                                            // Continued hash: These registers 
                                            // should be written with the 
                                            // context data, before the start 
                                            // of a resumed hash session (the 
                                            // new_hash bit in the HASH_MODE 
                                            // register is 0 when starting a 
                                            // hash session). New hash: When 
                                            // initiating a new hash session 
                                            // (the new_hash bit in the 
                                            // HASH_MODE register is high), the 
                                            // internal digest registers are 
                                            // automatically set to the SHA-256 
                                            // algorithm constant and these 
                                            // register should not be written. 
                                            // Reading from these registers 
                                            // provides the intermediate hash 
                                            // result (non-final hash 
                                            // operation) or the final hash 
                                            // result (final hash operation) 
                                            // after data processing. 

#define AES_HASH_DIGEST_C_HASH_DIGEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DIGEST_D register.
//
//*****************************************************************************
#define AES_HASH_DIGEST_D_HASH_DIGEST_M \
                                0xFFFFFFFF  // HASH_DIGEST[127:96] Hash digest 
                                            // registers Write operation: 
                                            // Continued hash: These registers 
                                            // should be written with the 
                                            // context data, before the start 
                                            // of a resumed hash session (the 
                                            // new_hash bit in the HASH_MODE 
                                            // register is 0 when starting a 
                                            // hash session). New hash: When 
                                            // initiating a new hash session 
                                            // (the new_hash bit in the 
                                            // HASH_MODE register is high), the 
                                            // internal digest registers are 
                                            // automatically set to the SHA-256 
                                            // algorithm constant and these 
                                            // register should not be written. 
                                            // Reading from these registers 
                                            // provides the intermediate hash 
                                            // result (non-final hash 
                                            // operation) or the final hash 
                                            // result (final hash operation) 
                                            // after data processing. 

#define AES_HASH_DIGEST_D_HASH_DIGEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DIGEST_E register.
//
//*****************************************************************************
#define AES_HASH_DIGEST_E_HASH_DIGEST_M \
                                0xFFFFFFFF  // HASH_DIGEST[159:128] Hash 
                                            // digest registers Write 
                                            // operation: Continued hash: These 
                                            // registers should be written with 
                                            // the context data, before the 
                                            // start of a resumed hash session 
                                            // (the new_hash bit in the 
                                            // HASH_MODE register is 0 when 
                                            // starting a hash session). New 
                                            // hash: When initiating a new hash 
                                            // session (the new_hash bit in the 
                                            // HASH_MODE register is high), the 
                                            // internal digest registers are 
                                            // automatically set to the SHA-256 
                                            // algorithm constant and these 
                                            // register should not be written. 
                                            // Reading from these registers 
                                            // provides the intermediate hash 
                                            // result (non-final hash 
                                            // operation) or the final hash 
                                            // result (final hash operation) 
                                            // after data processing. 

#define AES_HASH_DIGEST_E_HASH_DIGEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DIGEST_F register.
//
//*****************************************************************************
#define AES_HASH_DIGEST_F_HASH_DIGEST_M \
                                0xFFFFFFFF  // HASH_DIGEST[191:160] Hash 
                                            // digest registers Write 
                                            // operation: Continued hash: These 
                                            // registers should be written with 
                                            // the context data, before the 
                                            // start of a resumed hash session 
                                            // (the new_hash bit in the 
                                            // HASH_MODE register is 0 when 
                                            // starting a hash session). New 
                                            // hash: When initiating a new hash 
                                            // session (the new_hash bit in the 
                                            // HASH_MODE register is high), the 
                                            // internal digest registers are 
                                            // automatically set to the SHA-256 
                                            // algorithm constant and these 
                                            // register should not be written. 
                                            // Reading from these registers 
                                            // provides the intermediate hash 
                                            // result (non-final hash 
                                            // operation) or the final hash 
                                            // result (final hash operation) 
                                            // after data processing. 

#define AES_HASH_DIGEST_F_HASH_DIGEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DIGEST_G register.
//
//*****************************************************************************
#define AES_HASH_DIGEST_G_HASH_DIGEST_M \
                                0xFFFFFFFF  // HASH_DIGEST[223:192] Hash 
                                            // digest registers Write 
                                            // operation: Continued hash: These 
                                            // registers should be written with 
                                            // the context data, before the 
                                            // start of a resumed hash session 
                                            // (the new_hash bit in the 
                                            // HASH_MODE register is 0 when 
                                            // starting a hash session). New 
                                            // hash: When initiating a new hash 
                                            // session (the new_hash bit in the 
                                            // HASH_MODE register is high), the 
                                            // internal digest registers are 
                                            // automatically set to the SHA-256 
                                            // algorithm constant and these 
                                            // register should not be written. 
                                            // Reading from these registers 
                                            // provides the intermediate hash 
                                            // result (non-final hash 
                                            // operation) or the final hash 
                                            // result (final hash operation) 
                                            // after data processing. 

#define AES_HASH_DIGEST_G_HASH_DIGEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_HASH_DIGEST_H register.
//
//*****************************************************************************
#define AES_HASH_DIGEST_H_HASH_DIGEST_M \
                                0xFFFFFFFF  // HASH_DIGEST[255:224] Hash 
                                            // digest registers Write 
                                            // operation: Continued hash: These 
                                            // registers should be written with 
                                            // the context data, before the 
                                            // start of a resumed hash session 
                                            // (the new_hash bit in the 
                                            // HASH_MODE register is 0 when 
                                            // starting a hash session). New 
                                            // hash: When initiating a new hash 
                                            // session (the new_hash bit in the 
                                            // HASH_MODE register is high), the 
                                            // internal digest registers are 
                                            // automatically set to the SHA-256 
                                            // algorithm constant and these 
                                            // register should not be written. 
                                            // Reading from these registers 
                                            // provides the intermediate hash 
                                            // result (non-final hash 
                                            // operation) or the final hash 
                                            // result (final hash operation) 
                                            // after data processing. 

#define AES_HASH_DIGEST_H_HASH_DIGEST_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_ALG_SEL register.
//
//*****************************************************************************
#define AES_CTRL_ALG_SEL_TAG    0x80000000  // If this bit is cleared to 0, 
                                            // the DMA operation involves only 
                                            // data. If this bit is set, the 
                                            // DMA operation includes a TAG 
                                            // (Authentication Result / 
                                            // Digest). For SHA-256 operation, 
                                            // a DMA must be set up for both 
                                            // input data and TAG. For any 
                                            // other selected module, setting 
                                            // this bit only allows a DMA that 
                                            // reads the TAG. No data allowed 
                                            // to be transferred to or from the 
                                            // selected module via the DMA. 
#define AES_CTRL_ALG_SEL_TAG_M  0x80000000
#define AES_CTRL_ALG_SEL_TAG_S  31
#define AES_CTRL_ALG_SEL_HASH   0x00000004  // If set to one, selects the hash 
                                            // engine as destination for the 
                                            // DMA The maximum transfer size to 
                                            // DMA engine is set to 64 bytes 
                                            // for reading and 32 bytes for 
                                            // writing (the latter is only 
                                            // applicable if the hash result is 
                                            // written out through the DMA). 
#define AES_CTRL_ALG_SEL_HASH_M 0x00000004
#define AES_CTRL_ALG_SEL_HASH_S 2
#define AES_CTRL_ALG_SEL_AES    0x00000002  // If set to one, selects the AES 
                                            // engine as source/destination for 
                                            // the DMA The read and write 
                                            // maximum transfer size to the DMA 
                                            // engine is set to 16 bytes. 
#define AES_CTRL_ALG_SEL_AES_M  0x00000002
#define AES_CTRL_ALG_SEL_AES_S  1
#define AES_CTRL_ALG_SEL_KEYSTORE \
                                0x00000001  // If set to one, selects the Key 
                                            // Store as destination for the DMA 
                                            // The maximum transfer size to DMA 
                                            // engine is set to 32 bytes 
                                            // (however transfers of 16, 24 and 
                                            // 32 bytes are allowed) 

#define AES_CTRL_ALG_SEL_KEYSTORE_M \
                                0x00000001
#define AES_CTRL_ALG_SEL_KEYSTORE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_PROT_EN register.
//
//*****************************************************************************
#define AES_CTRL_PROT_EN_PROT_EN \
                                0x00000001  // If this bit is cleared to 0, 
                                            // m_h_prot[1] on the AHB mater 
                                            // interface always remains 0. If 
                                            // this bit is set to one, the 
                                            // m_h_prot[1] signal on the master 
                                            // AHB bus is asserted to 1 if an 
                                            // AHB read operation is performed, 
                                            // using DMA, with the key store 
                                            // module as destination. 

#define AES_CTRL_PROT_EN_PROT_EN_M \
                                0x00000001
#define AES_CTRL_PROT_EN_PROT_EN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_SW_RESET register.
//
//*****************************************************************************
#define AES_CTRL_SW_RESET_SW_RESET \
                                0x00000001  // If this bit is set to 1, the 
                                            // following modules are reset: - 
                                            // Master control internal state is 
                                            // reset. That includes interrupt, 
                                            // error status register, and 
                                            // result available interrupt 
                                            // generation FSM. - Key store 
                                            // module state is reset. That 
                                            // includes clearing the written 
                                            // area flags; therefore, the keys 
                                            // must be reloaded to the key 
                                            // store module. Writing 0 has no 
                                            // effect. The bit is self cleared 
                                            // after executing the reset. 

#define AES_CTRL_SW_RESET_SW_RESET_M \
                                0x00000001
#define AES_CTRL_SW_RESET_SW_RESET_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_INT_CFG register.
//
//*****************************************************************************
#define AES_CTRL_INT_CFG_LEVEL  0x00000001  // If this bit is 0, the interrupt 
                                            // output is a pulse. If this bit 
                                            // is set to 1, the interrupt is a 
                                            // level interrupt that must be 
                                            // cleared by writing the interrupt 
                                            // clear register. This bit is 
                                            // applicable for both interrupt 
                                            // output signals. 
#define AES_CTRL_INT_CFG_LEVEL_M \
                                0x00000001
#define AES_CTRL_INT_CFG_LEVEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_INT_EN register.
//
//*****************************************************************************
#define AES_CTRL_INT_EN_DMA_IN_DONE \
                                0x00000002  // If this bit is set to 0, the 
                                            // DMA input done (irq_dma_in_done) 
                                            // interrupt output is disabled and 
                                            // remains 0. If this bit is set to 
                                            // 1, the DMA input done interrupt 
                                            // output is enabled. 

#define AES_CTRL_INT_EN_DMA_IN_DONE_M \
                                0x00000002
#define AES_CTRL_INT_EN_DMA_IN_DONE_S 1
#define AES_CTRL_INT_EN_RESULT_AV \
                                0x00000001  // If this bit is set to 0, the 
                                            // result available (irq_result_av) 
                                            // interrupt output is disabled and 
                                            // remains 0. If this bit is set to 
                                            // 1, the result available 
                                            // interrupt output is enabled. 

#define AES_CTRL_INT_EN_RESULT_AV_M \
                                0x00000001
#define AES_CTRL_INT_EN_RESULT_AV_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_INT_CLR register.
//
//*****************************************************************************
#define AES_CTRL_INT_CLR_DMA_BUS_ERR \
                                0x80000000  // If 1 is written to this bit, 
                                            // the DMA bus error status is 
                                            // cleared. Writing 0 has no 
                                            // effect. 

#define AES_CTRL_INT_CLR_DMA_BUS_ERR_M \
                                0x80000000
#define AES_CTRL_INT_CLR_DMA_BUS_ERR_S 31
#define AES_CTRL_INT_CLR_KEY_ST_WR_ERR \
                                0x40000000  // If 1 is written to this bit, 
                                            // the key store write error status 
                                            // is cleared. Writing 0 has no 
                                            // effect. 

#define AES_CTRL_INT_CLR_KEY_ST_WR_ERR_M \
                                0x40000000
#define AES_CTRL_INT_CLR_KEY_ST_WR_ERR_S 30
#define AES_CTRL_INT_CLR_KEY_ST_RD_ERR \
                                0x20000000  // If 1 is written to this bit, 
                                            // the key store read error status 
                                            // is cleared. Writing 0 has no 
                                            // effect. 

#define AES_CTRL_INT_CLR_KEY_ST_RD_ERR_M \
                                0x20000000
#define AES_CTRL_INT_CLR_KEY_ST_RD_ERR_S 29
#define AES_CTRL_INT_CLR_DMA_IN_DONE \
                                0x00000002  // If 1 is written to this bit, 
                                            // the DMA in done 
                                            // (irq_dma_in_done) interrupt 
                                            // output is cleared. Writing 0 has 
                                            // no effect. Note that clearing an 
                                            // interrupt makes sense only if 
                                            // the interrupt output is 
                                            // programmed as level (refer to 
                                            // CTRL_INT_CFG). 

#define AES_CTRL_INT_CLR_DMA_IN_DONE_M \
                                0x00000002
#define AES_CTRL_INT_CLR_DMA_IN_DONE_S 1
#define AES_CTRL_INT_CLR_RESULT_AV \
                                0x00000001  // If 1 is written to this bit, 
                                            // the result available 
                                            // (irq_result_av) interrupt output 
                                            // is cleared. Writing 0 has no 
                                            // effect. Note that clearing an 
                                            // interrupt makes sense only if 
                                            // the interrupt output is 
                                            // programmed as level (refer to 
                                            // CTRL_INT_CFG). 

#define AES_CTRL_INT_CLR_RESULT_AV_M \
                                0x00000001
#define AES_CTRL_INT_CLR_RESULT_AV_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_INT_SET register.
//
//*****************************************************************************
#define AES_CTRL_INT_SET_DMA_IN_DONE \
                                0x00000002  // If 1 is written to this bit, 
                                            // the DMA data in done 
                                            // (irq_dma_in_done) interrupt 
                                            // output is set to one. Writing 0 
                                            // has no effect. If the interrupt 
                                            // configuration register is 
                                            // programmed to pulse, clearing 
                                            // the DMA data in done 
                                            // (irq_dma_in_done) interrupt is 
                                            // not needed. If it is programmed 
                                            // to level, clearing the interrupt 
                                            // output should be done by writing 
                                            // the interrupt clear register 
                                            // (CTRL_INT_CLR). 

#define AES_CTRL_INT_SET_DMA_IN_DONE_M \
                                0x00000002
#define AES_CTRL_INT_SET_DMA_IN_DONE_S 1
#define AES_CTRL_INT_SET_RESULT_AV \
                                0x00000001  // If 1 is written to this bit, 
                                            // the result available 
                                            // (irq_result_av) interrupt output 
                                            // is set to one. Writing 0 has no 
                                            // effect. If the interrupt 
                                            // configuration register is 
                                            // programmed to pulse, clearing 
                                            // the result available 
                                            // (irq_result_av) interrupt is not 
                                            // needed. If it is programmed to 
                                            // level, clearing the interrupt 
                                            // output should be done by writing 
                                            // the interrupt clear register 
                                            // (CTRL_INT_CLR). 

#define AES_CTRL_INT_SET_RESULT_AV_M \
                                0x00000001
#define AES_CTRL_INT_SET_RESULT_AV_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_INT_STAT register.
//
//*****************************************************************************
#define AES_CTRL_INT_STAT_DMA_BUS_ERR \
                                0x80000000  // This bit is set when a DMA bus 
                                            // error is detected during a DMA 
                                            // operation. The value of this 
                                            // register is held until it is 
                                            // cleared through the CTRL_INT_CLR 
                                            // register. Note: This error is 
                                            // asserted if an error is detected 
                                            // on the AHB master interface 
                                            // during a DMA operation. 

#define AES_CTRL_INT_STAT_DMA_BUS_ERR_M \
                                0x80000000
#define AES_CTRL_INT_STAT_DMA_BUS_ERR_S 31
#define AES_CTRL_INT_STAT_KEY_ST_WR_ERR \
                                0x40000000  // This bit is set when a write 
                                            // error is detected during the DMA 
                                            // write operation to the key store 
                                            // memory. The value of this 
                                            // register is held until it is 
                                            // cleared through the CTRL_INT_CLR 
                                            // register. Note: This error is 
                                            // asserted if a DMA operation does 
                                            // not cover a full key area or 
                                            // more areas are written than 
                                            // expected. 

#define AES_CTRL_INT_STAT_KEY_ST_WR_ERR_M \
                                0x40000000
#define AES_CTRL_INT_STAT_KEY_ST_WR_ERR_S 30
#define AES_CTRL_INT_STAT_KEY_ST_RD_ERR \
                                0x20000000  // This bit is set when a read 
                                            // error is detected during the 
                                            // read of a key from the key 
                                            // store, while copying it to the 
                                            // AES core. The value of this 
                                            // register is held until it is 
                                            // cleared through the CTRL_INT_CLR 
                                            // register. Note: This error is 
                                            // asserted if a key location is 
                                            // selected in the key store that 
                                            // is not available. 

#define AES_CTRL_INT_STAT_KEY_ST_RD_ERR_M \
                                0x20000000
#define AES_CTRL_INT_STAT_KEY_ST_RD_ERR_S 29
#define AES_CTRL_INT_STAT_DMA_IN_DONE \
                                0x00000002  // This read only bit returns the 
                                            // actual DMA data in done 
                                            // (irq_data_in_done) interrupt 
                                            // status of the DMA data in done 
                                            // interrupt output pin 
                                            // (irq_data_in_done). 

#define AES_CTRL_INT_STAT_DMA_IN_DONE_M \
                                0x00000002
#define AES_CTRL_INT_STAT_DMA_IN_DONE_S 1
#define AES_CTRL_INT_STAT_RESULT_AV \
                                0x00000001  // This read only bit returns the 
                                            // actual result available 
                                            // (irq_result_av) interrupt status 
                                            // of the result available 
                                            // interrupt output pin 
                                            // (irq_result_av). 

#define AES_CTRL_INT_STAT_RESULT_AV_M \
                                0x00000001
#define AES_CTRL_INT_STAT_RESULT_AV_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_OPTIONS register.
//
//*****************************************************************************
#define AES_CTRL_OPTIONS_TYPE_M 0xFF000000  // This field is 0x01 for the 
                                            // TYPE1 device. 
#define AES_CTRL_OPTIONS_TYPE_S 24
#define AES_CTRL_OPTIONS_AHBINTERFACE \
                                0x00010000  // AHB interface is available If 
                                            // this bit is 0, the EIP-120t has 
                                            // a TCM interface. 

#define AES_CTRL_OPTIONS_AHBINTERFACE_M \
                                0x00010000
#define AES_CTRL_OPTIONS_AHBINTERFACE_S 16
#define AES_CTRL_OPTIONS_SHA_256 \
                                0x00000100  // The HASH core supports SHA-256. 

#define AES_CTRL_OPTIONS_SHA_256_M \
                                0x00000100
#define AES_CTRL_OPTIONS_SHA_256_S 8
#define AES_CTRL_OPTIONS_AES_CCM \
                                0x00000080  // AES-CCM is available as a 
                                            // single operation. 

#define AES_CTRL_OPTIONS_AES_CCM_M \
                                0x00000080
#define AES_CTRL_OPTIONS_AES_CCM_S 7
#define AES_CTRL_OPTIONS_AES_GCM \
                                0x00000040  // AES-GCM is available as a 
                                            // single operation. 

#define AES_CTRL_OPTIONS_AES_GCM_M \
                                0x00000040
#define AES_CTRL_OPTIONS_AES_GCM_S 6
#define AES_CTRL_OPTIONS_AES_256 \
                                0x00000020  // AES core supports 256-bit keys 
                                            // Note: If both AES-128 and 
                                            // AES-256 are set to one, the AES 
                                            // core supports 192-bit keys as 
                                            // well. 

#define AES_CTRL_OPTIONS_AES_256_M \
                                0x00000020
#define AES_CTRL_OPTIONS_AES_256_S 5
#define AES_CTRL_OPTIONS_AES_128 \
                                0x00000010  // AES core supports 128-bit keys. 

#define AES_CTRL_OPTIONS_AES_128_M \
                                0x00000010
#define AES_CTRL_OPTIONS_AES_128_S 4
#define AES_CTRL_OPTIONS_HASH   0x00000004  // HASH Core is available. 
#define AES_CTRL_OPTIONS_HASH_M 0x00000004
#define AES_CTRL_OPTIONS_HASH_S 2
#define AES_CTRL_OPTIONS_AES    0x00000002  // AES core is available. 
#define AES_CTRL_OPTIONS_AES_M  0x00000002
#define AES_CTRL_OPTIONS_AES_S  1
#define AES_CTRL_OPTIONS_KEYSTORE \
                                0x00000001  // KEY STORE is available. 

#define AES_CTRL_OPTIONS_KEYSTORE_M \
                                0x00000001
#define AES_CTRL_OPTIONS_KEYSTORE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// AES_CTRL_VERSION register.
//
//*****************************************************************************
#define AES_CTRL_VERSION_MAJOR_VERSION_M \
                                0x0F000000  // Major version number 

#define AES_CTRL_VERSION_MAJOR_VERSION_S 24
#define AES_CTRL_VERSION_MINOR_VERSION_M \
                                0x00F00000  // Minor version number 

#define AES_CTRL_VERSION_MINOR_VERSION_S 20
#define AES_CTRL_VERSION_PATCH_LEVEL_M \
                                0x000F0000  // Patch level Starts at 0 at 
                                            // first delivery of this version 

#define AES_CTRL_VERSION_PATCH_LEVEL_S 16
#define AES_CTRL_VERSION_EIP_NUMBER_COMPL_M \
                                0x0000FF00  // These bits simply contain the 
                                            // complement of bits [7:0] (0x87), 
                                            // used by a driver to ascertain 
                                            // that the EIP-120t register is 
                                            // indeed read. 

#define AES_CTRL_VERSION_EIP_NUMBER_COMPL_S 8
#define AES_CTRL_VERSION_EIP_NUMBER_M \
                                0x000000FF  // These bits encode the EIP 
                                            // number for the EIP-120t, this 
                                            // field contains the value 120 
                                            // (decimal) or 0x78. 

#define AES_CTRL_VERSION_EIP_NUMBER_S 0


#endif // __HW_AES_H__

