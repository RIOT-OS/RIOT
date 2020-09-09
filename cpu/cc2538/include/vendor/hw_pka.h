/******************************************************************************
*  Filename:       hw_pka.h
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

#ifndef __HW_PKA_H__
#define __HW_PKA_H__

//*****************************************************************************
//
// The following are defines for the PKA register offsets.
//
//*****************************************************************************
#define PKA_APTR                0x44004000  // PKA vector A address During 
                                            // execution of basic PKCP 
                                            // operations, this register is 
                                            // double buffered and can be 
                                            // written with a new value for the 
                                            // next operation; when not 
                                            // written, the value remains 
                                            // intact. During the execution of 
                                            // sequencer-controlled complex 
                                            // operations, this register may 
                                            // not be written and its value is 
                                            // undefined at the conclusion of 
                                            // the operation. The driver 
                                            // software cannot rely on the 
                                            // written value to remain intact. 
#define PKA_BPTR                0x44004004  // PKA vector B address During 
                                            // execution of basic PKCP 
                                            // operations, this register is 
                                            // double buffered and can be 
                                            // written with a new value for the 
                                            // next operation; when not 
                                            // written, the value remains 
                                            // intact. During the execution of 
                                            // sequencer-controlled complex 
                                            // operations, this register may 
                                            // not be written and its value is 
                                            // undefined at the conclusion of 
                                            // the operation. The driver 
                                            // software cannot rely on the 
                                            // written value to remain intact. 
#define PKA_CPTR                0x44004008  // PKA vector C address During 
                                            // execution of basic PKCP 
                                            // operations, this register is 
                                            // double buffered and can be 
                                            // written with a new value for the 
                                            // next operation; when not 
                                            // written, the value remains 
                                            // intact. During the execution of 
                                            // sequencer-controlled complex 
                                            // operations, this register may 
                                            // not be written and its value is 
                                            // undefined at the conclusion of 
                                            // the operation. The driver 
                                            // software cannot rely on the 
                                            // written value to remain intact. 
#define PKA_DPTR                0x4400400C  // PKA vector D address During 
                                            // execution of basic PKCP 
                                            // operations, this register is 
                                            // double buffered and can be 
                                            // written with a new value for the 
                                            // next operation; when not 
                                            // written, the value remains 
                                            // intact. During the execution of 
                                            // sequencer-controlled complex 
                                            // operations, this register may 
                                            // not be written and its value is 
                                            // undefined at the conclusion of 
                                            // the operation. The driver 
                                            // software cannot rely on the 
                                            // written value to remain intact. 
#define PKA_ALENGTH             0x44004010  // PKA vector A length During 
                                            // execution of basic PKCP 
                                            // operations, this register is 
                                            // double buffered and can be 
                                            // written with a new value for the 
                                            // next operation; when not 
                                            // written, the value remains 
                                            // intact. During the execution of 
                                            // sequencer-controlled complex 
                                            // operations, this register may 
                                            // not be written and its value is 
                                            // undefined at the conclusion of 
                                            // the operation. The driver 
                                            // software cannot rely on the 
                                            // written value to remain intact. 
#define PKA_BLENGTH             0x44004014  // PKA vector B length During 
                                            // execution of basic PKCP 
                                            // operations, this register is 
                                            // double buffered and can be 
                                            // written with a new value for the 
                                            // next operation; when not 
                                            // written, the value remains 
                                            // intact. During the execution of 
                                            // sequencer-controlled complex 
                                            // operations, this register may 
                                            // not be written and its value is 
                                            // undefined at the conclusion of 
                                            // the operation. The driver 
                                            // software cannot rely on the 
                                            // written value to remain intact. 
#define PKA_SHIFT               0x44004018  // PKA bit shift value For basic 
                                            // PKCP operations, modifying the 
                                            // contents of this register is 
                                            // made impossible while the 
                                            // operation is being performed. 
                                            // For the ExpMod-variable and 
                                            // ExpMod-CRT operations, this 
                                            // register is used to indicate the 
                                            // number of odd powers to use 
                                            // (directly as a value in the 
                                            // range 1-16). For the ModInv and 
                                            // ECC operations, this register is 
                                            // used to hold a completion code. 
#define PKA_FUNCTION            0x4400401C  // PKA function This register 
                                            // contains the control bits to 
                                            // start basic PKCP as well as 
                                            // complex sequencer operations. 
                                            // The run bit can be used to poll 
                                            // for the completion of the 
                                            // operation. Modifying bits [11:0] 
                                            // is made impossible during the 
                                            // execution of a basic PKCP 
                                            // operation. During the execution 
                                            // of sequencer-controlled complex 
                                            // operations, this register is 
                                            // modified; the run and stall 
                                            // result bits are set to zero at 
                                            // the conclusion, but other bits 
                                            // are undefined. Attention: 
                                            // Continuously reading this 
                                            // register to poll the run bit is 
                                            // not allowed when executing 
                                            // complex sequencer operations 
                                            // (the sequencer cannot access the 
                                            // PKCP when this is done). Leave 
                                            // at least one sysclk cycle 
                                            // between poll operations. 
#define PKA_COMPARE             0x44004020  // PKA compare result This 
                                            // register provides the result of 
                                            // a basic PKCP compare operation. 
                                            // It is updated when the run bit 
                                            // in the PKA_FUNCTION register is 
                                            // reset at the end of that 
                                            // operation. Status after a 
                                            // complex sequencer operation is 
                                            // unknown 
#define PKA_MSW                 0x44004024  // PKA most-significant-word of 
                                            // result vector This register 
                                            // indicates the (word) address in 
                                            // the PKA RAM where the most 
                                            // significant nonzero 32-bit word 
                                            // of the result is stored. Should 
                                            // be ignored for modulo 
                                            // operations. For basic PKCP 
                                            // operations, this register is 
                                            // updated when the run bit in the 
                                            // PKA_FUNCTION register is reset 
                                            // at the end of the operation. For 
                                            // the complex-sequencer controlled 
                                            // operations, updating of the 
                                            // final value matching the actual 
                                            // result is done near the end of 
                                            // the operation; note that the 
                                            // result is only meaningful if no 
                                            // errors were detected and that 
                                            // for ECC operations, the PKA_MSW 
                                            // register will provide 
                                            // information for the x-coordinate 
                                            // of the result point only. 
#define PKA_DIVMSW              0x44004028  // PKA most-significant-word of 
                                            // divide remainder This register 
                                            // indicates the (32-bit word) 
                                            // address in the PKA RAM where the 
                                            // most significant nonzero 32-bit 
                                            // word of the remainder result for 
                                            // the basic divide and modulo 
                                            // operations is stored. Bits [4:0] 
                                            // are loaded with the bit number 
                                            // of the most-significant nonzero 
                                            // bit in the most-significant 
                                            // nonzero word when MS one control 
                                            // bit is set. For divide, modulo, 
                                            // and MS one reporting, this 
                                            // register is updated when the RUN 
                                            // bit in the PKA_FUNCTION register 
                                            // is reset at the end of the 
                                            // operation. For the complex 
                                            // sequencer controlled operations, 
                                            // updating of bits [4:0] of this 
                                            // register with the 
                                            // most-significant bit location of 
                                            // the actual result is done near 
                                            // the end of the operation. The 
                                            // result is meaningful only if no 
                                            // errors were detected and that 
                                            // for ECC operations; the 
                                            // PKA_DIVMSW register provides 
                                            // information for the x-coordinate 
                                            // of the result point only. 
#define PKA_SEQ_CTRL            0x440040C8  // PKA sequencer control and 
                                            // status register The sequencer is 
                                            // interfaced with the outside 
                                            // world through a single control 
                                            // and status register. With the 
                                            // exception of bit [31], the 
                                            // actual use of bits in the 
                                            // separate sub-fields of this 
                                            // register is determined by the 
                                            // sequencer firmware. This 
                                            // register need only be accessed 
                                            // when the sequencer program is 
                                            // stored in RAM. The reset value 
                                            // of the RESTE bit depends upon 
                                            // the option chosen for sequencer 
                                            // program storage. 
#define PKA_OPTIONS             0x440040F4  // PKA hardware options register 
                                            // This register provides the host 
                                            // with a means to determine the 
                                            // hardware configuration 
                                            // implemented in this PKA engine, 
                                            // focused on options that have an 
                                            // effect on software interacting 
                                            // with the module. Note: (32 x 
                                            // (1st LNME nr. of PEs + 1st LNME 
                                            // FIFO RAM depth - 10)) equals the 
                                            // maximum modulus vector length 
                                            // (in bits) that can be handled by 
                                            // the modular exponentiation and 
                                            // ECC operations executed on a PKA 
                                            // engine that includes an LNME. 
#define PKA_SW_REV              0x440040F8  // PKA firmware revision and 
                                            // capabilities register This 
                                            // register allows the host access 
                                            // to the internal firmware 
                                            // revision number of the PKA 
                                            // Engine for software driver 
                                            // matching and diagnostic 
                                            // purposes. This register also 
                                            // contains a field that encodes 
                                            // the capabilities of the embedded 
                                            // firmware. The PKA_SW_REV 
                                            // register is written by the 
                                            // firmware within a few clock 
                                            // cycles after starting up that 
                                            // firmware. The hardware reset 
                                            // value is zero, indicating that 
                                            // the information has not been 
                                            // written yet. 
#define PKA_REVISION            0x440040FC  // PKA hardware revision register 
                                            // This register allows the host 
                                            // access to the hardware revision 
                                            // number of the PKA engine for 
                                            // software driver matching and 
                                            // diagnostic purposes. It is 
                                            // always located at the highest 
                                            // address in the access space of 
                                            // the module and contains an 
                                            // encoding of the EIP number (with 
                                            // its complement as signature) for 
                                            // recognition of the hardware 
                                            // module. 


//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_APTR register.
//
//*****************************************************************************
#define PKA_APTR_APTR_M         0x000007FF  // This register specifies the 
                                            // location of vector A within the 
                                            // PKA RAM. Vectors are identified 
                                            // through the location of their 
                                            // least-significant 32-bit word. 
                                            // Note that bit [0] must be zero 
                                            // to ensure that the vector starts 
                                            // at an 8-byte boundary. 
#define PKA_APTR_APTR_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_BPTR register.
//
//*****************************************************************************
#define PKA_BPTR_BPTR_M         0x000007FF  // This register specifies the 
                                            // location of vector B within the 
                                            // PKA RAM. Vectors are identified 
                                            // through the location of their 
                                            // least-significant 32-bit word. 
                                            // Note that bit [0] must be zero 
                                            // to ensure that the vector starts 
                                            // at an 8-byte boundary. 
#define PKA_BPTR_BPTR_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_CPTR register.
//
//*****************************************************************************
#define PKA_CPTR_CPTR_M         0x000007FF  // This register specifies the 
                                            // location of vector C within the 
                                            // PKA RAM. Vectors are identified 
                                            // through the location of their 
                                            // least-significant 32-bit word. 
                                            // Note that bit [0] must be zero 
                                            // to ensure that the vector starts 
                                            // at an 8-byte boundary. 
#define PKA_CPTR_CPTR_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_DPTR register.
//
//*****************************************************************************
#define PKA_DPTR_DPTR_M         0x000007FF  // This register specifies the 
                                            // location of vector D within the 
                                            // PKA RAM. Vectors are identified 
                                            // through the location of their 
                                            // least-significant 32-bit word. 
                                            // Note that bit [0] must be zero 
                                            // to ensure that the vector starts 
                                            // at an 8-byte boundary. 
#define PKA_DPTR_DPTR_S         0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_ALENGTH register.
//
//*****************************************************************************
#define PKA_ALENGTH_ALENGTH_M   0x000001FF  // This register specifies the 
                                            // length (in 32-bit words) of 
                                            // Vector A. 
#define PKA_ALENGTH_ALENGTH_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_BLENGTH register.
//
//*****************************************************************************
#define PKA_BLENGTH_BLENGTH_M   0x000001FF  // This register specifies the 
                                            // length (in 32-bit words) of 
                                            // Vector B. 
#define PKA_BLENGTH_BLENGTH_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_SHIFT register.
//
//*****************************************************************************
#define PKA_SHIFT_NUM_BITS_TO_SHIFT_M \
                                0x0000001F  // This register specifies the 
                                            // number of bits to shift the 
                                            // input vector (in the range 0-31) 
                                            // during a Rshift or Lshift 
                                            // operation. 

#define PKA_SHIFT_NUM_BITS_TO_SHIFT_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_FUNCTION register.
//
//*****************************************************************************
#define PKA_FUNCTION_STALL_RESULT \
                                0x01000000  // When written with a 1b, 
                                            // updating of the PKA_COMPARE, 
                                            // PKA_MSW and PKA_DIVMSW 
                                            // registers, as well as resetting 
                                            // the run bit is stalled beyond 
                                            // the point that a running 
                                            // operation is actually finished. 
                                            // Use this to allow software 
                                            // enough time to read results from 
                                            // a previous operation when the 
                                            // newly started operation is known 
                                            // to take only a short amount of 
                                            // time. If a result is waiting, 
                                            // the result registers is updated 
                                            // and the run bit is reset in the 
                                            // clock cycle following writing 
                                            // the stall result bit back to 0b. 
                                            // The Stall result function may 
                                            // only be used for basic PKCP 
                                            // operations. 

#define PKA_FUNCTION_STALL_RESULT_M \
                                0x01000000
#define PKA_FUNCTION_STALL_RESULT_S 24
#define PKA_FUNCTION_RUN        0x00008000  // The host sets this bit to 
                                            // instruct the PKA module to begin 
                                            // processing the basic PKCP or 
                                            // complex sequencer operation. 
                                            // This bit is reset low 
                                            // automatically when the operation 
                                            // is complete. The complement of 
                                            // this bit is output as 
                                            // interrupts[1]. After a reset, 
                                            // the run bit is always set to 1b. 
                                            // Depending on the option, program 
                                            // ROM or program RAM, the 
                                            // following applies: Program ROM - 
                                            // The first sequencer instruction 
                                            // sets the bit to 0b. This is done 
                                            // immediately after the hardware 
                                            // reset is released. Program RAM - 
                                            // The sequencer must set the bit 
                                            // to 0b. As a valid firmware may 
                                            // not have been loaded, the 
                                            // sequencer is held in software 
                                            // reset after the hardware reset 
                                            // is released (the reset bit in 
                                            // PKA_SEQ_CRTL is set to 1b). 
                                            // After the FW image is loaded and 
                                            // the Reset bit is cleared, the 
                                            // sequencer starts to execute the 
                                            // FW. The first instruction clears 
                                            // the run bit. In both cases a few 
                                            // clock cycles are needed before 
                                            // the first instruction is 
                                            // executed and the run bit state 
                                            // has been propagated. 
#define PKA_FUNCTION_RUN_M      0x00008000
#define PKA_FUNCTION_RUN_S      15
#define PKA_FUNCTION_SEQUENCER_OPERATIONS_M \
                                0x00007000  // These bits select the complex 
                                            // sequencer operation to perform: 
                                            // 000b: None 001b: ExpMod-CRT 
                                            // 010b: ExpMod-ACT4 (compatible 
                                            // with EIP2315) 011b: ECC-ADD (if 
                                            // available in firmware, otherwise 
                                            // reserved) 100b: ExpMod-ACT2 
                                            // (compatible with EIP2316) 101b: 
                                            // ECC-MUL (if available in 
                                            // firmware, otherwise reserved) 
                                            // 110b: ExpMod-variable 111b: 
                                            // ModInv (if available in 
                                            // firmware, otherwise reserved) 
                                            // The encoding of these operations 
                                            // is determined by sequencer 
                                            // firmware. 

#define PKA_FUNCTION_SEQUENCER_OPERATIONS_S 12
#define PKA_FUNCTION_COPY       0x00000800  // Perform copy operation 
#define PKA_FUNCTION_COPY_M     0x00000800
#define PKA_FUNCTION_COPY_S     11
#define PKA_FUNCTION_COMPARE    0x00000400  // Perform compare operation 
#define PKA_FUNCTION_COMPARE_M  0x00000400
#define PKA_FUNCTION_COMPARE_S  10
#define PKA_FUNCTION_MODULO     0x00000200  // Perform modulo operation 
#define PKA_FUNCTION_MODULO_M   0x00000200
#define PKA_FUNCTION_MODULO_S   9
#define PKA_FUNCTION_DIVIDE     0x00000100  // Perform divide operation 
#define PKA_FUNCTION_DIVIDE_M   0x00000100
#define PKA_FUNCTION_DIVIDE_S   8
#define PKA_FUNCTION_LSHIFT     0x00000080  // Perform left shift operation 
#define PKA_FUNCTION_LSHIFT_M   0x00000080
#define PKA_FUNCTION_LSHIFT_S   7
#define PKA_FUNCTION_RSHIFT     0x00000040  // Perform right shift operation 
#define PKA_FUNCTION_RSHIFT_M   0x00000040
#define PKA_FUNCTION_RSHIFT_S   6
#define PKA_FUNCTION_SUBTRACT   0x00000020  // Perform subtract operation 
#define PKA_FUNCTION_SUBTRACT_M 0x00000020
#define PKA_FUNCTION_SUBTRACT_S 5
#define PKA_FUNCTION_ADD        0x00000010  // Perform add operation 
#define PKA_FUNCTION_ADD_M      0x00000010
#define PKA_FUNCTION_ADD_S      4
#define PKA_FUNCTION_MS_ONE     0x00000008  // Loads the location of the Most 
                                            // Significant one bit within the 
                                            // result word indicated in the 
                                            // PKA_MSW register into bits [4:0] 
                                            // of the PKA_DIVMSW register - can 
                                            // only be used with basic PKCP 
                                            // operations, except for Divide, 
                                            // Modulo and Compare. 
#define PKA_FUNCTION_MS_ONE_M   0x00000008
#define PKA_FUNCTION_MS_ONE_S   3
#define PKA_FUNCTION_ADDSUB     0x00000002  // Perform combined add/subtract 
                                            // operation 
#define PKA_FUNCTION_ADDSUB_M   0x00000002
#define PKA_FUNCTION_ADDSUB_S   1
#define PKA_FUNCTION_MULTIPLY   0x00000001  // Perform multiply operation 
#define PKA_FUNCTION_MULTIPLY_M 0x00000001
#define PKA_FUNCTION_MULTIPLY_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_COMPARE register.
//
//*****************************************************************************
#define PKA_COMPARE_A_GREATER_THAN_B \
                                0x00000004  // Vector_A is greater than 
                                            // Vector_B 

#define PKA_COMPARE_A_GREATER_THAN_B_M \
                                0x00000004
#define PKA_COMPARE_A_GREATER_THAN_B_S 2
#define PKA_COMPARE_A_LESS_THAN_B \
                                0x00000002  // Vector_A is less than Vector_B 

#define PKA_COMPARE_A_LESS_THAN_B_M \
                                0x00000002
#define PKA_COMPARE_A_LESS_THAN_B_S 1
#define PKA_COMPARE_A_EQUALS_B  0x00000001  // Vector_A is equal to Vector_B 
#define PKA_COMPARE_A_EQUALS_B_M \
                                0x00000001
#define PKA_COMPARE_A_EQUALS_B_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_MSW register.
//
//*****************************************************************************
#define PKA_MSW_RESULT_IS_ZERO  0x00008000  // The result vector is all 
                                            // zeroes, ignore the address 
                                            // returned in bits [10:0] 
#define PKA_MSW_RESULT_IS_ZERO_M \
                                0x00008000
#define PKA_MSW_RESULT_IS_ZERO_S 15
#define PKA_MSW_MSW_ADDRESS_M   0x000007FF  // Address of the most-significant 
                                            // nonzero 32-bit word of the 
                                            // result vector in PKA RAM 
#define PKA_MSW_MSW_ADDRESS_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_DIVMSW register.
//
//*****************************************************************************
#define PKA_DIVMSW_RESULT_IS_ZERO \
                                0x00008000  // The result vector is all 
                                            // zeroes, ignore the address 
                                            // returned in bits [10:0] 

#define PKA_DIVMSW_RESULT_IS_ZERO_M \
                                0x00008000
#define PKA_DIVMSW_RESULT_IS_ZERO_S 15
#define PKA_DIVMSW_MSW_ADDRESS_M \
                                0x000007FF  // Address of the most significant 
                                            // nonzero 32-bit word of the 
                                            // remainder result vector in PKA 
                                            // RAM 

#define PKA_DIVMSW_MSW_ADDRESS_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_SEQ_CTRL register.
//
//*****************************************************************************
#define PKA_SEQ_CTRL_RESET      0x80000000  // Option program ROM: Reset value 
                                            // = 0. Read/Write, reset value 0b 
                                            // (ZERO). Writing 1b resets the 
                                            // sequencer, write to 0b to 
                                            // restart operations again. As the 
                                            // reset value is 0b, the sequencer 
                                            // will automatically start 
                                            // operations executing from 
                                            // program ROM. This bit should 
                                            // always be written with zero and 
                                            // ignored when reading this 
                                            // register. Option Program RAM: 
                                            // Reset value =1. Read/Write, 
                                            // reset value 1b (ONE). When 1b, 
                                            // the sequencer is held in a reset 
                                            // state and the PKA_PROGRAM area 
                                            // is accessible for loading the 
                                            // sequencer program (while the 
                                            // PKA_DATA_RAM is inaccessible), 
                                            // write to 0b to (re)start 
                                            // sequencer operations and disable 
                                            // PKA_PROGRAM area accessibility 
                                            // (also enables the PKA_DATA_RAM 
                                            // accesses). Resetting the 
                                            // sequencer (in order to load 
                                            // other firmware) should only be 
                                            // done when the PKA Engine is not 
                                            // performing any operations (i.e. 
                                            // the run bit in the PKA_FUNCTION 
                                            // register should be zero). 
#define PKA_SEQ_CTRL_RESET_M    0x80000000
#define PKA_SEQ_CTRL_RESET_S    31
#define PKA_SEQ_CTRL_SEQUENCER_STATUS_M \
                                0x0000FF00  // These read-only bits can be 
                                            // used by the sequencer to 
                                            // communicate status to the 
                                            // outside world. Bit [8] is also 
                                            // used as sequencer interrupt, 
                                            // with the complement of this bit 
                                            // ORed into the run bit in 
                                            // PKA_FUNCTION. This field should 
                                            // always be written with zeroes 
                                            // and ignored when reading this 
                                            // register. 

#define PKA_SEQ_CTRL_SEQUENCER_STATUS_S 8
#define PKA_SEQ_CTRL_SW_CONTROL_STATUS_M \
                                0x000000FF  // These bits can be used by 
                                            // software to trigger sequencer 
                                            // operations. External logic can 
                                            // set these bits by writing 1b, 
                                            // cannot reset them by writing 0b. 
                                            // The sequencer can reset these 
                                            // bits by writing 0b, cannot set 
                                            // them by writing 1b. Setting the 
                                            // run bit in PKA_FUNCTION together 
                                            // with a nonzero sequencer 
                                            // operations field automatically 
                                            // sets bit [0] here. This field 
                                            // should always be written with 
                                            // zeroes and ignored when reading 
                                            // this register. 

#define PKA_SEQ_CTRL_SW_CONTROL_STATUS_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_OPTIONS register.
//
//*****************************************************************************
#define PKA_OPTIONS_FIRST_LNME_FIFO_DEPTH_M \
                                0xFF000000  // Number of words in the first 
                                            // LNME's FIFO RAM Should be 
                                            // ignored if LNME configuration is 
                                            // 0. The contents of this field 
                                            // indicate the actual depth as 
                                            // selected by the LNME FIFO RAM 
                                            // size strap input, fifo_size_sel. 
                                            // Note: Reset value is undefined 

#define PKA_OPTIONS_FIRST_LNME_FIFO_DEPTH_S 24
#define PKA_OPTIONS_FIRST_LNME_NR_OF_PES_M \
                                0x003F0000  // Number of processing elements 
                                            // in the pipeline of the first 
                                            // LNME Should be ignored if LNME 
                                            // configuration is 0. Note: Reset 
                                            // value is undefined. 

#define PKA_OPTIONS_FIRST_LNME_NR_OF_PES_S 16
#define PKA_OPTIONS_MMM3A       0x00001000  // Reserved for a future 
                                            // functional extension to the LNME 
                                            // Always 0b 
#define PKA_OPTIONS_MMM3A_M     0x00001000
#define PKA_OPTIONS_MMM3A_S     12
#define PKA_OPTIONS_INT_MASKING 0x00000800  // Value 0b indicates that the 
                                            // main interrupt output (bit [1] 
                                            // of the interrupts output bus) is 
                                            // the direct complement of the run 
                                            // bit in the PKA_CONTROL register, 
                                            // value 1b indicates that 
                                            // interrupt masking logic is 
                                            // present for this output. Note: 
                                            // Reset value is undefined 
#define PKA_OPTIONS_INT_MASKING_M \
                                0x00000800
#define PKA_OPTIONS_INT_MASKING_S 11
#define PKA_OPTIONS_PROTECTION_OPTION_M \
                                0x00000700  // Value 0 indicates no additional 
                                            // protection against side channel 
                                            // attacks, value 1 indicates the 
                                            // SCAP option, value 3 indicates 
                                            // the PROT option; other values 
                                            // are reserved. Note: Reset value 
                                            // is undefined 

#define PKA_OPTIONS_PROTECTION_OPTION_S 8
#define PKA_OPTIONS_PROGRAM_RAM 0x00000080  // Value 1b indicates sequencer 
                                            // program storage in RAM, value 0b 
                                            // in ROM. Note: Reset value is 
                                            // undefined 
#define PKA_OPTIONS_PROGRAM_RAM_M \
                                0x00000080
#define PKA_OPTIONS_PROGRAM_RAM_S 7
#define PKA_OPTIONS_SEQUENCER_CONFIGURATION_M \
                                0x00000060  // Value 1 indicates a standard 
                                            // sequencer; other values are 
                                            // reserved. 

#define PKA_OPTIONS_SEQUENCER_CONFIGURATION_S 5
#define PKA_OPTIONS_LNME_CONFIGURATION_M \
                                0x0000001C  // Value 0 indicates NO LNME, 
                                            // value 1 indicates one standard 
                                            // LNME (with alpha = 32, beta = 
                                            // 8); other values reserved. Note: 
                                            // Reset value is undefined 

#define PKA_OPTIONS_LNME_CONFIGURATION_S 2
#define PKA_OPTIONS_PKCP_CONFIGURATION_M \
                                0x00000003  // Value 1 indicates a PKCP with a 
                                            // 16x16 multiplier, value 2 
                                            // indicates a PKCP with a 32x32 
                                            // multiplier, other values 
                                            // reserved. Note: Reset value is 
                                            // undefined. 

#define PKA_OPTIONS_PKCP_CONFIGURATION_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_SW_REV register.
//
//*****************************************************************************
#define PKA_SW_REV_FW_CAPABILITIES_M \
                                0xF0000000  // 4-bit binary encoding for the 
                                            // functionality implemented in the 
                                            // firmware. Value 0 indicates 
                                            // basic ModExp with/without CRT. 
                                            // Value 1 adds Modular Inversion, 
                                            // value 2 adds Modular Inversion 
                                            // and ECC operations. Values 3-15 
                                            // are reserved. 

#define PKA_SW_REV_FW_CAPABILITIES_S 28
#define PKA_SW_REV_MAJOR_FW_REVISION_M \
                                0x0F000000  // 4-bit binary encoding of the 
                                            // major firmware revision number 

#define PKA_SW_REV_MAJOR_FW_REVISION_S 24
#define PKA_SW_REV_MINOR_FW_REVISION_M \
                                0x00F00000  // 4-bit binary encoding of the 
                                            // minor firmware revision number 

#define PKA_SW_REV_MINOR_FW_REVISION_S 20
#define PKA_SW_REV_FW_PATCH_LEVEL_M \
                                0x000F0000  // 4-bit binary encoding of the 
                                            // firmware patch level, initial 
                                            // release will carry value zero 
                                            // Patches are used to remove bugs 
                                            // without changing the 
                                            // functionality or interface of a 
                                            // module. 

#define PKA_SW_REV_FW_PATCH_LEVEL_S 16
//*****************************************************************************
//
// The following are defines for the bit fields in the PKA_REVISION register.
//
//*****************************************************************************
#define PKA_REVISION_MAJOR_HW_REVISION_M \
                                0x0F000000  // 4-bit binary encoding of the 
                                            // major hardware revision number 

#define PKA_REVISION_MAJOR_HW_REVISION_S 24
#define PKA_REVISION_MINOR_HW_REVISION_M \
                                0x00F00000  // 4-bit binary encoding of the 
                                            // minor hardware revision number 

#define PKA_REVISION_MINOR_HW_REVISION_S 20
#define PKA_REVISION_HW_PATCH_LEVEL_M \
                                0x000F0000  // 4-bit binary encoding of the 
                                            // hardware patch level, initial 
                                            // release will carry value zero 
                                            // Patches are used to remove bugs 
                                            // without changing the 
                                            // functionality or interface of a 
                                            // module. 

#define PKA_REVISION_HW_PATCH_LEVEL_S 16
#define PKA_REVISION_COMPLEMENT_OF_BASIC_EIP_NUMBER_M \
                                0x0000FF00  // Bit-by-bit logic complement of 
                                            // bits [7:0], EIP-28 gives 0xE3 

#define PKA_REVISION_COMPLEMENT_OF_BASIC_EIP_NUMBER_S 8
#define PKA_REVISION_BASIC_EIP_NUMBER_M \
                                0x000000FF  // 8-bit binary encoding of the 
                                            // EIP number, EIP-28 gives 0x1C 

#define PKA_REVISION_BASIC_EIP_NUMBER_S 0


#endif // __HW_PKA_H__

