/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_greth
 * @{
 *
 * @file
 * @brief       Gaisler GRETH Ethernet MAC netdev driver
 *
 * Legacy-mode netdev driver (confirm_send = NULL): send() blocks until DMA
 * completes and returns the byte count directly.
 *
 * TX/RX descriptor tables and data buffers are file-scope static so
 * they can carry their own alignment attribute without propagating it into greth_t
 * (which would trigger -Wcast-align when casting netdev_t* → greth_t*).
 *
 * @author      Matvii Ivashchenko
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdbool.h>
#include <string.h>

#include "net/netdev/eth.h"
#include "net/ethernet.h"
#include "iolist.h"

#include "log.h"
#include "plic.h"

#include "greth.h"
#include "greth_regs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/* GRLIB requirement: descriptor table base must be aligned to at least the
 * table byte size (128 descriptors × 8 bytes = 1024 bytes). */
static alignas(1024) greth_desc_t _tx_desc[CONFIG_GRETH_TX_DESC_NUM];

static alignas(1024) greth_desc_t _rx_desc[CONFIG_GRETH_RX_DESC_NUM];

static uint8_t _tx_buf[GRETH_BUF_SIZE];
static uint8_t _rx_buf[CONFIG_GRETH_RX_DESC_NUM][GRETH_BUF_SIZE];

/*
 * D-cache coherency (Zicbom extension)
 *
 * NOEL-V has a write-back D-cache that is NOT coherent with the GRETH DMA
 * (AHB master). Without explicit cache management:
 *   - CPU writes to descriptors/buffers stay in dirty lines; DMA reads stale
 *     zeros from DRAM → TX never starts.
 *   - DMA writes frames to DRAM; CPU reads stale cache → RX frame corrupted.
 *
 * cbo.flush — write dirty lines to DRAM before DMA reads (TX path).
 * cbo.inval — discard cached copy before CPU reads DMA-written data (RX path).
 */
#define GRETH_CACHE_LINE_SIZE   32u

/**
 * @name    Busy-wait iteration bounds for hardware polling (not time-based)
 * @{
 */
#define GRETH_MDIO_BUSY_TIMEOUT     (200000u)   /**< MDIO transaction completion */
#define GRETH_MDIO_START_TIMEOUT    (10000u)    /**< MDIO BUSY assert after command */
#define GRETH_RESET_TIMEOUT         (100000u)   /**< MAC software reset */
#define GRETH_TX_TIMEOUT            (1000000u)  /**< TX DMA completion */
/** @} */

static inline void _cbo_flush(void *addr)
{
    __asm__ volatile(
        ".option push\n\t"
        ".option arch, +zicbom\n\t"
        "cbo.flush 0(%0)\n\t"
        ".option pop"
        :: "r"(addr) : "memory");
}

static inline void _cbo_inval(void *addr)
{
    __asm__ volatile(
        ".option push\n\t"
        ".option arch, +zicbom\n\t"
        "cbo.inval 0(%0)\n\t"
        ".option pop"
        :: "r"(addr) : "memory");
}

static void _dcache_flush_range(const void *p, size_t len)
{
    uintptr_t addr = (uintptr_t)p & ~(uintptr_t)(GRETH_CACHE_LINE_SIZE - 1u);
    uintptr_t end  = (uintptr_t)p + len;
    while (addr < end) {
        _cbo_flush((void *)addr);
        addr += GRETH_CACHE_LINE_SIZE;
    }
}

static void _dcache_inval_range(const void *p, size_t len)
{
    uintptr_t addr = (uintptr_t)p & ~(uintptr_t)(GRETH_CACHE_LINE_SIZE - 1u);
    uintptr_t end  = (uintptr_t)p + len;
    while (addr < end) {
        _cbo_inval((void *)addr);
        addr += GRETH_CACHE_LINE_SIZE;
    }
}

/* Cast via uintptr_t to silence "int-to-pointer cast of different size" on
 * rv64 when casting a uint32_t APB address to a pointer. */
#define GRETH_REGS(dev) \
    ((greth_regs_t *)(uintptr_t)((dev)->params->base_addr))

static inline void _mmio_write(volatile uint32_t *addr, uint32_t val)
{
    *addr = val;
}

static inline uint32_t _mmio_read(const volatile uint32_t *addr)
{
    return *addr;
}

static void _mdio_wait(greth_regs_t *regs)
{
    for (unsigned i = 0; i < GRETH_MDIO_BUSY_TIMEOUT; i++) {
        if (!(_mmio_read(&regs->mdio) & GRETH_MDIO_BUSY)) {
            return;
        }
    }
}

/* Two-phase poll: phase 1 waits for BUSY to assert (transaction started),
 * phase 2 waits for it to clear (done). Detecting an inaccessible bus early
 * early avoids wasting the full completion timeout.
 * Returns the 16-bit value, or: -EIO data not valid, -ENODEV bus
 * inaccessible, -ETIMEDOUT stuck busy. */
static int _mdio_read(greth_regs_t *regs, unsigned phy_addr, unsigned reg)
{
    _mdio_wait(regs);

    uint32_t cmd = (phy_addr << GRETH_MDIO_PHYSHIFT) |
                   (reg      << GRETH_MDIO_REGSHIFT)  |
                   GRETH_MDIO_OP_RD;
    _mmio_write(&regs->mdio, cmd);

    bool busy_seen = false;
    for (unsigned i = 0; i < GRETH_MDIO_START_TIMEOUT; i++) {
        if (_mmio_read(&regs->mdio) & GRETH_MDIO_BUSY) {
            busy_seen = true;
            break;
        }
    }
    if (!busy_seen) {
        return -ENODEV;
    }

    for (unsigned i = 0; i < GRETH_MDIO_BUSY_TIMEOUT; i++) {
        uint32_t val = _mmio_read(&regs->mdio);
        if (!(val & GRETH_MDIO_BUSY)) {
            if (val & GRETH_MDIO_NVALID) {
                return -EIO;
            }
            return (int)((val >> GRETH_MDIO_DATASHIFT) & 0xFFFF);
        }
    }
    return -ETIMEDOUT;
}

static void _mdio_write_reg(greth_regs_t *regs, unsigned phy_addr,
                            unsigned reg, uint16_t data)
{
    _mdio_wait(regs);
    uint32_t cmd = ((uint32_t)data  << GRETH_MDIO_DATASHIFT) |
                   (phy_addr        << GRETH_MDIO_PHYSHIFT)   |
                   (reg             << GRETH_MDIO_REGSHIFT)   |
                   GRETH_MDIO_OP_WR;
    _mmio_write(&regs->mdio, cmd);
    _mdio_wait(regs);
}

static int _phy_detect(greth_regs_t *regs)
{
    /* mdio[15:11] holds the address from the last MDIO operation — when GRMON
     * left the PHY in a known state this gives the correct address without a scan. */
    uint32_t mdio = _mmio_read(&regs->mdio);
    unsigned phy_addr = (mdio >> GRETH_MDIO_PHYSHIFT) & 0x1F;

    int status = _mdio_read(regs, phy_addr, GRETH_MII_STATUS);
    /* status == 0 means all-zeros: no real IEEE 802.3 PHY (bit 0, extended-capable,
     * is always 1 in a real PHY). Fall through to full scan. */
    if (status > 0) {
        DEBUG("[greth] PHY found at MDIO addr %u (from register), status=0x%04x\n",
              phy_addr, status);
        return (int)phy_addr;
    }

    for (unsigned i = 0; i < 32; i++) {
        status = _mdio_read(regs, i, GRETH_MII_STATUS);
        if (status == -ENODEV) {
            LOG_WARNING("[greth] MDIO bus not accessible\n");
            break;
        }
        if (status > 0 && status != 0xFFFF) {
            DEBUG("[greth] PHY found at MDIO addr %u (scan), status=0x%04x\n",
                  i, status);
            return (int)i;
        }
    }
    LOG_WARNING("[greth] no PHY found via MDIO (mdio_reg=0x%08" PRIx32 ")\n", mdio);
    return -ENODEV;
}

static void _phy_reset_and_aneg(greth_t *dev)
{
    greth_regs_t *regs = GRETH_REGS(dev);

    _mdio_write_reg(regs, dev->phy_addr, GRETH_MII_CTRL, GRETH_MII_CTRL_RST);

    for (unsigned i = 0; i < GRETH_ANEG_TIMEOUT; i++) {
        int ctrl = _mdio_read(regs, dev->phy_addr, GRETH_MII_CTRL);
        if (ctrl >= 0 && !(ctrl & GRETH_MII_CTRL_RST)) {
            break;
        }
    }

    int ctrl = _mdio_read(regs, dev->phy_addr, GRETH_MII_CTRL);
    if (ctrl < 0 || !(ctrl & GRETH_MII_CTRL_ANEG)) {
        return;
    }

    for (unsigned i = 0; i < GRETH_ANEG_TIMEOUT; i++) {
        int status = _mdio_read(regs, dev->phy_addr, GRETH_MII_STATUS);
        if (status >= 0 && (status & GRETH_MII_STATUS_ANEG_DONE)) {
            return;
        }
    }
    DEBUG("[greth] auto-negotiation timeout — continuing anyway\n");
}

static void _phy_configure_mac(greth_t *dev)
{
    greth_regs_t *regs = GRETH_REGS(dev);

    int phy_ctrl = _mdio_read(regs, dev->phy_addr, GRETH_MII_CTRL);
    int phy_stat = _mdio_read(regs, dev->phy_addr, GRETH_MII_STATUS);

    bool full_duplex;
    bool speed_100;

    if (phy_ctrl == -ENODEV || phy_stat == -ENODEV) {
        /* MDIO pins not routed to PL on this design — GRMON configured the PHY
         * via PS. Default to 100FD (standard auto-neg result for this PHY). */
        LOG_WARNING("[greth] MDIO not accessible, defaulting to 100 Mbps full-duplex\n");
        full_duplex = true;
        speed_100   = true;
    }
    else if (phy_ctrl > 0 && (phy_ctrl & GRETH_MII_CTRL_ANEG)) {
        /* Auto-neg used. MII_CTRL bits 13/8 are forced-mode fields and do NOT
         * reflect the negotiated result. AND LPA and ADV for highest common mode. */
        int adv = _mdio_read(regs, dev->phy_addr, GRETH_MII_ADV);
        int lpa = _mdio_read(regs, dev->phy_addr, GRETH_MII_LPA);
        if (adv < 0) {
            adv = 0;
        }
        if (lpa < 0) {
            lpa = 0;
        }
        int common = adv & lpa;

        DEBUG("[greth] PHY addr=%u ctrl=0x%04x stat=0x%04x adv=0x%04x lpa=0x%04x\n",
              dev->phy_addr, phy_ctrl, phy_stat, adv, lpa);

        bool link_up = (phy_stat & GRETH_MII_STATUS_LINK) != 0;
        if (!link_up || common == 0) {
            /* Link not up at init means LPA is invalid. Default to 100FD. */
            speed_100 = true;
            full_duplex = true;
        }
        else if (common & GRETH_MII_LPA_100_FD) {
            speed_100 = true;
            full_duplex = true;
        }
        else if (common & GRETH_MII_LPA_100_HD) {
            speed_100 = true;
            full_duplex = false;
        }
        else if (common & GRETH_MII_LPA_10_FD) {
            speed_100 = false;
            full_duplex = true;
        }
        else {
            speed_100 = false;
            full_duplex = false;
        }
    }
    else if (phy_ctrl > 0) {
        full_duplex = (phy_ctrl & GRETH_MII_CTRL_FD)     != 0;
        speed_100   = (phy_ctrl & GRETH_MII_CTRL_SPD100) != 0;
        DEBUG("[greth] PHY addr=%u ctrl=0x%04x stat=0x%04x (forced mode)\n",
              dev->phy_addr, phy_ctrl, phy_stat);
    }
    else {
        LOG_WARNING("[greth] unexpected PHY state (ctrl=%d stat=%d), defaulting to 100FD\n",
                    phy_ctrl, phy_stat);
        full_duplex = true;
        speed_100   = true;
    }

    uint32_t mac_ctrl = GRETH_CTRL_EDCLDIS;
    if (full_duplex) {
        mac_ctrl |= GRETH_CTRL_FD;
    }
    if (speed_100) {
        mac_ctrl |= GRETH_CTRL_SPD;
    }
    _mmio_write(&regs->ctrl, mac_ctrl);

    DEBUG("[greth] MAC ctrl=0x%08" PRIx32 " => %s duplex, %s Mbps\n",
          mac_ctrl, full_duplex ? "full" : "half", speed_100 ? "100" : "10");
}

static void _set_mac_address(greth_t *dev)
{
    greth_regs_t *regs = GRETH_REGS(dev);
    const uint8_t *mac = dev->params->mac;

    _mmio_write(&regs->mac_msb, ((uint32_t)mac[0] << 8) | mac[1]);
    _mmio_write(&regs->mac_lsb, ((uint32_t)mac[2] << 24) |
                                ((uint32_t)mac[3] << 16) |
                                ((uint32_t)mac[4] <<  8) |
                                 (uint32_t)mac[5]);
}

static void _tx_desc_init(greth_t *dev)
{
    for (unsigned i = 0; i < CONFIG_GRETH_TX_DESC_NUM; i++) {
        uint32_t ctrl = (i == CONFIG_GRETH_TX_DESC_NUM - 1) ? GRETH_BD_WR : 0;
        dev->tx_desc[i].ctrl = ctrl;
        dev->tx_desc[i].addr = 0;
    }
    dev->tx_idx = 0;
    _dcache_flush_range(dev->tx_desc,
                        CONFIG_GRETH_TX_DESC_NUM * sizeof(greth_desc_t));
}

static void _rx_desc_init(greth_t *dev)
{
    for (unsigned i = 0; i < CONFIG_GRETH_RX_DESC_NUM; i++) {
        uint32_t ctrl = GRETH_BD_EN | GRETH_BD_IE;
        if (i == CONFIG_GRETH_RX_DESC_NUM - 1) {
            ctrl |= GRETH_BD_WR;
        }
        dev->rx_desc[i].ctrl = ctrl;
        dev->rx_desc[i].addr = (uint32_t)(uintptr_t)_rx_buf[i];
    }
    dev->rx_idx = 0;
    _dcache_flush_range(dev->rx_desc,
                        CONFIG_GRETH_RX_DESC_NUM * sizeof(greth_desc_t));
}

static greth_t *_greth_dev_ptr;
static volatile uint32_t _greth_pending_status;

static void _greth_isr(int irq)
{
    (void)irq;

    greth_t *dev = _greth_dev_ptr;
    if (!dev || !dev->netdev.event_callback) {
        return;
    }

    greth_regs_t *regs = GRETH_REGS(dev);
    uint32_t status = _mmio_read(&regs->status);

    /* W1C in ISR before plic_complete_interrupt(): GRETH is level-triggered;
     * leaving status bits set would cause the interrupt to re-fire immediately. */
    _mmio_write(&regs->status, status);
    _greth_pending_status |= status;

    DEBUG("[greth] ISR: status=0x%08" PRIx32 "\n", status);

    dev->netdev.event_callback(&dev->netdev, NETDEV_EVENT_ISR);
}

static int _init(netdev_t *netdev)
{
    greth_t *dev = (greth_t *)(void *)netdev;
    greth_regs_t *regs = GRETH_REGS(dev);

    dev->tx_desc = _tx_desc;
    dev->rx_desc = _rx_desc;
    dev->tx_buf  = _tx_buf;

    /* The GRETH descriptor and buffer pointers are programmed into 32-bit
     * hardware registers. On a 64-bit host the DMA engine cannot reach memory
     * above 4 GiB, so the descriptor tables and buffers must live in the low
     * 4 GiB. Fail loudly if a future memory layout violates this. */
    assert(((uintptr_t)dev->tx_desc >> 32) == 0);
    assert(((uintptr_t)dev->rx_desc >> 32) == 0);
    assert(((uintptr_t)dev->tx_buf  >> 32) == 0);

    uint32_t cap  = _mmio_read(&regs->ctrl);
    dev->gbit     = (cap & GRETH_CTRL_GBIT_CAP) != 0;
    bool has_edcl = (cap & GRETH_CTRL_EDCL_CAP) != 0;
    DEBUG("[greth] cap=0x%08" PRIx32 " gbit=%d has_edcl=%d\n",
          cap, dev->gbit, (int)has_edcl);

    /* SW reset clears all GRETH registers and the DMA's internal descriptor
     * pointer. Required on every boot: grmon 'run' does NOT reset peripherals,
     * so a stale pointer from a previous run would corrupt the TX/RX rings.
     * RST does NOT affect the EDCL state machine — GRMON's debug link survives. */
    _mmio_write(&regs->ctrl, GRETH_CTRL_RST);
    for (unsigned i = 0; i < GRETH_RESET_TIMEOUT; i++) {
        if (!(_mmio_read(&regs->ctrl) & GRETH_CTRL_RST)) {
            break;
        }
    }
    DEBUG("[greth] SW reset done: ctrl=0x%08" PRIx32 "\n",
          _mmio_read(&regs->ctrl));

    int phy = _phy_detect(regs);
    if (phy < 0) {
        /* MDIO may be unreachable while the PHY still works (set up by the
         * boot loader) -- fall back to address 0 instead of failing init */
        phy = 0;
    }
    dev->phy_addr = (unsigned)phy;
    DEBUG("[greth] PHY at MDIO address %u\n", dev->phy_addr);

    _phy_reset_and_aneg(dev);
    _phy_configure_mac(dev);
    _set_mac_address(dev);
    _tx_desc_init(dev);
    _rx_desc_init(dev);

    _mmio_write(&regs->tx_desc, (uint32_t)(uintptr_t)dev->tx_desc);
    _mmio_write(&regs->rx_desc, (uint32_t)(uintptr_t)dev->rx_desc);

    _greth_dev_ptr = dev;
    plic_set_priority(dev->params->irq, 1);
    plic_set_isr_cb(dev->params->irq, _greth_isr);
    plic_enable_interrupt(dev->params->irq);

    /* PRO (promiscuous) required for IPv6: Neighbor Solicitations go to
     * Ethernet multicast (33:33:ff:xx:xx:xx); without PRO GRETH drops them
     * and NDP resolution from remote hosts is impossible. */
    uint32_t ctrl = _mmio_read(&regs->ctrl);
    _mmio_write(&regs->ctrl, ctrl | GRETH_CTRL_RXEN | GRETH_CTRL_RXIRQEN
                                  | GRETH_CTRL_PRO);

    DEBUG("[greth] init done: ctrl=0x%08" PRIx32
           " tx_desc=0x%08" PRIx32 "(hw=0x%08" PRIx32 ")"
           " rx_desc=0x%08" PRIx32 "(hw=0x%08" PRIx32 ")"
          " gbit=%d\n",
          _mmio_read(&regs->ctrl),
          (uint32_t)(uintptr_t)dev->tx_desc, _mmio_read(&regs->tx_desc),
          (uint32_t)(uintptr_t)dev->rx_desc, _mmio_read(&regs->rx_desc),
          dev->gbit);

    /* Post NETDEV_EVENT_ISR, not LINK_UP directly: gnrc_netif holds
     * gnrc_netif_acquire() during init(), and gnrc_ipv6_nib_iface_up()
     * triggered by LINK_UP also calls gnrc_netif_acquire() → deadlock.
     * _isr() emits LINK_UP on its first invocation after init() returns. */
    if (dev->netdev.event_callback) {
        dev->netdev.event_callback(&dev->netdev, NETDEV_EVENT_ISR);
    }

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    greth_t *dev = (greth_t *)(void *)netdev;
    greth_regs_t *regs = GRETH_REGS(dev);

    ssize_t total = iolist_to_buffer(iolist, dev->tx_buf, GRETH_BUF_SIZE);
    if (total < 0) {
        DEBUG("[greth] send: frame too large\n");
        return -ENOBUFS;
    }

    if ((size_t)total < ETHERNET_MIN_LEN) {
        memset(dev->tx_buf + total, 0, ETHERNET_MIN_LEN - (size_t)total);
        total = ETHERNET_MIN_LEN;
    }

    unsigned idx = dev->tx_idx;
    greth_desc_t *desc = &dev->tx_desc[idx];

    for (unsigned i = 0; i < GRETH_TX_TIMEOUT; i++) {
        if (!(_mmio_read(&desc->ctrl) & GRETH_BD_EN)) {
            break;
        }
    }

    desc->addr = (uint32_t)(uintptr_t)dev->tx_buf;
    /* IE=1: DMA sets GRETH_STATUS_TXIRQ on completion — TX-done detection via
     * the non-cached APB status register, independent of cbo_inval correctness. */
    uint32_t ctrl = (uint32_t)total | GRETH_BD_EN | GRETH_BD_IE;
    if (idx == CONFIG_GRETH_TX_DESC_NUM - 1) {
        ctrl |= GRETH_BD_WR;
    }
    desc->ctrl = ctrl;
    dev->tx_idx = (idx + 1) % CONFIG_GRETH_TX_DESC_NUM;

    _dcache_flush_range(dev->tx_buf, (size_t)total);
    _dcache_flush_range(desc, sizeof(*desc));

    /* Ensure cache-flush AHB writes reach SDRAM before the APB write that kicks
     * the DMA — without this the store buffer may reorder TXEN ahead of flush. */
    __asm__ volatile("fence ow, ow" ::: "memory");

    _cbo_inval((void *)desc);
    uint32_t verify_ctrl = _mmio_read(&desc->ctrl);
    DEBUG("[greth] TX[%u] S1-FLUSH: armed=0x%08" PRIx32 " dram=0x%08" PRIx32
           " buf=0x%08" PRIx32 " hw_tdesc=0x%08" PRIx32 "\n",
           idx, ctrl, verify_ctrl,
           (uint32_t)(uintptr_t)dev->tx_buf, _mmio_read(&regs->tx_desc));

    uint32_t mac_ctrl  = _mmio_read(&regs->ctrl);
    uint32_t clean_ctrl = mac_ctrl & (GRETH_CTRL_FD | GRETH_CTRL_PRO |
                                      GRETH_CTRL_SPD | GRETH_CTRL_GB |
                                      GRETH_CTRL_RXEN | GRETH_CTRL_RXIRQEN |
                                      GRETH_CTRL_TXIRQEN | GRETH_CTRL_TXEN);
    _mmio_write(&regs->ctrl, clean_ctrl & ~GRETH_CTRL_TXEN);
    DEBUG("[greth] TX[%u] S2-TXEN0: ctrl=0x%08" PRIx32 " status=0x%08" PRIx32
           " (clean=0x%08" PRIx32 ")\n",
           idx, _mmio_read(&regs->ctrl), _mmio_read(&regs->status), clean_ctrl);

    _mmio_write(&regs->ctrl, clean_ctrl | GRETH_CTRL_TXEN | GRETH_CTRL_TXIRQEN);
    DEBUG("[greth] TX[%u] S3-TXEN1: ctrl=0x%08" PRIx32 " status=0x%08" PRIx32 "\n",
          idx, _mmio_read(&regs->ctrl), _mmio_read(&regs->status));

    _cbo_inval((void *)desc);
    __asm__ volatile("fence ir, ir" ::: "memory");
    DEBUG("[greth] TX[%u] S4-IMM:   desc=0x%08" PRIx32 "\n",
           idx, _mmio_read(&desc->ctrl));

    /* Wait for TX completion via two independent signals:
     *   A) TXIRQ or TXERR in the non-cached APB status register (IE=1 in desc).
     *   B) Descriptor EN=0 via cbo_inval from DRAM. */
    unsigned tx_wait;
    bool status_exit  = false;
    bool txen_cleared = false;
    static const unsigned _samples[] = { 100, 1000, 10000, 100000, 500000, 999999 };
    unsigned _si = 0;
    for (tx_wait = 0; tx_wait < GRETH_TX_TIMEOUT; tx_wait++) {
        uint32_t hw_s = _mmio_read(&regs->status);
        uint32_t s    = hw_s | _greth_pending_status;
        if (s & (GRETH_STATUS_TXIRQ | GRETH_STATUS_TXERR)) {
            /* W1C only TX bits. Never W1C RXIRQ here: an RX frame that arrived
             * during TX polling would be silently lost. Transfer any RX bits
             * from the hardware register into pending for _isr() to dispatch. */
            _mmio_write(&regs->status,
                        hw_s & (GRETH_STATUS_TXIRQ | GRETH_STATUS_TXERR));
            _greth_pending_status |= hw_s & (GRETH_STATUS_RXIRQ |
                                             GRETH_STATUS_RXERR);
            _greth_pending_status &= ~(GRETH_STATUS_TXIRQ | GRETH_STATUS_TXERR);
            DEBUG("[greth] TX[%u] STATUS-EXIT[%u]: hw_s=0x%08" PRIx32
                   " pending=0x%08" PRIx32 "\n",
                   idx, tx_wait, hw_s, _greth_pending_status);
            status_exit = true;
            break;
        }

        uint32_t c = _mmio_read(&regs->ctrl);
        if (!(c & GRETH_CTRL_TXEN)) {
            DEBUG("[greth] TX[%u] TXEN-CLEARED[%u]: ctrl=0x%08" PRIx32
                   " hw_tdesc=0x%08" PRIx32 "\n",
                   idx, tx_wait, c, _mmio_read(&regs->tx_desc));
            txen_cleared = true;
            break;
        }

        _cbo_inval((void *)desc);
        __asm__ volatile("fence ir, ir" ::: "memory");
        uint32_t d = _mmio_read(&desc->ctrl);
        if (!(d & GRETH_BD_EN)) {
            break;
        }

        if (_si < 6 && tx_wait == _samples[_si]) {
            DEBUG("[greth] TX[%u] S5-POLL[%u]: desc=0x%08" PRIx32
                   " ctrl=0x%08" PRIx32 " status=0x%08" PRIx32 "\n",
                   idx, tx_wait, d,
                   _mmio_read(&regs->ctrl), _mmio_read(&regs->status));
            _si++;
        }
    }

    _cbo_inval((void *)desc);
    uint32_t post_ctrl = _mmio_read(&desc->ctrl);
    DEBUG("[greth] TX[%u] DONE: wait=%u via=%s desc=0x%08" PRIx32
          " ctrl=0x%08" PRIx32 " status=0x%08" PRIx32 "\n",
          idx, tx_wait, status_exit ? "STATUS" : "DESC",
          post_ctrl, _mmio_read(&regs->ctrl), _mmio_read(&regs->status));

    bool desc_done = !(post_ctrl & GRETH_BD_EN);

    if (!status_exit && !txen_cleared && !desc_done) {
        LOG_ERROR("[greth] TX[%u] timeout (DMA never ran)\n", idx);
        return -ETIMEDOUT;
    }
    if (txen_cleared && !desc_done) {
        LOG_ERROR("[greth] TX[%u] TXEN cleared but descriptor still armed\n", idx);
        return -EIO;
    }
    if (status_exit && !desc_done) {
        LOG_WARNING("[greth] TX[%u] status OK but descriptor still armed\n", idx);
    }
    if (post_ctrl & GRETH_TXBD_ERR_MASK) {
        LOG_ERROR("[greth] TX error bits: 0x%08" PRIx32 "\n",
                  post_ctrl & GRETH_TXBD_ERR_MASK);
        return -EIO;
    }

    /* RX events accumulated during TX polling (RXIRQ was preserved in pending
     * instead of being W1C'd): signal the netif thread to dispatch RX_COMPLETE.
     * A double-post of NETDEV_EVENT_ISR is harmless — _isr() with pending=0
     * is a no-op. */
    if (_greth_pending_status & (GRETH_STATUS_RXIRQ | GRETH_STATUS_RXERR)) {
        DEBUG("[greth] TX[%u] DONE: flushing pending RX (pending=0x%08" PRIx32 ")\n",
               idx, _greth_pending_status);
        if (dev->netdev.event_callback) {
            dev->netdev.event_callback(&dev->netdev, NETDEV_EVENT_ISR);
        }
    }

    return (int)total;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info;
    greth_t *dev = (greth_t *)(void *)netdev;

    unsigned idx = dev->rx_idx;
    greth_desc_t *desc = &dev->rx_desc[idx];

    /* Invalidate so we read the DMA's write (EN=0 + length), not stale EN=1. */
    _cbo_inval((void *)desc);
    uint32_t ctrl = _mmio_read(&desc->ctrl);

    DEBUG("[greth] _recv: idx=%u ctrl=0x%08" PRIx32 "\n", idx, ctrl);

    if (ctrl & GRETH_BD_EN) {
        return 0;
    }

    int frame_len = (int)(ctrl & GRETH_BD_LEN_MASK);

    if (buf == NULL && len == 0) {
        return frame_len;
    }

    if (ctrl & GRETH_RXBD_ERR_MASK) {
        DEBUG("[greth] RX error ctrl=0x%08" PRIx32 "\n", ctrl);
        frame_len = -EIO;
        goto rearm;
    }

    if (buf == NULL || (size_t)frame_len > len) {
        if (buf != NULL) {
            frame_len = -ENOBUFS;
        }
        goto rearm;
    }

    /* Invalidate RX buffer so memcpy reads DMA-written DRAM, not stale cache. */
    _dcache_inval_range(_rx_buf[idx], (size_t)frame_len);
    memcpy(buf, _rx_buf[idx], (size_t)frame_len);

rearm:
    {
        uint32_t new_ctrl = GRETH_BD_EN | GRETH_BD_IE;
        if (idx == CONFIG_GRETH_RX_DESC_NUM - 1) {
            new_ctrl |= GRETH_BD_WR;
        }
        desc->addr = (uint32_t)(uintptr_t)_rx_buf[idx];
        desc->ctrl = new_ctrl;
        _dcache_flush_range(desc, sizeof(*desc));
    }

    dev->rx_idx = (idx + 1) % CONFIG_GRETH_RX_DESC_NUM;

    /* Re-enable RX DMA in case it stalled when all descriptors were EN=0. */
    greth_regs_t *regs = GRETH_REGS(dev);
    uint32_t mac_ctrl = _mmio_read(&regs->ctrl);
    _mmio_write(&regs->ctrl, mac_ctrl | GRETH_CTRL_RXEN);

    return frame_len;
}

static void _isr(netdev_t *netdev)
{
    /* First call is synthetic (posted from _init()): emit LINK_UP here rather
     * than in _init() because gnrc_netif holds gnrc_netif_acquire() during init
     * and gnrc_ipv6_nib_iface_up() also calls it → deadlock if called directly. */
    static bool _link_up_sent = false;
    if (!_link_up_sent) {
        _link_up_sent = true;
        netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);
        return;
    }

    uint32_t status = _greth_pending_status;
    _greth_pending_status = 0;

    DEBUG("[greth] _isr: pending=0x%08" PRIx32 "\n", status);

    if (status & GRETH_STATUS_RXIRQ) {
        DEBUG("[greth] _isr: RX_COMPLETE!\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
    if (status & (GRETH_STATUS_RXERR | GRETH_STATUS_TXERR)) {
        DEBUG("[greth] error status: 0x%08" PRIx32 "\n", status);
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    greth_t *dev = (greth_t *)(void *)netdev;

    if (opt == NETOPT_ADDRESS) {
        if (max_len < ETHERNET_ADDR_LEN) {
            return -ENOBUFS;
        }
        memcpy(val, dev->params->mac, ETHERNET_ADDR_LEN);
        return ETHERNET_ADDR_LEN;
    }

    return netdev_eth_get(netdev, opt, val, max_len);
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t val_len)
{
    greth_t *dev = (greth_t *)(void *)netdev;

    if (opt == NETOPT_PROMISCUOUSMODE) {
        greth_regs_t *regs = GRETH_REGS(dev);
        uint32_t ctrl = _mmio_read(&regs->ctrl);
        if (*(const netopt_enable_t *)val == NETOPT_ENABLE) {
            ctrl |= GRETH_CTRL_PRO;
        }
        else {
            ctrl &= ~GRETH_CTRL_PRO;
        }
        _mmio_write(&regs->ctrl, ctrl);
        return sizeof(netopt_enable_t);
    }

    return netdev_eth_set(netdev, opt, val, val_len);
}

static const netdev_driver_t _greth_driver = {
    .init         = _init,
    .send         = _send,
    .recv         = _recv,
    .isr          = _isr,
    .get          = _get,
    .set          = _set,
    .confirm_send = NULL,   /* legacy mode: send() returns byte count directly */
};

void greth_setup(greth_t *dev, const greth_params_t *params, uint8_t index)
{
    assert(dev);
    assert(params);

    dev->params        = params;
    dev->netdev.driver = &_greth_driver;
    dev->tx_desc       = NULL;
    dev->rx_desc       = NULL;
    dev->tx_buf        = NULL;
    dev->rx_idx        = 0;
    dev->tx_idx        = 0;
    dev->phy_addr      = 0;
    dev->gbit          = false;

    netdev_register(&dev->netdev, NETDEV_GRETH, index);
}
