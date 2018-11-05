/* flashchip.h
 *
 * sdk_flashchip_t structure used by the SDK and some bootrom routines
 *
 * This is in a separate include file because it's referenced by several other
 * headers which are otherwise independent of each other.
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Alex Stewart and Angus Gratton
 * BSD Licensed as described in the file LICENSE
 */

/*
Copyright (C) 2015 Alex Stewart and Angus Gratton
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FLASHCHIP_H
#define FLASHCHIP_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/* SDK/bootrom uses this structure internally to account for flash size.

   chip_size field is initialised during startup from the flash size
   saved in the image header (on the first 8 bytes of SPI flash).

   Other field are initialised to hardcoded values by the SDK.

   ** NOTE: This structure is passed to some bootrom routines and is therefore
   fixed.  Be very careful if you want to change it that you do not break
   things. **

   Based on RE work by @foogod at
   http://esp8266-re.foogod.com/wiki/Flashchip_%28IoT_RTOS_SDK_0.9.9%29
*/
typedef struct {
    uint32_t device_id;
    uint32_t chip_size;   /* in bytes */
    uint32_t block_size;  /* in bytes */
    uint32_t sector_size; /* in bytes */
    uint32_t page_size;   /* in bytes */
    uint32_t status_mask;
} sdk_flashchip_t;

extern sdk_flashchip_t sdk_flashchip;

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* FLASHCHIP_H */
