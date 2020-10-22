/*
 * Copyright (C) 2016 Unwired Devices <info@unwds.com>
 *               2017 Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx127x
 * @{
 * @file
 * @brief       Implementation of get and set functions for SX127X
 *
 * @author      Eugene P. <ep@unwds.com>
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "ztimer.h"

#include "net/lora.h"

#include "sx127x.h"
#include "sx127x_registers.h"
#include "sx127x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

uint8_t sx127x_get_state(const sx127x_t *dev)
{
    return dev->settings.state;
}

void sx127x_set_state(sx127x_t *dev, uint8_t state)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        switch (state) {
        case SX127X_RF_IDLE:
            DEBUG("[sx127x] Change state: IDLE\n");
            break;
        case SX127X_RF_RX_RUNNING:
            DEBUG("[sx127x] Change state: RX\n");
            break;
        case SX127X_RF_TX_RUNNING:
            DEBUG("[sx127x] Change state: TX\n");
            break;
        default:
            DEBUG("[sx127x] Change state: UNKNOWN\n");
            break;
        }
    }

    dev->settings.state = state;
}

void sx127x_set_modem(sx127x_t *dev, uint8_t modem)
{
    DEBUG("[sx127x] set modem: %d\n", modem);

    if ((sx127x_reg_read(dev, SX127X_REG_OPMODE) & SX127X_RF_LORA_OPMODE_LONGRANGEMODE_ON) != 0) {
        dev->settings.modem = SX127X_MODEM_LORA;
    }
    else {
        dev->settings.modem = SX127X_MODEM_FSK;
    }

    /* Skip if unchanged to avoid resetting the transceiver below (may end up
     * in crashes) */
    if (dev->settings.modem == modem) {
        DEBUG("[sx127x] already using modem: %d\n", modem);
        return;
    }

    dev->settings.modem = modem;

    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            /* Todo */
            break;
        case SX127X_MODEM_LORA:
            sx127x_set_op_mode(dev, SX127X_RF_OPMODE_SLEEP);
            sx127x_reg_write(dev, SX127X_REG_OPMODE,
                             (sx127x_reg_read(dev, SX127X_REG_OPMODE) &
                              SX127X_RF_LORA_OPMODE_LONGRANGEMODE_MASK) |
                             SX127X_RF_LORA_OPMODE_LONGRANGEMODE_ON);

            sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1, 0x00);
            sx127x_reg_write(dev, SX127X_REG_DIOMAPPING2, 0x00);
            break;
        default:
            break;
    }
}

uint8_t sx127x_get_syncword(const sx127x_t *dev)
{
    return sx127x_reg_read(dev, SX127X_REG_LR_SYNCWORD);
}

void sx127x_set_syncword(sx127x_t *dev, uint8_t syncword)
{
    DEBUG("[sx127x] Set syncword: %02x\n", syncword);

    sx127x_reg_write(dev, SX127X_REG_LR_SYNCWORD, syncword);
}

uint32_t sx127x_get_channel(const sx127x_t *dev)
{
    return (((uint32_t)sx127x_reg_read(dev, SX127X_REG_FRFMSB) << 16) |
            (sx127x_reg_read(dev, SX127X_REG_FRFMID) << 8) |
            (sx127x_reg_read(dev, SX127X_REG_FRFLSB))) * LORA_FREQUENCY_RESOLUTION_DEFAULT;
}

void sx127x_set_channel(sx127x_t *dev, uint32_t channel)
{
    DEBUG("[sx127x] Set channel: %" PRIu32 "\n", channel);

    /* Save current operating mode */
    dev->settings.channel = channel;

    channel = (uint32_t)((double) channel / (double)LORA_FREQUENCY_RESOLUTION_DEFAULT);

    /* Write frequency settings into chip */
    sx127x_reg_write(dev, SX127X_REG_FRFMSB, (uint8_t)((channel >> 16) & 0xFF));
    sx127x_reg_write(dev, SX127X_REG_FRFMID, (uint8_t)((channel >> 8) & 0xFF));
    sx127x_reg_write(dev, SX127X_REG_FRFLSB, (uint8_t)(channel & 0xFF));
}

