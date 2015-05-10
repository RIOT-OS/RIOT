/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ng_at86rf2xx
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
 *
 * @}
 */

#include "hwtimer.h"
#include "periph/cpuid.h"
#include "byteorder.h"
#include "net/ng_ieee802154.h"
#include "net/ng_netbase.h"
#include "ng_at86rf2xx_registers.h"
#include "ng_at86rf2xx_internal.h"
#include "ng_at86rf2xx_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


#define RESET_DELAY             (0U)        /* must be > 625ns */


static void _irq_handler(void *arg)
{
    msg_t msg;
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *) arg;

    /* tell driver thread about the interrupt */
    msg.type = NG_NETDEV_MSG_TYPE_EVENT;
    msg_send(&msg, dev->mac_pid);
}

int ng_at86rf2xx_init(ng_at86rf2xx_t *dev, spi_t spi, spi_speed_t spi_speed,
                      gpio_t cs_pin, gpio_t int_pin,
                      gpio_t sleep_pin, gpio_t reset_pin)
{
    dev->driver = &ng_at86rf2xx_driver;

    /* initialize device descriptor */
    dev->spi = spi;
    dev->cs_pin = cs_pin;
    dev->int_pin = int_pin;
    dev->sleep_pin = sleep_pin;
    dev->reset_pin = reset_pin;

    /* initialise SPI */
    spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, spi_speed);
    /* initialise GPIOs */
    gpio_init_out(dev->cs_pin, GPIO_NOPULL);
    gpio_set(dev->cs_pin);
    gpio_init_out(dev->sleep_pin, GPIO_NOPULL);
    gpio_clear(dev->sleep_pin);
    gpio_init_out(dev->reset_pin, GPIO_NOPULL);
    gpio_set(dev->reset_pin);
    gpio_init_int(dev->int_pin, GPIO_NOPULL, GPIO_RISING, _irq_handler, dev);

    /* test if the SPI is set up correctly and the device is responding */
    if (ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PART_NUM) !=
        NG_AT86RF2XX_PARTNUM) {
        DEBUG("[ng_at86rf2xx] error: unable to read correct part number\n");
        return -1;
    }

    /* reset device to default values and put it into RX state */
    ng_at86rf2xx_reset(dev);
    return 0;
}

void ng_at86rf2xx_reset(ng_at86rf2xx_t *dev)
{
#if CPUID_ID_LEN
    uint8_t cpuid[CPUID_ID_LEN];
    eui64_t addr_long;
#endif

    /* trigger hardware reset */
    gpio_clear(dev->reset_pin);
    hwtimer_wait(HWTIMER_TICKS(RESET_DELAY));
    gpio_set(dev->reset_pin);
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
    ng_at86rf2xx_set_addr_long(dev, NTOHLL(addr_long.uint64.u64));
    ng_at86rf2xx_set_addr_short(dev, NTOHS(addr_long.uint16[0].u16));
#else
    ng_at86rf2xx_set_addr_long(dev, NG_AT86RF2XX_DEFAULT_ADDR_LONG);
    ng_at86rf2xx_set_addr_short(dev, NG_AT86RF2XX_DEFAULT_ADDR_SHORT);
#endif
    /* set default PAN id */
    ng_at86rf2xx_set_pan(dev, NG_AT86RF2XX_DEFAULT_PANID);
    /* set default channel */
    ng_at86rf2xx_set_chan(dev, NG_AT86RF2XX_DEFAULT_CHANNEL);
    /* set default TX power */
    ng_at86rf2xx_set_txpower(dev, NG_AT86RF2XX_DEFAULT_TXPOWER);
    /* set default options */
    ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_AUTOACK, true);
    ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_CSMA, true);
    ng_at86rf2xx_set_option(dev, NG_AT86RF2XX_OPT_TELL_RX_END, true);
    /* set default protocol */
#ifdef MODULE_NG_SIXLOWPAN
    dev->proto = NG_NETTYPE_SIXLOWPAN;
#else
    dev->proto = NG_NETTYPE_UNDEF;
