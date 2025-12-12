// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdio.h>

#include "sdkconfig.h"

#include "esp_attr.h"

#include "esp8266/eagle_soc.h"
#include "esp8266/uart_register.h"
#include "esp8266/rom_functions.h"

#ifdef RIOT_OS

/* the putchar definition from stdio.h can't be used here */
#undef putchar

/* inidicator is true when stdio is initialized */
int stdio_is_initialized = 0;

#endif /* RIOT_OS */

#ifndef CONFIG_CONSOLE_UART_NONE
static void uart_putc(int c)
{
#ifdef RIOT_OS
    if (stdio_is_initialized) {
        putchar(c);
        return;
    }
#endif /* RIOT_OS */

    while (1) {
        uint32_t fifo_cnt = READ_PERI_REG(UART_STATUS(CONFIG_CONSOLE_UART_NUM)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);

        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126)
            break;
    }

    WRITE_PERI_REG(UART_FIFO(CONFIG_CONSOLE_UART_NUM) , c);
}
#else
#define uart_putc(_c) { }
#endif

int __attribute__ ((weak)) ets_putc(int c)
{
#ifdef CONFIG_NEWLIB_STDOUT_LINE_ENDING_CRLF
    if (c == '\n')
        uart_putc('\r');
#elif defined(CONFIG_NEWLIB_STDOUT_LINE_ENDING_CR)
    if (c == '\n')
        c = '\r';
#endif

    uart_putc(c);

    return c;
}

#if defined(CONFIG_USING_NEW_ETS_VPRINTF) && !defined(BOOTLOADER_BUILD)

#define FILL_0      0x01
#define FILL_LEFT   0x02
#define POINTOR     0x04
#define ALTERNATE   0x08
#define OUPUT_INT   0x10
#define START       0x20

#define VINT_STR_MAX 10

typedef union _val_cache {
    uint8_t         val8;
    int32_t         val32;
    uint32_t        val32u;
    const char      *valcp;
} val_cache_t;

typedef struct _val_attr {
    int8_t          type;
    uint8_t         state;
    uint8_t         fillbytes;
    uint8_t         precision;

    val_cache_t     value;
} val_attr_t;

#define isdigit(_c)                 ((_c <= '9') && (_c >= '0'))
#define fill_num(_attr)             ((attr)->fillbytes)
#define isfill_0(_attr)             (fill_num(_attr) && ((_attr)->state & FILL_0))
#define isfill_left(_attr)          (fill_num(_attr) && ((_attr)->state & FILL_LEFT))
#define isstart(_attr)              ((_attr)->state & START)

static inline void ets_printf_ch_mutlti(uint32_t c, uint32_t len)
{
    while (len--)
        ets_putc(c);
}

static inline void ets_printf_buf(const char *s, uint32_t len)
{
    while (len--)
        ets_putc(*s++);
}

static int ets_printf_str(const val_attr_t * const attr)
{
    const char *s = attr->value.valcp;
    s = s == NULL ? "<null>" : s;

    if (fill_num(attr)) {
        unsigned char left;
        unsigned char len;

        while (*s != '\0')
            s++;
        len = s - attr->value.valcp;
        left = fill_num(attr) > len ? fill_num(attr) - len : 0;

        if (!isfill_left(attr)) {
            ets_printf_ch_mutlti(' ', left);
        }

        ets_printf_buf(attr->value.valcp, len);

        if (isfill_left(attr)) {
            ets_printf_ch_mutlti(' ', left);
        }
    } else {
        while (*s != '\0')
            ets_putc(*s++);
    }

    return 0;
}

