/*
 * Copyright (C) 2020 HAW Hamburg
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
 * @brief   Common header for ieee802154_hal tests.
 *
 * @author  José I. Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef COMMON_H
#define COMMON_H

#include "net/ieee802154/radio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Application-internal functions and variables for ieee802154_hal tests
 * @internal
 * @{
 */
void ieee802154_hal_test_init_devs(void);
ieee802154_dev_t *ieee802154_hal_test_get_dev(int id);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */
/** @} */
