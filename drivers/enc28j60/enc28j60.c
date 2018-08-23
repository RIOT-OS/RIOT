/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_enc28j60
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

#include "luid.h"
#include "mutex.h"
#include "xtimer.h"
#include "assert.h"
#include "net/ethernet.h"
#include "net/netdev_driver_glue.h"
#include "net/netdev/eth.h"

#include "enc28j60.h"
#include "enc28j60_regs.h"

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
#define STARTUP_TIMEOUT             (1000)

/**
 * @brief   Set SPI speed fixed to 10MHz
 *
 * The SPI speed is set to a fixed value, as it must be > 8MHz (see the devices
 * errata sheet).
 */
#define SPI_CLK                     SPI_CLK_10MHZ

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
#define BUF_TX_END                  (BUFFER_SIZE - 1)
#define BUF_RX_START                (0)
#define BUF_RX_END                  (BUF_TX_START - 1)

/**
 * @brief  Maximum transmission time
 *
 * The time in us that is required to send an Ethernet frame of maximum length
 * (Preamle + SFD + 1518 byte) at 10 Mbps in full duplex mode with a guard
 * period of 9,6 us. This time is used as time out for send operations.
 */
#define MAX_TX_TIME                 (1230U)

static void switch_bank(enc28j60_t *dev, int8_t bank)
{
    /* only switch bank if needed */
    if ((bank < 0) || (dev->bank == bank)) {
        return;
    }
    /* clear old value */
    spi_transfer_reg(dev->spi, dev->cs_pin, (CMD_BFC | REG_ECON1), 0x03);
    /* set new value */
    spi_transfer_reg(dev->spi, dev->cs_pin, (CMD_BFS | REG_ECON1), bank);
    /* remember active bank */
    dev->bank = bank;
}

static uint8_t cmd_rcr(enc28j60_t *dev, uint8_t reg, int8_t bank)
{
    uint8_t res;

    /* start transaction */
    spi_acquire(dev->spi, dev->cs_pin, SPI_MODE_0, SPI_CLK);

    switch_bank(dev, bank);
    res = spi_transfer_reg(dev->spi, dev->cs_pin, (CMD_RCR | reg), 0);

    /* finish SPI transaction */
    spi_release(dev->spi);

    return res;
}

static uint8_t cmd_rcr_miimac(enc28j60_t *dev, uint8_t reg, int8_t bank)
{
    char res[2];

    /* start transaction */
    spi_acquire(dev->spi, dev->cs_pin, SPI_MODE_0, SPI_CLK);

    switch_bank(dev, bank);
    spi_transfer_regs(dev->spi, dev->cs_pin, (CMD_RCR | reg), NULL, res, 2);

    /* finish SPI transaction */
    spi_release(dev->spi);

    return (uint8_t)res[1];
}

static void cmd_wcr(enc28j60_t *dev, uint8_t reg, int8_t bank, uint8_t value)
{
    /* start transaction */
    spi_acquire(dev->spi, dev->cs_pin, SPI_MODE_0, SPI_CLK);

    switch_bank(dev, bank);
    spi_transfer_reg(dev->spi, dev->cs_pin, (CMD_WCR | reg), value);

    /* finish SPI transaction */
    spi_release(dev->spi);
}

static void cmd_bfs(enc28j60_t *dev, uint8_t reg, int8_t bank, uint8_t mask)
{
    /* start transaction */
    spi_acquire(dev->spi, dev->cs_pin, SPI_MODE_0, SPI_CLK);

    switch_bank(dev, bank);
    spi_transfer_reg(dev->spi, dev->cs_pin, (CMD_BFS | reg), mask);

    /* finish SPI transaction */
    spi_release(dev->spi);
}

static void cmd_bfc(enc28j60_t *dev, uint8_t reg, int8_t bank, uint8_t mask)
{
    /* start transaction */
    spi_acquire(dev->spi, dev->cs_pin, SPI_MODE_0, SPI_CLK);

    switch_bank(dev, bank);
    spi_transfer_reg(dev->spi, dev->cs_pin, (CMD_BFC | reg), mask);

    /* finish SPI transaction */
    spi_release(dev->spi);
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
    while (cmd_rcr_miimac(dev, REG_B3_MISTAT, 3) & MISTAT_BUSY) {}
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
    while (cmd_rcr_miimac(dev, REG_B3_MISTAT, 3) & MISTAT_BUSY) {}
}

