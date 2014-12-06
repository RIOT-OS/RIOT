/**
  * cc2420.c - Implementation of cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  * Copyright (C) 2014 Kévin Roussel <Kevin.Roussel@inria.fr>
  *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
  */

#include "crash.h"
#include "cc2420.h"
#include "cc2420_spi.h"
#include "cc2420_settings.h"
#include "cc2420_arch.h"
#include "hwtimer.h"
#include "netdev/802154.h"
#include "netdev/base.h"
#include "transceiver.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"


/* number of attempts to turn on transceiver at startup, before panicking */
#define CC2420_STARTUP_ATTEMPTS  3

/* startup timeout (2 ms) in 16MHz-clock cycles */
#define CC2420_STARTUP_TIMEOUT  32000U

/* default source address length for sending in number of byte */
static size_t _default_src_addr_len = 2;

/* implementation of driver's functions */

int cc2420_initialize(netdev_t *dev)
{
    cc2420_spi_init();
    hwtimer_wait(CC2420_WAIT_TIME);
    cc2420_reset();

    int ok = 0;

    for (int i = 0; i < CC2420_STARTUP_ATTEMPTS; i++) {
        ok = cc2420_on();

        if (ok) {
            break;
        }
    }

    if (!ok) {
        core_panic(0x2420, "Could not start CC2420 radio transceiver");
    }

    hwtimer_wait(CC2420_WAIT_TIME);

    uint16_t reg = cc2420_read_reg(CC2420_REG_MDMCTRL0);
    reg |= CC2420_ADR_DECODE;                     /* enable auto adress decoding */
    reg |= CC2420_AUTOACK;                        /* enable auto ACK */
    reg |= CC2420_AUTOCRC;                        /* enable auto CRC */
    reg &= ~(CC2420_RES_FRM_MODE);                /* disable reserved frames */
    cc2420_write_reg(CC2420_REG_MDMCTRL0, reg);

    /* Change default values as recomended in the data sheet, */
    /* RX bandpass filter = 1.3uA. */
    reg = cc2420_read_reg(CC2420_REG_RXCTRL1);
    reg |= CC2420_RXBPF_LOCUR;
    cc2420_write_reg(CC2420_REG_RXCTRL1, reg);

    /* Set the FIFOP threshold to maximum. */
    cc2420_write_reg(CC2420_REG_IOCFG0, 127);

    /* Turn off "Security enable" (page 32). */
    reg = cc2420_read_reg(CC2420_REG_SECCTRL0);
    reg &= ~CC2420_RXFIFO_PROTECTION;
    cc2420_write_reg(CC2420_REG_SECCTRL0, reg);

    cc2420_set_tx_power(0);
    cc2420_set_channel(CC2420_DEFAULT_CHANNR);
    cc2420_set_pan(0x1111);
    DEBUG("CC2420 initialized and set to channel %i and pan 0x1111\n", CC2420_DEFAULT_CHANNR);
    cc2420_init_interrupts();
    cc2420_switch_to_rx();

    return 0;
}

#ifdef MODULE_TRANSCEIVER
void cc2420_init(kernel_pid_t tpid)
{
    transceiver_pid = tpid;
    cc2420_initialize(NULL); /* TODO: actually use netdev */
}
#endif

int cc2420_on(void)
{
    /* enable transceiver's crystal oscillator */
    cc2420_strobe(CC2420_STROBE_XOSCON);
    /* wait for the oscillator to be stable */
    unsigned int delay_on = 0;

    do {
        delay_on++;

        if (delay_on >= CC2420_STARTUP_TIMEOUT) {
            /* could not start up radio transceiver!*/
            return 0;
        }
    } while ((cc2420_status_byte() & CC2420_STATUS_XOSC16M_STABLE) == 0);

    hwtimer_wait(CC2420_WAIT_TIME);
    /* discard any potential garbage in TX buffer */
    cc2420_strobe(CC2420_STROBE_FLUSHTX);
    /* switch to RX mode */
    cc2420_switch_to_rx();
    /* OK, radio is on */
    return 1;
}

void cc2420_off(void)
{
    /* halt any pending communication... */
    cc2420_strobe(CC2420_STROBE_RFOFF);
    /* ... and put the transceiver in power-down mode */
    cc2420_strobe(CC2420_STROBE_XOSCOFF);
}

int cc2420_is_on(void)
{
    return ((cc2420_status_byte() & CC2420_STATUS_XOSC16M_STABLE) != 0);
}

void cc2420_switch_to_rx(void)
{
    cc2420_strobe(CC2420_STROBE_RFOFF);
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
    cc2420_strobe(CC2420_STROBE_RXON);
}

void cc2420_rxoverflow_irq(void)
{
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
    /* CC2420 datasheet says to do this twice... */
    cc2420_strobe(CC2420_STROBE_FLUSHRX);
}

void cc2420_rx_irq(void)
{
    cc2420_rx_handler();
}

void cc2420_event(netdev_t *dev, uint32_t event_type)
{
    (void)dev;
    (void)event_type;
}

