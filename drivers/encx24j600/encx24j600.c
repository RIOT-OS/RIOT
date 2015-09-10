/*
 * Copyright (C) 2015 Ell-i open source co-operative
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_encx24j600
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
#include "vtimer.h"

#include "net/netdev2.h"
#include "net/eui64.h"
#include "net/ethernet.h"
//#include "net/ethernet/hdr.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define ENCX24J600_SPI_SPEED SPI_SPEED_1MHZ

#define ENCX24J600_INIT_DELAY 100000U

#define ENC_BUFFER_START 0x0000
#define ENC_BUFFER_SIZE  0x6000
#define ENC_BUFFER_END   0x5FFF
#define RX_BUFFER_START (0x5340) /* Default value */
#define RX_BUFFER_END   (ENC_BUFFER_END)
#define TX_BUFFER_LEN   (0x2000)
#define TX_BUFFER_END   (RX_BUFFER_START)
#define TX_BUFFER_START (TX_BUFFER_END - TX_BUFFER_LEN)

static void cmd(encx24j600_t *dev, char cmd);
static void reg_set(encx24j600_t *dev, uint8_t reg, uint16_t value);
static uint16_t reg_get(encx24j600_t *dev, uint8_t reg);
static void reg_clear_bits(encx24j600_t *dev, uint8_t reg, uint16_t mask);
static inline int _packets_available(encx24j600_t *dev);

static int _get_iid(netdev2_t *netdev, eui64_t *value, size_t max_len);
static void _get_mac_addr(netdev2_t *dev, uint8_t* buf);

/* netdev2 interface */
static int _send(netdev2_t *netdev, const struct iovec *vector, int count);
static int _recv(netdev2_t *netdev, char* buf, int len);
static int _init(netdev2_t *dev);
static void _isr(netdev2_t *dev);
int _get(netdev2_t *dev, netopt_t opt, void *value, size_t max_len);
int _set(netdev2_t *dev, netopt_t opt, void *value, size_t value_len);

const static netdev2_driver_t netdev2_driver_encx24j600 = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static inline void lock(encx24j600_t *dev) {
    mutex_lock(&dev->mutex);
}

static inline void unlock(encx24j600_t *dev) {
    mutex_unlock(&dev->mutex);
}

void encx24j600_setup(encx24j600_t *dev, spi_t spi, gpio_t cs, gpio_t int_pin)
{
    dev->netdev.driver = &netdev2_driver_encx24j600;
    dev->spi = spi;
    dev->cs = cs;
    dev->int_pin = int_pin;
    dev->rx_next_ptr = RX_BUFFER_START;

    mutex_init(&dev->mutex);
}

static void encx24j600_isr(void *arg)
{
    encx24j600_t *dev = (encx24j600_t *) arg;

    /* disable interrupt line */
    gpio_irq_disable(dev->int_pin);

    /* call netdev2 hook */
    dev->netdev.event_callback((netdev2_t*) dev, NETDEV2_EVENT_ISR, NULL);
}

static void _isr(netdev2_t *netdev)
{
    encx24j600_t *dev = (encx24j600_t *) netdev;

    uint16_t eir;

    lock(dev);
    cmd(dev, CLREIE);

    eir = reg_get(dev, EIR);

    /* check & handle link state change */
    if (eir & LINKIF) {
        uint16_t estat = reg_get(dev, ESTAT);

        netdev2_event_t event = (estat & PHYLNK) ?
            NETDEV2_EVENT_LINK_DOWN :
            NETDEV2_EVENT_LINK_UP;

        netdev->event_callback(netdev, event, NULL);
    }

    /* check & handle available packets */
    if (eir & PKTIF) {
        while (_packets_available(dev)) {
            unlock(dev);
            netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE,
                    NULL);
            lock(dev);
        }
    }

    /* drop all flags */
    reg_clear_bits(dev, EIR, LINKIF);

    /* re-enable interrupt */
    gpio_irq_enable(dev->int_pin);
    cmd(dev, SETEIE);

    unlock(dev);
}

