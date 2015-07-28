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
 * @file
 * @brief       Basic functionality of kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @}
 */
#include "panic.h"
#include "kw2xrf.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_reg.h"
#include "mutex.h"
#include "msg.h"
#include "periph/gpio.h"
#include "periph/cpuid.h"
#include "net/ng_netbase.h"
#include "net/ng_ieee802154.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Internal driver event type in case of an unexpected interrupt
 */
#define ISR_EVENT_UNKNOWN      (0x0020)

/* Modem_PA_PWR Register (PA Power Control) has a valid range from 3-31 */
#define MKW2XDRF_PA_RANGE_MAX      31       /**< Maximum value of PA Power Control Register */
#define MKW2XDRF_PA_RANGE_MIN      3        /**< Minimum value of PA Power Control Register */

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

static gpio_t kw2xrf_gpio_int;

void kw2xrf_set_option(kw2xrf_t *dev, uint16_t option, bool state);

int kw2xrf_set_tx_power(kw2xrf_t *dev, int8_t *val, size_t len)
{
    if (val[0] > MKW2XDRF_OUTPUT_POWER_MAX) {
        val[0] = MKW2XDRF_OUTPUT_POWER_MAX;
    }

    if (val[0] < MKW2XDRF_OUTPUT_POWER_MIN) {
        val[0] = MKW2XDRF_OUTPUT_POWER_MIN;
    }

    uint8_t level = pow_lt[val[0] - MKW2XDRF_OUTPUT_POWER_MIN];
    kw2xrf_write_dreg(MKW2XDM_PA_PWR, MKW2XDM_PA_PWR(level));
    return 2;
}

int kw2xrf_get_channel(kw2xrf_t *dev, uint8_t *val, size_t max)
{
    if (max < 2) {
        return -EOVERFLOW;
    }

    uint8_t pll_int = kw2xrf_read_dreg(MKW2XDM_PLL_INT0);
    uint16_t pll_frac = kw2xrf_read_dreg(MKW2XDM_PLL_FRAC0_LSB);
    pll_frac |= ((uint16_t)kw2xrf_read_dreg(MKW2XDM_PLL_FRAC0_MSB) << 8);

    for (int i = 0; i < 16; i++) {
        if ((pll_frac_lt[i] == pll_frac) && (pll_int_lt[i] == pll_int)) {
            val[0] = i + 11;
            val[1] = 0;
            return 2;
        }
    }

    return -EINVAL;
}

int kw2xrf_get_sequence(void)
{
    int reg = 0;
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
    reg &= MKW2XDM_PHY_CTRL1_XCVSEQ_MASK;
    return reg;
}

void kw2xrf_set_sequence(kw2xrf_t *dev, kw2xrf_physeq_t seq)
{
    uint8_t reg = 0;

    /* Only interrupt interruptable states */
    uint8_t curr_seq = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
    curr_seq &= (MKW2XDM_PHY_CTRL1_XCVSEQ_MASK);

    if ((curr_seq == XCVSEQ_RECEIVE) || (curr_seq == XCVSEQ_CONTINUOUS_CCA)) {
        /* Clear all pending interrupts */
        gpio_irq_disable(kw2xrf_gpio_int);

        /* abort any ongoing sequence */
        DEBUG("kw2xrf_tx: abort SEQ_STATE: %x\n", kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));
        reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
        reg &= ~(MKW2XDM_PHY_CTRL1_XCVSEQ_MASK);
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);

        /* Mask all possible interrupts */
        reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL3);
        reg |= MKW2XDM_PHY_CTRL3_WAKE_MSK;
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL3, reg);

        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, 0x7f);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS2, 0x03);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS3, 0xff);

        gpio_irq_enable(kw2xrf_gpio_int);
    }

#ifdef DEVELHELP
    uint16_t max_tries = 0;
