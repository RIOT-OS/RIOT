/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     kw2xrf
 * @{
 * @file        kw2xrf.c
 * @brief       Basic functionality of kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @}
 */
#include "crash.h"
#include "kw2xrf.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_internal.h"
#include "hwtimer.h"
#include "kernel_types.h"
#include "netdev/802154.h"
#include "netdev/base.h"
#include "thread.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

static uint16_t radio_pan;
static uint8_t  radio_channel;
static uint16_t radio_address;
static uint64_t radio_address_long;

#define MKW2XDRF_OUTPUT_POWER_MAX  8
#define MKW2XDRF_OUTPUT_POWER_MIN  (-35)

#define MKW2XDRF_PA_RANGE_MAX      31
#define MKW2XDRF_PA_RANGE_MIN      3

/* event type to signalize receive event */
/* TODO: Event_Nr used from cc2420 driver */
#define MKW2XDRF_NETDEV_EVENT_RX  (3452342219)

/* default source address length for sending in number of byte */
static size_t _default_src_addr_len = 2;

/* PLL integer lookup table */
static const uint8_t pll_int_lt[16] = {
    11, 11, 11, 11,
    11, 11, 12, 12,
    12, 12, 12, 12,
    13, 13, 13, 13
};

/* PLL frequency fractional lookup table */
static const uint16_t pll_frac_lt[16] = {
    10240, 20480, 30720, 40960,
    51200, 61440, 6144, 16384,
    26624, 36864, 47104, 57344,
    2048, 12288, 22528, 32768
};

static const uint8_t pow_lt[44] = {
    3, 4, 4, 5,
    6, 6, 7, 7,
    8, 9, 9, 10,
    11, 11, 12, 13,
    13, 14, 14, 15,
    16, 16, 17, 18,
    18, 19, 20, 20,
    21, 21, 22, 23,
    23, 24, 25, 25,
    26, 27, 27, 28,
    28, 29, 30, 31
};

static const int level_lt[29] = {
    -35, -34, -32, -31,
    -29, -28, -26, -25,
    -23, -22, -20, -19,
    -17, -16, -14, -13,
    -11, -10, -8, -7,
    -5, -4, -2, -1,
    1, 2, 4, 5,
    7
};

/* implementation of driver's functions */

void kw2xrf_init_interrupts(void)
{
    kw2xrf_write_dreg(MKW2XDM_IRQSTS1, 0xff);
    kw2xrf_write_dreg(MKW2XDM_IRQSTS2, MKW2XDM_IRQSTS2_PB_ERR_IRQ
                      | MKW2XDM_IRQSTS2_WAKE_IRQ);
    kw2xrf_write_dreg(MKW2XDM_IRQSTS3, 0x0f);
    DEBUG("IRQSTS1: %x\n", kw2xrf_read_dreg(MKW2XDM_IRQSTS1));
    DEBUG("IRQSTS2: %x\n", kw2xrf_read_dreg(MKW2XDM_IRQSTS2));
    DEBUG("IRQSTS3: %x\n", kw2xrf_read_dreg(MKW2XDM_IRQSTS3));
    uint8_t reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL2);
    //reg &= ~(MKW2XDM_PHY_CTRL2_RXMSK | MKW2XDM_PHY_CTRL2_SEQMSK);
    reg &= ~(MKW2XDM_PHY_CTRL2_RXMSK);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL2, reg);
    /* activate promiscous mode for testing*/
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
    reg |= MKW2XDM_PHY_CTRL4_PROMISCUOUS;
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, reg);
}

int kw2xrf_initialize(netdev_t *dev)
{
    if (dev == NULL || dev->driver != ((netdev_driver_t *)(&kw2xrf_driver))) {
        return -ENODEV;
    }

    dev->type = NETDEV_TYPE_802154;
    dev->more = NULL;
    //dev->event_handler = thread_getpid();

    kw2xrf_spi_init();

    if (!kw2xrf_on()) {
        core_panic(0x42, "Could not start MKW2XD radio transceiver");
    }

    kw2xrf_set_tx_power(0);
#ifdef MODULE_CONFIG
    radio_pan = sysconfig.radio_pan_id;
#else
    radio_pan = 0x0001;
#endif
    kw2xrf_set_pan(radio_pan);
    radio_channel = MKW2XDRF_DEFAULT_CHANNR;
    kw2xrf_set_channel(radio_channel);
    DEBUG("MKW2XDRF initialized and set to channel %i and pan 0x1111\n", MKW2XDRF_DEFAULT_CHANNR);

    kw2xrf_read_iregs(MKW2XDMI_MACSHORTADDRS0_LSB, (uint8_t *)&radio_address, 2);
    kw2xrf_read_iregs(MKW2XDMI_MACLONGADDRS0_0, (uint8_t *)&radio_address_long, 8);

    kw2xrf_init_interrupts();
    kw2xrf_switch_to_rx();

    return 1;
}

