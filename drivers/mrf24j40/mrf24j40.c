/*
 * Copyright (C) 2016 Hochschule für Angewandte Wissenschaften Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Implementation of public functions for MRF24J40 drivers
 *
 * @author      Bernhard Nägele <bernhard@naegele-privat.de>
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 * @}
 */

#include "periph/cpuid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "mrf24j40.h"
#include "mrf24j40_registers.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void mrf24j40_setup(mrf24j40_t *dev, const mrf24j40_params_t *params)
{
    netdev2_t *netdev = (netdev2_t *)dev;

    netdev->driver = &mrf24j40_driver;
    /* initialize device descriptor */
    memcpy(&dev->params, params, sizeof(at86rf2xx_params_t));
    dev->idle_state = MRF24J40_RFSTATE_RX;
    dev->state = MRF24J40_RFSTATE_SLEEP;
    /* initialise SPI */
    spi_init_master(dev->params.spi, SPI_CONF_FIRST_RISING, params->spi_speed);
}

int mrf24j40_init(mrf24j40_t *dev, spi_t spi, spi_speed_t spi_speed,
                   gpio_t cs_pin, gpio_t int_pin,
                   gpio_t sleep_pin, gpio_t reset_pin)
{
    dev->driver = &mrf24j40_driver;

    /* initialize device descriptor */
    dev->spi = spi;
    dev->cs_pin = cs_pin;
    dev->int_pin = int_pin;
    dev->sleep_pin = sleep_pin;
    dev->reset_pin = reset_pin;
    //dev->idle_state = ;

    /* initialise SPI */
    spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, spi_speed);
    /* initialise GPIOs */
    gpio_init(dev->cs_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->cs_pin);
    gpio_init(dev->wake_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_clear(dev->wake_pin);
    gpio_init(dev->reset_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->reset_pin);
    gpio_init_int(dev->int_pin, GPIO_NOPULL, GPIO_RISING, _irq_handler, dev);

    /* make sure device is not sleeping, so we can query part number */
    mrf24j40_assert_awake(dev);

    /* test if the SPI is set up correctly and the device is responding */
    if (mrf24j40_reg_read(dev, MRF24J40_REG__PART_NUM) !=
        MRF24J40_PARTNUM) {
        DEBUG("[mrf24j40] error: unable to read correct part number\n");
        return -1;
    }

    /* reset device to default values and put it into RX state */
    mrf24j40_reset(dev);

    return 0;
}

void mrf24j40_reset(mrf24j40_t *dev)
{
#if CPUID_LEN
    uint8_t cpuid[CPUID_LEN];
    eui64_t addr_long;
#endif

    mrf24j40_hardware_reset(dev);

    /* Here starts init-process as described on MRF24J40 Manual Chap. 3.2 */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_PACON2, 0x98);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXSTBL, 0x95);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON0, 0x03);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON1, 0x01);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON2, 0x80);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON6, 0x90);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON7, 0x80);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON8, 0x10);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_SLPCON1, 0x21);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG2, 0x80);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_CCAEDTH, 0x60);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG6, 0x40);


    /* clear interrupt flags */
    mrf24j40_clear_irq_flags(dev);
    /* enable interrupts */
    mrf24j40_set_interrupts(dev);

    /* set default PAN id */
    mrf24j40_set_pan(dev, MRF24J40_DEFAULT_PANID);
    /* set default channel */
    mrf24j40_set_chan(dev, MRF24J40_DEFAULT_CHANNEL);

    /* set default TX power */
    mrf24j40_set_txpower(dev, MRF24J40_DEFAULT_TXPOWER);

    mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x04);	//  – Reset RF state machine.
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x00);	// part 2

	xtimer_usleep(20);				/* Delay at least 192us */


    /* Reset state machine to ensure a known state */
    mrf24j40_reset_state_machine(dev);

    /* reset options and sequence number */
    dev->seq_nr = 0;
    dev->options = 0;
    /* set short and long address */
#if CPUID_LEN
    cpuid_get(cpuid);

#if CPUID_LEN < 8
    /* in case CPUID_LEN < 8, fill missing bytes with zeros */
    for (int i = CPUID_LEN; i < 8; i++) {
        cpuid[i] = 0;
    }
