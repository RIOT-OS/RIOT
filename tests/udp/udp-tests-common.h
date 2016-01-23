/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup udptest
 * @{
 *
 * @file
 * @brief   Test value definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef UDP_TESTS_COMMON_H_
#define UDP_TESTS_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif


#define TEST_SRC_IPV6   { { 0xcb, 0xd0, 0x46, 0xa2, 0xe6, 0xde, 0xf0, 0xcf, \
                            0xa5, 0x29, 0xfb, 0x20, 0x14, 0x01, 0x64, 0x5e  } }
#define TEST_SRC_PORT   (26629)
#define TEST_DST_IPV6   { { 0xa2, 0xfd, 0x04, 0xfb, 0xde, 0xe6, 0x31, 0x2f, \
                            0xb3, 0xa9, 0xc3, 0x34, 0x9f, 0x40, 0x61, 0xe0  } }
#define TEST_DST_PORT   (38021)
#define TEST_DATA       "×b¼Ë8\x84pË¦ÞÃpv'\x96ÿëĀfþ\n\x82\x05·3<ÉR^V"
#define TEST_CSUM       (0xe35c)
#define TEST_CTX        (0)


#ifdef __cplusplus
}
#endif

#endif /* UDP_TESTS_COMMON_H_ */
/** @} */
