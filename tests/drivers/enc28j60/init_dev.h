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
 * @brief   Device-specific test header file ENC28J60 ethernet device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdint.h>

#include "kernel_defines.h"
#include "net/netdev.h"

#include "enc28j60.h"
#include "enc28j60_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENC28J60_NUM   ARRAY_SIZE(enc28j60_params)
#define NETDEV_ETH_MINIMAL_NUMOF      ENC28J60_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