void cc2420_set_monitor(int mode)
{
    uint16_t reg = cc2420_read_reg(CC2420_REG_MDMCTRL0);

    if (mode) {
        reg &= ~CC2420_ADR_DECODE;
    }
    else {
        reg |= CC2420_ADR_DECODE;
    }

    cc2420_write_reg(CC2420_REG_MDMCTRL0, reg);
}

int cc2420_get_monitor(void)
{
    uint16_t reg = cc2420_read_reg(CC2420_REG_MDMCTRL0);
    return ((reg & CC2420_ADR_DECODE) == 0);
}

int cc2420_set_channel(unsigned int chan)
{
    if (chan < 11 || chan > 26) {
        DEBUG("Invalid channel %i set. Valid channels are 11 through 26\n", chan);
        return -1;
    }

    /*
     * calculation from http://www.ti.com/lit/ds/symlink/cc2420.pdf p.50
     */
    uint16_t freq = 357 + (5 * (chan - 11));
    cc2420_write_reg(CC2420_REG_FSCTRL, freq);
    return ((unsigned int) chan);
}

unsigned int cc2420_get_channel(void)
{
    /* undo calculation from cc2420_set_channel() */
    return ((cc2420_read_reg(CC2420_REG_FSCTRL) - 357) / 5) + 11;
}

uint16_t cc2420_set_address(uint16_t addr)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(addr & 0xFF);
    buf[1] = (uint8_t)(addr >> 8);
    cc2420_write_ram(CC2420_RAM_SHORTADR, buf, 2);
    cc2420_set_address_long(0xFFFF & addr);
    return addr;
}

uint64_t cc2420_set_address_long(uint64_t addr)
{
    uint8_t buf[8];
    buf[0] = (uint8_t)(addr & 0xFF);
    buf[1] = (uint8_t)(addr >> 8);
    buf[2] = (uint8_t)(addr >> 16);
    buf[3] = (uint8_t)(addr >> 24);
    buf[4] = (uint8_t)(addr >> 32);
    buf[5] = (uint8_t)(addr >> 40);
    buf[6] = (uint8_t)(addr >> 48);
    buf[7] = (uint8_t)(addr >> 56);
    cc2420_write_ram(CC2420_RAM_IEEEADR, buf, 8);
    return addr;
}

uint16_t cc2420_get_address(void)
{
    uint16_t addr;
    cc2420_read_ram(CC2420_RAM_SHORTADR, (uint8_t *)&addr, sizeof(addr));
    return addr;
}

uint64_t cc2420_get_address_long(void)
{
    uint64_t addr;
    cc2420_read_ram(CC2420_RAM_IEEEADR, (uint8_t *)&addr, sizeof(addr));
    return addr;
}

uint16_t cc2420_set_pan(uint16_t pan)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(pan & 0xFF);
    buf[1] = (uint8_t)(pan >> 8);
    cc2420_write_ram(CC2420_RAM_PANID, buf, 2);
    return pan;
}

uint16_t cc2420_get_pan(void)
{
    uint16_t pan;
    cc2420_read_ram(CC2420_RAM_PANID, (uint8_t *)&pan, sizeof(pan));
    return pan;
}

static const uint8_t DBM_TO_LEVEL[32] = {
    31, 27, 25, 23, 21, 19, 17, 15, 13, 12, 11, 10, 9, 8, 7, 7,
    6, 6, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

int cc2420_set_tx_power(int pow)
{
    uint16_t txctrl_reg = cc2420_read_reg(CC2420_REG_TXCTRL);
    /* reset PA_LEVEL bits */
    txctrl_reg &= 0xFFE0;

    /* constrain power in transceiver's acceptable set of values */
    if (pow > 0) {
        pow = 0;
    }

    if (pow < -25) {
        pow = -25;
    }

    /* determine TX level from power in dBm */
    uint8_t level = DBM_TO_LEVEL[-pow];
    /* put wanted value in PA_LEVEL bits, and write back register */
    txctrl_reg |= level;
    cc2420_write_reg(CC2420_REG_TXCTRL, txctrl_reg);
    return pow;
}

static const int LEVEL_TO_DBM[32] = {
    -25, -25, -25, -24, -21, -19, -17, -15, -13, -12, -11, -10, -9, -8, -7, -7,
    -6, -6, -5, -5, -4, -4, -3, -3, -2, -2, -1, -1, 0, 0, 0, 0
};

int cc2420_get_tx_power(void)
{
    uint16_t txctrl_reg = cc2420_read_reg(CC2420_REG_TXCTRL);
    /* PA_LEVEL is in the 5 least-significant bits of TXCTRL register */
    uint8_t level = txctrl_reg & 0x001F;
    /* determine output power in dBm from TX level */
    int pow = LEVEL_TO_DBM[level];
    return pow;
}

int cc2420_channel_clear(netdev_t *dev)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
        return -ENODEV;
    }

    return (cc2420_get_cca() != 0) ? 1 : 0;
}

