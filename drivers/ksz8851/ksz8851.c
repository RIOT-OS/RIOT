/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ksz8851
 * @{
 *
 * @file
 * @brief       Device driver implementation for KSZ8851SNL devices
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "checksum/crc32.h"
#include "iolist.h"
#include "ksz8851.h"
#include "ksz8851_regs.h"
#include "net/ethernet.h"
#include "net/eui_provider.h"
#include "net/netdev/eth.h"
#include "time_units.h"
#include "ztimer.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define SPI_BUS                 (dev->p.spi)
#define CS_PIN                  (dev->p.cs)
#define SPI_MODE                SPI_MODE_0

/** @brief Interrupt sources enabled by this driver */
#define KSZ8851_IRQ_MASK \
    (IER_LCIE | IER_TXIE | IER_RXIE | IER_RXOIE)

/*
 * The first 4 bytes read back after the FIFO read opcode are always
 * dummy/undefined data and must be discarded, followed by 4 more bytes
 * which duplicate the frame's status word and byte count (already read
 * through the RXFHSR/RXFHBCR registers), see datasheet section 3.5.6.
 */
#define FIFO_RX_HEADER_LEN      (8)
/** Number of trailing CRC bytes included in the RX byte count */
#define RX_CRC_LEN              (4)
/** Frame header (control word + byte count) written ahead of TX payload */
#define TX_HEADER_LEN            (4)

#define RESET_DELAY_US          (10000U)
#define TXQCR_MAX_POLLS         (10000U)

static const netdev_driver_t netdev_driver_ksz8851;

static uint16_t reg_read(ksz8851_t *dev, uint8_t addr)
{
    uint8_t be = (addr & 0x02) ? BE_WORD_HIGH : BE_WORD_LOW;
    uint16_t cmd = (CMD_READ << 8) | ((uint16_t)be << 10) | ((uint16_t)addr << 2);
    uint8_t lo, hi;

    spi_transfer_u16_be(SPI_BUS, CS_PIN, true, cmd);
    lo = spi_transfer_byte(SPI_BUS, CS_PIN, true, 0);
    hi = spi_transfer_byte(SPI_BUS, CS_PIN, false, 0);

    return (uint16_t)lo | ((uint16_t)hi << 8);
}

static void reg_write(ksz8851_t *dev, uint8_t addr, uint16_t val)
{
    uint8_t be = (addr & 0x02) ? BE_WORD_HIGH : BE_WORD_LOW;
    uint16_t cmd = (CMD_WRITE << 8) | ((uint16_t)be << 10) | ((uint16_t)addr << 2);

    spi_transfer_u16_be(SPI_BUS, CS_PIN, true, cmd);
    spi_transfer_bytes(SPI_BUS, CS_PIN, false, &val, NULL, sizeof(val));
}

static void reg_set_bits(ksz8851_t *dev, uint8_t addr, uint16_t bits)
{
    reg_write(dev, addr, reg_read(dev, addr) | bits);
}

static void reg_clear_bits(ksz8851_t *dev, uint8_t addr, uint16_t bits)
{
    reg_write(dev, addr, reg_read(dev, addr) & ~bits);
}

/* Reset the QMU frame data pointer to the start of the next frame in RXQ */
static void rx_frame_pointer_reset(ksz8851_t *dev)
{
    reg_clear_bits(dev, REG_RXFDPR, RXFDPR_RXFP_MASK);
}

/* Discard the frame currently at the head of the RXQ */
static void rx_flush(ksz8851_t *dev)
{
    reg_clear_bits(dev, REG_RXCR1, RXCR1_RXE);
    reg_set_bits(dev, REG_RXCR1, RXCR1_FRXQ);
    reg_clear_bits(dev, REG_RXCR1, RXCR1_FRXQ);
    reg_set_bits(dev, REG_RXCR1, RXCR1_RXE);
}

static int fifo_read(ksz8851_t *dev, uint8_t *data, size_t len, size_t skip, size_t trailer)
{
    spi_transfer_byte(SPI_BUS, CS_PIN, true, CMD_FIFO_READ);
    while (skip--) {
        spi_transfer_byte(SPI_BUS, CS_PIN, true, 0);
    }
    spi_transfer_bytes(SPI_BUS, CS_PIN, trailer > 0, NULL, data, len);
    /* discard CRC and padding up to the double word boundary */
    while (trailer--) {
        spi_transfer_byte(SPI_BUS, CS_PIN, trailer > 0, 0);
    }

    return (int)len;
}

static void set_macaddr(ksz8851_t *dev, const uint8_t *mac)
{
    reg_write(dev, REG_MARH, ((uint16_t)mac[0] << 8) | mac[1]);
    reg_write(dev, REG_MARM, ((uint16_t)mac[2] << 8) | mac[3]);
    reg_write(dev, REG_MARL, ((uint16_t)mac[4] << 8) | mac[5]);
}

static void get_macaddr(ksz8851_t *dev, uint8_t *mac)
{
    uint16_t marh = reg_read(dev, REG_MARH);
    uint16_t marm = reg_read(dev, REG_MARM);
    uint16_t marl = reg_read(dev, REG_MARL);

    mac[0] = marh >> 8;
    mac[1] = marh & 0xff;
    mac[2] = marm >> 8;
    mac[3] = marm & 0xff;
    mac[4] = marl >> 8;
    mac[5] = marl & 0xff;
}

static void chip_reset(ksz8851_t *dev)
{
    reg_write(dev, REG_GRR, GRR_GLOBAL_SOFT_RESET);
    ztimer_sleep(ZTIMER_USEC, RESET_DELAY_US);
    reg_write(dev, REG_GRR, 0);
}

/* The chip indexes its 64 bit hash table with the upper 6 bits of the
 * big-endian Ethernet CRC32 of the destination address. crc32() yields the
 * bit-reversed form of that CRC with the final inversion applied, so the
 * wanted bits are the 6 lowest bits of ~crc32(), in reverse order. */
static uint8_t mcast_hash(const uint8_t *addr)
{
    uint32_t crc = ~crc32(addr, ETHERNET_ADDR_LEN);
    uint8_t hash = 0;

    for (unsigned i = 0; i < 6; i++, crc >>= 1) {
        hash = (hash << 1) | (crc & 1);
    }

    return hash;
}

static void write_mcast_table(ksz8851_t *dev)
{
    uint16_t table[4] = { 0 };

    for (unsigned i = 0; i < KSZ8851_MCAST_HASH_BINS; i++) {
        if (dev->mcast_refs[i]) {
            table[i / 16] |= 1U << (i % 16);
        }
    }

    for (unsigned i = 0; i < ARRAY_SIZE(table); i++) {
        reg_write(dev, REG_MAHTR0 + 2 * i, table[i]);
    }
}

static int mcast_update(ksz8851_t *dev, const uint8_t *addr, bool join)
{
    uint8_t hash = mcast_hash(addr);

    if (join) {
        if (dev->mcast_refs[hash] == UINT8_MAX) {
            return -ENOMEM;
        }
        if (dev->mcast_refs[hash]++) {
            /* bin already set */
            return 0;
        }
    }
    else {
        if (dev->mcast_refs[hash] == 0) {
            return -EINVAL;
        }
        if (--dev->mcast_refs[hash]) {
            /* bin still in use */
            return 0;
        }
    }

    spi_acquire(dev->p.spi, dev->p.cs, SPI_MODE, dev->p.clk);
    /* the hash table may only be modified while RX is stopped */
    reg_clear_bits(dev, REG_RXCR1, RXCR1_RXE);
    write_mcast_table(dev);
    reg_set_bits(dev, REG_RXCR1, RXCR1_RXE);
    spi_release(dev->p.spi);

    return 0;
}

static void irq_handler(void *arg)
{
    ksz8851_t *dev = arg;

    netdev_trigger_event_isr(&dev->netdev);
}

void ksz8851_setup(ksz8851_t *dev, const ksz8851_params_t *params, uint8_t index)
{
    dev->netdev.driver = &netdev_driver_ksz8851;
    dev->p = *params;
    dev->link_up = false;
    dev->frame_id = 0;
    memset(dev->mcast_refs, 0, sizeof(dev->mcast_refs));

    spi_init_cs(dev->p.spi, dev->p.cs);

    if (gpio_is_valid(dev->p.reset)) {
        gpio_init(dev->p.reset, GPIO_OUT);
        gpio_set(dev->p.reset);
    }

    if (gpio_is_valid(dev->p.irq)) {
        gpio_init_int(dev->p.irq, GPIO_IN, GPIO_FALLING, irq_handler, dev);
    }

    netdev_register(&dev->netdev, NETDEV_KSZ8851, index);
}

static int _init(netdev_t *netdev)
{
    ksz8851_t *dev = (ksz8851_t *)netdev;
    uint16_t cider;
    uint8_t mac[ETHERNET_ADDR_LEN];

    if (gpio_is_valid(dev->p.reset)) {
        gpio_clear(dev->p.reset);
        ztimer_sleep(ZTIMER_USEC, 10);
        gpio_set(dev->p.reset);
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    }

    spi_acquire(dev->p.spi, dev->p.cs, SPI_MODE, dev->p.clk);

    /* Also perform a soft reset through the register interface, in case
     * the reset line is not wired up or the chip did not fully settle
     * after a power-on reset. */
    chip_reset(dev);

    cider = reg_read(dev, REG_CIDER);
    if ((cider & CIDER_CHIP_ID_MASK) != CIDER_FAMILY_ID) {
        spi_release(dev->p.spi);
        DEBUG("[ksz8851] init: unexpected chip ID 0x%04x\n", cider);
        return -ENODEV;
    }

    netdev_eui48_get(netdev, (eui48_t *)mac);
    set_macaddr(dev, mac);

    reg_write(dev, REG_TXFDPR, TXFDPR_TXFPAI);
    reg_write(dev, REG_TXCR, TXCR_TXFCE | TXCR_TXPE | TXCR_TXCE);

    reg_write(dev, REG_RXFDPR, RXFDPR_RXFPAI);
    /* Hash perfect filtering: unicast must match the MAC address,
     * multicast must match the hash table */
    write_mcast_table(dev);
    reg_write(dev, REG_RXCR1,
              RXCR1_RXUDPFCC | RXCR1_RXTCPFCC | RXCR1_RXIPFCC | RXCR1_RXPAFMA | RXCR1_RXFCE |
                  RXCR1_RXBE | RXCR1_RXME | RXCR1_RXUE);
    reg_write(dev, REG_RXCR2,
              RXCR2_SRDBL2 | RXCR2_IUFFP | RXCR2_RXIUFCEZ | RXCR2_UDPLFE | RXCR2_RXICMPFCC);
    reg_write(dev, REG_RXQCR, RXQCR_RXFCTE | RXQCR_ADRFE);

    /* Generate a RX interrupt as soon as a single frame is received */
    reg_write(dev, REG_RXFCTR, 1);

    reg_write(dev, REG_ISR, ISR_CLEAR_ALL);
    reg_write(dev, REG_IER, KSZ8851_IRQ_MASK);

    reg_set_bits(dev, REG_TXCR, TXCR_TXE);
    reg_set_bits(dev, REG_RXCR1, RXCR1_RXE);

    /* Sync the cached link state with the current hardware state, since
     * the link-change interrupt only fires on a transition and may never
     * trigger if the link is already up by the time IER is configured. */
    dev->link_up = reg_read(dev, REG_P1SR) & P1SR_LINK_GOOD;

    spi_release(dev->p.spi);

    DEBUG("[ksz8851] init: done\n");

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    ksz8851_t *dev = (ksz8851_t *)netdev;
    size_t len = iolist_size(iolist);
    uint8_t header[TX_HEADER_LEN];
    uint16_t txmir;
    unsigned pad;
    int ret = (int)len;

    /* TXQ writes must end on a double word boundary */
    pad = (4 - ((TX_HEADER_LEN + len) % 4)) % 4;

    spi_acquire(dev->p.spi, dev->p.cs, SPI_MODE, dev->p.clk);

    txmir = reg_read(dev, REG_TXMIR);
    if ((txmir & TXMIR_TXMA_MASK) < TX_HEADER_LEN + len + pad) {
        DEBUG("[ksz8851] send: TX queue full, dropping frame\n");
        spi_release(dev->p.spi);
        return -EBUSY;
    }

    /* Control word: no TX-done interrupt, frame ID for TXSR bookkeeping */
    header[0] = dev->frame_id++ & 0x3f;
    header[1] = 0;
    header[2] = len & 0xff;
    header[3] = (len >> 8) & 0xff;

    reg_set_bits(dev, REG_RXQCR, RXQCR_SDA);

    spi_transfer_byte(SPI_BUS, CS_PIN, true, CMD_FIFO_WRITE);
    spi_transfer_bytes(SPI_BUS, CS_PIN, true, header, NULL, sizeof(header));
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        bool cont = (iol->iol_next != NULL) || pad;
        spi_transfer_bytes(SPI_BUS, CS_PIN, cont, iol->iol_base, NULL, iol->iol_len);
    }
    uint32_t dummy = 0;
    spi_transfer_bytes(SPI_BUS, CS_PIN, false, &dummy, NULL, pad);

    reg_clear_bits(dev, REG_RXQCR, RXQCR_SDA);
    reg_set_bits(dev, REG_TXQCR, TXQCR_METFE);

    unsigned retries = TXQCR_MAX_POLLS;
    while (reg_read(dev, REG_TXQCR) & TXQCR_METFE) {
        if (--retries == 0) {
            DEBUG("[ksz8851] send: TX enqueue timeout\n");
            ret = -EIO;
            break;
        }
    }

    spi_release(dev->p.spi);

    return ret;
}

