/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 Radio HAL implementation for the KW2x RF driver
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "net/gnrc.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"
#include "kw2xrf.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_getset.h"
#include "kw2xrf_intern.h"
#include "net/ieee802154/radio.h"
#include "kw2xrf_params.h"
#include "event/thread.h"
#include "xtimer.h"

static const ieee802154_radio_ops_t kw2xrf_ops;

void _print_sts1_state(uint8_t sts1, uint8_t phyctl2) {
    printf("MKW2XDM_IRQSTS1_RX_FRM_PEND:    %s\n", sts1 & MKW2XDM_IRQSTS1_RX_FRM_PEND ? "SET" : "off");
    printf("MKW2XDM_IRQSTS1_PLL_UNLOCK_IRQ: %s (%s)\n", sts1 & MKW2XDM_IRQSTS1_PLL_UNLOCK_IRQ ? "SET" : "off",
                                                        phyctl2 & MKW2XDM_PHY_CTRL2_PLL_UNLOCK_MSK ? "masked" : "ACTIVE");
    printf("MKW2XDM_IRQSTS1_FILTERFAIL_IRQ: %s (%s)\n", sts1 & MKW2XDM_IRQSTS1_FILTERFAIL_IRQ ? "SET" : "off",
                                                        phyctl2 & MKW2XDM_PHY_CTRL2_FILTERFAIL_MSK ? "masked" : "ACTIVE");
    printf("MKW2XDM_IRQSTS1_RXWTRMRKIRQ:    %s (%s)\n", sts1 & MKW2XDM_IRQSTS1_RXWTRMRKIRQ ? "SET" : "off",
                                                        phyctl2 & MKW2XDM_PHY_CTRL2_RX_WMRK_MSK ? "masked" : "ACTIVE");
    printf("MKW2XDM_IRQSTS1_CCAIRQ:         %s (%s)\n", sts1 & MKW2XDM_IRQSTS1_CCAIRQ ? "SET" : "off",
                                                        phyctl2 & MKW2XDM_PHY_CTRL2_CCAMSK ? "masked" : "ACTIVE");
    printf("MKW2XDM_IRQSTS1_RXIRQ:          %s (%s)\n", sts1 & MKW2XDM_IRQSTS1_RXIRQ ? "SET" : "off",
                                                        phyctl2 & MKW2XDM_PHY_CTRL2_RXMSK ? "masked" : "ACTIVE");
    printf("MKW2XDM_IRQSTS1_TXIRQ:          %s (%s)\n", sts1 & MKW2XDM_IRQSTS1_TXIRQ ? "SET" : "off",
                                                        phyctl2 & MKW2XDM_PHY_CTRL2_TXMSK ? "masked" : "ACTIVE");
    printf("MKW2XDM_IRQSTS1_SEQIRQ:         %s (%s)\n", sts1 & MKW2XDM_IRQSTS1_SEQIRQ ? "SET" : "off",
                                                        phyctl2 & MKW2XDM_PHY_CTRL2_SEQMSK ? "masked" : "ACTIVE");
}

