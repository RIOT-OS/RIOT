/* esp/rtc_regs.h
 *
 * ESP8266 RTC register definitions
 *
 * Not compatible with ESP SDK register access code.
 *
 * RTC peripheral remains powered during deep sleep, and RTC clock
 * is used to wake from deep sleep when RTC.COUNTER == RTC.COUNTER_ALARM.
 *
 * "GPIO16" is a special GPIO pin connected to the RTC subsystem,
 * GPIO16 must be connected to reset to allow wake from deep sleep.
 *
 * The contents of scratch registers RTC.SCRATCH[] are preserved
 * across reset, including wake from sleep (unconfirmed). Contents of
 * RTCMEM are also preserved.
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

#ifndef RTC_REGS_H
#define RTC_REGS_H

#ifndef DOXYGEN

#include "esp/types.h"
#include "common_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_BASE 0x60000700
#define RTC (*(struct RTC_REGS *)(RTC_BASE))

//FIXME: need to understand/clarify distinction between GPIO_CONF and GPIO_CFG[]
// Note: GPIO_CFG[3] is also known as PAD_XPD_DCDC_CONF in eagle_soc.h

struct RTC_REGS {
    uint32_t volatile CTRL0;        // 0x00
    uint32_t volatile COUNTER_ALARM;    // 0x04
    uint32_t volatile RESET_REASON0;    // 0x08       //FIXME: need better name
    uint32_t volatile _unknownc;        // 0x0c
    uint32_t volatile _unknown10;       // 0x10
    uint32_t volatile RESET_REASON1;    // 0x14       //FIXME: need better name
    uint32_t volatile RESET_REASON2;    // 0x18       //FIXME: need better name
    uint32_t volatile COUNTER;          // 0x1c
    uint32_t volatile INT_SET;          // 0x20
    uint32_t volatile INT_CLEAR;        // 0x24
    uint32_t volatile INT_ENABLE;       // 0x28
    uint32_t volatile _unknown2c;       // 0x2c
    uint32_t volatile SCRATCH[4];       // 0x30 - 3c
    uint32_t volatile _unknown40;       // 0x40
    uint32_t volatile _unknown44;       // 0x44
    uint32_t volatile _unknown48;       // 0x48
    uint32_t volatile _unknown4c[7];    // 0x4c - 0x64
    uint32_t volatile GPIO_OUT;         // 0x68
    uint32_t volatile _unknown6c[2];    // 0x6c - 0x70
    uint32_t volatile GPIO_ENABLE;      // 0x74
    uint32_t volatile _unknown80[5];    // 0x78 - 0x88
    uint32_t volatile GPIO_IN;          // 0x8c
    uint32_t volatile GPIO_CONF;        // 0x90
    uint32_t volatile GPIO_CFG[6];      // 0x94 - 0xa8
};

_Static_assert(sizeof(struct RTC_REGS) == 0xac, "RTC_REGS is the wrong size");

/* Details for CTRL0 register */

/* Writing this bit causes a software reset but
   the device then fails in ets_main.c (needs other parameters set?) */
#define RTC_CTRL0_BIT31 BIT(31)

/* Details for RESET_REASONx registers */

/* The following are used in sdk_rtc_get_reset_reason().  Details are still a
 * bit sketchy regarding exactly what they mean/do.. */

#define RTC_RESET_REASON1_CODE_M  0x0000000f
#define RTC_RESET_REASON1_CODE_S  0

#define RTC_RESET_REASON2_CODE_M  0x0000003f
#define RTC_RESET_REASON2_CODE_S  8

/* Writing this bit causes the ESP to go into some kind of unrecoverable boot loop */
#define RTC_RESET_REASON0_BIT20 BIT(20)

/* Both bits 20 & 21 can be set & cleared from software,
   BIT21 appears to be checked inside sdk_rtc_get_reset_reason() */
#define RTC_RESET_REASON0_BIT21 BIT(21)
#define RTC_RESET_REASON0_BIT22 BIT(22)

/* Details for GPIO_CONF register */

#define RTC_GPIO_CONF_OUT_ENABLE BIT(0)

/* Details for GPIO_CFG[3] register controlling GPIO16 (possibly others?) */

#define RTC_GPIO_CFG3_PIN_PULLUP          BIT(2)
#define RTC_GPIO_CFG3_PIN_PULLDOWN        BIT(3)
#define RTC_GPIO_CFG3_PIN_PULLUP_SLEEP    BIT(4)
#define RTC_GPIO_CFG3_PIN_PULLDOWN_SLEEP  BIT(5)

/* The PIN_FUNC values here are probably similar to the
   values used to set the iomux registers...? */
#define RTC_GPIO_CFG3_PIN_FUNC_M 0x00000043
#define RTC_GPIO_CFG3_PIN_FUNC_S 0

/* This should be the function value needed to have GPIO16 be the alarm
   output from the RTC. FIXME: Needs to be validated. */
#define RTC_GPIO_CFG3_PIN_FUNC_RTC_GPIO0  BIT(0)

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* RTC_REGS_H */