static int _recv(netdev_t *netdev, void *buf, size_t max_len, void *info)
{
    ksz8851_t *dev = (ksz8851_t *)netdev;
    uint16_t rxfhsr, rxfhbcr;
    size_t frame_len;
    int ret;

    (void)info;

    spi_acquire(dev->p.spi, dev->p.cs, SPI_MODE, dev->p.clk);

    rxfhsr = reg_read(dev, REG_RXFHSR);
    if (!(rxfhsr & RXFHSR_RXFV) || (rxfhsr & RXFHSR_RX_ERRORS)) {
        DEBUG("[ksz8851] recv: dropping bad RX frame (RXFHSR=0x%04x)\n", rxfhsr);
        rx_flush(dev);
        spi_release(dev->p.spi);
        return 0;
    }

    rxfhbcr = reg_read(dev, REG_RXFHBCR) & RXFHBCR_RXBC_MASK;
    if (rxfhbcr <= RX_CRC_LEN) {
        rx_flush(dev);
        spi_release(dev->p.spi);
        return 0;
    }

    frame_len = rxfhbcr - RX_CRC_LEN;

    if (buf == NULL) {
        if (max_len > 0) {
            /* caller asked us to drop the pending frame */
            rx_flush(dev);
        }
        spi_release(dev->p.spi);
        return (int)frame_len;
    }

    if (frame_len > max_len) {
        rx_flush(dev);
        spi_release(dev->p.spi);
        return -ENOBUFS;
    }

    rx_frame_pointer_reset(dev);
    reg_set_bits(dev, REG_RXQCR, RXQCR_SDA);
    ret = fifo_read(dev, buf, frame_len, FIFO_RX_HEADER_LEN, ((rxfhbcr + 3) & ~3) - frame_len);
    reg_clear_bits(dev, REG_RXQCR, RXQCR_SDA);

    spi_release(dev->p.spi);

    return (ret < 0) ? ret : (int)frame_len;
}