void _print_irq_state(uint8_t *dregs) {

    printf("MKW2XDM_IRQSTS1:    0x%02X\n", dregs[MKW2XDM_IRQSTS1]);
    printf("MKW2XDM_IRQSTS2:    0x%02X\n", dregs[MKW2XDM_IRQSTS2]);
    printf("MKW2XDM_IRQSTS3:    0x%02X\n", dregs[MKW2XDM_IRQSTS3]);
    printf("MKW2XDM_PHY_CTRL1:  0x%02X\n", dregs[MKW2XDM_PHY_CTRL1]);

    printf("       TMRTRIGEN:   %s\n", dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_TMRTRIGEN ? "on" : "off");
    printf("       SLOTTED:     %s\n", dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_SLOTTED ? "on" : "off");
    printf("       CCABFRTX:    %s\n", dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_CCABFRTX ? "on" : "off");
    printf("       RXACKRQD:    %s\n", dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_RXACKRQD ? "on" : "off");
    printf("       AUTOACK:     %s\n", dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_AUTOACK ? "on" : "off");

    printf("MKW2XDM_PHY_CTRL2:  0x%02X\n", dregs[MKW2XDM_PHY_CTRL2]);
    printf("MKW2XDM_PHY_CTRL3:  0x%02X\n", dregs[MKW2XDM_PHY_CTRL3]);
    printf("MKW2XDM_RX_FRM_LEN: 0x%02X\n", dregs[MKW2XDM_RX_FRM_LEN]);
    printf("MKW2XDM_PHY_CTRL4:  0x%02X\n", dregs[MKW2XDM_PHY_CTRL4]);
    printf("MKW2XDM_PHY_CTRL4_TRCV_MSK      %s\n", dregs[MKW2XDM_PHY_CTRL4] & MKW2XDM_PHY_CTRL4_TRCV_MSK ? "off" : "on");
    printf("MKW2XDM_PHY_CTRL1_TMRTRIGEN     %s\n", dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_TMRTRIGEN ? "on" : "off");
    printf("-------------------------------------\n");

    _print_sts1_state(dregs[MKW2XDM_IRQSTS1], dregs[MKW2XDM_PHY_CTRL2]);

    printf("MKW2XDM_IRQSTS2_CRCVALID:       %s (%s)\n", dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_CRCVALID ? "SET" : "off",
                                                        dregs[MKW2XDM_PHY_CTRL2] & MKW2XDM_PHY_CTRL2_CRC_MSK ? "masked" : "ACTIVE");
    printf("MKW2XDM_IRQSTS2_CCA:            %s\n", dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_CCA ? "SET" : "off");
    printf("MKW2XDM_IRQSTS2_SRCADDR:        %s\n", dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_SRCADDR ? "SET" : "off");
    printf("MKW2XDM_IRQSTS2_PI:             %s\n", dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_PI ? "SET" : "off");
    printf("MKW2XDM_IRQSTS2_TMRSTATUS:      %s\n", dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_TMRSTATUS ? "SET" : "off");

    printf("MKW2XDM_IRQSTS2_PB_ERR_IRQ:     %s (%s)\n", dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_PB_ERR_IRQ ? "SET" : "off",
                                                   dregs[MKW2XDM_PHY_CTRL3] & MKW2XDM_PHY_CTRL3_PB_ERR_MSK ? "masked" : "ACTIVE");
    printf("MKW2XDM_IRQSTS2_WAKE_IRQ:       %s (%s)\n", dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_WAKE_IRQ ? "SET" : "off",
                                                   dregs[MKW2XDM_PHY_CTRL3] & MKW2XDM_PHY_CTRL3_WAKE_MSK ? "masked" : "ACTIVE");

    printf("MKW2XDM_IRQSTS3_TMR4IRQ:        %s (%s|%s)\n", dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR4IRQ ? "SET" : "off",
                                                           dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR4MSK ? "masked" : "ACTIVE",
                                                           dregs[MKW2XDM_PHY_CTRL3] & MKW2XDM_PHY_CTRL3_TMR4CMP_EN ? "compare" : "no-compare");
    printf("MKW2XDM_IRQSTS3_TMR3IRQ:        %s (%s|%s)\n", dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR3IRQ ? "SET" : "off",
                                                           dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR3MSK ? "masked" : "ACTIVE",
                                                           dregs[MKW2XDM_PHY_CTRL3] & MKW2XDM_PHY_CTRL3_TMR3CMP_EN ? "compare" : "no-compare");
    printf("MKW2XDM_IRQSTS3_TMR2IRQ:        %s (%s|%s)\n", dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR2IRQ ? "SET" : "off",
                                                           dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR2MSK ? "masked" : "ACTIVE",
                                                           dregs[MKW2XDM_PHY_CTRL3] & MKW2XDM_PHY_CTRL3_TMR2CMP_EN ? "compare" : "no-compare");
    printf("MKW2XDM_IRQSTS3_TMR1IRQ:        %s (%s|%s)\n", dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR1IRQ ? "SET" : "off",
                                                           dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR1MSK ? "masked" : "ACTIVE",
                                                           dregs[MKW2XDM_PHY_CTRL3] & MKW2XDM_PHY_CTRL3_TMR1CMP_EN ? "compare" : "no-compare");
}

