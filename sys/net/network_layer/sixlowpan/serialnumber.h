/*
 * Copyright 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * Header file for serial number arithmetics [RFC1982]
 */

#ifndef _SIXLOWPAN_SERIALNUMBER_H
#define _SIXLOWPAN_SERIALNUMBER_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum serial_comp_res_t {
    LESS = 0,
    EQUAL = 1,
    GREATER = 2,
    UNDEF = 3,
} serial_comp_res_t;

/**
 * @brief   Addition for 8-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC1982 section 3.1).
 * @param[in]   s   first summand in [0 .. 2^8 - 1].
 * @param[in]   n   second summand in [0 .. 2^7 - 1].
 * @return sum corresponding RFC1982 section 3.1 if n in [0 .. 2^7 - 1] or
 *          -1 if n not in [0 .. 2^7 - 1].
 **/
int serial_add8(uint8_t s, uint8_t n);

/**
 * @brief   Addition for 16-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC1982 section 3.1).
 * @param[in]   s   first summand in [0 .. 2^16 - 1].
 * @param[in]   n   second summand in [0 .. 2^15 - 1].
 * @return sum corresponding RFC1982 section 3.1 if n in [0 .. 2^15 - 1] or
 *          -1 if n not in [0 .. 2^15 - 1].
 **/
int serial_add16(uint16_t s, uint16_t n);

/**
 * @brief   Addition for 32-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC1982 section 3.1).
 * @param[in]   s   first summand in [0 .. 2^32 - 1].
 * @param[in]   n   second summand in [0 .. 2^31 - 1].
 * @return sum corresponding RFC1982 section 3.1 if n in [0 .. 2^31 - 1] or
 *          -1 if n not in [0 .. 2^31 - 1].
 **/
int serial_add32(uint32_t s, uint32_t n);

/**
 * @brief   Comparison of 8-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC1982 section 3.2).
 * @param[in]   s1   first argument.
 * @param[in]   s2   second argument.
 * @return  LESS if s1 < s2.
 *          EQUAL if s1 = s2.
 *          GREATER if s1 > s2.
 *          else UNDEF (see RFC1982 section 3.2).
 **/
serial_comp_res_t serial_comp8(uint8_t s1, uint8_t s2);

/**
 * @brief   Comparison of 16-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC1982 section 3.2).
 * @param[in]   s1   first argument.
 * @param[in]   s2   second argument.
 * @return  LESS if s1 < s2.
 *          EQUAL if s1 = s2.
 *          GREATER if s1 > s2.
 *          else UNDEF (see RFC1982 section 3.2).
 **/
serial_comp_res_t serial_comp16(uint16_t s1, uint16_t s2);

/**
 * @brief   Comparison of 32-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC1982 section 3.2).
 * @param[in]   s1   first argument.
 * @param[in]   s2   second argument.
 * @return  LESS if s1 < s2.
 *          EQUAL if s1 = s2.
 *          GREATER if s1 > s2.
 *          else UNDEF (see RFC1982 section 3.2).
 **/
serial_comp_res_t serial_comp32(uint32_t s1, uint32_t s2);

#ifdef __cplusplus
}
#endif

#endif /* _SIXLOWPAN_SERIALNUMBER_H*/
