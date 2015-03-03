/*
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
 * @brief       Common functions for at86rf2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "ng_at86rf2xx_registers.h"
#include "ng_at86rf2xx_spi.h"

#include "net/ng_netdev.h"
#include "net/ng_netconf.h"
#include "net/ng_netif/hdr.h"

#include "net/ng_pktbuf.h"

#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_RETRIES (100)

static uint8_t dmb_to_tx_pow[] =
    {0xF, 0xE, 0xE, 0xE, 0xE, 0xE, 0xD, 0xD, 0xD, 0xD, 0xC,
     0xC, 0xB, 0xB, 0xA, 0x9, 0x8, 0x7, 0x6, 0x5, 0x3, 0x0};
static const int tx_pow_to_dbm[] =
    {4, 3, 3, 3, 2, 2, 1, 0, -1, -2, -3, -4, -6, -8, -12, -17};

void ng_at86rf2xx_isr_event(ng_netdev_t *device, uint32_t event_type);

void ng_at86rf2xx_reset(ng_at86rf2xx_t *dev)
{
    DEBUG("RESET\n");
    gpio_clear(dev->reset_pin);
    /*put pins to default values */
    gpio_set(dev->cs_pin);
    gpio_clear(dev->sleep_pin);
    /* additional waiting to comply to min rst pulse width */
    uint8_t volatile delay = 50; /* volatile to ensure it isn't optimized away */
    while (--delay);
    gpio_set(dev->reset_pin);
}

void ng_at86rf2xx_on(ng_at86rf2xx_t *dev)
{
    /* Send a FORCE TRX OFF command */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_STATE, NG_AT86RF2XX_TRX_STATE__FORCE_TRX_OFF);

    /* busy wait for TRX_OFF state */
    do {
        int delay = _MAX_RETRIES;
        if (!--delay) {
            DEBUG("at86rf2xx : ERROR : could not enter TRX_OFF mode\n");
            return;
        }
    } while (ng_at86rf2xx_get_status(dev) != NG_AT86RF2XX_TRX_STATUS__TRX_OFF);

    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_STATE, NG_AT86RF2XX_TRX_STATE__RX_ON);

    /* change into basic reception mode to initialise CSMA seed by RNG */
    do {
        int delay = _MAX_RETRIES;
        if (!--delay) {
            DEBUG("at86rf2xx : ERROR : could not enter RX_ON mode\n");
            return;
        }
    } while (ng_at86rf2xx_get_status(dev) != NG_AT86RF2XX_TRX_STATUS__RX_ON);

    /* read RNG values into CSMA_SEED_0 */
    for (int i=0; i<7; i+=2) {
        uint8_t tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_CC_CCA);
        tmp = ((tmp&0x60)>>5);
        ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__CSMA_SEED_0, tmp << i);
    }
    /* read CSMA_SEED_1 and write back with RNG value */
    uint8_t tmp = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__CSMA_SEED_1);
    tmp |= ((ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_CC_CCA)&0x60)>>5);
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__CSMA_SEED_1, tmp);
}

void ng_at86rf2xx_switch_to_rx(ng_at86rf2xx_t *dev)
{
    gpio_irq_disable(dev->int_pin);

    /* now change to PLL_ON state for extended operating mode */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_STATE, NG_AT86RF2XX_TRX_STATE__PLL_ON);

    do {
        int max_wait = _MAX_RETRIES;
        if (!--max_wait) {
            DEBUG("at86rf2xx : ERROR : could not enter PLL_ON mode\n");
            break;
        }
    } while (ng_at86rf2xx_get_status(dev) != NG_AT86RF2XX_TRX_STATUS__PLL_ON);

    /* RX_START and TRX_END */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__IRQ_MASK,
                           ( NG_AT86RF2XX_IRQ_STATUS_MASK__RX_START | NG_AT86RF2XX_IRQ_STATUS_MASK__TRX_END));

    /* Read IRQ to clear it */
    ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__IRQ_STATUS);

    /* Enable IRQ interrupt */
    gpio_irq_enable(dev->int_pin);

    /* Enter RX_AACK_ON state */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_STATE, NG_AT86RF2XX_TRX_STATE__RX_AACK_ON);

    do {
        int max_wait = _MAX_RETRIES;
        if (!--max_wait) {
            DEBUG("at86rf2xx : ERROR : could not enter RX_AACK_ON mode\n");
            break;
        }
    } while (ng_at86rf2xx_get_status(dev) != NG_AT86RF2XX_TRX_STATUS__RX_AACK_ON);
}

