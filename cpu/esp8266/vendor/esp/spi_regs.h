/** esp/spi.h
 *
 * Configuration of SPI registers.
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

#ifndef SPI_REGS_H
#define SPI_REGS_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include "esp/types.h"
#include "common_macros.h"

/* Register definitions for the SPI peripherals on the ESP8266.
 *
 * There are twp SPI devices built into the ESP8266:
 *   SPI(0) is at 0x60000200
 *   SPI(1) is at 0x60000100
 * (note that the device number order is reversed in memory)
 *
 * Each device is allocated a block of 64 32-bit registers (256 bytes of
 * address space) to communicate with application code.
 */

#define SPI_BASE 0x60000200
#define SPI(i) (*(struct SPI_REGS *)(0x60000200 - (i)*0x100))

#define SPI0_BASE SPI_BASE
#define SPI1_BASE (SPI_BASE - 0x100)

struct SPI_REGS {
    uint32_t volatile CMD;          // 0x00
    uint32_t volatile ADDR;         // 0x04
    uint32_t volatile CTRL0;        // 0x08
    uint32_t volatile CTRL1;        // 0x0c
    uint32_t volatile RSTATUS;      // 0x10
    uint32_t volatile CTRL2;        // 0x14
    uint32_t volatile CLOCK;        // 0x18
    uint32_t volatile USER0;        // 0x1c
    uint32_t volatile USER1;        // 0x20
    uint32_t volatile USER2;        // 0x24
    uint32_t volatile WSTATUS;      // 0x28
    uint32_t volatile PIN;          // 0x2c
    uint32_t volatile SLAVE0;       // 0x30
    uint32_t volatile SLAVE1;       // 0x34
    uint32_t volatile SLAVE2;       // 0x38
    uint32_t volatile SLAVE3;       // 0x3c
    uint32_t volatile W[16];        // 0x40 - 0x7c
    uint32_t volatile _unused[28];  // 0x80 - 0xec
    uint32_t volatile EXT0;         // 0xf0
    uint32_t volatile EXT1;         // 0xf4
    uint32_t volatile EXT2;         // 0xf8
    uint32_t volatile EXT3;         // 0xfc
};

_Static_assert(sizeof(struct SPI_REGS) == 0x100, "SPI_REGS is the wrong size");

/* Details for CMD register */

#define SPI_CMD_READ                       BIT(31)
#define SPI_CMD_WRITE_ENABLE               BIT(30)
#define SPI_CMD_WRITE_DISABLE              BIT(29)
#define SPI_CMD_READ_ID                    BIT(28)
#define SPI_CMD_READ_SR                    BIT(27)
#define SPI_CMD_WRITE_SR                   BIT(26)
#define SPI_CMD_PP                         BIT(25)
#define SPI_CMD_SE                         BIT(24)
#define SPI_CMD_BE                         BIT(23)
#define SPI_CMD_CE                         BIT(22)
#define SPI_CMD_DP                         BIT(21)
#define SPI_CMD_RES                        BIT(20)
#define SPI_CMD_HPM                        BIT(19)
#define SPI_CMD_USR                        BIT(18)

/* Details for CTRL0 register */

#define SPI_CTRL0_WR_BIT_ORDER             BIT(26)
#define SPI_CTRL0_RD_BIT_ORDER             BIT(25)
#define SPI_CTRL0_QIO_MODE                 BIT(24)
#define SPI_CTRL0_DIO_MODE                 BIT(23)
#define SPI_CTRL0_QOUT_MODE                BIT(20)
#define SPI_CTRL0_DOUT_MODE                BIT(14)
#define SPI_CTRL0_FASTRD_MODE              BIT(13)
#define SPI_CTRL0_CLOCK_EQU_SYS_CLOCK      BIT(12)
#define SPI_CTRL0_CLOCK_NUM_M              0x0000000F
#define SPI_CTRL0_CLOCK_NUM_S              8
#define SPI_CTRL0_CLOCK_HIGH_M             0x0000000F
#define SPI_CTRL0_CLOCK_HIGH_S             4
#define SPI_CTRL0_CLOCK_LOW_M              0x0000000F
#define SPI_CTRL0_CLOCK_LOW_S              0

/* Mask for the CLOCK_NUM/CLOCK_HIGH/CLOCK_LOW combined, in case one wants
 * to set them all as a single value.
 */
#define SPI_CTRL0_CLOCK_M                  0x00000FFF
#define SPI_CTRL0_CLOCK_S                  0

