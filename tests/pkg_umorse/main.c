/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Check if the uMorse package builds.
 *
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "led.h"
#include "periph_conf.h"
#include "xtimer.h"
#define UMORSE_MSLEEP(m)    xtimer_msleep(m)

#include "umorse.h"
#include "print.h"

#define CODE_LEN	(64U)

void blink_dit(void *args, uint8_t flags)
{
    (void) args;
    (void) flags;

    LED0_ON;
    UMORSE_MSLEEP(UMORSE_DELAY_DIT);
    LED0_OFF;
}

void blink_dah(void *args, uint8_t flags)
{
    (void) args;
    (void) flags;

    LED0_ON;
    UMORSE_MSLEEP(UMORSE_DELAY_DAH);
    LED0_OFF;
}

void blink_nil(void *args, uint8_t flags)
{
    (void) args;

    uint8_t cnt = flags & UMORSE_MASK_COUNT;
    UMORSE_MSLEEP(cnt * UMORSE_DELAY_DIT);
}

static const umorse_out_t blink = {
    .dit = blink_dit,
    .dah = blink_dah,
    .nil = blink_nil,
    .params = NULL
};

static const umorse_out_t print = {
    .dit = umorse_print_dit,
    .dah = umorse_print_dah,
    .nil = umorse_print_nil,
    .params = NULL
};

static const char text[] = "Hello RIOT-OS!\nThis is uMorse.";

int test_umorse_out(void)
{
    uint8_t code[CODE_LEN];

    printf("> Translate text of length %u into Morse code:\n\"%s\"\n",
           (unsigned)strlen(text), text);

    memset(code, 0, CODE_LEN);
    printf("> using aligned encoding:\n");
    int ret = umorse_encode_aligned(text, sizeof(text), code, sizeof(code));
    if (ret < 0) {
        return 1;
    }
    umorse_output(&print, code, ret, 0x0);
    printf("> encoded length=%d\n", ret);

    memset(code, 0, CODE_LEN);
    printf("> using compact encoding:\n");
    ret = umorse_encode_compact(text, sizeof(text), code, sizeof(code));
    if (ret < 0) {
        return 2;
    }
    umorse_output(&print, code, ret, UMORSE_FLAG_NODELAY);
    umorse_output(&blink, code, ret, 0x0);
    printf("> encoded length=%d\n", ret);

    return 0;
}

int main(void)
{
    LED0_OFF;

    puts("[START]");
    int ret = test_umorse_out();
    if (ret) {
        puts("[FAILURE]");
        return 1;
    }
    puts("[SUCCESS]");
    return 0;
}
