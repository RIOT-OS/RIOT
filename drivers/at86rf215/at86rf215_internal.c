/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Low-Level functions for the AT86RF215 driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "periph/spi.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "at86rf215_internal.h"

#include <string.h>

#define SPIDEV          (dev->params.spi)
#define CSPIN           (dev->params.cs_pin)

static inline void getbus(const at86rf215_t *dev)
{
    spi_acquire(SPIDEV, CSPIN, SPI_MODE_0, dev->params.spi_clk);
}

/* only to be used by at86rf215_hardware_reset()
   can't use normal at86rf215_reg_read() because
   we already hold the lock */
static inline uint8_t _get_rf_state_with_lock(at86rf215_t *dev)
{
    uint16_t reg = htons(dev->RF->RG_STATE | FLAG_READ);
    spi_transfer_bytes(SPIDEV, CSPIN, true, &reg, NULL, sizeof(reg));
    return spi_transfer_byte(SPIDEV, CSPIN, false, 0) & STATE_STATE_MASK;
}

int at86rf215_hardware_reset(at86rf215_t *dev)
{
    /* bail out if no module is connected */
    if (at86rf215_reg_read(dev, RG_RF_PN) == 0) {
        return -ENODEV;
    }

    /* prevent access during reset */
    getbus(dev);

    /* trigger hardware reset */
    gpio_clear(dev->params.reset_pin);
    xtimer_usleep(AT86RF215_RESET_PULSE_WIDTH);
    gpio_set(dev->params.reset_pin);
    xtimer_usleep(AT86RF215_RESET_DELAY);

    while (_get_rf_state_with_lock(dev) != RF_STATE_TRXOFF) {}
    spi_release(SPIDEV);

    /* clear interrupts */
    at86rf215_reg_read(dev, RG_RF09_IRQS);
    at86rf215_reg_read(dev, RG_RF24_IRQS);
    at86rf215_reg_read(dev, RG_BBC0_IRQS);
    at86rf215_reg_read(dev, RG_BBC1_IRQS);

    return 0;
}

void at86rf215_reg_write(const at86rf215_t *dev, uint16_t reg, uint8_t value)
{
    reg = htons(reg | FLAG_WRITE);

    getbus(dev);
    spi_transfer_bytes(SPIDEV, CSPIN, true, &reg, NULL, sizeof(reg));
    spi_transfer_byte(SPIDEV, CSPIN, false, value);
    spi_release(SPIDEV);
}

void at86rf215_reg_write_bytes(const at86rf215_t *dev, uint16_t reg, const void *data, size_t len)
{
    reg = htons(reg | FLAG_WRITE);

    getbus(dev);
    spi_transfer_bytes(SPIDEV, CSPIN, true, &reg, NULL, sizeof(reg));
    spi_transfer_bytes(SPIDEV, CSPIN, false, data, NULL, len);
    spi_release(SPIDEV);
}

uint8_t at86rf215_reg_read(const at86rf215_t *dev, uint16_t reg)
{
    uint8_t val;

    reg = htons(reg | FLAG_READ);

    getbus(dev);
    spi_transfer_bytes(SPIDEV, CSPIN, true, &reg, NULL, sizeof(reg));
    val = spi_transfer_byte(SPIDEV, CSPIN, false, 0);
    spi_release(SPIDEV);

    return val;
}

void at86rf215_reg_read_bytes(const at86rf215_t *dev, uint16_t reg, void *data, size_t len)
{
    reg = htons(reg | FLAG_READ);

    getbus(dev);
    spi_transfer_bytes(SPIDEV, CSPIN, true, &reg, NULL, sizeof(reg));
    spi_transfer_bytes(SPIDEV, CSPIN, false, NULL, data, len);
    spi_release(SPIDEV);
}

void at86rf215_filter_ack(at86rf215_t *dev, bool on)
{
    /* only listen for ACK frames */
    uint8_t val = on ? (1 << IEEE802154_FCF_TYPE_ACK)
                     : (1 << IEEE802154_FCF_TYPE_BEACON)
                     | (1 << IEEE802154_FCF_TYPE_DATA)
                     | (1 << IEEE802154_FCF_TYPE_MACCMD);

    at86rf215_reg_write(dev, dev->BBC->RG_AFFTM, val);
}

void at86rf215_get_random(at86rf215_t *dev, void *data, size_t len)
{
    uint8_t state = at86rf215_reg_read(dev, dev->BBC->RG_PC);
    at86rf215_disable_baseband(dev);

    uint8_t rxbwc = at86rf215_reg_read(dev, dev->RF->RG_RXBWC);

    /* The analog frontend of the radio must be configured to the
       widest filter bandwidth; The bit RXBWC.IFS must be set to 1 */
    at86rf215_reg_write(dev, dev->RF->RG_RXBWC, 0x1B);

    uint8_t *data8 = data;
    while (len--) {
        *data8++ = at86rf215_reg_read(dev, dev->RF->RG_RNDV);
    }

    at86rf215_reg_write(dev, dev->RF->RG_RXBWC, rxbwc);
    /* set baseband in prior state */
    at86rf215_reg_write(dev, dev->BBC->RG_PC, state);

}

uint16_t at86rf215_chan_valid(at86rf215_t *dev, uint16_t chan)
{
    if (is_subGHz(dev)) {
        if (chan >= dev->num_chans) {
            return dev->num_chans - 1;
        }
    } else {
        if (chan < IEEE802154_CHANNEL_MIN) {
            return IEEE802154_CHANNEL_MIN;
        } else if (chan >= IEEE802154_CHANNEL_MIN + dev->num_chans) {
            return IEEE802154_CHANNEL_MIN + dev->num_chans - 1;
        }
    }

    return chan;
}

#include "net/gnrc/netif/internal.h"
bool at86rf215_switch_mode(at86rf215_t *dev, uint8_t new_mode)
{
    if (new_mode == dev->mode) {
        return false;
    }

    netdev_t *netdev = (netdev_t *)dev;
    bool pdu_changed = (new_mode == AT86RF215_MODE_LEGACY_OQPSK ||
                        dev->mode == AT86RF215_MODE_LEGACY_OQPSK);

    dev->mode = new_mode;

    if (pdu_changed) {
        netdev->event_callback(netdev, NETDEV_EVENT_PDU_CHANGED);
    }

    return pdu_changed;
}

const char* at86rf215_hw_state2a(uint8_t state)
{
    switch (state) {
    case RF_STATE_TRXOFF: return "TRXOFF";
    case RF_STATE_TXPREP: return "TXPREP";
    case RF_STATE_TX: return "TX";
    case RF_STATE_RX: return "RX";
    case RF_STATE_TRANSITION: return "TRANSITION";
    case RF_STATE_RESET: return "RESET";
    default: return "invalid";
    }
}

const char* at86rf215_sw_state2a(at86rf215_state_t state) {
    switch (state) {
    case AT86RF215_STATE_OFF: return "OFF";
    case AT86RF215_STATE_IDLE: return "IDLE";
    case AT86RF215_STATE_RX_SEND_ACK: return "RX (sending ACK)";
    case AT86RF215_STATE_TX: return "TX";
    case AT86RF215_STATE_TX_WAIT_ACK: return "TX (wait for ACK)";
    case AT86RF215_STATE_SLEEP: return "SLEEP";
    default: return "invalid";
    }
}
