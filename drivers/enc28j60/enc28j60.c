/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_enc28j60
 * @{
 *
 * @file
 * @brief       Implementation of ENC28J60 driver interfaces
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "mutex.h"
#include "xtimer.h"
#include "assert.h"
#include "net/ethernet.h"
#include "net/netdev2/eth.h"

#include "enc28j60.h"
#include "enc28j60_regs.h"

#if CPUID_LEN
#include "periph/cpuid.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Amount of time to hold the reset pin low on reset
 */
#define DELAY_RESET                 (1000U)         /* 1ms */

/**
 * @brief   If the clock is not stable after these amount of tries we abort the
 *          initialization
 */
#define STARTUP_TIMEOUT             (1000U)

/**
 * @brief   Set SPI speed fixed to 10MHz
 *
 * The SPI speed is set to a fixed value, as it must be > 8MHz (see the devices
 * errata sheet).
 */
#define SPI_SPEED                   SPI_SPEED_10MHZ

/**
 * @brief   The devices build-in buffer size
 *
 * This is a shared buffer that is freely configurable to be used for TX and RX
 */
#define BUFFER_SIZE                 (0x2000)

/**
 * @brief   Buffer configuration
 *
 * We use the split the buffer in the following way:
 * - RX: 6k [0x0000 to 0x17fe]
 * - TX: 2k [0x1800 to 0x1ffe]
 *
 * RX must start at buffer address 0x0000 (see errata sheet, section 5)
 */
#define BUF_TX_START                ((BUFFER_SIZE / 4) * 3)
#define BUF_TX_END                  (BUFFER_SIZE - 2)
#define BUF_RX_START                (0)
#define BUF_RX_END                  (BUF_TX_START - 2)


static void switch_bank(enc28j60_t *dev, int8_t bank)
{
    /* only switch bank if needed */
    if ((bank < 0) || (dev->bank == bank)) {
        return;
    }
    /* clear old value */
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi, CMD_BFC | REG_ECON1, 0x03, 0);
    gpio_set(dev->cs_pin);
    /* set new value */
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi, CMD_BFS | REG_ECON1, bank, 0);
    gpio_set(dev->cs_pin);
    /* remember active bank */
    dev->bank = bank;
}

static uint8_t cmd_rcr(enc28j60_t *dev, uint8_t reg, int8_t bank)
{
    char res;

    switch_bank(dev, bank);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi, CMD_RCR | reg, 0, &res);
    gpio_set(dev->cs_pin);
    return (uint8_t)res;
}

static uint8_t cmd_rcr_miimac(enc28j60_t *dev, uint8_t reg, int8_t bank)
{
    char res[2];

    switch_bank(dev, bank);
    gpio_clear(dev->cs_pin);
    spi_transfer_regs(dev->spi, CMD_RCR | reg, NULL, res, 2);
    gpio_set(dev->cs_pin);
    return (uint8_t)res[1];
}

static void cmd_wcr(enc28j60_t *dev, uint8_t reg, int8_t bank, uint8_t value)
{
    switch_bank(dev, bank);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi, CMD_WCR | reg, (char)value, 0);
    gpio_set(dev->cs_pin);
}

static void cmd_bfs(enc28j60_t *dev, uint8_t reg, int8_t bank, uint8_t mask)
{
    switch_bank(dev, bank);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi, CMD_BFS | reg, mask, 0);
    gpio_set(dev->cs_pin);
}

static void cmd_bfc(enc28j60_t *dev, uint8_t reg, int8_t bank, uint8_t mask)
{
    switch_bank(dev, bank);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi, CMD_BFC | reg, mask, 0);
    gpio_set(dev->cs_pin);
}

static uint16_t cmd_r_addr(enc28j60_t *dev, uint8_t addr)
{
    uint8_t low = cmd_rcr(dev, addr, 0);
    uint8_t high = cmd_rcr(dev, addr + 1, 0);
    return (uint16_t)((high << 8) | low);
}

static void cmd_w_addr(enc28j60_t *dev, uint8_t addr, uint16_t val)
{
    cmd_wcr(dev, addr, 0, (val & 0xff));
    cmd_wcr(dev, addr + 1, 0, (val >> 8));
}

