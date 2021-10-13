/* esp/gpio_regs.h
 *
 * ESP8266 GPIO register definitions
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

#ifndef GPIO_REGS_H
#define GPIO_REGS_H

#ifndef DOXYGEN

#include "esp/types.h"
#include "common_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_BASE 0x60000300
#define GPIO (*(struct GPIO_REGS *)(GPIO_BASE))

/** GPIO output registers GPIO.OUT, GPIO.OUT_SET, GPIO.OUT_CLEAR:
 *
 * _SET and _CLEAR write-only registers set and clear bits in the main register,
 * respectively.
 *
 * i.e.
 *   GPIO.OUT_SET = BIT(3);
 * and
 *   GPIO.OUT |= BIT(3);
 *
 * ... are equivalent, but the former uses fewer CPU cycles.
 *
 * ENABLE_OUT / ENABLE_OUT_SET / ENABLE_OUT_CLEAR:
 *
 * Determine whether the corresponding GPIO has its output enabled or not.
 * When clear, GPIO can function as an input.  When set, GPIO will drive its
 * output (and IN register will simply reflect the output state).
 *
 * (_SET/_CLEAR function similarly to OUT registers)
 *
 * STATUS / STATUS_SET / STATUS_CLEAR:
 *
 * Indicates which GPIOs have triggered an interrupt.  Interrupt status should
 * be reset by writing to STATUS or STATUS_CLEAR.
 *
 * (_SET/_CLEAR function similarly to OUT registers)
 *
 * IN:
 *
 * Low 16 bits represent GPIO0-15 state (see also above).
 *
 * High 16 bits represent "strapping pins" values captured at reset time:
 * bit 31 - GPIO10 (SD_DATA3)
 * bit 30 - GPIO9 (SD_DATA2)
 * bit 29 - GPIO7 (SD_DATA0)
 * bit 18 - GPIO15 (MTDO)
 * bit 17 - GPIO0
 * bit 16 - GPIO2
 * (In other words, highest 3 and lowest 3 bits of 16-bit half-word are used).
 * BootROM uses strapping pin values to determine boot mode.
 *
 * Source and more information: 0D-ESP8266__Pin_List*.xlsx document
 */

struct GPIO_REGS {
    uint32_t volatile OUT;              // 0x00
    uint32_t volatile OUT_SET;          // 0x04
    uint32_t volatile OUT_CLEAR;        // 0x08
    uint32_t volatile ENABLE_OUT;       // 0x0c
    uint32_t volatile ENABLE_OUT_SET;   // 0x10
    uint32_t volatile ENABLE_OUT_CLEAR; // 0x14
    uint32_t volatile IN;               // 0x18
    uint32_t volatile STATUS;           // 0x1c
    uint32_t volatile STATUS_SET;       // 0x20
    uint32_t volatile STATUS_CLEAR;     // 0x24
    uint32_t volatile CONF[16];         // 0x28 - 0x64
    uint32_t volatile PWM;              // 0x68
    uint32_t volatile RTC_CALIB;        // 0x6c
    uint32_t volatile RTC_CALIB_RESULT; // 0x70
};

_Static_assert(sizeof(struct GPIO_REGS) == 0x74, "GPIO_REGS is the wrong size");

/* Details for additional OUT register fields */

/* Bottom 16 bits of GPIO.OUT are for GPIOs 0-15, but upper 16 bits
   are used to configure the input signalling pins for Bluetooth
   Coexistence config (see esp/phy.h for a wrapper function).
*/
#define GPIO_OUT_PIN_MASK 0x0000FFFF
#define GPIO_OUT_BT_COEXIST_MASK 0x03FF0000
#define GPIO_OUT_BT_ACTIVE_ENABLE BIT(24)
#define GPIO_OUT_BT_PRIORITY_ENABLE BIT(25)
#define GPIO_OUT_BT_ACTIVE_PIN_M 0x0F
#define GPIO_OUT_BT_ACTIVE_PIN_S 16
#define GPIO_OUT_BT_PRIORITY_PIN_M 0x0F
#define GPIO_OUT_BT_PRIORITY_PIN_S 20

/* Details for CONF[i] registers */

/* GPIO.CONF[i] control the pin behavior for the corresponding GPIO in/output.
 *
 * GPIO_CONF_CONFIG (multi-value)
 *     FIXME: Unclear what these do.  Need to find a better name.
 *
 * GPIO_CONF_WAKEUP_ENABLE (boolean)
 *     Can an interrupt contion on this pin wake the processor from a sleep
 *     state?
 *
 * GPIO_CONF_INTTYPE (multi-value)
 *     Under what conditions this GPIO input should generate an interrupt.
 *     (see gpio_inttype_t enum below for values)
 *
 * GPIO_CONF_OPEN_DRAIN (boolean)
 *     If this bit is set, the pin is in "open drain" mode - a high output state
 *     will leave the pin floating but not source any current. If bit is cleared,
 *     the pin is in push/pull mode so a high output state will drive the pin up
 *     to +Vcc (3.3V).  In either case, a low output state will pull the pin down
 *     to ground.
 *
 *     GPIO_CONF_OPEN_DRAIN does not appear to work on all pins.
 *
 *
 * GPIO_CONF_SOURCE_PWM (boolean)
 *     When set, GPIO pin output will be connected to the sigma-delta PWM
 *     generator (controlled by the GPIO.PWM register).  When cleared, pin
 *     output will function as a normal GPIO output (controlled by the
 *     GPIO.OUT* registers).
 */

#define GPIO_CONF_CONFIG_M       0x00000003
#define GPIO_CONF_CONFIG_S       11
#define GPIO_CONF_WAKEUP_ENABLE  BIT(10)
#define GPIO_CONF_INTTYPE_M      0x00000007
#define GPIO_CONF_INTTYPE_S      7
#define GPIO_CONF_OPEN_DRAIN     BIT(2)
#define GPIO_CONF_SOURCE_PWM     BIT(0)

/* Valid values for the GPIO_CONF_INTTYPE field */
typedef enum {
    GPIO_INTTYPE_NONE       = 0,
    GPIO_INTTYPE_EDGE_POS   = 1,
    GPIO_INTTYPE_EDGE_NEG   = 2,
    GPIO_INTTYPE_EDGE_ANY   = 3,
    GPIO_INTTYPE_LEVEL_LOW  = 4,
    GPIO_INTTYPE_LEVEL_HIGH = 5,
} gpio_inttype_t;

/* Details for PWM register */

#define GPIO_PWM_ENABLE          BIT(16)
#define GPIO_PWM_PRESCALER_M     0x000000ff
#define GPIO_PWM_PRESCALER_S     8
#define GPIO_PWM_TARGET_M        0x000000ff
#define GPIO_PWM_TARGET_S        0

/* Details for RTC_CALIB register */

#define GPIO_RTC_CALIB_START     BIT(31)
#define GPIO_RTC_CALIB_PERIOD_M  0x000003ff
#define GPIO_RTC_CALIB_PERIOD_S  0

/* Details for RTC_CALIB_RESULT register */

#define GPIO_RTC_CALIB_RESULT_READY       BIT(31)
#define GPIO_RTC_CALIB_RESULT_READY_REAL  BIT(30)
#define GPIO_RTC_CALIB_RESULT_VALUE_M     0x000fffff
#define GPIO_RTC_CALIB_RESULT_VALUE_S     0

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* GPIO_REGS_H */
