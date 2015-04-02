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
#include "mutex.h"
#include "msg.h"
#include "periph/gpio.h"
#include "net/ng_netbase.h"

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

int _set_tx_power(kw2xrf_t *dev, uint8_t *val, size_t len)
{

    if (len < 1) {
        return -EOVERFLOW;
    }

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

int _get_channel(kw2xrf_t *dev, uint8_t *val, size_t max)
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

int _get_sequence(void)
{
   int reg = 0;
   reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
   reg &= MKW2XDM_PHY_CTRL1_XCVSEQ_MASK;
   return reg;
}

void _set_sequence(kw2xrf_physeq_t seq)
{
    /* Mask all interrupts; disable interrupts for exclusive SPI access */
    int tmp_irq_state = disableIRQ();
    uint8_t reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
    reg |= MKW2XDM_PHY_CTRL4_TRCV_MSK;
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, reg);
    restoreIRQ(tmp_irq_state);

    if (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE)) {
        /* abort any ongoing sequence */
        DEBUG("tx: abort SEQ_STATE: %x\n", kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
        while (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));

        /* Clear all pending interrupts */
        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, 0x7f);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS2, 0x03);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS3, 0xff);

        /* Mask all possible interrupts */
        reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL2);
        reg |= ~(MKW2XDM_PHY_CTRL2_CRC_MSK);
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL2, reg);

        reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL3);
        reg |= (MKW2XDM_PHY_CTRL3_PB_ERR_MSK | MKW2XDM_PHY_CTRL3_WAKE_MSK);
        reg &= (MKW2XDM_PHY_CTRL3_PB_ERR_MSK | MKW2XDM_PHY_CTRL3_WAKE_MSK);
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL2, reg);
    }
    /* For all sequences only enable SEQ-irq, that is set when sequence was completed */
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL2);
    reg &= ~(MKW2XDM_PHY_CTRL2_SEQMSK);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL2, reg);

    /* Progrmm new sequence */
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
    reg &= ~(MKW2XDM_PHY_CTRL1_XCVSEQ_MASK);
    reg |= MKW2XDM_PHY_CTRL1_XCVSEQ(seq);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
    DEBUG("kw2xrf: Set sequence to %i\n", seq);

    /* Unmask all interrupts */
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
    reg &= ~(MKW2XDM_PHY_CTRL4_TRCV_MSK);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, reg);
}

int _set_channel(kw2xrf_t *dev, uint8_t *val, size_t len)
{
    /* Save old sequence to restore this state later */
    uint8_t old_seq = _get_sequence();
    if(old_seq){
    _set_sequence(XCVSEQ_IDLE);
    }
    if (val[0] < 11 || val[0] > 26) {
        DEBUG("Invalid channel %i set. Valid channels are 11 through 26\n", val[0]);
        return -EINVAL;
    }

    if (len != 2 || val[1] != 0) {
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

    if(old_seq){
        _set_sequence(old_seq);
    }

    return 2;
}

void kw2xrf_irq_handler(void *args)
{
    msg_t msg;
    kw2xrf_t *dev = (kw2xrf_t *)args;
    uint8_t irqst1 = kw2xrf_read_dreg(MKW2XDM_IRQSTS1);
    DEBUG("kw2xrf_irq_handler: irqst1: %x\n",irqst1);
    if ((irqst1 & MKW2XDM_IRQSTS1_RXIRQ) && (irqst1 & MKW2XDM_IRQSTS1_SEQIRQ)) {
        msg.content.value = NETDEV_EVENT_RX_COMPLETE;
        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_RXIRQ | MKW2XDM_IRQSTS1_SEQIRQ);
    }
    else if ((irqst1 & MKW2XDM_IRQSTS1_TXIRQ) && (irqst1 & MKW2XDM_IRQSTS1_SEQIRQ)) {
        msg.content.value = NETDEV_EVENT_TX_COMPLETE;
        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_TXIRQ | MKW2XDM_IRQSTS1_SEQIRQ);
    }
    else if ((irqst1 & MKW2XDM_IRQSTS1_CCAIRQ) && (irqst1 & MKW2XDM_IRQSTS1_SEQIRQ)) {
        msg.content.value = NETDEV_EVENT_TX_STARTED;
        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_CCAIRQ | MKW2XDM_IRQSTS1_SEQIRQ);
    }
    else {
        msg.content.value = ISR_EVENT_UNKNOWN;
        /* Clear all interrupts to prevent ISR-loop */
        kw2xrf_write_dreg(MKW2XDM_IRQSTS1, 0x7f);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS2, 0x03);
        kw2xrf_write_dreg(MKW2XDM_IRQSTS3, 0xff);
    }

    /* packet is complete */
    msg.type = NG_NETDEV_MSG_TYPE_EVENT;
    msg_send_int(&msg, dev->mac_pid);
}