static void update_link_status(ksz8851_t *dev)
{
    uint16_t p1sr;
    bool link_up;

    p1sr = reg_read(dev, REG_P1SR);
    link_up = p1sr & P1SR_LINK_GOOD;

    if (link_up == dev->link_up) {
        return;
    }

    dev->link_up = link_up;
    dev->netdev.event_callback(&dev->netdev,
                               link_up ? NETDEV_EVENT_LINK_UP : NETDEV_EVENT_LINK_DOWN);
}

static void _isr(netdev_t *netdev)
{
    ksz8851_t *dev = (ksz8851_t *)netdev;
    uint16_t isr, rxfctr;
    uint8_t frame_count;

    /* INTRN is level-based: keep servicing until ISR reads back as zero,
     * otherwise events latched while handling the previous ones keep the
     * line asserted and no further falling edge will be seen. */
    while (1) {
        spi_acquire(dev->p.spi, dev->p.cs, SPI_MODE, dev->p.clk);

        isr = reg_read(dev, REG_ISR) & KSZ8851_IRQ_MASK;
        if (isr == 0) {
            spi_release(dev->p.spi);
            break;
        }
        reg_write(dev, REG_ISR, isr);

        frame_count = 0;
        if (isr & IER_RXIE) {
            rxfctr = reg_read(dev, REG_RXFCTR);
            frame_count = (uint8_t)(rxfctr >> 8);
        }

        if (isr & IER_RXOIE) {
            DEBUG("[ksz8851] isr: RX overrun\n");
        }

        if (isr & IER_LCIE) {
            update_link_status(dev);
        }

        spi_release(dev->p.spi);

        /* netdev->event_callback() re-enters the driver synchronously
         * (recv() is called before this returns), so it is safe to keep
         * using the frame count sampled above: each iteration consumes
         * exactly one frame from the hardware queue. */
        while (frame_count--) {
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    ksz8851_t *dev = (ksz8851_t *)netdev;
    int res;

    switch (opt) {
    case NETOPT_ADDRESS:
        if (max_len < ETHERNET_ADDR_LEN) {
            return -EINVAL;
        }
        spi_acquire(dev->p.spi, dev->p.cs, SPI_MODE, dev->p.clk);
        get_macaddr(dev, value);
        spi_release(dev->p.spi);
        return ETHERNET_ADDR_LEN;
    case NETOPT_LINK:
        spi_acquire(dev->p.spi, dev->p.cs, SPI_MODE, dev->p.clk);
        res = reg_read(dev, REG_P1SR) & P1SR_LINK_GOOD;
        spi_release(dev->p.spi);
        *((netopt_enable_t *)value) = res ? NETOPT_ENABLE : NETOPT_DISABLE;
        return sizeof(netopt_enable_t);
    default:
        res = netdev_eth_get(netdev, opt, value, max_len);
        break;
    }

    return res;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len)
{
    ksz8851_t *dev = (ksz8851_t *)netdev;
    int res;

    switch (opt) {
    case NETOPT_ADDRESS:
        if (value_len != ETHERNET_ADDR_LEN) {
            return -EINVAL;
        }
        spi_acquire(dev->p.spi, dev->p.cs, SPI_MODE, dev->p.clk);
        set_macaddr(dev, value);
        spi_release(dev->p.spi);
        return ETHERNET_ADDR_LEN;
    case NETOPT_L2_GROUP:
    case NETOPT_L2_GROUP_LEAVE:
        if (value_len != ETHERNET_ADDR_LEN) {
            return -EINVAL;
        }
        res = mcast_update(dev, value, opt == NETOPT_L2_GROUP);
        return (res < 0) ? res : ETHERNET_ADDR_LEN;
    default:
        return netdev_eth_set(netdev, opt, value, value_len);
    }
}

static const netdev_driver_t netdev_driver_ksz8851 = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};
