/*
 * Copyright (C) 2023 Stefan Schmidt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_w5500
 * @{
 *
 * @file
 * @brief       Device driver implementation for w5500 Ethernet devices
 *
 * @author      Stefan Schmidt <stemschmidt@gmail.com>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "iolist.h"
#include "net/ethernet.h"
#include "net/eui_provider.h"
#include "net/netdev/eth.h"
#include "net/ethernet/hdr.h"
#include "ztimer.h"

#include "w5500.h"
#include "w5500_regs.h"

#define ENABLE_DEBUG        0
#include "debug.h"
#include "log.h"

/* Socket definitions. */
#define BSB_SOCKET_BASE         (0x08)      /**< Base for Socket n registers. */
#define RXBUF_16KB_TO_S0        (0x10)      /**< Receive memory size: 16 kB. */
#define TXBUF_16KB_TO_S0        (0x10)      /**< Transmit memory size: 16 kB. */

static const netdev_driver_t netdev_driver_w5500;

static uint8_t read_register(w5500_t *dev, uint16_t reg)
{
    uint16_t address = reg & 0x07ff;
    uint8_t command = (uint8_t)((reg & 0xf800) >> 8u) | CMD_READ;

    spi_transfer_u16_be(dev->p.spi, dev->p.cs, true, address);
    spi_transfer_byte(dev->p.spi, dev->p.cs, true, command);
    return spi_transfer_byte(dev->p.spi, dev->p.cs, false, 0u);
}

static void write_register(w5500_t *dev, uint16_t reg, uint8_t data)
{
    uint16_t address = reg & 0x07ff;
    uint8_t command = (uint8_t)((reg & 0xf800) >> 8u) | CMD_WRITE;

    spi_transfer_u16_be(dev->p.spi, dev->p.cs, true, address);
    spi_transfer_byte(dev->p.spi, dev->p.cs, true, command);
    spi_transfer_byte(dev->p.spi, dev->p.cs, false, data);
}

static uint16_t read_addr(w5500_t *dev, uint16_t addr_high, uint16_t addr_low)
{
    uint16_t res = (read_register(dev, addr_high) << 8);

    res |= read_register(dev, addr_low);
    return res;
}

static void write_address(w5500_t *dev,
                  uint16_t addr_high, uint16_t addr_low, uint16_t val)
{
    write_register(dev, addr_high, (uint8_t)(val >> 8));
    write_register(dev, addr_low, (uint8_t)(val & 0xff));
}

static void read_chunk(w5500_t *dev, uint16_t addr, uint8_t *data, size_t len)
{
    uint16_t address = addr & 0x07ff;
    uint8_t command = (uint8_t)((addr & 0xf800) >> 8u) | CMD_READ;

    spi_transfer_u16_be(dev->p.spi, dev->p.cs, true, address);
    spi_transfer_byte(dev->p.spi, dev->p.cs, true, command);
    spi_transfer_bytes(dev->p.spi, dev->p.cs, false, NULL, data, len);
}

static void write_chunk(w5500_t *dev, uint16_t addr, uint8_t *data, size_t len)
{
    uint16_t address = addr & 0x07ff;
    uint8_t command = (uint8_t)((addr & 0xf800) >> 8u) | CMD_WRITE;

    spi_transfer_u16_be(dev->p.spi, dev->p.cs, true, address);
    spi_transfer_byte(dev->p.spi, dev->p.cs, true, command);
    spi_transfer_bytes(dev->p.spi, dev->p.cs, false, data, NULL, len);
}

static void extint(void *arg)
{
    w5500_t *dev = (w5500_t *)arg;

    netdev_trigger_event_isr(&dev->netdev);

    ztimer_set(ZTIMER_MSEC, &dev->timerInstance, dev->p.polling_interval_ms);
}

