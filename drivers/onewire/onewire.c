/*
 * Copyright (C) 2016-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_onewire
 * @{
 *
 * @file
 * @brief       1-Wire bus driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "assert.h"
#include "xtimer.h"
#include "onewire.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/* bus timings, all in microseconds */
#define T_RESET_HOLD_US     (480)
#define T_RESET_SAMPLE_US   (70)

#define T_RW_PULSE_US       (3)
#define T_W_HOLD_US         (57)
#define T_W_RECOVER_US      (10)
#define T_R_SAMPLE_US       (7)
#define T_R_RECOVER_US      (55)

static inline int _read_bit(const onewire_t *owi)
{
    gpio_clear(owi->pin);
    xtimer_usleep(T_RW_PULSE_US);
    gpio_init(owi->pin, owi->imode);
    xtimer_usleep(T_R_SAMPLE_US);
    int in = (gpio_read(owi->pin)) ? 1 : 0;
    xtimer_usleep(T_R_RECOVER_US);
    gpio_init(owi->pin, owi->omode);
    gpio_set(owi->pin);

    return in;
}

static inline void _write_bit(const onewire_t *owi, int out)
{
    gpio_clear(owi->pin);
    if (out) {
        /* shift out a `1` */
        xtimer_usleep(T_RW_PULSE_US);
        gpio_set(owi->pin);
        xtimer_usleep(T_W_HOLD_US);
    }
    else {
        /* shift out a `0` */
        xtimer_usleep(T_RW_PULSE_US + T_W_HOLD_US);
        gpio_set(owi->pin);
    }
    xtimer_usleep(T_W_RECOVER_US);
}

int onewire_init(onewire_t *owi, const onewire_params_t *params)
{
    owi->pin = params->pin;

    owi->omode = params->pin_mode;
    owi->imode = (owi->omode == GPIO_OD_PU) ? GPIO_IN_PU : GPIO_IN;

    if (gpio_init(owi->pin, owi->omode) != 0) {
        return ONEWIRE_ERR_PINCFG;
    }
    gpio_set(owi->pin);
    return ONEWIRE_OK;
}

int onewire_reset(const onewire_t *owi, const onewire_rom_t *rom)
{
    int res = ONEWIRE_OK;

    gpio_clear(owi->pin);
    xtimer_usleep(T_RESET_HOLD_US);
    gpio_set(owi->pin);

    xtimer_usleep(T_RESET_SAMPLE_US);
    gpio_init(owi->pin, owi->imode);
    if (gpio_read(owi->pin)) {
        res = ONEWIRE_NODEV;
    }

    xtimer_usleep(T_RESET_HOLD_US - T_RESET_SAMPLE_US);
    gpio_init(owi->pin, owi->omode);
    gpio_set(owi->pin);

    if ((res == ONEWIRE_OK) && (rom != NULL)) {
        onewire_write_byte(owi, ONEWIRE_ROM_MATCH);
        onewire_write(owi, rom->u8, sizeof(onewire_rom_t));
    }

    return res;
}

void onewire_read(const onewire_t *owi, void *data, size_t len)
{
    uint8_t *buf = (uint8_t *)data;

    for (size_t pos = 0; pos < (size_t)len; pos++) {
        /* read the next byte from the bus */
        uint8_t tmp = 0;
        for (int i = 0; i < 8; i++) {
            tmp |= (_read_bit(owi) << i);
        }
        buf[pos] = tmp;
    }
}

void onewire_write(const onewire_t *owi, const void *data, size_t len)
{
    const uint8_t *buf = (uint8_t *)data;

    for (size_t pos = 0; pos < (size_t)len; pos ++) {

        for (int i = 0; i < 8; i++) {
            _write_bit(owi, (buf[pos] & (1 << i)));
        }
    }
}

int onewire_search(const onewire_t *owi, onewire_rom_t *rom, int ld)
{
    /* initialize the search state */
    if (ld == ONEWIRE_SEARCH_FIRST) {
        memset(rom, 0, sizeof(onewire_rom_t));
    }

    /* send reset condition and issue search ROM command */
    if (onewire_reset(owi, NULL) != ONEWIRE_OK) {
        return ONEWIRE_NODEV;
    }
    onewire_write_byte(owi, ONEWIRE_ROM_SEARCH);

    /* start search. Refer to e.g. to
     * https://pdfserv.maximintegrated.com/en/an/AN937.pdf, page 51 and 52 for
     * a detailed description on the search algorithm */
    int marker = 0;
    int pos = 1;
    for (unsigned b = 0; b < sizeof(onewire_rom_t); b++) {
        for (int i = 0; i < 8; i++) {
            int bit1 = _read_bit(owi);
            int bit2 = _read_bit(owi);

            if (bit1 == bit2) {
                if (pos < ld) {
                    bit1 = (rom->u8[b] & (1 << i)) ? 1 : 0;
                    if (bit1 == 0) {
                        marker = pos;
                    }
                }
                else if (pos == ld) {
                    bit1 = 1;
                }
                else {
                    marker = pos;
                    bit1 = 0;
                }
            }

            rom->u8[b] &= ~(1 << i);
            rom->u8[b] |= (bit1 << i);
            _write_bit(owi, bit1);
            pos++;
        }
    }

    return marker;
}
