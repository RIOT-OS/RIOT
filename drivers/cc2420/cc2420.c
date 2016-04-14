/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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

#include "cc2420.h"
#include "cc2420_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Some constant hardware configuration values
 */
#define SPI_MODE            (SPI_CONF_FIRST_RISING)

extern const netdev2_driver_t cc2420_driver;

/**
 * @brief   Translation from dBm to PA level
 *
 * Entry 0 in the array corresponds to -25dBm (min), entry 25 to 0dBm (max), so
 * `PA_level = power_dbm_to_pa[DBM + 25]`. We use only the 3 MSB of the 5-bit
 * level, leading to 8 distinct power settings (the 8 settings listed in the
 * datasheet in section 28, page 51).
 */
static const uint8_t power_dbm_to_pa[26] = {
     3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  7,  7,  7,
     7,  7, 11, 11, 11, 15, 15, 19, 19, 23, 23, 27, 31
};

/**
 * @brief   Translate PA level to dBm
 *
 * As we use only the 3 MSB of the PA level value, we have 8 distinct settings.
 * We get the dBm value with `DBM = power_pa_to_dbm(PA >> 2).
 */
static const int8_t power_pa_to_dbm[8] = {
    -25, -15, -10, -7, -5, -3, -1, 0
};


static inline uint16_t to_u16(void *buf)
{
    return *((uint16_t *)buf);
}

static inline int16_t to_i16(void *buf)
{
    return *((int16_t *)buf);
}

static inline bool to_bool(void *buf)
{
    return *((bool *)buf);
}

static inline int w_u16(void *buf, uint16_t val)
{
    memcpy(buf, &val, sizeof(uint16_t));
    return sizeof(uint16_t);
}

static inline int w_i16(void *buf, int16_t val)
{
    memcpy(buf, &val, sizeof(int16_t));
    return sizeof(int16_t);
}

static inline int opt_state(void *buf, bool cond)
{
    *((netopt_enable_t *)buf) = !!(cond);
    return sizeof(netopt_enable_t);
}


static uint8_t strobe(const cc2420_t *dev, const uint8_t command)
{
    char res;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_byte(dev->params.spi, (char)command, (char *)&res);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);

    return res;
}

static void reg_write(const cc2420_t *dev,
                      const uint8_t addr,
                      const uint16_t value)
{
    uint16_t tmp = byteorder_htons(value).u16;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_regs(dev->params.spi, CC2420_REG_WRITE | addr,
                      (char *)&tmp, NULL, 2);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

static uint16_t reg_read(const cc2420_t *dev, const uint8_t addr)
{
    network_uint16_t tmp;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_regs(dev->params.spi, CC2420_REG_READ | addr,
                      NULL, (char *)&tmp, 2);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);

    return byteorder_ntohs(tmp);
}

