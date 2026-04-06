/*
 * SPDX-FileCopyrightText: 2016 Fundación Inria Chile
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sx127x
 * @{
 * @file
 * @brief       Netdev adaptation for the sx127x driver
 *
 * @author      Eugene P. <ep@unwds.com>
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @}
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "ztimer.h"

#include "net/netopt.h"
#include "net/netdev.h"
#include "net/netdev/lora.h"
#include "net/lora.h"

#include "sx127x_registers.h"
#include "sx127x_internal.h"
#include "sx127x_netdev.h"
#include "sx127x.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Internal helper functions */
static int _set_state(sx127x_t *dev, netopt_state_t state);
static int _get_state(sx127x_t *dev, void *val);
void _on_dio0_irq(void *arg);
void _on_dio1_irq(void *arg);
void _on_dio2_irq(void *arg);
void _on_dio3_irq(void *arg);

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    DEBUG("[sx127x] Sending packet now.\n");
    sx127x_t *dev = container_of(netdev, sx127x_t, netdev);

    if (sx127x_get_state(dev) == SX127X_RF_TX_RUNNING) {
        DEBUG("[sx127x] Cannot send packet: radio already in transmitting "
              "state.\n");
        return -ENOTSUP;
    }

    uint8_t size = iolist_size(iolist);

    /* Ignore send if packet size is 0 */
    if (size == 0) {
        return 0;
    }

    switch (dev->settings.modem) {
    case SX127X_MODEM_FSK:
        /* todo */
        break;
    case SX127X_MODEM_LORA:
        /* Initializes the payload size */
        if (!sx127x_get_fixed_header_len_mode(dev)) {
            DEBUG("[sx127x] Modem option is LoRa.\n");
            sx127x_set_payload_length(dev, size);
        }

        /* Full buffer used for Tx */
        sx127x_reg_write(dev, SX127X_REG_LR_FIFOTXBASEADDR, 0x00);
        sx127x_reg_write(dev, SX127X_REG_LR_FIFOADDRPTR, 0x00);

        /* FIFO operations can not take place in Sleep mode
         * So wake up the chip */
        if (sx127x_get_op_mode(dev) == SX127X_RF_OPMODE_SLEEP) {
            sx127x_set_standby(dev);
            DEBUG("[sx127x] Waiting for chip to wake up.\n");
            ztimer_sleep(ZTIMER_MSEC, SX127X_RADIO_WAKEUP_TIME);     /* wait for chip wake up */
        }

        /* Write payload buffer */
        for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
            if (iol->iol_len > 0) {
                sx127x_write_fifo(dev, iol->iol_base, iol->iol_len);
                DEBUG("[sx127x] Wrote to payload buffer.\n");
            }
        }
        break;
    default:
        DEBUG("[sx127x] netdev: Unsupported modem (%d)\n",
              dev->settings.modem);
        break;
    }

    sx127x_set_tx(dev);

    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    sx127x_t *dev = container_of(netdev, sx127x_t, netdev);
    volatile uint8_t irq_flags = 0;
    uint8_t size = 0;

    switch (dev->settings.modem) {
    case SX127X_MODEM_FSK:
        /* todo */
        break;
    case SX127X_MODEM_LORA:
        /* Clear IRQ */
        sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS, SX127X_RF_LORA_IRQFLAGS_RXDONE);

        irq_flags = sx127x_reg_read(dev, SX127X_REG_LR_IRQFLAGS);
        if ((irq_flags & SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR_MASK) ==
            SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR) {
            /* Clear IRQ */
            sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS,
                             SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR);

            if (!(dev->settings.lora.flags & SX127X_RX_CONTINUOUS_FLAG)) {
                sx127x_set_state(dev, SX127X_RF_IDLE);
            }

            ztimer_remove(ZTIMER_MSEC, &dev->_internal.rx_timeout_timer);
            netdev->event_callback(netdev, NETDEV_EVENT_CRC_ERROR);
            return -EBADMSG;
        }

        netdev_lora_rx_info_t *packet_info = info;
        if (packet_info) {
            uint8_t snr_value = sx127x_reg_read(dev, SX127X_REG_LR_PKTSNRVALUE);
            if (snr_value & 0x80) {     /* The SNR is negative */
                /* Invert and divide by 4 */
                packet_info->snr = -1 * ((~snr_value + 1) & 0xFF) >> 2;
            }
            else {
                /* Divide by 4 */
                packet_info->snr = (snr_value & 0xFF) >> 2;
            }

            int16_t rssi = sx127x_reg_read(dev, SX127X_REG_LR_PKTRSSIVALUE);

            if (packet_info->snr < 0) {
#if defined(MODULE_SX1272)
                packet_info->rssi = SX127X_RSSI_OFFSET + rssi + (rssi >> 4) + packet_info->snr;
#else /* MODULE_SX1276 */
                if (dev->settings.channel > SX127X_RF_MID_BAND_THRESH) {
                    packet_info->rssi = SX127X_RSSI_OFFSET_HF + rssi + (rssi >> 4) +
                                        packet_info->snr;
                }
                else {
                    packet_info->rssi = SX127X_RSSI_OFFSET_LF + rssi + (rssi >> 4) +
                                        packet_info->snr;
                }
#endif
            }
            else {
#if defined(MODULE_SX1272)
                packet_info->rssi = SX127X_RSSI_OFFSET + rssi + (rssi >> 4);
#else /* MODULE_SX1276 */
                if (dev->settings.channel > SX127X_RF_MID_BAND_THRESH) {
                    packet_info->rssi = SX127X_RSSI_OFFSET_HF + rssi + (rssi >> 4);
                }
                else {
                    packet_info->rssi = SX127X_RSSI_OFFSET_LF + rssi + (rssi >> 4);
                }
#endif
            }
        }

        size = sx127x_reg_read(dev, SX127X_REG_LR_RXNBBYTES);
        if (buf == NULL) {
            return size;
        }

        if (size > len) {
            return -ENOBUFS;
        }

        if (!(dev->settings.lora.flags & SX127X_RX_CONTINUOUS_FLAG)) {
            sx127x_set_state(dev, SX127X_RF_IDLE);
        }

        ztimer_remove(ZTIMER_MSEC, &dev->_internal.rx_timeout_timer);
        /* Read the last packet from FIFO */
        uint8_t last_rx_addr = sx127x_reg_read(dev, SX127X_REG_LR_FIFORXCURRENTADDR);
        sx127x_reg_write(dev, SX127X_REG_LR_FIFOADDRPTR, last_rx_addr);
        sx127x_read_fifo(dev, (uint8_t *)buf, size);
        break;
    default:
        break;
    }

    return size;
}

