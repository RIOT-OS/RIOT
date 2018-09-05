/* esp/iomux_regs.h
 *
 * ESP8266 IOMUX register definitions
 *
 * Not compatible with ESP SDK register access code.
 *
 * Note that IOMUX register order is _not_ the same as GPIO order. See
 * esp/iomux.h for programmer-friendly IOMUX configuration options.
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

#ifndef IOMUX_REGS_H
#define IOMUX_REGS_H

#ifndef DOXYGEN

#include "esp/types.h"
#include "common_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IOMUX_BASE 0x60000800
#define IOMUX (*(struct IOMUX_REGS *)(IOMUX_BASE))

struct IOMUX_REGS {
    uint32_t volatile CONF;    // 0x00
    uint32_t volatile PIN[16]; // 0x04 - 0x40
};

_Static_assert(sizeof(struct IOMUX_REGS) == 0x44, "IOMUX_REGS is the wrong size");

/* Details for CONF register */

#define IOMUX_CONF_SPI0_CLOCK_EQU_SYS_CLOCK  BIT(8)
#define IOMUX_CONF_SPI1_CLOCK_EQU_SYS_CLOCK  BIT(9)

/* Details for PIN registers */

#define IOMUX_PIN_OUTPUT_ENABLE        BIT(0)
#define IOMUX_PIN_OUTPUT_ENABLE_SLEEP  BIT(1)
#define IOMUX_PIN_PULLDOWN_SLEEP       BIT(2)
#define IOMUX_PIN_PULLUP_SLEEP         BIT(3)
#define IOMUX_PIN_FUNC_LOW_M           0x00000003
#define IOMUX_PIN_FUNC_LOW_S           4
#define IOMUX_PIN_PULLDOWN             BIT(6)
#define IOMUX_PIN_PULLUP               BIT(7)
#define IOMUX_PIN_FUNC_HIGH_M          0x00000004
#define IOMUX_PIN_FUNC_HIGH_S          6

#define IOMUX_PIN_FUNC_MASK            0x00000130

/* WARNING: Macro evaluates argument twice */
#define IOMUX_FUNC(val) (VAL2FIELD_M(IOMUX_PIN_FUNC_LOW, val) | VAL2FIELD_M(IOMUX_PIN_FUNC_HIGH, val))

#define IOMUX_GPIO0   IOMUX.PIN[12]
#define IOMUX_GPIO1   IOMUX.PIN[5]
#define IOMUX_GPIO2   IOMUX.PIN[13]
#define IOMUX_GPIO3   IOMUX.PIN[4]
#define IOMUX_GPIO4   IOMUX.PIN[14]
#define IOMUX_GPIO5   IOMUX.PIN[15]
#define IOMUX_GPIO6   IOMUX.PIN[6]
#define IOMUX_GPIO7   IOMUX.PIN[7]
#define IOMUX_GPIO8   IOMUX.PIN[8]
#define IOMUX_GPIO9   IOMUX.PIN[9]
#define IOMUX_GPIO10  IOMUX.PIN[10]
#define IOMUX_GPIO11  IOMUX.PIN[11]
#define IOMUX_GPIO12  IOMUX.PIN[0]
#define IOMUX_GPIO13  IOMUX.PIN[1]
#define IOMUX_GPIO14  IOMUX.PIN[2]
#define IOMUX_GPIO15  IOMUX.PIN[3]

#define IOMUX_GPIO0_FUNC_GPIO              IOMUX_FUNC(0)
#define IOMUX_GPIO0_FUNC_SPI0_CS2          IOMUX_FUNC(1)
#define IOMUX_GPIO0_FUNC_CLOCK_OUT         IOMUX_FUNC(4)

#define IOMUX_GPIO1_FUNC_UART0_TXD         IOMUX_FUNC(0)
#define IOMUX_GPIO1_FUNC_SPI0_CS1          IOMUX_FUNC(1)
#define IOMUX_GPIO1_FUNC_GPIO              IOMUX_FUNC(3)
#define IOMUX_GPIO1_FUNC_CLOCK_RTC_BLINK   IOMUX_FUNC(4)

#define IOMUX_GPIO2_FUNC_GPIO              IOMUX_FUNC(0)
#define IOMUX_GPIO2_FUNC_I2SO_WS           IOMUX_FUNC(1)
#define IOMUX_GPIO2_FUNC_UART1_TXD         IOMUX_FUNC(2)
#define IOMUX_GPIO2_FUNC_UART0_TXD         IOMUX_FUNC(4)

#define IOMUX_GPIO3_FUNC_UART0_RXD         IOMUX_FUNC(0)
#define IOMUX_GPIO3_FUNC_I2SO_DATA         IOMUX_FUNC(1)
#define IOMUX_GPIO3_FUNC_GPIO              IOMUX_FUNC(3)
#define IOMUX_GPIO3_FUNC_CLOCK_XTAL_BLINK  IOMUX_FUNC(4)

