/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "luid.h"
#include "byteorder.h"
#include "net/gnrc.h"
#include "mrf24j40_registers.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_netdev.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void mrf24j40_setup(mrf24j40_t *dev, const mrf24j40_params_t *params)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &mrf24j40_driver;
    /* initialize device descriptor */
    dev->params = *params;
}

int mrf24j40_reset(mrf24j40_t *dev)
{
    eui64_t addr_long;

    int res = mrf24j40_init(dev);

    if (res < 0) {
        return res;
    }

    netdev_ieee802154_reset(&dev->netdev);

    /* get an 8-byte unique ID to use as hardware address */
    luid_get(addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN);
    addr_long.uint8[0] &= ~(0x01);
    addr_long.uint8[0] |=  (0x02);
    /* set short and long address */
    mrf24j40_set_addr_long(dev, ntohll(addr_long.uint64.u64));
    mrf24j40_set_addr_short(dev, ntohs(addr_long.uint16[0].u16));

    mrf24j40_set_chan(dev, IEEE802154_DEFAULT_CHANNEL);

    /* configure Immediate Sleep and Wake-Up mode */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_WAKECON, MRF24J40_WAKECON_IMMWAKE);

    /* set default options */
    mrf24j40_set_option(dev, IEEE802154_FCF_PAN_COMP, true);
    mrf24j40_set_option(dev, NETDEV_IEEE802154_SRC_MODE_LONG, true);
    mrf24j40_set_option(dev, NETDEV_IEEE802154_ACK_REQ, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_CSMA, true);

    /* go into RX state */
    mrf24j40_reset_tasks(dev);
    dev->state = 0;
    mrf24j40_set_state(dev, MRF24J40_PSEUDO_STATE_IDLE);
    DEBUG("mrf24j40_reset(): reset complete.\n");

    return 0;
}

bool mrf24j40_cca(mrf24j40_t *dev, int8_t *rssi)
{
    uint8_t tmp_ccaedth;
    uint8_t status;
    uint8_t tmp_rssi;

    mrf24j40_assert_awake(dev);
    mrf24j40_enable_lna(dev);

    /* trigger CCA measurement */
    /* take a look onto datasheet chapter 3.6.1 */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG6, MRF24J40_BBREG6_RSSIMODE1);
    /* wait for result to be ready */
    do {
        status = mrf24j40_reg_read_short(dev, MRF24J40_REG_BBREG6);
    } while (!(status & MRF24J40_BBREG2_RSSIRDY));
    mrf24j40_assert_sleep(dev);
    /* return according to measurement */
    tmp_ccaedth = mrf24j40_reg_read_short(dev, MRF24J40_REG_CCAEDTH);       /* Energy detection threshold */
    tmp_rssi = mrf24j40_reg_read_long(dev, MRF24J40_REG_RSSI);
    if (rssi != NULL) {
        *rssi = mrf24j40_dbm_from_reg(tmp_rssi);
    }

    mrf24j40_enable_auto_pa_lna(dev);

    if (tmp_rssi < tmp_ccaedth) {
        /* channel is clear */
        return true;            /* idle */
    }
    else {
        /* channel is busy */
        return false;
    }
}

void mrf24j40_tx_prepare(mrf24j40_t *dev)
{
    DEBUG("[mrf24j40] TX_Prepare, Current state: %x\n", dev->state);
    do {
        mrf24j40_update_tasks(dev);
    } while (!(dev->pending & MRF24J40_TASK_TX_DONE));
    mrf24j40_assert_awake(dev);
    dev->pending &= ~(MRF24J40_TASK_TX_DONE);
    dev->tx_frame_len = IEEE802154_FCS_LEN;
}

size_t mrf24j40_tx_load(mrf24j40_t *dev, uint8_t *data, size_t len, size_t offset)
{

    DEBUG("[mrf24j40] TX_load, Current state: %x\n", dev->state);
    dev->tx_frame_len += (uint8_t)len;

    mrf24j40_tx_normal_fifo_write(dev, MRF24J40_TX_NORMAL_FIFO + offset + 2, data, len);
    return offset + len;
}

void mrf24j40_tx_exec(mrf24j40_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;


    dev->tx_frame_len = dev->tx_frame_len - IEEE802154_FCS_LEN;
    /* write frame length field in FIFO */
    mrf24j40_tx_normal_fifo_write(dev, MRF24J40_TX_NORMAL_FIFO + 1, &(dev->tx_frame_len), 1);

    /* write header length to FIFO address 0x00 */
    /* from figure 3-11 datasheet: header length =      2 Bytes Frame Control
     *                                              +	1 Byte Seq. No.
     *                                              +	4 to 20 Bytes Addressing Fields
     */
    mrf24j40_reg_write_long(dev, MRF24J40_TX_NORMAL_FIFO, dev->header_len);

    if (dev->fcf_low & IEEE802154_FCF_ACK_REQ) {
        mrf24j40_reg_write_short(dev, MRF24J40_REG_TXNCON, MRF24J40_TXNCON_TXNACKREQ | MRF24J40_TXNCON_TXNTRIG);
    }
    else {
        mrf24j40_reg_write_short(dev, MRF24J40_REG_TXNCON, MRF24J40_TXNCON_TXNTRIG);
    }
    if (netdev->event_callback && (dev->netdev.flags & MRF24J40_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    }
}
