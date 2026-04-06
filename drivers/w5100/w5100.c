/*
 * SPDX-FileCopyrightText: 2016-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_w5100
 * @{
 *
 * @file
 * @brief       Device driver implementation for w5100 Ethernet devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "assert.h"

#include "net/eui_provider.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"

#include "w5100.h"
#include "w5100_regs.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define SPI_CONF            SPI_MODE_0
#define RMSR_DEFAULT_VALUE  (0x55)

#define S0_MEMSIZE          (0x2000)
#define S0_MASK             (S0_MEMSIZE - 1)
#define S0_TX_BASE          (0x4000)
#define S0_RX_BASE          (0x6000)

static const netdev_driver_t netdev_driver_w5100;

static inline void send_addr(w5100_t *dev, uint16_t addr)
{
    spi_transfer_u16_be(dev->p.spi, dev->p.cs, true, addr);
}

static uint8_t rreg(w5100_t *dev, uint16_t reg)
{
    spi_transfer_byte(dev->p.spi, dev->p.cs, true, CMD_READ);
    send_addr(dev, reg++);
    return spi_transfer_byte(dev->p.spi, dev->p.cs, false, 0);
}

static void wreg(w5100_t *dev, uint16_t reg, uint8_t data)
{
    spi_transfer_byte(dev->p.spi, dev->p.cs, true, CMD_WRITE);
    send_addr(dev, reg);
    spi_transfer_byte(dev->p.spi, dev->p.cs, false, data);
}

static uint16_t raddr(w5100_t *dev, uint16_t addr_high, uint16_t addr_low)
{
    uint16_t res = (rreg(dev, addr_high) << 8);

    res |= rreg(dev, addr_low);
    return res;
}

static void waddr(w5100_t *dev,
                  uint16_t addr_high, uint16_t addr_low, uint16_t val)
{
    wreg(dev, addr_high, (uint8_t)(val >> 8));
    wreg(dev, addr_low, (uint8_t)(val & 0xff));
}

static void rchunk(w5100_t *dev, uint16_t addr, uint8_t *data, size_t len)
{
    /* reading a chunk must be split in multiple single byte reads, as the
     * device does not support auto address increment via SPI */
    for (int i = 0; i < (int)len; i++) {
        data[i] = rreg(dev, addr++);
    }
}

static void wchunk(w5100_t *dev, uint16_t addr, uint8_t *data, size_t len)
{
    /* writing a chunk must be split in multiple single byte writes, as the
     * device does not support auto address increment via SPI */
    for (int i = 0; i < (int)len; i++) {
        wreg(dev, addr++, data[i]);
    }
}

static void extint(void *arg)
{
    w5100_t *dev = (w5100_t *)arg;

    netdev_trigger_event_isr(&dev->nd);
}

void w5100_setup(w5100_t *dev, const w5100_params_t *params, uint8_t index)
{
    assert(dev);
    assert(params);

    /* initialize netdev structure */
    dev->nd.driver = &netdev_driver_w5100;
    dev->nd.event_callback = NULL;
    dev->nd.context = dev;

    /* initialize the device descriptor */
    dev->p = *params;

    /* initialize the chip select pin and the external interrupt pin */
    spi_init_cs(dev->p.spi, dev->p.cs);
    gpio_init_int(dev->p.evt, GPIO_IN, GPIO_FALLING, extint, dev);

    netdev_register(&dev->nd, NETDEV_W5100, index);
}

static int init(netdev_t *netdev)
{
    w5100_t *dev = (w5100_t *)netdev;
    uint8_t tmp;
    eui48_t hwaddr;

    /* get access to the SPI bus for the duration of this function */
    spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);

    /* test the SPI connection by reading the value of the RMSR register */
    tmp = rreg(dev, REG_TMSR);
    if (tmp != RMSR_DEFAULT_VALUE) {
        spi_release(dev->p.spi);
        LOG_ERROR("[w5100] error: no SPI connection\n");
        return W5100_ERR_BUS;
    }

    /* reset the device */
    wreg(dev, REG_MODE, MODE_RESET);
    while (rreg(dev, REG_MODE) & MODE_RESET) {}

    /* initialize the device, start with writing the MAC address */
    netdev_eui48_get(netdev, &hwaddr);
    wchunk(dev, REG_SHAR0, hwaddr.uint8, sizeof(hwaddr));

    /* configure all memory to be used by socket 0 */
    wreg(dev, REG_RMSR, RMSR_8KB_TO_S0);
    wreg(dev, REG_TMSR, TMSR_8KB_TO_S0);

    /* configure interrupt pin to trigger on socket 0 events */
    wreg(dev, REG_IMR, IMR_S0_INT);

    /* next we configure socket 0 to work in MACRAW mode */
    wreg(dev, S0_MR, MR_MACRAW);
    wreg(dev, S0_CR, CR_OPEN);

    /* set the source IP address to something random to prevent the device to do
     * stupid thing (e.g. answering ICMP echo requests on its own) */
    wreg(dev, REG_SIPR0, 0x01);
    wreg(dev, REG_SIPR1, 0x01);
    wreg(dev, REG_SIPR2, 0x01);
    wreg(dev, REG_SIPR3, 0x01);

    /* start receiving packets */
    wreg(dev, S0_CR, CR_RECV);

    /* release the SPI bus again */
    spi_release(dev->p.spi);

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static uint16_t tx_upload(w5100_t *dev, uint16_t start, void *data, size_t len)
{
    if ((start + len) >= (S0_TX_BASE + S0_MEMSIZE)) {
        size_t limit = ((S0_TX_BASE + S0_MEMSIZE) - start);
        wchunk(dev, start, data, limit);
        wchunk(dev, S0_TX_BASE, &((uint8_t *)data)[limit], len - limit);
        return (S0_TX_BASE + len - limit);
    }
    else {
        wchunk(dev, start, data, len);
        waddr(dev, S0_TX_WR0, S0_TX_WR1, start + len);
        return (start + len);
    }
}

