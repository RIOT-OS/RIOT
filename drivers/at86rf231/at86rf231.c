/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf231
 * @{
 *
 * @file
 * @brief       Driver implementation of the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "at86rf231.h"
#include "at86rf231_spi.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "kernel_types.h"
#include "transceiver.h"
#include "hwtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef AT86RF231_SPI_SPEED
#define SPI_SPEED    SPI_SPEED_5MHZ
#else
#define SPI_SPEED    AT86RF231_SPI_SPEED
#endif

#define _MAX_RETRIES    (100)

static uint16_t radio_pan;
static uint8_t  radio_channel;
static uint16_t radio_address;
static uint64_t radio_address_long;

netdev_802154_raw_packet_cb_t at86rf231_raw_packet_cb;
netdev_rcv_data_cb_t at86rf231_data_packet_cb;

/* default source address length for sending in number of byte */
static size_t _default_src_addr_len = 2;

uint8_t  driver_state;

void at86rf231_gpio_spi_interrupts_init(void);
void at86rf231_reset(void);

#ifdef MODULE_TRANSCEIVER
void at86rf231_init(kernel_pid_t tpid)
{
    transceiver_pid = tpid;
    at86rf231_initialize(NULL);
}
#endif

int at86rf231_initialize(netdev_t *dev)
{
    at86rf231_gpio_spi_interrupts_init();

    at86rf231_reset();

    at86rf231_on();

    /* TODO :
     * and configure security, power
     */
#ifdef MODULE_CONFIG
    at86rf231_set_pan(sysconfig.radio_pan_id);
#else
    at86rf231_set_pan(0x0001);
#endif

    radio_channel = at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA) & AT86RF231_PHY_CC_CCA_MASK__CHANNEL;

    radio_address = 0x00FF & (uint16_t)at86rf231_reg_read(AT86RF231_REG__SHORT_ADDR_0);
    radio_address |= at86rf231_reg_read(AT86RF231_REG__SHORT_ADDR_1) << 8;

    radio_address_long = 0x00000000000000FF & (uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_0);
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 8;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 16;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 24;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 32;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 40;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 48;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 56;

    return 0;
}

int at86rf231_on(void)
{
    /* Send a FORCE TRX OFF command */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__FORCE_TRX_OFF);

    /* busy wait for TRX_OFF state */
    do {
        int delay = _MAX_RETRIES;
        if (!--delay) {
            DEBUG("at86rf231 : ERROR : could not enter TRX_OFF mode\n");
            return 0;
        }
    } while (at86rf231_get_status() != AT86RF231_TRX_STATUS__TRX_OFF);

    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__RX_ON);

    /* change into basic reception mode to initialise CSMA seed by RNG */
    do {
        int delay = _MAX_RETRIES;
        if (!--delay) {
            DEBUG("at86rf231 : ERROR : could not enter RX_ON mode\n");
            return 0;
        }
    } while (at86rf231_get_status() != AT86RF231_TRX_STATUS__RX_ON);

    /* read RNG values into CSMA_SEED_0 */
    for (int i=0; i<7; i+=2) {
        uint8_t tmp = at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA);
        tmp = ((tmp&0x60)>>5);
        at86rf231_reg_write(AT86RF231_REG__CSMA_SEED_0, tmp << i);
    }
    /* read CSMA_SEED_1 and write back with RNG value */
    uint8_t tmp = at86rf231_reg_read(AT86RF231_REG__CSMA_SEED_1);
    tmp |= ((at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA)&0x60)>>5);
    at86rf231_reg_write(AT86RF231_REG__CSMA_SEED_1, tmp);

    /* change into reception mode */
    at86rf231_switch_to_rx();

    return 1;
}

void at86rf231_off(void)
{
    /* TODO */
}

int at86rf231_is_on(void)
{
    return ((at86rf231_get_status() & 0x1f) != 0);
}