void w5500_setup(w5500_t *dev, const w5500_params_t *params, uint8_t index)
{
    assert(dev);
    assert(params);

    /* Initialize netdev structure. */
    dev->netdev.driver = &netdev_driver_w5500;
    dev->netdev.event_callback = NULL;
    dev->netdev.context = dev;
    /* Initialize the device descriptor. */
    dev->p = *params;

    dev->frame_size = 0u;
    dev->link_up = false;
    dev->frame_sent = false;

    /* Initialize the chip select pin. */
    spi_init_cs(dev->p.spi, dev->p.cs);
    if (gpio_is_valid(dev->p.irq)) {
        /* Initialize the external interrupt pin. */
        gpio_init_int(dev->p.irq, GPIO_IN, GPIO_FALLING, extint, dev);
    }
    netdev_register(&dev->netdev, NETDEV_W5500, index);
}

static int init(netdev_t *netdev)
{
    w5500_t *dev = (w5500_t *)netdev;
    uint8_t tmp;

    spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);

    /* Reset the device. */
    write_register(dev, REG_MODE, MODE_RESET);
    while (read_register(dev, REG_MODE) & MODE_RESET) {}

    /* Test the SPI connection by reading the value of the version register. */
    tmp = read_register(dev, REG_VERSIONR);
    if (tmp != CHIP_VERSION) {
        spi_release(dev->p.spi);
        LOG_ERROR("[w5500] error: invalid chip ID %x\n", tmp);
        return -ENODEV;
    }

    /* Write the MAC address. */
    eui48_t address;
    netdev_eui48_get(netdev, &address);
    write_chunk(dev, REG_SHAR0, (uint8_t*)&address, sizeof(eui48_t));

    /* Configure 16 kB memory to be used by socket 0. */
    write_register(dev, REG_S0_RXBUF_SIZE, RXBUF_16KB_TO_S0);
    write_register(dev, REG_S0_TXBUF_SIZE, TXBUF_16KB_TO_S0);

    /* Configure remaining RX/TX buffers to 0 kB. */
    for (uint8_t socket = 1u; socket < 8u; socket++) {
        uint16_t bsb = (socket << 5u) | 0x08 | CMD_WRITE;
        uint16_t Sn_RXBUF_SIZE = Sn_RXBUF_SIZE_BASE | bsb;
        uint16_t Sn_TXBUF_SIZE = Sn_TXBUF_SIZE_BASE | bsb;
        write_register(dev, Sn_RXBUF_SIZE, 0u);
        write_register(dev, Sn_TXBUF_SIZE, 0u);
    }
    /* Next we configure socket 0 to work in MACRAW mode with MAC filtering. */
    write_register(dev, REG_S0_MR, (MR_MACRAW | ENABLE_MAC_FILTER | ENABLE_MULTICAST_FILTER));

    /* Set the source IP address to something random to prevent the device to do
     * stupid thing (e.g. answering ICMP echo requests on its own). */
    write_register(dev, REG_SIPR0, 0x00);
    write_register(dev, REG_SIPR1, 0x00);
    write_register(dev, REG_SIPR2, 0x00);
    write_register(dev, REG_SIPR3, 0x00);

    dev->timerInstance.callback = extint;
    dev->timerInstance.arg = dev;
    ztimer_set(ZTIMER_MSEC, &dev->timerInstance, dev->p.polling_interval_ms);

    if (gpio_is_valid(dev->p.irq)) {
        /* Configure interrupt pin to trigger on socket 0 events. */
        write_register(dev, REG_SIMR, IMR_S0_INT);
    }
    /* Open socket. */
    write_register(dev, REG_S0_CR, CR_OPEN);

    spi_release(dev->p.spi);

    return 0;
}

static void write_tx0_buffer(w5500_t *dev, uint16_t address, uint8_t *data, uint16_t size)
{
    spi_transfer_u16_be(dev->p.spi, dev->p.cs, true, address);
    spi_transfer_byte(dev->p.spi, dev->p.cs, true, SOCKET0_TX_BUFFER | CMD_WRITE);

    spi_transfer_bytes(dev->p.spi, dev->p.cs, false, data, NULL, size);
}

