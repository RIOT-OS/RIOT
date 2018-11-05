/* "Boot ROM" function signatures

   Note that a lot of the ROM functions used in the IoT SDK aren't
   referenced from the Espressif RTOS SDK, and are probably incompatible.
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

#ifndef ROM_H
#define ROM_H

#include "esp/types.h"
#include "flashchip.h"

#ifdef    __cplusplus
extern "C" {
#endif

void Cache_Read_Disable(void);

/* http://esp8266-re.foogod.com/wiki/Cache_Read_Enable

   Note: when compiling non-OTA we use the ROM version of this
   function, but for OTA we use the version in extras/rboot-ota that
   maps the correct flash page for OTA support.
 */
void Cache_Read_Enable(uint32_t odd_even, uint32_t mb_count, uint32_t no_idea);

/* Low-level SPI flash read/write routines */
int Enable_QMode(sdk_flashchip_t *chip);
int Disable_QMode(sdk_flashchip_t *chip);
int SPI_page_program(sdk_flashchip_t *chip, uint32_t dest_addr, uint32_t *src_addr, uint32_t size);
int SPI_read_data(sdk_flashchip_t *chip, uint32_t src_addr, uint32_t *dest_addr, uint32_t size);
int SPI_write_enable(sdk_flashchip_t *chip);
int SPI_sector_erase(sdk_flashchip_t *chip, uint32_t addr);
int SPI_read_status(sdk_flashchip_t *chip, uint32_t *status);
int SPI_write_status(sdk_flashchip_t *chip, uint32_t status);
int Wait_SPI_Idle(sdk_flashchip_t *chip);

#ifdef    __cplusplus
}
#endif

#endif /* ROM_H */
