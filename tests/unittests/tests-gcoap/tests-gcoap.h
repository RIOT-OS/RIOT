/*
 * Copyright (c) 2016 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unit tests for the gcoap module
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */
#ifndef TESTS_GCOAP_H
#define TESTS_GCOAP_H

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 *
 * Includes simple success case tests to get a resource from a server, and
 * to provide a resource for a client.
 */
void tests_gcoap(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_GCOAP_H */
/** @} */