static void _set_sequence(kw2xrf_t *kw_dev, kw2xrf_physeq_t seq)
{
    uint8_t ctl1 = kw2xrf_read_dreg(kw_dev, MKW2XDM_PHY_CTRL1);
    ctl1 &= ~(MKW2XDM_PHY_CTRL1_XCVSEQ_MASK);
    ctl1 |= MKW2XDM_PHY_CTRL1_XCVSEQ(seq);
    kw2xrf_write_dreg(kw_dev, MKW2XDM_PHY_CTRL1, ctl1);
}

/* The first byte returned by the radio is always IRQSTS1. Thus, giving IRQSTS2
   as the start address (first byte written) reduces communicaiton overhead by
   one byte and reduces turnaround time for radio handling.
   (Starting at address IRQSTS1 instead, effectively returns IRQSTS1 twice) */
void _kw2xrf_read_dregs_from_sts1(kw2xrf_t *dev, uint8_t *buf, uint8_t length)
{
    spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_0,
                dev->params->spi_clk);
    uint8_t cmd = (MKW2XDM_IRQSTS2 | MKW2XDRF_REG_READ);
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, true, &cmd, buf, 1);
    if (length > 1) {
        spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, NULL,
                           &buf[1], length - 1);
    }
    spi_release(dev->params->spi);
}

static void _start_tx(kw2xrf_t *kw_dev)
{
    uint8_t pctl1 = kw2xrf_read_dreg(kw_dev, MKW2XDM_PHY_CTRL1);

    if (kw_dev->ack_requested) {
        /* expect an ACK after TX */
        pctl1 |= MKW2XDM_PHY_CTRL1_RXACKRQD;
    } else {
        /* don't expect an ACK after TX */
        pctl1 &= ~MKW2XDM_PHY_CTRL1_RXACKRQD;
    }

    /* A (T) sequence performs a simple transmit and returns to idle, a (TR)
    sequence waits for the requested ACK response after the transmission */
    pctl1 &= ~MKW2XDM_PHY_CTRL1_XCVSEQ_MASK;
    pctl1 |= MKW2XDM_PHY_CTRL1_XCVSEQ(kw_dev->ack_requested ? XCVSEQ_TX_RX :
                                                              XCVSEQ_TRANSMIT);
    kw2xrf_write_dreg(kw_dev, MKW2XDM_PHY_CTRL1, pctl1);
}

