/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SEQ_H
#define SEQ_H

/**
 * @ingroup     sys_seq
 * @{
 *
 * @file
 * @brief       Serial Number Arithmetic (RFC 1982)
 * @see         <a href="https://tools.ietf.org/html/rfc1982">
 *                  RFC 1982 - Serial Number Arithmetic
 *              </a>
 *
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Maximum for the addition of a positive integer. X denotes the size of the space
 */
#define SEQ_LIMIT(X)            (X >> 1)

/**
 * @brief       A 8 bit sequence number
 */
typedef uint8_t seq8_t;

/**
 * @brief       A 16 bit sequence number
 */
typedef uint16_t seq16_t;

/**
 * @brief       A 32 bit sequence number
 */
typedef uint32_t seq32_t;

/**
 * @brief       A 64 bit sequence number
 */
typedef uint64_t seq64_t;

/**
 * @brief           Addition of a 8 bit sequence number @p s and a positive integer @p n
 *                  in the serial number @p space
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.1">
 *                      3.1. Addition
 *                  </a>
 * @param[in]       s       sequence number
 * @param[in]       n       positive integer in the range of [0 .. ((@p space / 2) - 1)]
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          s + n, if valid
 * @return          s, if n is out of range
 */
seq8_t seq8_adds(seq8_t s, uint8_t n, uint8_t space);

/**
 * @brief           Addition of a 8 bit sequence number @p s and a positive integer @p n
 *                  in the serial number space UINT8_MAX
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.1">
 *                      3.1. Addition
 *                  </a>
 * @param[in]       s       sequence number
 * @param[in]       n       positive integer in the range of [0 .. 127]
 * @return          s + n, if valid
 * @return          s, if n is out of range
 */
static inline seq8_t seq8_add(seq8_t s, uint8_t n)
{
    return seq8_adds(s, n, UINT8_MAX);
}

/**
 * @brief           Increment a sequence number @p s by 1 in the serial number @p space
 * @param[in]       s       sequence number
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          s + 1
 */
static inline seq8_t seq8_incs(seq8_t s, uint8_t space)
{
    return seq8_adds(s, 1, space);
}

/**
 * @brief           Increment a sequence number @p s by 1 in the serial number space UINT8_MAX
 * @param[in]       s       sequence number
 * @return          s + 1
 */
static inline seq8_t seq8_inc(seq8_t s)
{
    return seq8_adds(s, 1, UINT8_MAX);
}

/**
 * @brief           Compare sequence numbers @p s1, @p s2 in the serial number @p space
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.2">
 *                      3.2. Comparison
 *                  </a>
 * @param[in]       s1      first sequence number
 * @param[in]       s2      second sequence number
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          -1, if s1 < s2
 * @return          0, if s1 == s2
 * @return          1, if s1 > s2
 * @return          -EINVAL, if comparison of the pair (s1,s2) is undefined
 */
int seq8_compares(seq8_t s1, seq8_t s2, uint8_t space);

/**
 * @brief           Compare sequence numbers @p s1, @p s2 in the serial number space UINT8_MAX
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.2">
 *                      3.2. Comparison
 *                  </a>
 * @param[in]       s1      first sequence number
 * @param[in]       s2      second sequence number
 * @return          -1, if s1 < s2
 * @return          0, if s1 == s2
 * @return          1, if s1 > s2
 * @return          -EINVAL, if comparison of the pair (s1,s2) is undefined
 */
static inline int seq8_compare(seq8_t s1, seq8_t s2)
{
    return seq8_compares(s1, s2, UINT8_MAX);
}

/**
 * @brief           Addition of a 16 bit sequence number @p s and a positive integer @p n
 *                  in the serial number @p space
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.1">
 *                      3.1. Addition
 *                  </a>
 * @param[in]       s       sequence number
 * @param[in]       n       positive integer in the range of [0 .. ((@p space / 2) - 1)]
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          s + n, if valid
 * @return          s, if n is out of range
 */