static uint16_t cmd_r_phy(enc28j60_t *dev, uint8_t reg)
{
    /* set target register for reading */
    cmd_wcr(dev, REG_B2_MIREGADR, 2, reg);
    /* trigger register read and wait for results */
    cmd_wcr(dev, REG_B2_MICMD, 2, MICMD_MIIRD);
    cmd_wcr(dev, REG_B2_MICMD, 2, 0x00);
    while (cmd_rcr_miimac(dev, REG_B3_MISTAT, 3) & MISTAT_BUSY);
    /* results */
    uint8_t low = cmd_rcr_miimac(dev, REG_B2_MIRDL, 2);
    uint8_t high = cmd_rcr_miimac(dev, REG_B2_MIRDH, 2);
    return (uint16_t)((high << 8) | low);
}

static void cmd_w_phy(enc28j60_t *dev, uint8_t reg, uint16_t val)
{
    /* set target register and values to write */
    cmd_wcr(dev, REG_B2_MIREGADR, 2, reg);
    cmd_wcr(dev, REG_B2_MIWRL, 2, (val & 0xff));
    cmd_wcr(dev, REG_B2_MIWRH, 2, (val >> 8));
    /* wait until the transaction is finished */
    while (cmd_rcr_miimac(dev, REG_B3_MISTAT, 3) & MISTAT_BUSY);
}

static void cmd_rbm(enc28j60_t *dev, uint8_t *data, size_t len)
{
    gpio_clear(dev->cs_pin);
    spi_transfer_regs(dev->spi, CMD_RBM, NULL, (char *)data, len);
    gpio_set(dev->cs_pin);
}

static void cmd_wbm(enc28j60_t *dev, uint8_t *data, size_t len)
{
    gpio_clear(dev->cs_pin);
    spi_transfer_regs(dev->spi, CMD_WBM, (char *)data, NULL, len);
    gpio_set(dev->cs_pin);
}

static void mac_get(enc28j60_t *dev, uint8_t *mac)
{
    mac[0] = cmd_rcr_miimac(dev, REG_B3_MAADR6, 3);
    mac[1] = cmd_rcr_miimac(dev, REG_B3_MAADR5, 3);
    mac[2] = cmd_rcr_miimac(dev, REG_B3_MAADR4, 3);
    mac[3] = cmd_rcr_miimac(dev, REG_B3_MAADR3, 3);
    mac[4] = cmd_rcr_miimac(dev, REG_B3_MAADR2, 3);
    mac[5] = cmd_rcr_miimac(dev, REG_B3_MAADR1, 3);
}

static void mac_set(enc28j60_t *dev, uint8_t *mac)
{
    cmd_wcr(dev, REG_B3_MAADR6, 3, mac[0]);
    cmd_wcr(dev, REG_B3_MAADR5, 3, mac[1]);
    cmd_wcr(dev, REG_B3_MAADR4, 3, mac[2]);
    cmd_wcr(dev, REG_B3_MAADR3, 3, mac[3]);
    cmd_wcr(dev, REG_B3_MAADR2, 3, mac[4]);
    cmd_wcr(dev, REG_B3_MAADR1, 3, mac[5]);
}

static void on_int(void *arg)
{
    netdev2_t *netdev = (netdev2_t *)arg;
    netdev->event_callback(arg, NETDEV2_EVENT_ISR, netdev->isr_arg);
}

static int nd_send(netdev2_t *netdev, const struct iovec *data, int count)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;
    uint8_t ctrl = 0;
    int c = 0;

    mutex_lock(&dev->devlock);

#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += count;
#endif

    /* set write pointer */
    cmd_w_addr(dev, ADDR_WRITE_PTR, BUF_TX_START);
    /* write control byte and the actual data into the buffer */
    cmd_wbm(dev, &ctrl, 1);
    for (int i = 0; i < count; i++) {
        c += data[i].iov_len;
        cmd_wbm(dev, (uint8_t *)data[i].iov_base, data[i].iov_len);
    }
    /* set TX end pointer */
    cmd_w_addr(dev, ADDR_TX_END, cmd_r_addr(dev, ADDR_WRITE_PTR) - 1);
    /* trigger the send process */
    cmd_bfs(dev, REG_ECON1, -1, ECON1_TXRTS);

    mutex_unlock(&dev->devlock);
    return c;
}

