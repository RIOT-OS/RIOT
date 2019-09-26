/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @brief       Auto initialize network interfaces
 *
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef AUTO_INIT_NETIF_H
#define AUTO_INIT_NETIF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#define AT86RF2XX_NUM ARRAY_SIZE(at86rf2xx_params)
#else
#define AT86RF2XX_NUM (0)
#endif

#ifdef MODULE_CC2420
#include "cc2420_params.h"
#define CC2420_NUM ARRAY_SIZE(cc2420_params)
#else
#define CC2420_NUM (0)
#endif

#ifdef MODULE_KW2XRF
#include "kw2xrf_params.h"
#define KW2XRF_NUM ARRAY_SIZE(kw2xrf_params)
#else
#define KW2XRF_NUM (0)
#endif

#ifdef MODULE_MRF24J40
#include "mrf24j40_params.h"
#define MRF24J40_NUM ARRAY_SIZE(mrf24j40_params)
#else
#define MRF24J40_NUM (0)
#endif

#ifdef MODULE_SLIPDEV
#include "slipdev_params.h"
#define SLIPDEV_NUM ARRAY_SIZE(slipdev_params)
#else
#define SLIPDEV_NUM (0)
#endif

#ifdef MODULE_SX127X
#include "sx127x_params.h"
#define SX127X_NUM ARRAY_SIZE(sx127x_params)
#else
#define SX127X_NUM (0)
#endif

#ifdef MODULE_W5100
#include "w5100_params.h"
#define W5100_NUM ARRAY_SIZE(w5100_params)
#else
#define W5100_NUM (0)
#endif

#ifdef MODULE_XBEE
#include "xbee_params.h"
#define XBEE_NUM ARRAY_SIZE(xbee_params)
#else
#define XBEE_NUM (0)
#endif

#ifdef MODULE_NETDEV_TAP
#include "netdev_tap_params.h"
#define NETDEV_TAP_NUM (NETDEV_TAP_MAX)
#else
#define NETDEV_TAP_NUM (0)
#endif

#ifdef MODULE_SOCKET_ZEP
#include "socket_zep_params.h"
#define NETDEV_SOCKET_ZEP_NUM (SOCKET_ZEP_MAX)
#else
#define NETDEV_SOCKET_ZEP_NUM (0)
#endif

#ifdef MODULE_CC2538_RF
#define CC2538_RF_NUM (1)
#else
#define CC2538_RF_NUM (0)
#endif

#ifdef MODULE_CDCECM
#define CDCECM_NUM (1)
#else
#define CDCECM_NUM (0)
#endif

#ifdef MODULE_ETHOS
#define ETHOS_NUM (1)
#else
#define ETHOS_NUM (0)
#endif

#ifdef MODULE_NRF802154
#define NRF802154_NUM (1)
#else
#define NRF802154_NUM (0)
#endif

#ifdef MODULE_STM32_ETH
#define STM32_ETH_NUM (1)
#else
#define STM32_ETH_NUM (0)
#endif

#define GNRC_NETIF_NUMOF (AT86RF2XX_NUM + CC2420_NUM + KW2XRF_NUM + MRF24J40_NUM    \
                         + SLIPDEV_NUM + SX127X_NUM + W5100_NUM + XBEE_NUM          \
                         + CC2538_RF_NUM + CDCECM_NUM + ETHOS_NUM + NRF802154_NUM   \
                         + STM32_ETH_NUM + NETDEV_TAP_NUM + NETDEV_SOCKET_ZEP_NUM)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AUTO_INIT_NETIF_H */
