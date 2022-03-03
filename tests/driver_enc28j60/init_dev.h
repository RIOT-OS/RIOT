/*
 * Copyright (C) 2022 HAW Hamburg
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
 * @brief   Device-specific test header file ENC28J60 ethernet device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */
#ifndef INIT_DEV_H
#define INIT_DEV_H

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

#endif /* INIT_DEV_H */
/** @} */