#else
    for (int i = 8; i < CPUID_LEN; i++) {
        cpuid[i & 0x07] ^= cpuid[i];
    }
#endif
    /* make sure we mark the address as non-multicast and not globally unique */
    cpuid[0] &= ~(0x01);
    cpuid[0] |= 0x02;
    /* copy and set long address */
    memcpy(&addr_long, cpuid, 8);
    mrf24j40_set_addr_long(dev, NTOHLL(addr_long.uint64.u64));
    mrf24j40_set_addr_short(dev, NTOHS(addr_long.uint16[0].u16));
#else
    mrf24j40_set_addr_long(dev, MRF24J40_DEFAULT_ADDR_LONG);
    mrf24j40_set_addr_short(dev, MRF24J40_DEFAULT_ADDR_SHORT);
#endif


    /* set default options */								
    mrf24j40_set_option(dev, MRF24J40_OPT_AUTOACK, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_CSMA, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_TELL_RX_START, false);
    mrf24j40_set_option(dev, MRF24J40_OPT_TELL_RX_END, true);
    /* set default protocol */
#ifdef MODULE_GNRC_SIXLOWPAN
    dev->proto = GNRC_NETTYPE_SIXLOWPAN;
#else
    dev->proto = GNRC_NETTYPE_UNDEF;
#endif
    /* enable safe mode (protect RX FIFO until reading data starts) */
    mrf24j40_reg_write(dev, MRF24J40_REG__TRX_CTRL_2, MRF24J40_TRX_CTRL_2_MASK__RX_SAFE_MODE);
    /* don't populate masked interrupt flags to IRQ_STATUS register */     /* Bernhard - read modify write */
    uint8_t tmp = mrf24j40_reg_read(dev, MRF24J40_REG__TRX_CTRL_1);
    tmp &= ~(MRF24J40_TRX_CTRL_1_MASK__IRQ_MASK_MODE);
    mrf24j40_reg_write(dev, MRF24J40_REG__TRX_CTRL_1, tmp);

    /* disable clock output to save power */
    tmp = mrf24j40_reg_read(dev, MRF24J40_REG__TRX_CTRL_0);
    tmp &= ~(MRF24J40_TRX_CTRL_0_MASK__CLKM_CTRL);
    tmp &= ~(MRF24J40_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
    tmp |= (MRF24J40_TRX_CTRL_0_CLKM_CTRL__OFF);
    mrf24j40_reg_write(dev, MRF24J40_REG__TRX_CTRL_0, tmp);



    /* go into RX state */
    mrf24j40_set_state(dev, MRF24J40_RFSTATE_RX);

    DEBUG("mrf24j40_reset(): reset complete.\n");
}

void mrf24j40_tx_prepare(mrf24j40_t *dev)
{
    
    mrf24j40_set_state(dev, MRF24J40_RFSTATE_TX);
    dev->tx_frame_len = IEEE802154_FCS_LEN;
}

size_t mrf24j40_tx_load(mrf24j40_t *dev, uint8_t *data,
                         size_t len)
{
    dev->tx_frame_len += (uint8_t)len;
    mrf24j40_tx_write(dev, data, len);
    return len;
}

void mrf24j40_tx_exec(mrf24j40_t *dev)
{
    netdev2_t *netdev = (netdev2_t *)dev;

    /* write frame length field in FIFO */
    mrf24j40_tx_write(dev, 0, &(dev->tx_frame_len), 1);
    /* trigger sending of pre-loaded frame */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXNCON,1);
    if (netdev->event_callback &&
        (dev->netdev.flags & MRF24J40_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV2_EVENT_TX_STARTED, NULL);
    }
}

size_t mrf24j40_send(mrf24j40_t *dev, uint8_t *data, size_t len)
{
    /* check data length */
    if (len > MRF24J40_MAX_PKT_LENGTH) {
        return 0;
    }
    mrf24j40_tx_prepare(dev);
    mrf24j40_tx_load(dev, data, len, 0);
    mrf24j40_tx_exec(dev);
    return len;
}