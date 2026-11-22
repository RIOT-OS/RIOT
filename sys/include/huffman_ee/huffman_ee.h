/*
 * SPDX-FileCopyrightText: 2026 Harshita
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_huffman_ee Energy-aware Huffman coding
 * @ingroup     sys
 * @brief       Huffman coding that picks the 0/1 labels with the lowest
 *              transmission energy
 *
 * # What this module does
 *
 * Normal Huffman coding gives frequent symbols short codewords, but it does
 * not care which bits those codewords are made of. On many radios sending a
 * `1` costs more energy than sending a `0`, and every change between `0` and
 * `1` costs a little extra as well.
 *
 * This module builds the same tree shape as normal Huffman coding, so the
 * codeword lengths stay optimal. The only difference is the labelling: at
 * every merge of two subtrees there are exactly two ways to hand out the
 * labels 0 and 1. This module tries both and keeps the cheaper one, where
 * the cost of a codeword is
 *
 *     alpha * (number of 1 bits) + beta * (bit transitions inside the codeword)
 *
 * The energy of a whole stream also counts transitions between two
 * neighbouring codewords, weighted by lambda:
 *
 *     E = alpha * N1 + beta * Nt + lambda * Ns
 *
 * `lambda` is only used when *measuring* energy (huffman_ee_energy()). It is
 * not used while building the code, because it depends on which symbol
 * follows which, and that is not known when the tree is built.
 *
 * # Integer weights
 *
 * RIOT avoids floating point, so alpha, beta and lambda are unsigned
 * integers. The labelling decisions only depend on the ratio alpha : beta, so
 * a fractional setting can be written by scaling all weights by the same
 * factor. For example alpha = 1.5, beta = 1 becomes alpha = 3, beta = 2 and
 * builds exactly the same code.
 *
 * # Memory
 *
 * The module never allocates memory. Everything lives in a
 * huffman_ee_table_t that the caller provides. Building a code needs about
 * 16 bytes of stack per symbol of the alphabet.
 *
 * @{
 *
 * @file
 * @brief       Public API of the energy-aware Huffman coding module
 *
 * @author      Harshita <bansalharshita3012@gmail.com>
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of different symbols one table can hold
 *
 * Can be overridden at build time, e.g.
 * `CFLAGS += -DCONFIG_HUFFMAN_EE_MAX_SYMBOLS=64`. Must be at most 32767.
 */
#ifndef CONFIG_HUFFMAN_EE_MAX_SYMBOLS
#define CONFIG_HUFFMAN_EE_MAX_SYMBOLS   (256)
#endif

/**
 * @brief   Longest codeword (in bits) that can be produced
 *
 * Codewords are stored in a `uint32_t`. A very skewed distribution over a
 * large alphabet can need longer codes; the build functions then return
 * `-E2BIG` instead of producing a wrong code.
 */
#define HUFFMAN_EE_MAX_CODE_LEN         (32)

/**
 * @brief   Largest allowed value for alpha, beta and lambda
 *
 * Keeps all cost calculations safely inside 64 bit integers.
 */
#define HUFFMAN_EE_MAX_WEIGHT           (0xFFFFU)

/**
 * @brief   One codeword
 *
 * The codeword is stored in the lowest `length` bits of `bits`. The bit that
 * is transmitted first is the highest of those bits. Example: the codeword
 * `101` is stored as `bits = 0x5`, `length = 3`.
 */
typedef struct {
    uint32_t bits;      /**< the codeword itself */
    uint8_t length;     /**< number of valid bits in @p bits */
} huffman_ee_code_t;

/**
 * @brief   Energy weights (see the module description)
 */
typedef struct {
    uint32_t alpha;     /**< cost of transmitting a `1` bit */
    uint32_t beta;      /**< cost of a bit transition inside a codeword */
    uint32_t lambda;    /**< cost of a transition between two codewords */
} huffman_ee_params_t;

