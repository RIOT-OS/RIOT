/*
 * SPDX-FileCopyrightText: 2016 Brandon Lin
 * SPDX-FileCopyrightText: 2016-2018 Bas Stottelaar
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 * @file
 *
 * @brief   Fortuna CSPRNG implementation.
 *
 * @author  Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <string.h>
#include "fortuna.h"
#include "kernel_defines.h"
#if FORTUNA_RESEED_INTERVAL_MS > 0 && IS_USED(MODULE_FORTUNA_RESEED)
#include "atomic_utils.h"
#if IS_USED(MODULE_ZTIMER_MSEC)
#include "ztimer.h"
#else
#include "xtimer.h"
#endif
#endif

/**
 * @brief Helper to increment the 128-bit counter (see section 9.4).
 */
static inline void fortuna_increment_counter(fortuna_state_t *state)
{
    state->gen.counter.split.l++;

    /* on overflow of low, increment high */
    if (state->gen.counter.split.l == 0) {
        state->gen.counter.split.h++;
    }
}

/*
 * Corresponds to section 9.4.2 (step 1/3).
 */
static void fortuna_reseed_init(fortuna_state_t *state)
{
    sha256_init(&state->scratchpad.sha256);
    sha256_update(&state->scratchpad.sha256, state->gen.key, 32);
}

/*
 * Corresponds to section 9.4.2 (step 2/3).
 */
static void fortuna_reseed_update(fortuna_state_t *state, const uint8_t *seed,
                                  size_t length)
{
    sha256_update(&state->scratchpad.sha256, seed, length);
}

/*
 * Corresponds to section 9.4.2 (step 3/3).
 */
static void fortuna_reseed_finish(fortuna_state_t *state)
{
    sha256_final(&state->scratchpad.sha256, state->gen.key);
    fortuna_increment_counter(state);

#if FORTUNA_CLEANUP
    memset(&state->scratchpad, 0, sizeof(state->scratchpad));
#endif
}

/*
 * Corresponds to section 9.4.3.
 * Returns 0 on success; A negative value otherwise
 */
static int fortuna_generate_blocks(fortuna_state_t *state, uint8_t *out,
                                   size_t blocks)
{
    /* check if generator has been seeded */
    if (state->gen.counter.split.l == 0 && state->gen.counter.split.h == 0) {
        return -1;
    }

    /* initialize cipher based on state */
    int res = aes_init(&state->scratchpad.cipher, state->gen.key,
                       FORTUNA_AES_KEY_SIZE);

    if (res != CIPHER_INIT_SUCCESS) {
#if FORTUNA_CLEANUP
        memset(&state->scratchpad, 0, sizeof(state->scratchpad));
#endif
        return -2;
    }

    for (size_t i = 0; i < blocks; i++) {
        res = aes_encrypt(&state->scratchpad.cipher, state->gen.counter.bytes,
                          out + (i * 16));
        if (res != 1) {
            return -3;
        }
        fortuna_increment_counter(state);
    }

#if FORTUNA_CLEANUP
    memset(&state->scratchpad, 0, sizeof(state->scratchpad));
#endif

    return 0;
}

/*
 * Corresponds to section 9.4.4.
 */
static int fortuna_pseudo_random_data(fortuna_state_t *state, uint8_t *out,
                                      size_t bytes)
{
    uint8_t buf[16];

#if FORTUNA_RESEED_LIMIT
    /* maximum number of bytes per read is FORTUNA_RESEED_LIMIT */
    if (bytes > FORTUNA_RESEED_LIMIT) {
        return -1;
    }
#endif

    /* check if generator has been seeded */
    if (state->gen.counter.split.l == 0 && state->gen.counter.split.h == 0) {
        return -2;
    }

    /* generate blocks per 16 bytes */
    size_t blocks = bytes / 16;

    if (fortuna_generate_blocks(state, out, blocks)) {
#if FORTUNA_CLEANUP
        memset(buf, 0, sizeof(buf));
#endif
        return -3;
    }

    /* generate one block for the remaining bytes */
    size_t remaining = bytes % 16;

    if (remaining) {
        if (fortuna_generate_blocks(state, buf, 1)) {
#if FORTUNA_CLEANUP
            memset(buf, 0, sizeof(buf));
#endif
            return -3;
        }

        memcpy(out + (blocks * 16), buf, remaining);
    }

    /* switch to a new key to avoid later compromises of this output */
    if (fortuna_generate_blocks(state, state->gen.key, 2)) {
#if FORTUNA_CLEANUP
        memset(buf, 0, sizeof(buf));
#endif
        return -3;
    }

#if FORTUNA_CLEANUP
    memset(buf, 0, sizeof(buf));
#endif

    return 0;
}

#if FORTUNA_RESEED_INTERVAL_MS > 0 && IS_USED(MODULE_FORTUNA_RESEED)
void _reseed_callback(void *arg)
{
    fortuna_state_t *state = (fortuna_state_t *) arg;
    atomic_store_u8(&state->needs_reseed, 1);
}