uint32_t sx127x_get_time_on_air(const sx127x_t *dev, uint8_t pkt_len)
{
    uint32_t air_time = 0;

    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            /* todo */
            break;
        case SX127X_MODEM_LORA:
        {
            double bw = 0.0;

            /* Note: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported. */
            switch (dev->settings.lora.bandwidth) {
                case LORA_BW_125_KHZ:
                    bw = 125e3;
                    break;
                case LORA_BW_250_KHZ:
                    bw = 250e3;
                    break;
                case LORA_BW_500_KHZ:
                    bw = 500e3;
                    break;
                default:
                    DEBUG("Invalid bandwidth: %d\n", dev->settings.lora.bandwidth);
                    break;
            }

            /* Symbol rate : time for one symbol [secs] */
            double rs = bw / (1 << dev->settings.lora.datarate);
            double ts = 1 / rs;

            /* time of preamble */
            double t_preamble = (dev->settings.lora.preamble_len + 4.25) * ts;

            /* Symbol length of payload and time */
            double tmp =
                ceil(
                    (8 * pkt_len - 4 * dev->settings.lora.datarate + 28
                     + 16 * (dev->settings.lora.flags & SX127X_ENABLE_CRC_FLAG)
                     - (!(dev->settings.lora.flags & SX127X_ENABLE_FIXED_HEADER_LENGTH_FLAG) ? 20 : 0))
                    / (double) (4 * dev->settings.lora.datarate
                                - (((dev->settings.lora.flags & SX127X_LOW_DATARATE_OPTIMIZE_FLAG)
                                    > 0) ? 2 : 0)))
                * (dev->settings.lora.coderate + 4);
            double n_payload = 8 + ((tmp > 0) ? tmp : 0);
            double t_payload = n_payload * ts;

            /* Time on air */
            double t_on_air = t_preamble + t_payload;

            /* return milli seconds */
            air_time = floor(t_on_air * 1e3 + 0.999);
        }
        break;
    }

    return air_time;
}

void sx127x_set_sleep(sx127x_t *dev)
{
    DEBUG("[sx127x] Set sleep\n");

    /* Disable running timers */
    ztimer_remove(ZTIMER_MSEC, &dev->_internal.tx_timeout_timer);
    ztimer_remove(ZTIMER_MSEC, &dev->_internal.rx_timeout_timer);

    /* Put chip into sleep */
    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_SLEEP);
    sx127x_set_state(dev,  SX127X_RF_IDLE);
}

void sx127x_set_standby(sx127x_t *dev)
{
    DEBUG("[sx127x] Set standby\n");

    /* Disable running timers */
    ztimer_remove(ZTIMER_MSEC, &dev->_internal.tx_timeout_timer);
    ztimer_remove(ZTIMER_MSEC, &dev->_internal.rx_timeout_timer);

    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_STANDBY);
    sx127x_set_state(dev,  SX127X_RF_IDLE);
}

void sx127x_set_rx(sx127x_t *dev)
{
    DEBUG("[sx127x] Set RX\n");

#ifdef SX127X_USE_TX_SWITCH
    gpio_clear(dev->params.tx_switch_pin);
#endif
#ifdef SX127X_USE_RX_SWITCH
    gpio_set(dev->params.rx_switch_pin);
#endif

    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            /* todo */
            break;
        case SX127X_MODEM_LORA:
        {
            sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ,
                             ((sx127x_reg_read(dev, SX127X_REG_LR_INVERTIQ) &
                               SX127X_RF_LORA_INVERTIQ_TX_MASK &
                               SX127X_RF_LORA_INVERTIQ_RX_MASK) |
                              ((dev->settings.lora.flags & SX127X_IQ_INVERTED_FLAG) ? SX127X_RF_LORA_INVERTIQ_RX_ON :SX127X_RF_LORA_INVERTIQ_RX_OFF) |
                              SX127X_RF_LORA_INVERTIQ_TX_OFF));
            sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ2,
                             ((dev->settings.lora.flags & SX127X_IQ_INVERTED_FLAG) ? SX127X_RF_LORA_INVERTIQ2_ON : SX127X_RF_LORA_INVERTIQ2_OFF));