static uint16_t get_free_size_in_tx_buffer(w5500_t *dev)
{
    uint16_t tx_free = read_addr(dev, REG_S0_TX_FSR0, REG_S0_TX_FSR1);
    uint16_t tmp = read_addr(dev, REG_S0_TX_FSR0, REG_S0_TX_FSR1);

    /* See Note in the description of Sn_TX_FSR in the datasheet: This is a 16 bit value,
       so we read it until we get the same value twice. The W5500 will update it
       while transmitting data. */
    while (tx_free != tmp) {
        tx_free = tmp;
        tmp = read_addr(dev, REG_S0_TX_FSR0, REG_S0_TX_FSR1);
    }

    return tx_free;
}

static int send(netdev_t *netdev, const iolist_t *iolist)
{
    w5500_t *dev = (w5500_t *)netdev;
    int result = -ENODEV;

    /* Get access to the SPI bus for the duration of this function. */
    spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);

    uint8_t tmp = read_register(dev, REG_PHYCFGR);

    if (tmp & PHY_LINK_UP) {
        uint16_t tx_free = get_free_size_in_tx_buffer(dev);
        uint16_t data_length = (uint16_t)iolist_size(iolist);

        /* Get the write pointer. */
        uint16_t socket0_write_pointer = read_addr(dev, REG_S0_TX_WR0, REG_S0_TX_WR1);

        /* Make sure we can write the full packet. */
        if (data_length <= tx_free) {
            /* reset the frame size information */
            dev->frame_size = 0u;

            for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
                size_t len = iol->iol_len;
                write_tx0_buffer(dev, socket0_write_pointer + dev->frame_size,
                                 iol->iol_base, len);
                dev->frame_size += len;
            }
            /* Update the write pointer. */
            write_address(dev, REG_S0_TX_WR0, REG_S0_TX_WR1,
                  socket0_write_pointer + dev->frame_size);
            /* Trigger the sending process. */
            write_register(dev, REG_S0_CR, CR_SEND);

            DEBUG("[w5500] send: transferred %i byte (at 0x%04x)\n", data_length,
                  (int)socket0_write_pointer);
            result = 0;
        }
    }
    else {
        DEBUG("%s: link is down: PHYCFGR = 0x%02x\n", __func__, tmp);
        result = -EIO;
    }

    /* release the SPI bus again */
    spi_release(dev->p.spi);

    return result;
}

static int confirm_send(netdev_t *netdev, void *info)
{
    w5500_t *dev = (w5500_t *)netdev;

    (void)info;

    if (dev->frame_sent == false) {
        return -EAGAIN;
    }
    else {
        return dev->frame_size;
    }
}

static void read_rx0_buffer(w5500_t *dev, uint16_t address, uint8_t *data, uint16_t size)
{
    spi_transfer_u16_be(dev->p.spi, dev->p.cs, true, address);
    spi_transfer_byte(dev->p.spi, dev->p.cs, true, SOCKET0_RX_BUFFER);

    spi_transfer_bytes(dev->p.spi, dev->p.cs, false, NULL, data, size);
}

static uint16_t get_size_in_rx_buffer(w5500_t *dev)
{
    uint16_t received = read_addr(dev, REG_S0_RX_RSR0, REG_S0_RX_RSR1);
    uint16_t tmp = read_addr(dev, REG_S0_RX_RSR0, REG_S0_RX_RSR1);

    /* See Note in the description of Sn_RX_RSR in the datasheet: This is a 16 bit value,
       so we read it until we get the same value twice. The W5500 will update it
       while receiving data, when we read the same value again we can assume that
       the current packet is fully received. */
    while (received != tmp) {
        received = tmp;
        tmp = read_addr(dev, REG_S0_RX_RSR0, REG_S0_RX_RSR1);
    }

    return received;
}