/* Set up interrupt sources, triggered by the radio-module */
void kw2xrf_init_interrupts(kw2xrf_t *dev)
{
    /* Disable all interrups:
     * Selectively enable only one interrupt source selectively in sequence manager.
     * After reset state all interrupts are disabled, except WAKE_IRQ.
     */
    kw2xrf_write_dreg(MKW2XDM_IRQSTS1, 0x7f);
    kw2xrf_write_dreg(MKW2XDM_IRQSTS2, 0x03);
    kw2xrf_write_dreg(MKW2XDM_IRQSTS3, 0xff);

    /* Mask all possible interrupts */
    int reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL2);
    reg |= ~(MKW2XDM_PHY_CTRL2_CRC_MSK);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL2, reg);

    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL3);
    reg |= (MKW2XDM_PHY_CTRL3_PB_ERR_MSK | MKW2XDM_PHY_CTRL3_WAKE_MSK);
    reg &= (MKW2XDM_PHY_CTRL3_PB_ERR_MSK | MKW2XDM_PHY_CTRL3_WAKE_MSK);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL2, reg);

    /* set up GPIO-pin used for IRQ */
    gpio_init_int(GPIO_KW2XDRF, GPIO_NOPULL, GPIO_FALLING, &kw2xrf_irq_handler, dev);
}

int _set_pan(kw2xrf_t *dev, uint8_t *val, size_t len)
{
    if (len != 2) {
        return -EINVAL;
    }
    kw2xrf_write_iregs(MKW2XDMI_MACPANID0_LSB, val, 2);
    dev->radio_pan = val[1];
    dev->radio_pan = (dev->radio_pan) <<8;
    dev->radio_pan |= val[0];
    return 2;
}

int _get_pan(kw2xrf_t *dev, uint8_t *val, size_t max)
{
    if (max < 2) {
        return -EOVERFLOW;
    }
    kw2xrf_read_iregs(MKW2XDMI_MACPANID0_LSB, val, 2);
    return 2;
}

int _get_proto(kw2xrf_t *dev, uint8_t *val, size_t max)
{
    if (max < sizeof(ng_nettype_t)) {
        return -EOVERFLOW;
    }
    memcpy(val, &(dev->proto), sizeof(ng_nettype_t));
    return sizeof(ng_nettype_t);
}

int _set_proto(kw2xrf_t *dev, uint8_t *val, size_t len)
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
    _set_sequence(XCVSEQ_IDLE);

    dev->state = NETCONF_STATE_SLEEP;
    return 0;
}

int _get_addr_short(kw2xrf_t *dev, uint8_t *val, size_t len)
{
    if (len < 2) {
        return -EOVERFLOW;
    }
    kw2xrf_read_iregs(MKW2XDMI_MACSHORTADDRS0_LSB, val, 2);
    return 2;
}

int _get_addr_long(kw2xrf_t *dev, uint8_t *val, size_t len)
{
    if (len < 8) {
        return -EOVERFLOW;
    }
    kw2xrf_read_iregs(MKW2XDMI_MACLONGADDRS0_0, val, 8);
    return 8;
}

int _set_addr(kw2xrf_t *dev, uint8_t *val, size_t len)
{
    if (len == 2) {
        kw2xrf_write_iregs(MKW2XDMI_MACSHORTADDRS0_LSB, val, 2);
        dev->addr_short[0] = val[0];
        dev->addr_short[1] = val[1];
        return 2;
    }
    else if (len == 8){
        kw2xrf_write_iregs(MKW2XDMI_MACLONGADDRS0_0, val, 8);
        for (int i=0; i<8; i++) {
            dev->addr_long[i] = val[i];
        }
        return 8;
    }
    return -ENOTSUP;
}

