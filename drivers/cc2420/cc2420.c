/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Implementation of public functions for CC2420 driver
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "xtimer.h"
#include "periph/cpuid.h"
#include "byteorder.h"
#include "panic.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "cc2420_registers.h"
#include "cc2420_internal.h"
#include "cc2420_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _irq_handler(void *arg)
{
    msg_t msg;
    cc2420_t *dev = (cc2420_t *) arg;

    /* tell driver thread about the interrupt */
    msg.type = GNRC_NETDEV_MSG_TYPE_EVENT;
    msg_send(&msg, dev->mac_pid);
}

int cc2420_init(cc2420_t *dev, const cc2420_params_t *params)
{
    dev->driver = &cc2420_driver;

    /* initialize device descriptor */
    dev->spi = params->spi;
    dev->cs_pin = params->cs_pin;
    dev->fifo_int_pin = params->fifo_int_pin;
    dev->fifop_int_pin = params->fifop_int_pin;
    dev->reset_pin = params->reset_pin;
    dev->power_pin = params->power_pin;
    dev->idle_state = CC2420_STATE_IDLE;

    /* initialise SPI */
    spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, params->spi_speed);

    /* initialise GPIOs */
    gpio_init(dev->cs_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->cs_pin);
    gpio_init(dev->reset_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->reset_pin);
    gpio_init(params->power_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_clear(params->power_pin);
    gpio_init_int(dev->fifo_int_pin, GPIO_NOPULL, GPIO_RISING, _irq_handler, dev);
    gpio_init_int(dev->fifop_int_pin, GPIO_NOPULL, GPIO_RISING, _irq_handler, dev);
    gpio_set(dev->reset_pin);

    cc2420_reset(dev);
    return 0;
}

void cc2420_reset(cc2420_t *dev)
{
#if CPUID_ID_LEN
    uint8_t cpuid[CPUID_ID_LEN];
    eui64_t addr_long;
#endif

    /* Power on and toggle reset */
    gpio_set(dev->power_pin);
    gpio_clear(dev->reset_pin);
    xtimer_usleep(CC2420_WAIT_TIME);
    gpio_set(dev->reset_pin);

    /* enable transceiver's crystal oscillator */
    cc2420_strobe(dev, CC2420_STROBE_XOSCON);

    /* wait for the oscillator to be stable */
    xtimer_usleep(CC2420_WAIT_TIME);
    if ((CC2420_STATUS_BYTE() & CC2420_STATUS_XOSC16M_STABLE) == 0) {
        /* could not start up radio transceiver! */
        core_panic(0x2420, "Could not start CC2420 radio transceiver");
    }

    /* discard any potential garbage in TX buffer */
    cc2420_strobe(dev, CC2420_STROBE_FLUSHTX);

    /* reset options and sequence number */
    dev->seq_nr = 0;
    dev->options = 0;
    /* set short and long address */
#if CPUID_ID_LEN
    cpuid_get(cpuid);

#if CPUID_ID_LEN < 8
    /* in case CPUID_ID_LEN < 8, fill missing bytes with zeros */
    for (int i = CPUID_ID_LEN; i < 8; i++) {
        cpuid[i] = 0;
    }
#else
    for (int i = 8; i < CPUID_ID_LEN; i++) {
        cpuid[i & 0x07] ^= cpuid[i];
    }
#endif
    /* make sure we mark the address as non-multicast and not globally unique */
    cpuid[0] &= ~(0x01);
    cpuid[0] |= 0x02;
    /* copy and set long address */
    memcpy(&addr_long, cpuid, 8);
    cc2420_set_addr_long(dev, NTOHLL(addr_long.uint64.u64));
    cc2420_set_addr_short(dev, NTOHS(addr_long.uint16[0].u16));
#else
    cc2420_set_addr_long(dev, CC2420_DEFAULT_ADDR_LONG);
    cc2420_set_addr_short(dev, CC2420_DEFAULT_ADDR_SHORT);
#endif
    /* set default PAN id */
    cc2420_set_pan(dev, CC2420_DEFAULT_PANID);
    /* set default channel */
    cc2420_set_chan(dev, CC2420_DEFAULT_CHANNEL);
    /* set default TX power */
    cc2420_set_txpower(dev, CC2420_DEFAULT_TXPOWER);
    /* set default options */
    cc2420_set_option(dev, CC2420_OPT_AUTOACK, true);
    cc2420_set_option(dev, CC2420_OPT_CSMA, true);
    cc2420_set_option(dev, CC2420_OPT_TELL_RX_START, false);
    cc2420_set_option(dev, CC2420_OPT_TELL_RX_END, true);
    /* set default protocol */
#ifdef MODULE_NG_SIXLOWPAN
    dev->proto = GNRC_NETTYPE_SIXLOWPAN;
#else
    dev->proto = GNRC_NETTYPE_UNDEF;
#endif

    /* Change default values as recomended in the data sheet, */
    /* RX bandpass filter = 1.3uA. */
    uint16_t reg = cc2420_reg_read(dev, CC2420_REG_RXCTRL1);
    reg |= CC2420_RXBPF_LOCUR;
    cc2420_reg_write(dev, CC2420_REG_RXCTRL1, reg);

    /* Set the FIFOP threshold to maximum. */
    cc2420_reg_write(dev, CC2420_REG_IOCFG0, 127);

    /* Turn off "Security enable" (page 32). */
    reg = cc2420_reg_read(dev, CC2420_REG_SECCTRL0);
    reg &= ~CC2420_RXFIFO_PROTECTION;
    cc2420_reg_write(dev, CC2420_REG_SECCTRL0, reg);

    /* go into RX state */
    cc2420_set_state(dev, CC2420_STATE_RX_ON);
}

