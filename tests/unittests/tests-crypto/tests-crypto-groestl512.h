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
 * @brief       Test vectors for the Grøstl-512 hash function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 */

#ifndef __TESTS_CRYPTO_GROESTL512_H_
#define __TESTS_CRYPTO_GROESTL512_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Grøstl-512 test vectors */

#define TV1_TEXT "abc"
#define TV1_TEXT_LEN 3 
#define TV1_HASH \
"\x70\xe1\xc6\x8c\x60\xdf\x3b\x65\x53\x39\xd6\x7d\xc2\x91\xcc\x3f" \
"\x1d\xde\x4e\xf3\x43\xf1\x1b\x23\xfd\xd4\x49\x57\x69\x38\x15\xa7" \
"\x5a\x83\x39\xc6\x82\xfc\x28\x32\x25\x13\xfd\x1f\x28\x3c\x18\xe5" \
"\x3c\xff\x2b\x26\x4e\x06\xbf\x83\xa2\xf0\xac\x8c\x1f\x6f\xbf\xf6"

#define TV2_TEXT \
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqopqrpqr" \
"sqrstrstustuvtuvwuvwxvwxywxyzxyzayzabzabcabcdbcdecdefdefg"
#define TV2_TEXT_LEN 120 
#define TV2_HASH \
"\x9f\x08\x67\xf9\x41\xb5\xf3\xf2\x52\x0e\x7b\x60\xb6\xe6\x15\xec" \
"\xa8\x2b\x61\xe2\xc5\xdd\x81\x0f\x56\x24\x50\x46\x6f\x6a\x80\xfd" \
"\x72\xe6\x39\x1f\x82\x9d\xea\x65\x6c\x4f\x84\xcd\xd7\x61\x5e\x20" \
"\x98\xa9\x93\x36\xd3\x30\xb7\x22\x62\x99\xe4\x13\x9d\x3d\xef\x75"

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_GROESTL512_H_ */
/** @} */
