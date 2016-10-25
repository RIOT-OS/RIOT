/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   xbee driver parameters example, used by auto_init_gnrc_netif
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef XBEE_PARAMS_H
#define XBEE_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

static xbee_params_t xbee_params[] = {
    { .uart = XBEE_UART,
      .baudrate = 9600U,
      .sleep_pin = GPIO_UNDEF,
      .reset_pin = GPIO_UNDEF
    },
};

#ifdef __cplusplus
}
#endif
#endif /* XBEE_PARAMS_H */
/** @} */
