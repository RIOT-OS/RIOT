/* esp/timer_regs.h
 *
 * ESP8266 Timer register definitions
 *
 * Not compatible with ESP SDK register access code.
 */

/*
Copyright (c) 2015, SuperHouse Automation Pty Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef TIMER_REGS_H
#define TIMER_REGS_H

#ifndef DOXYGEN

#include "esp/types.h"
#include "common_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TIMER_BASE 0x60000600
#define TIMER(i) (*(struct TIMER_REGS *)(TIMER_BASE + (i)*0x20))
#define TIMER_FRC1 TIMER(0)
#define TIMER_FRC2 TIMER(1)

/* TIMER registers
 *
 * ESP8266 has two hardware timer counters, FRC1 and FRC2.
 *
 * FRC1 is a 24-bit countdown timer, triggers interrupt when reaches zero.
 * FRC2 is a 32-bit countup timer, can set a variable match value to trigger an interrupt.
 *
 * FreeRTOS tick timer appears to come from XTensa core tick timer0,
 * not either of these.  FRC2 is used in the FreeRTOS SDK however. It
 * is set to free-run, interrupting periodically via updates to the
 * ALARM register. sdk_ets_timer_init configures FRC2 and assigns FRC2
 * interrupt handler at sdk_vApplicationTickHook+0x68
 */

struct TIMER_REGS {            // FRC1  FRC2
    uint32_t volatile LOAD;    // 0x00  0x20
    uint32_t volatile COUNT;   // 0x04  0x24
    uint32_t volatile CTRL;    // 0x08  0x28
    uint32_t volatile STATUS;  // 0x0c  0x2c
    uint32_t volatile ALARM;   //       0x30
};

_Static_assert(sizeof(struct TIMER_REGS) == 0x14, "TIMER_REGS is the wrong size");

#define TIMER_FRC1_MAX_LOAD 0x7fffff

/* Details for LOAD registers */

/* Behavior for FRC1:
 *
 * When TIMER_CTRL_RELOAD is cleared in TIMER(0).CTRL, FRC1 will
 * reload to its max value once underflowed (unless the load
 * value is rewritten in the interrupt handler.)
 *
 * When TIMER_CTRL_RELOAD is set in TIMER(0).CTRL, FRC1 will reload
 * from the load register value once underflowed.
 *
 * Behavior for FRC2:
 *
 * If TIMER_CTRL_RELOAD is cleared in TIMER(1).CTRL, writing to
 * this register will update the FRC2 COUNT value.
 *
 * If TIMER_CTRL_RELOAD is set in TIMER(1).CTRL, the behaviour
 * appears to be the same except that writing 0 to the load register
 * both sets the COUNT register to 0 and disables the timer, even if
 * the TIMER_CTRL_RUN bit is set.
 *
 * Offsets 0x34, 0x38, 0x3c all seem to read back the LOAD_REG value
 * also (but have no known function.)
 */

/* Details for CTRL registers */

/* Observed behaviour is like this:
 *
 *  * When TIMER_CTRL_INT_HOLD is set, the interrupt status bit
 *    TIMER_CTRL_INT_STATUS remains set when the timer interrupt
 *    triggers, unless manually cleared by writing 0 to
 *    TIMER(x).STATUS.  While the interrupt status bit stays set
 *    the timer will continue to run normally, but the interrupt
 *    (INUM_TIMER_FRC1 or INUM_TIMER_FRC2) won't trigger again.
 *
 *  * When TIMER_CTRL_INT_HOLD is cleared (default), there's no need to
 *    manually write to TIMER(x).STATUS. The interrupt status bit
 *    TIMER_CTRL_INT_STATUS automatically clears after the interrupt
 *    triggers, and the interrupt handler will run again
 *    automatically.
 */

/* The values for TIMER_CTRL_CLKDIV control how many CPU clock cycles amount to
 * one timer clock cycle.  For valid values, see the timer_clkdiv_t enum below.
 */

/* TIMER_CTRL_INT_STATUS gets set when interrupt fires, and cleared on a write
 * to TIMER(x).STATUS (or cleared automatically if TIMER_CTRL_INT_HOLD is not
 * set).
 */

#define TIMER_CTRL_INT_HOLD    BIT(0)
#define TIMER_CTRL_CLKDIV_M    0x00000003
#define TIMER_CTRL_CLKDIV_S    2
#define TIMER_CTRL_RELOAD      BIT(6)
#define TIMER_CTRL_RUN         BIT(7)
#define TIMER_CTRL_INT_STATUS  BIT(8)

typedef enum {
    TIMER_CLKDIV_1 = 0,
    TIMER_CLKDIV_16 = 1,
    TIMER_CLKDIV_256 = 2,
} timer_clkdiv_t;

/* Details for STATUS registers */

/* Reading this register always returns the value in
 * TIMER(x).LOAD
 *
 * Writing zero to this register clears the FRC1
 * interrupt status.
 */

/* Details for FRC2.ALARM register */

/* Interrupt match value for FRC2. When COUNT == ALARM,
   the interrupt fires.
*/

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* TIMER_REGS_H */
