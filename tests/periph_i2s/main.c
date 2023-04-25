/*
 * Copyright (C) 2023 Freie Universität Berlin
 *                    Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the low-level I2S peripheral driver
 *
 * Not to be confused with the I2C peripheral test application
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "bitarithm.h"
#include "macros/utils.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/i2s.h"
#include "shell.h"

#include <ztimer.h>

#define TRANSACTIONS_NUM        4
#define TRANSACTION_MAX_SIZE    128

#define INVALID_ARGS    puts("Error: Invalid number of arguments");

#define BUFSIZE         (256U)

#define CONVERT_ERROR   (-32768)

#define ARG_ERROR       (-1)

/* 1 kHz stereo sine wave at 48 kHz sample rate */
static const uint32_t sine[] = {
    0x00000000, 0x00000000, 0x10b5150f, 0x10b5150f, 0x2120fb82, 0x2120fb82, 0x30fbc54c, 0x30fbc54c,
    0x3fffffff, 0x3fffffff, 0x4debe4fd, 0x4debe4fd, 0x5a827999, 0x5a827999, 0x658c9a2c, 0x658c9a2c,
    0x6ed9eba0, 0x6ed9eba0, 0x7641af3b, 0x7641af3b, 0x7ba3751c, 0x7ba3751c, 0x7ee7aa4a, 0x7ee7aa4a,
    0x7fffffff, 0x7fffffff, 0x7ee7aa4a, 0x7ee7aa4a, 0x7ba3751c, 0x7ba3751c, 0x7641af3b, 0x7641af3b,
    0x6ed9eba0, 0x6ed9eba0, 0x658c9a2c, 0x658c9a2c, 0x5a827999, 0x5a827999, 0x4debe4fd, 0x4debe4fd,
    0x3fffffff, 0x3fffffff, 0x30fbc54c, 0x30fbc54c, 0x2120fb82, 0x2120fb82, 0x10b5150f, 0x10b5150f,
    0x00000000, 0x00000000, 0xef4aeaf1, 0xef4aeaf1, 0xdedf047e, 0xdedf047e, 0xcf043ab4, 0xcf043ab4,
    0xc0000001, 0xc0000001, 0xb2141b03, 0xb2141b03, 0xa57d8667, 0xa57d8667, 0x9a7365d4, 0x9a7365d4,
    0x91261460, 0x91261460, 0x89be50c5, 0x89be50c5, 0x845c8ae4, 0x845c8ae4, 0x811855b6, 0x811855b6,
    0x80000001, 0x80000001, 0x811855b6, 0x811855b6, 0x845c8ae4, 0x845c8ae4, 0x89be50c5, 0x89be50c5,
    0x91261460, 0x91261460, 0x9a7365d4, 0x9a7365d4, 0xa57d8667, 0xa57d8667, 0xb2141b03, 0xb2141b03,
    0xc0000001, 0xc0000001, 0xcf043ab4, 0xcf043ab4, 0xdedf047e, 0xdedf047e, 0xef4aeaf1, 0xef4aeaf1,
};

static i2s_transaction_t transactions[TRANSACTIONS_NUM];
static i2s_buf_t transaction_buffer[TRANSACTIONS_NUM][TRANSACTION_MAX_SIZE];
static size_t _pos;

static size_t _write_transaction_buffer_u32(i2s_t dev, i2s_transaction_t *transaction, size_t sine_offset)
{
    size_t transaction_offset = 0;
    size_t bytes_written = 0;

    uint32_t samples[8];

    do {
        size_t to_copy = MIN(ARRAY_SIZE(samples), ARRAY_SIZE(sine) - sine_offset);
        for (size_t i = 0; i < to_copy; i++) {
            samples[i] = sine[i+sine_offset];
        }
        bytes_written = i2s_write_transaction_buffer(dev, samples, sizeof(samples), transaction, transaction_offset);
        //printf("C %u B, so: %u, to: %u, bw: %u\n", (unsigned)to_copy, (unsigned)sine_offset, (unsigned)transaction_offset, (unsigned)bytes_written);
        transaction_offset += bytes_written;
        sine_offset += bytes_written/sizeof(uint32_t);
        if (sine_offset >= ARRAY_SIZE(sine)) {
            //printf("Truncating: %u\n", (unsigned)sine_offset);
            sine_offset -= ARRAY_SIZE(sine);
        }
    } while (bytes_written != 0);

    return sine_offset;
}

static size_t _write_transaction_buffer_u24(i2s_t dev, i2s_transaction_t *transaction, size_t sine_offset)
{
    size_t transaction_offset = 0;
    size_t bytes_written = 0;

    uint32_t samples[8];

    do {
        size_t to_copy = MIN(ARRAY_SIZE(samples), ARRAY_SIZE(sine) - sine_offset);
        for (size_t i = 0; i < to_copy; i++) {
            /* Shift and sine extend */
            samples[i] = sine[i+sine_offset] & 0xFFFFFF00UL;
        }
        bytes_written = i2s_write_transaction_buffer(dev, samples, sizeof(samples), transaction, transaction_offset);
        transaction_offset += bytes_written;
        sine_offset += bytes_written/sizeof(uint32_t);
        if (sine_offset >= ARRAY_SIZE(sine)) {
            sine_offset -= ARRAY_SIZE(sine);
        }
    } while (bytes_written != 0);

    return sine_offset;
}

