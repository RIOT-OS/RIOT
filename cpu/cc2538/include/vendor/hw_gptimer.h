/******************************************************************************
*  Filename:       hw_gptimer.h
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

#ifndef __HW_GPTIMER_H__
#define __HW_GPTIMER_H__

//*****************************************************************************
//
// The following are defines for the GPTIMER register offsets.
//
//*****************************************************************************
#define GPTIMER_O_CFG           0x00000000  // GPTM configuration This 
                                            // register configures the global 
                                            // operation of the GPTM. The value 
                                            // written to this register 
                                            // determines whether the GPTM is 
                                            // in 32-bit mode (concatenated 
                                            // timers) or in 16-bit mode 
                                            // (individual, split timers). 
#define GPTIMER_O_TAMR          0x00000004  // GPTM Timer A mode This register 
                                            // configures the GPTM based on the 
                                            // configuration selected in the 
                                            // CFG register. This register 
                                            // controls the modes for Timer A 
                                            // when it is used individually. 
                                            // When Timer A and Timer B are 
                                            // concatenated, this register 
                                            // controls the modes for both 
                                            // Timer A and Timer B, and the 
                                            // contents of TBMR are ignored. 
#define GPTIMER_O_TBMR          0x00000008  // GPTM Timer B mode This register 
                                            // configures the GPTM based on the 
                                            // configuration selected in the 
                                            // CFG register. This register 
                                            // controls the modes for Timer B 
                                            // when it is used individually. 
                                            // When Timer A and Timer B are 
                                            // concatenated, this register is 
                                            // ignored and TBMR controls the 
                                            // modes for both Timer A and Timer 
                                            // B. 
#define GPTIMER_O_CTL           0x0000000C  // GPTM control This register is 
                                            // used alongside the CFG and TnMR 
                                            // registers to fine-tune the timer 
                                            // configuration, and to enable 
                                            // other features such as timer 
                                            // stall. 
#define GPTIMER_O_SYNC          0x00000010  // GPTM synchronize Note: This 
                                            // register is implemented on GPTM 
                                            // 0 base address only. This 
                                            // register does however, allow 
                                            // software to synchronize a number 
                                            // of timers. 
#define GPTIMER_O_IMR           0x00000018  // GPTM interrupt mask This 
                                            // register allows software to 
                                            // enable and disable GPTM 
                                            // controller-level interrupts. 
                                            // Setting a bit enables the 
                                            // corresponding interrupt, while 
                                            // clearing a bit disables it. 
#define GPTIMER_O_RIS           0x0000001C  // GPTM raw interrupt status This 
                                            // register shows the state of the 
                                            // GPTM internal interrupt signal. 
                                            // These bits are set whether or 
                                            // not the interrupt is masked in 
                                            // the IMR register. Each bit can 
                                            // be cleared by writing 1 to its 
                                            // corresponding bit in ICR. 
#define GPTIMER_O_MIS           0x00000020  // GPTM masked interrupt status 
                                            // This register shows the state of 
                                            // the GPTM controller-level 
                                            // interrupt. If an interrupt is 
                                            // unmasked in IMR, and there is an 
                                            // event that causes the interrupt 
                                            // to be asserted, the 
                                            // corresponding bit is set in this 
                                            // register. All bits are cleared 
                                            // by writing 1 to the 
                                            // corresponding bit in ICR. 
#define GPTIMER_O_ICR           0x00000024  // GPTM interrupt clear This 
                                            // register is used to clear the 
                                            // status bits in the RIS and MIS 
                                            // registers. Writing 1 to a bit 
                                            // clears the corresponding bit in 
                                            // the RIS and MIS registers. 
#define GPTIMER_O_TAILR         0x00000028  // GPTM Timer A interval load When 
                                            // the Timer is counting down, this 
                                            // register is used to load the 
                                            // starting count value into the 
                                            // Timer. When the Timer is 
                                            // counting up, this register sets 
                                            // the upper bound for the timeout 
                                            // event. When a GPTM is configured 
                                            // to one of the 32-bit modes, 
                                            // TAILR appears as a 32-bit 
                                            // register (the upper 16-bits 
                                            // correspond to the contents of 
                                            // the GPTM Timer B Interval Load 
                                            // (TBILR) register). In a 16-bit 
                                            // mode, the upper 16 bits of this 
                                            // register read as 0s and have no 
                                            // effect on the state of TBILR. 
#define GPTIMER_O_TBILR         0x0000002C  // GPTM Timer B interval load When 
                                            // the Timer is counting down, this 
                                            // register is used to load the 
                                            // starting count value into the 
                                            // Timer. When the Timer is 
                                            // counting up, this register sets 
                                            // the upper bound for the time-out 
                                            // event. When a GPTM is configured 
                                            // to one of the 32-bit modes, the 
                                            // contents of bits [15:0] in this 
                                            // register are loaded into the 
                                            // upper 16 bits of the TAILR 
                                            // register. Reads from this 
                                            // register return the current 
                                            // value of Timer B and writes are 
                                            // ignored. In a 16-bit mode, bits 
                                            // [15:0] are used for the load 
                                            // value. Bits [31:16] are reserved 
                                            // in both cases. 
#define GPTIMER_O_TAMATCHR      0x00000030  // GPTM Timer A match This 
                                            // register is loaded with a match 
                                            // value. Interrupts can be 
                                            // generated when the Timer value 
                                            // is equal to the value in this 
                                            // register in one-shot or periodic 
                                            // mode. When a GPTM is configured 
                                            // to one of the 32-bit modes, 
                                            // TAMATCHR appears as a 32-bit 
                                            // register (the upper 16-bits 
                                            // correspond to the contents of 
                                            // the GPTM Timer B match 
                                            // (GPTMTBMATCHR) register). In a 
                                            // 16-bit mode, the upper 16 bits 
                                            // of this register read as 0s and 
                                            // have no effect on the state of 
                                            // TBMATCHR. 
#define GPTIMER_O_TBMATCHR      0x00000034  // PTM Timer B match This register 
                                            // is loaded with a match value. 
                                            // Interrupts can be generated when 
                                            // the Timer value is equal to the 
                                            // value in this register in 
                                            // one-shot or periodic mode. When 
                                            // a GPTM is configured to one of 
                                            // the 32-bit modes, the contents 
                                            // of bits [15:0] in this register 
                                            // are loaded into the upper 16 
                                            // bits of the TAMATCHR register. 
                                            // Reads from this register return 
                                            // the current match value of Timer 
                                            // B and writes are ignored. In a 
                                            // 16-bit mode, bits [15:0] are 
                                            // used for the match value. Bits 
                                            // [31:16] are reserved in both 
                                            // cases. 
#define GPTIMER_O_TAPR          0x00000038  // GPTM Timer A prescale This 
                                            // register allows software to 
                                            // extend the range of the 16-bit 
                                            // Timers in periodic and one-shot 
                                            // modes. 
#define GPTIMER_O_TBPR          0x0000003C  // GPTM Timer B prescale This 
                                            // register allows software to 
                                            // extend the range of the 16-bit 
                                            // Timers in periodic and one-shot 
                                            // modes. 
#define GPTIMER_O_TAPMR         0x00000040  // GPTM Timer A prescale match 
                                            // This register effectively 
                                            // extends the range of TAMATCHR to 
                                            // 24 bits when operating in 
                                            // 16-bit, one-shot or periodic 
                                            // mode. 
#define GPTIMER_O_TBPMR         0x00000044  // GPTM Timer B prescale match 
                                            // This register effectively 
                                            // extends the range ofMTBMATCHR to 
                                            // 24 bits when operating in 
                                            // 16-bit, one-shot or periodic 
                                            // mode. 
#define GPTIMER_O_TAR           0x00000048  // GPTM Timer A This register 
                                            // shows the current value of the 
                                            // Timer A counter. When a GPTM is 
                                            // configured to one of the 32-bit 
                                            // modes, TAR appears as a 32-bit 
                                            // register (the upper 16-bits 
                                            // correspond to the contents of 
                                            // the GPTM Timer B (TBR) 
                                            // register). In the16-bit Input 
                                            // edge count, input edge time, and 
                                            // PWM modes, bits [15:0] contain 
                                            // the value of the counter and 
                                            // bits 23:16 contain the value of 
                                            // the prescaler, which is the 
                                            // upper 8 bits of the count. Bits 
                                            // [31:24] always read as 0. To 
                                            // read the value of the prescaler 
                                            // in 16-bit, one-shot and periodic 
                                            // modes, read bits [23:16] in the 
                                            // TAV register. 
#define GPTIMER_O_TBR           0x0000004C  // GPTM Timer B This register 
                                            // shows the current value of the 
                                            // Timer B counter. When a GPTM is 
                                            // configured to one of the 32-bit 
                                            // modes, the contents of bits 
                                            // [15:0] in this register are 
                                            // loaded into the upper 16 bits of 
                                            // the TAR register. Reads from 
                                            // this register return the current 
                                            // value of Timer B. In a 16-bit 
                                            // mode, bits 15:0 contain the 
                                            // value of the counter and bits 
                                            // [23:16] contain the value of the 
                                            // prescaler in Input edge count, 
                                            // input edge time, and PWM modes, 
                                            // which is the upper 8 bits of the 
                                            // count. Bits [31:24] always read 
                                            // as 0. To read the value of the 
                                            // prescaler in 16-bit, one-shot 
                                            // and periodic modes, read bits 
                                            // [23:16] in the TBV register. 
#define GPTIMER_O_TAV           0x00000050  // GPTM Timer A value When read, 
                                            // this register shows the current, 
                                            // free-running value of Timer A in 
                                            // all modes. Software can use this 
                                            // value to determine the time 
                                            // elapsed between an interrupt and 
                                            // the ISR entry when using the 
                                            // snapshot feature with the 
                                            // periodic operating mode. When 
                                            // written, the value written into 
                                            // this register is loaded into the 
                                            // TAR register on the next clock 
                                            // cycle. When a GPTM is configured 
                                            // to one of the 32-bit modes, TAV 
                                            // appears as a 32-bit register 
                                            // (the upper 16-bits correspond to 
                                            // the contents of the GPTM Timer B 
                                            // Value (TBV) register). In a 
                                            // 16-bit mode, bits [15:0] contain 
                                            // the value of the counter and 
                                            // bits [23:16] contain the 
                                            // current, free-running value of 
                                            // the prescaler, which is the 
                                            // upper 8 bits of the count in 
                                            // input edge count, input edge 
                                            // time, PWM and one-shot or 
                                            // periodic up count modes. In 
                                            // one-shot or periodic down count 
                                            // modes, the prescaler stored in 
                                            // [23:16] is a true prescaler, 
                                            // meaning bits [23:16] count down 
                                            // before decrementing the value in 
                                            // bits [15:0]. The prescaler its 
                                            // [31:24] always read as 0. 
#define GPTIMER_O_TBV           0x00000054  // GPTM Timer B value When read, 
                                            // this register shows the current, 
                                            // free-running value of Timer B in 
                                            // all modes. Software can use this 
                                            // value to determine the time 
                                            // elapsed between an interrupt and 
                                            // the ISR entry. When written, the 
                                            // value written into this register 
                                            // is loaded into the TBR register 
                                            // on the next clock cycle. When a 
                                            // GPTM is configured to one of the 
                                            // 32-bit modes, the contents of 
                                            // bits 15:0 in this register are 
                                            // loaded into the upper 16 bits of 
                                            // the TAV register. Reads from 
                                            // this register return the current 
                                            // free-running value of Timer B. 
                                            // In a 16-bit mode, bits [15:0] 
                                            // contain the value of the counter 
                                            // and bits [23:16] contain the 
                                            // current, free-running value of 
                                            // the prescaler, which is the 
                                            // upper 8 bits of the count in 
                                            // input edge count, input edge 
                                            // time, PWM and one-shot or 
                                            // periodic up count modes. In 
                                            // one-shot or periodic down count 
                                            // modes, the prescaler stored in 
                                            // [23:16] is a true prescaler, 
                                            // meaning bits [23:16] count down 
                                            // before decrementing the value in 
                                            // bits [15:0]. The prescaler its 
                                            // [31:24] always read as 0. 
#define GPTIMER_O_TAPS          0x0000005C  // GPTM Timer A prescale snapshot 
                                            // For the 32-bit wide GPTM, this 
                                            // register shows the current value 
                                            // of the Timer A prescaler in the 
                                            // 32-bit modes. This register is 
                                            // ununsed in 16-bit GPTM mode. 
#define GPTIMER_O_TBPS          0x00000060  // GPTM Timer B prescale snapshot 
                                            // For the 32-bit wide GPTM, this 
                                            // register shows the current value 
                                            // of the Timer B prescaler in the 
                                            // 32-bit modes. This register is 
                                            // ununsed in 16-bit GPTM mode. 
#define GPTIMER_O_TAPV          0x00000064  // GPTM Timer A prescale value For 
                                            // the 32-bit wide GPTM, this 
                                            // register shows the current 
                                            // free-running value of the Timer 
                                            // A prescaler in the 32-bit modes. 
                                            // Software can use this value in 
                                            // conjunction with the TAV 
                                            // register to determine the time 
                                            // elapsed between an interrupt and 
                                            // the ISR entry. This register is 
                                            // ununsed in 16- or 32-bit GPTM 
                                            // mode. 
#define GPTIMER_O_TBPV          0x00000068  // GPTM Timer B prescale value For 
                                            // the 32-bit wide GPTM, this 
                                            // register shows the current 
                                            // free-running value of the Timer 
                                            // B prescaler in the 32-bit modes. 
                                            // Software can use this value in 
                                            // conjunction with the TBV 
                                            // register to determine the time 
                                            // elapsed between an interrupt and 
                                            // the ISR entry. This register is 
                                            // ununsed in 16- or 32-bit GPTM 
                                            // mode. 
#define GPTIMER_O_PP            0x00000FC0  // GPTM peripheral properties The 
                                            // PP register provides information 
                                            // regarding the properties of the 
                                            // general-purpose Timer module. 


//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_CFG register.
//
//*****************************************************************************
#define GPTIMER_CFG_GPTMCFG_M   0x00000007  // GPTM configuration The GPTMCFG 
                                            // values are defined as follows: 
                                            // 0x0: 32-bit timer configuration. 
                                            // 0x1: 32-bit real-time clock 0x2: 
                                            // Reserved 0x3: Reserved 0x4: 
                                            // 16-bit timer configuration. The 
                                            // function is controlled by bits 
                                            // [1:0] of GPTMTAMR and GPTMTBMR. 
                                            // 0x5-0x7: Reserved 
#define GPTIMER_CFG_GPTMCFG_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TAMR register.
//
//*****************************************************************************
#define GPTIMER_TAMR_TAPLO      0x00000800  // Legacy PWM operation 0: Legacy 
                                            // operation 1: CCP is set to 1 on 
                                            // time-out. 
#define GPTIMER_TAMR_TAPLO_M    0x00000800
#define GPTIMER_TAMR_TAPLO_S    11
#define GPTIMER_TAMR_TAMRSU     0x00000400  // Timer A match register update 
                                            // mode 0: Update GPTMAMATCHR and 
                                            // GPTMAPR if used on the next 
                                            // cycle. 1: Update GPTMAMATCHR and 
                                            // GPTMAPR if used on the next 
                                            // time-out. If the timer is 
                                            // disabled (TAEN is clear) when 
                                            // this bit is set, GPTMTAMATCHR 
                                            // and GPTMTAPR are updated when 
                                            // the timer is enabled. If the 
                                            // timer is stalled (TASTALL is 
                                            // set), GPTMTAMATCHR and GPTMTAPR 
                                            // are updated according to the 
                                            // configuration of this bit. 
#define GPTIMER_TAMR_TAMRSU_M   0x00000400
#define GPTIMER_TAMR_TAMRSU_S   10
#define GPTIMER_TAMR_TAPWMIE    0x00000200  // GPTM Timer A PWM interrupt 
                                            // enable This bit enables 
                                            // interrupts in PWM mode on 
                                            // rising, falling, or both edges 
                                            // of the CCP output. 0: Interrupt 
                                            // is disabled. 1: Interrupt is 
                                            // enabled. This bit is valid only 
                                            // in PWM mode. 
#define GPTIMER_TAMR_TAPWMIE_M  0x00000200
#define GPTIMER_TAMR_TAPWMIE_S  9
#define GPTIMER_TAMR_TAILD      0x00000100  // GPTM Timer A PWM interval load 
                                            // write 0: Update the GPTMTAR 
                                            // register with the value in the 
                                            // GPTMTAILR register on the next 
                                            // cycle. If the prescaler is used, 
                                            // update the GPTMTAPS register 
                                            // with the value in the GPTMTAPR 
                                            // register on the next cycle. 1: 
                                            // Update the GPTMTAR register with 
                                            // the value in the GPTMTAILR 
                                            // register on the next cycle. If 
                                            // the prescaler is used, update 
                                            // the GPTMTAPS register with the 
                                            // value in the GPTMTAPR register 
                                            // on the next time-out. 
#define GPTIMER_TAMR_TAILD_M    0x00000100
#define GPTIMER_TAMR_TAILD_S    8
#define GPTIMER_TAMR_TASNAPS    0x00000080  // GPTM Timer A snap-shot mode 0: 
                                            // Snap-shot mode is disabled. 1: 
                                            // If Timer A is configured in 
                                            // periodic mode, the actual 
                                            // free-running value of Timer A is 
                                            // loaded at the time-out event 
                                            // into the GPTM Timer A (GPTMTAR) 
                                            // register. 
#define GPTIMER_TAMR_TASNAPS_M  0x00000080
#define GPTIMER_TAMR_TASNAPS_S  7
#define GPTIMER_TAMR_TAWOT      0x00000040  // GPTM Timer A wait-on-trigger 0: 
                                            // Timer A begins counting as soon 
                                            // as it is enabled. 1: If Timer A 
                                            // is enabled (TAEN is set in the 
                                            // GPTMCTL register), Timer A does 
                                            // not begin counting until it 
                                            // receives a trigger from the 
                                            // Timer in the previous position 
                                            // in the daisy-chain. This bit 
                                            // must be clear for GP Timer 
                                            // module 0, Timer A. 
#define GPTIMER_TAMR_TAWOT_M    0x00000040
#define GPTIMER_TAMR_TAWOT_S    6
#define GPTIMER_TAMR_TAMIE      0x00000020  // GPTM Timer A match interrupt 
                                            // enable 0: The match interrupt is 
                                            // disabled. 1: An interrupt is 
                                            // generated when the match value 
                                            // in the GPTMTAMATCHR register is 
                                            // reached in the one-shot and 
                                            // periodic modes. 
#define GPTIMER_TAMR_TAMIE_M    0x00000020
#define GPTIMER_TAMR_TAMIE_S    5
#define GPTIMER_TAMR_TACDIR     0x00000010  // GPTM Timer A count direction 0: 
                                            // The timer counts down. 1: The 
                                            // timer counts up. When counting 
                                            // up, the timer starts from a 
                                            // value of 0x0. 
#define GPTIMER_TAMR_TACDIR_M   0x00000010
#define GPTIMER_TAMR_TACDIR_S   4
#define GPTIMER_TAMR_TAAMS      0x00000008  // GPTM Timer A alternate mode 0: 
                                            // Capture mode is enabled. 1: PWM 
                                            // mode is enabled. Note: To enable 
                                            // PWM mode, the TACM bit must be 
                                            // cleared and the TAMR field must 
                                            // be configured to 0x2. 
#define GPTIMER_TAMR_TAAMS_M    0x00000008
#define GPTIMER_TAMR_TAAMS_S    3
#define GPTIMER_TAMR_TACMR      0x00000004  // GPTM Timer A capture mode 0: 
                                            // Edge-count mode 1: Edge-time 
                                            // mode 
#define GPTIMER_TAMR_TACMR_M    0x00000004
#define GPTIMER_TAMR_TACMR_S    2
#define GPTIMER_TAMR_TAMR_M     0x00000003  // GPTM Timer A mode 0x0: Reserved 
                                            // 0x1: One-shot mode 0x2: Periodic 
                                            // mode 0x3: Capture mode The timer 
                                            // mode is based on the timer 
                                            // configuration defined by bits 
                                            // [2:0] in the GPTMCFG register. 
#define GPTIMER_TAMR_TAMR_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TBMR register.
//
//*****************************************************************************
#define GPTIMER_TBMR_TBPLO      0x00000800  // Legacy PWM operation 0: Legacy 
                                            // operation 1: CCP is set to 1 on 
                                            // time-out. 
#define GPTIMER_TBMR_TBPLO_M    0x00000800
#define GPTIMER_TBMR_TBPLO_S    11
#define GPTIMER_TBMR_TBMRSU     0x00000400  // Timer B match register update 
                                            // mode 0: Update the GPTMBMATCHR 
                                            // and the GPTMBPR, if used on the 
                                            // next cycle. 1: Update the 
                                            // GPTMBMATCHR and the GPTMBPR, if 
                                            // used on the next time-out. If 
                                            // the timer is disabled (TAEN is 
                                            // clear) when this bit is set, 
                                            // GPTMTBMATCHR and GPTMTBPR are 
                                            // updated when the timer is 
                                            // enabled. If the timer is stalled 
                                            // (TBSTALL is set), GPTMTBMATCHR 
                                            // and GPTMTBPR are updated 
                                            // according to the configuration 
                                            // of this bit. 
#define GPTIMER_TBMR_TBMRSU_M   0x00000400
#define GPTIMER_TBMR_TBMRSU_S   10
#define GPTIMER_TBMR_TBPWMIE    0x00000200  // GPTM Timer B PWM interrupt 
                                            // enable This bit enables 
                                            // interrupts in PWM mode on 
                                            // rising, falling, or both edges 
                                            // of the CCP output. 0: Interrupt 
                                            // is disabled. 1: Interrupt is 
                                            // enabled. This bit is valid only 
                                            // in PWM mode. 
#define GPTIMER_TBMR_TBPWMIE_M  0x00000200
#define GPTIMER_TBMR_TBPWMIE_S  9
#define GPTIMER_TBMR_TBILD      0x00000100  // GPTM Timer B PWM interval load 
                                            // write 0: Update the GPTMTBR 
                                            // register with the value in the 
                                            // GPTMTBILR register on the next 
                                            // cycle. If the prescaler is used, 
                                            // update the GPTMTBPS register 
                                            // with the value in the GPTMTBPR 
                                            // register on the next cycle. 1: 
                                            // Update the GPTMTBR register with 
                                            // the value in the GPTMTBILR 
                                            // register on the next cycle. If 
                                            // the prescaler is used, update 
                                            // the GPTMTBPS register with the 
                                            // value in the GPTMTBPR register 
                                            // on the next time-out. 
#define GPTIMER_TBMR_TBILD_M    0x00000100
#define GPTIMER_TBMR_TBILD_S    8
#define GPTIMER_TBMR_TBSNAPS    0x00000080  // GPTM Timer B snap-shot mode 0: 
                                            // Snap-shot mode is disabled. 1: 
                                            // If Timer B is configured in the 
                                            // periodic mode, the actual 
                                            // free-running value of Timer A is 
                                            // loaded into the GPTM Timer B 
                                            // (GPTMTBR) register at the 
                                            // time-out event. 
#define GPTIMER_TBMR_TBSNAPS_M  0x00000080
#define GPTIMER_TBMR_TBSNAPS_S  7
#define GPTIMER_TBMR_TBWOT      0x00000040  // GPTM Timer B wait-on-trigger 0: 
                                            // Timer B begins counting as soon 
                                            // as it is enabled. 1: If Timer B 
                                            // is enabled (TBEN is set in the 
                                            // GPTMCTL register), Timer B does 
                                            // not begin counting until it 
                                            // receives a trigger from the 
                                            // timer in the previous position 
                                            // in the daisy-chain. 
#define GPTIMER_TBMR_TBWOT_M    0x00000040
#define GPTIMER_TBMR_TBWOT_S    6
#define GPTIMER_TBMR_TBMIE      0x00000020  // GPTM Timer B match interrupt 
                                            // enable 0: The match interrupt is 
                                            // disabled. 1: An interrupt is 
                                            // generated when the match value 
                                            // in the GPTMTBMATCHR register is 
                                            // reached in the one-shot and 
                                            // periodic modes. 
#define GPTIMER_TBMR_TBMIE_M    0x00000020
#define GPTIMER_TBMR_TBMIE_S    5
#define GPTIMER_TBMR_TBCDIR     0x00000010  // GPTM Timer B count direction 0: 
                                            // The timer counts down. 1: The 
                                            // timer counts up. When counting 
                                            // up, the timer starts from a 
                                            // value of 0x0. 
#define GPTIMER_TBMR_TBCDIR_M   0x00000010
#define GPTIMER_TBMR_TBCDIR_S   4
#define GPTIMER_TBMR_TBAMS      0x00000008  // GPTM Timer B alternate mode 0: 
                                            // Capture mode is enabled. 1: PWM 
                                            // mode is enabled. Note: To enable 
                                            // PWM mode, the TBCM bit must be 
                                            // cleared and the TBMR field must 
                                            // be configured to 0x2. 
#define GPTIMER_TBMR_TBAMS_M    0x00000008
#define GPTIMER_TBMR_TBAMS_S    3
#define GPTIMER_TBMR_TBCMR      0x00000004  // GPTM Timer B capture mode 0: 
                                            // Edge-count mode 1: Edge-time 
                                            // mode 
#define GPTIMER_TBMR_TBCMR_M    0x00000004
#define GPTIMER_TBMR_TBCMR_S    2
#define GPTIMER_TBMR_TBMR_M     0x00000003  // GPTM Timer B mode 0x0: Reserved 
                                            // 0x1: One-shot timer mode 0x2: 
                                            // Periodic timer mode 0x3: Capture 
                                            // mode The timer mode is based on 
                                            // the timer configuration defined 
                                            // by bits [2:0] in the GPTMCFG 
                                            // register. 
#define GPTIMER_TBMR_TBMR_S     0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_CTL register.
//
//*****************************************************************************
#define GPTIMER_CTL_TBPWML      0x00004000  // GPTM Timer B PWM output level 
                                            // 0: Output is unaffected. 1: 
                                            // Output is inverted. 
#define GPTIMER_CTL_TBPWML_M    0x00004000
#define GPTIMER_CTL_TBPWML_S    14
#define GPTIMER_CTL_TBOTE       0x00002000  // GPTM Timer B output trigger 
                                            // enable 0: The ADC trigger of 
                                            // output Timer B is disabled. 1: 
                                            // The ADC trigger of output Timer 
                                            // B is enabled. 
#define GPTIMER_CTL_TBOTE_M     0x00002000
#define GPTIMER_CTL_TBOTE_S     13
#define GPTIMER_CTL_TBEVENT_M   0x00000C00  // GPTM Timer B event mode 0x0: 
                                            // Positive edge 0x1: Negative edge 
                                            // 0x2: Reserved 0x3: Both edges 
#define GPTIMER_CTL_TBEVENT_S   10
#define GPTIMER_CTL_TBSTALL     0x00000200  // GPTM Timer B stall enable 0: 
                                            // Timer B continues counting while 
                                            // the processor is halted by the 
                                            // debugger. 1: Timer B freezes 
                                            // counting while the processor is 
                                            // halted by the debugger. 
#define GPTIMER_CTL_TBSTALL_M   0x00000200
#define GPTIMER_CTL_TBSTALL_S   9
#define GPTIMER_CTL_TBEN        0x00000100  // GPTM Timer B enable 0: Timer B 
                                            // is disabled. 1: Timer B is 
                                            // enabled and begins counting or 
                                            // the capture logic is enabled 
                                            // based on the GPTMCFG register. 
#define GPTIMER_CTL_TBEN_M      0x00000100
#define GPTIMER_CTL_TBEN_S      8
#define GPTIMER_CTL_TAPWML      0x00000040  // GPTM Timer A PWM output level 
                                            // 0: Output is unaffected. 1: 
                                            // Output is inverted. 
#define GPTIMER_CTL_TAPWML_M    0x00000040
#define GPTIMER_CTL_TAPWML_S    6
#define GPTIMER_CTL_TAOTE       0x00000020  // GPTM Timer A output trigger 
                                            // enable 0: The ADC trigger of 
                                            // output Timer A is disabled. 1: 
                                            // The ADC trigger of output Timer 
                                            // A is enabled. 
#define GPTIMER_CTL_TAOTE_M     0x00000020
#define GPTIMER_CTL_TAOTE_S     5
#define GPTIMER_CTL_TAEVENT_M   0x0000000C  // GPTM Timer A event mode 0x0: 
                                            // Positive edge 0x1: Negative edge 
                                            // 0x2: Reserved 0x3: Both edges 
#define GPTIMER_CTL_TAEVENT_S   2
#define GPTIMER_CTL_TASTALL     0x00000002  // GPTM Timer A stall enable 0: 
                                            // Timer A continues counting while 
                                            // the processor is halted by the 
                                            // debugger. 1: Timer A freezes 
                                            // counting while the processor is 
                                            // halted by the debugger. 
#define GPTIMER_CTL_TASTALL_M   0x00000002
#define GPTIMER_CTL_TASTALL_S   1
#define GPTIMER_CTL_TAEN        0x00000001  // GPTM Timer A enable 0: Timer A 
                                            // is disabled. 1: Timer A is 
                                            // enabled and begins counting or 
                                            // the capture logic is enabled 
                                            // based on the GPTMCFG register. 
#define GPTIMER_CTL_TAEN_M      0x00000001
#define GPTIMER_CTL_TAEN_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_SYNC register.
//
//*****************************************************************************
#define GPTIMER_SYNC_SYNC3_M    0x000000C0  // Synchronize GPTM3 0x0: GPTM3 is 
                                            // not affected. 0x1: A time-out 
                                            // event for Timer A of GPTM3 is 
                                            // triggered. 0x2: A time-out event 
                                            // for Timer B of GPTM3 is 
                                            // triggered. 0x3: A time-out event 
                                            // for Timer A and Timer B of GPTM3 
                                            // is triggered. 
#define GPTIMER_SYNC_SYNC3_S    6
#define GPTIMER_SYNC_SYNC2_M    0x00000030  // Synchronize GPTM2 0x0: GPTM2 is 
                                            // not affected. 0x1: A time-out 
                                            // event for Timer A of GPTM2 is 
                                            // triggered. 0x2: A time-out event 
                                            // for Timer B of GPTM2 is 
                                            // triggered. 0x3: A time-out event 
                                            // for Timer A and Timer B of GPTM2 
                                            // is triggered. 
#define GPTIMER_SYNC_SYNC2_S    4
#define GPTIMER_SYNC_SYNC1_M    0x0000000C  // Synchronize GPTM1 0x0: GPTM1 is 
                                            // not affected. 0x1: A time-out 
                                            // event for Timer A of GPTM1 is 
                                            // triggered. 0x2: A time-out event 
                                            // for Timer B of GPTM1 is 
                                            // triggered. 0x3: A time-out event 
                                            // for Timer A and Timer B of GPTM1 
                                            // is triggered. 
#define GPTIMER_SYNC_SYNC1_S    2
#define GPTIMER_SYNC_SYNC0_M    0x00000003  // Synchronize GPTM0 0x0: GPTM0 is 
                                            // not affected. 0x1: A time-out 
                                            // event for Timer A of GPTM0 is 
                                            // triggered. 0x2: A time-out event 
                                            // for Timer B of GPTM0 is 
                                            // triggered. 0x3: A time-out event 
                                            // for Timer A and Timer B of GPTM0 
                                            // is triggered. 
#define GPTIMER_SYNC_SYNC0_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_IMR register.
//
//*****************************************************************************
#define GPTIMER_IMR_TBMIM       0x00000800  // GPTM Timer B match interrupt 
                                            // mask 0: Interrupt is disabled. 
                                            // 1: Interrupt is enabled. 
#define GPTIMER_IMR_TBMIM_M     0x00000800
#define GPTIMER_IMR_TBMIM_S     11
#define GPTIMER_IMR_CBEIM       0x00000400  // GPTM Timer B capture event 
                                            // interrupt mask 0: Interrupt is 
                                            // disabled. 1: Interrupt is 
                                            // enabled. 
#define GPTIMER_IMR_CBEIM_M     0x00000400
#define GPTIMER_IMR_CBEIM_S     10
#define GPTIMER_IMR_CBMIM       0x00000200  // GPTM Timer B capture match 
                                            // interrupt mask 0: Interrupt is 
                                            // disabled. 1: Interrupt is 
                                            // enabled. 
#define GPTIMER_IMR_CBMIM_M     0x00000200
#define GPTIMER_IMR_CBMIM_S     9
#define GPTIMER_IMR_TBTOIM      0x00000100  // GPTM Timer B time-out interrupt 
                                            // mask 0: Interrupt is disabled. 
                                            // 1: Interrupt is enabled. 
#define GPTIMER_IMR_TBTOIM_M    0x00000100
#define GPTIMER_IMR_TBTOIM_S    8
#define GPTIMER_IMR_TAMIM       0x00000010  // GPTM Timer A match interrupt 
                                            // mask 0: Interrupt is disabled. 
                                            // 1: Interrupt is enabled. 
#define GPTIMER_IMR_TAMIM_M     0x00000010
#define GPTIMER_IMR_TAMIM_S     4
#define GPTIMER_IMR_CAEIM       0x00000004  // GPTM Timer A capture event 
                                            // interrupt mask 0: Interrupt is 
                                            // disabled. 1: Interrupt is 
                                            // enabled. 
#define GPTIMER_IMR_CAEIM_M     0x00000004
#define GPTIMER_IMR_CAEIM_S     2
#define GPTIMER_IMR_CAMIM       0x00000002  // GPTM Timer A capture match 
                                            // interrupt mask 0: Interrupt is 
                                            // disabled. 1: Interrupt is 
                                            // enabled. 
#define GPTIMER_IMR_CAMIM_M     0x00000002
#define GPTIMER_IMR_CAMIM_S     1
#define GPTIMER_IMR_TATOIM      0x00000001  // GPTM Timer A time-out interrupt 
                                            // mask 0: Interrupt is disabled. 
                                            // 1: Interrupt is enabled. 
#define GPTIMER_IMR_TATOIM_M    0x00000001
#define GPTIMER_IMR_TATOIM_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_RIS register.
//
//*****************************************************************************
#define GPTIMER_RIS_TBMRIS      0x00000800  // GPTM Timer B match raw 
                                            // interrupt 
#define GPTIMER_RIS_TBMRIS_M    0x00000800
#define GPTIMER_RIS_TBMRIS_S    11
#define GPTIMER_RIS_CBERIS      0x00000400  // GPTM Timer B capture event raw 
                                            // interrupt 
#define GPTIMER_RIS_CBERIS_M    0x00000400
#define GPTIMER_RIS_CBERIS_S    10
#define GPTIMER_RIS_CBMRIS      0x00000200  // GPTM Timer B capture match raw 
                                            // interrupt 
#define GPTIMER_RIS_CBMRIS_M    0x00000200
#define GPTIMER_RIS_CBMRIS_S    9
#define GPTIMER_RIS_TBTORIS     0x00000100  // GPTM Timer B time-out raw 
                                            // interrupt 
#define GPTIMER_RIS_TBTORIS_M   0x00000100
#define GPTIMER_RIS_TBTORIS_S   8
#define GPTIMER_RIS_TAMRIS      0x00000010  // GPTM Timer A match raw 
                                            // interrupt 
#define GPTIMER_RIS_TAMRIS_M    0x00000010
#define GPTIMER_RIS_TAMRIS_S    4
#define GPTIMER_RIS_CAERIS      0x00000004  // GPTM Timer A capture event raw 
                                            // interrupt 
#define GPTIMER_RIS_CAERIS_M    0x00000004
#define GPTIMER_RIS_CAERIS_S    2
#define GPTIMER_RIS_CAMRIS      0x00000002  // GPTM Timer A capture match raw 
                                            // interrupt 
#define GPTIMER_RIS_CAMRIS_M    0x00000002
#define GPTIMER_RIS_CAMRIS_S    1
#define GPTIMER_RIS_TATORIS     0x00000001  // GPTM Timer A time-out raw 
                                            // interrupt 
#define GPTIMER_RIS_TATORIS_M   0x00000001
#define GPTIMER_RIS_TATORIS_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_MIS register.
//
//*****************************************************************************
#define GPTIMER_MIS_TBMMIS      0x00000800  // GPTM Timer B match masked 
                                            // interrupt 
#define GPTIMER_MIS_TBMMIS_M    0x00000800
#define GPTIMER_MIS_TBMMIS_S    11
#define GPTIMER_MIS_CBEMIS      0x00000400  // GPTM Timer B capture event 
                                            // masked interrupt 
#define GPTIMER_MIS_CBEMIS_M    0x00000400
#define GPTIMER_MIS_CBEMIS_S    10
#define GPTIMER_MIS_CBMMIS      0x00000200  // GPTM Timer B capture match 
                                            // masked interrupt 
#define GPTIMER_MIS_CBMMIS_M    0x00000200
#define GPTIMER_MIS_CBMMIS_S    9
#define GPTIMER_MIS_TBTOMIS     0x00000100  // GPTM Timer B time-out masked 
                                            // interrupt 
#define GPTIMER_MIS_TBTOMIS_M   0x00000100
#define GPTIMER_MIS_TBTOMIS_S   8
#define GPTIMER_MIS_TAMRIS      0x00000010  // GPTM Timer A match raw 
                                            // interrupt 
#define GPTIMER_MIS_TAMRIS_M    0x00000010
#define GPTIMER_MIS_TAMRIS_S    4
#define GPTIMER_MIS_CAEMIS      0x00000004  // GPTM Timer A capture event raw 
                                            // interrupt 
#define GPTIMER_MIS_CAEMIS_M    0x00000004
#define GPTIMER_MIS_CAEMIS_S    2
#define GPTIMER_MIS_CAMMIS      0x00000002  // GPTM Timer A capture match raw 
                                            // interrupt 
#define GPTIMER_MIS_CAMMIS_M    0x00000002
#define GPTIMER_MIS_CAMMIS_S    1
#define GPTIMER_MIS_TATOMIS     0x00000001  // GPTM Timer A time-out raw 
                                            // interrupt 
#define GPTIMER_MIS_TATOMIS_M   0x00000001
#define GPTIMER_MIS_TATOMIS_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_ICR register.
//
//*****************************************************************************
#define GPTIMER_ICR_WUECINT     0x00010000  // GPTM write update error 
                                            // interrupt clear 
#define GPTIMER_ICR_WUECINT_M   0x00010000
#define GPTIMER_ICR_WUECINT_S   16
#define GPTIMER_ICR_TBMCINT     0x00000800  // GPTM Timer B match interrupt 
                                            // clear 
#define GPTIMER_ICR_TBMCINT_M   0x00000800
#define GPTIMER_ICR_TBMCINT_S   11
#define GPTIMER_ICR_CBECINT     0x00000400  // GPTM Timer B capture event 
                                            // Interrupt clear 
#define GPTIMER_ICR_CBECINT_M   0x00000400
#define GPTIMER_ICR_CBECINT_S   10
#define GPTIMER_ICR_CBMCINT     0x00000200  // GPTM Timer B capture match 
                                            // interrupt clear 
#define GPTIMER_ICR_CBMCINT_M   0x00000200
#define GPTIMER_ICR_CBMCINT_S   9
#define GPTIMER_ICR_TBTOCINT    0x00000100  // GPTM Timer B time-out interrupt 
                                            // clear 
#define GPTIMER_ICR_TBTOCINT_M  0x00000100
#define GPTIMER_ICR_TBTOCINT_S  8
#define GPTIMER_ICR_TAMCINT     0x00000010  // GPTM Timer A match interrupt 
                                            // clear 
#define GPTIMER_ICR_TAMCINT_M   0x00000010
#define GPTIMER_ICR_TAMCINT_S   4
#define GPTIMER_ICR_CAECINT     0x00000004  // GPTM Timer A capture event 
                                            // Interrupt clear 
#define GPTIMER_ICR_CAECINT_M   0x00000004
#define GPTIMER_ICR_CAECINT_S   2
#define GPTIMER_ICR_CAMCINT     0x00000002  // GPTM Timer A capture match 
                                            // interrupt clear 
#define GPTIMER_ICR_CAMCINT_M   0x00000002
#define GPTIMER_ICR_CAMCINT_S   1
#define GPTIMER_ICR_TATOCINT    0x00000001  // GPTM Timer A time-out interrupt 
                                            // clear 
#define GPTIMER_ICR_TATOCINT_M  0x00000001
#define GPTIMER_ICR_TATOCINT_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPTIMER_O_TAILR register.
//
//*****************************************************************************
#define GPTIMER_TAILR_TAILR_M   0xFFFFFFFF  // GPTM A interval load register 
#define GPTIMER_TAILR_TAILR_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPTIMER_O_TBILR register.
//
//*****************************************************************************
#define GPTIMER_TBILR_TBILR_M   0x0000FFFF  // GPTM B interval load register 
#define GPTIMER_TBILR_TBILR_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPTIMER_O_TAMATCHR register.
//
//*****************************************************************************
#define GPTIMER_TAMATCHR_TAMR_M 0xFFFFFFFF  // GPTM Timer A match register 
#define GPTIMER_TAMATCHR_TAMR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPTIMER_O_TBMATCHR register.
//
//*****************************************************************************
#define GPTIMER_TBMATCHR_TBMR_M 0x0000FFFF  // GPTM Timer B match register 
#define GPTIMER_TBMATCHR_TBMR_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TAPR register.
//
//*****************************************************************************
#define GPTIMER_TAPR_TAPSR_M    0x000000FF  // GPTM Timer A prescale 
#define GPTIMER_TAPR_TAPSR_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TBPR register.
//
//*****************************************************************************
#define GPTIMER_TBPR_TBPSR_M    0x000000FF  // GPTM Timer B prescale 
#define GPTIMER_TBPR_TBPSR_S    0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPTIMER_O_TAPMR register.
//
//*****************************************************************************
#define GPTIMER_TAPMR_TAPSR_M   0x000000FF  // GPTM Timer A prescale match 
#define GPTIMER_TAPMR_TAPSR_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// GPTIMER_O_TBPMR register.
//
//*****************************************************************************
#define GPTIMER_TBPMR_TBPSR_M   0x000000FF  // GPTM Timer B prescale match 
#define GPTIMER_TBPMR_TBPSR_S   0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TAR register.
//
//*****************************************************************************
#define GPTIMER_TAR_TAR_M       0xFFFFFFFF  // GPTM Timer A register 
#define GPTIMER_TAR_TAR_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TBR register.
//
//*****************************************************************************
#define GPTIMER_TBR_TBR_M       0x0000FFFF  // GPTM Timer B register 
#define GPTIMER_TBR_TBR_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TAV register.
//
//*****************************************************************************
#define GPTIMER_TAV_TAV_M       0xFFFFFFFF  // GPTM Timer A register 
#define GPTIMER_TAV_TAV_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TBV register.
//
//*****************************************************************************
#define GPTIMER_TBV_PRE_M       0x00FF0000  // GPTM Timer B prescale register 
                                            // (16-bit mode) 
#define GPTIMER_TBV_PRE_S       16
#define GPTIMER_TBV_TBV_M       0x0000FFFF  // GPTM Timer B register 
#define GPTIMER_TBV_TBV_S       0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TAPS register.
//
//*****************************************************************************
#define GPTIMER_TAPS_PSS_M      0x0000FFFF  // GPTM Timer A prescaler 
#define GPTIMER_TAPS_PSS_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TBPS register.
//
//*****************************************************************************
#define GPTIMER_TBPS_PSS_M      0x0000FFFF  // GPTM Timer B prescaler 
#define GPTIMER_TBPS_PSS_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TAPV register.
//
//*****************************************************************************
#define GPTIMER_TAPV_PSV_M      0x0000FFFF  // GPTM Timer A prescaler value 
#define GPTIMER_TAPV_PSV_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_TBPV register.
//
//*****************************************************************************
#define GPTIMER_TBPV_PSV_M      0x0000FFFF  // GPTM Timer B prescaler value 
#define GPTIMER_TBPV_PSV_S      0
//*****************************************************************************
//
// The following are defines for the bit fields in the GPTIMER_O_PP register.
//
//*****************************************************************************
#define GPTIMER_PP_ALTCLK       0x00000040  // Alternate clock source 0: Timer 
                                            // is not capable of using an 
                                            // alternate clock. 1: Timer is 
                                            // capable of using an alternate 
                                            // clock. 
#define GPTIMER_PP_ALTCLK_M     0x00000040
#define GPTIMER_PP_ALTCLK_S     6
#define GPTIMER_PP_SYNCNT       0x00000020  // Synchronized start 0: Timer is 
                                            // not capable of synchronizing the 
                                            // count value with other timers. 
                                            // 1: Timer is capable of 
                                            // synchronizing the count value 
                                            // with other timers. 
#define GPTIMER_PP_SYNCNT_M     0x00000020
#define GPTIMER_PP_SYNCNT_S     5
#define GPTIMER_PP_CHAIN        0x00000010  // Chain with other timers 0: 
                                            // Timer is not capable of chaining 
                                            // with previously numbered Timers. 
                                            // 1: Timer is capable of chaining 
                                            // with previously numbered timers. 
#define GPTIMER_PP_CHAIN_M      0x00000010
#define GPTIMER_PP_CHAIN_S      4
#define GPTIMER_PP_SIZE_M       0x0000000F  // Timer size 0: Timer A and Timer 
                                            // B are 16 bits wide with 8-bit 
                                            // prescale. 1: Timer A and Timer B 
                                            // are 32 bits wide with 16-bit 
                                            // prescale. 
#define GPTIMER_PP_SIZE_S       0


#endif // __HW_GPTIMER_H__

