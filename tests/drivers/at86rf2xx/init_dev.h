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
 * @brief   Device-specific test header file AT86RF2XX IEEE 802.15.4 device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */
#ifndef INIT_DEV_H
#define INIT_DEV_H

#include "at86rf2xx_params.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AT86RF2XX_NUM                   ARRAY_SIZE(at86rf2xx_params)
#define NETDEV_IEEE802154_MINIMAL_NUMOF AT86RF2XX_NUM

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* INIT_DEV_H */
