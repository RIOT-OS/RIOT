/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotboot_serial
 * @{
 *
 * @file
 * @brief       Serial Bootloader
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "stdio_uart.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "periph/flashpage.h"
#include "unaligned.h"
#include "checksum/crc8.h"
#include "riotboot/serial.h"
#include "riotboot/magic.h"
#include "riotboot/bootloader_selection.h"

#include "board.h"

#ifndef RIOTBOOT_UART_DEV
#define RIOTBOOT_UART_DEV       STDIO_UART_DEV
#endif

#ifndef RIOTBOOT_UART_BAUDRATE
#define RIOTBOOT_UART_BAUDRATE  STDIO_UART_BAUDRATE
#endif

#ifndef RIOTBOOT_DELAY_MS
#define RIOTBOOT_DELAY_MS       (1000)
#endif

#define RX_BUF_LEN              (40)

/* we send a characters each iteration -> 8 bit + start & stop bit */
#define RIOTBOOT_DELAY (RIOTBOOT_DELAY_MS * RIOTBOOT_UART_BAUDRATE / 10000)

static inline void uart_write_byte(uart_t uart, uint8_t data)
{
    uart_write(uart, &data, 1);
}

static inline bool _boot_pin(void)
{
#ifdef BTN_BOOTLOADER_PIN
    if (BTN_BOOTLOADER_INVERTED) {
        return !gpio_read(BTN_BOOTLOADER_PIN);
    }
    else {
        return gpio_read(BTN_BOOTLOADER_PIN);
    }
#else
    return false;
#endif
}

static inline void _boot_led_toggle(void)
{
#ifdef LED_BOOTLOADER_PIN
    static unsigned count = RIOTBOOT_DELAY / 10;

    if (--count == 0) {
        count = RIOTBOOT_DELAY / 10;
        LED_BOOTLOADER_TOGGLE;
    }
#endif
}

/* send 'hello' byte until we get enter bootloader byte or timeout */
static bool _bootdelay(unsigned tries, volatile bool *boot_default)
{
    uint32_t *magic = (void *)(uintptr_t)RIOTBOOT_MAGIC_ADDR;

    if (*magic == RIOTBOOT_MAGIC) {
        *magic = 0;
        return false;
    }

    if (tries == 0) {
        return true;
    }

    while (--tries && *boot_default) {
        uart_write_byte(RIOTBOOT_UART_DEV, 0);
        if (_boot_pin()) {
            return false;
        }
        _boot_led_toggle();
    }

    return *boot_default;
}

__attribute__ ((aligned(4)))
static struct {
    uint8_t pos;                /* current pos in rx buffer */
    uint8_t remaining;          /* remaining bytes to read  */
    union {
        uint8_t u8[RX_BUF_LEN]; /* rx buffer */
        struct {
            uint8_t type;       /* command type */
            uint8_t len;        /* length of data (without checksum) */
            uint8_t data[];     /* data is aligned at word boundary  */
        } val;
    } rx;
} ctx;

/**
 * Format:
 * [ Type (1 byte) | Length (1 byte) | value (n bytes) | checksum (1 byte) ]
 */
static void _uart_rx_cmd(void *arg, uint8_t data)
{
    bool *reading = arg;
    uint8_t crc;

    /* ignore RX while processing buffer */
    if (!reading) {
        return;
    }

    ctx.rx.u8[ctx.pos] = data;

    switch (ctx.pos) {
    case 0:
        switch (data) {
        case RIOTBOOT_CMD_BOOT:
        case RIOTBOOT_CMD_ERASE:
        case RIOTBOOT_CMD_WRITE:
        case RIOTBOOT_CMD_GET_PAGE:
            ctx.remaining = 1;
            break;
        /* re-create initial sync handshake if already in bootloader */
        case RIOTBOOT_PROBE:
            uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_WAITING);
            return;
        case RIOTBOOT_ENTER_LOADER:
            uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_READY);
            *reading = false;
            return;
        default:
            return;
        }

        break;
    case 1:
        /* data length + checksum byte + dummy byte for fall-through */
        ctx.remaining = data + 2;

        /* boot command needs no checksum */
        if (ctx.rx.val.type == RIOTBOOT_CMD_BOOT) {
            *reading = false;
            return;
        }

        /* bail out early if the buffer would not fit */
        /* data len + sizeof(type, len) */
        if (ctx.remaining + 2 >= RX_BUF_LEN) {
            crc = 0;
            goto error;
        }

    /* fall-through */
    default:

        /* end of data block not reached */
        if (--ctx.remaining) {
            break;
        }

    /* fall-through */
    case (RX_BUF_LEN - 1):

        /* calculate checksum */
        crc = crc8(ctx.rx.u8, ctx.pos, RIOTBOOT_CRC8_POLY, 0xFF);

error:
        ctx.remaining = 0;
        ctx.pos = 0;

        /* checksum error */
        if (crc != data) {
            uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_BAD_CRC);
            break;
        }

        *reading = false;
        break;
    }

    if (ctx.remaining) {
        ++ctx.pos;
    }

    return;
}