static int _init(netdev_t *netdev)
{
    sx127x_t *sx127x = container_of(netdev, sx127x_t, netdev);

    sx127x->irq = 0;
    sx127x_radio_settings_t settings;

    settings.channel = SX127X_CHANNEL_DEFAULT;
    settings.modem = SX127X_MODEM_DEFAULT;
    settings.state = SX127X_RF_IDLE;

    sx127x->settings = settings;

    /* Launch initialization of driver and device */
    DEBUG("[sx127x] netdev: initializing driver...\n");
    if (sx127x_init(sx127x) != SX127X_INIT_OK) {
        DEBUG("[sx127x] netdev: initialization failed\n");
        return -1;
    }

    sx127x_init_radio_settings(sx127x);
    /* Put chip into sleep */
    sx127x_set_sleep(sx127x);

    DEBUG("[sx127x] netdev: initialization done\n");

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static void _isr(netdev_t *netdev)
{
    sx127x_t *dev = container_of(netdev, sx127x_t, netdev);

    uint8_t interruptReg = sx127x_reg_read(dev, SX127X_REG_LR_IRQFLAGS);

    if (interruptReg & (SX127X_RF_LORA_IRQFLAGS_TXDONE |
                        SX127X_RF_LORA_IRQFLAGS_RXDONE)) {
        _on_dio0_irq(dev);
    }

    if (interruptReg & SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT) {
        _on_dio1_irq(dev);
    }

    if (interruptReg & SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL) {
        _on_dio2_irq(dev);
    }

    if (interruptReg & (SX127X_RF_LORA_IRQFLAGS_CADDETECTED |
                        SX127X_RF_LORA_IRQFLAGS_CADDONE |
                        SX127X_RF_LORA_IRQFLAGS_VALIDHEADER)) {
        _on_dio3_irq(dev);
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    (void)max_len;   /* unused when compiled without debug, assert empty */
    sx127x_t *dev = container_of(netdev, sx127x_t, netdev);

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
    case NETOPT_STATE:
        assert(max_len >= sizeof(netopt_state_t));
        return _get_state(dev, val);

    case NETOPT_DEVICE_TYPE:
        assert(max_len >= sizeof(uint16_t));
        *((uint16_t *)val) = NETDEV_TYPE_LORA;
        return sizeof(uint16_t);

    case NETOPT_CHANNEL_FREQUENCY:
        assert(max_len >= sizeof(uint32_t));
        *((uint32_t *)val) = sx127x_get_channel(dev);
        return sizeof(uint32_t);

    case NETOPT_BANDWIDTH:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx127x_get_bandwidth(dev);
        return sizeof(uint8_t);

    case NETOPT_SPREADING_FACTOR:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx127x_get_spreading_factor(dev);
        return sizeof(uint8_t);

    case NETOPT_CODING_RATE:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx127x_get_coding_rate(dev);
        return sizeof(uint8_t);

    case NETOPT_MAX_PDU_SIZE:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx127x_get_max_payload_len(dev);
        return sizeof(uint8_t);

    case NETOPT_INTEGRITY_CHECK:
        assert(max_len >= sizeof(netopt_enable_t));
        *((netopt_enable_t *)val) = sx127x_get_crc(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);

    case NETOPT_CHANNEL_HOP:
        assert(max_len >= sizeof(netopt_enable_t));
        *((netopt_enable_t *)val) =
            (dev->settings.lora.flags &
             SX127X_CHANNEL_HOPPING_FLAG) ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);

    case NETOPT_CHANNEL_HOP_PERIOD:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = sx127x_get_hop_period(dev);
        return sizeof(uint8_t);

    case NETOPT_SINGLE_RECEIVE:
        assert(max_len >= sizeof(uint8_t));
        *((netopt_enable_t *)val) = sx127x_get_rx_single(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);

    case NETOPT_TX_POWER:
        assert(max_len >= sizeof(int16_t));
        *((int16_t *)val) = (int16_t)sx127x_get_tx_power(dev);
        return sizeof(int16_t);

    case NETOPT_SYNCWORD:
        assert(max_len >= sizeof(uint8_t));
        *((uint8_t *)val) = (uint8_t)sx127x_get_syncword(dev);
        return sizeof(uint8_t);

    case NETOPT_RANDOM:
        assert(max_len >= sizeof(uint32_t));
        *((uint32_t *)val) = (uint32_t)sx127x_random(dev);
        return sizeof(uint32_t);

    case NETOPT_IQ_INVERT:
        assert(max_len >= sizeof(uint8_t));
        *((netopt_enable_t *)val) = sx127x_get_iq_invert(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);

    case NETOPT_RSSI:
        assert(max_len >= sizeof(int16_t));
        *((int16_t *)val) = sx127x_read_rssi(dev);
        return sizeof(int16_t);

    default:
        break;
    }

    return -ENOTSUP;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    (void)len; /* unused when compiled without debug, assert empty */

    sx127x_t *dev = container_of(netdev, sx127x_t, netdev);
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
    case NETOPT_STATE:
        assert(len == sizeof(netopt_state_t));
        return _set_state(dev, *((const netopt_state_t *)val));

    case NETOPT_DEVICE_TYPE:
        assert(len <= sizeof(uint16_t));
        /* Only LoRa modem is supported for the moment */
        if (*(const uint16_t *)val == NETDEV_TYPE_LORA) {
            sx127x_set_modem(dev, SX127X_MODEM_LORA);
            return sizeof(uint16_t);
        }
        else {
            return -EINVAL;
        }

    case NETOPT_CHANNEL_FREQUENCY:
        assert(len <= sizeof(uint32_t));
        sx127x_set_channel(dev, *((const uint32_t *)val));
        return sizeof(uint32_t);

    case NETOPT_BANDWIDTH:
        assert(len <= sizeof(uint8_t));
        uint8_t bw = *((const uint8_t *)val);
        if (bw > LORA_BW_500_KHZ) {
            res = -EINVAL;
            break;
        }
        sx127x_set_bandwidth(dev, bw);
        return sizeof(uint8_t);

    case NETOPT_SPREADING_FACTOR:
        assert(len <= sizeof(uint8_t));
        uint8_t sf = *((const uint8_t *)val);
        if ((sf < LORA_SF6) || (sf > LORA_SF12)) {
            res = -EINVAL;
            break;
        }
        sx127x_set_spreading_factor(dev, sf);
        return sizeof(uint8_t);

    case NETOPT_CODING_RATE:
        assert(len <= sizeof(uint8_t));
        uint8_t cr = *((const uint8_t *)val);
        if ((cr < LORA_CR_4_5) || (cr > LORA_CR_4_8)) {
            res = -EINVAL;
            break;
        }
        sx127x_set_coding_rate(dev, cr);
        return sizeof(uint8_t);

    case NETOPT_MAX_PDU_SIZE:
        assert(len <= sizeof(uint8_t));
        sx127x_set_max_payload_len(dev, *((const uint8_t *)val));
        return sizeof(uint8_t);

    case NETOPT_INTEGRITY_CHECK:
        assert(len <= sizeof(netopt_enable_t));
        sx127x_set_crc(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(netopt_enable_t);

    case NETOPT_CHANNEL_HOP:
        assert(len <= sizeof(netopt_enable_t));
        sx127x_set_freq_hop(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(netopt_enable_t);

    case NETOPT_CHANNEL_HOP_PERIOD:
        assert(len <= sizeof(uint8_t));
        sx127x_set_hop_period(dev, *((const uint8_t *)val));
        return sizeof(uint8_t);

    case NETOPT_SINGLE_RECEIVE:
        assert(len <= sizeof(netopt_enable_t));
        sx127x_set_rx_single(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(netopt_enable_t);

    case NETOPT_RX_SYMBOL_TIMEOUT:
        assert(len <= sizeof(uint16_t));
        sx127x_set_symbol_timeout(dev, *((const uint16_t *)val));
        return sizeof(uint16_t);

    case NETOPT_RX_TIMEOUT:
        assert(len <= sizeof(uint32_t));
        sx127x_set_rx_timeout(dev, *((const uint32_t *)val));
        return sizeof(uint32_t);

    case NETOPT_TX_TIMEOUT:
        assert(len <= sizeof(uint32_t));
        sx127x_set_tx_timeout(dev, *((const uint32_t *)val));
        return sizeof(uint32_t);

    case NETOPT_TX_POWER:
        assert(len <= sizeof(int16_t));
        int16_t power = *((const int16_t *)val);
        if ((power < INT8_MIN) || (power > INT8_MAX)) {
            res = -EINVAL;
            break;
        }
        sx127x_set_tx_power(dev, (int8_t)power);
        return sizeof(int16_t);

    case NETOPT_FIXED_HEADER:
        assert(len <= sizeof(netopt_enable_t));
        sx127x_set_fixed_header_len_mode(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(netopt_enable_t);

    case NETOPT_PDU_SIZE:
        assert(len <= sizeof(uint16_t));
        sx127x_set_payload_length(dev, *((const uint16_t *)val));
        return sizeof(uint16_t);

    case NETOPT_PREAMBLE_LENGTH:
        assert(len <= sizeof(uint16_t));
        sx127x_set_preamble_length(dev, *((const uint16_t *)val));
        return sizeof(uint16_t);

    case NETOPT_SYNCWORD:
        assert(len <= sizeof(uint8_t));
        sx127x_set_syncword(dev, *((uint8_t *)val));
        return sizeof(uint8_t);

    case NETOPT_IQ_INVERT:
        assert(len <= sizeof(netopt_enable_t));
        sx127x_set_iq_invert(dev, *((const netopt_enable_t *)val) ? true : false);
        return sizeof(bool);

    default:
        break;
    }

    return res;
}

static int _set_state(sx127x_t *dev, netopt_state_t state)
{
    switch (state) {
    case NETOPT_STATE_SLEEP:
        sx127x_set_sleep(dev);
        break;

    case NETOPT_STATE_STANDBY:
        sx127x_set_standby(dev);
        break;

    case NETOPT_STATE_IDLE:
        /* set permanent listening */
        sx127x_set_rx_timeout(dev, 0);
        sx127x_set_rx(dev);
        break;

    case NETOPT_STATE_RX:
        sx127x_set_rx(dev);
        break;

    case NETOPT_STATE_TX:
        sx127x_set_tx(dev);
        break;

    case NETOPT_STATE_RESET:
        sx127x_reset(dev);
        break;

    default:
        return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static int _get_state(sx127x_t *dev, void *val)
{
    uint8_t op_mode;

    op_mode = sx127x_get_op_mode(dev);
    netopt_state_t state = NETOPT_STATE_OFF;

    switch (op_mode) {
    case SX127X_RF_OPMODE_SLEEP:
        state = NETOPT_STATE_SLEEP;
        break;

    case SX127X_RF_OPMODE_STANDBY:
        state = NETOPT_STATE_STANDBY;
        break;

    case SX127X_RF_OPMODE_TRANSMITTER:
        state = NETOPT_STATE_TX;
        break;

    case SX127X_RF_OPMODE_RECEIVER:
    case SX127X_RF_LORA_OPMODE_RECEIVER_SINGLE:
        /* Sx127x is in receive mode:
         * -> need to check if the device is currently receiving a packet */
        if (sx127x_reg_read(dev, SX127X_REG_LR_MODEMSTAT) &
            SX127X_RF_LORA_MODEMSTAT_MODEM_STATUS_SIGNAL_DETECTED) {
            state = NETOPT_STATE_RX;
        }
        else {
            state = NETOPT_STATE_IDLE;
        }
        break;

    default:
        break;
    }
    memcpy(val, &state, sizeof(netopt_state_t));
    return sizeof(netopt_state_t);
}

void _on_dio0_irq(void *arg)
{
    sx127x_t *dev = arg;
    netdev_t *netdev = &dev->netdev;

    switch (dev->settings.state) {
    case SX127X_RF_RX_RUNNING:
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        break;
    case SX127X_RF_TX_RUNNING:
        ztimer_remove(ZTIMER_MSEC, &dev->_internal.tx_timeout_timer);
        switch (dev->settings.modem) {
        case SX127X_MODEM_LORA:
            /* Clear IRQ */
            sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS,
                             SX127X_RF_LORA_IRQFLAGS_TXDONE);
        /* Intentional fall-through */
        case SX127X_MODEM_FSK:
        default:
            sx127x_set_state(dev, SX127X_RF_IDLE);
            netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
            break;
        }
        break;
    case SX127X_RF_IDLE:
        DEBUG("[sx127x] netdev: sx127x_on_dio0: IDLE state\n");
        break;
    default:
        DEBUG("[sx127x] netdev: sx127x_on_dio0: unknown state [%d]\n",
              dev->settings.state);
        break;
    }
}

void _on_dio1_irq(void *arg)
{
    /* Get interrupt context */
    sx127x_t *dev = arg;
    netdev_t *netdev = &dev->netdev;

    switch (dev->settings.state) {
    case SX127X_RF_RX_RUNNING:
        switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            /* todo */
            break;
        case SX127X_MODEM_LORA:
            ztimer_remove(ZTIMER_MSEC, &dev->_internal.rx_timeout_timer);
            /*  Clear Irq */
            sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS, SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT);
            sx127x_set_state(dev, SX127X_RF_IDLE);
            netdev->event_callback(netdev, NETDEV_EVENT_RX_TIMEOUT);
            break;
        default:
            break;
        }
        break;
    case SX127X_RF_TX_RUNNING:
        switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            /* todo */
            break;
        case SX127X_MODEM_LORA:
            break;
        default:
            break;
        }
        break;
    default:
        puts("[sx127x] netdev: sx127x_on_dio1: unknown state");
        break;
    }
}

void _on_dio2_irq(void *arg)
{
    /* Get interrupt context */
    sx127x_t *dev = arg;
    netdev_t *netdev = &dev->netdev;

    switch (dev->settings.state) {
    case SX127X_RF_RX_RUNNING:
        switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            /* todo */
            break;
        case SX127X_MODEM_LORA:
            if (dev->settings.lora.flags & SX127X_CHANNEL_HOPPING_FLAG) {
                /* Clear IRQ */
                sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS,
                                 SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL);

                dev->_internal.last_channel = (sx127x_reg_read(dev, SX127X_REG_LR_HOPCHANNEL) &
                                               SX127X_RF_LORA_HOPCHANNEL_CHANNEL_MASK);
                netdev->event_callback(netdev, NETDEV_EVENT_FHSS_CHANGE_CHANNEL);
            }

            break;
        default:
            break;
        }
        break;
    case SX127X_RF_TX_RUNNING:
        switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            break;
        case SX127X_MODEM_LORA:
            if (dev->settings.lora.flags & SX127X_CHANNEL_HOPPING_FLAG) {
                /* Clear IRQ */
                sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS,
                                 SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL);

                dev->_internal.last_channel = (sx127x_reg_read(dev, SX127X_REG_LR_HOPCHANNEL) &
                                               SX127X_RF_LORA_HOPCHANNEL_CHANNEL_MASK);
                netdev->event_callback(netdev, NETDEV_EVENT_FHSS_CHANGE_CHANNEL);
            }
            break;
        default:
            break;
        }
        break;
    default:
        puts("[sx127x] netdev: sx127x_on_dio2: unknown state");
        break;
    }
}