static int send(netdev_t *netdev, const iolist_t *iolist)
{
    w5100_t *dev = (w5100_t *)netdev;
    int sum = 0;

    /* get access to the SPI bus for the duration of this function */
    spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);

    uint16_t tx_wr = raddr(dev, S0_TX_WR0, S0_TX_WR1);
    uint16_t pos = (tx_wr & S0_MASK) + S0_TX_BASE;

    /* the register is only set correctly after the first send pkt, so we need
     * this fix here */
    if (pos == 0) {
        pos = S0_TX_BASE;
    }

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        size_t len = iol->iol_len;
        pos = tx_upload(dev, pos, iol->iol_base, len);
        sum += len;
    }

    waddr(dev, S0_TX_WR0, S0_TX_WR1, tx_wr + sum);

    /* trigger the sending process */
    wreg(dev, S0_CR, CR_SEND_MAC);
    while (!(rreg(dev, S0_IR) & IR_SEND_OK)) {}
    wreg(dev, S0_IR, IR_SEND_OK);

    DEBUG("[w5100] send: transferred %i byte (at 0x%04x)\n", sum, (int)pos);

    /* release the SPI bus again */
    spi_release(dev->p.spi);

    return sum;
}

static inline void drop(w5100_t *dev, uint16_t num, uint16_t rp, uint16_t psize)
{
    /* set the new read pointer address */
    waddr(dev, S0_RX_RD0, S0_RX_RD1, rp + psize);
    wreg(dev, S0_CR, CR_RECV);

    /* if RX buffer now empty, clear RECV interrupt flag */
    if ((num - psize) == 0) {
        wreg(dev, S0_IR, IR_RECV);
    }
}

static int recv(netdev_t *netdev, void *buf, size_t max_len, void *info)
{
    (void)info;
    w5100_t *dev = (w5100_t *)netdev;
    uint8_t *in_buf = (uint8_t *)buf;
    unsigned len = 0;

    /* get access to the SPI bus for the duration of this function */
    spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);

    uint16_t num = raddr(dev, S0_RX_RSR0, S0_RX_RSR1);

    if (num > 0) {
        /* find the size of the next packet in the RX buffer */
        uint16_t rp = raddr(dev, S0_RX_RD0, S0_RX_RD1);
        uint16_t psize = raddr(dev, (S0_RX_BASE + (rp & S0_MASK)),
                               (S0_RX_BASE + ((rp + 1) & S0_MASK)));
        len = psize - 2;

        DEBUG("[w5100] recv: got packet of %i byte (at 0x%04x)\n", len,
              (int)rp);

        /* read the actual data into the given buffer if wanted */
        if (in_buf != NULL) {
            /* Is provided buffer big enough? */
            if (len > max_len) {
                drop(dev, num, rp, psize);
                spi_release(dev->p.spi);
                return -ENOBUFS;
            }
            uint16_t pos = rp + 2;

            for (unsigned i = 0; i < len; i++) {
                in_buf[i] = rreg(dev, (S0_RX_BASE + ((pos++) & S0_MASK)));
            }

            DEBUG("[w5100] recv: read %i byte from device (at 0x%04x)\n",
                  len, (int)rp);
        }

        /* if frame received OR drop requested, remove frame from RX buffer */
        if (max_len > 0) {
            drop(dev, num, rp, psize);
        }
    }

    /* release the SPI bus again */
    spi_release(dev->p.spi);

    return (int)len;
}

static void isr(netdev_t *netdev)
{
    uint8_t ir;
    w5100_t *dev = (w5100_t *)netdev;

    /* read interrupt register */
    spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);
    ir = rreg(dev, S0_IR);
    spi_release(dev->p.spi);

    /* we only react on RX events, and if we see one, we read from the RX buffer
     * until it is empty */
    while (ir & IR_RECV) {
        DEBUG("[w5100] netdev RX complete\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);

        /* reread interrupt register */
        spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);
        ir = rreg(dev, S0_IR);
        spi_release(dev->p.spi);
    }
}

static int get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    w5100_t *dev = (w5100_t *)netdev;
    int res = 0;

    switch (opt) {
    case NETOPT_ADDRESS:
        assert(max_len >= ETHERNET_ADDR_LEN);
        spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);
        rchunk(dev, REG_SHAR0, value, ETHERNET_ADDR_LEN);
        spi_release(dev->p.spi);
        res = ETHERNET_ADDR_LEN;
        break;
    default:
        res = netdev_eth_get(netdev, opt, value, max_len);
        break;
    }

    return res;
}

static const netdev_driver_t netdev_driver_w5100 = {
    .send = send,
    .recv = recv,
    .init = init,
    .isr = isr,
    .get = get,
    .set = netdev_eth_set,
};
