/******************************************************************************
*  Filename:       hw_rfcore_sfr.h
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

#ifndef __HW_RFCORE_SFR_H__
#define __HW_RFCORE_SFR_H__

//*****************************************************************************
//
// The following are defines for the RFCORE_SFR register offsets.
//
//*****************************************************************************
#define RFCORE_SFR_MTCSPCFG     0x40088800  // MAC Timer event configuration 
#define RFCORE_SFR_MTCTRL       0x40088804  // MAC Timer control register 
#define RFCORE_SFR_MTIRQM       0x40088808  // MAC Timer interrupt mask 
#define RFCORE_SFR_MTIRQF       0x4008880C  // MAC Timer interrupt flags 
#define RFCORE_SFR_MTMSEL       0x40088810  // MAC Timer multiplex select 
#define RFCORE_SFR_MTM0         0x40088814  // MAC Timer multiplexed register 
                                            // 0 
#define RFCORE_SFR_MTM1         0x40088818  // MAC Timer multiplexed register 
                                            // 1 
#define RFCORE_SFR_MTMOVF2      0x4008881C  // MAC Timer multiplexed overflow 
                                            // register 2 
#define RFCORE_SFR_MTMOVF1      0x40088820  // MAC Timer multiplexed overflow 
                                            // register 1 
#define RFCORE_SFR_MTMOVF0      0x40088824  // MAC Timer multiplexed overflow 
                                            // register 0 
#define RFCORE_SFR_RFDATA       0x40088828  // The TX FIFO and RX FIFO may be 
                                            // accessed through this register. 
                                            // Data is written to the TX FIFO 
                                            // when writing to the RFD 
                                            // register. Data is read from the 
                                            // RX FIFO when the RFD register is 
                                            // read. The XREG registers 
                                            // RXFIFOCNT and TXFIFOCNT provide 
                                            // information on the amount of 
                                            // data in the FIFOs. The FIFO 
                                            // contents can be cleared by 
                                            // issuing SFLUSHRX and SFLUSHTX. 
#define RFCORE_SFR_RFERRF       0x4008882C  // RF error interrupt flags 
#define RFCORE_SFR_RFIRQF1      0x40088830  // RF interrupt flags 
#define RFCORE_SFR_RFIRQF0      0x40088834  // RF interrupt flags 
#define RFCORE_SFR_RFST         0x40088838  // RF CSMA-CA/strobe processor 


//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTCSPCFG register.
//
//*****************************************************************************
#define RFCORE_SFR_MTCSPCFG_MACTIMER_EVENMT_CFG_M \
                                0x00000070  // Selects the event that triggers 
                                            // an MT_EVENT2 pulse 000: 
                                            // MT_per_event 001: MT_cmp1_event 
                                            // 010: MT_cmp2_event 011: 
                                            // MTovf_per_event 100: 
                                            // MTovf_cmp1_event 101: 
                                            // MTovf_cmp2_event 110: Reserved 
                                            // 111: No event 

#define RFCORE_SFR_MTCSPCFG_MACTIMER_EVENMT_CFG_S 4
#define RFCORE_SFR_MTCSPCFG_MACTIMER_EVENT1_CFG_M \
                                0x00000007  // Selects the event that triggers 
                                            // an MT_EVENT1 pulse 000: 
                                            // MT_per_event 001: MT_cmp1_event 
                                            // 010: MT_cmp2_event 011: 
                                            // MTovf_per_event 100: 
                                            // MTovf_cmp1_event 101: 
                                            // MTovf_cmp2_event 110: Reserved 
                                            // 111: No event 

#define RFCORE_SFR_MTCSPCFG_MACTIMER_EVENT1_CFG_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTCTRL register.
//
//*****************************************************************************
#define RFCORE_SFR_MTCTRL_LATCH_MODE \
                                0x00000008  // 0: Reading MTM0 with 
                                            // MTMSEL.MTMSEL = 000 latches the 
                                            // high byte of the timer, making 
                                            // it ready to be read from MTM1. 
                                            // Reading MTMOVF0 with 
                                            // MTMSEL.MTMOVFSEL = 000 latches 
                                            // the two most-significant bytes 
                                            // of the overflow counter, making 
                                            // it possible to read these from 
                                            // MTMOVF1 and MTMOVF2. 1: Reading 
                                            // MTM0 with MTMSEL.MTMSEL = 000 
                                            // latches the high byte of the 
                                            // timer and the entire overflow 
                                            // counter at once, making it 
                                            // possible to read the values from 
                                            // MTM1, MTMOVF0, MTMOVF1, and 
                                            // MTMOVF2. 

#define RFCORE_SFR_MTCTRL_LATCH_MODE_M \
                                0x00000008
#define RFCORE_SFR_MTCTRL_LATCH_MODE_S 3
#define RFCORE_SFR_MTCTRL_STATE 0x00000004  // State of MAC Timer 0: Timer 
                                            // idle 1: Timer running 
#define RFCORE_SFR_MTCTRL_STATE_M \
                                0x00000004
#define RFCORE_SFR_MTCTRL_STATE_S 2
#define RFCORE_SFR_MTCTRL_SYNC  0x00000002  // 0: Starting and stopping of 
                                            // timer is immediate; that is, 
                                            // synchronous with clk_rf_32m. 1: 
                                            // Starting and stopping of timer 
                                            // occurs at the first positive 
                                            // edge of the 32-kHz clock. For 
                                            // more details regarding timer 
                                            // start and stop, see Section 
                                            // 22.4. 
#define RFCORE_SFR_MTCTRL_SYNC_M \
                                0x00000002
#define RFCORE_SFR_MTCTRL_SYNC_S 1
#define RFCORE_SFR_MTCTRL_RUN   0x00000001  // Write 1 to start timer, write 0 
                                            // to stop timer. When read, it 
                                            // returns the last written value. 
#define RFCORE_SFR_MTCTRL_RUN_M 0x00000001
#define RFCORE_SFR_MTCTRL_RUN_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTIRQM register.
//
//*****************************************************************************
#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE2M \
                                0x00000020  // Enables the 
                                            // MACTIMER_OVF_COMPARE2 interrupt 

#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE2M_M \
                                0x00000020
#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE2M_S 5
#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE1M \
                                0x00000010  // Enables the 
                                            // MACTIMER_OVF_COMPARE1 interrupt 

#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE1M_M \
                                0x00000010
#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE1M_S 4
#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_PERM \
                                0x00000008  // Enables the MACTIMER_OVF_PER 
                                            // interrupt 

#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_PERM_M \
                                0x00000008
#define RFCORE_SFR_MTIRQM_MACTIMER_OVF_PERM_S 3
#define RFCORE_SFR_MTIRQM_MACTIMER_COMPARE2M \
                                0x00000004  // Enables the MACTIMER_COMPARE2 
                                            // interrupt 

#define RFCORE_SFR_MTIRQM_MACTIMER_COMPARE2M_M \
                                0x00000004
#define RFCORE_SFR_MTIRQM_MACTIMER_COMPARE2M_S 2
#define RFCORE_SFR_MTIRQM_MACTIMER_COMPARE1M \
                                0x00000002  // Enables the MACTIMER_COMPARE1 
                                            // interrupt 

#define RFCORE_SFR_MTIRQM_MACTIMER_COMPARE1M_M \
                                0x00000002
#define RFCORE_SFR_MTIRQM_MACTIMER_COMPARE1M_S 1
#define RFCORE_SFR_MTIRQM_MACTIMER_PERM \
                                0x00000001  // Enables the MACTIMER_PER 
                                            // interrupt 

#define RFCORE_SFR_MTIRQM_MACTIMER_PERM_M \
                                0x00000001
#define RFCORE_SFR_MTIRQM_MACTIMER_PERM_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTIRQF register.
//
//*****************************************************************************
#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_COMPARE2F \
                                0x00000020  // Set when the MAC Timer overflow 
                                            // counter counts to the value set 
                                            // at MTovf_cmp2 

#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_COMPARE2F_M \
                                0x00000020
#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_COMPARE2F_S 5
#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_COMPARE1F \
                                0x00000010  // Set when the MAC Timer overflow 
                                            // counter counts to the value set 
                                            // at Timer 2 MTovf_cmp1 

#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_COMPARE1F_M \
                                0x00000010
#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_COMPARE1F_S 4
#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_PERF \
                                0x00000008  // Set when the MAC Timer overflow 
                                            // counter would have counted to a 
                                            // value equal to MTovf_per, but 
                                            // instead wraps to 0 

#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_PERF_M \
                                0x00000008
#define RFCORE_SFR_MTIRQF_MACTIMER_OVF_PERF_S 3
#define RFCORE_SFR_MTIRQF_MACTIMER_COMPARE2F \
                                0x00000004  // Set when the MAC Timer counter 
                                            // counts to the value set at 
                                            // MT_cmp2 

#define RFCORE_SFR_MTIRQF_MACTIMER_COMPARE2F_M \
                                0x00000004
#define RFCORE_SFR_MTIRQF_MACTIMER_COMPARE2F_S 2
#define RFCORE_SFR_MTIRQF_MACTIMER_COMPARE1F \
                                0x00000002  // Set when the MAC Timer counter 
                                            // counts to the value set at 
                                            // MT_cmp1 

#define RFCORE_SFR_MTIRQF_MACTIMER_COMPARE1F_M \
                                0x00000002
#define RFCORE_SFR_MTIRQF_MACTIMER_COMPARE1F_S 1
#define RFCORE_SFR_MTIRQF_MACTIMER_PERF \
                                0x00000001  // Set when the MAC Timer counter 
                                            // would have counted to a value 
                                            // equal to MT_per, but instead 
                                            // wraps to 0 

#define RFCORE_SFR_MTIRQF_MACTIMER_PERF_M \
                                0x00000001
#define RFCORE_SFR_MTIRQF_MACTIMER_PERF_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTMSEL register.
//
//*****************************************************************************
#define RFCORE_SFR_MTMSEL_MTMOVFSEL_M \
                                0x00000070  // The value of this register 
                                            // selects the internal registers 
                                            // that are modified or read when 
                                            // accessing MTMOVF0, MTMOVF1, and 
                                            // MTMOVF2. 000: MTovf (overflow 
                                            // counter) 001: MTovf_cap 
                                            // (overflow capture) 010: 
                                            // MTovf_per (overflow period) 011: 
                                            // MTovf_cmp1 (overflow compare 1) 
                                            // 100: MTovf_cmp2 (overflow 
                                            // compare 2) 101 to 111: Reserved 

#define RFCORE_SFR_MTMSEL_MTMOVFSEL_S 4
#define RFCORE_SFR_MTMSEL_MTMSEL_M \
                                0x00000007  // The value of this register 
                                            // selects the internal registers 
                                            // that are modified or read when 
                                            // accessing MTM0 and MTM1. 000: 
                                            // MTtim (timer count value) 001: 
                                            // MT_cap (timer capture) 010: 
                                            // MT_per (timer period) 011: 
                                            // MT_cmp1 (timer compare 1) 100: 
                                            // MT_cmp2 (timer compare 2) 101 to 
                                            // 111: Reserved MTM0 

#define RFCORE_SFR_MTMSEL_MTMSEL_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTM0 register.
//
//*****************************************************************************
#define RFCORE_SFR_MTM0_MTM0_M  0x000000FF  // Indirectly returns and modifies 
                                            // bits [7:0] of an internal 
                                            // register depending on the value 
                                            // of MTMSEL.MTMSEL. When reading 
                                            // the MTM0 register with 
                                            // MTMSEL.MTMSEL set to 000 and 
                                            // MTCTRL.LATCH_MODE set to 0, the 
                                            // timer (MTtim) value is latched. 
                                            // When reading the MTM0 register 
                                            // with MTMSEL.MTMSEL set to 000 
                                            // and MTCTRL.LATCH_MODE set to 1, 
                                            // the timer (MTtim) and overflow 
                                            // counter (MTovf) values are 
                                            // latched. 
#define RFCORE_SFR_MTM0_MTM0_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTM1 register.
//
//*****************************************************************************
#define RFCORE_SFR_MTM1_MTM1_M  0x000000FF  // Indirectly returns and modifies 
                                            // bits [15:8] of an internal 
                                            // register, depending on the value 
                                            // of MTMSEL.MTMSEL. When reading 
                                            // the MTM0 register with 
                                            // MTMSEL.MTMSEL set to 000, the 
                                            // timer (MTtim) value is latched. 
                                            // Reading this register with 
                                            // MTMSEL.MTMSEL set to 000 returns 
                                            // the latched value of 
                                            // MTtim[15:8]. 
#define RFCORE_SFR_MTM1_MTM1_S  0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTMOVF2 register.
//
//*****************************************************************************
#define RFCORE_SFR_MTMOVF2_MTMOVF2_M \
                                0x000000FF  // Indirectly returns and modifies 
                                            // bits [23:16] of an internal 
                                            // register, depending on the value 
                                            // of MTMSEL.MTMOVFSEL. Reading 
                                            // this register with 
                                            // MTMSEL.MTMOVFSEL set to 000 
                                            // returns the latched value of 
                                            // MTovf[23:16]. 

#define RFCORE_SFR_MTMOVF2_MTMOVF2_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTMOVF1 register.
//
//*****************************************************************************
#define RFCORE_SFR_MTMOVF1_MTMOVF1_M \
                                0x000000FF  // Indirectly returns and modifies 
                                            // bits [15:8] of an internal 
                                            // register, depending on the value 
                                            // of MTMSEL.MTMSEL. Reading this 
                                            // register with MTMSEL.MTMOVFSEL 
                                            // set to 000 returns the latched 
                                            // value of MTovf[15:8]. 

#define RFCORE_SFR_MTMOVF1_MTMOVF1_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_MTMOVF0 register.
//
//*****************************************************************************
#define RFCORE_SFR_MTMOVF0_MTMOVF0_M \
                                0x000000FF  // Indirectly returns and modifies 
                                            // bits [7:0] of an internal 
                                            // register, depending on the value 
                                            // of MTMSEL.MTMOVFSEL. When 
                                            // reading the MTMOVF0 register 
                                            // with MTMSEL.MTMOVFSEL set to 000 
                                            // and MTCTRL.LATCH_MODE set to 0, 
                                            // the overflow counter value 
                                            // (MTovf) is latched. When reading 
                                            // the MTM0 register with 
                                            // MTMSEL.MTMOVFSEL set to 000 and 
                                            // MTCTRL.LATCH_MODE set to 1, the 
                                            // overflow counter value (MTovf) 
                                            // is latched. 

#define RFCORE_SFR_MTMOVF0_MTMOVF0_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_RFDATA register.
//
//*****************************************************************************
#define RFCORE_SFR_RFDATA_RFD_M 0x000000FF  // Data written to the register is 
                                            // written to the TX FIFO. When 
                                            // reading this register, data from 
                                            // the RX FIFO is read. 
#define RFCORE_SFR_RFDATA_RFD_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_RFERRF register.
//
//*****************************************************************************
#define RFCORE_SFR_RFERRF_STROBEERR \
                                0x00000040  // A command strobe was issued 
                                            // when it could not be processed. 
                                            // Triggered if trying to disable 
                                            // the radio when it is already 
                                            // disabled, or when trying to do a 
                                            // SACK, SACKPEND, or SNACK command 
                                            // when not in active RX. 0: No 
                                            // interrupt pending 1: Interrupt 
                                            // pending 

#define RFCORE_SFR_RFERRF_STROBEERR_M \
                                0x00000040
#define RFCORE_SFR_RFERRF_STROBEERR_S 6
#define RFCORE_SFR_RFERRF_TXUNDERF \
                                0x00000020  // TX FIFO underflowed. 0: No 
                                            // interrupt pending 1: Interrupt 
                                            // pending 

#define RFCORE_SFR_RFERRF_TXUNDERF_M \
                                0x00000020
#define RFCORE_SFR_RFERRF_TXUNDERF_S 5
#define RFCORE_SFR_RFERRF_TXOVERF \
                                0x00000010  // TX FIFO overflowed. 0: No 
                                            // interrupt pending 1: Interrupt 
                                            // pending 

#define RFCORE_SFR_RFERRF_TXOVERF_M \
                                0x00000010
#define RFCORE_SFR_RFERRF_TXOVERF_S 4
#define RFCORE_SFR_RFERRF_RXUNDERF \
                                0x00000008  // RX FIFO underflowed. 0: No 
                                            // interrupt pending 1: Interrupt 
                                            // pending 

#define RFCORE_SFR_RFERRF_RXUNDERF_M \
                                0x00000008
#define RFCORE_SFR_RFERRF_RXUNDERF_S 3
#define RFCORE_SFR_RFERRF_RXOVERF \
                                0x00000004  // RX FIFO overflowed. 0: No 
                                            // interrupt pending 1: Interrupt 
                                            // pending 

#define RFCORE_SFR_RFERRF_RXOVERF_M \
                                0x00000004
#define RFCORE_SFR_RFERRF_RXOVERF_S 2
#define RFCORE_SFR_RFERRF_RXABO 0x00000002  // Reception of a frame was 
                                            // aborted. 0: No interrupt pending 
                                            // 1: Interrupt pending 
#define RFCORE_SFR_RFERRF_RXABO_M \
                                0x00000002
#define RFCORE_SFR_RFERRF_RXABO_S 1
#define RFCORE_SFR_RFERRF_NLOCK 0x00000001  // The frequency synthesizer 
                                            // failed to achieve lock after 
                                            // time-out, or lock is lost during 
                                            // reception. The receiver must be 
                                            // restarted to clear this error 
                                            // situation. 0: No interrupt 
                                            // pending 1: Interrupt pending 
#define RFCORE_SFR_RFERRF_NLOCK_M \
                                0x00000001
#define RFCORE_SFR_RFERRF_NLOCK_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_RFIRQF1 register.
//
//*****************************************************************************
#define RFCORE_SFR_RFIRQF1_CSP_WAIT \
                                0x00000020  // Execution continued after a 
                                            // wait instruction in CSP. 0: No 
                                            // interrupt pending 1: Interrupt 
                                            // pending 

#define RFCORE_SFR_RFIRQF1_CSP_WAIT_M \
                                0x00000020
#define RFCORE_SFR_RFIRQF1_CSP_WAIT_S 5
#define RFCORE_SFR_RFIRQF1_CSP_STOP \
                                0x00000010  // CSP has stopped program 
                                            // execution. 0: No interrupt 
                                            // pending 1: Interrupt pending 

#define RFCORE_SFR_RFIRQF1_CSP_STOP_M \
                                0x00000010
#define RFCORE_SFR_RFIRQF1_CSP_STOP_S 4
#define RFCORE_SFR_RFIRQF1_CSP_MANINT \
                                0x00000008  // Manual interrupt generated from 
                                            // CSP 0: No interrupt pending 1: 
                                            // Interrupt pending 

#define RFCORE_SFR_RFIRQF1_CSP_MANINT_M \
                                0x00000008
#define RFCORE_SFR_RFIRQF1_CSP_MANINT_S 3
#define RFCORE_SFR_RFIRQF1_RFIDLE \
                                0x00000004  // Radio state-machine has entered 
                                            // the IDLE state. 0: No interrupt 
                                            // pending 1: Interrupt pending 

#define RFCORE_SFR_RFIRQF1_RFIDLE_M \
                                0x00000004
#define RFCORE_SFR_RFIRQF1_RFIDLE_S 2
#define RFCORE_SFR_RFIRQF1_TXDONE \
                                0x00000002  // A complete frame has been 
                                            // transmitted. 0: No interrupt 
                                            // pending 1: Interrupt pending 

#define RFCORE_SFR_RFIRQF1_TXDONE_M \
                                0x00000002
#define RFCORE_SFR_RFIRQF1_TXDONE_S 1
#define RFCORE_SFR_RFIRQF1_TXACKDONE \
                                0x00000001  // An acknowledgement frame has 
                                            // been completely transmitted. 0: 
                                            // No interrupt pending 1: 
                                            // Interrupt pending 

#define RFCORE_SFR_RFIRQF1_TXACKDONE_M \
                                0x00000001
#define RFCORE_SFR_RFIRQF1_TXACKDONE_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_RFIRQF0 register.
//
//*****************************************************************************
#define RFCORE_SFR_RFIRQF0_RXMASKZERO \
                                0x00000080  // The RXENABLE register has gone 
                                            // from a nonzero state to an 
                                            // all-zero state. 0: No interrupt 
                                            // pending 1: Interrupt pending 

#define RFCORE_SFR_RFIRQF0_RXMASKZERO_M \
                                0x00000080
#define RFCORE_SFR_RFIRQF0_RXMASKZERO_S 7
#define RFCORE_SFR_RFIRQF0_RXPKTDONE \
                                0x00000040  // A complete frame has been 
                                            // received. 0: No interrupt 
                                            // pending 1: Interrupt pending 

#define RFCORE_SFR_RFIRQF0_RXPKTDONE_M \
                                0x00000040
#define RFCORE_SFR_RFIRQF0_RXPKTDONE_S 6
#define RFCORE_SFR_RFIRQF0_FRAME_ACCEPTED \
                                0x00000020  // Frame has passed frame 
                                            // filtering. 0: No interrupt 
                                            // pending 1: Interrupt pending 

#define RFCORE_SFR_RFIRQF0_FRAME_ACCEPTED_M \
                                0x00000020
#define RFCORE_SFR_RFIRQF0_FRAME_ACCEPTED_S 5
#define RFCORE_SFR_RFIRQF0_SRC_MATCH_FOUND \
                                0x00000010  // Source match is found. 0: No 
                                            // interrupt pending 1: Interrupt 
                                            // pending 

#define RFCORE_SFR_RFIRQF0_SRC_MATCH_FOUND_M \
                                0x00000010
#define RFCORE_SFR_RFIRQF0_SRC_MATCH_FOUND_S 4
#define RFCORE_SFR_RFIRQF0_SRC_MATCH_DONE \
                                0x00000008  // Source matching is complete. 0: 
                                            // No interrupt pending 1: 
                                            // Interrupt pending 

#define RFCORE_SFR_RFIRQF0_SRC_MATCH_DONE_M \
                                0x00000008
#define RFCORE_SFR_RFIRQF0_SRC_MATCH_DONE_S 3
#define RFCORE_SFR_RFIRQF0_FIFOP \
                                0x00000004  // The number of bytes in the RX 
                                            // FIFO is greater than the 
                                            // threshold. Also raised when a 
                                            // complete frame is received, and 
                                            // when a packet is read out 
                                            // completely and more complete 
                                            // packets are available. 0: No 
                                            // interrupt pending 1: Interrupt 
                                            // pending 

#define RFCORE_SFR_RFIRQF0_FIFOP_M \
                                0x00000004
#define RFCORE_SFR_RFIRQF0_FIFOP_S 2
#define RFCORE_SFR_RFIRQF0_SFD  0x00000002  // SFD has been received or 
                                            // transmitted. 0: No interrupt 
                                            // pending 1: Interrupt pending 
#define RFCORE_SFR_RFIRQF0_SFD_M \
                                0x00000002
#define RFCORE_SFR_RFIRQF0_SFD_S 1
#define RFCORE_SFR_RFIRQF0_ACT_UNUSED \
                                0x00000001  // Reserved 0: No interrupt 
                                            // pending 1: Interrupt pending 

#define RFCORE_SFR_RFIRQF0_ACT_UNUSED_M \
                                0x00000001
#define RFCORE_SFR_RFIRQF0_ACT_UNUSED_S 0
//*****************************************************************************
//
// The following are defines for the bit fields in the 
// RFCORE_SFR_RFST register.
//
//*****************************************************************************
#define RFCORE_SFR_RFST_INSTR_M 0x000000FF  // Data written to this register 
                                            // is written to the CSP 
                                            // instruction memory. Reading this 
                                            // register returns the CSP 
                                            // instruction currently being 
                                            // executed. 
#define RFCORE_SFR_RFST_INSTR_S 0


#endif // __HW_RFCORE_SFR_H__

