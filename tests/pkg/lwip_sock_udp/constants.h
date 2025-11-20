/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @brief
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

#define _TEST_PORT_LOCAL    (0x2c94)
#define _TEST_PORT_REMOTE   (0xa615)
#define _TEST_NETIF         (1)
#define _TEST_TIMEOUT       (1000000U)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define _TEST_ADDR4_LOCAL  (0x964fa8c0U)   /* 192.168.79.150 */
# define _TEST_ADDR4_REMOTE (0x6b4fa8c0U)   /* 192.168.79.107 */
# define _TEST_ADDR4_WRONG  (0x4c6b4c25U)
# define _TEST_ADDR4_MASK   (0x00ffffffU)   /* 255.255.255.0 */
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
# define _TEST_ADDR4_LOCAL  (0xc0a84f96U)   /* 192.168.79.150 */
# define _TEST_ADDR4_REMOTE (0xc0a84f6bU)   /* 192.168.79.107 */
# define _TEST_ADDR4_WRONG  (0x254c6b4cU)
# define _TEST_ADDR4_MASK   (0xffffff00U)   /* 255.255.255.0 */
#else
# error "Byte order is neither little nor big!"
#endif
#define _TEST_ADDR4_GW      (0UL)           /* so we can test unreachability */
#define _TEST_ADDR6_LOCAL   { 0x2f, 0xc4, 0x11, 0x5a, 0xe6, 0x91, 0x8d, 0x5d, \
                              0x8c, 0xd1, 0x47, 0x07, 0xb7, 0x6f, 0x9b, 0x48 }
#define _TEST_ADDR6_REMOTE  { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                              0x93, 0xcf, 0x11, 0xe1, 0x72, 0x44, 0xc5, 0x9d }
#define _TEST_ADDR6_WRONG   { 0x2a, 0xce, 0x5d, 0x4e, 0xc8, 0xbf, 0x86, 0xf7, \
                              0x85, 0x49, 0xb4, 0x19, 0xf2, 0x28, 0xde, 0x9b }

#ifdef __cplusplus
}
#endif
