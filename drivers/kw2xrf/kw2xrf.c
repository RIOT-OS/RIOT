/*
 * SPDX-FileCopyrightText: 2016 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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

void kw2xrf_reset_phy(kw2xrf_t *dev)
{
    dev->tx_power = KW2XRF_DEFAULT_TX_POWER;
    kw2xrf_set_tx_power(dev, dev->tx_power);

    kw2xrf_set_channel(dev, KW2XRF_DEFAULT_CHANNEL);

    kw2xrf_set_address(dev);

    kw2xrf_set_cca_mode(dev, 1);

    kw2xrf_set_rx_watermark(dev, 1);

    if (!IS_ACTIVE(CONFIG_IEEE802154_AUTO_ACK_DISABLE)) {
        kw2xrf_set_option(dev, KW2XRF_OPT_AUTOACK, true);
    }

    kw2xrf_set_option(dev, KW2XRF_OPT_ACK_REQ, true);
    kw2xrf_set_option(dev, KW2XRF_OPT_AUTOCCA, true);

    kw2xrf_set_power_mode(dev, KW2XRF_AUTODOZE);
    kw2xrf_set_sequence(dev, dev->idle_state);

    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2, MKW2XDM_PHY_CTRL2_SEQMSK);

    kw2xrf_enable_irq_b(dev);

    DEBUG("[kw2xrf] init phy and (re)set to channel %d.\n",
          KW2XRF_DEFAULT_CHANNEL);
}
