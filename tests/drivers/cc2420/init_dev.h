/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Device-specific test header file CC2420 IEEE 802.15.4 device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "cc2420_params.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC2420_NUM                      ARRAY_SIZE(cc2420_params)
#define NETDEV_IEEE802154_MINIMAL_NUMOF CC2420_NUM

#ifdef __cplusplus
}
#endif

/** @} */