seq16_t seq16_adds(seq16_t s, uint16_t n, uint16_t space);

/**
 * @brief           Addition of a 16 bit sequence number @p s and a positive integer @p n
 *                  in the serial number space UINT16_MAX
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.1">
 *                      3.1. Addition
 *                  </a>
 * @param[in]       s       sequence number
 * @param[in]       n       positive integer in the range of [0 .. 127]
 * @return          s + n, if valid
 * @return          s, if n is out of range
 */
static inline seq16_t seq16_add(seq16_t s, uint16_t n)
{
    return seq16_adds(s, n, UINT16_MAX);
}

/**
 * @brief           Increment a sequence number @p s by 1 in the serial number @p space
 * @param[in]       s       sequence number
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          s + 1
 */
static inline seq16_t seq16_incs(seq16_t s, uint16_t space)
{
    return seq16_adds(s, 1, space);
}

/**
 * @brief           Increment a sequence number @p s by 1 in the serial number space UINT16_MAX
 * @param[in]       s       sequence number
 * @return          s + 1
 */
static inline seq16_t seq16_inc(seq16_t s)
{
    return seq16_adds(s, 1, UINT16_MAX);
}

/**
 * @brief           Compare sequence numbers @p s1, @p s2 in the serial number @p space
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.2">
 *                      3.2. Comparison
 *                  </a>
 * @param[in]       s1      first sequence number
 * @param[in]       s2      second sequence number
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          -1, if s1 < s2
 * @return          0, if s1 == s2
 * @return          1, if s1 > s2
 * @return          -EINVAL, if comparison of the pair (s1,s2) is undefined
 */
int seq16_compares(seq16_t s1, seq16_t s2, uint16_t space);

/**
 * @brief           Compare sequence numbers @p s1, @p s2 in the serial number space UINT16_MAX
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.2">
 *                      3.2. Comparison
 *                  </a>
 * @param[in]       s1      first sequence number
 * @param[in]       s2      second sequence number
 * @return          -1, if s1 < s2
 * @return          0, if s1 == s2
 * @return          1, if s1 > s2
 * @return          -EINVAL, if comparison of the pair (s1,s2) is undefined
 */
static inline int seq16_compare(seq16_t s1, seq16_t s2)
{
    return seq16_compares(s1, s2, UINT16_MAX);
}

/**
 * @brief           Addition of a 32 bit sequence number @p s and a positive integer @p n
 *                  in the serial number @p space
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.1">
 *                      3.1. Addition
 *                  </a>
 * @param[in]       s       sequence number
 * @param[in]       n       positive integer in the range of [0 .. ((@p space / 2) - 1)]
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          s + n, if valid
 * @return          s, if n is out of range
 */
seq32_t seq32_adds(seq32_t s, uint32_t n, uint32_t space);

/**
 * @brief           Addition of a 32 bit sequence number @p s and a positive integer @p n
 *                  in the serial number space UINT32_MAX
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.1">
 *                      3.1. Addition
 *                  </a>
 * @param[in]       s       sequence number
 * @param[in]       n       positive integer in the range of [0 .. 127]
 * @return          s + n, if valid
 * @return          s, if n is out of range
 */
static inline seq32_t seq32_add(seq32_t s, uint32_t n)
{
    return seq32_adds(s, n, UINT32_MAX);
}

/**
 * @brief           Increment a sequence number @p s by 1 in the serial number @p space
 * @param[in]       s       sequence number
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          s + 1
 */
static inline seq32_t seq32_incs(seq32_t s, uint32_t space)
{
    return seq32_adds(s, 1, space);
}

/**
 * @brief           Increment a sequence number @p s by 1 in the serial number space UINT32_MAX
 * @param[in]       s       sequence number
 * @return          s + 1
 */
static inline seq32_t seq32_inc(seq32_t s)
{
    return seq32_adds(s, 1, UINT32_MAX);
}

