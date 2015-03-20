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
 * @brief       Test vectors for the convertion between c25510 and ed25519
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 *
 */

#ifndef __TESTS_CRYPTO_MORPH25519_H_
#define __TESTS_CRYPTO_MORPH25519_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Morph25519 test vectors */
#define TV1_RANDOM \
"\x3a\x54\x76\x68\xe8\x59\xfb\x7b\x11\x2a\x1e\x2d\xd7\xef\xcb\x73" \
"\x91\x76\xab\x8c\xfd\x1d\x9f\x22\x48\x47\xfc\xe3\x62\xeb\xd9\x9c"
#define TV2_RANDOM \
"\x3a\x46\xd1\x21\xd8\xe6\x49\xbf\x21\x58\x53\x8e\x03\x39\xc4\x28" \
"\x8f\x44\xb9\xdc\x83\x8d\x1a\xa9\x6a\xc9\xe6\xf7\x05\xf3\xdd\x40"
#define TV3_RANDOM \
"\x5d\xe4\xfd\x9d\x01\x7a\x58\xa0\x8c\x8c\xbd\x6d\x05\x5e\xb1\xec" \
"\xab\x89\xf7\x51\x99\xcf\x5d\x45\xe1\xcf\xbc\xb3\xe3\x1f\xa8\x40"

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_MORPH25519_H_ */
/** @} */
