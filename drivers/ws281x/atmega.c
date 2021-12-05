/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 *
 * @file
 * @brief       Implementation of `ws281x_write_buffer()` for ATmega MCUs
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "clk.h"
#include "atmega_gpio.h"
#include "ws281x.h"
#include "ws281x_params.h"
#include "ws281x_constants.h"
#include "periph_cpu.h"

/*
 * Data encoding according to the datasheets of the WS2812 and the SK6812:
 * - Encoding of zero bit:
 *   1. High for 325ns ± 125ns (average of WS2812 and SK6812)
 *   2. Low for 850ns ± 100ns (average of WS2812 and SK6812)
 * - Encoding of one bit:
 *   1. High for 650ns ± 100ns (average of WS2812 and SK6812)
 *   2. Low for 600ns ± 150ns (for both WS2812 and SK6812)
 *
 * While the data sheet claims the opposite, the precision requirement for
 * the duration of the second phase (the low period) is in reality super
 * lax. Therefore, we bit bang the high period precisely in inline assembly,
 * and completely ignore the timing of the low phase. This works just fine.
 *
 * Further: For an 8MHz clock, we need a single CPU cycle access to the
 * GPIO port. The is only possible with the `out` instruction, which takes
 * the port to write to as immediate. Thus, it has to be known at compile
 * time. We therefore simply provide 3 implementations for each of the
 * three most commonly used GPIO ports. For the 16MHz version accessing the
 * GPIO port via memory using the st instruction works fine, so no limitations
 * on the GPIO por there.
 *
 * On final trick: A relative jump to the next instructions takes two
 * CPU cycles; this, its 2 NOPs for the price of one (in terms of ROM
 * consumption).
 *
 * High phase timings
 *
 * +------------+-------------------+-------------------+-------------------+
 * | Data       | Device/Frequency  | Min               | Max               |
 * +------------+-------------------+-------------------+-------------------+
 * | 0 bit      | WS2812            | 200ns             | 500ns             |
 * | 1 bit      | WS2812            | 550ns             | 850ns (*)         |
 * +------------+-------------------+-------------------+-------------------+
 * | 0 bit      | SK6812            | 150ns             | 450ns             |
 * | 1 bit      | SK6812            | 450ns             | 750ns             |
 * +------------+-------------------+-------------------+-------------------+
 * | 0 bit      | WS2812/SK6812     | 200ns             | 450ns             |
 * | 1 bit      | WS2812/SK6812     | 550ns             | 750ns             |
 * +------------+-------------------+-------------------+-------------------+
 * | 0 bit      | 8 MHz             |  2 Cycles (250ns) |  3 Cycles (375ns) |
 * | 1 bit      | 8 MHz             |  5 Cycles (625ns) |  6 Cycles (750ns) |
 * +------------+-------------------+-------------------+-------------------+
 * | 0 bit      | 16 MHz            |  4 Cycles (250ns) |  7 Cycles (438ns) |
 * | 1 bit      | 16 MHz            |  9 Cycles (563ns) | 12 Cycles (750ns) |
 * +------------+-------------------+-------------------+-------------------+
 *
 * (*) The high time for encoding a 1 bit on the WS2812 has no upper bound in
 *     practise; A high period of 5 seconds has been reported to work reliable.
 */

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    assert(dev);
    const uint8_t *pos = buf;
    const uint8_t *end = pos + size;
    uint16_t port_addr = atmega_port_addr(dev->params.pin);
    uint8_t mask_on, mask_off;

    {
        uint8_t port_state = _SFR_MEM8(port_addr);
        mask_on = port_state | (1 << atmega_pin_num(dev->params.pin));
        mask_off = port_state & ~(1 << atmega_pin_num(dev->params.pin));
    }

    if (coreclk() >= 7500000U && coreclk() <= 8500000U) {
        const uint8_t port_num = atmega_port_num(dev->params.pin);
        switch (port_num) {
            case PORT_B:
                while (pos < end) {
                    uint8_t cnt = 8;
                    uint8_t data = *pos;
                    while (cnt > 0) {
                        __asm__ volatile(                       /* Cycles | 1 | 0 */
                            "out    %[port], %[on]"     "\n\t"  /* 1      | x | x */
                            "sbrs   %[data], 7"         "\n\t"  /* 1      | x | x */
                            "out    %[port], %[off]"    "\n\t"  /* 1      | - | x */
                            "rjmp   .+0"                "\n\t"  /* 2      | x | x */
                            "out    %[port], %[off]"    "\n\t"  /* 1      | x | x */
                            /* Total CPU cycles for high period:
                             * 5 cycles for bit 1, 2 cycles for bit 0 */
                            : [data]    "+r" (data)
                            : [port]    "I" (_SFR_IO_ADDR(PORTB)),
                              [on]      "r" (mask_on),
                              [off]     "r" (mask_off)
                        );
                        cnt--;
                        data <<= 1;
                    }
                    pos++;
                }
                break;
#ifdef PORT_C
            case PORT_C:
                while (pos < end) {
                    uint8_t cnt = 8;
                    uint8_t data = *pos;
                    while (cnt > 0) {
                        __asm__ volatile(                       /* Cycles | 1 | 0 */
                            "out    %[port], %[on]"     "\n\t"  /* 1      | x | x */
                            "sbrs   %[data], 7"         "\n\t"  /* 1      | x | x */
                            "out    %[port], %[off]"    "\n\t"  /* 1      | - | x */
                            "rjmp   .+0"                "\n\t"  /* 2      | x | x */
                            "out    %[port], %[off]"    "\n\t"  /* 1      | x | x */
                            /* Total CPU cycles for high period:
                             * 5 cycles for bit 1, 2 cycles for bit 0 */
                            : [data]    "+r" (data)
                            : [port]    "I" (_SFR_IO_ADDR(PORTC)),
                              [on]      "r" (mask_on),
                              [off]     "r" (mask_off)
                        );
                        cnt--;
                        data <<= 1;
                    }
                    pos++;
                }
                break;
#endif
            case PORT_D:
                while (pos < end) {
                    uint8_t cnt = 8;
                    uint8_t data = *pos;
                    while (cnt > 0) {
                        __asm__ volatile(                       /* Cycles | 1 | 0 */
                            "out    %[port], %[on]"     "\n\t"  /* 1      | x | x */
                            "sbrs   %[data], 7"         "\n\t"  /* 1      | x | x */
                            "out    %[port], %[off]"    "\n\t"  /* 1      | - | x */
                            "rjmp   .+0"                "\n\t"  /* 2      | x | x */
                            "out    %[port], %[off]"    "\n\t"  /* 1      | x | x */
                            /* Total CPU cycles for high period:
                             * 5 cycles for bit 1, 2 cycles for bit 0 */
                            : [data]    "+r" (data)
                            : [port]    "I" (_SFR_IO_ADDR(PORTD)),
                              [on]      "r" (mask_on),
                              [off]     "r" (mask_off)
                        );
                        cnt--;
                        data <<= 1;
                    }
                    pos++;
                }
                break;
            default:
                assert(0);
                break;
        }
    }
    else if (coreclk() >= 15500000U && coreclk() <= 16500000U) {
        while (pos < end) {
            uint8_t cnt = 8;
            uint8_t data = *pos;
            while (cnt > 0) {
                __asm__ volatile(                       /* CPU Cycles | 1 | 0 */
                    "st     %a[port], %[on]"    "\n\t"  /* 2          | x | x */
                    "sbrc   %[data], 7"         "\n\t"  /* 1          | x | x */
                    "rjmp   .+0"                "\n\t"  /* 2          | x | - */
                    "sbrc   %[data], 7"         "\n\t"  /* 1          | x | x */
                    "rjmp   .+0"                "\n\t"  /* 2          | x | - */
                    "sbrc   %[data], 7"         "\n\t"  /* 1          | x | x */
                    "nop"                       "\n\t"  /* 1          | x | - */
                    "st     %a[port], %[off]"   "\n\t"  /* 2          | x | x */
                    /* Total CPU cycles for high period:
                     * 10 cycles for bit 1, 5 cycles for bit 0 */
                    : [data]    "+r" (data)
                    : [on]      "r" (mask_on),
                      [port]    "e" (port_addr),
                      [off]     "r" (mask_off)
                );
                cnt--;
                data <<= 1;
            }
            pos++;
        }
    }
    else {
        /* No low level WS281x implementation for ATmega CPUs for the used CPU
         * clock. Abusing the linker to give a compile time error instead of
         * emitting a broken firmware */
        extern void clock_frequency_is_not_supported_by_ws281x_driver(void);
        clock_frequency_is_not_supported_by_ws281x_driver();
    }
}

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ws281x_t));
    dev->params = *params;

    if (gpio_init(dev->params.pin, GPIO_OUT)) {
        return -EIO;
    }

    return 0;
}
