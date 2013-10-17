/*
 * isr.c - mc1322x specific isr
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include "mc1322x.h"

static void (*tmr_isr_funcs[4])(void) = {
        tmr0_isr,
        tmr1_isr,
        tmr2_isr,
        tmr3_isr
};

void irq_register_timer_handler(int timer, void (*isr)(void))
{
        tmr_isr_funcs[timer] = isr;
}


__attribute__ ((section (".irq")))
__attribute__ ((interrupt("IRQ")))
void irq(void)
{
        while ((ITC->NIPEND)) {

                if(ITC->NIPENDbits.TMR) {
                        /* dispatch to individual timer isrs if they exist */
                        /* timer isrs are responsible for determining if they */
                        /* caused an interrupt */
                        /* and clearing their own interrupt flags */
                        if (tmr_isr_funcs[0] != 0) { (tmr_isr_funcs[0])(); }
                        if (tmr_isr_funcs[1] != 0) { (tmr_isr_funcs[1])(); }
                        if (tmr_isr_funcs[2] != 0) { (tmr_isr_funcs[2])(); }
                        if (tmr_isr_funcs[3] != 0) { (tmr_isr_funcs[3])(); }
                }

                if(ITC->NIPENDbits.MACA) {
                        if(maca_isr != 0) { maca_isr(); }
                }
                if(ITC->NIPENDbits.UART1) {
                        if(uart1_isr != 0) { uart1_isr(); }
                }
                if(ITC->NIPENDbits.UART2) {
                        if(uart2_isr != 0) { uart2_isr(); }
                }
                if(ITC->NIPENDbits.CRM) {
                        // if(rtc_wu_evt() && (rtc_isr != 0)) { rtc_isr(); }
                        // if(kbi_evnt(4) && (kbi4_isr != 0)) { kbi4_isr(); }
                        // if(kbi_evnt(5) && (kbi5_isr != 0)) { kbi5_isr(); }
                        // if(kbi_evnt(6) && (kbi6_isr != 0)) { kbi6_isr(); }
                        // if(kbi_evnt(7) && (kbi7_isr != 0)) { kbi7_isr(); }

                        if (CRM->STATUSbits.CAL_DONE && CRM->CAL_CNTLbits.CAL_IEN && cal_isr)
                        {
                                CRM->STATUSbits.CAL_DONE = 0;
                                cal_isr();
                        }
                }
                if(ITC->NIPENDbits.ASM) {
                        if(asm_isr != 0) { asm_isr(); }
                }
                if (ITC->NIPENDbits.I2C) {
                        if (i2c_isr != 0) { i2c_isr(); }
                }

                ITC->INTFRC = 0; /* stop forcing interrupts */

        }
}