void _on_dio3_irq(void *arg)
{
    /* Get interrupt context */
    sx127x_t *dev = arg;
    netdev_t *netdev = &dev->netdev;

    switch (dev->settings.state) {
    case SX127X_RF_CAD:
        switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            break;
        case SX127X_MODEM_LORA:
            /* Clear IRQ */
            sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS,
                             SX127X_RF_LORA_IRQFLAGS_CADDETECTED |
                             SX127X_RF_LORA_IRQFLAGS_CADDONE);

            /* Send event message */
            dev->_internal.is_last_cad_success = ((sx127x_reg_read(dev, SX127X_REG_LR_IRQFLAGS) &
                                                   SX127X_RF_LORA_IRQFLAGS_CADDETECTED) ==
                                                  SX127X_RF_LORA_IRQFLAGS_CADDETECTED);
            netdev->event_callback(netdev, NETDEV_EVENT_CAD_DONE);
            break;
        default:
            puts("[sx127x] netdev: sx127x_on_dio3: unknown modem");
            break;
        }
        break;
    case SX127X_RF_RX_RUNNING:
        switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            break;
        case SX127X_MODEM_LORA:
            /* Clear IRQ */
            sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS, SX127X_RF_LORA_IRQFLAGS_VALIDHEADER);

            netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
            break;
        default:
            break;
        }
        break;
    default:
        DEBUG("[sx127x] netdev: sx127x_on_dio3: unknown state");
        /* at least the related interrupts should be cleared in this case */
        sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGS,
                         SX127X_RF_LORA_IRQFLAGS_VALIDHEADER |
                         SX127X_RF_LORA_IRQFLAGS_CADDETECTED |
                         SX127X_RF_LORA_IRQFLAGS_CADDONE);
        break;
    }
}

const netdev_driver_t sx127x_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};