/**
 * @brief   A complete code: the symbols, their frequencies and codewords
 *
 * Filled in by the `huffman_ee_build*()` functions. The entries at index `i`
 * of the three arrays belong together.
 */
typedef struct {
    uint16_t symbol[CONFIG_HUFFMAN_EE_MAX_SYMBOLS];             /**< symbol values */
    uint32_t freq[CONFIG_HUFFMAN_EE_MAX_SYMBOLS];               /**< how often each occurs */
    huffman_ee_code_t code[CONFIG_HUFFMAN_EE_MAX_SYMBOLS];      /**< codeword of each */
    uint16_t num_symbols;                                       /**< entries in use */
} huffman_ee_table_t;

/**
 * @brief   Bit counts of an encoded stream (the N1, Nt and Ns of the paper)
 */
typedef struct {
    uint32_t ones;          /**< N1: number of `1` bits */
    uint32_t transitions;   /**< Nt: 0/1 changes inside codewords */
    uint32_t boundaries;    /**< Ns: 0/1 changes between neighbouring codewords */
} huffman_ee_counts_t;

/**
 * @brief   Build the energy-aware code (the method from the paper)
 *
 * At every merge both labellings are tried and the one with the lower
 * `alpha * N1 + beta * Nt` cost is kept. Ties are resolved in favour of
 * giving the first (lighter) subtree the label 0.
 *
 * @param[out] table        table to fill in
 * @param[in]  symbols      the symbol values, all different
 * @param[in]  freqs        how often each symbol occurs, all greater than 0
 * @param[in]  num_symbols  length of @p symbols and @p freqs
 * @param[in]  params       energy weights (only alpha and beta are used here)
 *
 * @retval  0           success
 * @retval  -EINVAL     bad arguments (NULL pointer, no symbols, zero
 *                      frequency, duplicate symbol, weight too large)
 * @retval  -E2BIG      too many symbols, or a codeword longer than
 *                      ::HUFFMAN_EE_MAX_CODE_LEN would be needed
 */
int huffman_ee_build(huffman_ee_table_t *table, const uint16_t *symbols,
                     const uint32_t *freqs, size_t num_symbols,
                     const huffman_ee_params_t *params);

/**
 * @brief   Build the OPTIMAL code for the fixed Huffman tree (exact)
 *
 * Works on the same tree as huffman_ee_build(), but instead of choosing the
 * labels greedy at every merge, an exact dynamic program picks the labelling
 * with the lowest possible `alpha * N1 + beta * Nt` for that tree. It is
 * therefore never worse than huffman_ee_build() (it is often equal, because
 * the greedy choice happens to be optimal on most small distributions). Ties
 * are resolved in favour of giving the first subtree the label 0.
 *
 * Only the alpha and beta weights are used (as in huffman_ee_build()).
 *
 * @note    Needs more stack than the other builders, roughly 60 bytes per
 *          symbol of the alphabet.
 *
 * Parameters and return values as for huffman_ee_build().
 */
int huffman_ee_build_exact(huffman_ee_table_t *table, const uint16_t *symbols,
                           const uint32_t *freqs, size_t num_symbols,
                           const huffman_ee_params_t *params);

/**
 * @brief   Build the OPTIMAL code including the boundary cost (exact)
 *
 * Like huffman_ee_build_exact(), but the objective also contains
 * `lambda * Ns`, where Ns is the expected number of transitions between
 * neighbouring codewords. Ns can only be predicted for independent symbols,
 * so this assumes the symbols are drawn independently with the given
 * frequencies (Ns = firstBitProb * lastBitProb ... ). Use it when the stream
 * is close to i.i.d.; for strongly correlated streams huffman_ee_build_exact()
 * is the safer choice.
 *
 * The choice of lambda can change the optimal labelling, which is why lambda
 * participates here (unlike in huffman_ee_build()).
 *
 * @note    Needs more stack than the other builders, roughly 60 bytes per
 *          symbol of the alphabet.
 *
 * Additional return value on top of those of huffman_ee_build(): -E2BIG if
 * the weights and the total frequency are so large that the scaled costs
 * would not fit in 64 bits.
 */