#endif

    /* Wait for all other states finish */
    while (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE)) {
#ifdef DEVELHELP
        max_tries++;

        /* At 10MHz SPI-Clock, 40000 should be in the magnitue of 0.1s */
        if (max_tries == 40000) {
            DEBUG("kw2xrf_error: device does not finish sequence\n");
            core_panic(-EBUSY, "kw2xrf_error: device does not finish sequence");
        }

#endif
    }

    /* For all sequences only enable SEQ-irq will be set when sequence completed */
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL2);
    reg &= ~(MKW2XDM_PHY_CTRL2_SEQMSK);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL2, reg);

    /* Progrmm new sequence */
    switch (seq) {
        case XCVSEQ_IDLE:
            dev->state = NG_NETOPT_STATE_SLEEP;
            break;

        case XCVSEQ_RECEIVE:
            dev->state = NG_NETOPT_STATE_IDLE;
            break;

        case XCVSEQ_TRANSMIT:
            dev->state = NG_NETOPT_STATE_TX;
            break;

        case XCVSEQ_CCA:
            dev->state = NG_NETOPT_STATE_TX;
            break;

        case XCVSEQ_TX_RX:
            dev->state = NG_NETOPT_STATE_TX;
            break;

        case XCVSEQ_CONTINUOUS_CCA:
            dev->state = NG_NETOPT_STATE_TX;
            break;

        default:
            DEBUG("kw2xrf: undefined state assigned to phy\n");
            dev->state = NG_NETOPT_STATE_IDLE;
    }

    /* Mapping of TX-sequences depending on AUTOACK flag */
    /* TODO: This should only used in combination with
     *       an CSMA-MAC layer. Currently not working
     */
    /*if((seq == XCVSEQ_TRANSMIT) || (seq == XCVSEQ_TX_RX)) {
        if((dev->option) & KW2XRF_OPT_AUTOACK) {
            seq = XCVSEQ_TX_RX;
        }
        else {
            seq = XCVSEQ_TRANSMIT;
        }
    }*/

    DEBUG("kw2xrf: Set sequence to %i\n", seq);
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
    reg &= ~(MKW2XDM_PHY_CTRL1_XCVSEQ_MASK);
    reg |= MKW2XDM_PHY_CTRL1_XCVSEQ(seq);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
}

int kw2xrf_set_channel(kw2xrf_t *dev, uint8_t *val, size_t len)
{
    /* Save old sequence to restore this state later */
    uint8_t old_seq = kw2xrf_get_sequence();

    if (old_seq) {
        kw2xrf_set_sequence(dev, XCVSEQ_IDLE);
    }

    if ((val[0] < 11) || (val[0] > 26)) {
        DEBUG("kw2xrf: Invalid channel %i set. Valid channels are 11 through 26\n", val[0]);
        return -ENOTSUP;
    }

    if ((len != 2) || (val[1] != 0)) {
        DEBUG("kw2xrf: set channel failed, len: %u, val[0]:%u\n", len, val[0]);
        return -EINVAL;
    }

    /*
     * Fc = 2405 + 5(k - 11) , k = 11,12,...,26
     *
     * Equation for PLL frequency, MKW2xD Reference Manual, p.255 :
     * F = ((PLL_INT0 + 64) + (PLL_FRAC0/65536))32MHz
     *
     */
    uint8_t tmp = val[0] - 11;
    kw2xrf_write_dreg(MKW2XDM_PLL_INT0, MKW2XDM_PLL_INT0_VAL(pll_int_lt[tmp]));
    kw2xrf_write_dreg(MKW2XDM_PLL_FRAC0_LSB, (uint8_t)pll_frac_lt[tmp]);
    kw2xrf_write_dreg(MKW2XDM_PLL_FRAC0_MSB, (uint8_t)(pll_frac_lt[tmp] >> 8));

    DEBUG("kw2xrf: set channel to %u\n", val[0]);

    if (old_seq) {
        kw2xrf_set_sequence(dev, old_seq);
    }

    return 2;
}

void kw2xrf_irq_handler(void *args)
{
    msg_t msg;
    kw2xrf_t *dev = (kw2xrf_t *)args;

    /* notify driver thread about the interrupt */
    msg.type = NG_NETDEV_MSG_TYPE_EVENT;
    msg_send_int(&msg, dev->mac_pid);
}

/* Set up interrupt sources, triggered by the radio-module */
void kw2xrf_init_interrupts(kw2xrf_t *dev, gpio_t int_pin)
{
    /* Clear all pending interrupts */
    kw2xrf_write_dreg(MKW2XDM_IRQSTS1, 0x7f);
    kw2xrf_write_dreg(MKW2XDM_IRQSTS2, 0x03);
    kw2xrf_write_dreg(MKW2XDM_IRQSTS3, 0xff);

    /* Disable all interrups:
     * Selectively enable only one interrupt source selectively in sequence manager.
     * After reset state all interrupts are disabled, except WAKE_IRQ.
     */
    int reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL3);
    reg |= MKW2XDM_PHY_CTRL3_WAKE_MSK;
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL3, reg);

    /* set up GPIO-pin used for IRQ */
    gpio_init_int(int_pin, GPIO_NOPULL, GPIO_FALLING, &kw2xrf_irq_handler, dev);
}

int kw2xrf_set_pan(kw2xrf_t *dev, uint16_t pan)
{
    dev->radio_pan = pan;

    uint8_t val_ar[2];
    val_ar[1] = (pan >> 8);
    val_ar[0] = (uint8_t)pan;
    kw2xrf_write_iregs(MKW2XDMI_MACPANID0_LSB, val_ar, 2);
    return 2;
}