int kw2xrf_on(void)
{
    uint8_t tmp;
    /* check modem's crystal oscillator, CLK_OUT shall be 4MHz */
    tmp = kw2xrf_read_dreg(MKW2XDM_CLK_OUT_CTRL);

    if (tmp != 0x8Bu) {
        return 0;
    }

    DEBUG("SEQ_STATE: %x\n", kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));

    /* enable RFon mode */
    kw2xrf_write_dreg(MKW2XDM_PWR_MODES,
                      (MKW2XDM_PWR_MODES_XTALEN | MKW2XDM_PWR_MODES_PMC_MODE));

    /* abort any ongoing sequence */
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(0));

    return 1;
}

void kw2xrf_off(void)
{
    /* abort any ongoing sequence */
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(0));
    /* TODO: put the SiP in low-power mode */
    kw2xrf_write_dreg(MKW2XDM_PWR_MODES, MKW2XDM_PWR_MODES_XTALEN);
}

int kw2xrf_is_on(void)
{
    uint8_t tmp;
    tmp = kw2xrf_read_dreg(MKW2XDM_PWR_MODES);
    return ((tmp & MKW2XDM_PWR_MODES_PMC_MODE) != 0);
}

void kw2xrf_switch_to_rx(void)
{
    DEBUG("switch_to_rx: SEQ_STATE: %x\n", kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));

    /* check state befor new sequence */
    if (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE)) {
        /* abort any ongoing sequence */
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(0));

        while (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));
    }

    /* programm RX sequence */
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(1));
}

/* TODO: Rename, function is general and not rx-specific */
void kw2xrf_rx_irq(void *args)
{
    uint8_t irqst = kw2xrf_read_dreg(MKW2XDM_IRQSTS1);
    DEBUG("rx_irq: IRQSTS1: %x\n", irqst);
    /*
    DEBUG("rx_irq: IRQSTS2: %x\n", kw2xrf_read_dreg(MKW2XDM_IRQSTS2));
    DEBUG("rx_irq: CTRL1: %x\n", kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1));
    DEBUG("rx_irq: CTRL2: %x\n", kw2xrf_read_dreg(MKW2XDM_PHY_CTRL2));
    DEBUG("rx_irq: CTRL3: %x\n", kw2xrf_read_dreg(MKW2XDM_PHY_CTRL3));
    DEBUG("rx_irq: CTRL4: %x\n", kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4));
    DEBUG("rx_irq: PLL_INT0: %x\n", kw2xrf_read_dreg(MKW2XDM_PLL_INT0));
    DEBUG("rx_irq: PLL_FRAC0_LSB: %x\n", kw2xrf_read_dreg(MKW2XDM_PLL_FRAC0_LSB));
    DEBUG("rx_irq: PLL_FRAC0_MSB: %x\n", kw2xrf_read_dreg(MKW2XDM_PLL_FRAC0_MSB));
    DEBUG("rx_irq: PA_PWR: %x\n", kw2xrf_read_dreg(MKW2XDM_PA_PWR));
    DEBUG("rx_irq: PWR_MODES: %x\n", kw2xrf_read_dreg(MKW2XDM_PWR_MODES));
    */

    if (irqst & MKW2XDM_IRQSTS1_RXIRQ) {
        /* handle receive */
        kw2xrf_rx_handler();
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(0));
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(1));
    }

    kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_RXIRQ
                      | MKW2XDM_IRQSTS1_SEQIRQ);
}

uint16_t kw2xrf_set_address(uint16_t addr)
{
    radio_address = addr;

    uint8_t buf[2];
    buf[0] = (uint8_t)addr;
    buf[1] = (uint8_t)(addr >> 8);
    kw2xrf_write_iregs(MKW2XDMI_MACSHORTADDRS0_LSB, buf, 2);
    return addr;
}

uint16_t kw2xrf_get_address(void)
{
#ifdef DEBUG_ENABLED
    uint16_t addr;
    kw2xrf_read_iregs(MKW2XDMI_MACSHORTADDRS0_LSB, (uint8_t *)&addr, sizeof(addr));
    return addr;
#else
    return radio_address;
#endif
}