void at86rf231_switch_to_rx(void)
{
    gpio_irq_disable(AT86RF231_INT);

    /* now change to PLL_ON state for extended operating mode */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__PLL_ON);

    do {
        int max_wait = _MAX_RETRIES;
        if (!--max_wait) {
            DEBUG("at86rf231 : ERROR : could not enter PLL_ON mode\n");
            break;
        }
    } while (at86rf231_get_status() != AT86RF231_TRX_STATUS__PLL_ON);

    /* Reset IRQ to TRX END only */
    at86rf231_reg_write(AT86RF231_REG__IRQ_MASK, AT86RF231_IRQ_STATUS_MASK__TRX_END);

    /* Read IRQ to clear it */
    at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);

    /* Enable IRQ interrupt */
    gpio_irq_enable(AT86RF231_INT);

    /* Enter RX_AACK_ON state */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__RX_AACK_ON);

    do {
        int max_wait = _MAX_RETRIES;
        if (!--max_wait) {
            DEBUG("at86rf231 : ERROR : could not enter RX_AACK_ON mode\n");
            break;
        }
    } while (at86rf231_get_status() != AT86RF231_TRX_STATUS__RX_AACK_ON);
}

void at86rf231_rxoverflow_irq(void)
{
    /* TODO */
}

void at86rf231_rx_irq(void)
{
    /* check if we are in sending state */
    if (driver_state == AT_DRIVER_STATE_SENDING) {
        /* Read IRQ to clear it */
        at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);
        /* tx done, listen again */
        at86rf231_switch_to_rx();
        /* clear internal state */
        driver_state = AT_DRIVER_STATE_DEFAULT;
    }
    else {
        /* handle receive */
        at86rf231_rx_handler();
    }
}

int at86rf231_add_raw_recv_callback(netdev_t *dev,
                                    netdev_802154_raw_packet_cb_t recv_cb)
{
    (void)dev;

    if (at86rf231_raw_packet_cb == NULL){
        at86rf231_raw_packet_cb = recv_cb;
        return 0;
    }

    return -ENOBUFS;

}

int at86rf231_rem_raw_recv_callback(netdev_t *dev,
                                    netdev_802154_raw_packet_cb_t recv_cb)
{
    (void)dev;

    at86rf231_raw_packet_cb = NULL;
    return 0;
}

int at86rf231_add_data_recv_callback(netdev_t *dev,
                                     netdev_rcv_data_cb_t recv_cb)
{
    (void)dev;

    if (at86rf231_data_packet_cb == NULL){
        at86rf231_data_packet_cb = recv_cb;
        return 0;
    }

    return -ENOBUFS;
}

int at86rf231_rem_data_recv_callback(netdev_t *dev,
                                     netdev_rcv_data_cb_t recv_cb)
{
    (void)dev;

    at86rf231_data_packet_cb = NULL;
    return 0;
}

radio_address_t at86rf231_set_address(radio_address_t address)
{
    radio_address = address;

    uint8_t old_state = at86rf231_get_status() & AT86RF231_TRX_STATUS_MASK__TRX_STATUS;

    /* Go to state PLL_ON */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__PLL_ON);

    /* wait until it is on PLL_ON state */
    do {
        int max_wait = _MAX_RETRIES;
        if (!--max_wait) {
            DEBUG("at86rf231 : ERROR : could not enter PLL_ON mode\n");
            break;
        }
    } while ((at86rf231_get_status() & AT86RF231_TRX_STATUS_MASK__TRX_STATUS)
             != AT86RF231_TRX_STATUS__PLL_ON);

    at86rf231_reg_write(AT86RF231_REG__SHORT_ADDR_0, (uint8_t)(0x00FF & radio_address));
    at86rf231_reg_write(AT86RF231_REG__SHORT_ADDR_1, (uint8_t)(radio_address >> 8));

    /* Go to state old state */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, old_state);

    /* wait until it is on old state */
    do {
        int max_wait = _MAX_RETRIES;
        if (!--max_wait) {
            DEBUG("at86rf231 : ERROR : could not enter old state %x\n", old_state);
            break;
        }
    } while ((at86rf231_get_status() & AT86RF231_TRX_STATUS_MASK__TRX_STATUS)
             != old_state);

    return radio_address;
}

radio_address_t at86rf231_get_address(void)
{
    return radio_address;
}