int kw2xrf_get_proto(kw2xrf_t *dev, uint8_t *val, size_t max)
{
    if (max < sizeof(ng_nettype_t)) {
        return -EOVERFLOW;
    }

    memcpy(val, &(dev->proto), sizeof(ng_nettype_t));
    return sizeof(ng_nettype_t);
}

int kw2xrf_set_proto(kw2xrf_t *dev, uint8_t *val, size_t len)
{
    if (len != sizeof(ng_nettype_t)) {
        return -EINVAL;
    }

    memcpy(&(dev->proto), val, sizeof(ng_nettype_t));
    return sizeof(ng_nettype_t);
}

int kw2xrf_on(kw2xrf_t *dev)
{
    uint8_t tmp;
    /* check modem's crystal oscillator, CLK_OUT shall be 4MHz */
    tmp = kw2xrf_read_dreg(MKW2XDM_CLK_OUT_CTRL);

    if (tmp != 0x8Bu) {
        return -1;
    }

    DEBUG("SEQ_STATE: %x\n", kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));

    /* enable RFon mode */
    kw2xrf_write_dreg(MKW2XDM_PWR_MODES,
                      (MKW2XDM_PWR_MODES_XTALEN | MKW2XDM_PWR_MODES_PMC_MODE));

    /* abort any ongoing sequence */
    kw2xrf_set_sequence(dev, XCVSEQ_IDLE);

    dev->state = NG_NETOPT_STATE_SLEEP;
    return 0;
}

int kw2xrf_set_addr(kw2xrf_t *dev, uint16_t addr)
{
    uint8_t val_ar[2];
    val_ar[0] = (addr >> 8);
    val_ar[1] = (uint8_t)addr;
    dev->addr_short[0] = val_ar[0];
    dev->addr_short[1] = val_ar[1];
    kw2xrf_write_iregs(MKW2XDMI_MACSHORTADDRS0_LSB, val_ar, 2);
    return sizeof(uint16_t);
}

int kw2xrf_set_addr_long(kw2xrf_t *dev, uint64_t addr)
{
    for (int i = 0; i < 8; i++) {
        dev->addr_long[i] = (addr >> ((7 - i) * 8));
    }

    kw2xrf_write_iregs(MKW2XDMI_MACLONGADDRS0_0, (dev->addr_long), 8);

    return sizeof(uint64_t);
}

int kw2xrf_init(kw2xrf_t *dev, spi_t spi, spi_speed_t spi_speed,
                gpio_t cs_pin, gpio_t int_pin)
{
    uint8_t reg = 0;
    uint8_t tmp[2];
    kw2xrf_gpio_int = int_pin;
#if CPUID_ID_LEN
    uint8_t cpuid[CPUID_ID_LEN];
    eui64_t addr_long;
#endif

    /* check device parameters */
    if (dev == NULL) {
        return -ENODEV;
    }

    kw2xrf_spi_init(spi, spi_speed, cs_pin);

    if (kw2xrf_on(dev) != 0) {
        core_panic(0x42, "Could not start MKW2XD radio transceiver");
    }

    /* General initialization of interrupt sources.
     * sets radio to idle mode with all interrupt masked
     */
    kw2xrf_init_interrupts(dev, kw2xrf_gpio_int);

    /* set device driver */
    dev->driver = &kw2xrf_driver;
    /* set default option */
    dev->proto = KW2XRF_DEFAULT_PROTOCOL;
    dev->option = 0;

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
    kw2xrf_set_addr_long(dev, NTOHLL(addr_long.uint64.u64));
    kw2xrf_set_addr(dev, NTOHS(addr_long.uint16[3].u16));
#else
    kw2xrf_set_addr_long(dev, KW2XRF_DEFAULT_SHORT_ADDR);
    kw2xrf_set_addr(dev, KW2XRF_DEFAULT_ADDR_LONG);
#endif

    /* set default TX-Power */
    dev->tx_power = KW2XRF_DEFAULT_TX_POWER;
    kw2xrf_set_tx_power(dev, &(dev->tx_power), sizeof(dev->tx_power));

    /* set default channel */
    dev->radio_channel = KW2XRF_DEFAULT_CHANNEL;
    tmp[0] = dev->radio_channel;
    tmp[1] = 0;
    kw2xrf_set_channel(dev, tmp, 2);
    /* set default PAN ID */
    kw2xrf_set_pan(dev, KW2XRF_DEFAULT_PANID);

    /* CCA Setup */
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
    /* Set up CCA mode 1 (RSSI threshold) */
    reg |= MKW2XDM_PHY_CTRL4_CCATYPE(1);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, reg);
    DEBUG("kw2xrf: Initialized and set to channel %i and pan %i.\n",
          KW2XRF_DEFAULT_CHANNEL, KW2XRF_DEFAULT_PANID);

    kw2xrf_set_option(dev, KW2XRF_OPT_AUTOACK, true);
    kw2xrf_set_option(dev, KW2XRF_OPT_CSMA, true);

    /* Switch to Receive state per default after initialization */
    kw2xrf_set_sequence(dev, XCVSEQ_RECEIVE);
    return 0;
}