int kw2xrf_init(kw2xrf_t *dev) {
    uint8_t reg = 0;
    uint8_t tmp[2];
    uint8_t tx_pwr = 0;

    /* check device parameters */
    if (dev == NULL) {
        return -ENODEV;
    }

    kw2xrf_spi_init();
    if (!(kw2xrf_on(dev) == 0)) {
        core_panic(0x42, "Could not start MKW2XD radio transceiver");
    }

    /* Gerneral initialization of interrupt sources.
     * sets radio to idle modewith all interrupt masked
     */
    kw2xrf_init_interrupts(dev);

    /* set device driver */
    dev->driver = &kw2xrf_driver;
    /* set default options */
    dev->proto = KW2XRF_DEFAULT_PROTOCOL;
    dev->options = 0;
    dev->addr_short[0] = (uint8_t)(KW2XRF_DEFAULT_SHORT_ADDR >> 8);
    dev->addr_short[1] = (uint8_t)(KW2XRF_DEFAULT_SHORT_ADDR);
    /* set default short address */
    _set_addr(dev, dev->addr_short, 2);
    /* load long address */
    _get_addr_long(dev, dev->addr_long, 8);

    _set_tx_power(dev, &tx_pwr, 1);

    /* set default channel */
    tmp[0] = KW2XRF_DEFAULT_CHANNEL;
    tmp[1] = 0;
    _set_channel(dev, tmp, 2);
    /* set default PAN ID */
    tmp[0] = (uint8_t)(KW2XRF_DEFAULT_PANID & 0xff);
    tmp[1] = (uint8_t)(KW2XRF_DEFAULT_PANID >> 8);
    _set_pan(dev, tmp, 2);

    /* CCA Setup */
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
    reg |= MKW2XDM_PHY_CTRL4_CCATYPE(1); /* Set up CCA mode 1 (RSSI threshold) */
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, reg);
    DEBUG("kw2xrf: Initialized and set to channel %i and pan %i.\n",
    KW2XRF_DEFAULT_CHANNEL, KW2XRF_DEFAULT_PANID);

    /* TODO: For testing, ignores CRCVALID in receive mode */
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL2);
    reg &= ~(MKW2XDM_PHY_CTRL2_CRC_MSK);
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL2, reg);

    /* TODO: For testing, set up promiscous mode */
    reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL4);
    reg |= MKW2XDM_PHY_CTRL4_PROMISCUOUS;
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL4, reg);
    return 0;
}

int _add_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
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

int _rem_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
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

int _get(ng_netdev_t *netdev, ng_netconf_opt_t opt, void *value, size_t max_len)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETCONF_OPT_ADDRESS:
            return _get_addr_short(dev, (uint8_t *)value, max_len);
        case NETCONF_OPT_ADDRESS_LONG:
            return _get_addr_long(dev, (uint8_t *)value, max_len);
        case NETCONF_OPT_CHANNEL:
            return _get_channel(dev, (uint8_t *)value, max_len);
        case NETCONF_OPT_NID:
            return _get_pan(dev, (uint8_t *)value, max_len);
        case NETCONF_OPT_PROTO:
            return _get_proto(dev, (uint8_t *)value, max_len);
        case NETCONF_OPT_STATE:
            if (max_len < sizeof(ng_netconf_state_t)) {
                return -EOVERFLOW;
            }
            value = (int *)&(dev->state);
            return 0;
        default:
            return -ENOTSUP;
    }
}

int _set(ng_netdev_t *netdev, ng_netconf_opt_t opt, void *value, size_t value_len)
{
    uint8_t reg;
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETCONF_OPT_CHANNEL:
            return _set_channel(dev, (uint8_t *)value, value_len);
        case NETCONF_OPT_ADDRESS:
            return _set_addr(dev, (uint8_t *)value, value_len);
        case NETCONF_OPT_NID:
            return _set_pan(dev, (uint8_t *)value, value_len);
        case NETCONF_OPT_IS_CHANNEL_CLR:
            _set_sequence(XCVSEQ_CCA);
            return 0;
        case NETCONF_OPT_TX_POWER:
            _set_tx_power(dev, (uint8_t *)value, value_len);
            return 0;
        case NETCONF_OPT_PROTO:
            return _set_proto(dev, (uint8_t *)value, value_len);
        case NETCONF_OPT_CCA_BEFORE_TX:
            reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
            reg |= MKW2XDM_PHY_CTRL1_CCABFRTX;          /* Set up CCA before TX */
            kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
            return 0;
        case NETCONF_OPT_AUTOACK:
            reg = kw2xrf_read_dreg(MKW2XDM_PHY_CTRL1);
            /* Set up HW generated automatic ACK after Receive */
            reg |= MKW2XDM_PHY_CTRL1_AUTOACK;
            kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, reg);
            return 0;
        case NETCONF_OPT_STATE:
                if(*((ng_netconf_state_t *)value) == NETCONF_STATE_TX) {
                    DEBUG("kw2xrf: Sending now.\n");
                    _set_sequence(XCVSEQ_TRANSMIT);
                    dev->state = NETCONF_STATE_TX;
                    return 0;
                }
                else if(*((ng_netconf_state_t *)value) == NETCONF_STATE_SLEEP) {
                    _set_sequence(XCVSEQ_IDLE);
                    dev->state = NETCONF_STATE_SLEEP;
                    return 0;
                }
                else if(*((ng_netconf_state_t *)value) == NETCONF_STATE_IDLE) {
                    _set_sequence(XCVSEQ_RECEIVE);
                    dev->state = NETCONF_STATE_IDLE;
                    return 0;
                }
                /* TODO: Implement Off state here, when LPM functions are implemented */
        default:
            return -ENOTSUP;
    }
}