/**
 * @brief           Compare sequence numbers @p s1, @p s2 in the serial number @p space
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.2">
 *                      3.2. Comparison
 *                  </a>
 * @param[in]       s1      first sequence number
 * @param[in]       s2      second sequence number
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          -1, if s1 < s2
 * @return          0, if s1 == s2
 * @return          1, if s1 > s2
 * @return          -EINVAL, if comparison of the pair (s1,s2) is undefined
 */
int seq32_compares(seq32_t s1, seq32_t s2, uint32_t space);

/**
 * @brief           Compare sequence numbers @p s1, @p s2 in the serial number space UINT32_MAX
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.2">
 *                      3.2. Comparison
 *                  </a>
 * @param[in]       s1      first sequence number
 * @param[in]       s2      second sequence number
 * @return          -1, if s1 < s2
 * @return          0, if s1 == s2
 * @return          1, if s1 > s2
 * @return          -EINVAL, if comparison of the pair (s1,s2) is undefined
 */
static inline int seq32_compare(seq32_t s1, seq32_t s2)
{
    return seq32_compares(s1, s2, UINT32_MAX);
}

/**
 * @brief           Addition of a 64 bit sequence number @p s and a positive integer @p n
 *                  in the serial number @p space
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.1">
 *                      3.1. Addition
 *                  </a>
 * @param[in]       s       sequence number
 * @param[in]       n       positive integer in the range of [0 .. ((@p space / 2) - 1)]
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          s + n, if valid
 * @return          s, if n is out of range
 */
seq64_t seq64_adds(seq64_t s, uint64_t n, uint64_t space);

/**
 * @brief           Addition of a 64 bit sequence number @p s and a positive integer @p n
 *                  in the serial number space UINT64_MAX
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.1">
 *                      3.1. Addition
 *                  </a>
 * @param[in]       s       sequence number
 * @param[in]       n       positive integer in the range of [0 .. 127]
 * @return          s + n, if valid
 * @return          s, if n is out of range
 */
static inline seq64_t seq64_add(seq64_t s, uint64_t n)
{
    return seq64_adds(s, n, UINT64_MAX);
}

/**
 * @brief           Increment a sequence number @p s by 1 in the serial number @p space
 * @param[in]       s       sequence number
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          s + 1
 */
static inline seq64_t seq64_incs(seq64_t s, uint64_t space)
{
    return seq64_adds(s, 1, space);
}

/**
 * @brief           Increment a sequence number @p s by 1 in the serial number space UINT64_MAX
 * @param[in]       s       sequence number
 * @return          s + 1
 */
static inline seq64_t seq64_inc(seq64_t s)
{
    return seq64_adds(s, 1, UINT64_MAX);
}

/**
 * @brief           Compare sequence numbers @p s1, @p s2 in the serial number @p space
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.2">
 *                      3.2. Comparison
 *                  </a>
 * @param[in]       s1      first sequence number
 * @param[in]       s2      second sequence number
 * @param[in]       space   serial number space must be a power of 2 minus 1
 * @return          -1, if s1 < s2
 * @return          0, if s1 == s2
 * @return          1, if s1 > s2
 * @return          -EINVAL, if comparison of the pair (s1,s2) is undefined
 */
int seq64_compares(seq64_t s1, seq64_t s2, uint64_t space);

/**
 * @brief           Compare sequence numbers @p s1, @p s2 in the serial number space UINT64_MAX
 * @see             <a href="https://tools.ietf.org/html/rfc1982#section-3.2">
 *                      3.2. Comparison
 *                  </a>
 * @param[in]       s1      first sequence number
 * @param[in]       s2      second sequence number
 * @return          -1, if s1 < s2
 * @return          0, if s1 == s2
 * @return          1, if s1 > s2
 * @return          -EINVAL, if comparison of the pair (s1,s2) is undefined
 */
static inline int seq64_compare(seq64_t s1, seq64_t s2)
{
    return seq64_compares(s1, s2, UINT64_MAX);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SEQ_H */