#endif
    /* enable safe mode (protect RX FIFO until reading data starts) */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_CTRL_2,
                          NG_AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);
#ifdef MODULE_NG_AT86RF212
    ng_at86rf2xx_set_freq(NG_AT86RF2XX_FREQ_915MHZ);
#endif
    /* enable interrupts */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__IRQ_MASK,
                          NG_AT86RF2XX_IRQ_STATUS_MASK__TRX_END);
    /* go into RX state */
    ng_at86rf2xx_set_state(dev, NG_AT86RF2XX_STATE_RX_AACK_ON);

    DEBUG("ng_at86rf2xx_reset(): reset complete.\n");
}

bool ng_at86rf2xx_cca(ng_at86rf2xx_t *dev)
{
    uint8_t tmp;
    uint8_t status;

    /* trigger CCA measurment */
    tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_CC_CCA);
    tmp &= NG_AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__PHY_CC_CCA, tmp);
    /* wait for result to be ready */
    do {
        status = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__TRX_STATUS);
    } while (!(status & NG_AT86RF2XX_TRX_STATUS_MASK__CCA_DONE));
    /* return according to measurement */
    if (status & NG_AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS) {
        return true;
    }
    else {
        return false;
    }
}

size_t ng_at86rf2xx_send(ng_at86rf2xx_t *dev, uint8_t *data, size_t len)
{
    /* check data length */
    if (len > NG_AT86RF2XX_MAX_PKT_LENGTH) {
        DEBUG("[ng_at86rf2xx] Error: data to send exceeds max packet size\n");
        return 0;
    }
    ng_at86rf2xx_tx_prepare(dev);
    ng_at86rf2xx_tx_load(dev, data, len, 0);
    ng_at86rf2xx_tx_exec(dev);
    return len;
}

void ng_at86rf2xx_tx_prepare(ng_at86rf2xx_t *dev)
{
    uint8_t state;

    /* make sure ongoing transmissions are finished */
    do {
        state = ng_at86rf2xx_get_state(dev);
    }
    while (state == NG_AT86RF2XX_STATE_BUSY_RX_AACK);
    dev->idle_state = state;
    ng_at86rf2xx_set_state(dev, NG_AT86RF2XX_STATE_TX_ARET_ON);
    dev->frame_len = NG_IEEE802154_FCS_LEN;
}

size_t ng_at86rf2xx_tx_load(ng_at86rf2xx_t *dev, uint8_t *data,
                            size_t len, size_t offset)
{
    dev->frame_len += (uint8_t)len;
    ng_at86rf2xx_sram_write(dev, offset + 1, data, len);
    return offset + len;
}

void ng_at86rf2xx_tx_exec(ng_at86rf2xx_t *dev)
{
    /* write frame length field in FIFO */
    ng_at86rf2xx_sram_write(dev, 0, &(dev->frame_len), 1);
    /* trigger sending of pre-loaded frame */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_STATE,
                           NG_AT86RF2XX_TRX_STATE__TX_START);
    if (dev->event_cb && (dev->options & NG_AT86RF2XX_OPT_TELL_TX_START)) {
        dev->event_cb(NETDEV_EVENT_TX_STARTED, NULL);
    }
}

size_t ng_at86rf2xx_rx_len(ng_at86rf2xx_t *dev)
{
    uint8_t res;
    ng_at86rf2xx_fb_read(dev, &res, 1);
    return (size_t)(res - 2);           /* extract the PHR and LQI field */
}

void ng_at86rf2xx_rx_read(ng_at86rf2xx_t *dev, uint8_t *data, size_t len,
                          size_t offset)
{
    /* when reading from SRAM, the different chips from the AT86RF2xx family
     * behave differently: the AT86F233, the AT86RF232 and the ATRF86212B return
     * frame length field (PHR) at position 0 and the first data byte at
     * position 1.
     * The AT86RF231 does not return the PHR field and return
     * the first data byte at position 0.
     */
#ifndef MODULE_NG_AT86RF231
    ng_at86rf2xx_sram_read(dev, offset + 1, data, len);
#else
    ng_at86rf2xx_sram_read(dev, offset, data, len);
#endif
}
