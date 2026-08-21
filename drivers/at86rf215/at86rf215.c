/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Implementation of public functions for AT86RF215 driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "at86rf215.h"
#include "at86rf215_internal.h"

void at86rf215_reset(at86rf215_t *dev)
{
    /* Reset state machine to ensure a known state */
    at86rf215_rf_cmd(dev, CMD_RF_TRXOFF);
    at86rf215_await_state(dev, RF_STATE_TRXOFF);
    dev->state = AT86RF215_STATE_TRXOFF;

    if (!dev->sibling) {
        /* disable 2.4-GHz IRQs if the interface is not enabled */
        if (is_subGHz(dev)) {
            at86rf215_reg_write(dev, RG_BBC1_IRQM, 0);
            at86rf215_reg_write(dev, RG_RF24_IRQM, 0);
            at86rf215_reg_write(dev, RG_RF24_CMD, CMD_RF_SLEEP);

        /* disable sub-GHz IRQs if the interface is not enabled */
        } else {
            at86rf215_reg_write(dev, RG_BBC0_IRQM, 0);
            at86rf215_reg_write(dev, RG_RF09_IRQM, 0);
            at86rf215_reg_write(dev, RG_RF09_CMD, CMD_RF_SLEEP);
        }
    }

    /* disable clock output */
if (!IS_ACTIVE(CONFIG_AT86RF215_USE_CLOCK_OUTPUT)){
    at86rf215_reg_write(dev, RG_RF_CLKO, 0);
}
    /* allow to configure board-specific trim */
#ifdef CONFIG_AT86RF215_TRIM_VAL
    at86rf215_set_trim(dev, CONFIG_AT86RF215_TRIM_VAL);
#endif

#if CONFIG_AT86RF215_RPC_EN
    dev->rpc_enable = true;
#else
    dev->rpc_enable = false;
#endif

    /* enable address filter 0 */
    at86rf215_reg_write(dev, dev->BBC->RG_AFC0, AFC0_AFEN0_MASK );
    at86rf215_reg_write(dev, dev->BBC->RG_AMAACKPD, AMAACKPD_PD0_MASK);
    /* enable fcs filter */
    at86rf215_reg_or(dev, dev->BBC->RG_PC, PC_FCSFE_MASK);

    if (IS_USED(MODULE_IEEE802154_RX_TIMESTAMP)) {
        at86rf215_reg_write(dev, dev->BBC->RG_CNTC,
                                 CNTC_EN_MASK | CNTC_CAPRXS_MASK);
    }

    /* enable auto-ACK with Frame Checksum & Data Rate derived from RX frame */
    uint8_t reg = AMCS_AACKFA_MASK | AMCS_AACKDR_MASK | AMCS_AACK_MASK;
    dev->auto_mode = AT86RF215_AM_AUTO_ACK;
    /* send acks respective to 802.15.4-2006 */
    reg &= ~AMCS_AACKS_MASK;
    /* disable cca before tx and tx2rx to avoid conflicts */
    reg &= ~AMCS_CCATX_MASK;
    reg &= ~AMCS_TX2RX_MASK;
    at86rf215_reg_write(dev, dev->BBC->RG_AMCS, reg);

    /* enable IRQS */
    at86rf215_reg_write(dev, dev->BBC->RG_IRQM, BB_IRQ_RXFS | BB_IRQ_TXFE | BB_IRQ_RXFE);
    at86rf215_reg_write(dev, dev->RF->RG_IRQM,  RF_IRQ_EDC | RF_IRQ_TRXRDY);
}

void at86rf215_tx_done(at86rf215_t *dev)
{
    /* re-enable AACK*/
    at86rf215_filter_ack_only(dev, false);
    at86rf215_set_auto_mode(dev, AT86RF215_AM_AUTO_ACK, true);
    /* re-enable reduced power consumption */
    at86rf215_enable_rpc(dev);
}

int at86rf215_tx_exec(at86rf215_t *dev)
{
    at86rf215_rf_cmd(dev, CMD_RF_TX);

    dev->state = AT86RF215_STATE_TX;
    return 0;
}