uint64_t at86rf231_set_address_long(uint64_t address)
{
    radio_address_long = address;

    uint8_t old_state = at86rf231_get_status() & AT86RF231_TRX_STATUS_MASK__TRX_STATUS;

    /* Go to state PLL_ON */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__PLL_ON);

    /* wait until it is on PLL_ON state */
    do {
        int max_wait = _MAX_RETRIES;
        if (!--max_wait) {
            DEBUG("at86rf231 : ERROR : could not enter PLL_ON mode\n");
            break;
        }
    } while ((at86rf231_get_status() & AT86RF231_TRX_STATUS_MASK__TRX_STATUS)
             != AT86RF231_TRX_STATUS__PLL_ON);

    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_0, (uint8_t)(0x00000000000000FF & radio_address_long));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_1, (uint8_t)(0x000000000000FF00 & radio_address_long >> 8));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_2, (uint8_t)(0x0000000000FF0000 & radio_address_long >> 16));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_3, (uint8_t)(0x00000000FF000000 & radio_address_long >> 24));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_4, (uint8_t)(0x000000FF00000000 & radio_address_long >> 32));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_5, (uint8_t)(0x0000FF0000000000 & radio_address_long >> 40));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_6, (uint8_t)(0x00FF000000000000 & radio_address_long >> 48));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_7, (uint8_t)(radio_address_long >> 56));

    /* Go to state old state */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, old_state);

    /* wait until it is on old state */
    do {
        int max_wait = _MAX_RETRIES;
        if (!--max_wait) {
            DEBUG("at86rf231 : ERROR : could not enter old state %x\n", old_state);
            break;
        }
    } while ((at86rf231_get_status() & AT86RF231_TRX_STATUS_MASK__TRX_STATUS)
             != old_state);

    return radio_address_long;
}

uint64_t at86rf231_get_address_long(void)
{
    return radio_address_long;
}

uint16_t at86rf231_set_pan(uint16_t pan)
{
    radio_pan = pan;

    at86rf231_reg_write(AT86RF231_REG__PAN_ID_0, (uint8_t)(0x00FF & radio_pan));
    at86rf231_reg_write(AT86RF231_REG__PAN_ID_1, (uint8_t)(radio_pan >> 8));

    return radio_pan;
}

uint16_t at86rf231_get_pan(void)
{
    return radio_pan;
}

int at86rf231_set_channel(unsigned int channel)
{
    radio_channel = channel;

    if (channel < AT86RF231_MIN_CHANNEL ||
        channel > AT86RF231_MAX_CHANNEL) {
#if DEVELHELP
        puts("[at86rf231] channel out of range!");
#endif
        return -1;
    }

    at86rf231_reg_write(AT86RF231_REG__PHY_CC_CCA, (radio_channel & AT86RF231_PHY_CC_CCA_MASK__CHANNEL));

    return radio_channel;
}

unsigned int at86rf231_get_channel(void)
{
    return radio_channel;
}

void at86rf231_set_monitor(int mode)
{
    /* read register */
    uint8_t tmp = at86rf231_reg_read(AT86RF231_REG__XAH_CTRL_1);
    /* set promicuous mode depending on *mode* */
    if (mode) {
        at86rf231_reg_write(AT86RF231_REG__XAH_CTRL_1, (tmp|AT86RF231_XAH_CTRL_1__AACK_PROM_MODE));
    }
    else {
        at86rf231_reg_write(AT86RF231_REG__XAH_CTRL_1, (tmp&(~AT86RF231_XAH_CTRL_1__AACK_PROM_MODE)));
    }
}

int at86rf231_get_monitor(void)
{
    return (at86rf231_reg_read(AT86RF231_REG__XAH_CTRL_1) & AT86RF231_XAH_CTRL_1__AACK_PROM_MODE);
}

void at86rf231_gpio_spi_interrupts_init(void)
{
    /* SPI init */
    spi_init_master(AT86RF231_SPI, SPI_CONF_FIRST_RISING, SPI_SPEED);
    /* IRQ0 */
    gpio_init_int(AT86RF231_INT, GPIO_NOPULL, GPIO_RISING, (gpio_cb_t)at86rf231_rx_irq, NULL);
    /* CS */
    gpio_init_out(AT86RF231_CS, GPIO_NOPULL);
    /* SLEEP */
    gpio_init_out(AT86RF231_SLEEP, GPIO_NOPULL);
    /* RESET */
    gpio_init_out(AT86RF231_RESET, GPIO_NOPULL);
}