int kw2xrf_add_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }

    if (dev->event_cb != NULL) {
        return -ENOBUFS;
    }

    dev->event_cb = cb;
    return 0;
}

int kw2xrf_rem_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
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

uint64_t kw2xrf_get_addr_long(kw2xrf_t *dev)
{
    uint64_t addr;
    uint8_t *ap = (uint8_t *)(&addr);

    for (int i = 0; i < 8; i++) {
        ap[i] = dev->addr_long[7 - i];
    }

    return addr;
}

int kw2xrf_get(ng_netdev_t *netdev, ng_netopt_t opt, void *value, size_t max_len)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NG_NETOPT_ADDRESS:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            *((uint16_t *)value) = ((dev->addr_short[0] << 8) | dev->addr_short[1]);
            return sizeof(uint16_t);

        case NG_NETOPT_ADDRESS_LONG:
            if (max_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }

            *((uint64_t *)value) = kw2xrf_get_addr_long(dev);
            return sizeof(uint64_t);

        case NG_NETOPT_ADDR_LEN:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            *((uint16_t *)value) = 2;
            return sizeof(uint16_t);

        case NG_NETOPT_SRC_LEN:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (dev->option & KW2XRF_OPT_SRC_ADDR_LONG) {
                *((uint16_t *)value) = 8;
            }
            else {
                *((uint16_t *)value) = 2;
            }

            return sizeof(uint16_t);

        case NG_NETOPT_NID:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            *((uint16_t *)value) = dev->radio_pan;
            return sizeof(uint16_t);

        case NG_NETOPT_CHANNEL:
            return kw2xrf_get_channel(dev, (uint8_t *)value, max_len);

        case NG_NETOPT_PROTO:
            return kw2xrf_get_proto(dev, (uint8_t *)value, max_len);

        case NG_NETOPT_STATE:
            if (max_len < sizeof(ng_netopt_state_t)) {
                return -EOVERFLOW;
            }

            *(ng_netopt_state_t *)value = *(ng_netopt_state_t *) & (dev->state);
            return 0;

        case NG_NETOPT_TX_POWER:
            if (max_len < 1) {
                return -EOVERFLOW;
            }

            *(int16_t *)value = dev->tx_power;
            return 0;

        case NG_NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }

            *((uint16_t *)value) = KW2XRF_MAX_PKT_LENGTH;
            return sizeof(uint16_t);

        case NG_NETOPT_PRELOADING:
            *((ng_netopt_enable_t *)value) = !!(dev->option & KW2XRF_OPT_PRELOADING);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_AUTOACK:
            *((ng_netopt_enable_t *)value) = !!(dev->option & KW2XRF_OPT_AUTOACK);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_PROMISCUOUSMODE:
            *((ng_netopt_enable_t *)value) = !!(dev->option & KW2XRF_OPT_PROMISCUOUS);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RAWMODE:
            *((ng_netopt_enable_t *)value) = !!(dev->option & KW2XRF_OPT_RAWDUMP);
            return sizeof(ng_netopt_enable_t);

        default:
            return -ENOTSUP;
    }
}

