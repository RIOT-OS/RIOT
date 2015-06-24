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
 * @file        tests-crypto-rabbit.h
 * @brief       Test vectors for the Rabbit cipher unittests
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 */

#ifndef __TESTS_CRYPTO_RABBIT_H_
#define __TESTS_CRYPTO_RABBIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 48
#define EXPECT_SIZE 32

/* Rabbit Test vectors */

/* Test 1: Key setup and encryption/decryption/prng */
#define TV1_KEY \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define TV1_EXPECT \
"\x02\xF7\x4A\x1C\x26\x45\x6B\xF5\xEC\xD6\xA5\x36\xF0\x54\x57\xB1" \
"\xA7\x8A\xC6\x89\x47\x6C\x69\x7B\x39\x0C\x9C\xC5\x15\xD8\xE8\x88" \
"\xEF\x9A\x69\x71\x8B\x82\x49\xA1\xA7\x3C\x5A\x6E\x5B\x90\x45\x95"

/* Test 2: Key setup and encryption/decryption/prng */
#define TV2_KEY \
"\xC2\x1F\xCF\x38\x81\xCD\x5E\xE8\x62\x8A\xCC\xB0\xA9\x89\x0D\xF8"
#define TV2_EXPECT \
"\x3D\x02\xE0\xC7\x30\x55\x91\x12\xB4\x73\xB7\x90\xDE\xE0\x18\xDF" \
"\xCD\x6D\x73\x0C\xE5\x4E\x19\xF0\xC3\x5E\xC4\x79\x0E\xB6\xC7\x4A" \
"\x9F\xB4\x92\xE1\xB5\x40\x36\x3A\xE3\x83\xC0\x1F\x9F\xA2\x26\x1A"

/* Test 3: Key setup and encryption/decryption/prng */
#define TV3_KEY \
"\x1D\x27\x2C\x6A\x2D\x8E\x3D\xFC\xAC\x14\x05\x6B\x78\xD6\x33\xA0"
#define TV3_EXPECT \
"\xA3\xA9\x7A\xBB\x80\x39\x38\x20\xB7\xE5\x0C\x4A\xBB\x53\x82\x3D" \
"\xC4\x42\x37\x99\xC2\xEF\xC9\xFF\xB3\xA4\x12\x5F\x1F\x4C\x99\xA8" \
"\x97\xC0\x73\x3F\xF1\xF1\x8D\x25\x6A\x59\xE2\xBA\xAB\xC1\xF4\xF1"

/* Test 4: Key setup, iv setup and encryption/decryption/prng */
#define TV4_KEY \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define TV4_IV \
"\x00\x00\x00\x00\x00\x00\x00\x00"
#define TV4_EXPECT \
"\xED\xB7\x05\x67\x37\x5D\xCD\x7C\xD8\x95\x54\xF8\x5E\x27\xA7\xC6" \
"\x8D\x4A\xDC\x70\x32\x29\x8F\x7B\xD4\xEF\xF5\x04\xAC\xA6\x29\x5F" \
"\x66\x8F\xBF\x47\x8A\xDB\x2B\xE5\x1E\x6C\xDE\x29\x2B\x82\xDE\x2A"

/* Test 5: Key setup, iv setup and encryption/decryption/prng */
#define TV5_KEY \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define TV5_IV \
"\x59\x7E\x26\xC1\x75\xF5\x73\xC3"
#define TV5_EXPECT \
"\x6D\x7D\x01\x22\x92\xCC\xDC\xE0\xE2\x12\x00\x58\xB9\x4E\xCD\x1F" \
"\x2E\x6F\x93\xED\xFF\x99\x24\x7B\x01\x25\x21\xD1\x10\x4E\x5F\xA7" \
"\xA7\x9B\x02\x12\xD0\xBD\x56\x23\x39\x38\xE7\x93\xC3\x12\xC1\xEB"

/* Test 6: Key setup, iv setup and encryption/decryption/prng */
#define TV6_KEY \
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define TV6_IV \
"\x27\x17\xF4\xD2\x1A\x56\xEB\xA6"
#define TV6_EXPECT \
"\x4D\x10\x51\xA1\x23\xAF\xB6\x70\xBF\x8D\x85\x05\xC8\xD8\x5A\x44" \
"\x03\x5B\xC3\xAC\xC6\x67\xAE\xAE\x5B\x2C\xF4\x47\x79\xF2\xC8\x96" \
"\xCB\x51\x15\xF0\x34\xF0\x3D\x31\x17\x1C\xA7\x5F\x89\xFC\xCB\x9F"

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_RABBIT_H_ */
/** @} */
