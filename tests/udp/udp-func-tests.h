/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  udptest
 * @{
 *
 * @file
 * @brief   UDP function tests definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef UDP_FUNC_TESTS_H_
#define UDP_FUNC_TESTS_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generates tests for UDP functions.
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *udp_func_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* UDP_FUNC_TESTS_H_ */
/** @} */