void kw2xrf_set_option(kw2xrf_t *dev, uint16_t option, bool state)
{
    uint8_t reg;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    if (state) {
        dev->option |= option;

        /* trigger option specific actions */
        switch (option) {
            case KW2XRF_OPT_CSMA:
                DEBUG("[kw2xrf] opt: enabling CSMA mode\n");
                reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
                reg |= MKW2XDM_PHY_CTRL1_CCABFRTX;
                kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
                break;

            case KW2XRF_OPT_PROMISCUOUS:
                DEBUG("[kw2xrf] opt: enabling PROMISCUOUS mode\n");
                /* disable auto ACKs in promiscuous mode */
                reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
                reg &= ~(MKW2XDM_PHY_CTRL1_AUTOACK);
                kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
                /* enable promiscuous mode */
                reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
                reg |= MKW2XDM_PHY_CTRL4_PROMISCUOUS;
                kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, reg);
                break;

            case KW2XRF_OPT_AUTOACK:
                DEBUG("[kw2xrf] opt: enabling auto ACKs\n");
                reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
                reg |= MKW2XDM_PHY_CTRL1_AUTOACK;
                kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
                break;

            default:
                /* do nothing */
                break;
        }
    }
    else {
        dev->option &= ~(option);

        /* trigger option specific actions */
        switch (option) {
            case KW2XRF_OPT_CSMA:
                reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
                reg &= ~(MKW2XDM_PHY_CTRL1_CCABFRTX);
                kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
                break;

            case KW2XRF_OPT_PROMISCUOUS:
                /* disable promiscuous mode */
                reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
                reg &= ~(MKW2XDM_PHY_CTRL4_PROMISCUOUS);
                kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, reg);

                /* re-enable AUTOACK only if the option is set */
                if (dev->option & KW2XRF_OPT_AUTOACK) {
                    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
                    reg |= MKW2XDM_PHY_CTRL1_AUTOACK;
                    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
                }

                break;

            case KW2XRF_OPT_AUTOACK:
                reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
                reg &= ~(MKW2XDM_PHY_CTRL1_AUTOACK);
                kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
                break;

            default:
                /* do nothing */
                break;
        }
    }
}

