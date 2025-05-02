/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#define _TEST_NETIF         (0)
#define _TEST_PORT_LOCAL    (0x0022)
#define _TEST_PORT_REMOTE   (0x0033)
#define _TEST_TIMEOUT       (1000000U)
#define _TEST_ADDR_LOCAL    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5b, 0x48 }
#define _TEST_ADDR_REMOTE   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xf7 }
#define _TEST_ADDR_WRONG    { 0x2a, 0xce, 0x5d, 0x4e, 0xc8, 0xbf, 0x86, 0xf7, \
                              0x85, 0x49, 0xb4, 0x19, 0xf2, 0x28, 0xde, 0x9b }

#ifdef __cplusplus
}
#endif

#endif /* CONSTANTS_H */
