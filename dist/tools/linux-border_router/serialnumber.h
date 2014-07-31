/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file    serialnumber.h
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @brief   Serial number arithmetics after RFC 1982, section 3
 */

#ifndef SERIALNUMBER_H
#define SERIALNUMBER_H
#include <stdint.h>

/**
 * @brief Results for the serial number comparisson.
 */
typedef enum serial_comp_res_t {
    LESS, EQUAL, GREATER, UNDEF
} serial_comp_res_t;

/**
 * @brief   Addition for 8-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC 1982, section 3.1).
 * @param[in]   s   first summand in [0 .. 2^8 - 1].
 * @param[in]   n   second summand in [0 .. 2^7 - 1].
 * @return  Sum corresponding RFC1982 section 3.1 if <em>n</em> in [0 .. 2^7 - 1] or
 *          -1 if <em>n</em> not in [0 .. 2^7 - 1].
 **/
int serial_add8(uint8_t s, uint8_t n);

/**
 * @brief   Addition for 16-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC 1982, section 3.1).
 * @param[in]   s   first summand in [0 .. 2^16 - 1].
 * @param[in]   n   second summand in [0 .. 2^15 - 1].
 * @return  Sum corresponding RFC 1982 section 3.1 if <em>n</em> in [0 .. 2^15 - 1] or
 *          -1 if <em>n</em> not in [0 .. 2^15 - 1].
 **/
int serial_add16(uint16_t s, uint16_t n);

/**
 * @brief   Addition for 32-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC1982, section 3.1).
 * @param[in]   s   first summand in [0 .. 2^32 - 1].
 * @param[in]   n   second summand in [0 .. 2^31 - 1].
 * @return  Sum corresponding RFC 1982 section 3.1 if <em>n</em> in [0 .. 2^31 - 1] or
 *          -1 if <em>n</em> not in [0 .. 2^31 - 1].
 **/
int serial_add32(uint32_t s, uint32_t n);

/**
 * @brief   Comparison of 8-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC 1982, section 3.2).
 * @param[in]   s1   first argument.
 * @param[in]   s2   second argument.
 * @return  <tt>LESS</tt> if <em>s1</em> < <em>s2</em>.
 *          <tt>EQUAL</tt> if <em>s1</em> = <em>s2</em>.
 *          <tt>GREATER</tt> if <em>s1</em> > <em>s2</em>.
 *          else <tt>UNDEF</tt> (see RFC 1982, section 3.2).
 **/
serial_comp_res_t serial_comp8(uint8_t s1, uint8_t s2);

/**
 * @brief   Comparison of 16-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC 1982, section 3.2).
 * @param[in]   s1   first argument.
 * @param[in]   s2   second argument.
 * @return  <tt>LESS</tt> if <em>s1</em> < <em>s2</em>.
 *          <tt>EQUAL</tt> if <em>s1</em> = <em>s2</em>.
 *          <tt>GREATER</tt> if <em>s1</em> > <em>s2</em>.
 *          else <tt>UNDEF</tt> (see RFC 1982, section 3.2).
 **/
serial_comp_res_t serial_comp16(uint16_t s1, uint16_t s2);

/**
 * @brief   Comparison of 32-bit unsigned integers in serial number
 *          arithmetics (corresponding RFC1982, section 3.2).
 * @param[in]   s1   first argument.
 * @param[in]   s2   second argument.
 * @return  <tt>LESS</tt> if <em>s1</em> < <em>s2</em>.
 *          <tt>EQUAL</tt> if <em>s1</em> = <em>s2</em>.
 *          <tt>GREATER</tt> if <em>s1</em> > <em>s2</em>.
 *          else <tt>UNDEF</tt> (see RFC 1982, section 3.2).
 **/
serial_comp_res_t serial_comp32(uint32_t s1, uint32_t s2);

#endif /* SERIALNUMBER_H*/