int kw2xrf_set(ng_netdev_t *netdev, ng_netopt_t opt, void *value, size_t value_len)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NG_NETOPT_CHANNEL:
            return kw2xrf_set_channel(dev, (uint8_t *)value, value_len);

        case NG_NETOPT_ADDRESS:
            if (value_len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            return kw2xrf_set_addr(dev, *((uint16_t *)value));

        case NG_NETOPT_ADDRESS_LONG:
            if (value_len > sizeof(uint64_t)) {
                return -EOVERFLOW;
            }

            return kw2xrf_set_addr_long(dev, *((uint64_t *)value));

        case NG_NETOPT_SRC_LEN:
            if (value_len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (*((uint16_t *)value) == 2) {
                kw2xrf_set_option(dev, KW2XRF_OPT_SRC_ADDR_LONG,
                                  false);
            }
            else if (*((uint16_t *)value) == 8) {
                kw2xrf_set_option(dev, KW2XRF_OPT_SRC_ADDR_LONG,
                                  true);
            }
            else {
                return -ENOTSUP;
            }

            return sizeof(uint16_t);

        case NG_NETOPT_NID:
            if (value_len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            return kw2xrf_set_pan(dev, *((uint16_t *)value));

        case NG_NETOPT_TX_POWER:
            if (value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            kw2xrf_set_tx_power(dev, (int8_t *)value, value_len);
            return sizeof(uint16_t);

        case NG_NETOPT_PROTO:
            return kw2xrf_set_proto(dev, (uint8_t *)value, value_len);

        case NG_NETOPT_AUTOACK:
            /* Set up HW generated automatic ACK after Receive */
            kw2xrf_set_option(dev, KW2XRF_OPT_AUTOACK,
                              ((bool *)value)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_PROMISCUOUSMODE:
            kw2xrf_set_option(dev, KW2XRF_OPT_PROMISCUOUS,
                              ((bool *)value)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_RAWMODE:
            kw2xrf_set_option(dev, KW2XRF_OPT_RAWDUMP,
                              ((bool *)value)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_PRELOADING:
            kw2xrf_set_option(dev, KW2XRF_OPT_PRELOADING,
                              ((bool *)value)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_AUTOCCA:
            kw2xrf_set_option(dev, KW2XRF_OPT_CSMA,
                              ((bool *)value)[0]);
            return sizeof(ng_netopt_enable_t);

        case NG_NETOPT_STATE:
            if (*((ng_netopt_state_t *)value) == NG_NETOPT_STATE_TX) {
                DEBUG("kw2xrf: Sending now.\n");
                kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
                return sizeof(ng_netopt_state_t);
            }
            else if (*((ng_netopt_state_t *)value) == NG_NETOPT_STATE_SLEEP) {
                kw2xrf_set_sequence(dev, XCVSEQ_IDLE);
                return sizeof(ng_netopt_state_t);
            }
            else if (*((ng_netopt_state_t *)value) == NG_NETOPT_STATE_IDLE) {
                kw2xrf_set_sequence(dev, XCVSEQ_RECEIVE);
                return sizeof(ng_netopt_state_t);
            }

            /* TODO: Implement Off state here, when LPM functions are implemented */

        default:
            return -ENOTSUP;
    }
}

/* TODO: generalize and move to ng_ieee802154 */
/* TODO: include security header implications */
static size_t _get_frame_hdr_len(uint8_t *mhr)
{
    uint8_t tmp;
    size_t len = 3;

    /* figure out address sizes */
    tmp = (mhr[1] & NG_IEEE802154_FCF_DST_ADDR_MASK);

    if (tmp == NG_IEEE802154_FCF_DST_ADDR_SHORT) {
        len += 4;
    }
    else if (tmp == NG_IEEE802154_FCF_DST_ADDR_LONG) {
        len += 10;
    }
    else if (tmp != NG_IEEE802154_FCF_DST_ADDR_VOID) {
        return 0;
    }

    tmp = (mhr[1] & NG_IEEE802154_FCF_SRC_ADDR_MASK);

    if (tmp == NG_IEEE802154_FCF_SRC_ADDR_VOID) {
        return len;
    }
    else {
        if (!(mhr[0] & NG_IEEE802154_FCF_PAN_COMP)) {
            len += 2;
        }

        if (tmp == NG_IEEE802154_FCF_SRC_ADDR_SHORT) {
            return (len + 2);
        }
        else if (tmp == NG_IEEE802154_FCF_SRC_ADDR_LONG) {
            return (len + 8);
        }
    }

    return 0;
}

/* TODO: generalize and move to ng_ieee802154 */
static ng_pktsnip_t *_make_netif_hdr(uint8_t *mhr)
{
    uint8_t tmp;
    uint8_t *addr;
    uint8_t src_len, dst_len;
    ng_pktsnip_t *snip;
    ng_netif_hdr_t *hdr;

    /* figure out address sizes */
    tmp = mhr[1] & NG_IEEE802154_FCF_SRC_ADDR_MASK;

    if (tmp == NG_IEEE802154_FCF_SRC_ADDR_SHORT) {
        src_len = 2;
    }
    else if (tmp == NG_IEEE802154_FCF_SRC_ADDR_LONG) {
        src_len = 8;
    }
    else if (tmp == 0) {
        src_len = 0;
    }
    else {
        return NULL;
    }

    tmp = mhr[1] & NG_IEEE802154_FCF_DST_ADDR_MASK;

    if (tmp == NG_IEEE802154_FCF_DST_ADDR_SHORT) {
        dst_len = 2;
    }
    else if (tmp == NG_IEEE802154_FCF_DST_ADDR_LONG) {
        dst_len = 8;
    }
    else if (tmp == 0) {
        dst_len = 0;
    }
    else {
        return NULL;
    }

    /* allocate space for header */
    snip = ng_pktbuf_add(NULL, NULL, sizeof(ng_netif_hdr_t) + src_len + dst_len,
                         NG_NETTYPE_NETIF);

    if (snip == NULL) {
        return NULL;
    }

    /* fill header */
    hdr = (ng_netif_hdr_t *)snip->data;
    ng_netif_hdr_init(hdr, src_len, dst_len);

    if (dst_len > 0) {
        tmp = 5 + dst_len;
        addr = ng_netif_hdr_get_dst_addr(hdr);

        for (int i = 0; i < dst_len; i++) {
            addr[i] = mhr[5 + (dst_len - i) - 1];
        }
    }
    else {
        tmp = 3;
    }

    if (!(mhr[0] & NG_IEEE802154_FCF_PAN_COMP)) {
        tmp += 2;
    }

    if (src_len > 0) {
        addr = ng_netif_hdr_get_src_addr(hdr);

        for (int i = 0; i < src_len; i++) {
            addr[i] = mhr[tmp + (src_len - i) - 1];
        }
    }

    return snip;
}

void _receive_data(kw2xrf_t *dev)
{
    size_t pkt_len, hdr_len;
    ng_pktsnip_t *hdr, *payload = NULL;
    ng_netif_hdr_t *netif;

    /* get size of the received packet */
    pkt_len = kw2xrf_read_dreg(MKW2XDM_RX_FRM_LEN);

    /* read PSDU */
    kw2xrf_read_fifo(dev->buf, pkt_len + 1);

    /* abort here already if no event callback is registered */
    if (!dev->event_cb) {
        return;
    }

    /* If RAW-mode is selected direclty forward pkt, MAC does the rest */
    if (dev->option & KW2XRF_OPT_RAWDUMP) {
        payload = ng_pktbuf_add(NULL, NULL, pkt_len, NG_NETTYPE_UNDEF);

        if (payload == NULL) {
            DEBUG("kw2xf: error: unable to allocate RAW data\n");
            return;
        }

        payload->data = dev->buf;
        dev->event_cb(NETDEV_EVENT_RX_COMPLETE, payload);
        return;
    }

    /* get FCF field and compute 802.15.4 header length */
    hdr_len = _get_frame_hdr_len(dev->buf);

    if (hdr_len == 0) {
        DEBUG("kw2xrf error: unable parse incoming frame header\n");
        return;
    }

    /* read the rest of the header and parse the netif header from it */
    hdr = _make_netif_hdr(dev->buf);

    if (hdr == NULL) {
        DEBUG("kw2xrf error: unable to allocate netif header\n");
        return;
    }

    /* fill missing fields in netif header */
    netif = (ng_netif_hdr_t *)hdr->data;
    netif->if_pid = thread_getpid();
    netif->lqi = dev->buf[pkt_len];
    /* lqi and rssi are directly related to each other in the kw2x-device.
     * The rssi-unit is dBm and in this case alwaysnegative, nevertheless
     * a positive value is reported.
     */
    netif->rssi = -((netif->lqi) - 286.6) / 2.69333;

    payload = ng_pktbuf_add(hdr, (void *) & (dev->buf[hdr_len]),
                            pkt_len - hdr_len - 2, dev->proto);

    if (payload == NULL) {
        DEBUG("kw2xrf: ERROR allocating payload in packet buffer on RX\n");
        ng_pktbuf_release(hdr);
        return;
    }

    dev->event_cb(NETDEV_EVENT_RX_COMPLETE, payload);
}

void kw2xrf_isr_event(ng_netdev_t *netdev, uint32_t event_type)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    uint8_t irqst1 = kw2xrf_read_dreg(MKW2XDM_IRQSTS1);
    uint8_t irqst2 = kw2xrf_read_dreg(MKW2XDM_IRQSTS2);

    if ((irqst1 & MKW2XDM_IRQSTS1_RXIRQ) && (irqst1 & MKW2XDM_IRQSTS1_SEQIRQ)) {
        /* RX */
        DEBUG("kw2xrf: RX Int\n");
        _receive_data(dev);
        kw2xrf_set_sequence(dev, XCVSEQ_RECEIVE);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_RXIRQ | MKW2XDM_IRQSTS1_SEQIRQ);
    }
    else if ((irqst1 & MKW2XDM_IRQSTS1_TXIRQ) && (irqst1 & MKW2XDM_IRQSTS1_SEQIRQ)) {
        /* TX_Complete */
        /* Device is automatically in Radio-idle state when TX is done */
        kw2xrf_set_sequence(dev, XCVSEQ_RECEIVE);
        DEBUG("kw2xrf: TX Complete\n");
        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_TXIRQ | MKW2XDM_IRQSTS1_SEQIRQ);
    }
    else if ((irqst1 & MKW2XDM_IRQSTS1_CCAIRQ) && (irqst1 & MKW2XDM_IRQSTS1_SEQIRQ)) {
        /* TX_Started (CCA_done) */
        if (irqst2 & MKW2XDM_IRQSTS2_CCA) {
            DEBUG("kw2xrf: CCA done -> Channel busy\n");
        }
        else {
            DEBUG("kw2xrf: CCA done -> Channel idle\n");
        }

        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_CCAIRQ | MKW2XDM_IRQSTS1_SEQIRQ);
        kw2xrf_set_sequence(dev, XCVSEQ_RECEIVE);
    }
    else {
        /* Unknown event */
        /* Clear all interrupts to prevent ISR-loop */
        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, 0x7f);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS2, 0x03);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS3, 0xff);
        DEBUG("kw2xrf_isr_event: unknown Interrupt\n");
        kw2xrf_set_sequence(dev, XCVSEQ_RECEIVE);
        return;
    }
}

