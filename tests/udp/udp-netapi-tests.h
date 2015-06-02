/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    udptest UDP tests
 * @brief       Tests @ref net_ng_udp using @ref net_ng_nettest and
 *              @ref unittests
 * @{
 *
 * @file
 * @brief   UDP netapi tests definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef UDP_NETAPI_TESTS_H_
#define UDP_NETAPI_TESTS_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief   Initializes UDP netapi tests globally.
*
* @return  0 on success.
* @return  a negative error code on error.
*/
int udp_netapi_tests_init(void);

/**
* @brief   Generates tests for UDP netapi interface.
*
* @return  embUnit tests if successful, NULL if not.
*/
Test *udp_netapi_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* UDP_NETAPI_TESTS_H_ */
/** @} */
