/*
 * Copyright (C) 2015 Ell-i open source co-operative
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_encx24j600
 * @{
 *
 * @file
 * @brief       Internal functions for the ENCX24J600 driver
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "mutex.h"
#include "encx24j600.h"
#include "encx24j600_internal.h"
#include "encx24j600_defines.h"
#include "xtimer.h"

#include "net/netdev.h"
#include "net/netdev/eth.h"
#include "net/eui64.h"
#include "net/ethernet.h"

#ifdef MODULE_NETSTATS_L2
#include <string.h>
#include "net/netstats.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SPI_CLK                 SPI_CLK_1MHZ
#define SPI_MODE                SPI_MODE_0

#define ENCX24J600_INIT_DELAY   (100000U)

#define ENC_BUFFER_START        (0x0000)
#define ENC_BUFFER_SIZE         (0x6000)
#define ENC_BUFFER_END          (0x5FFF)
#define RX_BUFFER_START         (0x5340) /* Default value */
#define RX_BUFFER_END           (ENC_BUFFER_END)
#define TX_BUFFER_LEN           (0x2000)
#define TX_BUFFER_END           (RX_BUFFER_START)
#define TX_BUFFER_START         (TX_BUFFER_END - TX_BUFFER_LEN)

static void cmd(encx24j600_t *dev, char cmd);
static void reg_set(encx24j600_t *dev, uint8_t reg, uint16_t value);
static uint16_t reg_get(encx24j600_t *dev, uint8_t reg);
static void reg_clear_bits(encx24j600_t *dev, uint8_t reg, uint16_t mask);
static inline int _packets_available(encx24j600_t *dev);

static void _get_mac_addr(netdev_t *dev, uint8_t* buf);

/* netdev interface */
static int _send(netdev_t *netdev, const iolist_t *iolist);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev_t *dev);
static void _isr(netdev_t *dev);
static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len);

static const netdev_driver_t netdev_driver_encx24j600 = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = netdev_eth_set,
};

static inline void lock(encx24j600_t *dev) {
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
}

static inline void unlock(encx24j600_t *dev) {
    spi_release(dev->spi);
}

void encx24j600_setup(encx24j600_t *dev, const encx24j600_params_t *params)
{
    dev->netdev.driver = &netdev_driver_encx24j600;
    dev->spi = params->spi;
    dev->cs = params->cs_pin;
    dev->int_pin = params->int_pin;
    dev->rx_next_ptr = RX_BUFFER_START;
}

static void encx24j600_isr(void *arg)
{
    encx24j600_t *dev = (encx24j600_t *) arg;

    /* disable interrupt line */
    gpio_irq_disable(dev->int_pin);

    /* call netdev hook */
    dev->netdev.event_callback((netdev_t*) dev, NETDEV_EVENT_ISR);
}

static void _isr(netdev_t *netdev)
{
    encx24j600_t *dev = (encx24j600_t *) netdev;

    uint16_t eir;

    lock(dev);
    cmd(dev, ENC_CLREIE);

    eir = reg_get(dev, ENC_EIR);

    /* check & handle link state change */
    if (eir & ENC_LINKIF) {
        uint16_t estat = reg_get(dev, ENC_ESTAT);

        netdev_event_t event = (estat & ENC_PHYLNK) ?
            NETDEV_EVENT_LINK_DOWN :
            NETDEV_EVENT_LINK_UP;

        netdev->event_callback(netdev, event);
    }

    /* check & handle available packets */
    if (eir & ENC_PKTIF) {
        while (_packets_available(dev)) {
            unlock(dev);
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
            lock(dev);
        }
    }

    /* drop all flags */
    reg_clear_bits(dev, ENC_EIR, ENC_LINKIF);

    /* re-enable interrupt */
    gpio_irq_enable(dev->int_pin);
    cmd(dev, ENC_SETEIE);

    unlock(dev);
}

static inline void enc_spi_transfer(encx24j600_t *dev, char *out, char *in, int len)
{
    spi_transfer_bytes(dev->spi, dev->cs, false, out, in, len);
}