/* TODO: Move to ng_ieee802.15.4 as soon as ready */
int _assemble_tx_buf(kw2xrf_t *dev, ng_pktsnip_t *pkt)
{
    ng_netif_hdr_t *hdr;
    hdr = (ng_netif_hdr_t *)pkt->data;
    int index = 0;

    if (dev == NULL) {
        ng_pktbuf_release(pkt);
        return -ENODEV;
    }

    /* get netif header check address length */
    hdr = (ng_netif_hdr_t *)pkt->data;

    /* FCF, set up data frame, request for ack, panid_compression */
    /* TODO: Currently we don´t request for Ack in this device.
     * since this is a soft_mac device this has to be
     * handled in a upcoming CSMA-MAC layer.
     */
    /*if(dev->option & KW2XRF_OPT_AUTOACK) {
        dev->buf[1] = 0x61;
    }
    else {
        dev->buf[1] = 0x51;
    }*/
    dev->buf[1] = 0x51;

    /* set sequence number */
    dev->buf[3] = dev->seq_nr++;

    index = 4;

    /* set destination pan_id */
    dev->buf[index++] = (uint8_t)((dev->radio_pan) & 0xff);
    dev->buf[index++] = (uint8_t)((dev->radio_pan) >> 8);

    /* fill in destination address */
    if (hdr->flags &
        (NG_NETIF_HDR_FLAGS_BROADCAST | NG_NETIF_HDR_FLAGS_MULTICAST)) {
        dev->buf[2] = 0x88;
        dev->buf[index++] = 0xff;
        dev->buf[index++] = 0xff;
        /* set source address */
        dev->buf[index++] = (uint8_t)(dev->addr_short[0]);
        dev->buf[index++] = (uint8_t)(dev->addr_short[1]);
    }
    else if (hdr->dst_l2addr_len == 2) {
        /* set to short addressing mode */
        dev->buf[2] = 0x88;
        /* set destination address, byte order is inverted */
        dev->buf[index++] = (ng_netif_hdr_get_dst_addr(hdr))[1];
        dev->buf[index++] = (ng_netif_hdr_get_dst_addr(hdr))[0];
        /* set source pan_id */
        //dev->buf[index++] = (uint8_t)((dev->radio_pan) >> 8);
        //dev->buf[index++] = (uint8_t)((dev->radio_pan) & 0xff);
        /* set source address */
        dev->buf[index++] = (uint8_t)(dev->addr_short[0]);
        dev->buf[index++] = (uint8_t)(dev->addr_short[1]);
    }
    else if (hdr->dst_l2addr_len == 8) {
        /* default to use long address mode for src and dst */
        dev->buf[2] |= 0xcc;
        /* set destination address located directly after ng_ifhrd_t in memory */
        memcpy(&(dev->buf)[index], ng_netif_hdr_get_dst_addr(hdr), 8);
        index += 8;
        /* set source pan_id, wireshark expects it there */
        //dev->buf[index++] = (uint8_t)((dev->radio_pan) >> 8);
        //dev->buf[index++] = (uint8_t)((dev->radio_pan) & 0xff);

        /* set source address */
        memcpy(&(dev->buf[index]), dev->addr_long, 8);
        index += 8;
    }
    else {
        ng_pktbuf_release(pkt);
        return -ENOMSG;
    }

    return index;
}

