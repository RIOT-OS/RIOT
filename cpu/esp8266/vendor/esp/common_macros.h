/* Some common compiler macros
 *
 * Not esp8266-specific.
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Superhouse Automation Pty Ltd
 * BSD Licensed as described in the file LICENSE
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

#ifndef COMMON_MACROS_H
#define COMMON_MACROS_H

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNUSED __attributed((unused))

#if defined(MCU_ESP8266) && !defined(BIT)
#define BIT(X) (1<<(X))
#endif

/* These macros convert values to/from bitfields specified by *_M and *_S (mask
 * and shift) constants.  Used primarily with ESP8266 register access.
 */

#define VAL2FIELD(fieldname, value) ((value) << fieldname##_S)
#define FIELD2VAL(fieldname, regbits) (((regbits) >> fieldname##_S) & fieldname##_M)

#define FIELD_MASK(fieldname) (fieldname##_M << fieldname##_S)
#define SET_FIELD(regbits, fieldname, value) (((regbits) & ~FIELD_MASK(fieldname)) | VAL2FIELD(fieldname, value))

/* VAL2FIELD/SET_FIELD do not normally check to make sure that the passed value
 * will fit in the specified field (without clobbering other bits).  This makes
 * them faster and is usually fine.  If you do need to make sure that the value
 * will not overflow the field, use VAL2FIELD_M or SET_FIELD_M (which will
 * first mask the supplied value to only the allowed number of bits) instead.
 */
#define VAL2FIELD_M(fieldname, value) (((value) & fieldname##_M) << fieldname##_S)
#define SET_FIELD_M(regbits, fieldname, value) (((regbits) & ~FIELD_MASK(fieldname)) | VAL2FIELD_M(fieldname, value))

/* Set bits in reg with specified mask.
 */
#define SET_MASK_BITS(reg, mask) (reg) |= (mask)

/* Clear bits in reg with specified mask
 */
#define CLEAR_MASK_BITS(reg, mask) (reg) &= ~(mask)

/* Use the IRAM macro to place functions into Instruction RAM (IRAM)
   instead of flash (aka irom).

   (This is the opposite to the Espressif SDK, where functions default
   to being placed in IRAM but the ICACHE_FLASH_ATTR attribute will
   place them in flash.)

   Use the IRAM attribute for functions which are called when the
   flash may not be available (for example during NMI exceptions), or
   for functions which are called very frequently and need high
   performance.

   Usage example:

   void IRAM high_performance_function(void)
   {
       // do important thing here
   }

   Bear in mind IRAM is limited (32KB), compared to up to 1MB of flash.
*/
#define IRAM __attribute__((section(".iram1.text")))

/* Use the RAM macro to place constant data (rodata) into RAM (data
   RAM) instead of the default placement in flash. This is useful for
   constant data which needs high performance access.

   Usage example:

   const RAM uint8_t constants[] = { 1, 2, 3, 7 };

   When placing string literals in RAM, they need to be declared with
   the type "const char[]" not "const char *"

   Usage example:

   const RAM char hello_world[] = "Hello World";
*/
#define RAM __attribute__((section(".data")))

/* Use the IRAM_DATA macro to place data into Instruction RAM (IRAM)
   instead of the default of flash (for constant data) or data RAM
   (for non-constant data).

   This may be useful to free up data RAM. However all data read from
   any instruction space (either IRAM or Flash) must be 32-bit aligned
   word reads. Reading unaligned data stored with IRAM_DATA will be
   slower than reading data stored in RAM. You can't perform unaligned
   writes to IRAM.
*/
#define IRAM_DATA __attribute__((section(".iram1.data")))

/* Use the IROM macro to store constant values in IROM flash. In
  esp-open-rtos this is already the default location for most constant
  data (rodata), so you don't need this attribute in 99% of cases.

  The exceptions are to mark data in the core & freertos libraries,
  where the default for constant data storage is RAM.

  (Unlike the Espressif SDK you don't need to use an attribute like
  ICACHE_FLASH_ATTR for functions, they go into flash by default.)

  Important to note: IROM flash is accessed via 32-bit word aligned
  reads. esp-open-rtos does some magic to "fix" unaligned reads, but
  performance is reduced.
*/
#ifdef    __cplusplus
    #define IROM __attribute__((section(".irom0.text")))
    #define IROM_LIT __attribute__((section(".irom0.literal")))
#else
    #define IROM __attribute__((section(".irom0.text")))
    #define IROM_LIT __attribute__((section(".irom0.literal"))) const
#endif

#ifdef __cplusplus
}
#endif

#endif /* COMMON_MACROS_H */