static int nd_recv(netdev2_t *netdev, char *buf, int max_len, void *info)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;
    uint8_t head[6];
    size_t size;
    uint16_t next;

    (void)info;
    mutex_lock(&dev->devlock);

    /* set read pointer to RX read address */
    cmd_w_addr(dev, ADDR_READ_PTR, cmd_r_addr(dev, ADDR_RX_READ));
    /* read packet header */
    cmd_rbm(dev, head, 6);
    /* TODO: care for endianess */
    next = (uint16_t)((head[1] << 8) | head[0]);
    size = (size_t)((head[3] << 8) | head[2]) - 4;  /* discard CRC */

    if (buf != NULL) {
#ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev2->stats.rx_bytes += size;
#endif
        /* read packet content into the supplied buffer */
        if (size <= max_len) {
            cmd_rbm(dev, (uint8_t *)buf, size);
        } else {
            DEBUG("[enc28j60] recv: unable to get packet - buffer too small\n");
            size = 0;
        }
        /* release memory */
        cmd_w_addr(dev, ADDR_RX_READ, next);
        cmd_bfs(dev, REG_ECON2, -1, ECON2_PKTDEC);
    }

    mutex_unlock(&dev->devlock);
    return (int)size;
}

static int nd_init(netdev2_t *netdev)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;
    int res;
    uint8_t tmp;

    /* get exclusive access of the device */
    mutex_lock(&dev->devlock);

    /* setup the low-level interfaces */
    gpio_init(dev->reset_pin, GPIO_OUT);
    gpio_clear(dev->reset_pin);     /* this puts the device into reset state */
    gpio_init(dev->cs_pin, GPIO_OUT);
    gpio_set(dev->cs_pin);
    gpio_init_int(dev->int_pin, GPIO_IN, GPIO_FALLING, on_int, (void *)dev);
    res = spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, SPI_SPEED);
    if (res < 0) {
        DEBUG("[enc28j60] init: error initializing SPI bus [%i]\n", res);
        return -1;
    }

    /* wait at least 1ms and then release device from reset state */
    xtimer_usleep(DELAY_RESET);
    gpio_set(dev->reset_pin);

    /* wait for oscillator to be stable before proceeding */
    res = 0;
    do {
        tmp = cmd_rcr(dev, REG_ESTAT, -1);
        if (res++ >= STARTUP_TIMEOUT) {
            DEBUG("[enc28j60] init: error waiting for stable clock, SPI ok?\n");
            return -1;
        }
    } while (!(tmp & ESTAT_CLKRDY));

    /* disable clock output to save a little power */
    cmd_wcr(dev, REG_B3_ECOCON, 3, 0x00);

    /* BUFFER configuration */
    /* configure the RX buffer */
    cmd_w_addr(dev, ADDR_RX_START, BUF_RX_START);
    cmd_w_addr(dev, ADDR_RX_END, BUF_RX_END);
    cmd_w_addr(dev, ADDR_RX_READ, BUF_RX_START);
    /* configure the TX buffer */
    cmd_w_addr(dev, ADDR_TX_START, BUF_TX_START);
    cmd_w_addr(dev, ADDR_TX_END, BUF_TX_END);

    /* FILTER configuration */
    /* setup receive filters - we accept everything per default */
    cmd_wcr(dev, REG_B1_ERXFCON, 1, 0);

    /* MAC configuration */
    /* enable RX through filter and enable sending of RX and TX pause frames */
    tmp = (MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN);
    cmd_wcr(dev, REG_B2_MACON1, 2, tmp);
    /* enable full duplex mode, padding to min 60 byte + CRC for all frames and
     * CRC creation for all packets before transmission */
    tmp = (MACON3_FULDPX | MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
    cmd_wcr(dev, REG_B2_MACON3, 2, tmp);
    /* defer TX infinitely if medium is busy for IEEE 802.3 compliance */
    tmp = (MACON4_DEFER);
    cmd_wcr(dev, REG_B2_MACON4, 2, tmp);
    /* set back-to-back inter-packet gap -> 0x15 for full duplex */
    cmd_wcr(dev, REG_B2_MABBIPG, 2, MABBIPG_FD);
    /* set non-back-to-back inter packet gap -> 0x12 is default */
    cmd_wcr(dev, REG_B2_MAIPGL, 2, MAIPGL_FD);
    /* set default MAC address */
#if CPUID_LEN
    uint8_t macbuf[CPUID_LEN];
    cpuid_get(&macbuf);     /* we get the full ID but use only parts of it */
    macbuf[0] |= 0x02;      /* locally administered address */
    macbuf[0] &= ~0x01;     /* unicast address */
#else
    uint8_t macbuf[] = ENC28J60_FALLBACK_MAC;
#endif
    mac_set(dev, macbuf);

    /* PHY configuration */
    cmd_w_phy(dev, REG_PHY_PHCON1, PHCON1_PDPXMD);
    cmd_w_phy(dev, REG_PHY_PHIE, PHIE_PLNKIE | PHIE_PGEIE);

    /* Finishing touches */
    /* enable hardware flow control */
    cmd_wcr(dev, REG_B3_EFLOCON, 3, EFLOCON_FULDPXS | EFLOCON_FCEN1);
    /* enable auto-inc of read and write pointers for the RBM/WBM commands */
    cmd_bfs(dev, REG_ECON2, -1, ECON2_AUTOINC);
    /* enable receive, link and tx interrupts */
    cmd_bfc(dev, REG_EIR, -1, (EIR_LINKIF | EIR_PKTIF | EIR_RXERIF | EIR_TXIF |
                               EIR_TXERIF));
    cmd_bfs(dev, REG_EIE, -1, (EIE_INTIE | EIE_LINKIE | EIE_PKTIE | EIE_RXERIE |
                               EIE_TXIE | EIE_TXERIE));
    /* allow receiving bytes from now on */
    cmd_bfs(dev, REG_ECON1, -1, ECON1_RXEN);

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif
    mutex_unlock(&dev->devlock);
    return 0;
}

