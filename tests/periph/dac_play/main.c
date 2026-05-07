/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: MIT
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       DAC (audio) test application
 *
 *              Generates Sine, Square, Triangle and Sawtooth waves
 *              using a DAC.
 *              Connect a speaker or headphones to the DAC output
 *              pins of your board, you should be able to hear the
 *              generated sounds.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros/math.h"
#include "periph/dac.h"
#include "imath.h"
#include "shell.h"

#ifndef CONFIG_DAC_LINE
#define CONFIG_DAC_LINE 0
#endif

#ifndef CONIFG_DAC_BUF_SAMPLES
#define CONIFG_DAC_BUF_SAMPLES 2048
#endif

/* simple function to fill buffer the size of one period */
/* Sample period may get trimmed to align with wave period */
typedef uint16_t (*sample_gen_t)(uint16_t *dst, uint16_t len);

static uint16_t _fill_square_samples(uint16_t *dst, uint16_t len)
{
    uint8_t *buf = (void *)dst;

    memset(buf, 0x00, len);
    buf += len;
    memset(buf, 0xff, len);

    return len;
}

static uint16_t _fill_saw_samples(uint16_t *dst, uint16_t len)
{
    uint32_t delta = (0xffff << 8) / len;

    for (uint16_t i = 0; i < len; ++i) {
        dst[i] = (i * delta) >> 8;
    }

    return len;
}

static uint16_t _fill_triang_samples(uint16_t *dst, uint16_t len)
{
    uint32_t delta = (0xffff << 9) / len;

    for (uint16_t i = 0; i < len / 2; ++i) {
        dst[i] = (i * delta) >> 8;
    }
    dst += len / 2;
    for (uint16_t i = 0; i < len / 2; ++i) {
        dst[i] = 0xffff - ((i * delta) >> 8);
    }

    return len;
}

static uint16_t _fill_sine_samples(uint16_t *dst, uint16_t len)
{
    /* round up here to not exceed the supplied buffer */
    unsigned step = DIV_ROUND_UP(SINI_PERIOD, len);
    len = SINI_PERIOD / step;

    for (uint16_t i = 0; i < len; ++i) {
        dst[i] = fast_sini(i * step) - SINI_MIN;
        /* scale amplitude 12 -> 16 bit
           fast_sini() would clip since it goes from -0x1000 to 0x1000 */
        dst[i] = ((dst[i] << 3) * 64) / 65;
    }

    return len;
}

typedef struct {
    uint16_t buf[CONIFG_DAC_BUF_SAMPLES];
    uint32_t iterations;
    uint16_t num_samples;
} dac_play_ctx_t;

static void _play_cb(void *arg)
{
    dac_play_ctx_t *ctx = arg;

    if (--ctx->iterations) {
        dac_play(CONFIG_DAC_LINE, ctx->buf, ctx->num_samples, 0);
    } else {
        dac_play_teardown(CONFIG_DAC_LINE);
        /* restore idle level */
        dac_set(CONFIG_DAC_LINE, 1 << 15);
    }
}

static void play_function(uint32_t freq, uint32_t secs, sample_gen_t fun) {
    static dac_play_ctx_t ctx;

    uint32_t sample_rate = dac_get_freq();
    uint16_t len = sample_rate / freq;
    uint8_t flags = 0;

    if (len > ARRAY_SIZE(ctx.buf)) {
        printf("sample buffer (%u samples) too small for single period (%u samples)\n",
               ARRAY_SIZE(ctx.buf), len);
        return;
    }

    /* stop ongoing playback */
    if (ctx.iterations) {
        dac_play_teardown(CONFIG_DAC_LINE);
    }

    /* use secs == 0 to signal infinite playback */
    if (secs) {
        ctx.iterations = (secs * sample_rate) / len;

        dac_play_setup(CONFIG_DAC_LINE, _play_cb, &ctx);
    } else {
        ctx.iterations = UINT32_MAX;
        flags |= DAC_PLAY_LOOPED;

        dac_play_setup(CONFIG_DAC_LINE, NULL, NULL);
    }

    /* generate samples */
    ctx.num_samples = fun(ctx.buf, len);

    /* play the sample */
    dac_play(CONFIG_DAC_LINE, ctx.buf, ctx.num_samples, flags);
}

static int cmd_saw(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <freq> <secs>\n", argv[0]);
        return 1;
    }

    unsigned freq = atoi(argv[1]);
    unsigned secs = atoi(argv[2]);

    play_function(freq, secs, _fill_saw_samples);

    return 0;
}

static int cmd_triang(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <freq> <secs>\n", argv[0]);
        return 1;
    }

    unsigned freq = atoi(argv[1]);
    unsigned secs = atoi(argv[2]);

    play_function(freq, secs, _fill_triang_samples);

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

    play_function(freq, secs, _fill_sine_samples);

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

    play_function(freq, secs, _fill_square_samples);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "saw",    "Play Sawtooth wave /|", cmd_saw },
    { "triang", "Play Triangle wave /\\", cmd_triang },
    { "sine",   "Play Sine wave     ~", cmd_sine },
    { "square", "Play Square wave   _–", cmd_square },
    { NULL, NULL, NULL }
};

int main(void)
{
    dac_init(CONFIG_DAC_LINE);
    /* Initialize to the idle level of 16bit audio */
    dac_set(CONFIG_DAC_LINE, 1 << 15);

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