void kw2xrf_radio_hal_irq_handler(void *arg)
{
    ieee802154_dev_t *dev = arg;
    kw2xrf_t *kw_dev = dev->priv;
    kw2xrf_mask_irq_b(kw_dev);

    uint8_t dregs[MKW2XDM_PHY_CTRL2 +1];
    _kw2xrf_read_dregs_from_sts1(kw_dev, dregs, ARRAY_SIZE(dregs));
    LOG_DEBUG("0x%02X\n", dregs[MKW2XDM_IRQSTS1]);

    uint8_t sts1_clr = 0;
    bool indicate_hal_event = false;
    ieee802154_trx_ev_t hal_event;

    switch (dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_XCVSEQ_MASK) {
        case XCVSEQ_RECEIVE:
            if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_RXWTRMRKIRQ) {
                sts1_clr |= MKW2XDM_IRQSTS1_RXWTRMRKIRQ;
                hal_event = IEEE802154_RADIO_INDICATION_RX_START;
                indicate_hal_event = true;
                break; /* don't process further events on RX_START */
            }

            /* SEQ assertion during this state indicates a reception */
            if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ) {
                /* clear all pending IRQs asserted during RX sequence */
                sts1_clr |= dregs[MKW2XDM_IRQSTS1];

                if (!(dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_CRCVALID)) {
                    hal_event = IEEE802154_RADIO_INDICATION_CRC_ERROR;
                    indicate_hal_event = true;
                }

                if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_RXIRQ) {
                    sts1_clr |= MKW2XDM_IRQSTS1_RXIRQ;
                    hal_event = IEEE802154_RADIO_INDICATION_RX_DONE;
                    indicate_hal_event = true;
                }
            }
            break;
        case XCVSEQ_TRANSMIT:
            if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_TXIRQ) {
                sts1_clr |= MKW2XDM_IRQSTS1_TXIRQ;
            }

            if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ) {
                sts1_clr |= MKW2XDM_IRQSTS1_SEQIRQ;
                kw_dev->tx_done = true;
                hal_event = IEEE802154_RADIO_CONFIRM_TX_DONE;
                indicate_hal_event = true;
            }
            break;
        case XCVSEQ_CCA:
            /* handle after CCA *and* sequence (warmdown) finished */
            if ((dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_CCAIRQ) &&
                (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ)) {
                sts1_clr |= (MKW2XDM_IRQSTS1_CCAIRQ | MKW2XDM_IRQSTS1_SEQIRQ);

                kw_dev->ch_clear = !(dregs[MKW2XDM_IRQSTS2] &
                                     MKW2XDM_IRQSTS2_CCA);
                kw_dev->waiting_for_cca = false;

                /* if this cca was performed as "CCA-before TX" */
                if (kw_dev->tx_cca_pending) {
                    kw_dev->tx_cca_pending = false;
                    if (kw_dev->ch_clear) {
                        /* clear pending interrupts before TX */
                        kw2xrf_write_dreg(kw_dev, MKW2XDM_IRQSTS1, sts1_clr);
                        _start_tx(kw_dev);
                        kw2xrf_enable_irq_b(kw_dev);
                        return;
                    } else {
                        kw_dev->tx_done = true;
                        /* indicate TX_DONE. The confirm function will return
                           channel busy */
                        hal_event = IEEE802154_RADIO_CONFIRM_TX_DONE;
                        indicate_hal_event = true;
                        break;
                    }
                }

                hal_event = IEEE802154_RADIO_CONFIRM_CCA;
                indicate_hal_event = true;
            }
            break;
        case XCVSEQ_TX_RX:
            if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_TXIRQ) {
                sts1_clr |= MKW2XDM_IRQSTS1_TXIRQ;
                if (dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_RXACKRQD) {
                    /* Allow TMR3IRQ to cancel RX operation */
                    kw2xrf_timer3_seq_abort_on(kw_dev);
                    /* Enable interrupt for TMR3 and set timer */
                    kw2xrf_abort_rx_ops_enable(kw_dev, IEEE802154_ACK_TIMEOUT_SYMS);
                }
            }

            if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_RXIRQ) {
                sts1_clr |= MKW2XDM_IRQSTS1_RXIRQ;
            }

            if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_CCAIRQ) {
                kw_dev->ch_clear = !(dregs[MKW2XDM_IRQSTS2] &
                                     MKW2XDM_IRQSTS2_CCA);
                sts1_clr |= MKW2XDM_IRQSTS1_CCAIRQ;
            }

            if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ) {
                sts1_clr |= MKW2XDM_IRQSTS1_SEQIRQ;

                kw_dev->ack_rcvd = !(dregs[MKW2XDM_IRQSTS3] &
                                     MKW2XDM_IRQSTS3_TMR3IRQ);

                /* Disallow TMR3IRQ to cancel RX operation */
                kw2xrf_timer3_seq_abort_off(kw_dev);
                /* Disable interrupt for TMR3 and reset TMR3IRQ */
                kw2xrf_abort_rx_ops_disable(kw_dev);

                kw_dev->tx_done = true;
                hal_event = IEEE802154_RADIO_CONFIRM_TX_DONE;
                indicate_hal_event = true;
            }
            break;
        case XCVSEQ_IDLE:
            /* clear SEQ interrupt for explicit transitions to IDLE */
            sts1_clr |= MKW2XDM_IRQSTS1_SEQIRQ;
            break;
        default:
            break;
    }

    /* clear handled IRQs */
    kw2xrf_write_dreg(kw_dev, MKW2XDM_IRQSTS1, sts1_clr);

    kw2xrf_enable_irq_b(kw_dev);

    if (indicate_hal_event) {
        dev->cb(dev, hal_event);
    }
}

