/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "net/dev_eth.h"
#include "dev_eth_autoinit.h"

#ifdef MODULE_DEV_ETH_TAP
#include "dev_eth_tap.h"
#endif

#ifdef MODULE_ENCX24J600
#include "encx24j600.h"
encx24j600_t dev_eth_encx24j600;
#endif

dev_eth_t * const dev_eth_devices[] = {
#ifdef MODULE_DEV_ETH_TAP
    [DEV_ETH_TAP] = (dev_eth_t*)&dev_eth_tap,
#endif
#ifdef MODULE_ENCX24J600
    [DEV_ETH_ENCX24J600] = (dev_eth_t*)&dev_eth_encx24j600,
#endif
};

void dev_eth_autoinit(void)
{
#ifdef MODULE_ENCX24J600
    /* TODO: use sensible defines */
    encx24j600_setup(&dev_eth_encx24j600, SPI_0, GPIO_1, GPIO_2);
#endif
}