static void _get_page(uintptr_t addr)
{
    uint32_t page = flashpage_page((void *)addr);

    uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_OK);
    uart_write(RIOTBOOT_UART_DEV, (void *)&page, sizeof(page));
}

static void _erase(uint16_t sector)
{
    /* don't erase bootloader */
    if ((uintptr_t)flashpage_addr(sector) < SLOT0_OFFSET) {
        uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_ILLEGAL);
        return;
    }

    flashpage_erase(sector);

    uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_OK);
}

static void _write(uint32_t addr, uint8_t len, const uint8_t *data)
{
    /* don't overwrite bootloader */
    if (addr < SLOT0_OFFSET) {
        uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_ILLEGAL);
        return;
    }

    flashpage_write((void *)addr, data, len);

    uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_OK);
}

int riotboot_serial_loader(void)
{
    volatile bool reading = true;

#ifdef BTN_BOOTLOADER_PIN
    gpio_init(BTN_BOOTLOADER_PIN, BTN_BOOTLOADER_MODE);
#endif
#ifdef LED_BOOTLOADER_PIN
    gpio_init(LED_BOOTLOADER_PIN, GPIO_OUT);
    LED_BOOTLOADER_OFF;
#endif

    uart_init(RIOTBOOT_UART_DEV, RIOTBOOT_UART_BAUDRATE,
              _uart_rx_cmd, (void *)&reading);

    /* give the user some time to interrupt auto boot */
    if (_bootdelay(RIOTBOOT_DELAY, &reading)) {
        return -1;
    }

#ifdef LED_BOOTLOADER_ON
    LED_BOOTLOADER_ON;
#endif

    while (1) {

        /* we can't use mutex in riotboot */
        while (reading) {}

        switch (ctx.rx.val.type) {
        case RIOTBOOT_CMD_BOOT:
            switch (ctx.rx.val.len) {
            case '\n':
                uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_OK);
                return -1;
#ifdef MODULE_RIOTBOOT_SLOT
            case '0':
                uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_OK);
                return 0;
            case '1':
                uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_OK);
                return 1;
#endif
            default:
                uart_write_byte(RIOTBOOT_UART_DEV, RIOTBOOT_STAT_ILLEGAL);
                break;
            }
            break;
        case RIOTBOOT_CMD_GET_PAGE:
            _get_page(unaligned_get_u32(ctx.rx.val.data));
            break;
        case RIOTBOOT_CMD_ERASE:
            _erase(unaligned_get_u16(ctx.rx.val.data));
            break;
        case RIOTBOOT_CMD_WRITE:
            _write(unaligned_get_u32(ctx.rx.val.data),  /* address */
                   ctx.rx.val.len - sizeof(uint32_t),   /* sizeof(data) - sizeof(address) */
                   &ctx.rx.val.data[4]);                /* data */
            break;
        }

        reading = true;
    }

    return -1;
}