int kw2xrf_init(kw2xrf_t *dev, const kw2xrf_params_t *params, ieee802154_dev_t *hal,
                     gpio_cb_t cb, void *ctx)
{
    /* initialize device descriptor */
    dev->params = params;
    dev->idle_state = XCVSEQ_RECEIVE;
    dev->state = 0;
    dev->pending_tx = 0;

    kw2xrf_spi_init(dev);
    kw2xrf_set_power_mode(dev, KW2XRF_IDLE);
    LOG_DEBUG("[kw2xrf] enabling RX/TX completion and start events");
    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2, MKW2XDM_PHY_CTRL2_RX_WMRK_MSK);
    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2, MKW2XDM_PHY_CTRL2_RXMSK);
    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL2, MKW2XDM_PHY_CTRL2_TXMSK);
    LOG_DEBUG("[kw2xrf] setup finished\n");

    hal->driver = &kw2xrf_ops;
    hal->priv = dev;

    dev->cca_before_tx = true;
    kw2xrf_set_out_clk(dev);
    kw2xrf_disable_interrupts(dev);

    /* set up GPIO-pin used for IRQ */
    gpio_init_int(dev->params->int_pin, GPIO_IN, GPIO_FALLING, cb, ctx);

    kw2xrf_abort_sequence(dev);
    kw2xrf_update_overwrites(dev);
    kw2xrf_timer_init(dev, KW2XRF_TIMEBASE_62500HZ);

    kw2xrf_reset_phy(dev);

    /* Disable automatic CCA before TX (for T and TR sequences).
       Auto CCA was found to cause poor performance (significant packet loss)
       when performed before the transmission - even on clearchannel.
       As a workaround we perform manual CCA directly before transmission
       which doesn't show degraded performance.
       Note: the poor performance was only visible when transmitting data
             to other models of radios. I.e., kw2xrf to kw2xrf was fine, while
             kw2xrf to at862xx and nrf52 performs very bad. */
    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_CCABFRTX);

    LOG_DEBUG("[kw2xrf] init finished\n");
    return 0;
}

static int _write(ieee802154_dev_t *dev, const iolist_t *iolist)
{
    kw2xrf_t *kw_dev = dev->priv;

    /* get length */
    uint8_t len = iolist_size(iolist) + IEEE802154_FCS_LEN;

    if (len > KW2XRF_MAX_PKT_LENGTH) {
        LOG_ERROR("[kw2xrf] packet too large (%u byte) to be send\n", len);
        return -EOVERFLOW;
    }

    /* check if ack req bit is set to decide which transmit sequence to use to
       send the frame */
    uint8_t *data = iolist->iol_base;
    kw_dev->ack_requested = *data & IEEE802154_FCF_ACK_REQ;

    /* transfer packet data to radio buffer */
    spi_acquire(kw_dev->params->spi, kw_dev->params->cs_pin, SPI_MODE_0,
                kw_dev->params->spi_clk);
    spi_transfer_byte(kw_dev->params->spi, kw_dev->params->cs_pin, true,
                      MKW2XDRF_BUF_WRITE);
    spi_transfer_byte(kw_dev->params->spi, kw_dev->params->cs_pin, true, len);

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        /* start after pkt len byte */
        bool cont = iol->iol_next;
        spi_transfer_bytes(kw_dev->params->spi, kw_dev->params->cs_pin, cont,
                           iol->iol_base, NULL, iol->iol_len);
    }

    spi_release(kw_dev->params->spi);

    return 0;
}