static inline void enc_spi_transfer(encx24j600_t *dev, char *out, char *in, int len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs);
    spi_transfer_bytes(dev->spi, out, in, len);
    gpio_set(dev->cs);
    spi_release(dev->spi);
}

static inline uint16_t reg_get(encx24j600_t *dev, uint8_t reg)
{
    char cmd[4] = { RCRU, reg, 0, 0 };
    char result[4];

    enc_spi_transfer(dev, cmd, result, 4);

    return result[2] | (result[3] << 8);
}

static void phy_reg_set(encx24j600_t *dev, uint8_t reg, uint16_t value) {
    reg_set(dev, MIREGADR, reg | (1<<8));
    reg_set(dev, MIWR, value);
}

static void cmd(encx24j600_t *dev, char cmd) {
    spi_acquire(dev->spi);
    gpio_clear(dev->cs);
    spi_transfer_byte(dev->spi, cmd, NULL);
    gpio_set(dev->cs);
    spi_release(dev->spi);
}

static void cmdn(encx24j600_t *dev, uint8_t cmd, char *out, char *in, int len) {
    spi_acquire(dev->spi);
    gpio_clear(dev->cs);
    spi_transfer_byte(dev->spi, cmd, NULL);
    spi_transfer_bytes(dev->spi, out, in, len);
    gpio_set(dev->cs);
    spi_release(dev->spi);
}

static void reg_set(encx24j600_t *dev, uint8_t reg, uint16_t value)
{
    char cmd[4] = { WCRU, reg, value, value >> 8 };
    enc_spi_transfer(dev, cmd, NULL, 4);
}

static void reg_set_bits(encx24j600_t *dev, uint8_t reg, uint16_t mask)
{
    char cmd[4] = { BFSU, reg, mask, mask >> 8 };
    enc_spi_transfer(dev, cmd, NULL, 4);
}

static void reg_clear_bits(encx24j600_t *dev, uint8_t reg, uint16_t mask)
{
    char cmd[4] = { BFCU, reg, mask, mask >> 8 };
    enc_spi_transfer(dev, cmd, NULL, 4);
}

/*
 * @brief Read/Write to encx24j600's SRAM
 *
 * @param[in] dev   ptr to encx24j600 device handle
 * @param[in] cmd   either WGPDATA, RGPDATA, WRXDATA, RRXDATA, WUDADATA, RUDADATA
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

static int _init(netdev2_t *encdev)
{
    encx24j600_t *dev = (encx24j600_t *) encdev;

    DEBUG("encx24j600: starting initialization...\n");

    /* setup IO */
    gpio_init(dev->cs, GPIO_DIR_OUT, GPIO_PULLUP);
    gpio_set(dev->cs);
    gpio_init_int(dev->int_pin, GPIO_PULLUP, GPIO_FALLING, encx24j600_isr, (void*)dev);

    if (spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, ENCX24J600_SPI_SPEED) < 0) {
        return -1;
    }

    lock(dev);

    /* initialization procedure as described in data sheet (39935c.pdf) */
    do {
        do {
            vtimer_usleep(ENCX24J600_INIT_DELAY);
            reg_set(dev, EUDAST, 0x1234);
            vtimer_usleep(ENCX24J600_INIT_DELAY);
        } while (reg_get(dev, EUDAST) != 0x1234);

        while (!(reg_get(dev, ESTAT) & CLKRDY));

        /* issue System Reset */
        cmd(dev, SETETHRST);

        /* make sure initialization finalizes */
        vtimer_usleep(1000);
    } while (!(reg_get(dev, EUDAST) == 0x0000));

    /* configure flow control */
    phy_reg_set(dev, PHANA, 0x05E1);
    reg_set_bits(dev, ECON2, AUTOFC);

    /* setup receive buffer */
    reg_set(dev, ERXST, RX_BUFFER_START);
    reg_set(dev, ERXTAIL, RX_BUFFER_END);
    dev->rx_next_ptr = RX_BUFFER_START;

    /* configure receive filter to receive multicast frames */
    reg_set_bits(dev, ERXFCON, MCEN);

    /* setup interrupts */
    reg_set_bits(dev, EIE, PKTIE | LINKIE);
    cmd(dev, ENABLERX);
    cmd(dev, SETEIE);

    DEBUG("encx24j600: initialization complete.\n");

    unlock(dev);

    return 0;
}