static void _reseed_timer_set(fortuna_state_t *state)
{
    atomic_store_u8(&state->needs_reseed, 0);
#if IS_USED(MODULE_ZTIMER_MSEC)
    ztimer_set(ZTIMER_MSEC, &state->reseed_timer, FORTUNA_RESEED_INTERVAL_MS);
#else
    xtimer_set(&state->reseed_timer, FORTUNA_RESEED_INTERVAL_MS * US_PER_MS);
#endif
}

static void _reseed_timer_init(fortuna_state_t *state) {
    /* initialize reseed timer */
    state->reseed_timer.callback = _reseed_callback;
    state->reseed_timer.arg = state;
    _reseed_timer_set(state);
}
#endif

/*
 * Corresponds to section 9.4.1 and 9.5.4.
 */
int fortuna_init(fortuna_state_t *state)
{
    /* set everything to zero, then initialize the pools */
    memset(state, 0, sizeof(fortuna_state_t));

    for (int i = 0; i < (int) FORTUNA_POOLS; i++) {
        sha256_init(&state->pools[i].ctx);
    }

#if FORTUNA_RESEED_INTERVAL_MS > 0 && IS_USED(MODULE_FORTUNA_RESEED)
    /* reseed time init if required */
    _reseed_timer_init(state);
#endif

#if FORTUNA_LOCK
    /* initialize the lock */
    mutex_init(&state->lock);
#endif

    return 0;
}

/*
 * Corresponds to section 9.5.5.
 */
int fortuna_random_data(fortuna_state_t *state, uint8_t *out, size_t bytes)
{
    uint8_t buf[32];

#if FORTUNA_LOCK
    mutex_lock(&state->lock);
#endif

    /* reseed the generator if needed, before returning data */
#if FORTUNA_RESEED_INTERVAL_MS > 0 && IS_USED(MODULE_FORTUNA_RESEED)
    if (state->pools[0].len >= FORTUNA_MIN_POOL_SIZE &&
        atomic_load_u8(&state->needs_reseed)) {
#else
    if (state->pools[0].len >= FORTUNA_MIN_POOL_SIZE) {
#endif
        state->reseeds++;

        fortuna_reseed_init(state);

        for (int i = 0; i < (int) FORTUNA_POOLS; i++) {
            /* pool i is included if 2^i divides the reseed counter */
            if ((state->reseeds & (((uint32_t)1 << i) - 1)) == 0) {
                /* pools are written to via fortuna_add_random_event */
                sha256_final(&state->pools[i].ctx, buf);
                sha256_init(&state->pools[i].ctx);
                state->pools[i].len = 0;
                fortuna_reseed_update(state, buf, 32);
            }
        }

        fortuna_reseed_finish(state);

#if FORTUNA_RESEED_INTERVAL_MS > 0 && IS_USED(MODULE_FORTUNA_RESEED)
        _reseed_timer_set(state);
#endif

#if FORTUNA_CLEANUP
        memset(buf, 0, sizeof(buf));
#endif
    }

    /* read bytes from the generator */
    int res = fortuna_pseudo_random_data(state, out, bytes);

#if FORTUNA_LOCK
    mutex_unlock(&state->lock);
#endif

    return res;
}

/*
 * Corresponds to section 9.5.6.
 */
int fortuna_add_random_event(fortuna_state_t *state, const uint8_t *data,
                             uint8_t length, uint8_t source, uint8_t pool)
{
    if (length < 1 || length > 32) {
        return -1;
    }

    if (pool >= FORTUNA_POOLS) {
        return -2;
    }

#if FORTUNA_LOCK
    mutex_lock(&state->lock);
#endif

    uint8_t header[2];
    header[0] = source;
    header[1] = length;
    sha256_update(&state->pools[pool].ctx, header, 2);
    sha256_update(&state->pools[pool].ctx, (uint8_t *) data, length);
    state->pools[pool].len += length;

#if FORTUNA_LOCK
    mutex_unlock(&state->lock);
#endif

    return 0;
}

/*
 * Corresponds to section 9.6.1.
 */
int fortuna_write_seed(fortuna_state_t *state, fortuna_seed_t *out)
{
    return fortuna_random_data(state, (uint8_t *)out, FORTUNA_SEED_SIZE);
}

/*
 * Corresponds to section 9.6.2.
 */
int fortuna_update_seed(fortuna_state_t *state, fortuna_seed_t *inout)
{
#if FORTUNA_LOCK
    mutex_lock(&state->lock);
#endif

    /* reseed using the provided seed */
    fortuna_reseed_init(state);
    fortuna_reseed_update(state, (uint8_t *)inout, FORTUNA_SEED_SIZE);
    fortuna_reseed_finish(state);

#if FORTUNA_LOCK
    mutex_unlock(&state->lock);
#endif

    /* the seed file must be overwritten by a new seed file */
    return fortuna_random_data(state, (uint8_t *)inout, FORTUNA_SEED_SIZE);
}
