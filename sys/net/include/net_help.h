/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_help Net help
 * @ingroup     net
 * @brief       Helper functions for networking as byte order conversions and checksum calculations
 * @{
 *
 * @file
 *
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef __NET_HELP_H
#define __NET_HELP_H

#include <string.h>
#include <stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BITSET(var,pos) ((var) & (1<<(pos)))

/**
 * @brief   Computes the Internet Checksum for *buf* with initial value *init*
 *
 * @see <a href="https://tools.ietf.org/html/rfc1071">
 *          RFC 1071
 *      </a>
 *
 * @param[in] init      Initial value for the checksum (0 in most cases)
 * @param[in] buf       A byte array to calculate the checksum of
 * @param[in] buf_len   Length of *buf*
 *
 * @return  The Internet Checksum of *buf* with initial value *init*
 */
uint16_t net_help_csum(uint16_t init, uint8_t *buf, uint16_t buf_len);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __NET_HELP_H */
