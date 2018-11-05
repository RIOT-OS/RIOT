/* esp/wdev_regs.h
 *
 * ESP8266 register definitions for the "wdev" region (0x3FF2xxx)
 *
 * In the DPORT memory space, alongside DPORT regs. However mostly
 * concerned with the WiFi hardware interface.
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

#ifndef WDEV_REGS_H
#define WDEV_REGS_H

#ifndef DOXYGEN

#include "esp/types.h"
#include "common_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WDEV_BASE 0x3FF20000
#define WDEV (*(struct WDEV_REGS *)(WDEV_BASE))

/* Note: This memory region is not currently well understood.  Pretty much all
 * of the definitions here are from reverse-engineering the Espressif SDK code,
 * many are just educated guesses, and almost certainly some are misleading or
 * wrong.  If you can improve on any of this, please contribute!
 */

struct WDEV_REGS {
    uint32_t volatile _unknown0[768];  // 0x0000 - 0x0bfc
    uint32_t volatile SYS_TIME;        // 0x0c00
    uint32_t volatile _unknown1[144];  // 0x0c04 - 0x0e40
    uint32_t volatile HWRNG;           // 0xe44 HW RNG, see http://esp8266-re.foogod.com/wiki/Random_Number_Generator
} __attribute__ (( packed ));

_Static_assert(sizeof(struct WDEV_REGS) == 0xe48, "WDEV_REGS is the wrong size");

/* Extra paranoid check about the HWRNG address, as if this becomes
   wrong there will be no obvious symptoms apart from a lack of
   entropy.
*/
_Static_assert(&WDEV.HWRNG == (void*)0x3FF20E44, "HWRNG register is at wrong address");

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* WDEV_REGS_H */