static size_t _write_transaction_buffer_u16(i2s_t dev, i2s_transaction_t *transaction, size_t sine_offset)
{
    size_t transaction_offset = 0;
    size_t bytes_written = 0;

    uint16_t samples[16];

    do {
        size_t to_copy = MIN(ARRAY_SIZE(samples), ARRAY_SIZE(sine) - sine_offset);
        for (size_t i = 0; i < to_copy; i++) {
            /* Shift and sine extend */
            samples[i] = sine[i+sine_offset] >> 16;
        }
        bytes_written = i2s_write_transaction_buffer(dev, samples, sizeof(samples), transaction, transaction_offset);
        transaction_offset += bytes_written;
        sine_offset += bytes_written/sizeof(uint16_t);
        if (sine_offset >= ARRAY_SIZE(sine)) {
            sine_offset -= ARRAY_SIZE(sine);
        }
    } while (bytes_written != 0);
    return sine_offset;
}

static size_t _write_transaction_buffer(i2s_t dev, i2s_transaction_t *transaction, size_t sine_offset)
{
    uint8_t word_size = i2s_get_word_size(I2S_DEV(dev));
    switch (word_size) {
        case 32:
            sine_offset = _write_transaction_buffer_u32(dev, transaction, sine_offset);
            break;
        case 24:
            sine_offset = _write_transaction_buffer_u24(dev, transaction, sine_offset);
            break;
        case 16:
            sine_offset = _write_transaction_buffer_u16(dev, transaction, sine_offset);
            break;
    }
    return sine_offset;
}

static void _transaction_cb(i2s_t dev, i2s_transaction_t *transaction, void *arg)
{
    (void)arg;
    /* Clear the buffer to spot potential reuse issues */
    memset(transaction->buf, 0, TRANSACTION_MAX_SIZE);
    _pos = _write_transaction_buffer(dev, transaction, _pos);
    assert(_pos < ARRAY_SIZE(sine));
    i2s_add_transaction(dev, transaction);
}
static void _event_cb(i2s_t dev, i2s_state_t state, i2s_event_t event, void *arg)
{
    (void)dev;
    (void)arg;
    (void)state;
    switch (event) {
        case I2S_EVENT_ERR_TX_UDR:
            printf("TX underrun\n");
            break;
        case I2S_EVENT_ERR_RX_OVR:
            printf("RX overrun\n");
            break;
        case I2S_EVENT_ERR_FRAME:
            printf("Prame error\n");
            break;
        case I2S_EVENT_STOPPED:
            printf("Stopped!\n");
            break;
        default:
            printf("something else\n");
    }
}



static int _get_num(const char *str)
{
    errno = 0;
    char *temp;
    long val = strtol(str, &temp, 0);

    if (temp == str || *temp != '\0' ||
        ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE)) {
        val = CONVERT_ERROR;
    }
    return (int)val;
}

static int _get_mode(const char *str)
{
    if (strcmp(str, "target") == 0) {
        return I2S_MODE_TARGET;
    }
    return I2S_MODE_CONTROLLER;
}

static int _get_dir(const char *str)
{
    if (strcmp(str, "rx") == 0) {
        return I2S_DIRECTION_RECEIVE;
    }
    else if (strcmp(str, "tx") == 0) {
        return I2S_DIRECTION_TRANSMIT;
    }
    else if (strcmp(str, "both") == 0) {
        return I2S_DIRECTION_BOTH;
    }
    return I2S_DIRECTION_NONE;
}

static const char *_dir2str(i2s_direction_t dir)
{
    switch (dir) {
        case I2S_DIRECTION_NONE:
            return "None";
        case I2S_DIRECTION_RECEIVE:
            return "Receive";
        case I2S_DIRECTION_TRANSMIT:
            return "Transmit";
        case I2S_DIRECTION_BOTH:
            return "Both";
    }
    return "None";
}

static int _check_param(int argc, char **argv, int c_min, int c_max, char *use)
{
    int dev;

    if (argc - 1 < c_min || argc - 1 > c_max) {
        printf("Usage: %s %s\n", argv[0], use);
        INVALID_ARGS;
        return ARG_ERROR;
    }

    dev = _get_num(argv[1]);
    if (dev < 0 || dev >= (int)I2S_NUMOF) {
        printf("Error: No device, only %d supported\n", (int)I2S_NUMOF);
        return ARG_ERROR;
    }
    return dev;
}

