/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
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
 * @brief       Implementation of public functions for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */


#include "luid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void at86rf2xx_disable_clock_output(at86rf2xx_t *dev)
{
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
    tmp |= (AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, tmp);
}

static void at86rf2xx_enable_smart_idle(at86rf2xx_t *dev)
{
#if AT86RF2XX_SMART_IDLE_LISTENING
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_RPC);
    tmp |= (AT86RF2XX_TRX_RPC_MASK__RX_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__PDT_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__PLL_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__XAH_TX_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__IPAN_RPC_EN);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_RPC, tmp);
    at86rf2xx_set_rxsensitivity(dev, RSSI_BASE_VAL);
#else
    (void) dev;
#endif
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
    /* Reset state machine to ensure a known state */
    dev->is_sleep = false;

    at86rf2xx_set_internal_state(dev, AT86RF2XX_STATE_FORCE_TRX_OFF);

    /* set default TX power */
    at86rf2xx_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);
    /* set default options */
    at86rf2xx_set_auto_ack(dev, true);

    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2,
                        AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);

    /* configure smart idle listening feature */
    at86rf2xx_enable_smart_idle(dev);

    /* disable clock output to save power */
    at86rf2xx_disable_clock_output(dev);

    DEBUG("at86rf2xx_reset(): reset complete.\n");
}

void at86rf2xx_tx_exec(const at86rf2xx_t *dev)
{
    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);
}