#define IOMUX_GPIO4_FUNC_GPIO              IOMUX_FUNC(0)
#define IOMUX_GPIO4_FUNC_CLOCK_XTAL        IOMUX_FUNC(1)

#define IOMUX_GPIO5_FUNC_GPIO              IOMUX_FUNC(0)
#define IOMUX_GPIO5_FUNC_CLOCK_RTC         IOMUX_FUNC(1)

#define IOMUX_GPIO6_FUNC_SD_CLK            IOMUX_FUNC(0)
#define IOMUX_GPIO6_FUNC_SPI0_CLK          IOMUX_FUNC(1)
#define IOMUX_GPIO6_FUNC_GPIO              IOMUX_FUNC(3)
#define IOMUX_GPIO6_FUNC_UART1_CTS         IOMUX_FUNC(4)

#define IOMUX_GPIO7_FUNC_SD_DATA0          IOMUX_FUNC(0)
#define IOMUX_GPIO7_FUNC_SPI0_Q_MISO       IOMUX_FUNC(1)
#define IOMUX_GPIO7_FUNC_GPIO              IOMUX_FUNC(3)
#define IOMUX_GPIO7_FUNC_UART1_TXD         IOMUX_FUNC(4)

#define IOMUX_GPIO8_FUNC_SD_DATA1          IOMUX_FUNC(0)
#define IOMUX_GPIO8_FUNC_SPI0_D_MOSI       IOMUX_FUNC(1)
#define IOMUX_GPIO8_FUNC_GPIO              IOMUX_FUNC(3)
#define IOMUX_GPIO8_FUNC_UART1_RXD         IOMUX_FUNC(4)

#define IOMUX_GPIO9_FUNC_SD_DATA2          IOMUX_FUNC(0)
#define IOMUX_GPIO9_FUNC_SPI0_HD           IOMUX_FUNC(1)
#define IOMUX_GPIO9_FUNC_GPIO              IOMUX_FUNC(3)
#define IOMUX_GPIO9_FUNC_SPI1_HD           IOMUX_FUNC(4)

#define IOMUX_GPIO10_FUNC_SD_DATA3         IOMUX_FUNC(0)
#define IOMUX_GPIO10_FUNC_SPI0_WP          IOMUX_FUNC(1)
#define IOMUX_GPIO10_FUNC_GPIO             IOMUX_FUNC(3)
#define IOMUX_GPIO10_FUNC_SPI1_WP          IOMUX_FUNC(4)

#define IOMUX_GPIO11_FUNC_SD_CMD           IOMUX_FUNC(0)
#define IOMUX_GPIO11_FUNC_SPI0_CS0         IOMUX_FUNC(1)
#define IOMUX_GPIO11_FUNC_GPIO             IOMUX_FUNC(3)
#define IOMUX_GPIO11_FUNC_UART1_RTS        IOMUX_FUNC(4)

#define IOMUX_GPIO12_FUNC_MTDI             IOMUX_FUNC(0)
#define IOMUX_GPIO12_FUNC_I2SI_DATA        IOMUX_FUNC(1)
#define IOMUX_GPIO12_FUNC_SPI1_Q_MISO      IOMUX_FUNC(2)
#define IOMUX_GPIO12_FUNC_GPIO             IOMUX_FUNC(3)
#define IOMUX_GPIO12_FUNC_UART0_DTR        IOMUX_FUNC(4)

#define IOMUX_GPIO13_FUNC_MTCK             IOMUX_FUNC(0)
#define IOMUX_GPIO13_FUNC_I2SI_BCK         IOMUX_FUNC(1)
#define IOMUX_GPIO13_FUNC_SPI1_D_MOSI      IOMUX_FUNC(2)
#define IOMUX_GPIO13_FUNC_GPIO             IOMUX_FUNC(3)
#define IOMUX_GPIO13_FUNC_UART0_CTS        IOMUX_FUNC(4)

#define IOMUX_GPIO14_FUNC_MTMS             IOMUX_FUNC(0)
#define IOMUX_GPIO14_FUNC_I2SI_WS          IOMUX_FUNC(1)
#define IOMUX_GPIO14_FUNC_SPI1_CLK         IOMUX_FUNC(2)
#define IOMUX_GPIO14_FUNC_GPIO             IOMUX_FUNC(3)
#define IOMUX_GPIO14_FUNC_UART0_DSR        IOMUX_FUNC(4)

#define IOMUX_GPIO15_FUNC_MTDO             IOMUX_FUNC(0)
#define IOMUX_GPIO15_FUNC_I2SO_BCK         IOMUX_FUNC(1)
#define IOMUX_GPIO15_FUNC_SPI1_CS0         IOMUX_FUNC(2)
#define IOMUX_GPIO15_FUNC_GPIO             IOMUX_FUNC(3)
#define IOMUX_GPIO15_FUNC_UART0_RTS        IOMUX_FUNC(4)

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* IOMUX_REGS_H */