static int _print_i2s_error(int res)
{
    if (res == I2S_NODEV) {
        printf("Error: Device doesn't exist [%d]\n", res);
        return 1;
    }
    else if (res == I2S_NOCONF) {
        printf("Error: Device not configured [%d]\n", res);
        return 1;
    }
    else if (res == I2S_NOWIDTH) {
        printf("Error: word size not supported by device [%d]\n", res);
        return 1;
    }
    else if (res == I2S_NOCHANNEL) {
        printf("Error: Number of channels not supported by device [%d]\n", res);
        return 1;
    }
    else if (res == I2S_NODIR) {
        printf("Error: Data direction not supported by device [%d]\n", res);
        return 1;
    }
    else if (res == I2S_NODATA) {
        printf("Error: No data available before starting device [%d]\n", res);
        return 1;
    }
    printf("Error: Unknown error [%d]\n", res);
    return 1;
}

int cmd_i2s_init(int argc, char **argv)
{
    int dev = _check_param(argc, argv, 1, 1, "DEV");
    if (dev == ARG_ERROR) {
        return 1;
    }

    printf("Command: i2s_init(%i)\n", dev);
    int res = i2s_init(I2S_DEV(dev), _event_cb, _transaction_cb, NULL);

    if (res < 0) {
        return _print_i2s_error(res);
    }

    printf("Success: i2c_%i acquired\n", dev);
    return 0;
}

static int cmd_i2s_print_conf(int argc, char **argv)
{
    int dev = _check_param(argc, argv, 1, 1, "DEV");
    if (dev == ARG_ERROR) {
        return 1;
    }
    i2s_config_t conf;
    int res = i2s_get_configuration(I2S_DEV(dev), &conf);
    if (res < 0) {
        return _print_i2s_error(res);
    }

    printf("I2S_DEV(%d) configuration:\n", dev);
    printf("  mode: %s\n", conf.mode == I2S_MODE_CONTROLLER ? "controller" : "target");
    printf("  direction: %s\n", _dir2str(conf.direction));
    printf("  frame clock: %"PRIu32"\n", conf.frame_clock);
    printf("  bytes per transaction: %u\n", conf.transaction_bytes);
    printf("  word size: %u\n", conf.word_size);
    printf("  channels: %u\n", conf.channels);

    return 0;
}

int cmd_i2s_conf(int argc, char **argv)
{
    if (argc == 2) {
        return cmd_i2s_print_conf(argc, argv);
    }
    int dev = _check_param(argc, argv, 7, 7, "DEV MODE DIR CHANNELS CLOCK_HZ BUF_BYTES WORD_BITS");
    if (dev == ARG_ERROR) {
        return 1;
    }

    i2s_config_t conf = {
        .mode = _get_mode(argv[2]),
        .direction = _get_dir(argv[3]),
        .channels = _get_num(argv[4]),
        .frame_clock = _get_num(argv[5]),
        .transaction_bytes = _get_num(argv[6]),
        .word_size = _get_num(argv[7]),
    };

    int res = i2s_configure(I2S_DEV(dev), &conf);

    if (res < 0) {
        return _print_i2s_error(res);
    }

    return 0;
}

static int cmd_i2s_play(int argc, char **argv)
{
    int dev = _check_param(argc, argv, 1, 1, "DEV");
    if (dev == ARG_ERROR) {
        return 1;
    }

    size_t sine_offset = 0;
    for (size_t i = 0; i < TRANSACTIONS_NUM; i++) {
        transactions[i].buf = transaction_buffer[i];
        sine_offset = _write_transaction_buffer(I2S_DEV(dev), &transactions[i], sine_offset);

        i2s_add_transaction(I2S_DEV(dev), &transactions[i]);
    }

    _pos = sine_offset;

    int res = i2s_start(I2S_DEV(dev));

    if (res < 0) {
        return _print_i2s_error(res);
    }

    printf("I2S_DEV(%u): started playing\n", dev);
    return 0;
}

static int cmd_i2s_drain(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    int dev = _check_param(argc, argv, 1, 1, "DEV");
    if (dev == ARG_ERROR) {
        return 1;
    }

    int res = i2s_drain(I2S_DEV(dev));

    if (res < 0) {
        return _print_i2s_error(res);
    }

    return 0;
}

static int cmd_i2s_stop(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    int dev = _check_param(argc, argv, 1, 1, "DEV");
    if (dev == ARG_ERROR) {
        return 1;
    }

    int res = i2s_stop(I2S_DEV(dev));

    if (res < 0) {
        return _print_i2s_error(res);
    }

    return 0;
}

int cmd_i2s_get_devs(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    printf("Command: return I2S_NUMOF\n");
    printf("Success: Amount of i2s devices: [%d]\n", I2S_NUMOF);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "i2s_init", "Initialize an I2S peripheral", cmd_i2s_init },
    { "i2s_conf", "Configure an I2S peripheral", cmd_i2s_conf },
    { "i2s_play", "Play a waveform over an I2S peripheral", cmd_i2s_play },
    { "i2s_stop", "Stop an I2S peripheral", cmd_i2s_stop },
    { "i2s_drain", "Wait for an I2S peripheral to finish playing", cmd_i2s_drain },
    { "i2s_get_devs", "Get the amount of supported I2S peripherals", cmd_i2s_get_devs },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Start: Test for the low-level I2S driver");

    static_assert((TRANSACTION_MAX_SIZE % 8) == 0, "TRANSACTION_MAX_SIZE must be a multiple of 8");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
