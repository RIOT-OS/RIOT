/**
 * Copyright (C) 1992-2017 Arjen Lentz
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @ingroup     drivers_ds18b20
 * @{
 *
 * @file
 * @brief       Lookup table based OneWire CRC implementation
 *
 * @see         https://lentz.com.au/blog/calculating-crc-with-a-tiny-32-entry-lookup-table
 *
 * @author      Arjen Lentz
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>

/**
 * @brief   Dow-CRC using polynomial X^8 + X^5 + X^4 + X^0
 */
static const uint8_t dscrc_table[] = {
    0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
    0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
    0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
    0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74
};

/**
 * @brief   Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
 *          and the registers.
 */
uint8_t onewire_crc8(const uint8_t *addr, uint8_t len)
{
    uint8_t crc = 0;

    while (len--) {
        crc = *addr++ ^ crc;    /* just re-using crc as intermediate */
        crc = dscrc_table[crc & 0x0f] ^
              dscrc_table[16 + ((crc >> 4) & 0x0f)];
    }

    return crc;
}
