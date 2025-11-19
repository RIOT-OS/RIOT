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
 * @brief   Device-specific test header file AT86RF215 IEEE 802.15.4 device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "at86rf215_params.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AT86RF215_NUM                   ARRAY_SIZE(at86rf215_params)
/* AT86RF215 contains one sub-GHz and one 2.4 GHz driver */
#define NETDEV_IEEE802154_MINIMAL_NUMOF (2 * AT86RF215_NUM)

#ifdef __cplusplus
}
#endif

/** @} */