static void ram_read(const cc2420_t *dev, const uint16_t addr,
                     uint8_t *data, const size_t len)
{
    char tmp[] = { (CC2420_RAM      | (addr & 0x7f)),
                   (CC2420_RAM_READ | ((addr >> 1) & 0xc0)) };

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_bytes(dev->params.spi, tmp, NULL, 2);
    spi_transfer_bytes(dev->params.spi, NULL, (char*)data, len);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

static void ram_write(const cc2420_t *dev, const uint16_t addr,
                      const uint8_t *data, const size_t len)
{
    char tmp[] = { (CC2420_RAM       | (addr & 0x7f)),
                   (CC2420_RAM_WRITE | ((addr >> 1) & 0xc0)) };

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_bytes(dev->params.spi, tmp, NULL, 2);
    spi_transfer_bytes(dev->params.spi, (char*)data, NULL, len);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

static void fifo_read(const cc2420_t *dev, uint8_t *data, const size_t len)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_regs(dev->params.spi, CC2420_FIFO_READ,
                      NULL, (char *)data, len);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

static void fifo_write(const cc2420_t *dev, uint8_t *data, const size_t len)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_regs(dev->params.spi, CC2420_FIFO_WRITE,
                      (char *)data, NULL, len);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

/**
 * @brief   Get the device's status byte
 */
static inline uint8_t status(cc2420_t *dev)
{
    return strobe(dev, CC2420_STROBE_NOP);
}

/**
 * @brief   Get the device's current state
 */
static inline uint8_t state(cc2420_t *dev)
{
    return (uint8_t)reg_read(dev, CC2420_REG_FSMSTATE);
}

static inline void en_xosc(cc2420_t *dev)
{
    strobe(dev, CC2420_STROBE_XOSCON);
    xtimer_usleep(CC2420_XOSCON_DELAY);
}

static void fifop_evt(void *arg)
{
    netdev2_t *dev = (netdev2_t *)arg;
    dev->event_callback(dev, NETDEV2_EVENT_ISR, NULL);
}

/**
 * @todo    Move this function to a global module
 */
#if CPUID_ID_LEN
static void addr_from_cpuid(uint8_t *addr)
{
    /* option 1: generate addresses from CPUID */
    uint8_t cpuid[CPUID_ID_LEN];

    cpuid_get(cpuid);
    memcpy(addr, cpuid, 8);

#if CPUID_ID_LEN < 8
    /* in case CPUID_ID_LEN < 8, fill missing bytes with zeros */
    for (int i = CPUID_ID_LEN; i < 8; i++) {
        addr_long[i] = 0;
    }
#else
    /* in case CPUID_ID_LEN > 8, XOR those bytes on top of the first 8 */
    for (int i = 8; i < CPUID_ID_LEN; i++) {
        addr_long[i & 0x07] ^= cpuid[i];
    }
#endif

    /* make sure we mark the address as non-multicast and not globally unique */
    addr_long[0] &= ~(0x01);
    addr_long[0] |= 0x02;
}
#endif


void cc2420_setup(cc2420_t * dev, const cc2420_params_t *params)
{
    /* set pointer to the devices netdev functions */
    dev->netdev.netdev.driver = &cc2420_driver;
    /* TODO: remove once the netdev2/802154 shit is cleaned up */
    dev->netdev.seq = 0;
    dev->netdev.flags = NETDEV2_IEEE802154_PAN_COMP;
    /* pull in device configuration parameters */
    memcpy(&dev->params, params, sizeof(cc2420_params_t));
    /* reset device descriptor fields */
    dev->options = 0;
}

int cc2420_init(cc2420_t *dev)
{
    uint16_t reg;
    uint8_t addr[8] = CC2420_ADDR_FALLBACK;

    /* initialize power and reset pins -> put the device into reset state */
    gpio_init(dev->params.pin_reset, GPIO_OUT);
    gpio_set(dev->params.pin_reset);
    gpio_init(dev->params.pin_vrefen, GPIO_OUT);
    gpio_clear(dev->params.pin_vrefen);

    /* initialize the input lines */
    gpio_init(dev->params.pin_cca, GPIO_IN);
    gpio_init(dev->params.pin_sfd, GPIO_IN);
    gpio_init(dev->params.pin_fifo, GPIO_IN);
    gpio_init_int(dev->params.pin_fifop, GPIO_IN, GPIO_RISING, fifop_evt, dev);

    /* initialize the chip select line and the SPI bus */
    gpio_init(dev->params.pin_cs, GPIO_OUT);
    gpio_set(dev->params.pin_cs);
    spi_init_master(dev->params.spi, SPI_MODE, dev->params.spi_clk);

    /* power on and toggle reset */
    gpio_set(dev->params.pin_vrefen);
    gpio_clear(dev->params.pin_reset);
    xtimer_usleep(CC2420_RESET_DELAY);
    gpio_set(dev->params.pin_reset);

    /* test the connection to the device by reading MANFIDL register */
    reg = reg_read(dev, CC2420_REG_MANFIDL);
    if (reg != CC2420_MANFIDL_VAL) {
        DEBUG("[cc2420] init: unable to communicate with device\n");
        return -1;
    }

    /* turn on the oscillator and wait for it to be stable */
    en_xosc(dev);
    if (!(status(dev) & CC2420_STATUS_XOSC_STABLE)) {
        DEBUG("[cc2420] init: oscillator did not stabilize\n");
        return -1;
    }

    /* set default address, channel, PAN ID, and TX power */
#if CPUID_ID_LEN
    addr_from_cpuid(addr);
#endif
    cc2420_set_addr_short(dev, &addr[6]);
    cc2420_set_addr_long(dev, addr);
    cc2420_set_pan(dev, CC2420_PANID_DEFAULT);
    cc2420_set_chan(dev, CC2420_CHAN_DEFAULT);
    cc2420_set_txpower(dev, CC2420_TXPOWER_DEFAULT);


    /* set default options */
    cc2420_set_option(dev, CC2420_OPT_AUTOACK, true);
    cc2420_set_option(dev, CC2420_OPT_CSMA, true);
    cc2420_set_option(dev, CC2420_OPT_TELL_TX_START, true);
    cc2420_set_option(dev, CC2420_OPT_TELL_RX_END, true);

    /* change default RX bandpass filter to 1.3uA (as recommended) */
    reg = reg_read(dev, CC2420_REG_RXCTRL1);
    reg |= CC2420_RXCTRL1_RXBPF_LOCUR;
    reg_write(dev, CC2420_REG_RXCTRL1, reg);

    /* set the FIFOP threshold to maximum. */
    reg_write(dev, CC2420_REG_IOCFG0, CC2420_PKT_MAXLEN);

    /* turn off "Security enable" (page 33). */
    reg = reg_read(dev, CC2420_REG_SECCTRL0);
    reg &= ~CC2420_SECCTRL0_RXFIFO_PROT;
    reg_write(dev, CC2420_REG_SECCTRL0, reg);

    /* go into RX state */
    cc2420_set_state(dev, CC2420_GOTO_RX);

    /* set preamble length to 3 leading zero byte */
    reg = reg_read(dev, CC2420_REG_MDMCTRL0);
    reg &= ~(CC2420_MDMCTRL0_PREAMBLE_M);
    reg |= CC2420_MDMCTRL0_PREAMBLE_3B;
    reg_write(dev, CC2420_REG_MDMCTRL0, reg);

    return 0;
}

void cc2420_get_addr_short(cc2420_t *dev, uint8_t *addr)
{
    uint8_t tmp[2];

    ram_read(dev, CC2420_RAM_SHORTADR, tmp, 2);

    addr[0] = tmp[1];
    addr[1] = tmp[0];
}

void cc2420_set_addr_short(cc2420_t *dev, uint8_t *addr)
{
    uint8_t tmp[2];
    tmp[0] = addr[1];
    tmp[1] = addr[0];

    ram_write(dev, CC2420_RAM_SHORTADR, tmp, 2);

    /* TODO: remove after 802.15.4 / netdev2 cleanup */
    memcpy(dev->netdev.short_addr, addr, 2);
}

void cc2420_get_addr_long(cc2420_t *dev, uint8_t *addr)
{
    ram_read(dev, CC2420_RAM_IEEEADR, addr, 8);
}

void cc2420_set_addr_long(cc2420_t *dev, uint8_t *addr)
{
    ram_write(dev, CC2420_RAM_IEEEADR, addr, 8);

    /* TODO: remove after 802.15.4 / netdev2 cleanup */
    memcpy(dev->netdev.long_addr, addr, 8);
}

uint16_t cc2420_get_pan(cc2420_t *dev)
{
    le_uint16_t pan;

    ram_read(dev, CC2420_RAM_PANID, pan.u8, 2);
    return pan.u16;
}

void cc2420_set_pan(cc2420_t *dev, uint16_t pan)
{
    ram_write(dev, CC2420_RAM_PANID, (uint8_t *)&pan, 2);

    /* TODO: remove after 802.15.4 / netdev2 cleanup */
    dev->netdev.pan = pan;
}

uint16_t cc2420_get_chan(cc2420_t *dev)
{
    uint16_t chan;
    uint16_t freq = reg_read(dev, CC2420_REG_FSCTRL);

    chan = (((freq & CC2420_FSCTRL_FREQ_MASK) - 357) / 5) + 11;
    return chan;
}

int cc2420_set_chan(cc2420_t *dev, uint16_t chan)
{
    if ((chan < CC2420_CHAN_MIN) || (chan > CC2420_CHAN_MAX)) {
        DEBUG("[cc2420] set channel: given channel invalid\n");
        return -ENOTSUP;
    }

    /* calculation from http://www.ti.com/lit/ds/symlink/cc2420.pdf p.50 */
    uint16_t freq = reg_read(dev, CC2420_REG_FSCTRL);
    freq &= ~CC2420_FSCTRL_FREQ_MASK;
    freq |= (357 + (5 * (chan - 11)));
    reg_write(dev, CC2420_REG_FSCTRL, freq);

    /* TODO: remove after 802.15.4 / netdev2 cleanup */
    dev->netdev.chan = chan;

    return CC2420_RET_CHAN_OK;
}

int16_t cc2420_get_txpower(cc2420_t *dev)
{
    uint16_t txctrl = reg_read(dev, CC2420_REG_TXCTRL);
    return (int16_t)power_pa_to_dbm[(txctrl & CC2420_TXCTRL_PA_MASK) >> 2];
}

void cc2420_set_txpower(cc2420_t *dev, int16_t txpower)
{
    if (txpower > CC2420_TXPOWER_MAX) {
        txpower = CC2420_TXPOWER_MAX;
    }
    else if (txpower < CC2420_TXPOWER_MIN) {
        txpower = CC2420_TXPOWER_MIN;
    }

    uint16_t txctrl = reg_read(dev, CC2420_REG_TXCTRL);
    txctrl &= ~(CC2420_TXCTRL_PA_MASK);
    txctrl |= power_dbm_to_pa[txpower + 25];
    reg_write(dev, CC2420_REG_TXCTRL, txctrl);
}

int cc2420_set_option(cc2420_t *dev, uint16_t option, bool state)
{
    uint16_t reg;

    /* set option field */
    if (state) {
        dev->options |= option;
        /* trigger option specific actions */
        switch (option) {
            case CC2420_OPT_AUTOACK:
                DEBUG("[cc2420] set_opt: CC2420_OPT_AUTOACK\n");
                reg = reg_read(dev, CC2420_REG_MDMCTRL0);
                reg |= CC2420_MDMCTRL0_AUTOACK;
                reg_write(dev, CC2420_REG_MDMCTRL0, reg);
                /* TODO: remove after netdev2 cleanup */
                dev->netdev.flags |= NETDEV2_IEEE802154_ACK_REQ;
                break;

            case CC2420_OPT_CSMA:
                DEBUG("[cc2420] set_opt: CC2420_OPT_CSMA\n");
                /* TODO: en/disable csma */
                break;

            case CC2420_OPT_PROMISCUOUS:
                DEBUG("[cc2420] set_opt: CC2420_OPT_PROMISCUOUS\n");
                /* in promisc mode, AUTO ACKs are should be disabled */
                reg = reg_read(dev, CC2420_REG_MDMCTRL0);
                reg &= ~(CC2420_MDMCTRL0_AUTOACK);
                reg &= ~(CC2420_MDMCTRL0_ADR_DECODE);
                reg_write(dev, CC2420_REG_MDMCTRL0, reg);
                break;

            case CC2420_OPT_PRELOADING:
                DEBUG("[cc2420] set_opt: CC2420_OPT_PRELOADING\n");
                break;

            case CC2420_OPT_TELL_TX_START:
            case CC2420_OPT_TELL_TX_END:
            case CC2420_OPT_TELL_RX_START:
            case CC2420_OPT_TELL_RX_END:
                DEBUG("[cc2420] set_opt: TX/RX START/END\n");
                break;

            default:
                return -ENOTSUP;
        }
    }
    else {
        dev->options &= ~(option);
        /* trigger option specific actions */
        switch (option) {
            case CC2420_OPT_AUTOACK:
                DEBUG("[cc2420] clr_opt: CC2420_OPT_AUTOACK\n");
                reg = reg_read(dev, CC2420_REG_MDMCTRL0);
                reg &= ~(CC2420_MDMCTRL0_AUTOACK);
                reg_write(dev, CC2420_REG_MDMCTRL0, reg);
                /* TODO: remove after netdev cleanup */
                dev->netdev.flags &= ~(NETDEV2_IEEE802154_ACK_REQ);
                break;

            case CC2420_OPT_CSMA:
                DEBUG("[cc2420] clr_opt: CC2420_OPT_CSMA\n");
                /* TODO: en/disable csma */
                break;

            case CC2420_OPT_PROMISCUOUS:
                DEBUG("[cc2420] clr_opt: CC2420_OPT_PROMISCUOUS\n");
                reg = reg_read(dev, CC2420_REG_MDMCTRL0);
                reg |= CC2420_MDMCTRL0_ADR_DECODE;
                /* re-enable AUTOACK only if the option was set */
                if (dev->options & CC2420_OPT_AUTOACK) {
                    reg |= CC2420_MDMCTRL0_AUTOACK;
                }
                reg_write(dev, CC2420_REG_MDMCTRL0, reg);
                break;

            case CC2420_OPT_PRELOADING:
                DEBUG("[cc2420] clr_opt: CC2420_OPT_PRELOADING\n");
                break;

            case CC2420_OPT_TELL_TX_START:
            case CC2420_OPT_TELL_TX_END:
            case CC2420_OPT_TELL_RX_START:
            case CC2420_OPT_TELL_RX_END:
                DEBUG("[cc2420] clr_opt: TX/RX START/END\n");
                /* TODO */
                break;

            default:
                return -ENOTSUP;
        }
    }
    return sizeof(netopt_enable_t);
}

int cc2420_set_state(cc2420_t *dev, netopt_state_t cmd)
{
    if ((cc2420_get_state(dev) == NETOPT_STATE_OFF) &&
        (cmd != NETOPT_STATE_OFF)) {
        en_xosc(dev);
    }
    switch (cmd) {
        case NETOPT_STATE_OFF:
            strobe(dev, CC2420_STROBE_XOSCOFF);
            while (state(dev) != CC2420_STATE_PD) {}
            break;
        case NETOPT_STATE_SLEEP:
            strobe(dev, CC2420_STROBE_RFOFF);
            while (state(dev) != CC2420_STATE_IDLE) {}
            break;
        case NETOPT_STATE_IDLE:
            strobe(dev, CC2420_STROBE_FLUSHRX);
            strobe(dev, CC2420_STROBE_RXON);
            break;
        case NETOPT_STATE_TX:
            cc2420_tx_exec(dev);
            break;
        case NETOPT_STATE_RESET:
            cc2420_init(dev);
            break;
        case NETOPT_STATE_RX:
        default:
            DEBUG("[cc2420] set_state: called with invalid target state\n");
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

netopt_state_t cc2420_get_state(cc2420_t *dev)
{
    uint8_t cur_state = state(dev);

    if (cur_state == 0) {
        return NETOPT_STATE_OFF;
    }
    else if (cur_state == 1) {
        return NETOPT_STATE_SLEEP;
    }
    else if (((cur_state >= 32) && (cur_state <=39)) || (cur_state == 56)) {
        return NETOPT_STATE_TX;
    }
    else if ((cur_state >= 3) && (cur_state <= 6)) {
        return NETOPT_STATE_IDLE;
    }
    else {
        return NETOPT_STATE_RX;
    }
}

bool cc2420_cca(cc2420_t *dev)
{
    while (!(status(dev) & CC2420_STATUS_RSSI_VALID)) {}
    return gpio_read(dev->params.pin_cca);
}

size_t cc2420_send(cc2420_t *dev, const struct iovec *data, int count)
{
    size_t n = cc2420_tx_prepare(dev, data, count);

    if ((n > 0) && !(dev->options & CC2420_OPT_PRELOADING)) {
        cc2420_tx_exec(dev);
    }

    return n;
}

size_t cc2420_tx_prepare(cc2420_t *dev, const struct iovec *data, int count)
{
    size_t pkt_len = 2;     /* include the FCS (frame check sequence) */

    /* wait for any ongoing transmissions to be finished */
    while (cc2420_get_state(dev) & NETOPT_STATE_TX) {}

    /* get and check the length of the packet */
    for (int i = 0; i < count; i++) {
        pkt_len += data[i].iov_len;
    }
    if (pkt_len >= CC2420_PKT_MAXLEN) {
        DEBUG("[cc2420] tx_prep: unable to send, pkt too large\n");
        return 0;
    }

    /* flush TX FIFO and write new packet to it */
    strobe(dev, CC2420_STROBE_FLUSHTX);
    /* push packet length to TX FIFO */
    fifo_write(dev, (uint8_t *)&pkt_len, 1);
    /* push packet to TX FIFO */
    for (int i = 0; i < count; i++) {
        fifo_write(dev, (uint8_t *)data[i].iov_base, data[i].iov_len);
    }
    DEBUG("[cc2420] tx_prep: loaded %i byte into the TX FIFO\n", (int)pkt_len);

    return pkt_len;
}

void cc2420_tx_exec(cc2420_t *dev)
{
    /* make sure, any ongoing transmission is finished */
    DEBUG("[cc2420] tx_exec: waiting for any ongoing transmission\n");
    while (cc2420_get_state(dev) & NETOPT_STATE_TX) {}
    /* trigger the transmission */
    if (dev->options & CC2420_OPT_TELL_TX_START) {
        dev->netdev.netdev.event_callback(&dev->netdev.netdev,
                                          NETDEV2_EVENT_TX_STARTED, dev);
    }
    DEBUG("[cc2420] tx_exec: TX_START\n");
    if (dev->options & CC2420_OPT_CSMA) {
        DEBUG("[cc2420] tx_exec: triggering TX with CCA\n");
        strobe(dev, CC2420_STROBE_TXONCCA);
    }
    else {
        DEBUG("[cc2420] tx_exec: triggering TX without CCA\n");
        strobe(dev, CC2420_STROBE_TXON);
    }

    while (gpio_read(dev->params.pin_sfd)) {
        puts("\t...ongoing}");
    }
    if (dev->options & CC2420_OPT_TELL_TX_END) {
        dev->netdev.netdev.event_callback(&dev->netdev.netdev,
                                          NETDEV2_EVENT_TX_COMPLETE, dev);
    }
    DEBUG("[cc2420] tx_exec: TX_DONE\n");
}

int cc2420_rx(cc2420_t *dev, uint8_t *buf, size_t max_len, void *info)
{
    uint8_t len;

    /* get the packet length (without dropping it) (first byte in RX FIFO */
    ram_read(dev, CC2420_RAM_RXFIFO, &len, 1);
    len -= 2;   /* subtract RSSI and FCF */

    if (!buf) {
        DEBUG("[cc2420] recv: packet of length %i in RX FIFO\n", (int)len);
    }

    /* if a buffer is given, read (and drop) the packet */
    if (buf) {
        len = (len > max_len) ? max_len : len;

        /* drop length byte */
        fifo_read(dev, NULL, 1);
        /* read fifo contents */
        DEBUG("[cc2420] recv: reading %i byte of the packet\n", (int)len);
        fifo_read(dev, buf, len);

        uint8_t rssi;
        fifo_read(dev, &rssi, 1);
        DEBUG("[cc2420] recv: RSSI is %i\n", (int)rssi);
        /* finally flush the FIFO */
        strobe(dev, CC2420_STROBE_FLUSHRX);
        strobe(dev, CC2420_STROBE_FLUSHRX);
    }

    return (int)len;
}

static int init(netdev2_t *dev)
{
    return cc2420_init((cc2420_t *)dev);
}

static void isr(netdev2_t *netdev)
{
    netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE, NULL);
}

static int send(netdev2_t *netdev, const struct iovec *vector, int count)
{
    cc2420_t *dev = (cc2420_t *)netdev;
    return (int)cc2420_send(dev, vector, count);
}

static int recv(netdev2_t *netdev, char *buf, int len, void *info)
{
    cc2420_t *dev = (cc2420_t *)netdev;
    return (int)cc2420_rx(dev, (uint8_t *)buf, len, info);
}

static int get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    if (netdev == NULL) {
        return -ENODEV;
    }

    cc2420_t *dev = (cc2420_t *)netdev;

    int ext = netdev2_ieee802154_get(&dev->netdev, opt, val, max_len);
    if (ext > 0) {
        return ext;
    }

    switch (opt) {

        case NETOPT_ADDRESS:
            assert(max_len >= sizeof(uint16_t));
            cc2420_get_addr_short(dev, val);
            return sizeof(uint16_t);

        case NETOPT_ADDRESS_LONG:
            assert(max_len >= 8);
            cc2420_get_addr_long(dev, val);
            return 8;

        case NETOPT_NID:
            assert(max_len >= sizeof(uint16_t));
            return w_u16(val, cc2420_get_pan(dev));

        case NETOPT_CHANNEL:
            assert(max_len >= sizeof(uint16_t));
            return w_u16(val, cc2420_get_chan(dev));

        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            return w_i16(val, cc2420_get_txpower(dev));

        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = cc2420_get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_IS_CHANNEL_CLR:
            return opt_state(val, cc2420_cca(dev));

        case NETOPT_AUTOACK:
            return opt_state(val, (dev->options & CC2420_OPT_AUTOACK));

        case NETOPT_CSMA:
            return opt_state(val, (dev->options & CC2420_OPT_CSMA));

        case NETOPT_PRELOADING:
            return opt_state(val, (dev->options & CC2420_OPT_PRELOADING));

        case NETOPT_PROMISCUOUSMODE:
            return opt_state(val, (dev->options & CC2420_OPT_PROMISCUOUS));

        case NETOPT_RX_START_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_START));

        case NETOPT_RX_END_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_TX_END));

        case NETOPT_TX_START_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_START));

        case NETOPT_TX_END_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_END));

        default:
            return -ENOTSUP;
    }
}

