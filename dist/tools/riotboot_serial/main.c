/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>

#include <unistd.h>

#include "riotboot/serial.h"

extern int serial_open(int argc, char **argv);

typedef int (*line_cb_t)(void *ctx, uint32_t addr, uint8_t len, const char *s);

enum {
    IHEX_DATA,
    IHEX_EOF,
    IHEX_EXT_SEG_ADDR,
    IHEX_START_ADDR,
};

static uint8_t crc8(const uint8_t *data, size_t len,
                    uint8_t g_polynom, uint8_t crc)
{
    /* iterate over all bytes */
    for (size_t i=0; i < len; i++)
    {
        crc ^= data[i];

        for (int i = 0; i < 8; i++)
        {
            bool xor = crc & 0x80;
            crc = crc << 1;
            crc = xor ? crc ^ g_polynom : crc;
        }
    }

    return crc;
}

static char _get_char(int fd)
{
    char in;
    if (read(fd, &in, 1) == 1) {
        return in;
    }

    return 0;
}

static int _put_char(int fd, char c)
{
    return write(fd, &c, 1);
}

static int _write_crc(int fd, const void *buf, size_t count, uint8_t *crc)
{
    *crc = crc8(buf, count, RIOTBOOT_CRC8_POLY, *crc);
    return write(fd, buf, count);
}

static int _write_crc_byte(int fd, uint8_t byte, uint8_t *crc)
{
    return _write_crc(fd, &byte, 1, crc);
}

static int _get_result(int fd)
{
    while (1) {
        char c = _get_char(fd);

        switch (c) {
        case 0:
            continue;
        case RIOTBOOT_STAT_OK:
            printf("\r");
            return 0;
        case RIOTBOOT_STAT_BAD_CRC:
            puts("CRC Error");
            return 1;
        case RIOTBOOT_STAT_ILLEGAL:
            puts("Illegal Address");
            return -1;
        default:
            putchar(c);
        }
    };
}

static int cmd_erase(int fd, uint32_t sector)
{
    uint8_t crc = 0xFF;
    _write_crc_byte(fd, RIOTBOOT_CMD_ERASE, &crc);
    _write_crc_byte(fd, sizeof(sector), &crc);
    _write_crc(fd, &sector, sizeof(sector), &crc);
    write(fd, &crc, 1);

    return _get_result(fd);
}

__attribute__((unused))
static int cmd_write(int fd, uint32_t addr, const void *data, size_t len)
{
    uint8_t crc = 0xFF;
    _write_crc_byte(fd, RIOTBOOT_CMD_WRITE, &crc);
    _write_crc_byte(fd, sizeof(addr) + len, &crc);
    _write_crc(fd, &addr, sizeof(addr), &crc);
    _write_crc(fd, data, len, &crc);
    write(fd, &crc, 1);

    return _get_result(fd);
}

static int cmd_boot(int fd)
{
    _put_char(fd, RIOTBOOT_CMD_BOOT);
    _put_char(fd, '\n');
    return _get_result(fd);
}

static uint32_t _get_page(int fd, uint32_t addr)
{
    do {
        uint8_t crc = 0xFF;
        _write_crc_byte(fd, RIOTBOOT_CMD_GET_PAGE, &crc);
        _write_crc_byte(fd, sizeof(addr), &crc);
        _write_crc(fd, &addr, sizeof(addr), &crc);
        write(fd, &crc, 1);

    } while (_get_result(fd) > 1);

    uint32_t page = 0;
    read(fd, &page, sizeof(page));
    return page;
}

static uint32_t cmd_get_page(int fd, uint32_t addr)
{
    uint32_t page[2];

    do {
        page[0] = _get_page(fd, addr);
        page[1] = _get_page(fd, addr);
    } while (page[0] != page[1]);

    return page[0];
}

static uint8_t _int(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 0xA;
    }

    return 0;
}

static unsigned _hex_to_int(const char *s, size_t len)
{
    unsigned ret = 0;
    while (len--) {
        ret <<= 4;
        ret += _int(*s++);
    }

    return ret;
}