#if defined(MODULE_SX1276)
            /* ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal */
            if (dev->settings.lora.bandwidth < 9) {
                sx127x_reg_write(dev, SX127X_REG_LR_DETECTOPTIMIZE,
                                 sx127x_reg_read(dev, SX127X_REG_LR_DETECTOPTIMIZE) & 0x7F);
                sx127x_reg_write(dev, SX127X_REG_LR_TEST30, 0x00);
                switch (dev->settings.lora.bandwidth) {
                    case LORA_BW_125_KHZ: /* 125 kHz */
                        sx127x_reg_write(dev, SX127X_REG_LR_TEST2F, 0x40);
                        break;
                    case LORA_BW_250_KHZ: /* 250 kHz */
                        sx127x_reg_write(dev, SX127X_REG_LR_TEST2F, 0x40);
                        break;

                    default:
                        break;
                }
            }
            else {
                sx127x_reg_write(dev, SX127X_REG_LR_DETECTOPTIMIZE,
                                 sx127x_reg_read(dev, SX127X_REG_LR_DETECTOPTIMIZE) | 0x80);
            }
#endif

            /* Setup interrupts */
            if (dev->settings.lora.flags & SX127X_CHANNEL_HOPPING_FLAG) {
                sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGSMASK,
                                 /* SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT |
                                    SX127X_RF_LORA_IRQFLAGS_RXDONE |
                                    SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                                    SX127X_RF_LORA_IRQFLAGS_VALIDHEADER | */
                                 SX127X_RF_LORA_IRQFLAGS_TXDONE |
                                 SX127X_RF_LORA_IRQFLAGS_CADDONE |
                                 /* SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL | */
                                 SX127X_RF_LORA_IRQFLAGS_CADDETECTED);

                /* DIO0=RxDone, DIO1=RxTimeout, DIO2=FhssChangeChannel, DIO3=ValidHeader */
                sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1,
                                 (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING1) &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO0_MASK &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO2_MASK &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO3_MASK) |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO0_00 |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO1_00 |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO2_00 |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO3_01);
            }
            else {
                sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGSMASK,
                                 /* SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT |
                                    SX127X_RF_LORA_IRQFLAGS_RXDONE |
                                    SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                                    SX127X_RF_LORA_IRQFLAGS_VALIDHEADER | */
                                 SX127X_RF_LORA_IRQFLAGS_TXDONE |
                                 SX127X_RF_LORA_IRQFLAGS_CADDONE |
                                 SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                 SX127X_RF_LORA_IRQFLAGS_CADDETECTED);

                /* DIO0=RxDone, DIO1=RxTimeout, DIO3=ValidHeader */
                sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1,
                                 (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING1) &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO0_MASK &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO3_MASK) |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO0_00 |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO1_00 |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO3_01);
            }

            sx127x_reg_write(dev, SX127X_REG_LR_FIFORXBASEADDR, 0);
            sx127x_reg_write(dev, SX127X_REG_LR_FIFOADDRPTR, 0);
        }
        break;
    }

    sx127x_set_state(dev, SX127X_RF_RX_RUNNING);
    if (dev->settings.lora.rx_timeout != 0) {
        ztimer_set(ZTIMER_MSEC, &(dev->_internal.rx_timeout_timer), dev->settings.lora.rx_timeout);
    }


    if (dev->settings.lora.flags & SX127X_RX_CONTINUOUS_FLAG) {
        sx127x_set_op_mode(dev, SX127X_RF_LORA_OPMODE_RECEIVER);
    }
    else {
        sx127x_set_op_mode(dev, SX127X_RF_LORA_OPMODE_RECEIVER_SINGLE);
    }
}

