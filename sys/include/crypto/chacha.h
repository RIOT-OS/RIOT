/*
 * Copyright (C) 2008  D. J. Bernstein  (dedicated to the public domain)
 * Copyright (C) 2015  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       ChaCha stream cipher
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CRYPTO_CHACHA_H
#define CRYPTO_CHACHA_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A ChaCha cipher stream context.
 * @details Initialize with chacha_init().
 */
typedef struct
{
    uint32_t state[16]; /**< The current state of the stream. */
    uint8_t rounds; /**< Number of iterations. */
} chacha_ctx;

/**
 * @brief Initialize a ChaCha context
 *
 * @param[out] ctx     The context to initialize
 * @param[in]  rounds  Number of rounds. Recommended: 20. Also in use: 8 and 12.
 * @param[in]  key     The key to use.
 * @param[in]  keylen  Length (in bytes) of @p key. Must be 16 or 32.
 * @param[in]  nonce   IV / nonce to use.
 *
 * @returns `== 0` on success.
 * @returns `< 0` if an illegal value for @p rounds or @p keylen was suppplied.
 */
int chacha_init(chacha_ctx *ctx,
                unsigned rounds,
                const uint8_t *key, uint32_t keylen,
                const uint8_t nonce[8]);

/**
 * @brief Generate next block in the keystream.
 *
 * @details If you want to seek inside the cipher steam, then you have to
 *          update the clock in `ctx->state[13]:ctx->state[12]` manually.
 *
 * @warning You need to re-initialized the context with a new nonce after 2^64
 *          encrypted blocks, or the keystream will repeat!
 *
 * @param[in,out] ctx The ChaCha context
 * @param[out]    x   The block of the keystream (`sizeof(x) == 64`).
 */
void chacha_keystream_bytes(chacha_ctx *ctx, void *x);

/**
 * @brief Encode or decode a block of data.
 *
 * @details @p m is always the input regardless if it is the plaintext or ciphertext,
 *          and @p c vice verse.
 *
 * @warning You need to re-initialized the context with a new nonce after 2^64
 *          encrypted blocks, or the keystream will repeat!
 *
 * @param[in,out] ctx The ChaCha context.
 * @param[in]     m   The input.
 * @param[out]    c   The output.
 */
void chacha_encrypt_bytes(chacha_ctx *ctx, const uint8_t *m, uint8_t *c);

/**
 * @copydoc chacha_encrypt_bytes()
 */
static inline void chacha_decrypt_bytes(chacha_ctx *ctx, const uint8_t *m, uint8_t *c)
{
    chacha_encrypt_bytes(ctx, m, c);
}

/**
 * @brief Seed the pseudo-random number generator.
 *
 * @details You can seed the random number generator with up to 64 bytes of data.
 *          If you feed less than 64 bytes of data, then the privous state gets
 *          only partially overwritten.
 *
 *          If you want to supply multiple information, then you have to concatenate
 *          them manually before invoking the function.
 *
 *          The PRNG gets a random seed in the build process.
 *          You can set a deterministic value by supplying a comma separated
 *          argument to `make` like
 *          `RIOT_CHACHA_PRNG_DEFAULT="0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15"`.
 *
 * @param[in] data  Some random data.
 * @param[in] bytes Length of @p data in bytes where `0 < bytes <= 64`.
 */
void chacha_prng_seed(const void *data, size_t bytes);

/**
 * @brief Extract a number from the pseudo-random number generator.
 *
 * @warning After you have read 2^68 numbers you have to re-seed the PRNG.
 *          Otherwise the sequence will repeat.
 */
uint32_t chacha_prng_next(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef CRYPTO_CHACHA_H */

/**
 * @}
 */