static void cmd_rbm(enc28j60_t *dev, uint8_t *data, size_t len)
{
    /* start transaction */
    spi_acquire(dev->spi, dev->cs_pin, SPI_MODE_0, SPI_CLK);
    /* transfer data */
    spi_transfer_regs(dev->spi, dev->cs_pin, CMD_RBM, NULL, data, len);
    /* finish SPI transaction */
    spi_release(dev->spi);
}

static void cmd_wbm(enc28j60_t *dev, uint8_t *data, size_t len)
{
    /* start transaction */
    spi_acquire(dev->spi, dev->cs_pin, SPI_MODE_0, SPI_CLK);
    /* transfer data */
    spi_transfer_regs(dev->spi, dev->cs_pin, CMD_WBM, data, NULL, len);
    /* finish SPI transaction */
    spi_release(dev->spi);
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
    netdev_t *netdev = (netdev_t *)arg;
    netdev->event_callback(arg, NETDEV_EVENT_ISR);
}

static int nd_send(netdev_t *netdev, const iolist_t *iolist)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;
    uint8_t ctrl = 0;
    int c = 0;

    mutex_lock(&dev->devlock);

    if (cmd_rcr(dev, REG_ECON1, -1) & ECON1_TXRTS) {
        /* there is already a transmission in progress */
        if (xtimer_now_usec() - dev->tx_time > MAX_TX_TIME * 2) {
            /*
             * if transmission time exceeds the double of maximum transmission
             * time, we suppose that TX logic hangs and has to be reset
             */
            cmd_bfs(dev, REG_ECON1, -1, ECON1_TXRST);
            cmd_bfc(dev, REG_ECON1, -1, ECON1_TXRST);
        }
        else {
            /*
             * otherwise we suppose that the transmission is still in progress
             * and return EBUSY
             */
            mutex_unlock(&dev->devlock);
            return -EBUSY;
        }
    }

    /* set TX start pointer */
    cmd_w_addr(dev, ADDR_TX_START, BUF_TX_START);
    /* set write pointer */
    cmd_w_addr(dev, ADDR_WRITE_PTR, BUF_TX_START);
    /* write control byte and the actual data into the buffer */
    cmd_wbm(dev, &ctrl, 1);
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        c += iol->iol_len;
        cmd_wbm(dev, iol->iol_base, iol->iol_len);
    }
    /* set TX end pointer */
    cmd_w_addr(dev, ADDR_TX_END, cmd_r_addr(dev, ADDR_WRITE_PTR) - 1);
    /* trigger the send process */
    cmd_bfs(dev, REG_ECON1, -1, ECON1_TXRTS);
    /* set last transmission time for timeout handling */
    dev->tx_time = xtimer_now_usec();

#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += c;
#endif

    mutex_unlock(&dev->devlock);
    return c;
}

/*
 * Section 14 of errata sheet: Even values in ERXRDPT may corrupt receive
 * buffer as well as the next packet pointer. ERXRDPT need to be set always
 * at odd addresses. Following macros determine odd ERXRDPT from next packet
 * pointer and vise versa. Next packet pointer is always at even address
 * because of hardware padding.
 */
#define NEXT_TO_ERXRDPT(n) ((n == BUF_RX_START || n - 1 > BUF_RX_END) ? BUF_RX_END : n - 1)
#define ERXRDPT_TO_NEXT(e) ((e >= BUF_RX_END) ? BUF_RX_START : e + 1)

static int nd_recv(netdev_t *netdev, void *buf, size_t max_len, void *info)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;
    uint8_t head[6];
    uint16_t size;
    uint16_t next;

    (void)info;
    mutex_lock(&dev->devlock);

    /* set read pointer to RX read address */
    uint16_t rx_rd_ptr = cmd_r_addr(dev, ADDR_RX_READ);
    cmd_w_addr(dev, ADDR_READ_PTR, ERXRDPT_TO_NEXT(rx_rd_ptr));
    /* read packet header */
    cmd_rbm(dev, head, 6);
    /* TODO: care for endianess */
    next = (uint16_t)((head[1] << 8) | head[0]);
    size = (uint16_t)((head[3] << 8) | head[2]) - 4;  /* discard CRC */

    if (buf != NULL) {
#ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += size;
#endif
        /* read packet content into the supplied buffer */
        if (size <= max_len) {
            cmd_rbm(dev, (uint8_t *)buf, size);
        } else {
            DEBUG("[enc28j60] recv: unable to get packet - buffer too small\n");
            size = 0;
        }
        /* release memory */
        cmd_w_addr(dev, ADDR_RX_READ, NEXT_TO_ERXRDPT(next));
        cmd_bfs(dev, REG_ECON2, -1, ECON2_PKTDEC);
    }

    mutex_unlock(&dev->devlock);
    return (int)size;
}