void ng_at86rf2xx_initialise(ng_at86rf2xx_t *dev)
{
    /* initialise SPI */
    spi_acquire(dev->spi);
    spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, dev->spi_speed);
    spi_release(dev->spi);

    /* initialise GPIOs */
    gpio_init_int(dev->int_pin, GPIO_NOPULL, GPIO_RISING, ng_at86rf2xx_irq_handler, dev);
    gpio_init_out(dev->cs_pin, GPIO_NOPULL);
    gpio_init_out(dev->sleep_pin, GPIO_NOPULL);
    gpio_init_out(dev->reset_pin, GPIO_NOPULL);

    /* reset device */
    ng_at86rf2xx_reset(dev);

    ng_at86rf2xx_change_state(dev, NETCONF_STATE_IDLE);
}

int ng_at86rf2xx_change_state(ng_at86rf2xx_t *dev, ng_netconf_state_t state)
{
    DEBUG("Change state to ");
    switch (state) {
        case NETCONF_STATE_OFF:
            DEBUG("OFF\n");
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__TRX_STATE,
                                   NG_AT86RF2XX_TRX_STATE__TRX_OFF);
            break;

        case NETCONF_STATE_SLEEP:
            DEBUG("SLEEP\n");
            ng_at86rf2xx_change_state(dev, NETCONF_STATE_OFF);
            gpio_set(dev->sleep_pin);
            return 0;

        case NETCONF_STATE_IDLE:
            DEBUG("IDLE\n");
            if (dev->state == NETCONF_STATE_SLEEP) {
                gpio_clear(dev->sleep_pin);
            }
            ng_at86rf2xx_switch_to_rx(dev);
            break;

        case NETCONF_STATE_RX:
            DEBUG("RX\n");
            dev->state = NETCONF_STATE_RX;
            return 0;

        case NETCONF_STATE_TX:
            DEBUG("TX\n");
            if (dev->state == NETCONF_STATE_SLEEP) {
                gpio_clear(dev->sleep_pin);
            }
            if (dev->state == NETCONF_STATE_RX) {
                ng_at86rf2xx_change_state(dev, NETCONF_STATE_OFF);
            }
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__TRX_STATE,
                                   NG_AT86RF2XX_TRX_STATE__TX_ARET_ON);
            break;

        case NETCONF_STATE_RESET:
            ng_at86rf2xx_reset(dev);
            return ng_at86rf2xx_change_state(dev, NETCONF_STATE_IDLE);

        default:
            DEBUG("%02x not supported.\n");
            return -ENOTSUP;
    }

    int max_wait = _MAX_RETRIES;
    do {
        if (!--max_wait) {
            DEBUG("at86rf2xx : ERROR : could not enter %02x mode\n", des_mode);
        }
    } while (ng_at86rf2xx_get_status(dev)
             == NG_AT86RF2XX_TRX_STATUS__STATE_TRANSITION_IN_PROGRESS);

    if (max_wait) {
        dev->state = state;
        return 0;
    }

    return -ENOTSUP;
}