void _isr_event(ng_netdev_t *netdev, uint32_t event_type)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    uint8_t pkt_len = 0;
    ng_netif_hdr_t *hdr;
    uint8_t index = 0;
    uint8_t src_addr_len = 0;
    uint8_t dst_addr_len = 0;

    /* check device */
    if (dev == NULL) {
        return;
    }

    if (event_type == NETDEV_EVENT_RX_COMPLETE) {
        pkt_len = kw2xrf_read_dreg(MKW2XDM_RX_FRM_LEN);

        /* read PSDU */
        kw2xrf_read_fifo(dev->buf, pkt_len + 1);
        if((dev->buf[0] & 0x07) == 0x00) {
            DEBUG("kw2xrf: Beacon received; not handled yed -> dischard message\n");
            _set_sequence(XCVSEQ_RECEIVE);
            return;
        }
        if((dev->buf[0] & 0x07) == 0x02) {
            DEBUG("kw2xrf: ACK received; not handled yed -> dischard message\n");
            _set_sequence(XCVSEQ_RECEIVE);
            return;
        }
        if((dev->buf[0] & 0x07) == 0x03) {
            DEBUG("kw2xrf: MAC-cmd received; not handled yed -> dischard message\n");
            _set_sequence(XCVSEQ_RECEIVE);
            return;
        }
        if((dev->buf[0] & 0x07) != 0x01) {   /* No Data frame either */
            DEBUG("kw2xrf: undefined message received; -> dischard message\n");
            _set_sequence(XCVSEQ_RECEIVE);
            return;
        }
        DEBUG("kw2xrf: Message received: size %i\n", pkt_len);

        /* src 16bit addr */
        if((dev->buf[1] & 0x0c) == 0x08) {
            src_addr_len = 0x02;
        }
        /* src 64bit addr */
        else if((dev->buf[1] & 0x0c) == 0x0c) {
            src_addr_len = 0x08;
        }
        else {
            DEBUG("Bogus src address length.\n");
        }
        /* dst 16bit addr */
        if((dev->buf[1] & 0xc0) == 0x80) {
            dst_addr_len = 0x02;
        }
        /* dst 64bit addr */
        else if((dev->buf[1] & 0xc0) == 0xc0) {
            dst_addr_len = 0x08;
        }
        else {
            DEBUG("Bogus src address length.\n");
        }
        DEBUG("Src addr len: %i, Dst addr len: %i", src_addr_len, dst_addr_len);
        /* allocate a pktsnip for generic header */
        ng_pktsnip_t *hdr_snip = ng_pktbuf_add(NULL, NULL, sizeof(ng_netif_hdr_t)+
                                 src_addr_len, NG_NETTYPE_UNDEF);
        if (hdr_snip == NULL) {
            DEBUG("kw2xrf: ERROR allocating header in packet buffer on RX\n");
            ng_pktbuf_release(hdr_snip);
            _set_sequence(XCVSEQ_RECEIVE);
            return;
        }
        hdr = (ng_netif_hdr_t *)hdr_snip->data;
        /* init generic header */
        ng_netif_hdr_init(hdr, src_addr_len, dst_addr_len);
        /* append src address into memory */
        if(hdr->dst_l2addr_len == 2) {
            ng_netif_hdr_set_src_addr(hdr, &(dev->buf[7]), hdr->src_l2addr_len);
            index = 7 + hdr->src_l2addr_len;
        }
        else {
            ng_netif_hdr_set_src_addr(hdr, &(dev->buf[11]), hdr->src_l2addr_len);
            index = 11 + hdr->src_l2addr_len;
        }
        hdr->if_pid = thread_getpid();
        hdr->rssi = kw2xrf_read_dreg(MKW2XDM_RSSI_CCA_CNT);
        hdr->lqi = dev->buf[pkt_len];

        ng_pktsnip_t *payload_snip = ng_pktbuf_add(hdr_snip, (void *)&(dev->buf[index + 1]),
                                     pkt_len - index, dev->proto);
        if(payload_snip == NULL) {
            DEBUG("kw2xrf: ERROR allocating payload in packet buffer on RX\n");
            ng_pktbuf_release(hdr_snip);
            _set_sequence(XCVSEQ_RECEIVE);
            return;
        }
        dev->event_cb(NETDEV_EVENT_RX_COMPLETE, payload_snip);
        _set_sequence(XCVSEQ_RECEIVE);
        return;
    }

    if (event_type == NETDEV_EVENT_TX_STARTED) {
        uint8_t irqst2 = kw2xrf_read_dreg(MKW2XDM_IRQSTS2);
        if (irqst2 & MKW2XDM_IRQSTS2_CCA) {
            DEBUG("kw2xrf: CCA done -> Channel busy\n");
        }
        DEBUG("kw2xrf: CCA done -> Channel idle\n");
    }

    if (event_type == NETDEV_EVENT_TX_COMPLETE) {
        /* Device is automatically in Radio-idle state when TX is done */
        dev->state = NETCONF_STATE_SLEEP;
        DEBUG("kw2xrf: TX Complete\n");
    }
}