uint64_t kw2xrf_set_address_long(uint64_t addr)
{
    radio_address_long = addr;

    uint8_t buf[8];
    buf[0] = (uint8_t)addr;
    buf[1] = (uint8_t)(addr >> 8);
    buf[2] = (uint8_t)(addr >> 16);
    buf[3] = (uint8_t)(addr >> 24);
    buf[4] = (uint8_t)(addr >> 32);
    buf[5] = (uint8_t)(addr >> 40);
    buf[6] = (uint8_t)(addr >> 48);
    buf[7] = (uint8_t)(addr >> 56);
    kw2xrf_write_iregs(MKW2XDMI_MACLONGADDRS0_0, buf, 8);
    return addr;
}

uint64_t kw2xrf_get_address_long(void)
{
#ifdef DEBUG_ENABLED
    uint64_t addr;
    kw2xrf_read_iregs(MKW2XDMI_MACLONGADDRS0_0, (uint8_t *)&addr, sizeof(addr));
    return addr;
#else
    return radio_address_long;
#endif
}

uint16_t kw2xrf_set_pan(uint16_t pan)
{
    radio_pan = pan;

    uint8_t buf[2];
    buf[0] = (uint8_t)pan;
    buf[1] = (uint8_t)(pan >> 8);
    kw2xrf_write_iregs(MKW2XDMI_MACPANID0_LSB, buf, 2);
    return pan;
}

uint16_t kw2xrf_get_pan(void)
{
#ifdef DEBUG_ENABLED
    uint16_t pan;
    kw2xrf_read_iregs(MKW2XDMI_MACPANID0_LSB, (uint8_t *)&pan, sizeof(pan));
    return pan;
#else
    return radio_pan;
#endif
}


int kw2xrf_set_channel(unsigned int ch)
{
    radio_channel = ch;

    if (ch < 11 || ch > 26) {
        DEBUG("Invalid channel %i set. Valid channels are 11 through 26\n", ch);
        return -1;
    }

    /*
     * Fc = 2405 + 5(k - 11) , k = 11,12,...,26
     *
     * Equation for PLL frequency, MKW2xD Reference Manual, p.255 :
     * F = ((PLL_INT0 + 64) + (PLL_FRAC0/65536))32MHz
     *
     */
    ch -= 11;
    kw2xrf_write_dreg(MKW2XDM_PLL_INT0, MKW2XDM_PLL_INT0_VAL(pll_int_lt[ch]));
    kw2xrf_write_dreg(MKW2XDM_PLL_FRAC0_LSB, (uint8_t)pll_frac_lt[ch]);
    kw2xrf_write_dreg(MKW2XDM_PLL_FRAC0_MSB, (uint8_t)(pll_frac_lt[ch] >> 8));
    return ((unsigned int) ch);
}

unsigned int kw2xrf_get_channel(void)
{
#ifdef DEBUG_ENABLED
    uint8_t pll_int = kw2xrf_read_dreg(MKW2XDM_PLL_INT0);
    uint16_t pll_frac = kw2xrf_read_dreg(MKW2XDM_PLL_FRAC0_LSB);
    pll_frac |= ((uint16_t)kw2xrf_read_dreg(MKW2XDM_PLL_FRAC0_MSB) << 8);

    for (int i = 0; i < 16; i++) {
        if ((pll_frac_lt[i] == pll_frac) && (pll_int_lt[i] == pll_int)) {
            return i + 11;
        }
    }

    return 0;
#else
    return radio_channel;
#endif
}

void kw2xrf_set_monitor(int mode)
{
    uint8_t tmp;
    tmp = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
    tmp |= MKW2XDM_PHY_CTRL4_PROMISCUOUS;
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, tmp);
}

int kw2xrf_get_monitor(void)
{
    uint8_t tmp;
    tmp = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
    return ((tmp & MKW2XDM_PHY_CTRL4_PROMISCUOUS) != 0);
}


int kw2xrf_set_tx_power(int pow)
{
    if (pow > MKW2XDRF_OUTPUT_POWER_MAX) {
        pow = MKW2XDRF_OUTPUT_POWER_MAX;
    }

    if (pow < MKW2XDRF_OUTPUT_POWER_MIN) {
        pow = MKW2XDRF_OUTPUT_POWER_MIN;
    }

    uint8_t level = pow_lt[pow - MKW2XDRF_OUTPUT_POWER_MIN];
    kw2xrf_write_dreg(MKW2XDM_PA_PWR, MKW2XDM_PA_PWR(level));
    return pow;
}

int kw2xrf_get_tx_power(void)
{
    uint8_t level = kw2xrf_read_dreg(MKW2XDM_PA_PWR);

    if (level > MKW2XDRF_PA_RANGE_MAX) {
        level = MKW2XDRF_PA_RANGE_MAX;
    }

    if (level < MKW2XDRF_PA_RANGE_MIN) {
        level = MKW2XDRF_PA_RANGE_MIN;
    }

    return level_lt[level - MKW2XDRF_PA_RANGE_MIN];
}