int ng_at86rf2xx_send(ng_netdev_t *device, ng_pktsnip_t *pkt)
{
    if (device == NULL) {
        return -ENODEV;
    }
    if (pkt == NULL) {
        return -ENOMSG;
    }

    uint8_t mhr[24];
    uint8_t index;
    uint8_t *data = pkt->data;
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t*) device;
    ng_netif_hdr_t *hdr = (ng_netif_hdr_t *) data;

    /* default to data kind TODO: provide means to specify */
    mhr[0] = 0x01;
    /* default to wants_ack TODO: provide means to specify */
    mhr[0] |= 0x20;
    if (hdr->dst_l2addr_len == 2) {
        /* default to compress pan TODO: provide means to specify dst PAN*/
        mhr[0] |= 0x40;
        /* default to 2 byte addresses for src and dst */
        mhr[1] |= 0x88;
        index = 3;
        mhr[index++] = (uint8_t)((dev->radio_pan)&0xff);
        mhr[index++] = (uint8_t)((dev->radio_pan)>>8);
        /* set destination address located directly after ng_ifhrd_t in memory */
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+2);
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+1);
        /* set source address */
        mhr[index++] = (uint8_t)((dev->radio_address)&0xff);
        mhr[index++] = (uint8_t)((dev->radio_address)>>8);
    }
    else if (hdr->dst_l2addr_len == 8) {
        /* default to use long address mode for src and dst */
        mhr[1] = 0xcc;
        /* set destination address located directly after ng_ifhrd_t in memory */
        index = 3;
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+8);
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+7);
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+6);
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+5);
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+4);
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+3);
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+2);
        mhr[index++] = (uint8_t)*(data+sizeof(ng_netif_hdr_t)+1);
        /* set source address */
        mhr[index++] = (uint8_t)((dev->radio_address_long)&0xff);
        mhr[index++] = (uint8_t)((dev->radio_address_long)>>8);
        mhr[index++] = (uint8_t)((dev->radio_address_long)>>16);
        mhr[index++] = (uint8_t)((dev->radio_address_long)>>24);
        mhr[index++] = (uint8_t)((dev->radio_address_long)>>32);
        mhr[index++] = (uint8_t)((dev->radio_address_long)>>40);
        mhr[index++] = (uint8_t)((dev->radio_address_long)>>48);
        mhr[index++] = (uint8_t)((dev->radio_address_long)>>56);
    }
    /* unsupported address length */
    else {
        return -ENOMSG;
    }
    /* set sequence number */
    mhr[2] = dev->seq_nr++;

    /* index counts sent bytes from now on */
    index += 1;

    /* Go to state PLL_ON */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_STATE, NG_AT86RF2XX_TRX_STATE__PLL_ON);

    /* wait until it is on PLL_ON state */
    do {
        int max_wait = 100;
        if (!--max_wait) {
            DEBUG("at86rf2xx : ERROR : could not enter PLL_ON mode\n");
            break;
        }
    } while ((ng_at86rf2xx_get_status(dev) & NG_AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS)
             != NG_AT86RF2XX_TRX_STATUS__PLL_ON);

    /* change into TX_ARET_ON state */
    ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__TRX_STATE, NG_AT86RF2XX_TRX_STATE__TX_ARET_ON);

    do {
        int max_wait = 100;
        if (!--max_wait) {
            DEBUG("at86rf2xx : ERROR : could not enter TX_ARET_ON mode\n");
            break;
        }
    } while (ng_at86rf2xx_get_status(dev) != NG_AT86RF2XX_TRX_STATUS__TX_ARET_ON);
    /* write MAC header to FIFO */
    ng_at86rf2xx_addr_write_fifo(dev, 0x00, data, index);

    while (pkt) {
        /* check we don't exceed FIFO size */
        if (index+pkt->size > NG_AT86RF2XX_MAX_PKT_LENGTH) {
            ng_pktbuf_release(pkt);
            DEBUG("Packet exceeded FIFO size.\n");
            return -ENOBUFS;
        }
        /* write snip data into FIFO */
        ng_at86rf2xx_addr_write_fifo(dev, index, pkt->data, pkt->size);
        /* count bytes */
        index += pkt->size;
        /* next snip */
        pkt = pkt->next;
    }

    DEBUG("Data loaded into FIFO.\n");
    if ((dev->options&(1<<NETCONF_OPT_PRELOADING)) == NETCONF_DISABLE) {
        DEBUG("Sending now.\n");
        ng_at86rf2xx_change_state(dev, NETCONF_STATE_TX);
    }

    return index;
}