void sx127x_set_tx(sx127x_t *dev)
{
#ifdef SX127X_USE_RX_SWITCH
    gpio_clear(dev->params.rx_switch_pin);
#endif
#ifdef SX127X_USE_TX_SWITCH
    gpio_set(dev->params.tx_switch_pin);
#endif

     switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            /* todo */
            break;
        case SX127X_MODEM_LORA:
        {
            if (dev->settings.lora.flags & SX127X_CHANNEL_HOPPING_FLAG) {
                sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGSMASK,
                                 SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT |
                                 SX127X_RF_LORA_IRQFLAGS_RXDONE |
                                 SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                                 SX127X_RF_LORA_IRQFLAGS_VALIDHEADER |
                                 /* SX127X_RF_LORA_IRQFLAGS_TXDONE | */
                                 SX127X_RF_LORA_IRQFLAGS_CADDONE |
                                 /* SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL | */
                                 SX127X_RF_LORA_IRQFLAGS_CADDETECTED);

                /* DIO0=TxDone, DIO2=FhssChangeChannel */
                sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1,
                                 (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING1 ) &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO0_MASK &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO2_MASK) |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO0_01 |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO2_00);
            }
            else
            {
                /* Enable TXDONE interrupt */
                sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGSMASK,
                                 SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT |
                                 SX127X_RF_LORA_IRQFLAGS_RXDONE |
                                 SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                                 SX127X_RF_LORA_IRQFLAGS_VALIDHEADER |
                                 /* SX127X_RF_LORA_IRQFLAGS_TXDONE | */
                                 SX127X_RF_LORA_IRQFLAGS_CADDONE |
                                 SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                 SX127X_RF_LORA_IRQFLAGS_CADDETECTED);

                /* Set TXDONE interrupt to the DIO0 line */
                sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1,
                                 (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING1) &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO0_MASK) |
                                  SX127X_RF_LORA_DIOMAPPING1_DIO0_01);
            }
        }
        break;
    }

    sx127x_set_state(dev, SX127X_RF_TX_RUNNING);

    /* Start TX timeout timer */
    if (dev->settings.lora.tx_timeout != 0) {
        ztimer_set(ZTIMER_MSEC, &(dev->_internal.tx_timeout_timer), dev->settings.lora.tx_timeout);
    }

    /* Put chip into transfer mode */
    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_TRANSMITTER );
}

uint8_t sx127x_get_max_payload_len(const sx127x_t *dev)
{
    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            return sx127x_reg_read(dev, SX127X_REG_PAYLOADLENGTH);

        case SX127X_MODEM_LORA:
            return sx127x_reg_read(dev, SX127X_REG_LR_PAYLOADMAXLENGTH);
    }

    /* should never be reached */
    return 0;
}

void sx127x_set_max_payload_len(const sx127x_t *dev, uint8_t maxlen)
{
    DEBUG("[sx127x] Set max payload len: %d\n", maxlen);

    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            sx127x_reg_write(dev, SX127X_REG_PAYLOADLENGTH, maxlen);
            break;

        case SX127X_MODEM_LORA:
            sx127x_reg_write(dev, SX127X_REG_LR_PAYLOADMAXLENGTH, maxlen);
            break;
    }
}

uint8_t sx127x_get_op_mode(const sx127x_t *dev)
{
    return sx127x_reg_read(dev, SX127X_REG_OPMODE) & ~SX127X_RF_OPMODE_MASK;
}

void sx127x_set_op_mode(const sx127x_t *dev, uint8_t op_mode)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        switch(op_mode) {
        case SX127X_RF_OPMODE_SLEEP:
            DEBUG("[sx127x] Set op mode: SLEEP\n");
            break;
        case SX127X_RF_OPMODE_STANDBY:
            DEBUG("[sx127x] Set op mode: STANDBY\n");
            break;
        case SX127X_RF_OPMODE_RECEIVER_SINGLE:
            DEBUG("[sx127x] Set op mode: RECEIVER SINGLE\n");
            break;
        case SX127X_RF_OPMODE_RECEIVER:
            DEBUG("[sx127x] Set op mode: RECEIVER\n");
            break;
        case SX127X_RF_OPMODE_TRANSMITTER:
            DEBUG("[sx127x] Set op mode: TRANSMITTER\n");
            break;
        default:
            DEBUG("[sx127x] Set op mode: UNKNOWN (%d)\n", op_mode);
            break;
        }
    }

    /* Replace previous mode value and setup new mode value */
    sx127x_reg_write(dev, SX127X_REG_OPMODE,
                     (sx127x_reg_read(dev, SX127X_REG_OPMODE) & SX127X_RF_OPMODE_MASK) | op_mode);
}

uint8_t sx127x_get_bandwidth(const sx127x_t *dev)
{
    return dev->settings.lora.bandwidth;
}