int cc2420_get_option(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t *value_len)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
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

            *((unsigned int *)value) = cc2420_get_channel();

            break;

        case NETDEV_OPT_ADDRESS:
            if (*value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint16_t)) {
                *value_len = sizeof(uint16_t);
            }

            *((uint16_t *)value) = cc2420_get_address();

            break;

        case NETDEV_OPT_NID:
            if (*value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint16_t)) {
                *value_len = sizeof(uint16_t);
            }

            *((uint16_t *)value) = cc2420_get_pan();

            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if (*value_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint64_t)) {
                *value_len = sizeof(uint64_t);
            }

            *((uint64_t *)value) = cc2420_get_address_long();

            break;

        case NETDEV_OPT_TX_POWER:
            if (*value_len < sizeof(int)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(int)) {
                *value_len = sizeof(int);
            }

            *((uint64_t *)value) = cc2420_get_tx_power();

            break;

        case NETDEV_OPT_MAX_PACKET_SIZE:
            if (*value_len == 0) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint8_t)) {
                *value_len = sizeof(uint8_t);
            }

            *((uint8_t *)value) = CC2420_MAX_PKT_LENGTH;

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

static int _type_pun_up_signed(void *value_out, size_t desired_len,
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
                    *((int64_t *)value_out) = (*((int64_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 4:
            switch (desired_len) {
                case 8:
                    *((int64_t *)value_out) = (int64_t)(*((int32_t *)value_in));
                    return 0;

                case 4:
                    *((int32_t *)value_out) = (*((int32_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 2:
            switch (desired_len) {
                case 8:
                    *((int64_t *)value_out) = (int64_t)(*((int16_t *)value_in));
                    return 0;

                case 4:
                    *((int32_t *)value_out) = (int32_t)(*((int16_t *)value_in));
                    return 0;

                case 2:
                    *((int16_t *)value_out) = (*((int16_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 1:
            switch (desired_len) {
                case 8:
                    *((int64_t *)value_out) = (int64_t)(*((int8_t *)value_in));
                    return 0;

                case 4:
                    *((int32_t *)value_out) = (int32_t)(*((int8_t *)value_in));
                    return 0;

                case 2:
                    *((int16_t *)value_out) = (int16_t)(*((int8_t *)value_in));
                    return 0;

                case 1:
                    *((int8_t *)value_out) = (*((int8_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        default:
            return -EINVAL;
    }
}

int cc2420_set_option(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t value_len)
{
    uint8_t set_value[sizeof(uint64_t)];
    int res = 0;

    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
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

                cc2420_set_channel(*v);
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

                cc2420_set_address(*v);
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

                cc2420_set_pan(*v);
            }

            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(uint64_t),
                                             value, value_len)) == 0) {
                uint64_t *v = (uint64_t *)set_value;
                /* TODO: error checking? */
                cc2420_set_address_long(*v);
            }

            break;

        case NETDEV_OPT_TX_POWER:
            if ((res = _type_pun_up_signed(set_value, sizeof(int),
                                           value, value_len)) == 0) {
                int *v = (int *)set_value;
                /* TODO: error checking? */
                cc2420_set_tx_power(*v);
            }

            break;

        case NETDEV_OPT_SRC_LEN:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(size_t),
                                             value, value_len)) == 0) {
                size_t *v = (size_t *)set_value;

                if (*v != 2 || *v != 8) {
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


int cc2420_get_state(netdev_t *dev, netdev_state_t *state)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
        return -ENODEV;
    }

    if (!cc2420_is_on()) {
        *state = NETDEV_STATE_POWER_OFF;
    }
    else if (cc2420_get_monitor()) {
        *state = NETDEV_STATE_PROMISCUOUS_MODE;
    }
    else {
        *state = NETDEV_STATE_RX_MODE;
    }

    return 0;
}

int cc2420_set_state(netdev_t *dev, netdev_state_t state)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
        return -ENODEV;
    }

    if (state != NETDEV_STATE_PROMISCUOUS_MODE && cc2420_get_monitor()) {
        cc2420_set_monitor(0);
    }

    switch (state) {
        case NETDEV_STATE_POWER_OFF:
            cc2420_off();
            break;

        case NETDEV_STATE_RX_MODE:
            cc2420_switch_to_rx();
            break;

        case NETDEV_STATE_PROMISCUOUS_MODE:
            cc2420_set_monitor(1);
            break;

        default:
            return -ENOTSUP;
    }

    return 0;
}

/* CC2420 netdev radio driver definition */
const netdev_802154_driver_t cc2420_driver = {
    cc2420_initialize,
    netdev_802154_send_data,
    cc2420_add_data_recv_callback,
    cc2420_rem_data_recv_callback,
    cc2420_get_option,
    cc2420_set_option,
    cc2420_get_state,
    cc2420_set_state,
    cc2420_event,
    cc2420_load_tx_buf,
    cc2420_transmit_tx_buf,
    netdev_802154_send,
    cc2420_add_raw_recv_callback,
    cc2420_rem_raw_recv_callback,
    cc2420_channel_clear,
};

netdev_t cc2420_netdev = { NETDEV_TYPE_802154, (netdev_driver_t *) &cc2420_driver, NULL };
