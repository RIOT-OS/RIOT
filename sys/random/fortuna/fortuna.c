/**
 * @brief Fortuna PRNG implementation.
 *
 * The MIT License applies to this software. See the included LICENSE.txt file
 * for more information.
 */

#include "fortuna.h"

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
 * Corresponds to section 9.4.2.
 */
static void fortuna_reseed(fortuna_state_t *state, const uint8_t *seed,
                           size_t length)
{
    sha256_context_t ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, state->gen.key, 32);
    sha256_update(&ctx, seed, length);
    sha256_final(&ctx, state->gen.key);

    /* if the generator was unseeded, this will mark it as seeded */
    fortuna_increment_counter(state);

#if FORTUNA_CLEANUP
    memset(&ctx, 0, sizeof(ctx));
#endif
}

/*
 * Corresponds to section 9.4.3.
 */
static int fortuna_generate_blocks(fortuna_state_t *state, uint8_t *out,
                                   size_t blocks)
{
    cipher_context_t cipher;

    /* check if generator has been seeded */
    if (state->gen.counter.split.l == 0 && state->gen.counter.split.h == 0) {
        return -1;
    }

    /* initialize cipher based on state */
    int res = aes_init(&cipher, state->gen.key, FORTUNA_AES_KEY_SIZE);

    if (res != CIPHER_INIT_SUCCESS) {
#if FORTUNA_CLEANUP
        memset(&cipher, 0, sizeof(cipher));
#endif
        return -2;
    }

    for (size_t i = 0; i < blocks; i++) {
        aes_encrypt(&cipher, state->gen.counter.bytes, out + (i * 16));
        fortuna_increment_counter(state);
    }

#if FORTUNA_CLEANUP
    memset(&cipher, 0, sizeof(cipher));
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

#if FORTUNA_RESEED_INTERVAL
    /* set last reseed to ensure initial time diff is correct */
    state->last_reseed = xtimer_now_usec64();
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
    uint8_t buf[FORTUNA_POOLS * 32];

#if FORTUNA_LOCK
    mutex_lock(&state->lock);
#endif

    /* reseed the generator if needed, before returning data */
#if FORTUNA_RESEED_INTERVAL
    if (state->pools[0].len >= FORTUNA_MIN_POOL_SIZE &&
        (xtimer_now_usec64() - state->last_reseed) > FORTUNA_RESEED_INTERVAL) {
#else
    if (state->pools[0].len >= FORTUNA_MIN_POOL_SIZE) {
#endif
        state->reseeds++;
        size_t len = 0;

        for (int i = 0; i < (int) FORTUNA_POOLS; i++) {
            if (state->reseeds | (1 << i)) {
                sha256_final(&state->pools[i].ctx, &buf[len]);
                sha256_init(&state->pools[i].ctx);
                state->pools[i].len = 0;

                /* append length of SHA-256 hash */
                len += 32;
            }
        }

        fortuna_reseed(state, buf, len);

#if FORTUNA_RESEED_INTERVAL
        state->last_reseed = xtimer_now_usec64();
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
 * Corresponds to section 9.6.2.
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
    fortuna_reseed(state, (uint8_t *)inout, FORTUNA_SEED_SIZE);

#if FORTUNA_LOCK
    mutex_unlock(&state->lock);
#endif

    /* the seed file must be overwritten by a new seed file */
    return fortuna_random_data(state, (uint8_t *)inout, FORTUNA_SEED_SIZE);
}
