/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 * @file
 * @brief       Basic functionality of kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */
#include <stdint.h>
#include <string.h>

#include "log.h"
#include "mutex.h"
#include "msg.h"
#include "periph/gpio.h"
#include "periph/cpuid.h"
#include "net/gnrc.h"
#include "net/ieee802154.h"
#include "luid.h"

#include "kw2xrf.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_netdev.h"
#include "kw2xrf_getset.h"
#include "kw2xrf_intern.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void kw2xrf_set_address(kw2xrf_t *dev)
{
    DEBUG("[kw2xrf] set MAC addresses\n");
    eui64_t addr_long;
    /* get an 8-byte unique ID to use as hardware address */
    luid_get(addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN);
    /* make sure we mark the address as non-multicast and not globally unique */
    addr_long.uint8[0] &= ~(0x01);
    addr_long.uint8[0] |=  (0x02);
    /* set short and long address */
    kw2xrf_set_addr_long(dev, ntohll(addr_long.uint64.u64));
    kw2xrf_set_addr_short(dev, ntohs(addr_long.uint16[0].u16));
}

void kw2xrf_setup(kw2xrf_t *dev, const kw2xrf_params_t *params)
{
    netdev_t *netdev = &dev->netdev.netdev;

    netdev->driver = &kw2xrf_driver;
    /* initialize device descriptor */
    dev->params = *params;
    dev->idle_state = XCVSEQ_RECEIVE;
    dev->state = 0;
    dev->pending_tx = 0;
    kw2xrf_spi_init(dev);
    kw2xrf_set_power_mode(dev, KW2XRF_IDLE);
    DEBUG("[kw2xrf] enabling RX/TX completion and start events");
    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2, MKW2XDM_PHY_CTRL2_RX_WMRK_MSK);
    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2, MKW2XDM_PHY_CTRL2_RXMSK);
    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2, MKW2XDM_PHY_CTRL2_TXMSK);
    DEBUG("[kw2xrf] setup finished\n");
}

int kw2xrf_init(kw2xrf_t *dev, gpio_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }

    kw2xrf_set_out_clk(dev);
    kw2xrf_disable_interrupts(dev);
    /* set up GPIO-pin used for IRQ */
    gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING, cb, dev);

    kw2xrf_abort_sequence(dev);
    kw2xrf_update_overwrites(dev);
    kw2xrf_timer_init(dev, KW2XRF_TIMEBASE_62500HZ);
    DEBUG("[kw2xrf] init finished\n");

    return 0;
}

void kw2xrf_reset_phy(kw2xrf_t *dev)
{
    netdev_ieee802154_reset(&dev->netdev);

    dev->tx_power = KW2XRF_DEFAULT_TX_POWER;
    kw2xrf_set_tx_power(dev, dev->tx_power);

    kw2xrf_set_channel(dev, KW2XRF_DEFAULT_CHANNEL);

    kw2xrf_set_address(dev);

    kw2xrf_set_cca_mode(dev, 1);

    kw2xrf_set_rx_watermark(dev, 1);

    kw2xrf_set_option(dev, KW2XRF_OPT_AUTOACK, true);
    kw2xrf_set_option(dev, KW2XRF_OPT_ACK_REQ, true);
    kw2xrf_set_option(dev, KW2XRF_OPT_AUTOCCA, true);

    kw2xrf_set_power_mode(dev, KW2XRF_AUTODOZE);
    kw2xrf_set_sequence(dev, dev->idle_state);

    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2, MKW2XDM_PHY_CTRL2_SEQMSK);

    kw2xrf_enable_irq_b(dev);

    DEBUG("[kw2xrf] init phy and (re)set to channel %d.\n",
          KW2XRF_DEFAULT_CHANNEL);
}