static void _low_datarate_optimize(sx127x_t *dev)
{
    if ( ((dev->settings.lora.bandwidth == LORA_BW_125_KHZ) &&
          ((dev->settings.lora.datarate == LORA_SF11) ||
           (dev->settings.lora.datarate == LORA_SF12))) ||
         ((dev->settings.lora.bandwidth == LORA_BW_250_KHZ) &&
          (dev->settings.lora.datarate == LORA_SF12))) {
        dev->settings.lora.flags |= SX127X_LOW_DATARATE_OPTIMIZE_FLAG;
    } else {
        dev->settings.lora.flags &= ~SX127X_LOW_DATARATE_OPTIMIZE_FLAG;
    }

#if defined(MODULE_SX1272)
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1,
                     (sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1) &
                      SX127X_RF_LORA_MODEMCONFIG1_LOWDATARATEOPTIMIZE_MASK) |
                     ((dev->settings.lora.flags & SX127X_LOW_DATARATE_OPTIMIZE_FLAG)));
#else /* MODULE_SX1276 */
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG3,
                     (sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG3) &
                      SX127X_RF_LORA_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK) |
                     ((dev->settings.lora.flags & SX127X_LOW_DATARATE_OPTIMIZE_FLAG) << 3));
#endif
}

static void _update_bandwidth(const sx127x_t *dev)
{
    uint8_t config1_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1);
#if defined(MODULE_SX1272)
    config1_reg &= SX1272_RF_LORA_MODEMCONFIG1_BW_MASK;
    switch (dev->settings.lora.bandwidth) {
    case LORA_BW_125_KHZ:
        config1_reg |=  SX1272_RF_LORA_MODEMCONFIG1_BW_125_KHZ;
        break;
    case LORA_BW_250_KHZ:
        config1_reg |=  SX1272_RF_LORA_MODEMCONFIG1_BW_250_KHZ;
        break;
    case LORA_BW_500_KHZ:
        config1_reg |=  SX1272_RF_LORA_MODEMCONFIG1_BW_500_KHZ;
        break;
    default:
        DEBUG("Unsupported bandwidth, %d", dev->settings.lora.bandwidth);
        break;
    }
#else /* MODULE_SX1276 */
    config1_reg &= SX1276_RF_LORA_MODEMCONFIG1_BW_MASK;
    switch (dev->settings.lora.bandwidth) {
    case LORA_BW_125_KHZ:
        config1_reg |= SX1276_RF_LORA_MODEMCONFIG1_BW_125_KHZ;
        break;
    case LORA_BW_250_KHZ:
        config1_reg |=  SX1276_RF_LORA_MODEMCONFIG1_BW_250_KHZ;
        break;
    case LORA_BW_500_KHZ:
        config1_reg |=  SX1276_RF_LORA_MODEMCONFIG1_BW_500_KHZ;
        break;
    default:
        DEBUG("[sx127x] Unsupported bandwidth, %d\n",
              dev->settings.lora.bandwidth);
        break;
    }
#endif
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1, config1_reg);
}

void sx127x_set_bandwidth(sx127x_t *dev, uint8_t bandwidth)
{
    DEBUG("[sx127x] Set bandwidth: %d\n", bandwidth);

    dev->settings.lora.bandwidth = bandwidth;

    _update_bandwidth((const sx127x_t *)dev);

    _low_datarate_optimize(dev);

    /* ERRATA sensitivity tweaks */
    if ((dev->settings.lora.bandwidth == LORA_BW_500_KHZ) &&
        (dev->settings.channel > SX127X_RF_MID_BAND_THRESH)) {
        /* ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth */
        sx127x_reg_write(dev, SX127X_REG_LR_TEST36, 0x02);
        sx127x_reg_write(dev, SX127X_REG_LR_TEST3A, 0x64);
    }
    else if (dev->settings.lora.bandwidth == LORA_BW_500_KHZ) {
        /* ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth */
        sx127x_reg_write(dev, SX127X_REG_LR_TEST36, 0x02);
        sx127x_reg_write(dev, SX127X_REG_LR_TEST3A, 0x7F);
    }
    else {
        /* ERRATA 2.1 - Sensitivity Optimization with another Bandwidth */
        sx127x_reg_write(dev, SX127X_REG_LR_TEST36, 0x03);
    }
}

uint8_t sx127x_get_spreading_factor(const sx127x_t *dev)
{
    return dev->settings.lora.datarate;
}

