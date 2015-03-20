/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Test vectors for the implementation of the Edwards-curve equivalent of Curve25519 
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 *
 */

#ifndef __TESTS_CRYPTO_ED25519_H_
#define __TESTS_CRYPTO_ED25519_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Ed25519 test vectors */
#define TV1_E \
"\x67\xc6\x69\x73\x51\xff\x4a\xec\x29\xcd\xba\xab\xf2\xfb\xe3\x46" \
"\x7c\xc2\x54\xf8\x1b\xe8\xe7\x8d\x76\x5a\x2e\x63\x33\x9f\xc9\x9a"

#define TV2_E \
"\x66\x32\x0d\xb7\x31\x58\xa3\x5a\x25\x5d\x05\x17\x58\xe9\x5e\xd4" \
"\xab\xb2\xcd\xc6\x9b\xb4\x54\x11\x0e\x82\x74\x41\x21\x3d\xdc\x87"

static const uint8_t ed25519_order[ED25519_EXPONENT_SIZE] = {
    0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
    0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

static const uint8_t curve_d[F25519_SIZE] = {
    0xa3, 0x78, 0x59, 0x13, 0xca, 0x4d, 0xeb, 0x75,
    0xab, 0xd8, 0x41, 0x41, 0x4d, 0x0a, 0x70, 0x00,
    0x98, 0xe8, 0x79, 0x77, 0x79, 0x40, 0xc7, 0x8c,
    0x73, 0xfe, 0x6f, 0x2b, 0xee, 0x6c, 0x03, 0x52
};

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_ED25519_H_ */
/** @} */