static int set(netdev2_t *netdev, netopt_t opt, void *val, size_t val_len)
{
    if (netdev == NULL) {
        return -ENODEV;
    }

    cc2420_t *dev = (cc2420_t *)netdev;

    int ext = netdev2_ieee802154_set(&dev->netdev, opt, val, val_len);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(val_len == 2);
            cc2420_set_addr_short(dev, (uint8_t *)val);
            return 2;

        case NETOPT_ADDRESS_LONG:
            assert(val_len == 8);
            cc2420_set_addr_long(dev, (uint8_t *)val);
            return 8;

        case NETOPT_NID:
            assert(val_len == sizeof(uint16_t));
            cc2420_set_pan(dev, to_u16(val));
            return sizeof(uint16_t);

        case NETOPT_CHANNEL:
            assert(val_len == sizeof(uint16_t));
            return cc2420_set_chan(dev, to_u16(val));

        case NETOPT_TX_POWER:
            assert(val_len == sizeof(int16_t));
            cc2420_set_txpower(dev, to_i16(val));
            return sizeof(int16_t);

        case NETOPT_STATE:
            assert(val_len == sizeof(netopt_state_t));
            return cc2420_set_state(dev, *((netopt_state_t *)val));

        case NETOPT_AUTOACK:
            return cc2420_set_option(dev, CC2420_OPT_AUTOACK, to_bool(val));

        case NETOPT_CSMA:
            return cc2420_set_option(dev, CC2420_OPT_CSMA, to_bool(val));

        case NETOPT_PRELOADING:
            return cc2420_set_option(dev, CC2420_OPT_PRELOADING, to_bool(val));

        case NETOPT_PROMISCUOUSMODE:
            return cc2420_set_option(dev, CC2420_OPT_PROMISCUOUS, to_bool(val));

        case NETOPT_RX_START_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_RX_START, to_bool(val));

        case NETOPT_RX_END_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_RX_END, to_bool(val));

        case NETOPT_TX_START_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_TX_START, to_bool(val));

        case NETOPT_TX_END_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_TX_END, to_bool(val));

        default:
            return ext;
    }

    return 0;
}

const netdev2_driver_t cc2420_driver = {
    .send = send,
    .recv = recv,
    .init = init,
    .isr = isr,
    .get = get,
    .set = set,
};
