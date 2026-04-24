/*
 * SPDX-FileCopyrightText: Keccak, Keyak and Ketje Teams
 * SPDX-FileCopyrightText: Guido Bertoni
 * SPDX-FileCopyrightText: Joan Daemen
 * SPDX-FileCopyrightText: Michaël Peeters
 * SPDX-FileCopyrightText: Gilles Van Assche
 * SPDX-FileCopyrightText: Ronny Van Keer
 * SPDX-FileCopyrightText: Mathias Tausig
 * SPDX-License-Identifier: CC0-1.0
 */

/**
 * @defgroup    sys_hashes_sha3 SHA-3
 * @ingroup     sys_hashes_unkeyed
 * @brief       Implementation of the SHA-3 hashing function
 * @{
 *
 * @file
 * @brief       Header definitions for the SHA-3 hash function
 *
 * @author      Implementation by the Keccak, Keyak and Ketje Teams (https://keccak.team/)
 * @author      RIOT OS adaptations by Mathias Tausig
 */

#ifndef HASHES_SHA3_H
#define HASHES_SHA3_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of SHA3-256 digests in bytes
 */
#define SHA3_256_DIGEST_LENGTH 32

/**
 * @brief   Length of SHA3-384 digests in bytes
 */
#define SHA3_384_DIGEST_LENGTH 48

/**
 * @brief   Length of SHA3-512 digests in bytes
 */
#define SHA3_512_DIGEST_LENGTH 64

/**
 * @brief Context for operations on a sponge with keccak permutation
 */
typedef struct {
    /** State of the Keccak sponge */
    unsigned char state[200];
    /** Current position within the state */
    unsigned int i;
    /** The suffix used for padding */
    unsigned char delimitedSuffix;
    /** The bitrate of the sponge */
    unsigned int rate;
    /** The capacity in bits of the sponge */
    unsigned int capacity;
    /** The rate in bytes of the sponge */
    unsigned int rateInBytes;
} keccak_state_t;

/**
 * @brief Initialise a sponge based on a keccak-1600 permutation
 *
 * @param[out] ctx             context handle to initialise
 * @param[in] rate             the desired rate of the sponge
 * @param[in] capacity         the desired capacity of the sponge
 * @param[in] delimitedSuffix  suffix to be appended to the message after the absorbation phase
 */
void Keccak_init(keccak_state_t *ctx, unsigned int rate, unsigned int capacity,
                 unsigned char delimitedSuffix);

/**
 * @brief Absorbs data into a sponge. Can be called multiple times
 *
 * @param[in,out] ctx       context handle of the sponge
 * @param[in] input         pointer to the data to be absorbed
 * @param[in] inputByteLen  length of the input data in bytes
 */
void Keccak_update(keccak_state_t *ctx, const unsigned char *input,
                   unsigned long long int inputByteLen);

/**
 * @brief Squeeze data from a sponge
 *
 * @param[in,out] ctx        context handle of the sponge
 * @param[out] output        the squeezed data
 * @param[in] outputByteLen  size of the data to be squeezed.
 */
void Keccak_final(keccak_state_t *ctx, unsigned char *output,
                  unsigned long long int outputByteLen);

/**
 * @brief SHA3-256 initialization.  Begins a SHA3-256 operation.
 *
 * @param[in] ctx  keccak_state_t handle to initialise
 */
void sha3_256_init(keccak_state_t *ctx);

/**
 * @brief Add bytes into the hash
 *
 * @param[in,out] ctx  context handle to use
 * @param[in] data     Input data
 * @param[in] len      Length of @p data
 */
void sha3_update(keccak_state_t *ctx, const void *data, size_t len);

/**
 * @brief SHA3-256 finalization.  Pads the input data and exports the hash value
 *
 * @param[in,out] ctx    context handle to use
 * @param[out] digest    resulting digest, this is the hash of all the bytes
 */
void sha3_256_final(keccak_state_t *ctx, void *digest);

/**
 * @brief SHA3-384 initialization.  Begins a SHA3-384 operation.
 *
 * @param[in] ctx  keccak_state_t handle to initialise
 */
void sha3_384_init(keccak_state_t *ctx);

/**
 * @brief SHA3-384 finalization.  Pads the input data and exports the hash value
 *
 * @param[in,out] ctx    context handle to use
 * @param[out] digest    resulting digest, this is the hash of all the bytes
 */
void sha3_384_final(keccak_state_t *ctx, void *digest);

/**
 * @brief SHA3-512 initialization.  Begins a SHA3-512 operation.
 *
 * @param[in] ctx  keccak_state_t handle to initialise
 */
void sha3_512_init(keccak_state_t *ctx);

/**
 * @brief SHA3-512 finalization.  Pads the input data and exports the hash value
 *
 * @param[in,out] ctx    context handle to use
 * @param[out] digest    resulting digest, this is the hash of all the bytes
 */

void sha3_512_final(keccak_state_t *ctx, void *digest);

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * useful for generating SHA3-256 from one buffer
 *
 * @param[in] data     pointer to the buffer to generate hash from
 * @param[in] len      length of the buffer
 * @param[out] digest  pointer to an array for the result, length must
 *                     be SHA3_256_DIGEST_LENGTH
 */
void sha3_256(void *digest, const void *data, size_t len);

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * useful for generating SHA3-384 from one buffer
 *
 * @param[in] data     pointer to the buffer to generate hash from
 * @param[in] len      length of the buffer
 * @param[out] digest  pointer to an array for the result, length must
 *                     be SHA3_384_DIGEST_LENGTH
 */
void sha3_384(void *digest, const void *data, size_t len);

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * useful for generating SHA3-512 from one buffer
 *
 * @param[in] data     pointer to the buffer to generate hash from
 * @param[in] len      length of the buffer
 * @param[out] digest  pointer to an array for the result, length must
 *                     be SHA3_512_DIGEST_LENGTH
 */
void sha3_512(void *digest, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* HASHES_SHA3_H */
/** @} */