int _send(ng_netdev_t *netdev, ng_pktsnip_t *pkt)
{
    uint8_t index=0;
    kw2xrf_t *dev = (kw2xrf_t*) netdev;
    ng_netif_hdr_t *hdr;
    hdr = (ng_netif_hdr_t *)pkt->data;

    if (pkt == NULL) {
        return -ENOMSG;
    }
    if (netdev == NULL) {
        ng_pktbuf_release(pkt);
        return -ENODEV;
    }

    /* get netif header check address length */
    hdr = (ng_netif_hdr_t *)pkt->data;
    if (!(hdr->dst_l2addr_len == 2 || hdr->dst_l2addr_len == 8)) {
        ng_pktbuf_release(pkt);
        return -ENOMSG;
    }

    /* FCF, set up data frame */
    dev->buf[1] = 0x01;
    /* set sequence number */
    dev->buf[3] = dev->seq_nr++;


    if (hdr->dst_l2addr_len == 2) {
        index = 4;
        /* set to short addressing mode */
        dev->buf[2] = 0x88;
        /* set destination pan_id TODO: not currect currently */
        dev->buf[index++] = (uint8_t)dev->radio_pan;
        dev->buf[index++] = (uint8_t)((dev->radio_pan)>>8);
        index += 2;
        /* set source pan_id */
        dev->buf[index++] = (uint8_t)dev->radio_pan;
        dev->buf[index++] = (uint8_t)((dev->radio_pan)>>8);
        /* set source address */
        dev->buf[index++] = dev->addr_short[1];
        dev->buf[index++] = dev->addr_short[0];
    }
    else if (hdr->dst_l2addr_len == 8) {
        /* default to use long address mode for src and dst */
        dev->buf[2] = 0xcc;
        /* set destination address located directly after ng_ifhrd_t in memory */
        index = 4;
        index += 8;
        /* set source address */
        for(int i=0; i<8; i++) {
            dev->buf[index++] = dev->addr_long[i];
        }
    }

    while (pkt) {
        /* check we don't exceed FIFO size */
        if (index+2+pkt->size > KW2XRF_MAX_PKT_LENGTH) {
            ng_pktbuf_release(pkt);
            DEBUG("Packet exceeded FIFO size.\n");
            return -ENOBUFS;
        }
        for (int i=0; i < pkt->size; i++) {
            uint8_t *tmp = pkt->data;
            dev->buf[index+i+1] = tmp[i];
        }
        /* count bytes */
        index += pkt->size;

        /* next snip */
        pkt = pkt->next;
    }
    dev->buf[0] = index+2; /* set packet size, reserve additional */

    DEBUG("kw2xrf: send packet with size %i\n", dev->buf[0]);
    kw2xrf_write_fifo(dev->buf, dev->buf[0]);

    if ((dev->options&(1<<NETCONF_OPT_PRELOADING)) == NETCONF_DISABLE) {
        DEBUG("kw2xrf: Sending now.\n");
        _set_sequence(XCVSEQ_TRANSMIT);
    }
    dev->state = NETCONF_STATE_TX;

    return index;
}

/* implementation of the netdev interface */
const ng_netdev_driver_t kw2xrf_driver = {
    .send_data = _send,
    .add_event_callback = _add_cb,
    .rem_event_callback = _rem_cb,
    .get = _get,
    .set = _set,
    .isr_event = _isr_event,
};