inline int kw2xrf_channel_clear(netdev_t *dev)
{
    if (dev != &kw2xrf_netdev) {
        return -ENODEV;
    }

    return 1;
}

int kw2xrf_get_option(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t *value_len)
{
    if (dev != &kw2xrf_netdev) {
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

            *((unsigned int *)value) = kw2xrf_get_channel();

            break;

        case NETDEV_OPT_ADDRESS:
            if (*value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint16_t)) {
                *value_len = sizeof(uint16_t);
            }

            *((uint16_t *)value) = kw2xrf_get_address();

            break;

        case NETDEV_OPT_NID:
            if (*value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint16_t)) {
                *value_len = sizeof(uint16_t);
            }

            *((uint16_t *)value) = kw2xrf_get_pan();

            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if (*value_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint64_t)) {
                *value_len = sizeof(uint64_t);
            }

            *((uint64_t *)value) = kw2xrf_get_address_long();

            break;

        case NETDEV_OPT_TX_POWER:
            if (*value_len < sizeof(int)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(int)) {
                *value_len = sizeof(int);
            }

            *((uint64_t *)value) = kw2xrf_get_tx_power();

            break;

        case NETDEV_OPT_MAX_PACKET_SIZE:
            if (*value_len == 0) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint8_t)) {
                *value_len = sizeof(uint8_t);
            }

            *((uint8_t *)value) = MKW2XDRF_MAX_PKT_LENGTH;

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

int kw2xrf_set_option(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t value_len)
{
    uint8_t set_value[sizeof(uint64_t)];
    int res = 0;

    if (dev != &kw2xrf_netdev) {
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

                kw2xrf_set_channel(*v);
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

                kw2xrf_set_address(*v);
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

                kw2xrf_set_pan(*v);
            }

            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(uint64_t),
                                             value, value_len)) == 0) {
                uint64_t *v = (uint64_t *)set_value;
                /* TODO: error checking? */
                kw2xrf_set_address_long(*v);
            }

            break;

        case NETDEV_OPT_TX_POWER:
            if ((res = _type_pun_up_signed(set_value, sizeof(int),
                                           value, value_len)) == 0) {
                int *v = (int *)set_value;
                /* TODO: error checking? */
                kw2xrf_set_tx_power(*v);
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

int kw2xrf_get_state(netdev_t *dev, netdev_state_t *state)
{
    if (dev != &kw2xrf_netdev) {
        return -ENODEV;
    }

    if (!kw2xrf_is_on()) {
        *state = NETDEV_STATE_POWER_OFF;
    }
    else if (kw2xrf_get_monitor()) {
        *state = NETDEV_STATE_PROMISCUOUS_MODE;
    }
    else {
        *state = NETDEV_STATE_RX_MODE;
    }

    return 0;
}

int kw2xrf_set_state(netdev_t *dev, netdev_state_t state)
{
    if (dev != &kw2xrf_netdev) {
        return -ENODEV;
    }

    if (state != NETDEV_STATE_PROMISCUOUS_MODE && kw2xrf_get_monitor()) {
        kw2xrf_set_monitor(0);
    }

    switch (state) {
        case NETDEV_STATE_POWER_OFF:
            kw2xrf_off();
            break;

        case NETDEV_STATE_RX_MODE:
            kw2xrf_switch_to_rx();
            break;

        case NETDEV_STATE_PROMISCUOUS_MODE:
            kw2xrf_set_monitor(1);
            break;

        default:
            return -ENOTSUP;
    }

    return 0;
}

void kw2xrf_event(netdev_t *dev, uint32_t event_type)
{
    (void)dev;

    switch (event_type) {
        case MKW2XDRF_NETDEV_EVENT_RX:
            kw2xrf_rx_handler();

            break;

        default:

            break;
    }
}

/* KW2XDRF netdev radio driver definition */
const netdev_802154_driver_t kw2xrf_driver = {
    kw2xrf_initialize,
    netdev_802154_send_data,
    kw2xrf_add_data_recv_callback,
    kw2xrf_rem_data_recv_callback,
    kw2xrf_get_option,
    kw2xrf_set_option,
    kw2xrf_get_state,
    kw2xrf_set_state,
    kw2xrf_event,
    kw2xrf_load_tx_buf,
    kw2xrf_transmit_tx_buf,
    netdev_802154_send,
    kw2xrf_add_raw_recv_callback,
    kw2xrf_rem_raw_recv_callback,
    kw2xrf_channel_clear,
};

netdev_t kw2xrf_netdev = { NETDEV_TYPE_802154, (netdev_driver_t *) &kw2xrf_driver};