void sx127x_set_spreading_factor(sx127x_t *dev, uint8_t datarate)
{
    DEBUG("[sx127x] Set spreading factor: %d\n", datarate);

    if (datarate == LORA_SF6 &&
        !(dev->settings.lora.flags & SX127X_ENABLE_FIXED_HEADER_LENGTH_FLAG)) {
        /* SF 6 is only valid when using explicit header mode */
        DEBUG("Spreading Factor 6 can only be used when explicit header "
              "mode is set, this mode is not supported by this driver."
              "Ignoring.\n");
        return;
    }

    dev->settings.lora.datarate = datarate;

    uint8_t config2_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG2);
    config2_reg &= SX127X_RF_LORA_MODEMCONFIG2_SF_MASK;
    config2_reg |= datarate << 4;
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG2, config2_reg);

    _low_datarate_optimize(dev);

    switch(dev->settings.lora.datarate) {
    case LORA_SF6:
        sx127x_reg_write(dev, SX127X_REG_LR_DETECTOPTIMIZE,
                         SX127X_RF_LORA_DETECTIONOPTIMIZE_SF6);
        sx127x_reg_write(dev, SX127X_REG_LR_DETECTIONTHRESHOLD,
                         SX127X_RF_LORA_DETECTIONTHRESH_SF6);
        break;
    default:
        sx127x_reg_write(dev, SX127X_REG_LR_DETECTOPTIMIZE,
                         SX127X_RF_LORA_DETECTIONOPTIMIZE_SF7_TO_SF12);
        sx127x_reg_write(dev, SX127X_REG_LR_DETECTIONTHRESHOLD,
                         SX127X_RF_LORA_DETECTIONTHRESH_SF7_TO_SF12);
        break;
    }
}

uint8_t sx127x_get_coding_rate(const sx127x_t *dev)
{
    return dev->settings.lora.coderate;
}

void sx127x_set_coding_rate(sx127x_t *dev, uint8_t coderate)
{
    DEBUG("[sx127x] Set coding rate: %d\n", coderate);

    dev->settings.lora.coderate = coderate;
    uint8_t config1_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1);

#if defined(MODULE_SX1272)
    config1_reg &= SX1272_RF_LORA_MODEMCONFIG1_CODINGRATE_MASK;
    config1_reg |= coderate << 3;
#else /* MODULE_SX1276 */
    config1_reg &= SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_MASK;
    config1_reg |= coderate << 1;
#endif

    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1, config1_reg);
}

static inline void _set_flag(sx127x_t *dev, uint8_t flag, bool value)
{
    if (value) {
        dev->settings.lora.flags |= flag;
    }
    else {
        dev->settings.lora.flags &= ~flag;
    }
}

bool sx127x_get_rx_single(const sx127x_t *dev)
{
    return !(dev->settings.lora.flags & SX127X_RX_CONTINUOUS_FLAG);
}

void sx127x_set_rx_single(sx127x_t *dev, bool single)
{
    DEBUG("[sx127x] Set RX single: %d\n", single);
    _set_flag(dev, SX127X_RX_CONTINUOUS_FLAG, !single);
}

bool sx127x_get_crc(const sx127x_t *dev)
{
#if defined(MODULE_SX1272)
    return (sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1) &
            SX1272_RF_LORA_MODEMCONFIG1_RXPAYLOADCRC_MASK);
#else /* MODULE_SX1276 */
    return (sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG2) &
            SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_MASK);
#endif
}

void sx127x_set_crc(sx127x_t *dev, bool crc)
{
    DEBUG("[sx127x] Set CRC: %d\n", crc);
    _set_flag(dev, SX127X_ENABLE_CRC_FLAG, crc);

#if defined(MODULE_SX1272)
    uint8_t config2_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1);
    config2_reg &= SX1272_RF_LORA_MODEMCONFIG1_RXPAYLOADCRC_MASK;
    config2_reg |= crc << 1;
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1, config2_reg);
#else /* MODULE_SX1276 */
    uint8_t config2_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG2);
    config2_reg &= SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_MASK;
    config2_reg |= crc << 2;
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG2, config2_reg);
#endif
}

uint8_t sx127x_get_hop_period(const sx127x_t *dev)
{
    return sx127x_reg_read(dev, SX127X_REG_LR_HOPPERIOD);
}

