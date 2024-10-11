/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nrf5x_nrfmin_gnrc GNRC adapter for nrfmin
 * @ingroup     drivers_nrf5x_nrfmin
 * @brief       Minimal driver for the NRF51 radio
 *
 * @{
 *
 * @file
 * @brief       GNRC adapter for nrfmin devices (e.g. nRF5x radios)
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NRFMIN_GNRC_H
#define NRFMIN_GNRC_H

#include "nrfmin.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the nrfmin GNRC adapter, also takes care of the nrfmin
 *          driver setup
 *
 * As we have never more than 1 nrfmin device on a board, we can make some
 * simplifications when it come to allocating device descriptors and adapter
 * data structures -> we do this right in the driver/adapter code, so this
 * function can be called from auto_init as is, without the need for external
 * memory allocation.
 */
void gnrc_netdev_nrfmin_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NRFMIN_GNRC_H */