static inline uint16_t reg_get(encx24j600_t *dev, uint8_t reg)
{
    char cmd[4] = { ENC_RCRU, reg, 0, 0 };
    char result[4];

    enc_spi_transfer(dev, cmd, result, 4);

    return result[2] | (result[3] << 8);
}

static void phy_reg_set(encx24j600_t *dev, uint8_t reg, uint16_t value) {
    reg_set(dev, ENC_MIREGADR, reg | (1<<8));
    reg_set(dev, ENC_MIWR, value);
}

static void cmd(encx24j600_t *dev, char cmd) {
    spi_transfer_byte(dev->spi, dev->cs, false, (uint8_t)cmd);
}

static void cmdn(encx24j600_t *dev, uint8_t cmd, char *out, char *in, int len) {
    spi_transfer_byte(dev->spi, dev->cs, true, cmd);
    spi_transfer_bytes(dev->spi, dev->cs, false, out, in, len);
}

static void reg_set(encx24j600_t *dev, uint8_t reg, uint16_t value)
{
    char cmd[4] = { ENC_WCRU, reg, value, value >> 8 };
    enc_spi_transfer(dev, cmd, NULL, 4);
}

static void reg_set_bits(encx24j600_t *dev, uint8_t reg, uint16_t mask)
{
    char cmd[4] = { ENC_BFSU, reg, mask, mask >> 8 };
    enc_spi_transfer(dev, cmd, NULL, 4);
}

static void reg_clear_bits(encx24j600_t *dev, uint8_t reg, uint16_t mask)
{
    char cmd[4] = { ENC_BFCU, reg, mask, mask >> 8 };
    enc_spi_transfer(dev, cmd, NULL, 4);
}

/*
 * @brief Read/Write to encx24j600's SRAM
 *
 * @param[in] dev   ptr to encx24j600 device handle
 * @param[in] cmd   either ENC_WGPDATA, ENC_RGPDATA, ENC_WRXDATA, ENC_RRXDATA, ENC_WUDADATA, ENC_RUDADATA
 * @param[in] addr  SRAM address to start reading. 0xFFFF means continue from old address
 * @param     ptr   pointer to buffer to read from / write to
 * @param[in] len   nr of bytes to read/write
 */
static void sram_op(encx24j600_t *dev, uint16_t cmd, uint16_t addr, char *ptr, int len)
{
    uint16_t reg;
    char* in = NULL;
    char* out = NULL;

    /* determine pointer addr
     *
     * all SRAM access commands have an
     * offset 0x5e to their pointer registers
     * */
    reg = cmd + 0x5e;

    /* read or write? bit 1 tells us */
    if (reg & 0x2) {
        out = ptr;
    } else {
        in = ptr;
    }

    /* set pointer */
    if (addr != 0xFFFF) {
        reg_set(dev, reg, addr);
    }

    /* copy data */
    cmdn(dev, cmd, in, out, len);
}

