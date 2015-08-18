/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
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
 *
 * @}
 */

#include "hwtimer.h"
#include "periph/cpuid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


#define RESET_DELAY             (1U)        /* must be > 625ns */


static void _irq_handler(void *arg)
{
    msg_t msg;
    at86rf2xx_t *dev = (at86rf2xx_t *) arg;

    /* tell driver thread about the interrupt */
    msg.type = NG_NETDEV_MSG_TYPE_EVENT;
    msg_send(&msg, dev->mac_pid);
}

int at86rf2xx_init(at86rf2xx_t *dev, spi_t spi, spi_speed_t spi_speed,
                   gpio_t cs_pin, gpio_t int_pin,
                   gpio_t sleep_pin, gpio_t reset_pin)
{
    dev->driver = &at86rf2xx_driver;

    /* initialize device descriptor */
    dev->spi = spi;
    dev->cs_pin = cs_pin;
    dev->int_pin = int_pin;
    dev->sleep_pin = sleep_pin;
    dev->reset_pin = reset_pin;
    dev->idle_state = AT86RF2XX_STATE_TRX_OFF;

    /* initialise SPI */
    spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, spi_speed);
    /* initialise GPIOs */
    gpio_init(dev->cs_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->cs_pin);
    gpio_init(dev->sleep_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_clear(dev->sleep_pin);
    gpio_init(dev->reset_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->reset_pin);
    gpio_init_int(dev->int_pin, GPIO_NOPULL, GPIO_RISING, _irq_handler, dev);

    /* test if the SPI is set up correctly and the device is responding */
    if (at86rf2xx_reg_read(dev, AT86RF2XX_REG__PART_NUM) !=
        AT86RF2XX_PARTNUM) {
        DEBUG("[at86rf2xx] error: unable to read correct part number\n");
        return -1;
    }

    /* reset device to default values and put it into RX state */
    at86rf2xx_reset(dev);
    return 0;
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
#if CPUID_ID_LEN
    uint8_t cpuid[CPUID_ID_LEN];
    eui64_t addr_long;
#endif

    /* wake from sleep in case radio is sleeping */
    gpio_clear(dev->sleep_pin);
    /* trigger hardware reset */
    gpio_clear(dev->reset_pin);
    hwtimer_wait(HWTIMER_TICKS(RESET_DELAY));
    gpio_set(dev->reset_pin);

    /* Reset state machine to ensure a known state */
    at86rf2xx_reset_state_machine(dev);

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
    at86rf2xx_set_addr_long(dev, NTOHLL(addr_long.uint64.u64));
    at86rf2xx_set_addr_short(dev, NTOHS(addr_long.uint16[0].u16));
#else
    at86rf2xx_set_addr_long(dev, AT86RF2XX_DEFAULT_ADDR_LONG);
    at86rf2xx_set_addr_short(dev, AT86RF2XX_DEFAULT_ADDR_SHORT);
#endif
    /* set default PAN id */
    at86rf2xx_set_pan(dev, AT86RF2XX_DEFAULT_PANID);
    /* set default channel */
    at86rf2xx_set_chan(dev, AT86RF2XX_DEFAULT_CHANNEL);
    /* set default TX power */
    at86rf2xx_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);
    /* set default options */
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_START, false);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_END, true);
    /* set default protocol */
#ifdef MODULE_NG_SIXLOWPAN
    dev->proto = NG_NETTYPE_SIXLOWPAN;
#else
    dev->proto = NG_NETTYPE_UNDEF;
#endif
    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2,
                        AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);
#ifdef MODULE_AT86RF212B
    at86rf2xx_set_freq(dev, AT86RF2XX_FREQ_915MHZ);
#endif

    /* don't populate masked interrupt flags to IRQ_STATUS register */
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_1);
    tmp &= ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_1, tmp);

    /* enable interrupts */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END);
    /* clear interrupt flags */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    /* go into RX state */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);

    DEBUG("at86rf2xx_reset(): reset complete.\n");
}

bool at86rf2xx_cca(at86rf2xx_t *dev)
{
    uint8_t tmp;
    uint8_t status;

    /* trigger CCA measurment */
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    tmp &= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, tmp);
    /* wait for result to be ready */
    do {
        status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS);
    } while (!(status & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE));
    /* return according to measurement */
    if (status & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS) {
        return true;
    }
    else {
        return false;
    }
}

size_t at86rf2xx_send(at86rf2xx_t *dev, uint8_t *data, size_t len)
{
    /* check data length */
    if (len > AT86RF2XX_MAX_PKT_LENGTH) {
        DEBUG("[at86rf2xx] Error: data to send exceeds max packet size\n");
        return 0;
    }
    at86rf2xx_tx_prepare(dev);
    at86rf2xx_tx_load(dev, data, len, 0);
    at86rf2xx_tx_exec(dev);
    return len;
}

void at86rf2xx_tx_prepare(at86rf2xx_t *dev)
{
    uint8_t state;

    /* make sure ongoing transmissions are finished */
    do {
        state = at86rf2xx_get_status(dev);
    }
    while (state == AT86RF2XX_STATE_BUSY_RX_AACK ||
           state == AT86RF2XX_STATE_BUSY_TX_ARET);
    if (state != AT86RF2XX_STATE_TX_ARET_ON) {
        dev->idle_state = state;
    }
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_TX_ARET_ON);
    dev->frame_len = IEEE802154_FCS_LEN;
}

size_t at86rf2xx_tx_load(at86rf2xx_t *dev, uint8_t *data,
                         size_t len, size_t offset)
{
    dev->frame_len += (uint8_t)len;
    at86rf2xx_sram_write(dev, offset + 1, data, len);
    return offset + len;
}

void at86rf2xx_tx_exec(at86rf2xx_t *dev)
{
    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &(dev->frame_len), 1);
    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);
    if (dev->event_cb && (dev->options & AT86RF2XX_OPT_TELL_TX_START)) {
        dev->event_cb(NETDEV_EVENT_TX_STARTED, NULL);
    }
}

size_t at86rf2xx_rx_len(at86rf2xx_t *dev)
{
    uint8_t res;
    at86rf2xx_fb_read(dev, &res, 1);
    return (size_t)(res - 2);           /* extract the PHR and LQI field */
}

void at86rf2xx_rx_read(at86rf2xx_t *dev, uint8_t *data, size_t len,
                       size_t offset)
{
    /* when reading from SRAM, the different chips from the AT86RF2xx family
     * behave differently: the AT86F233, the AT86RF232 and the ATRF86212B return
     * frame length field (PHR) at position 0 and the first data byte at
     * position 1.
     * The AT86RF231 does not return the PHR field and return
     * the first data byte at position 0.
     */
#ifndef MODULE_AT86RF231
    at86rf2xx_sram_read(dev, offset + 1, data, len);
#else
    at86rf2xx_sram_read(dev, offset, data, len);
#endif
}