int huffman_ee_build_exact_lambda(huffman_ee_table_t *table,
                                  const uint16_t *symbols, const uint32_t *freqs,
                                  size_t num_symbols,
                                  const huffman_ee_params_t *params);

/**
 * @brief   Build a normal Huffman code (baseline for comparisons)
 *
 * Same tree as huffman_ee_build(), but the first subtree of every merge
 * always gets the label 0.
 *
 * Parameters and return values as for huffman_ee_build(), without `params`.
 */
int huffman_ee_build_plain(huffman_ee_table_t *table, const uint16_t *symbols,
                           const uint32_t *freqs, size_t num_symbols);

/**
 * @brief   Build a Huffman code with random 0/1 labels (baseline)
 *
 * Normal Huffman coding is not unique: the tree shape is fixed but the 0/1
 * labels can be handed out in many ways. Averaging over several random
 * labellings gives a fairer baseline than one arbitrary choice.
 *
 * @param[in]  seed         seed of the random number generator; different
 *                          seeds give different labellings
 *
 * Other parameters and return values as for huffman_ee_build_plain().
 */
int huffman_ee_build_random(huffman_ee_table_t *table, const uint16_t *symbols,
                            const uint32_t *freqs, size_t num_symbols,
                            uint32_t seed);

/**
 * @brief   Encode a stream of symbols
 *
 * Bits are packed starting at the most significant bit of the first byte.
 * The unused bits of the last byte are set to 0.
 *
 * @param[in]  table        code to use
 * @param[in]  input        symbols to encode
 * @param[in]  input_len    number of symbols in @p input
 * @param[out] out          buffer for the encoded bits
 * @param[in]  out_size     size of @p out in bytes
 * @param[out] out_bits     number of valid bits written to @p out
 *
 * @retval  0           success
 * @retval  -EINVAL     bad argument, or a symbol that is not in the table
 * @retval  -ENOBUFS    @p out is too small
 */
int huffman_ee_encode(const huffman_ee_table_t *table, const uint16_t *input,
                      size_t input_len, void *out, size_t out_size,
                      size_t *out_bits);

/**
 * @brief   Decode a stream produced by huffman_ee_encode()
 *
 * @param[in]  table        code that was used for encoding
 * @param[in]  in           encoded data
 * @param[in]  in_bits      number of valid bits in @p in
 * @param[out] out          buffer for the decoded symbols
 * @param[in]  out_max      capacity of @p out in symbols
 * @param[out] out_len      number of symbols written to @p out
 *
 * @retval  0           success
 * @retval  -EINVAL     bad argument or data that is not a valid encoding
 * @retval  -ENOBUFS    @p out is too small
 */
int huffman_ee_decode(const huffman_ee_table_t *table, const void *in,
                      size_t in_bits, uint16_t *out, size_t out_max,
                      size_t *out_len);

/**
 * @brief   Count N1, Nt and Ns for a stream of symbols
 *
 * This looks at the codewords the stream would be encoded with, so it gives
 * the same numbers as counting in the encoded bits.
 *
 * @param[in]  table        code to use
 * @param[in]  input        symbols of the stream
 * @param[in]  input_len    number of symbols in @p input
 * @param[out] counts       the result
 *
 * @retval  0           success
 * @retval  -EINVAL     bad argument, or a symbol that is not in the table
 */
int huffman_ee_count(const huffman_ee_table_t *table, const uint16_t *input,
                     size_t input_len, huffman_ee_counts_t *counts);

/**
 * @brief   Energy of a stream: `alpha * N1 + beta * Nt + lambda * Ns`
 *
 * @param[in]  counts       result of huffman_ee_count()
 * @param[in]  params       energy weights
 *
 * @return  the energy, in the (arbitrary) unit of the weights
 */
uint64_t huffman_ee_energy(const huffman_ee_counts_t *counts,
                           const huffman_ee_params_t *params);

#ifdef __cplusplus
}
#endif

/** @} */