static int receive(netdev_t *netdev, void *buf, size_t max_len, void *info)
{
    (void)info;
    w5500_t *dev = (w5500_t *)netdev;
    uint16_t packet_size = 0u;
    int ret_value = 0;

    spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);

    uint16_t available_bytes = get_size_in_rx_buffer(dev);

    if (available_bytes > 0u) {
        uint16_t socket0_read_pointer = read_addr(dev, REG_S0_RX_RD0, REG_S0_RX_RD1);
        /* I could not find a hint in the documentation of the W5500, but different implementations
           pointed me to the fact that the W5500 stores the size of the packet right in front of the
           packet data. */
        read_rx0_buffer(dev, socket0_read_pointer, (uint8_t*)&packet_size, 2u);
        packet_size = ntohs(packet_size) - 2;
        ret_value = packet_size;

        if (max_len > 0u) { /* max_len > 0u: Client wants to read or drop the packet. */
            if (packet_size > max_len) {
                buf = NULL; /* Drop the packet. */
                ret_value = -ENOBUFS;
            }

            socket0_read_pointer += 2u;  /* Add the 2 bytes of the packet_size. */
            if (buf != NULL) {
                read_rx0_buffer(dev, socket0_read_pointer, (uint8_t *)buf, packet_size);
                DEBUG("[w5500] receive: got packet of %i byte (at 0x%04x)\n", packet_size,
                      (int)socket0_read_pointer);
            }
            else {
                DEBUG("[w5500] receive: drop packet of %i byte (at 0x%04x)\n", packet_size,
                      (int)socket0_read_pointer);
            }
            socket0_read_pointer += packet_size;
        }

        /* Update read pointer. */
        write_address(dev, REG_S0_RX_RD0, REG_S0_RX_RD1, socket0_read_pointer);
        write_register(dev, REG_S0_CR, CR_RECV);
        while (read_register(dev, REG_S0_CR)) {}
    }

    spi_release(dev->p.spi);

    return ret_value;
}

static void isr(netdev_t *netdev)
{
    w5500_t *dev = (w5500_t *)netdev;

    spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);
    /* Get phy status register. */
    uint8_t phy_status = read_register(dev, REG_PHYCFGR);
    /* Get socket 0 interrupt register. */
    uint8_t socket0_interrupt_status = read_register(dev, REG_S0_IR);

    /* Clear interrupts. */
    write_register(dev, REG_S0_IR, socket0_interrupt_status);
    spi_release(dev->p.spi);

    if ((phy_status & PHY_LINK_UP) && (dev->link_up == false)) {
        dev->link_up = true;
        DEBUG("%s: link is up: PHYCFGR = 0x%02x\n", __func__, phy_status);
        netdev->event_callback(&dev->netdev, NETDEV_EVENT_LINK_UP);
    }
    else if (!(phy_status & PHY_LINK_UP) && (dev->link_up == true)) {
        dev->link_up = false;
        netdev->event_callback(&dev->netdev, NETDEV_EVENT_LINK_DOWN);
        DEBUG("%s: link is down: PHYCFGR = 0x%02x\n", __func__, phy_status);
    }

    while (socket0_interrupt_status & (IR_RECV | IR_SEND_OK)) {
        if (socket0_interrupt_status & IR_RECV) {
            DEBUG("[w5500] netdev RX complete\n");
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
        if (socket0_interrupt_status & IR_SEND_OK) {
            DEBUG("[w5500] netdev TX complete\n");
            dev->frame_sent = true;
            netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
        }

        /* Check interrupt status again. */
        spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);
        socket0_interrupt_status = read_register(dev, REG_S0_IR);
        write_register(dev, REG_S0_IR, socket0_interrupt_status);
        spi_release(dev->p.spi);
    }
}

static int get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    w5500_t *dev = (w5500_t *)netdev;
    int res = 0;

    switch (opt) {
    case NETOPT_ADDRESS:
        assert(max_len >= ETHERNET_ADDR_LEN);
        spi_acquire(dev->p.spi, dev->p.cs, SPI_CONF, dev->p.clk);
        read_chunk(dev, REG_SHAR0, value, ETHERNET_ADDR_LEN);
        spi_release(dev->p.spi);
        res = ETHERNET_ADDR_LEN;
        break;
    case NETOPT_LINK:
        *((netopt_enable_t *)value) = dev->link_up ? NETOPT_ENABLE
                                                   : NETOPT_DISABLE;
        res = sizeof(netopt_enable_t);
        break;
    default:
        res = netdev_eth_get(netdev, opt, value, max_len);
        break;
    }

    return res;
}

static const netdev_driver_t netdev_driver_w5500 = {
    .send = send,
    .recv = receive,
    .confirm_send = confirm_send,
    .init = init,
    .isr = isr,
    .get = get,
    .set = netdev_eth_set,
};