int ng_at86rf2xx_get(ng_netdev_t *device, ng_netconf_opt_t opt,
                     void *value, size_t value_len)
{
    if (device == NULL) {
        return -ENODEV;
    }
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *) device;

    switch (opt) {
        case NETCONF_OPT_CHANNEL:
            if (value_len < sizeof(uint8_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(uint8_t);
            *((uint8_t*)value) = dev->radio_channel;
            break;

        case NETCONF_OPT_ADDRESS:
            if (value_len < sizeof(uint16_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(uint16_t);
            *((uint16_t*)value) = dev->radio_address;

            printf("addr %02x\n", *((uint16_t*)value));
            printf("addr %02x\n", dev->radio_address);
            break;

        case NETCONF_OPT_ADDRESS_LONG:
            if (value_len < sizeof(uint64_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(uint64_t);
            *((uint64_t*)value) = dev->radio_address_long;
            break;

        case NETCONF_OPT_NID:
            if (value_len < sizeof(uint16_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(uint16_t);
            *((uint16_t*)value) = dev->radio_pan;
            break;

        case NETCONF_OPT_AUTOACK:
            if (value_len < sizeof(ng_netconf_enable_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(ng_netconf_enable_t);
            /* driver is currently driven in operating modes which have to AUTOACK */
            *((ng_netconf_enable_t*)value) = NETCONF_ENABLE;
            break;

        case NETCONF_OPT_STATE:
            if (value_len < sizeof(ng_netconf_state_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(ng_netconf_state_t);
            *((ng_netconf_state_t*)value) = dev->state;
            break;

        case NETCONF_OPT_PRELOADING:
            if (value_len < sizeof(ng_netconf_enable_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(ng_netconf_enable_t);
            *((ng_netconf_enable_t*)value) = (dev->options>>NETCONF_OPT_PRELOADING);
            break;

        case NETCONF_OPT_PROMISCUOUSMODE:
            if (value_len < sizeof(ng_netconf_enable_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(ng_netconf_enable_t);
            *((ng_netconf_enable_t*)value) = (dev->options>>NETCONF_OPT_PROMISCUOUSMODE);
            break;

        case NETCONF_OPT_IS_CHANNEL_CLR:
            if (value_len < sizeof(ng_netconf_enable_t))
            {
                return -EOVERFLOW;
            }
            value_len = sizeof(ng_netconf_enable_t);
            /* trigger CCA measurment */
            uint8_t reg = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_CC_CCA);
            reg &= NG_AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
            ng_at86rf2xx_reg_write(dev, NG_AT86RF2XX_REG__PHY_CC_CCA, reg);
            /* busy wait for result */
            uint8_t status;
            do {
                status = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__TRX_STATUS);
            } while (!(status & NG_AT86RF2XX_TRX_STATUS_MASK__CCA_DONE));
            /* return according to measurement */
            if (status & NG_AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS) {
                *((ng_netconf_enable_t*)value) = NETCONF_ENABLE;
            }
            else {
                *((ng_netconf_enable_t*)value) = NETCONF_DISABLE;
            }
            break;

        case NETCONF_OPT_TX_POWER:
            if (value_len < sizeof(int8_t))
            {
                return -EOVERFLOW;
            }
            uint8_t pow = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_TX_PWR)
                        & NG_AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR;
            *((int8_t*)value) = tx_pow_to_dbm[pow];
            break;

        case NETCONF_OPT_ADDR_LEN:
        case NETCONF_OPT_MAX_PACKET_SIZE:
        default:
            return -ENOTSUP;
    }

    return 0;
}

int ng_at86rf2xx_set(ng_netdev_t *device, ng_netconf_opt_t opt,
                     void *value, size_t value_len)
{
    if (device == NULL) {
        return -ENODEV;
    }
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *) device;

    switch (opt) {
        case NETCONF_OPT_CHANNEL:
            if (value_len > sizeof(uint16_t))
            {
                return -EOVERFLOW;
            }
            if (*((uint8_t*)value) < NG_AT86RF2XX_MIN_CHANNEL ||
                *((uint8_t*)value) > NG_AT86RF2XX_MAX_CHANNEL) {
                return -ENOTSUP;
            }
            dev->radio_channel = *((uint8_t*)value);
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__PHY_CC_CCA,
                                   dev->radio_channel & NG_AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
            break;

        case NETCONF_OPT_ADDRESS:
            if (value_len > sizeof(uint16_t))
            {
                return -EOVERFLOW;
            }
            dev->radio_address = *((uint16_t*)value);
            printf("addr %02x\n", *((uint16_t*)value));
            printf("addr %02x\n", dev->radio_address);
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__SHORT_ADDR_0,
                                   dev->radio_address);
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__SHORT_ADDR_1,
                                   (dev->radio_address >> 8));
            break;

        case NETCONF_OPT_ADDRESS_LONG:
            if (value_len > sizeof(uint64_t))
            {
                return -EOVERFLOW;
            }
            dev->radio_address_long = *((uint64_t*)value);
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__IEEE_ADDR_0,
                                   dev->radio_address_long);
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__IEEE_ADDR_1,
                                   (dev->radio_address_long >> 8));
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__IEEE_ADDR_2,
                                   (dev->radio_address_long >> 16));
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__IEEE_ADDR_3,
                                   (dev->radio_address_long >> 24));
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__IEEE_ADDR_4,
                                   (dev->radio_address_long >> 32));
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__IEEE_ADDR_5,
                                   (dev->radio_address_long >> 40));
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__IEEE_ADDR_6,
                                   (dev->radio_address_long >> 48));
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__IEEE_ADDR_7,
                                   (dev->radio_address_long >> 56));
            break;

        case NETCONF_OPT_NID:
            if (value_len > sizeof(uint16_t))
            {
                return -EOVERFLOW;
            }
            dev->radio_pan = *((uint16_t*)value);
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__PAN_ID_0,
                                   dev->radio_pan);
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__PAN_ID_1,
                                   (dev->radio_pan >> 8));
            break;

        case NETCONF_OPT_AUTOACK:
            return -ENOTSUP;

        case NETCONF_OPT_STATE:
            if (value_len > sizeof(ng_netconf_state_t))
            {
                return -EOVERFLOW;
            }
            return ng_at86rf2xx_change_state(dev, *((ng_netconf_state_t*)value));

        case NETCONF_OPT_PRELOADING:
            if (value_len > sizeof(ng_netconf_enable_t))
            {
                return -EOVERFLOW;
            }
            dev->options |= (1<<NETCONF_OPT_PRELOADING);
            break;

        case NETCONF_OPT_PROMISCUOUSMODE:
            if (value_len > sizeof(ng_netconf_enable_t))
            {
                return -EOVERFLOW;
            }
            dev->options |= (1<<NETCONF_OPT_PROMISCUOUSMODE);
            break;

        case NETCONF_OPT_TX_POWER:
            if (value_len > sizeof(int8_t))
            {
                return -EOVERFLOW;
            }
            int8_t dbm = *((int8_t*)value);
            if (dbm < NG_AT86RF2XX_TX_POWER_MIN) {
                dbm = NG_AT86RF2XX_TX_POWER_MIN;
            }
            if (dbm > NG_AT86RF2XX_TX_POWER_MAX) {
                dbm = NG_AT86RF2XX_TX_POWER_MAX;
            }
            uint8_t pow = dmb_to_tx_pow[dbm - NG_AT86RF2XX_TX_POWER_MIN];
            ng_at86rf2xx_reg_write(dev,
                                   NG_AT86RF2XX_REG__PHY_TX_PWR,
                                   pow & NG_AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR);
            break;

        case NETCONF_OPT_IS_CHANNEL_CLR:
        case NETCONF_OPT_ADDR_LEN:
        case NETCONF_OPT_MAX_PACKET_SIZE:
        default:
            return -ENOTSUP;
    }

    return 0;
}