void sx127x_set_hop_period(sx127x_t *dev, uint8_t hop_period)
{
    DEBUG("[sx127x] Set Hop period: %d\n", hop_period);

    dev->settings.lora.freq_hop_period = hop_period;

    uint8_t tmp = sx127x_reg_read(dev, SX127X_REG_LR_PLLHOP);
    if (dev->settings.lora.flags & SX127X_CHANNEL_HOPPING_FLAG) {
        tmp |= SX127X_RF_LORA_PLLHOP_FASTHOP_ON;
        sx127x_reg_write(dev, SX127X_REG_LR_PLLHOP, tmp);
        sx127x_reg_write(dev, SX127X_REG_LR_HOPPERIOD, hop_period);
    }
}

bool  sx127x_get_fixed_header_len_mode(const sx127x_t *dev)
{
    return dev->settings.lora.flags & SX127X_ENABLE_FIXED_HEADER_LENGTH_FLAG;
}

void sx127x_set_fixed_header_len_mode(sx127x_t *dev, bool fixed_len)
{
    DEBUG("[sx127x] Set fixed header length: %d\n", fixed_len);

    _set_flag(dev, SX127X_ENABLE_FIXED_HEADER_LENGTH_FLAG, fixed_len);

    uint8_t config1_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1);
#if defined(MODULE_SX1272)
    config1_reg &= SX1272_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_MASK;
    config1_reg |= fixed_len << 2;
#else /* MODULE_SX1276 */
    config1_reg &= SX1276_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_MASK;
    config1_reg |= fixed_len;
#endif
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1, config1_reg);
}

uint8_t sx127x_get_payload_length(const sx127x_t *dev)
{
    return sx127x_reg_read(dev, SX127X_REG_LR_PAYLOADLENGTH);;
}

void sx127x_set_payload_length(sx127x_t *dev, uint8_t len)
{
    DEBUG("[sx127x] Set payload len: %d\n", len);

    sx127x_reg_write(dev, SX127X_REG_LR_PAYLOADLENGTH, len);
}

static inline uint8_t sx127x_get_pa_select(const sx127x_t *dev)
{
    if (dev->params.paselect == SX127X_PA_BOOST) {
        return SX127X_RF_PACONFIG_PASELECT_PABOOST;
    }

    return SX127X_RF_PACONFIG_PASELECT_RFO;
}

uint8_t sx127x_get_tx_power(const sx127x_t *dev)
{
    return dev->settings.lora.power;
}

void sx127x_set_tx_power(sx127x_t *dev, int8_t power)
{
    DEBUG("[sx127x] Set power: %d\n", power);

    dev->settings.lora.power = power;

    uint8_t pa_config = sx127x_reg_read(dev, SX127X_REG_PACONFIG);
#if defined(MODULE_SX1272)
    uint8_t pa_dac = sx127x_reg_read(dev, SX1272_REG_PADAC);
#else /* MODULE_SX1276 */
    uint8_t pa_dac = sx127x_reg_read(dev, SX1276_REG_PADAC);
#endif

    pa_config = ((pa_config & SX127X_RF_PACONFIG_PASELECT_MASK) |
                 sx127x_get_pa_select(dev));

#if defined(MODULE_SX1276)
    /* max power is 14dBm */
    pa_config = (pa_config & SX127X_RF_PACONFIG_MAX_POWER_MASK) | 0x70;
#endif

    sx127x_reg_write(dev, SX127X_REG_PARAMP, SX127X_RF_PARAMP_0050_US);

    if ((pa_config & SX127X_RF_PACONFIG_PASELECT_PABOOST)
        == SX127X_RF_PACONFIG_PASELECT_PABOOST) {
        if (power > 17) {
            pa_dac = ((pa_dac & SX127X_RF_PADAC_20DBM_MASK) |
                      SX127X_RF_PADAC_20DBM_ON);
        } else {
            pa_dac = ((pa_dac & SX127X_RF_PADAC_20DBM_MASK) |
                      SX127X_RF_PADAC_20DBM_OFF);
        }
        if ((pa_dac & SX127X_RF_PADAC_20DBM_ON) == SX127X_RF_PADAC_20DBM_ON) {
            if (power < 5) {
                power = 5;
            }
            if (power > 20) {
                power = 20;
            }

            pa_config = ((pa_config & SX127X_RF_PACONFIG_OUTPUTPOWER_MASK) |
                         (uint8_t)((uint16_t)(power - 5) & 0x0F));
        } else {
            if (power < 2) {
                power = 2;
            }
            if (power > 17) {
                power = 17;
            }

            pa_config = ((pa_config & SX127X_RF_PACONFIG_OUTPUTPOWER_MASK) |
                         (uint8_t)((uint16_t)(power - 2) & 0x0F));
        }
    } else {
        if (power < -1) {
            power = -1;
        }
        if (power > 14) {
            power = 14;
        }

        pa_config = ((pa_config & SX127X_RF_PACONFIG_OUTPUTPOWER_MASK) |
                     (uint8_t)((uint16_t)(power + 1) & 0x0F));
    }

    sx127x_reg_write(dev, SX127X_REG_PACONFIG, pa_config);
#if defined(MODULE_SX1272)
    sx127x_reg_write(dev, SX1272_REG_PADAC, pa_dac);
#else /* MODULE_SX1276 */
    sx127x_reg_write(dev, SX1276_REG_PADAC, pa_dac);
#endif
}

