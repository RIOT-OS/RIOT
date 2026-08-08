/*

Implementation by the Keccak, Keyak and Ketje Teams, namely, Guido Bertoni,
Joan Daemen, Michaël Peeters, Gilles Van Assche and Ronny Van Keer, hereby
denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
http://keccak.noekeon.org/
http://keyak.noekeon.org/
http://ketje.noekeon.org/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#ifndef KECCAK800_H
#define KECCAK800_H

#define KeccakP200_excluded
#define KeccakP400_excluded

#ifndef KeccakP800_excluded

#include "KeccakSpongeWidth800.h"
#include "keccak_defs.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A struct for an instance of Keccak-800, containing state and
 *          parameters.
 */
typedef struct {
    KeccakWidth800_SpongeInstance sponge; /**< The sponge instance. */
    unsigned int fixed_output_length;     /**< The output length of the sponge
                                               instance. */
    unsigned char delimited_suffix;       /**< The delimited suffix describing
                                               the padding rule. */
} keccak800_t;

/**
  * @brief      Function to initialize the Keccak[r, c] sponge function
  *             instance used in sequential hashing mode.
  * @param[in]  hash_instance     Pointer to the hash instance to be
  *                               initialized.
  * @param[in]  rate              The value of the rate r.
  * @param[in]  capacity          The value of the capacity c.
  * @param[in]  hashbitlen        The desired number of output bits,
  *                               or 0 for an arbitrarily-long output.
  * @param[in]  delimited_suffix  Bits that will be automatically appended to
  *                               the end of the input message, as in domain
  *                               separation. This is a byte containing from 0
  *                               to 7 bits formatted like the @a delimitedData
  *                               parameter of the
  *                               Keccak_SpongeAbsorbLastFewBits() function.
  * @pre        One must have r+c=800 and the rate a multiple of 8 bits in this
  *             implementation.
  * @return     SUCCESS if successful, FAIL otherwise.
  */
hash_return keccak800hash_initialize(
  keccak800_t *hash_instance,
  unsigned int rate,
  unsigned int capacity,
  unsigned int hashbitlen,
  unsigned char delimited_suffix);

/** @brief      Function to initialize a Keccak-800 instance for XOF
  *             with a security level of 128 bits.
  * @param[in]  hash_instance     Pointer to the hash instance to be
  *                               initialized.
  */
static inline void keccak800xof_128_initialize(keccak800_t *hash_instance) {
    keccak800hash_initialize(hash_instance, 544, 256, 0, 0x1F);
}

/** @brief      Function to initialize a Keccak-800 instance for XOF
  *             with a security level of 256 bits.
  * @param[in]  hash_instance     Pointer to the hash instance to be
  *                               initialized.
  */
static inline void keccak800xof_256_initialize(keccak800_t *hash_instance) {
    keccak800hash_initialize(hash_instance, 288, 512, 0, 0x1F);
}

/** @brief      Function to initialize a Keccak-800 instance for hashing
  *             with a security level of 128 bits.
  * @param[in]  hash_instance     Pointer to the hash instance to be
  *                               initialized.
  */
static inline void keccak800hash_128_initialize(keccak800_t *hash_instance) {
    keccak800hash_initialize(hash_instance, 544, 256, 128, 0x06);
}

/** @brief      Function to initialize a Keccak-800 instance for hashing
  *             with a security level and output size of 224 bits.
  * @param[in]  hash_instance     Pointer to the hash instance to be
  *                               initialized.
  */
static inline void keccak800hash_224_initialize(keccak800_t *hash_instance) {
    keccak800hash_initialize(hash_instance, 352, 448, 224, 0x06);
}

/** @brief      Function to initialize a Keccak-800 instance for hashing
  *             with a security level and output size of 256 bits.
  * @param[in]  hash_instance     Pointer to the hash instance to be
  *                               initialized.
  */
static inline void keccak800hash_256_initialize(keccak800_t *hash_instance) {
    keccak800hash_initialize(hash_instance, 288, 512, 256, 0x06);
}

/** @brief      Function to initialize a Keccak-800 instance for hashing
  *             with a security level and output size of 384 bits.
  * @param[in]  hash_instance     Pointer to the hash instance to be
  *                               initialized.
  */
static inline void keccak800hash_384_initialize(keccak800_t *hash_instance) {
    keccak800hash_initialize(hash_instance, 32, 768, 384, 0x06);
}

/**
  * @brief      Function to give input data to be absorbed.
  * @param[in]  hash_instance     Pointer to the hash instance initialized by
  *                               keccak800hash_initialize().
  * @param[in]  data              Pointer to the input data.
  *                               When @a databitLen is not a multiple of 8,
  *                               the last bits of data must be in the least
  *                               significant bits of the last byte
  *                               (little-endian convention).
  * @param[in]  databitLen        The number of input bits provided in the
  *                               input data.
  * @pre        In the previous call to keccak800hash_update(), databitlen was a
  *             multiple of 8.
  * @return     SUCCESS if successful, FAIL otherwise.
  */
hash_return keccak800hash_update(
    keccak800_t *hash_instance,
    const bit_sequence *data,
    bit_length databitlen);

/**
  * @brief      Function to call after all input blocks have been input and to
  *             get output bits if the length was specified when calling
  *             keccak800hash_initialize().
  * @param[in]  hash_instance     Pointer to the hash instance initialized by
  *                               keccak1600hash_initialize().
  * @note       If @a hashbitlen was not 0 in the call to
  *             keccak800hash_initialize(), the number of output bits is equal
  *             to @a hashbitlen.
  *             If @a hashbitlen was 0 in the call to
  *             keccak1600hash_initialize(), the output bits must be extracted
  *             using the keccak800hash_squeeze() function.
  * @param[in]  hashval           Pointer to the buffer where to store the
  *                               output data.
  * @return     SUCCESS if successful, FAIL otherwise.
  */
hash_return keccak800hash_final(
    keccak800_t *hash_instance,
    bit_sequence *hashval);

 /**
  * @brief      Function to squeeze output data.
  * @param[in]  hash_instance     Pointer to the hash instance initialized by
  *                               keccak800hash_initialize().
  * @param[in]  data              Pointer to the buffer where to store the
  *                               output data.
  * @param[in]  databitlen        The number of output bits desired (must be a
  *                               multiple of 8).
  * @pre        keccak1600hash_final() must have been already called.
  * @pre        @a databitlen is a multiple of 8.
  * @return     SUCCESS if successful, FAIL otherwise.
  */
hash_return keccak800hash_squeeze(
    keccak800_t *hash_instance,
    bit_sequence *data,
    bit_length databitlen);

#endif

#ifdef __cplusplus
}
#endif

#endif /* KECCAK800_H */
