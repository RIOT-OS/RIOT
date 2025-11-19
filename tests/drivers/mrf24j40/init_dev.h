/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Device-specific test header file MRF24J40 IEEE 802.15.4 device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "kernel_defines.h"
#include "mrf24j40_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MRF24J40_NUM                        ARRAY_SIZE(mrf24j40_params)
#define NETDEV_IEEE802154_MINIMAL_NUMOF     MRF24J40_NUM

#ifdef __cplusplus
}
#endif

/** @} */