static int _request_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    kw2xrf_t *kw_dev = dev->priv;
    int res = -EINVAL;
    (void) ctx;

    kw2xrf_mask_irq_b(kw_dev);
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        kw_dev->tx_done = false;
        if (kw_dev->cca_before_tx) {
            kw_dev->tx_cca_pending = true;
            _set_sequence(kw_dev, XCVSEQ_CCA);
        } else {
            _start_tx(kw_dev);
        }
        kw2xrf_enable_irq_b(kw_dev);
        res = 0;
        break;
    case IEEE802154_HAL_OP_SET_RX:
        kw2xrf_write_dreg(kw_dev, MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_RXIRQ |
                          MKW2XDM_IRQSTS1_RXWTRMRKIRQ);

        /* enable WTMRK and SEQ as indication for RX_START and RX_DONE */
        kw2xrf_write_dreg(kw_dev, MKW2XDM_PHY_CTRL2,
                          ~(MKW2XDM_PHY_CTRL2_SEQMSK |
                            MKW2XDM_PHY_CTRL2_RX_WMRK_MSK));

        _set_sequence(kw_dev, XCVSEQ_RECEIVE);
        res = 0;
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        kw2xrf_set_power_mode(kw_dev, KW2XRF_IDLE);

        /* clear any pending Tx interrupts */
        kw2xrf_write_dreg(kw_dev, MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_TXIRQ |
                                  MKW2XDM_IRQSTS1_SEQIRQ);

        /* enable SEQ IRQ as indication for TX_DONE and TX IRQ to
           indicate when the frame was transmitted to then possibly
           schedule an ACK timeout */
        kw2xrf_write_dreg(kw_dev, MKW2XDM_PHY_CTRL2,
                          ~(MKW2XDM_PHY_CTRL2_SEQMSK |
                            MKW2XDM_PHY_CTRL2_TXMSK));

        _set_sequence(kw_dev, XCVSEQ_IDLE);
        res = 0;
        break;
    case IEEE802154_HAL_OP_CCA:
        kw_dev->waiting_for_cca = true;
        kw2xrf_set_sequence(kw_dev, XCVSEQ_CCA);
        res = 0;
        break;
    default:
        assert(false);
    }

    return res;
}

static int _confirm_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    kw2xrf_t *kw_dev = dev->priv;
    int res = -EAGAIN;
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        if (!kw_dev->tx_done) {
            break;
        }
        if (ctx) {
            ieee802154_tx_info_t *info = ctx;
            if (!kw_dev->ch_clear) {
                info->status = TX_STATUS_MEDIUM_BUSY;
            }
            else if (kw_dev->ack_rcvd || !kw_dev->ack_requested) {
                info->status = TX_STATUS_SUCCESS;
            } else {
                info->status = TX_STATUS_NO_ACK;
            }
        }
        res = 0;
        break;
    case IEEE802154_HAL_OP_SET_RX:
        kw2xrf_enable_irq_b(kw_dev);
        res = 0;
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        if (kw2xrf_read_dreg(kw_dev, MKW2XDM_SEQ_STATE) == XCVSEQ_IDLE) {
            kw2xrf_write_dreg(kw_dev, MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_SEQIRQ);
            kw2xrf_enable_irq_b(kw_dev);
            res = 0;
        }
        else {
            res = -EAGAIN;
        }
        break;
    case IEEE802154_HAL_OP_CCA:
        if (!kw_dev->waiting_for_cca) {
            *((bool*) ctx) = kw_dev->ch_clear;
            res = 0;
        }
        break;
    default:
        assert(false);
    }

    return res;
}

static int _len(ieee802154_dev_t *dev)
{
    kw2xrf_t *kw_dev = dev->priv;
    size_t pkt_len = kw2xrf_read_dreg(kw_dev, MKW2XDM_RX_FRM_LEN) -
                     IEEE802154_FCS_LEN;
    return pkt_len;
}

