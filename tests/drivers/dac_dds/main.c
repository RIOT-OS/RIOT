/*
 * SPDX-FileCopyrightText: 2020 Beuth Hochschule für Technik Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       DAC (audio) test application
 *
 *              Generates Sine, Square and Sawtooth waves using
 *              a DAC.
 *              Connect a speaker or headphones to the DAC output
 *              pins of your board, you should be able to hear the
 *              generated sounds.
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "dac_dds.h"
#include "dac_dds_params.h"
#include "imath.h"
#include "shell.h"

#include "blob/hello.raw.h"

#ifndef DAC_DDS_CHAN
#define DAC_DDS_CHAN    0
#endif

#ifndef DAC_DDS_DAC
#define DAC_DDS_DAC    DAC_DDS_PARAM_DAC
#endif

#ifndef ENABLE_GREETING
#define ENABLE_GREETING 1
#endif

#ifndef min
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef DAC_BUF_SIZE
#define DAC_BUF_SIZE    (2048)
#endif

static bool res_16b = 0;
static unsigned sample_rate = 8000;

/* simple function to fill buffer with samples
 *
 * It is up to the caller to ensure that len is always at least period.
 * */
typedef void (*sample_gen_t)(uint8_t *dst, size_t len, uint16_t period);

static void _fill_saw_samples_8(uint8_t *buf, size_t len, uint16_t period)
{
    for (uint16_t i = 0; i < len; ++i) {
        buf[i] = (i * 0xFFUL) / period;
    }
}

static void _fill_saw_samples_16(uint8_t *buf, size_t len, uint16_t period)
{
    for (uint16_t i = 0; i < len; ++i) {
        uint16_t y = (i * 0xFFFFUL) / period;
        buf[i]   = y;
        buf[++i] = y >> 8;
    }
}

static void _fill_sine_samples_8(uint8_t *buf, size_t len, uint16_t period)
{
    uint16_t x = 0;
    unsigned step = SINI_PERIOD / period;

    for (uint16_t i = 0; i < period; ++i) {
        x += step;
        buf[i] = (fast_sini(x) - SINI_MIN) >> 6;
    }

    for (uint16_t i = period; i < len; i += period) {
        memcpy(&buf[i], &buf[0], period);
    }
}

static void _fill_sine_samples_16(uint8_t *buf, size_t len, uint16_t period)
{
    uint16_t x = 0;
    unsigned step = SINI_PERIOD / period;

    period *= 2;

    for (uint16_t i = 0; i < period; ++i) {
        x += step;

        uint16_t y = (fast_sini(x) - SINI_MIN) << 2;
        buf[i]   = y;
        buf[++i] = y >> 8;
    }

    for (uint16_t i = period; i < len; i += period) {
        memcpy(&buf[i], &buf[0], period);
    }
}

static void _fill_square_samples(uint8_t *buf, size_t len, uint16_t period)
{
    period /= 2;

    if (res_16b) {
        period *= 2;
    }

    for (uint8_t *end = buf + len; buf < end; ) {
        memset(buf, 0xFF, period);
        buf += period;
        memset(buf, 0x0, period);
        buf += period;
    }
}

static void _unlock(void *arg)
{
    mutex_unlock(arg);
}

static void play_function(uint16_t period, uint32_t samples, sample_gen_t fun)
{
    static uint8_t buf[DAC_BUF_SIZE];
    mutex_t lock = MUTEX_INIT_LOCKED;

    if (period > DAC_BUF_SIZE) {
        printf("Period duration exceeds sample buffer size.\n");
        return;
    }

    /* only work with whole wave periods */
    uint16_t len_aligned = DAC_BUF_SIZE - DAC_BUF_SIZE % period;

    /* One underrun indication is expected (for the first sample) */
    int underruns = -1;

    /* 16 bit samples doubles data rate */
    if (res_16b) {
        samples *= 2;
    }

    /* pre-calculate buffer */
    fun(buf, len_aligned, period);

    /* we want to block until the next buffer can be queued */
    dac_dds_set_cb(DAC_DDS_CHAN, _unlock, &lock);

    while (samples) {
        size_t len = min(samples, len_aligned);
        samples -= len;

        underruns += !dac_dds_play(DAC_DDS_CHAN, buf, len);

        /* wait for buffer flip */
        mutex_lock(&lock);
    }

    if (underruns != 0) {
        printf("During playback, %d underruns occurred.\n", underruns);
    }
}

#if IS_USED(ENABLE_GREETING)
static int cmd_greeting(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    if (sample_rate != 8000 || res_16b) {
        puts("Warning: audio clip was recoded with 8bit/8000 Hz");
    }

    puts("Play Greeting…");

    /* we only want to play a single sample */
    dac_dds_set_cb(DAC_DDS_CHAN, NULL, NULL);

    dac_dds_play(DAC_DDS_CHAN, hello_raw, hello_raw_len);

    return 0;
}
#endif

static void _dac_init(void)
{
    printf("init DAC DDS with %d bit, %u Hz\n", res_16b ? 16 : 8, sample_rate);
    dac_dds_init(DAC_DDS_CHAN, sample_rate,
                 res_16b ? DAC_FLAG_16BIT : DAC_FLAG_8BIT, NULL, NULL);
}

static int cmd_init(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <freq> <bit>\n", argv[0]);
        return 1;
    }

    if (argc > 2) {
        unsigned bit = atoi(argv[2]);

        if (bit != 8 && bit != 16) {
            printf("Only 8 and 16 bit samples supported.\n");
            return 1;
        }

        res_16b = bit == 16;
    }

    sample_rate = atoi(argv[1]);

    _dac_init();

    return 0;
}

static int cmd_saw(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <freq> <secs>\n", argv[0]);
        return 1;
    }

    unsigned freq = atoi(argv[1]);
    unsigned secs = atoi(argv[2]);

    play_function((sample_rate + freq/2) / freq, secs * sample_rate,
                  res_16b ? _fill_saw_samples_16 : _fill_saw_samples_8);

    return 0;
}

static int cmd_sine(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <freq> <secs>\n", argv[0]);
        return 1;
    }

    unsigned freq = atoi(argv[1]);
    unsigned secs = atoi(argv[2]);

    play_function((sample_rate + freq/2) / freq, secs * sample_rate,
                  res_16b ? _fill_sine_samples_16 : _fill_sine_samples_8);

    return 0;
}

static int cmd_square(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <freq> <secs>\n", argv[0]);
        return 1;
    }

    unsigned freq = atoi(argv[1]);
    unsigned secs = atoi(argv[2]);

    play_function((sample_rate + freq/2) / freq, secs * sample_rate,
                  _fill_square_samples);

    return 0;
}

static const shell_command_t shell_commands[] = {
#if IS_USED(ENABLE_GREETING)
    { "hello", "Play Greeting", cmd_greeting },
#endif
    { "init",   "Initialize DAC", cmd_init },
    { "saw",    "Play sawtooth wave", cmd_saw },
    { "sine",   "Play Sine wave", cmd_sine },
    { "square", "Play Square wave", cmd_square },
    { NULL, NULL, NULL }
};

int main(void)
{
    dac_init(DAC_DDS_DAC);
    /* Initialize to the idle level of 16bit audio */
    dac_set(DAC_DDS_DAC, 1 << 15);
    _dac_init();

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
