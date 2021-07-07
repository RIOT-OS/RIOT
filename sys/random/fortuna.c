/**
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_random
 * @{
 * @file
 *
 * @brief   Glue-code for Fortuna PRNG.
 *
 * @author  Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <string.h>

#include "log.h"
#include "mutex.h"
#include "fortuna/fortuna.h"

/**
 * @brief This holds the PRNG state.
 */
static fortuna_state_t fortuna_state;

/**
 * @brief Initialize the PRNG with a given number of bytes.
 */
static void _init(uint8_t *in, size_t bytes)
{
    fortuna_seed_t seed;

    /* ensure a seed of proper length is available, which may not be the case
       with a small input */
    memset(seed, 0, sizeof(seed));

    for (int i = 0; i < (int) bytes; i++) {
        seed[i % sizeof(seed)] ^= in[i];
    }

    /* initialize the PRNG state */
    fortuna_init(&fortuna_state);

    /* update the PRNG seed (seed will be overwritten by design!) */
    fortuna_update_seed(&fortuna_state, &seed);
}

/**
 * @brief Wrapper for fortuna_random_data that supports reading more than
 *        FORTUNA_RESEED_LIMIT and also checks the result.
 */
static void _read(uint8_t *out, size_t bytes)
{
    do {
        /* read at most chunk bytes to not exhaust the state at once */
        size_t chunk = (bytes < FORTUNA_RESEED_LIMIT) ?
                       bytes : FORTUNA_RESEED_LIMIT;

        int res = fortuna_random_data(&fortuna_state, out, chunk);

        if (res == -1) {
            LOG_ERROR("random: reading more bytes than allowed.\n");
        }
        else if (res == -2) {
            LOG_ERROR("random: PRNG not initialized.\n");
        }
        else if (res == -3) {
            LOG_ERROR("random: unknown error.\n");
        }

        /* advance bytes and buffer */
        bytes -= chunk;
        out += chunk;
    } while (bytes > FORTUNA_RESEED_LIMIT);
}

void random_init_by_array(uint32_t init_key[], int key_length)
{
    _init((uint8_t *) init_key, sizeof(uint32_t) * key_length);
}

void random_init(uint32_t s)
{
    _init((uint8_t *) &s, sizeof(s));
}

uint32_t random_uint32(void)
{
    uint32_t data;

    _read((uint8_t *) &data, sizeof(data));

    return data;
}