void at86rf231_reset(void)
{
    /* force reset */
    gpio_clear(AT86RF231_RESET);

    /* put pins to default values */
    gpio_set(AT86RF231_CS);
    gpio_clear(AT86RF231_SLEEP);

    /* additional waiting to comply to min rst pulse width */
    uint8_t volatile delay = 50; /* volatile to ensure it isn't optimized away */
    while (--delay);
    gpio_set(AT86RF231_RESET);
}

int at86rf231_get_option(netdev_t *dev, netdev_opt_t opt, void *value,
                         size_t *value_len)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see at86rf231_init) remove when adapter for transceiver exists */
    if (dev != &at86rf231_netdev) {
        return -ENODEV;
    }

    switch (opt) {
        case NETDEV_OPT_CHANNEL:
            if (*value_len < sizeof(unsigned int)) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(unsigned int)) {
                *value_len = sizeof(unsigned int);
            }
            *((unsigned int *)value) = at86rf231_get_channel();
            break;

        case NETDEV_OPT_ADDRESS:
            if (*value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(uint16_t)) {
                *value_len = sizeof(uint16_t);
            }
            *((uint16_t *)value) = at86rf231_get_address();
            break;

        case NETDEV_OPT_NID:
            if (*value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(uint16_t)) {
                *value_len = sizeof(uint16_t);
            }
            *((uint16_t *)value) = at86rf231_get_pan();
            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if (*value_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(uint64_t)) {
                *value_len = sizeof(uint64_t);
            }
            *((uint64_t *)value) = at86rf231_get_address_long();
            break;

        case NETDEV_OPT_MAX_PACKET_SIZE:
            if (*value_len == 0) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(uint8_t)) {
                *value_len = sizeof(uint8_t);
            }
            *((uint8_t *)value) = AT86RF231_MAX_PKT_LENGTH;
            break;

        case NETDEV_OPT_PROTO:
            if (*value_len < sizeof(netdev_proto_t)) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(netdev_proto_t)) {
                *value_len = sizeof(netdev_proto_t);
            }
            *((netdev_type_t *)value) = NETDEV_PROTO_802154;
            break;

        case NETDEV_OPT_SRC_LEN:
            if (*value_len < sizeof(size_t)) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(size_t)) {
                *value_len = sizeof(size_t);
            }
            *((size_t *)value) = _default_src_addr_len;

        default:
            return -ENOTSUP;
    }

    return 0;
}

static int _type_pun_up_unsigned(void *value_out, size_t desired_len,
                                 void *value_in, size_t given_len)
{
    if (given_len > desired_len) {
        return -EOVERFLOW;
    }

    /* XXX this is ugly, but bear with me */
    switch (given_len) {
        case 8:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (*((uint64_t *)value_in));
                    return 0;
                default:
                    return -EINVAL;
            }

        case 4:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint32_t *)value_in));
                    return 0;
                case 4:
                    *((uint32_t *)value_out) = (*((uint32_t *)value_in));
                    return 0;
                default:
                    return -EINVAL;
            }

        case 2:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint16_t *)value_in));
                    return 0;
                case 4:
                    *((uint32_t *)value_out) = (uint32_t)(*((uint16_t *)value_in));
                    return 0;
                case 2:
                    *((uint16_t *)value_out) = (*((uint16_t *)value_in));
                    return 0;
                default:
                    return -EINVAL;
            }

        case 1:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint8_t *)value_in));
                    return 0;
                case 4:
                    *((uint32_t *)value_out) = (uint32_t)(*((uint8_t *)value_in));
                    return 0;
                case 2:
                    *((uint16_t *)value_out) = (uint16_t)(*((uint8_t *)value_in));
                    return 0;
                case 1:
                    *((uint8_t *)value_out) = (*((uint8_t *)value_in));
                    return 0;
                default:
                    return -EINVAL;
            }

        default:
            return -EINVAL;
    }
}