int ng_at86rf2xx_add_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb) {
        return -ENOBUFS;
    }

    dev->event_cb = cb;
    return 0;
}

int ng_at86rf2xx_rem_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb != cb) {
        return -ENOENT;
    }

    dev->event_cb = NULL;
    return 0;
}

void ng_at86rf2xx_irq_handler(void *arg)
{
    uint8_t event;
    msg_t msg;
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *) arg;
    /* consume IRQ event */
    event = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__IRQ_STATUS);

    DEBUG("IRQ of type %02x\n", event);
    switch (event) {
        case NG_AT86RF2XX_IRQ_STATUS_MASK__RX_START:
            msg.type = NG_NETDEV_MSG_TYPE_EVENT;
            msg.content.value = NETDEV_EVENT_RX_STARTED;
            break;
        case NG_AT86RF2XX_IRQ_STATUS_MASK__TRX_END:
            msg.type = NG_NETDEV_MSG_TYPE_EVENT;
            if (dev->state == NETCONF_STATE_TX) {
                msg.content.value = NETDEV_EVENT_TX_COMPLETE;
            } else {
                msg.content.value = NETDEV_EVENT_RX_COMPLETE;
            }
            break;
        default:
            /* no other IRQs configured */
            return;
    }
    msg_try_send(&msg, dev->mac_pid);
}

