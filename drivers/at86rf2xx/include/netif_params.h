/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_netif
 * @brief
 * @{
 *
 * @file
 * @brief       Network interface parameter file.
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NETIF_PARAMS_H
#define NETIF_PARAMS_H

#include "net/netif.h"
#include "at86rf2xx_params.h"

#ifdef __cplusplus
extern "C" {
#endif

static const netif_params_t netif_params[] =
{
    { &at86rf2xx_params[0],
#ifdef MODULE_GNRC
      NETIF_TYPE_GNRC_DEFAULT,
#endif
      "WP0"
    }
};

#ifdef __cplusplus
}
#endif

#endif /* NETIF_PARAMS_H */
/** @} */