uint16_t sx127x_get_preamble_length(const sx127x_t *dev)
{
    return dev->settings.lora.preamble_len;
}

void sx127x_set_preamble_length(sx127x_t *dev, uint16_t preamble)
{
    DEBUG("[sx127x] Set preamble length: %d\n", preamble);

    dev->settings.lora.preamble_len = preamble;

    sx127x_reg_write(dev, SX127X_REG_LR_PREAMBLEMSB,
                     (preamble >> 8) & 0xFF);
    sx127x_reg_write(dev, SX127X_REG_LR_PREAMBLELSB,
                     preamble & 0xFF);
}

void sx127x_set_rx_timeout(sx127x_t *dev, uint32_t timeout)
{
    DEBUG("[sx127x] Set RX timeout: %" PRIu32 "\n", timeout);

    dev->settings.lora.rx_timeout = timeout;
}

void sx127x_set_tx_timeout(sx127x_t *dev, uint32_t timeout)
{
    DEBUG("[sx127x] Set TX timeout: %" PRIu32 "\n", timeout);

    dev->settings.lora.tx_timeout = timeout;
}

void sx127x_set_symbol_timeout(sx127x_t *dev, uint16_t timeout)
{
    DEBUG("[sx127x] Set symbol timeout: %d\n", timeout);

    uint8_t config2_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG2);
    config2_reg &= SX127X_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK;
    config2_reg |= (timeout >> 8) & ~SX127X_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK;
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG2, config2_reg);
    sx127x_reg_write(dev, SX127X_REG_LR_SYMBTIMEOUTLSB, timeout & 0xFF);
}

bool sx127x_get_iq_invert(const sx127x_t *dev)
{
    return dev->settings.lora.flags & SX127X_IQ_INVERTED_FLAG;
}

void sx127x_set_iq_invert(sx127x_t *dev, bool iq_invert)
{
    DEBUG("[sx127x] Set IQ invert: %d\n", iq_invert);

    _set_flag(dev, SX127X_IQ_INVERTED_FLAG, iq_invert);

    sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ,
                     (sx127x_reg_read(dev, SX127X_REG_LR_INVERTIQ) &
                      SX127X_RF_LORA_INVERTIQ_RX_MASK &
                      SX127X_RF_LORA_INVERTIQ_TX_MASK) |
                      SX127X_RF_LORA_INVERTIQ_RX_OFF |
                     (iq_invert ? SX127X_RF_LORA_INVERTIQ_TX_ON : SX127X_RF_LORA_INVERTIQ_TX_OFF));

    sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ2,
                     (iq_invert ? SX127X_RF_LORA_INVERTIQ2_ON : SX127X_RF_LORA_INVERTIQ2_OFF));
}

void sx127x_set_freq_hop(sx127x_t *dev, bool freq_hop_on)
{
    DEBUG("[sx127x] Set freq hop: %d\n", freq_hop_on);

     _set_flag(dev, SX127X_CHANNEL_HOPPING_FLAG, freq_hop_on);
}
