/**
 * @brief Fortuna PRNG implementation.
 *
 * The MIT License applies to this software. See the included LICENSE.txt file
 * for more information.
 */

/*
 * This is not your general purpose PRNG: it is hungry for memory.
 *
 * See https://www.schneier.com/cryptography/paperfiles/fortuna.pdf for the
 * implementation details. Code insipred by https://github.com/blin00/os (MIT
 * licensed), but heavily modified, stripped and improved for RIOT-OS.
 *
 * The implementation follows the paper, with a few exceptions:
 *
 * - Initialization of the generator and PRNG is combined in fortuna_init.
 * - The check if the generator is seeded is moved from fortuna_generate_blocks
 *   to fortuna_pseudo_random_data for optimization reasons.
 * - In fortuna_random_data, the check on state->reseeds == 0 is not performed.
 *   It would never provide a seed whenever the PRNG is initialized with a seed
 *   file, when entropy sources are not available (yet). It would still fail
 *   whenever fortuna_pseudo_random_data checks if the generator is seeded! See
 *   https://crypto.stackexchange.com/q/56390 for more information.
 */

#ifndef FORTUNA_H
#define FORTUNA_H

#include "xtimer.h"
#include "mutex.h"

#include "crypto/aes.h"
#include "hashes/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @bief Number of pools to use, which may not exceed 32. More pools means more
 *       memory usage, but makes it harder for an attacker to influence the
 *       state. The recommended value is 32, as discussed in section 9.5.2.
 */
#ifndef FORTUNA_POOLS
#define FORTUNA_POOLS               (32U)
#endif

/**
 * @brief Minimum number of bytes that should be in a pool. Higher values will
 *        delay reseeds even if good random entropy is collected. In section
 *        9.5.5, a suitable value of 64 bytes is suggested.
 */
#ifndef FORTUNA_MIN_POOL_SIZE
#define FORTUNA_MIN_POOL_SIZE       (64U)
#endif


/**
 * @brief Definition of the seed file size used to initialize the PRNG. The
 *        default value of 64 bytes is suggested by section 9.6.1.
 */
#ifndef FORTUNA_SEED_SIZE
#define FORTUNA_SEED_SIZE           (64U)
#endif

/**
 * @brief Reseed interval in us. After this interval, the PRNG must be
 *        reseeded. Per section 9.5.5, the recommended value is 100ms. Set to
 *        zero to disable this security feature.
 */
#ifndef FORTUNA_RESEED_INTERVAL
#define FORTUNA_RESEED_INTERVAL     (0)
#endif

/**
 * @brief Reseed limit in bytes. After this number of bytes, the PRNG must be
 *        reseeded. Per section 9.4.4, the recommended value is 2^20 bytes
 *        (=1 MB). Set to zero to disable this security feature.
 */
#ifndef FORTUNA_RESEED_LIMIT
#define FORTUNA_RESEED_LIMIT        (1U << 20)
#endif

/**
 * @brief Fortuna requires a block cipher with a 256-bit key to minimize
 *        permutation attacks.
 *
 * @note  RIOT-OS does not (yet) support AES-256. Therefore, at the expense of
 *        security, AES-128 is used instead. An detailed explanation on the
 *        effects can be found at https://crypto.stackexchange.com/a/5736.
 */
#ifndef FORTUNA_AES_KEY_SIZE
#define FORTUNA_AES_KEY_SIZE        (16U)
#endif

/**
 * @brief Use a mutex to lock concurrent access when running sensitive
 *        operations in multi-threaded applications.
 */
#ifndef FORTUNA_LOCK
#define FORTUNA_LOCK                (1)
#endif

/**
 * @brief When set, perform additional instructions to clear temporary
 *        variables to prevent leaking sensitive information.
 */
#ifndef FORTUNA_CLEANUP
#define FORTUNA_CLEANUP             (1)
#endif

/**
 * @brief Generator counter and key.
 */
typedef struct {
    uint8_t key[32];
    union {
        struct {
            uint64_t l;
            uint64_t h;
        } split;
        uint8_t bytes[16];
    } counter;
} fortuna_generator_t;

/**
 * @brief Pool for storing entropy.
 */
typedef struct {
    sha256_context_t ctx;
    uint32_t len;
} fortuna_pool_t;

/**
 * @brief The Fortuna state.
 */
typedef struct {
    fortuna_generator_t gen;
    fortuna_pool_t pools[FORTUNA_POOLS];
    uint32_t reseeds;
#if FORTUNA_RESEED_INTERVAL > 0
    uint64_t last_reseed;
#endif
#if FORTUNA_LOCK
    mutex_t lock;
#endif
} fortuna_state_t;

/**
 * @brief Type definition of a Fortuna seed file.
 */
typedef uint32_t fortuna_seed_t[FORTUNA_SEED_SIZE];

/**
 * @brief   Initialize the Fortuna PRNG state.
 *
 * It is possible to use this method to clear an existing state.
 *
 * @param[inout] state      PRNG state
 *
 * @return                  zero on successful initialization
 * @return                  non-zero on error
 */
int fortuna_init(fortuna_state_t *state);

/**
 * @brief   Read random bytes from the PRNG. The number of bytes may not exceed
 *          FORTUNA_RESEED_LIMIT bytes.
 *
 * @param[inout] state      PRNG state
 * @param[out] out          pointer to buffer
 * @param[in] bytes         number of bytes to write in buffer
 *
 * @return                  zero on success
 * @return                  -1 on reading more that FORTUNA_RESEED_LIMIT bytes
 * @return                  -2 on reading from unseeded PRNG
 * @return                  -3 on other error
 */
int fortuna_random_data(fortuna_state_t *state, uint8_t *out, size_t bytes);

/**
 * @brief   Add a entropy of a random event to one PRNG pool. The pool must
 *          exist and the source length must be 1-32 bytes.
 *
 * @param[inout] state      PRNG state
 * @param[in] data          pointer to entropy source
 * @param[in] length        length of entropy source
 * @param[in] source        source identifier (each source has its own ID)
 * @param[in] pool          pool number to add entropy to
 *
 * @return                  zero on success
 * @return                  -1 on zero bytes or more than 32 bytes
 * @return                  -2 on invalid pool number
 */
int fortuna_add_random_event(fortuna_state_t *state, const uint8_t *data,
                             uint8_t length, uint8_t source, uint8_t pool);

/**
 * @brief   Generate 64 bytes from the PRNG and write that to an output
 *          buffer for use on next startup.
 *
 * This method must be invoked before shutting down the PRNG (e.g. on system
 * shutdown).
 *
 * @param[inout] state      PRNG state
 * @param[out] data         pointer to output buffer for the seed
 *
 * @return                  zero on success
 */
int fortuna_write_seed(fortuna_state_t *state, fortuna_seed_t *out);

/**
 * @brief   Reseed the PRNG using a seed. By design, this value will be
 *          overwritten after use.
 *
 * This method should be invoked once on PRNG startup, in case a seed is
 * available.
 *
 * @param[inout] state      PRNG state
 * @param[inout] data       pointer to input and output buffer for the seed
 *
 * @return                  zero on success
 */
int fortuna_update_seed(fortuna_state_t *state, fortuna_seed_t *inout);

#ifdef __cplusplus
}
#endif

#endif /* FORTUNA_H */
/** @} */