bool cc2420_cca(cc2420_t *dev)
{
    long count = 0;
    do {
        count++;
        if (count >= CC2420_WAIT_TIME) {
            core_panic(0x2420, "cc2420_get_cca(): RSSI never valid!");
        }
    } while (!(CC2420_STATUS_BYTE() & CC2420_STATUS_RSSI_VALID));
    return CC2420_GIO1;
}

size_t cc2420_send(cc2420_t *dev, uint8_t *data, size_t len)
{
    /* check data length */
    if (len > CC2420_MAX_PKT_LENGTH) {
        DEBUG("[cc2420] Error: data to send exceeds max packet size\n");
        return 0;
    }
    cc2420_tx_prepare(dev);
    cc2420_tx_load(dev, data, len);
    cc2420_tx_exec(dev);
    return len;
}

void cc2420_tx_prepare(cc2420_t *dev)
{
    /* make sure ongoing transmissions are finished */
    while (cc2420_get_state(dev) & CC2420_STATE_TX_BUSY);

    cc2420_strobe(dev, CC2420_STROBE_FLUSHTX);

    cc2420_set_state(dev, CC2420_STATE_TX_PREP);
    dev->frame_len = IEEE802154_FCS_LEN;
}

size_t cc2420_tx_load(cc2420_t *dev, uint8_t *data, size_t len)
{
    dev->frame_len += (uint8_t)len;
    cc2420_fb_write(dev, (uint8_t*)&len, 1);
    cc2420_fb_write(dev, data, len);
    return len;
}

void cc2420_tx_exec(cc2420_t *dev)
{
    DEBUG("%s\n", __PRETTY_FUNCTION__);
    /* put tranceiver in idle mode */
    cc2420_strobe(dev, CC2420_STROBE_RFOFF);

    /* begin transmission: wait for preamble to be sent */
    cc2420_strobe(dev, CC2420_STROBE_TXON);
}

size_t cc2420_rx_len(cc2420_t *dev)
{
    uint8_t res;
    cc2420_fb_read(dev, &res, 1);
    return (size_t)(res - 2);           /* extract the PHR and LQI field */
}

void cc2420_rx_read(cc2420_t *dev, uint8_t *data, size_t len,
                          size_t offset)
{
    cc2420_ram_read(dev, offset, data, len);
}