static int _read(ieee802154_dev_t *dev, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    kw2xrf_t *kw_dev = dev->priv;

    size_t rxlen =  _len(dev);
    if (!buf) {
        return 0;
    }
    rxlen = size < rxlen ? size : rxlen;
    kw2xrf_read_fifo(kw_dev, (uint8_t *)buf, rxlen);

    if (info != NULL) {
        info->lqi = kw2xrf_read_dreg(kw_dev, MKW2XDM_LQI_VALUE);
        info->rssi = kw2xrf_get_rssi(info->lqi);
    }

    return rxlen;
}

static int _set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    kw2xrf_t *kw_dev = dev->priv;
    /* normalize to absolute value */
    if (threshold < 0) {
        threshold = -threshold;
    }

    kw2xrf_write_iregs(kw_dev, MKW2XDMI_CCA1_THRESH, (uint8_t*)&threshold, 1);
    kw2xrf_write_iregs(kw_dev, MKW2XDMI_CCA2_THRESH, (uint8_t*)&threshold, 1);

    return 0;
}

/* PLL integer and fractional lookup tables
 *
 * Fc = 2405 + 5(k - 11) , k = 11,12,...,26
 *
 * Equation for PLL frequency, MKW2xD Reference Manual, p.255 :
 * F = ((PLL_INT0 + 64) + (PLL_FRAC0/65536))32MHz
 *
 */
static const uint8_t pll_int_lt[16] = {
    11, 11, 11, 11,
    11, 11, 12, 12,
    12, 12, 12, 12,
    13, 13, 13, 13
};

static const uint16_t pll_frac_lt[16] = {
    10240, 20480, 30720, 40960,
    51200, 61440, 6144, 16384,
    26624, 36864, 47104, 57344,
    2048, 12288, 22528, 32768
};

static int _config_phy(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf)
{
    kw2xrf_t *kw_dev = dev->priv;
    kw2xrf_set_tx_power(kw_dev, conf->pow);
    uint8_t tmp = conf->channel - 11;
    kw2xrf_write_dreg(kw_dev, MKW2XDM_PLL_INT0, MKW2XDM_PLL_INT0_VAL(pll_int_lt[tmp]));
    kw2xrf_write_dreg(kw_dev, MKW2XDM_PLL_FRAC0_LSB, (uint8_t)pll_frac_lt[tmp]);
    kw2xrf_write_dreg(kw_dev, MKW2XDM_PLL_FRAC0_MSB, (uint8_t)(pll_frac_lt[tmp] >> 8));
    return 0;
}