/* Details for CTRL2 register */

#define SPI_CTRL2_CS_DELAY_NUM_M           0x0000000F
#define SPI_CTRL2_CS_DELAY_NUM_S           28
#define SPI_CTRL2_CS_DELAY_MODE_M          0x00000003
#define SPI_CTRL2_CS_DELAY_MODE_S          26
#define SPI_CTRL2_MOSI_DELAY_NUM_M         0x00000007
#define SPI_CTRL2_MOSI_DELAY_NUM_S         23
#define SPI_CTRL2_MOSI_DELAY_MODE_M        0x00000003
#define SPI_CTRL2_MOSI_DELAY_MODE_S        21
#define SPI_CTRL2_MISO_DELAY_NUM_M         0x00000007
#define SPI_CTRL2_MISO_DELAY_NUM_S         18
#define SPI_CTRL2_MISO_DELAY_MODE_M        0x00000003
#define SPI_CTRL2_MISO_DELAY_MODE_S        16

/* Details for CLOCK register */

#define SPI_CLOCK_EQU_SYS_CLOCK            BIT(31)
#define SPI_CLOCK_DIV_PRE_M                0x00001FFF
#define SPI_CLOCK_DIV_PRE_S                18
#define SPI_CLOCK_COUNT_NUM_M              0x0000003F
#define SPI_CLOCK_COUNT_NUM_S              12
#define SPI_CLOCK_COUNT_HIGH_M             0x0000003F
#define SPI_CLOCK_COUNT_HIGH_S             6
#define SPI_CLOCK_COUNT_LOW_M              0x0000003F
#define SPI_CLOCK_COUNT_LOW_S              0

/* Mask for the COUNT_NUM/COUNT_HIGH/COUNT_LOW combined, in case one wants
 * to set them all as a single value.
 */
#define SPI_CTRL0_COUNT_M                  0x0003FFFF
#define SPI_CTRL0_COUNT_S                  0

/* Details for USER0 register */

#define SPI_USER0_COMMAND                  BIT(31)
#define SPI_USER0_ADDR                     BIT(30)
#define SPI_USER0_DUMMY                    BIT(29)
#define SPI_USER0_MISO                     BIT(28)
#define SPI_USER0_MOSI                     BIT(27)
#define SPI_USER0_MOSI_HIGHPART            BIT(25)
#define SPI_USER0_MISO_HIGHPART            BIT(24)
#define SPI_USER0_SIO                      BIT(16)
#define SPI_USER0_FWRITE_QIO               BIT(15)
#define SPI_USER0_FWRITE_DIO               BIT(14)
#define SPI_USER0_FWRITE_QUAD              BIT(13)
#define SPI_USER0_FWRITE_DUAL              BIT(12)
#define SPI_USER0_WR_BYTE_ORDER            BIT(11)
#define SPI_USER0_RD_BYTE_ORDER            BIT(10)
#define SPI_USER0_CLOCK_OUT_EDGE           BIT(7)
#define SPI_USER0_CLOCK_IN_EDGE            BIT(6)
#define SPI_USER0_CS_SETUP                 BIT(5)
#define SPI_USER0_CS_HOLD                  BIT(4)
#define SPI_USER0_FLASH_MODE               BIT(2)
#define SPI_USER0_DUPLEX                   BIT(0)

/* Details for USER1 register */

#define SPI_USER1_ADDR_BITLEN_M            0x0000003F
#define SPI_USER1_ADDR_BITLEN_S            26
#define SPI_USER1_MOSI_BITLEN_M            0x000001FF
#define SPI_USER1_MOSI_BITLEN_S            17
#define SPI_USER1_MISO_BITLEN_M            0x000001FF
#define SPI_USER1_MISO_BITLEN_S            8
#define SPI_USER1_DUMMY_CYCLELEN_M         0x000000FF
#define SPI_USER1_DUMMY_CYCLELEN_S         0

/* Details for USER2 register */

#define SPI_USER2_COMMAND_BITLEN_M         0x0000000F
#define SPI_USER2_COMMAND_BITLEN_S         28
#define SPI_USER2_COMMAND_VALUE_M          0x0000FFFF
#define SPI_USER2_COMMAND_VALUE_S          0

/* Details for PIN register */

#define SPI_PIN_IDLE_EDGE                  BIT(29)  ///< CPOL
#define SPI_PIN_CS2_DISABLE                BIT(2)
#define SPI_PIN_CS1_DISABLE                BIT(1)
#define SPI_PIN_CS0_DISABLE                BIT(0)