int kw2xrf_send(ng_netdev_t *netdev, ng_pktsnip_t *pkt)
{
    int index = 0;
    kw2xrf_t *dev = (kw2xrf_t *) netdev;

    if (pkt == NULL) {
        return -ENOMSG;
    }

    ng_pktsnip_t *payload = pkt->next;

    if (netdev == NULL) {
        ng_pktbuf_release(pkt);
        return -ENODEV;
    }

    if (pkt->type == NG_NETTYPE_NETIF) {
        /* Build header and fills this already into the tx-buf */
        index = _assemble_tx_buf(dev, pkt);
        DEBUG("Assembled header for NG_NETTYPE_UNDEF to tx-buf, index: %i\n", index);
    }
    else if (pkt->type == NG_NETTYPE_UNDEF) {
        /* IEEE packet is already included in the header,
         * no need to build the header manually */
        DEBUG("Incoming packet of type NG_NETTYPE_802154: %i\n", index);
        DEBUG("size of pktsnip: %i\n", pkt->size);

        for (int i = 0; i < pkt->size; i++) {
            uint8_t *tmp = pkt->data;
            dev->buf[index + i + 1] = tmp[i];
        }

        /* count bytes */
        index += pkt->size;
    }
    else {
        DEBUG("Driver does not support this type of packet\n");
        return -ENOTSUP;
    }

    while (payload) {
        /* check we don't exceed FIFO size */
        if (index + 2 + payload->size > KW2XRF_MAX_PKT_LENGTH) {
            ng_pktbuf_release(pkt);
            DEBUG("Packet exceeded FIFO size.\n");
            return -ENOBUFS;
        }

        for (int i = 0; i < payload->size; i++) {
            uint8_t *tmp = payload->data;
            dev->buf[index + i] = tmp[i];
        }

        /* count bytes */
        index += payload->size;

        /* next snip */
        payload = payload->next;
    }

    dev->buf[0] = index + 1; /* set packet size */

    ng_pktbuf_release(pkt);
    DEBUG("kw2xrf: packet with size %i loaded to tx_buf\n", dev->buf[0]);
    kw2xrf_write_fifo(dev->buf, dev->buf[0]);

    if ((dev->option & KW2XRF_OPT_PRELOADING) == NG_NETOPT_DISABLE) {
        DEBUG("kw2xrf: Sending now.\n");
        kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
    }

    return index;
}

/* implementation of the netdev interface */
const ng_netdev_driver_t kw2xrf_driver = {
    .send_data = kw2xrf_send,
    .add_event_callback = kw2xrf_add_cb,
    .rem_event_callback = kw2xrf_rem_cb,
    .get = kw2xrf_get,
    .set = kw2xrf_set,
    .isr_event = kw2xrf_isr_event,
};