void ng_at86rf2xx_isr_event(ng_netdev_t *device, uint32_t event_type)
{
    ng_at86rf2xx_t *dev = (ng_at86rf2xx_t *) device;

    if (event_type == NETDEV_EVENT_RX_STARTED) {
        ng_at86rf2xx_change_state(dev, NETCONF_STATE_RX);
    }
    else if (event_type == NETDEV_EVENT_RX_COMPLETE) {
        uint8_t data[24];
        ng_netif_hdr_t *hdr;
        /* read MAC header fields */
        ng_at86rf2xx_addr_read_fifo(dev, 0x0, data, 0x24);
        /* determine src addr length */
        uint8_t addr_length = 0;
        /* src 64 bit addr */
        if ((data[1] & 0x0c) == 0x0c) {
            addr_length = 0x08;
        }
        /* src 16bit addr */
        else if ((data[1] & 0xc0) == 0x08) {
            addr_length = 0x02;
        }
        else {
            DEBUG("Bogus address length.");
        }
        /* allocate a pktsnip for generic header */
        ng_pktsnip_t *hdr_snip = ng_pktbuf_add(NULL, NULL, sizeof(ng_netif_hdr_t)+addr_length, NG_NETTYPE_UNDEF);

        if (hdr_snip == NULL) {
            DEBUG("Packet buffer full.\n");
            ng_pktbuf_release(hdr_snip);
            return;
        }
        hdr = (ng_netif_hdr_t*)hdr_snip->data;
        /* init generic header */
        ng_netif_hdr_init(hdr, addr_length, 0);
        /* append src address into memory */
        if (addr_length == 2) {
            ng_netif_hdr_set_src_addr(hdr, &(data[7]), addr_length);
        }
        else {
            ng_netif_hdr_set_src_addr(hdr, &(data[11]), addr_length);
        }
        hdr->if_pid = thread_getpid();
        /* in extended operating mode ED should be used instead of rssi */
        hdr->rssi = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__PHY_ED_LEVEL);
        /* lqi value is appended to payload and will be set later */

        /* read payload now */
        /* determine length */
        ng_at86rf2xx_bulk_read_fifo(dev, data, 1);
        /* allocate a pktsnip accordingly and link header snip */
        ng_pktsnip_t *payload_snip = ng_pktbuf_add(hdr_snip, NULL, data[0], NG_NETTYPE_UNDEF);
        /* copy data to pkt buffer */
        if (addr_length == 2) {
            ng_at86rf2xx_addr_read_fifo(dev, 0x09, payload_snip->data, data[0]);
        }
        else {
            ng_at86rf2xx_addr_read_fifo(dev, 0x13, payload_snip->data, data[0]);
        }

        /* read lqi from FIFO */
        ng_at86rf2xx_addr_read_fifo(dev, data[0]-1, &(hdr->lqi), 0x1);

        if (dev->event_cb) {
            DEBUG("Handed on packet.");
            dev->event_cb(NETDEV_EVENT_RX_COMPLETE, payload_snip);
        }
        else {
            DEBUG("Dropping packet, no one interested.");
            ng_pktbuf_release(payload_snip);
        }
    }
    else if (event_type == NETDEV_EVENT_TX_COMPLETE) {
        /* use option AUTOACK also to wait for ACK */
        if ((dev->options&(1<<NETCONF_OPT_AUTOACK)) == NETCONF_ENABLE) {
            uint8_t trac_status;
            do {
                trac_status = ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__TRX_STATE);
                trac_status &= NG_AT86RF2XX_TRX_STATE_MASK__TRAC;
            }
            while (trac_status == NG_AT86RF2XX_TRX_STATE__TRAC_INVALID);
        }
    }
}

const ng_netdev_driver_t ng_at86rf2xx_driver = {
    .send_data = ng_at86rf2xx_send,
    .add_event_callback = ng_at86rf2xx_add_event_callback,
    .rem_event_callback = ng_at86rf2xx_rem_event_callback,
    .get = ng_at86rf2xx_get,
    .set = ng_at86rf2xx_set,
    .isr_event = ng_at86rf2xx_isr_event,
};