static int ets_printf_int(val_attr_t * const attr, uint8_t hex)
{
    char buf[VINT_STR_MAX];
    unsigned char offset = VINT_STR_MAX;

    if (attr->value.val32u != 0) {
        for (; attr->value.val32u > 0; attr->value.val32u /= hex) {
            unsigned char c = attr->value.val32u % hex;
            if (c < 10)
                buf[--offset] = c + '0';
            else
                buf[--offset] = c + 'a' - 10;
        }
    } else
        buf[--offset] = '0';

    if (fill_num(attr)) {
        char fill_data = isfill_0(attr) ? '0' : ' ';
        unsigned char len = fill_num(attr) - (VINT_STR_MAX - offset);
        unsigned char left = fill_num(attr) > (VINT_STR_MAX - offset) ? len : 0;

        if (!isfill_left(attr)) {
            ets_printf_ch_mutlti(fill_data, left);
        }

        ets_printf_buf(&buf[offset], VINT_STR_MAX - offset);

        if (isfill_left(attr)) {
            ets_printf_ch_mutlti(fill_data, left);
        }
    } else {
        ets_printf_buf(&buf[offset], VINT_STR_MAX - offset);
    }

    return 0;
}

int ets_vprintf(const char *fmt, va_list va)
{
    for (; ;) {
        const char *ps = fmt;
        val_attr_t attr;

        while (*ps != '\0' && *ps != '%')
            ets_putc(*ps++);

        if (*ps == '\0')
            break;

        fmt = ps;

        attr.state = 0;
        attr.type = -1;
        attr.fillbytes = 0;
        attr.precision = 0;

        for (; ;) {
            switch (*++ps) {
                case 'd':
                case 'i':
                case 'u':
                case 'x':
                case 'c':
                case 's':
                case 'p':
                case '\0':
                    attr.type = *ps++;
                    break;
                case '#':
                    attr.state |= ALTERNATE;
                    ps++;
                    break;
                case '0'...'9':
                    if (!isstart(&attr) && *ps == '0') {
                        attr.state |= FILL_0;
                    } else {
                        if (attr.state & POINTOR)
                            attr.precision = attr.precision * 10 + *ps - '0';
                        else
                            attr.fillbytes = attr.fillbytes * 10 + *ps - '0';
                    }
                    break;
                case '.':
                    attr.state |= POINTOR;
                    break;
                case '-':
                    attr.state |= FILL_LEFT;
                    break;
                default:
                    attr.type = -2;
                    break;
            }

            attr.state |= START;

            if (attr.type != -1)
                break;
        }

        switch (attr.type) {
            case 'c':
                attr.value.val8 = (char)va_arg(va, int);
                ets_putc(attr.value.val8);
                break;
            case 's':
                attr.value.valcp = va_arg(va, const char *);
                ets_printf_str(&attr);
                break;
            case 'i':
            case 'd':
                attr.value.val32 = va_arg(va, int);
                if (attr.value.val32 < 0) {
                    ets_putc('-');
                    attr.value.val32 = -attr.value.val32;
                }
                ets_printf_int(&attr, 10);
                break;
            case 'u':
                attr.value.val32u = va_arg(va, unsigned int);
                ets_printf_int(&attr, 10);
                break;
            case 'x':
                attr.value.val32u = va_arg(va, unsigned int);
                ets_printf_int(&attr, 16);
                break;
            case 'p':
                attr.value.valcp = va_arg(va, const char *);
                ets_printf_int(&attr, 16);
            default:
                break;
        }

        fmt = ps;
    }

    return 0;
}

#else /* defined(CONFIG_USING_NEW_ETS_VPRINTF) && !defined(BOOTLOADER_BUILD) */

int ets_vprintf(const char *fmt, va_list ap)
{
    return ets_io_vprintf(ets_putc, fmt, ap);
}

#endif /* defined(CONFIG_USING_NEW_ETS_VPRINTF) && !defined(BOOTLOADER_BUILD) */

/**
 * Re-write ets_printf in SDK side, since ets_printf in ROM will use a global
 * variable which address is in heap region of SDK side. If use ets_printf in ROM,
 * this variable maybe re-write when heap alloc and modification.
 *
 * Using new "ets_vprintf" costs stack without alignment and accuracy:
 *                      just "fmt": 136 Bytes
 *                            "%s": 172 Bytes
 *      "%p", "%d, "%i, "%u", "%x": 215 Bytes
 */
int ets_printf(const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = ets_vprintf(fmt, ap);
    va_end(ap);

    return ret;
}