static int _init(netdev_t *encdev)
{
    encx24j600_t *dev = (encx24j600_t *) encdev;

    DEBUG("encx24j600: starting initialization...\n");

    /* setup IO */
    if (spi_init_cs(dev->spi, dev->cs) != SPI_OK) {
        return -1;
    }
    gpio_init_int(dev->int_pin, GPIO_IN_PU, GPIO_FALLING, encx24j600_isr, (void*)dev);

    lock(dev);

    /* initialization procedure as described in data sheet (39935c.pdf) */
    do {
        do {
            xtimer_usleep(ENCX24J600_INIT_DELAY);
            reg_set(dev, ENC_EUDAST, 0x1234);
            xtimer_usleep(ENCX24J600_INIT_DELAY);
        } while (reg_get(dev, ENC_EUDAST) != 0x1234);

        while (!(reg_get(dev, ENC_ESTAT) & ENC_CLKRDY)) {}

        /* issue System Reset */
        cmd(dev, ENC_SETETHRST);

        /* make sure initialization finalizes */
        xtimer_usleep(1000);
    } while (!(reg_get(dev, ENC_EUDAST) == 0x0000));

    /* configure flow control */
    phy_reg_set(dev, ENC_PHANA, 0x05E1);
    reg_set_bits(dev, ENC_ECON2, ENC_AUTOFC);

    /* setup receive buffer */
    reg_set(dev, ENC_ERXST, RX_BUFFER_START);
    reg_set(dev, ENC_ERXTAIL, RX_BUFFER_END);
    dev->rx_next_ptr = RX_BUFFER_START;

    /* configure receive filter to receive multicast frames */
    reg_set_bits(dev, ENC_ERXFCON, ENC_MCEN);

    /* setup interrupts */
    reg_set_bits(dev, ENC_EIE, ENC_PKTIE | ENC_LINKIE);
    cmd(dev, ENC_ENABLERX);
    cmd(dev, ENC_SETEIE);

    DEBUG("encx24j600: initialization complete.\n");

    unlock(dev);

#ifdef MODULE_NETSTATS_L2
    memset(&encdev->stats, 0, sizeof(netstats_t));
#endif
    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist) {
    encx24j600_t * dev = (encx24j600_t *) netdev;
    lock(dev);

    /* wait until previous packet has been sent */
    while ((reg_get(dev, ENC_ECON1) & ENC_TXRTS)) {}

    /* copy packet to SRAM */
    size_t len = 0;

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        sram_op(dev, ENC_WGPDATA, ((iol == iolist) ? TX_BUFFER_START : 0xFFFF), iol->iol_base, iol->iol_len);
        len += iol->iol_len;
    }

    /* set start of TX packet and length */
    reg_set(dev, ENC_ETXST, TX_BUFFER_START);
    reg_set(dev, ENC_ETXLEN, len);

    /* initiate sending */
    cmd(dev, ENC_SETTXRTS);

    /* wait for sending to complete */
    /* (not sure if it is needed, keeping the line uncommented) */
    /*while ((reg_get(dev, ENC_ECON1) & ENC_TXRTS)) {}*/

#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += len;
#endif

    unlock(dev);

    return len;
}

static inline int _packets_available(encx24j600_t *dev)
{
    /* return ENC_PKTCNT (low byte of ENC_ESTAT) */
    return reg_get(dev, ENC_ESTAT) & ~0xFF00;
}

static void _get_mac_addr(netdev_t *encdev, uint8_t* buf)
{
    encx24j600_t * dev = (encx24j600_t *) encdev;
    uint16_t *addr = (uint16_t *) buf;

    lock(dev);

    addr[0] = reg_get(dev, ENC_MAADR1);
    addr[1] = reg_get(dev, ENC_MAADR2);
    addr[2] = reg_get(dev, ENC_MAADR3);

    unlock(dev);
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    encx24j600_t * dev = (encx24j600_t *) netdev;
    encx24j600_frame_hdr_t hdr;

    (void)info;
    lock(dev);

    /* read frame header */
    sram_op(dev, ENC_RRXDATA, dev->rx_next_ptr, (char*)&hdr, sizeof(hdr));

    /* hdr.frame_len given by device contains 4 bytes checksum */
    size_t payload_len = hdr.frame_len - 4;


    if (buf) {
        if (payload_len > len) {
            /* payload exceeds buffer size */
            unlock(dev);
            return -ENOBUFS;
        }
#ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += payload_len;
#endif
        /* read packet (without 4 bytes checksum) */
        sram_op(dev, ENC_RRXDATA, 0xFFFF, buf, payload_len);

        /* decrement available packet count */
        cmd(dev, ENC_SETPKTDEC);

        dev->rx_next_ptr = hdr.rx_next_ptr;

        reg_set(dev, ENC_ERXTAIL, dev->rx_next_ptr - 2);
    }

    unlock(dev);

    return payload_len;
}

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = 0;

    switch (opt) {
        case NETOPT_ADDRESS:
            if (max_len < ETHERNET_ADDR_LEN) {
                res = -EINVAL;
            }
            else {
                _get_mac_addr(dev, (uint8_t*)value);
                res = ETHERNET_ADDR_LEN;
            }
            break;
        case NETOPT_LINK_CONNECTED:
            if (reg_get((encx24j600_t *)dev, ENC_ESTAT) & ENC_PHYLNK) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);
        default:
            res = netdev_eth_get(dev, opt, value, max_len);
            break;
    }

    return res;
}
