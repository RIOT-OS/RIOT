/*
 * Copyright (C) 2015  Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file        tests-crypto-nlsv2.h
 * @brief       Test vectors for the NLSv2 cipher unittests
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 */

#ifndef __TESTS_CRYPTO_NLSV2_H_
#define __TESTS_CRYPTO_NLSV2_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 100

/* NLSv2 test vectors */

#define TV1_KEY "test key 128bits"
#define TV1_NONCE "\0\0\0\0"
#define TV1_EXPECT \
"\x98\x24\x4b\xf3\x22\x43\xbc\x5d\x0b\x73\x63\xd4\x8b\x92\x15\xf1\xc0\x91\x34\x7f"

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_NLSV2_H_ */
/** @} */