static void nd_isr(netdev2_t *netdev)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;
    uint8_t eir = cmd_rcr(dev, REG_EIR, -1);

    while (eir != 0) {
        if (eir & EIR_LINKIF) {
            /* clear link state interrupt flag */
            cmd_r_phy(dev, REG_PHY_PHIR);
            /* go and tell the new link layer state to upper layers */
            if (cmd_r_phy(dev, REG_PHY_PHSTAT2) & PHSTAT2_LSTAT) {
                DEBUG("[enc28j60] isr: link up!\n");
                netdev->event_callback(netdev, NETDEV2_EVENT_LINK_UP, NULL);
            }
            else {
                DEBUG("[enc28j60] isr: link down!\n");
                netdev->event_callback(netdev, NETDEV2_EVENT_LINK_DOWN, NULL);
            }
        }
        if (eir & EIR_PKTIF) {
            do {
                DEBUG("[enc28j60] isr: packet received\n");
                netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE, NULL);
            } while (cmd_rcr(dev, REG_B1_EPKTCNT, 1) > 0);
        }
        if (eir & EIR_RXERIF) {
            DEBUG("[enc28j60] isr: incoming packet dropped - RX buffer full\n");
            cmd_bfc(dev, REG_EIR, -1, EIR_RXERIF);
        }
        if (eir & EIR_TXIF) {
            DEBUG("[enc28j60] isr: packet transmitted\n");
            netdev->event_callback(netdev, NETDEV2_EVENT_TX_COMPLETE, NULL);
            cmd_bfc(dev, REG_EIR, -1, EIR_TXIF);
        }
        if (eir & EIR_TXERIF) {
            DEBUG("[enc28j60] isr: error during transmission - pkt dropped\n");
            cmd_bfc(dev, REG_EIR, -1, EIR_TXERIF);
        }
        eir = cmd_rcr(dev, REG_EIR, -1);
    }
}

static int nd_get(netdev2_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            mac_get(dev, (uint8_t *)value);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev2_eth_get(netdev, opt, value, max_len);
    }
}

static int nd_set(netdev2_t *netdev, netopt_t opt, void *value, size_t value_len)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(value_len == ETHERNET_ADDR_LEN);
            mac_set(dev, (uint8_t *)value);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev2_eth_set(netdev, opt, value, value_len);
    }
}

const static netdev2_driver_t netdev2_driver_enc28j60 = {
    .send = nd_send,
    .recv = nd_recv,
    .init = nd_init,
    .isr = nd_isr,
    .get = nd_get,
    .set = nd_set,
};

void enc28j60_setup(enc28j60_t *dev, const enc28j60_params_t *params)
{
    dev->netdev.driver = &netdev2_driver_enc28j60;
    dev->spi = params->spi;
    dev->cs_pin = params->cs_pin;
    dev->int_pin = params->int_pin;
    dev->reset_pin = params->reset_pin;
    mutex_init(&dev->devlock);
    dev->bank = 99;                         /* mark as invalid */
}
