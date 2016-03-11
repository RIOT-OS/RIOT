/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_hashes Hashes
 * @ingroup     sys
 * @brief       Hash function library
 * @{
 *
 * @file
 * @brief       Hash function API
 *
 * @author      Jason Linehan <patientulysses@gmail.com>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef HASHES_H_
#define HASHES_H_

#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief djb2
 *
 * HISTORY
 * This algorithm (k=33) was first reported by Dan Bernstein many years
 * ago in comp.lang.c. Another version of this algorithm (now favored by
 * bernstein) uses XOR:
 *
 *      hash(i) = hash(i - 1) * 33 ^ str[i];
 *
 * The magic of number 33 (why it works better than many other constants,
 * prime or not) has never been adequately explained.
 *
 * @param buf input buffer to hash
 * @param len length of buffer
 * @return 32 bit sized hash
 */
uint32_t djb2_hash(const uint8_t *buf, size_t len);

/**
 * @brief sdbm
 *
 * HISTORY
 * This algorithm was created for sdbm (a public-domain reimplementation
 * of ndbm) database library. It was found to do well in scrambling bits,
 * causing better distribution of the keys and fewer splits. it also
 * happens to be a good general hashing function with good distribution.
 *
 * The actual function is
 *
 *      hash(i) = hash(i - 1) * 65599 + str[i];
 *
 * What is included below is the faster version used in gawk. [there is
 * even a faster, duff-device version] the magic constant 65599 was picked
 * out of thin air while experimenting with different constants, and turns
 * out to be a prime. this is one of the algorithms used in berkeley db
 * (see sleepycat) and elsewhere.
 *
 * @param buf input buffer to hash
 * @param len length of buffer
 * @return 32 bit sized hash
 */
uint32_t sdbm_hash(const uint8_t *buf, size_t len);

/**
 * @brief Kernighan and Ritchie
 *
 * HISTORY
 * This hash function appeared in K&R (1st ed) but at least the reader
 * was warned:
 *
 *      "This is not the best possible algorithm, but it has the merit
 *      of extreme simplicity."
 *
 * This is an understatement. It is a terrible hashing algorithm, and it
 * could have been much better without sacrificing its "extreme simplicity."
 * [see the second edition!]
 *
 * Many C programmers use this function without actually testing it, or
 * checking something like Knuth's Sorting and Searching, so it stuck.
 * It is now found mixed with otherwise respectable code, eg. cnews. sigh.
 * [see also: tpop]
 *
 * @param buf input buffer to hash
 * @param len length of buffer
 * @return 32 bit sized hash
 */
uint32_t kr_hash(const uint8_t *buf, size_t len);

/**
 * @brief Shift, Add, XOR
 *
 * @param buf input buffer to hash
 * @param len length of buffer
 * @return 32 bit sized hash
 */
uint32_t sax_hash(const uint8_t *buf, size_t len);

/**
 * @brief Donald E. Knuth
 *
 * HISTORY
 * Proposed by Donald E. Knuth in The Art Of Computer Programming Vol. 3,
 * under the topic of "Sorting and Search", Chapter 6.4.
 *
 * @param buf input buffer to hash
 * @param len length of buffer
 * @return 32 bit sized hash
 */
uint32_t dek_hash(const uint8_t *buf, size_t len);

/**
 * @brief Fowler–Noll–Vo
 *
 * NOTE
 * For a more fully featured and modern version of this hash, see fnv32.c
 *
 * @param buf input buffer to hash
 * @param len length of buffer
 * @return 32 bit sized hash
 */
uint32_t fnv_hash(const uint8_t *buf, size_t len);


/**
 * @brief Rotating
 *
 * found on
 * http://burtleburtle.net/bob/hash/doobs.html
 *
 * @param buf input buffer to hash
 * @param len length of buffer
 * @return 32 bit sized hash
 */
uint32_t rotating_hash(const uint8_t *buf, size_t len);

/**
 * @brief One at a time
 *
 * found on
 * http://burtleburtle.net/bob/hash/doobs.html
 *
 * @param buf input buffer to hash
 * @param len length of buffer
 * @return 32 bit sized hash
 */
uint32_t one_at_a_time_hash(const uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HASHES_H_ */