static int _write_out(void *ctx, uint32_t addr, uint8_t len, const char *s)
{
    int fd = *(int*)ctx;

    printf("0x%x: %u bytes ", addr, len);

    uint8_t crc = 0xFF;
    _write_crc_byte(fd, RIOTBOOT_CMD_WRITE, &crc);
    _write_crc_byte(fd, sizeof(addr) + len, &crc);
    _write_crc(fd, &addr, sizeof(addr), &crc);

    while (len--) {
        uint8_t byte = _hex_to_int(s, 2);
        _write_crc_byte(fd, byte, &crc);
        s += 2;
    }

    write(fd, &crc, 1);

    return _get_result(fd);
}

static void _parse_ihex(FILE *hex, line_cb_t cb, void *ctx)
{
    char line[48];
    char* pos;
    uint32_t offset = 0;
    unsigned line_count = 0;

    rewind(hex);
    while ((pos = fgets(line, sizeof(line), hex))) {

        ++line_count;
        if (*pos != ':') {
            printf("%u: skipping line", line_count);
            continue;
        }

        pos += 1;
        uint8_t len   = _hex_to_int(pos, 2);
        pos += 2;
        uint32_t addr = _hex_to_int(pos, 4) + offset;
        pos += 4;
        uint8_t type  = _hex_to_int(pos, 2);
        pos += 2;

        switch (type) {
        case IHEX_DATA:
            while (cb(ctx, addr, len, pos) > 0) {}
            break;
        case IHEX_EXT_SEG_ADDR:
            offset = _hex_to_int(pos, len) << 12;
            printf("%u: adding offset: %x\n", line_count, offset);
            break;
        case IHEX_EOF:
            return;
        case IHEX_START_ADDR:
            break;
        default:
            printf("%u: unknown record type %x\n", line_count, type);
        }
    }
}

static int _min_max(void *ctx, uint32_t addr, uint8_t len, const char *s)
{
    (void)s;

    uint32_t *min_max = ctx;

    if (addr < min_max[0]) {
        min_max[0] = addr;
    }

    if (addr + len > min_max[1]) {
        min_max[1] = addr + len;
    }

    return 0;
}

static void _connect_bootloader(int fd)
{
    const char spinner[] = "|/-\\";
    unsigned tries = 0;
    while (_get_char(fd) != RIOTBOOT_STAT_WAITING) {
        printf("connecting [%c]", spinner[tries % (sizeof(spinner) - 1)]);
        _put_char(fd, RIOTBOOT_PROBE);
        ++tries;

        if (tcdrain(fd)) {
            puts("\rSerial adapter disconnected");
            close(fd);
            exit(1);
        }

        printf("\r");
        fflush(stdout);
    }

    do {
        write(fd, "B\n", 2);
    } while (_get_char(fd) != RIOTBOOT_STAT_READY);

    puts("\rconnected.");
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <file> <tty> [baudrate]\n", argv[0]);
        return -1;
    }

    const char *file = argv[1];
    FILE *hex = fopen(file, "r");

    if (hex == NULL) {
        fprintf(stderr, "can't open '%s'\n", file);
        return -1;
    }

    /* parse hex file to get firmware address range */
    uint32_t min_max[2] = {UINT32_MAX, 0};
    _parse_ihex(hex, _min_max, min_max);

    printf("%u byte firmware (0x%x - 0x%x)\n",
           min_max[1] - min_max[0], min_max[0], min_max[1]);

    int fd = serial_open(argc - 2, &argv[2]);

    if (fd < 0) {
        fprintf(stderr, "can't open serial interface\n");
        goto error;
    }

    /* sync with the boorloader */
    _connect_bootloader(fd);

    /* find out how many pages we have to erase */
    uint32_t first_page = cmd_get_page(fd, min_max[0]);
    uint32_t last_page  = cmd_get_page(fd, min_max[1]);

    printf("erasing pages %u to %u\n", first_page, last_page);

    for (unsigned i = first_page; i <= last_page; ++i) {
        printf("erase %u ", i);
        cmd_erase(fd, i);
    }

    /* flash the firmware */
    _parse_ihex(hex, _write_out, &fd);

    cmd_boot(fd);

    close(fd);

error:
    fclose(hex);

    return 0;
}
