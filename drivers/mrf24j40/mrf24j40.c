/*
 * SPDX-FileCopyrightText: 2017 Neo Nenaco <neo@nenaco.de>
 * SPDX-FileCopyrightText: 2017 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Implementation of public functions for MRF24J40 drivers
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Neo Nenaco <neo@nenaco.de>
 *
 * @}
 */

#include "byteorder.h"
#include "mrf24j40_registers.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_netdev.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int mrf24j40_reset(mrf24j40_t *dev)
{
    int res = mrf24j40_init_hw(dev);

    if (res < 0) {
        return res;
    }

    /* configure Immediate Sleep and Wake-Up mode */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_WAKECON, MRF24J40_WAKECON_IMMWAKE);

    /* set default options */
    mrf24j40_set_option(dev, NETDEV_IEEE802154_ACK_REQ, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_CSMA, true);

    mrf24j40_reset_tasks(dev);
    DEBUG("mrf24j40_reset(): reset complete.\n");

    return 0;
}

size_t mrf24j40_tx_load(mrf24j40_t *dev, uint8_t *data, size_t len, size_t offset)
{

    DEBUG("[mrf24j40] TX_load\n");

    mrf24j40_tx_normal_fifo_write(dev, MRF24J40_TX_NORMAL_FIFO + offset + 2, data, len);
    return offset + len;
}
