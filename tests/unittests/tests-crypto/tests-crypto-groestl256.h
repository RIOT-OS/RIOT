/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
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
 * @brief       Test vectors for the Grøstl-256 hash function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 */

#ifndef __TESTS_CRYPTO_GROESTL256_H_
#define __TESTS_CRYPTO_GROESTL256_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Grøstl-256 test vectors */

#define TV1_TEXT	"abc"
#define TV1_TEXT_LEN	3 
#define TV1_HASH \
"\xf3\xc1\xbb\x19\xc0\x48\x80\x13\x26\xa7\xef\xbc\xf1\x6e\x3d\x78" \
"\x87\x44\x62\x49\x82\x9c\x37\x9e\x18\x40\xd1\xa3\xa1\xe7\xd4\xd2"

#define TV2_TEXT 	"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
#define TV2_TEXT_LEN	56 
#define TV2_HASH \
"\x22\xc2\x3b\x16\x0e\x56\x1f\x80\x92\x4d\x44\xf2\xcc\x59\x74\xcd" \
"\x5a\x1d\x36\xf6\x93\x24\x21\x18\x61\xe6\x3b\x9b\x6c\xb7\x97\x4c"

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_GROESTL256_H_ */
/** @} */