static int _off(ieee802154_dev_t *dev)
{
    kw2xrf_t *kw_dev = dev->priv;
    /* TODO: check if power down via RST_B is possible  */
    kw2xrf_set_power_mode(kw_dev, KW2XRF_HIBERNATE);
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value)
{
    const uint16_t *pan_id = value;
    const network_uint16_t *short_addr = value;
    const eui64_t *ext_addr = value;
    kw2xrf_t *kw_dev = dev->priv;
    switch(cmd) {
        case IEEE802154_AF_SHORT_ADDR:
            kw2xrf_set_addr_short(kw_dev, byteorder_ntohs(*short_addr));
            break;
        case IEEE802154_AF_EXT_ADDR:
            kw2xrf_set_addr_long(kw_dev, ext_addr->uint64.u64);
            break;
        case IEEE802154_AF_PANID:
            kw2xrf_set_pan(kw_dev, *pan_id);
            break;
        case IEEE802154_AF_PAN_COORD:
            return -ENOTSUP;
    }

    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    kw2xrf_t *kw_dev = dev->priv;
    /* enable xtal and put power management controller to high power mode */
    kw2xrf_set_power_mode(kw_dev, KW2XRF_IDLE);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    kw2xrf_t *kw_dev = dev->priv;
    size_t pwr_modes = kw2xrf_read_dreg(kw_dev, MKW2XDM_PWR_MODES);
    return (pwr_modes & MKW2XDM_PWR_MODES_XTAL_READY) ? 0 : -EAGAIN;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    kw2xrf_t *kw_dev = dev->priv;

    uint8_t cca_ctl_val = 0;
    uint8_t dev_mode = 0;
    switch (mode) {
        case IEEE802154_CCA_MODE_ED_THRESHOLD:
            dev_mode = 1;
            break;
        case IEEE802154_CCA_MODE_CARRIER_SENSING:
            dev_mode = 2;
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_AND_CS:
            dev_mode = 3;
            kw2xrf_read_iregs(kw_dev, MKW2XDMI_CCA_CTRL, &cca_ctl_val, 1);
            cca_ctl_val |= MKW2XDMI_CCA_CTRL_CCA3_AND_NOT_OR;
            kw2xrf_write_iregs(kw_dev, MKW2XDMI_CCA_CTRL, &cca_ctl_val, 1);
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_OR_CS:
            dev_mode = 3;
            kw2xrf_read_iregs(kw_dev, MKW2XDMI_CCA_CTRL, &cca_ctl_val, 1);
            cca_ctl_val &= ~(MKW2XDMI_CCA_CTRL_CCA3_AND_NOT_OR);
            kw2xrf_write_iregs(kw_dev, MKW2XDMI_CCA_CTRL, &cca_ctl_val, 1);
            break;
    }

    kw2xrf_set_cca_mode(kw_dev, dev_mode);
    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *dev, ieee802154_src_match_t cmd, const void *value)
{
    kw2xrf_t *kw_dev = dev->priv;
    switch(cmd) {
        case IEEE802154_SRC_MATCH_EN: {

            const bool en = *((const bool*) value);
            if (en) {
                kw2xrf_set_dreg_bit(kw_dev, MKW2XDM_SRC_CTRL,
                                        MKW2XDM_SRC_CTRL_ACK_FRM_PND);
            }
            else {
                kw2xrf_clear_dreg_bit(kw_dev, MKW2XDM_SRC_CTRL,
                                          MKW2XDM_SRC_CTRL_ACK_FRM_PND);
            }
            break;
        }
        default:
            return -ENOTSUP;
    }
    return 0;
}

static int _set_frame_filter_mode(ieee802154_dev_t *dev, ieee802154_filter_mode_t mode)
{
    kw2xrf_t *kw_dev = dev->priv;

    bool _promisc = false;

    switch (mode) {
        case IEEE802154_FILTER_ACCEPT:
            break;
        case IEEE802154_FILTER_PROMISC:
            _promisc = true;
            break;
        case IEEE802154_FILTER_ACK_ONLY:
            /* Not implemented */
            break;
        default:
            return -ENOTSUP;
    }

    if (_promisc) {
        kw2xrf_set_dreg_bit(kw_dev, MKW2XDM_PHY_CTRL4,
                            MKW2XDM_PHY_CTRL4_PROMISCUOUS);
    }
    else {
        kw2xrf_clear_dreg_bit(kw_dev, MKW2XDM_PHY_CTRL4,
                            MKW2XDM_PHY_CTRL4_PROMISCUOUS);
    }

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd, int8_t retries)
{
    kw2xrf_t *kw_dev = dev->priv;
    (void) bd;

    if (retries < 0) {
        kw_dev->cca_before_tx = false;
        return 0;
    } else if (retries == 0) {
        kw_dev->cca_before_tx = true;
        return 0;
    }

    return -EINVAL;
}

static const ieee802154_radio_ops_t kw2xrf_ops = {
    .caps =  IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_AUTO_ACK
          | IEEE802154_CAP_IRQ_CRC_ERROR
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_CCA_DONE
          | IEEE802154_CAP_IRQ_ACK_TIMEOUT
          | IEEE802154_CAP_PHY_OQPSK,
    .write = _write,
    .read = _read,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .len = _len,
    .off = _off,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_csma_params = _set_csma_params,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_frame_filter_mode = _set_frame_filter_mode,
};
