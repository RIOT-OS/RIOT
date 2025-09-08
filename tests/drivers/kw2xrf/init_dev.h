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
 * @brief   Device-specific test header file KW2XRF IEEE 802.15.4 device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "kernel_defines.h"
#include "kw2xrf_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KW2XRF_NUM                          ARRAY_SIZE(kw2xrf_params)
#define NETDEV_IEEE802154_MINIMAL_NUMOF     KW2XRF_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
