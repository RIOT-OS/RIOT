/*
 * Copyright (C) Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Common header for sx1276 tests
 *
 * @author  Eugene P. [ep@unwds.com]
 */
#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#include "sx1276.h"

#ifdef __cplusplus
extern "C" {
#endif


sx1276_t sx1276;

void init_radio(void);

/**
 * @brief   Application-internal functions and variables for sx1276 tests
 * @internal
 * @{
 */

int test1(int argc, char **argv);
int test2(int argc, char **argv);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H_ */
/** @} */