int at86rf231_set_option(netdev_t *dev, netdev_opt_t opt, void *value,
                         size_t value_len)
{
    uint8_t set_value[sizeof(uint64_t)];
    int res = 0;

    /* XXX: first check only for backwards compatibility with transceiver
     *      (see at86rf231_init) remove when adapter for transceiver exists */
    if (dev != &at86rf231_netdev) {
        return -ENODEV;
    }

    switch (opt) {
        case NETDEV_OPT_CHANNEL:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(unsigned int),
                                             value, value_len)) == 0) {
                unsigned int *v = (unsigned int *)set_value;
                if (*v > 26) {
                    return -EINVAL;
                }
                at86rf231_set_channel(*v);
            }
            break;

        case NETDEV_OPT_ADDRESS:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(uint16_t),
                                             value, value_len)) == 0) {
                uint16_t *v = (uint16_t *)set_value;
                if (*v == 0xffff) {
                    /* Do not allow setting to broadcast */
                    return -EINVAL;
                }
                at86rf231_set_address(*v);
            }
            break;

        case NETDEV_OPT_NID:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(uint16_t),
                                             value, value_len)) == 0) {
                uint16_t *v = (uint16_t *)set_value;
                if (*v == 0xffff) {
                    /* Do not allow setting to broadcast */
                    return -EINVAL;
                }
                at86rf231_set_pan(*v);
            }
            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(uint64_t),
                                             value, value_len)) == 0) {
                uint64_t *v = (uint64_t *)set_value;
                /* TODO: error checking? */
                at86rf231_set_address_long(*v);
            }
            break;

        case NETDEV_OPT_SRC_LEN:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(size_t),
                                             value, value_len)) == 0) {
                size_t *v = (size_t *)set_value;

                if (*v != 2 && *v != 8) {
                    return -EINVAL;
                }
                _default_src_addr_len = *v;
            }
            break;

        default:
            return -ENOTSUP;
    }

    return res;
}

int at86rf231_get_state(netdev_t *dev, netdev_state_t *state)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see at86rf231_init) remove when adapter for transceiver exists */
    if (dev != &at86rf231_netdev) {
        return -ENODEV;
    }

    if (!at86rf231_is_on()) {
        *state = NETDEV_STATE_POWER_OFF;
    }
    else if (at86rf231_get_monitor()) {
        *state = NETDEV_STATE_PROMISCUOUS_MODE;
    }
    else {
        *state = NETDEV_STATE_RX_MODE;
    }

    return 0;
}

int at86rf231_set_state(netdev_t *dev, netdev_state_t state)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see at86rf231_init) remove when adapter for transceiver exists */
    if (dev != &at86rf231_netdev) {
        return -ENODEV;
    }

    if (state != NETDEV_STATE_PROMISCUOUS_MODE && at86rf231_get_monitor()) {
        at86rf231_set_monitor(0);
    }

    switch (state) {
        case NETDEV_STATE_POWER_OFF:
            at86rf231_off();
            break;

        case NETDEV_STATE_RX_MODE:
            at86rf231_switch_to_rx();
            break;

        case NETDEV_STATE_PROMISCUOUS_MODE:
            at86rf231_set_monitor(1);
            break;

        default:
            return -ENOTSUP;
    }

    return 0;
}

int at86rf231_channel_is_clear(netdev_t *dev)
{
    (void)dev;
    /* channel is checked by hardware automatically before transmission */
    return 1;
}

void at86rf231_event(netdev_t *dev, uint32_t event_type)
{
    (void)dev;
    (void)event_type;
}

const netdev_802154_driver_t at86rf231_driver = {
    .init = at86rf231_initialize,
    .send_data = netdev_802154_send_data,
    .add_receive_data_callback = at86rf231_add_data_recv_callback,
    .rem_receive_data_callback = at86rf231_rem_data_recv_callback,
    .get_option = at86rf231_get_option,
    .set_option = at86rf231_set_option,
    .get_state = at86rf231_get_state,
    .set_state = at86rf231_set_state,
    .event = at86rf231_event,
    .load_tx = at86rf231_load_tx_buf,
    .transmit = at86rf231_transmit_tx_buf,
    .send = netdev_802154_send,
    .add_receive_raw_callback = at86rf231_add_raw_recv_callback,
    .rem_receive_raw_callback = at86rf231_rem_raw_recv_callback,
    .channel_is_clear = at86rf231_channel_is_clear,
};

netdev_t at86rf231_netdev = { NETDEV_TYPE_802154, (netdev_driver_t *) &at86rf231_driver, NULL };