static int nd_init(netdev_t *netdev)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;
    int res;
    uint8_t tmp;

    /* get exclusive access of the device */
    mutex_lock(&dev->devlock);

    /* setup the low-level interfaces */
    gpio_init(dev->reset_pin, GPIO_OUT);
    gpio_clear(dev->reset_pin);     /* this puts the device into reset state */
    res = spi_init_cs(dev->spi, dev->cs_pin);
    if (res != SPI_OK) {
        DEBUG("[enc28j60] init: error initializing the CS pin [%i]\n", res);
        return -1;
    }
    gpio_init_int(dev->int_pin, GPIO_IN, GPIO_FALLING, on_int, (void *)dev);

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
    cmd_w_addr(dev, ADDR_RX_READ, NEXT_TO_ERXRDPT(BUF_RX_START));
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
    uint8_t macbuf[ETHERNET_ADDR_LEN];
    luid_get(macbuf, ETHERNET_ADDR_LEN);
    macbuf[0] |= 0x02;      /* locally administered address */
    macbuf[0] &= ~0x01;     /* unicast address */
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

static void nd_isr(netdev_t *netdev)
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
                netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);
            }
            else {
                DEBUG("[enc28j60] isr: link down!\n");
                netdev->event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
            }
        }
        if (eir & EIR_PKTIF) {
            do {
                DEBUG("[enc28j60] isr: packet received\n");
                netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
            } while (cmd_rcr(dev, REG_B1_EPKTCNT, 1) > 0);
        }
        if (eir & EIR_RXERIF) {
            DEBUG("[enc28j60] isr: incoming packet dropped - RX buffer full\n");
            cmd_bfc(dev, REG_EIR, -1, EIR_RXERIF);
        }
        if (eir & EIR_TXIF) {
            DEBUG("[enc28j60] isr: packet transmitted\n");
            netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
            cmd_bfc(dev, REG_EIR, -1, EIR_TXIF);
        }
        if (eir & EIR_TXERIF) {
            DEBUG("[enc28j60] isr: error during transmission - pkt dropped\n");
            cmd_bfc(dev, REG_EIR, -1, EIR_TXERIF);
        }
        eir = cmd_rcr(dev, REG_EIR, -1);
    }
}

static int nd_get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            mac_get(dev, (uint8_t *)value);
            return ETHERNET_ADDR_LEN;
        case NETOPT_LINK_CONNECTED:
            if (cmd_r_phy(dev, REG_PHY_PHSTAT2) & PHSTAT2_LSTAT) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);
        default:
            return netdev_eth_get(netdev, opt, value, max_len);
    }
}

static int nd_set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len)
{
    enc28j60_t *dev = (enc28j60_t *)netdev;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(value_len == ETHERNET_ADDR_LEN);
            mac_set(dev, (uint8_t *)value);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, value, value_len);
    }
}

static const netdev_driver_t netdev_driver_enc28j60 = {
    .send = nd_send,
    .recv = nd_recv,
    .size = netdev_driver_glue_size,
    .drop = netdev_driver_glue_drop,
    .init = nd_init,
    .isr = nd_isr,
    .get = nd_get,
    .set = nd_set,
};

void enc28j60_setup(enc28j60_t *dev, const enc28j60_params_t *params)
{
    dev->netdev.driver = &netdev_driver_enc28j60;
    dev->spi = params->spi;
    dev->cs_pin = params->cs_pin;
    dev->int_pin = params->int_pin;
    dev->reset_pin = params->reset_pin;
    mutex_init(&dev->devlock);
    dev->bank = 99;                         /* mark as invalid */
    dev->tx_time = 0;
}