/* Details for SLAVE0 register */

#define SPI_SLAVE0_SYNC_RESET              BIT(31)
#define SPI_SLAVE0_MODE                    BIT(30)
#define SPI_SLAVE0_WR_RD_BUF_EN            BIT(29)
#define SPI_SLAVE0_WR_RD_STA_EN            BIT(28)
#define SPI_SLAVE0_CMD_DEFINE              BIT(27)
#define SPI_SLAVE0_TRANS_COUNT_M           0x0000000F
#define SPI_SLAVE0_TRANS_COUNT_S           23
#define SPI_SLAVE0_TRANS_DONE_EN           BIT(9)
#define SPI_SLAVE0_WR_STA_DONE_EN          BIT(8)
#define SPI_SLAVE0_RD_STA_DONE_EN          BIT(7)
#define SPI_SLAVE0_WR_BUF_DONE_EN          BIT(6)
#define SPI_SLAVE0_RD_BUF_DONE_EN          BIT(5)
#define SPI_SLAVE0_INT_EN_M                0x0000001f
#define SPI_SLAVE0_INT_EN_S                5
#define SPI_SLAVE0_TRANS_DONE              BIT(4)
#define SPI_SLAVE0_WR_STA_DONE             BIT(3)
#define SPI_SLAVE0_RD_STA_DONE             BIT(2)
#define SPI_SLAVE0_WR_BUF_DONE             BIT(1)
#define SPI_SLAVE0_RD_BUF_DONE             BIT(0)

/* Details for SLAVE1 register */

#define SPI_SLAVE1_STATUS_BITLEN_M         0x0000001F
#define SPI_SLAVE1_STATUS_BITLEN_S         27
#define SPI_SLAVE1_BUF_BITLEN_M            0x000001FF
#define SPI_SLAVE1_BUF_BITLEN_S            16
#define SPI_SLAVE1_RD_ADDR_BITLEN_M        0x0000003F
#define SPI_SLAVE1_RD_ADDR_BITLEN_S        10
#define SPI_SLAVE1_WR_ADDR_BITLEN_M        0x0000003F
#define SPI_SLAVE1_WR_ADDR_BITLEN_S        4
#define SPI_SLAVE1_WRSTA_DUMMY_ENABLE      BIT(3)
#define SPI_SLAVE1_RDSTA_DUMMY_ENABLE      BIT(2)
#define SPI_SLAVE1_WRBUF_DUMMY_ENABLE      BIT(1)
#define SPI_SLAVE1_RDBUF_DUMMY_ENABLE      BIT(0)

/* Details for SLAVE2 register */

#define SPI_SLAVE2_WRBUF_DUMMY_CYCLELEN_M  0x000000FF
#define SPI_SLAVE2_WRBUF_DUMMY_CYCLELEN_S  24
#define SPI_SLAVE2_RDBUF_DUMMY_CYCLELEN_M  0x000000FF
#define SPI_SLAVE2_RDBUF_DUMMY_CYCLELEN_S  16
#define SPI_SLAVE2_WRSTR_DUMMY_CYCLELEN_M  0x000000FF
#define SPI_SLAVE2_WRSTR_DUMMY_CYCLELEN_S  8
#define SPI_SLAVE2_RDSTR_DUMMY_CYCLELEN_M  0x000000FF
#define SPI_SLAVE2_RDSTR_DUMMY_CYCLELEN_S  0

/* Details for SLAVE3 register */

#define SPI_SLAVE3_WRSTA_CMD_VALUE_M       0x000000FF
#define SPI_SLAVE3_WRSTA_CMD_VALUE_S       24
#define SPI_SLAVE3_RDSTA_CMD_VALUE_M       0x000000FF
#define SPI_SLAVE3_RDSTA_CMD_VALUE_S       16
#define SPI_SLAVE3_WRBUF_CMD_VALUE_M       0x000000FF
#define SPI_SLAVE3_WRBUF_CMD_VALUE_S       8
#define SPI_SLAVE3_RDBUF_CMD_VALUE_M       0x000000FF
#define SPI_SLAVE3_RDBUF_CMD_VALUE_S       0

/* Details for EXT3 register */

#define SPI_EXT3_INT_HOLD_ENABLE_M         0x00000003
#define SPI_EXT3_INT_HOLD_ENABLE_S         0

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* SPI_REGS_H */
