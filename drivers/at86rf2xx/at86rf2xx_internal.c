/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *              called in functions of a specific device type
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "kernel_defines.h"
#include "luid.h"
#include "xtimer.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_internal_common.h"
#include "at86rf2xx_netdev.h"

void at86rf2xx_setup(at86rf2xx_t *dev)
{
    assert(dev->base.dev_type < AT86RF2XX_DEV_TYPE_NUM_OF);
    netdev_t *netdev = (netdev_t *)dev;
    netdev->driver = &at86rf2xx_driver;
    /* State to return after receiving or transmitting */
    dev->base.idle_state = AT86RF2XX_STATE_TRX_OFF;
    /* radio state is P_ON when first powered-on */
    dev->base.state = AT86RF2XX_STATE_P_ON;
    dev->base.pending_tx = 0;
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
    netdev_ieee802154_reset(&dev->base.netdev);

    eui64_t addr_long;
    network_uint16_t addr_short;

    /* generate EUI-64 and short address */
    luid_get_eui64(&addr_long);
    luid_get_short(&addr_short);


    /* set short and long address */
    at86rf2xx_set_addr_long(dev, &addr_long);
    at86rf2xx_set_addr_short(dev, &addr_short);

    /* set default options */
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);

    static const netopt_enable_t enable = NETOPT_ENABLE;
    netdev_ieee802154_set(&dev->base.netdev, NETOPT_ACK_REQ,
                          &enable, sizeof(enable));
}

bool at86rf2xx_cca(at86rf2xx_t *dev)
{
    uint8_t phy_cc_cca = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);

    phy_cc_cca |= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, phy_cc_cca);
    /* Spin until done (8 symbols + 12 µs = 128 µs + 12 µs for O-QPSK)*/
    uint8_t trx_status;
    do {
        trx_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS);
    } while ((trx_status & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE) == 0);
    /* return true if channel is clear */
    return !!(trx_status & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS);
}