static int _send(netdev2_t *netdev, const struct iovec *vector, int count) {
    encx24j600_t * dev = (encx24j600_t *) netdev;
    lock(dev);

    /* wait until previous packet has been sent */
    while ((reg_get(dev, ECON1) & TXRTS));

    /* copy packet to SRAM */
    size_t len = 0;

    for (int i = 0; i < count; i++) {
        sram_op(dev, WGPDATA, (i ? 0xFFFF : TX_BUFFER_START), vector[i].iov_base, vector[i].iov_len);
        len += vector[i].iov_len;
    }

    /* set start of TX packet and length */
    reg_set(dev, ETXST, TX_BUFFER_START);
    reg_set(dev, ETXLEN, len);

    /* initiate sending */
    cmd(dev, SETTXRTS);

    /* wait for sending to complete */
    /* (not sure if it is needed, keeping the line uncommented) */
    /*while ((reg_get(dev, ECON1) & TXRTS));*/

    unlock(dev);

    return len;
}

static inline int _packets_available(encx24j600_t *dev)
{
    /* return PKTCNT (low byte of ESTAT) */
    return reg_get(dev, ESTAT) & ~0xFF00;
}

static void _get_mac_addr(netdev2_t *encdev, uint8_t* buf)
{
    encx24j600_t * dev = (encx24j600_t *) encdev;
    uint16_t *addr = (uint16_t *) buf;

    lock(dev);

    addr[0] = reg_get(dev, MAADR1);
    addr[1] = reg_get(dev, MAADR2);
    addr[2] = reg_get(dev, MAADR3);

    unlock(dev);
}

static int _recv(netdev2_t *netdev, char* buf, int len)
{
    encx24j600_t * dev = (encx24j600_t *) netdev;
    encx24j600_frame_hdr_t hdr;

    lock(dev);

    /* read frame header */
    sram_op(dev, RRXDATA, dev->rx_next_ptr, (char*)&hdr, sizeof(hdr));

    /* hdr.frame_len given by device contains 4 bytes checksum */
    size_t payload_len = hdr.frame_len - 4;

    if (buf) {
        /* read packet (without 4 bytes checksum) */
        sram_op(dev, RRXDATA, 0xFFFF, buf, payload_len);

        /* decrement available packet count */
        cmd(dev, SETPKTDEC);

        dev->rx_next_ptr = hdr.rx_next_ptr;

        reg_set(dev, ERXTAIL, dev->rx_next_ptr - 2);
    }

    unlock(dev);

    return payload_len;
}

static int _get_iid(netdev2_t *netdev, eui64_t *value, size_t max_len)
{
    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    uint8_t addr[ETHERNET_ADDR_LEN];
    _get_mac_addr(netdev, addr);
    ethernet_get_iid(value, addr);

    return sizeof(eui64_t);
}

int _get(netdev2_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = 0;

    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            {
               uint16_t *tgt = (uint16_t *)value;
                *tgt = NETDEV2_TYPE_ETHERNET;
                res = 2;
                break;
            }
        case NETOPT_ADDRESS:
            if (max_len < ETHERNET_ADDR_LEN) {
                res = -EINVAL;
            }
            else {
                _get_mac_addr(dev, (uint8_t*)value);
                res = ETHERNET_ADDR_LEN;
            }
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(max_len == 2);
            uint16_t *tgt = (uint16_t*)value;
            *tgt=6;
            res = sizeof(uint16_t);
            break;
        case NETOPT_IPV6_IID:
            return _get_iid(dev, value, max_len);
        default:
            res = -ENOTSUP;
            break;
    }

    return res;
}

int _set(netdev2_t *dev, netopt_t opt, void *value, size_t value_len)
{
    int res = 0;

    switch (opt) {
        default:
            return -ENOTSUP;
    }

    return res;
}
