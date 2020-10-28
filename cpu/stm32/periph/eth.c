/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *               2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Low-level ETH driver implementation
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "bitarithm.h"
#include "iolist.h"
#include "luid.h"
#include "mii.h"
#include "mutex.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_STM32_ETH_LINK_UP)
#include "xtimer.h"
#define STM32_ETH_LINK_UP_TIMEOUT_US    (1UL * US_PER_SEC)

static xtimer_t _link_status_timer;
#endif /* IS_USED(MODULE_STM32_ETH_LINK_UP)  */

/* Set the value of the divider with the clock configured */
#if !defined(CLOCK_CORECLOCK) || CLOCK_CORECLOCK < (20000000U)
#error This peripheral requires a CORECLOCK of at least 20MHz
#elif CLOCK_CORECLOCK < (35000000U)
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div16
#elif CLOCK_CORECLOCK < (60000000U)
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div26
#elif CLOCK_CORECLOCK < (100000000U)
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div42
#elif CLOCK_CORECLOCK < (150000000U)
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div62
#else /* CLOCK_CORECLOCK < (20000000U) */
#define CLOCK_RANGE ETH_MACMIIAR_CR_Div102
#endif /* CLOCK_CORECLOCK < (20000000U) */

/* Default DMA buffer setup */
#ifndef ETH_RX_DESCRIPTOR_COUNT
#define ETH_RX_DESCRIPTOR_COUNT     (6U)
#endif
#ifndef ETH_TX_DESCRIPTOR_COUNT
#define ETH_TX_DESCRIPTOR_COUNT     (8U)
#endif
#ifndef ETH_RX_BUFFER_SIZE
#define ETH_RX_BUFFER_SIZE          (256U)
#endif

/* Bitmask to extract link state */
#define LINK_STATE                  (0x01)
/* Bitmask to extract notification state */
#define LINK_STATE_NOTIFIED         (0x02)
#define LINK_STATE_DOWN             (0x00)
#define LINK_STATE_UP               (0x01)
#define LINK_STATE_NOTIFIED_DOWN    (LINK_STATE_NOTIFIED | LINK_STATE_DOWN)
#define LINK_STATE_NOTIFIED_UP      (LINK_STATE_NOTIFIED | LINK_STATE_UP)

#if (ETH_RX_BUFFER_SIZE % 16) != 0
/* For compatibility with 128bit memory interfaces, the buffer size needs to
 * be a multiple of 16 Byte. For 64 bit memory interfaces need the size to be
 * a multiple of 8 Byte, for 32 bit a multiple of 4 byte is sufficient. */
#warning "ETH_RX_BUFFER_SIZE is not a multiple of 16. (See comment above.)"
#endif

#if ETH_RX_DESCRIPTOR_COUNT * ETH_RX_BUFFER_SIZE < 1524U
#warning "Total RX buffers lower than MTU, you won't receive huge frames!"
#endif

#define MIN(a, b) (((a) <= (b)) ? (a) : (b))

/* Synchronization between IRQ and thread context */
mutex_t stm32_eth_tx_completed = MUTEX_INIT_LOCKED;

/* Descriptors */
static edma_desc_t rx_desc[ETH_RX_DESCRIPTOR_COUNT];
static edma_desc_t tx_desc[ETH_TX_DESCRIPTOR_COUNT];
static edma_desc_t *rx_curr;
static edma_desc_t *tx_curr;

/* RX Buffers */
static char rx_buffer[ETH_RX_DESCRIPTOR_COUNT][ETH_RX_BUFFER_SIZE];

/* Netdev used in RIOT's API to upper layer */
netdev_t *_netdev;

#if IS_USED(MODULE_STM32_ETH_LINK_UP)
/* Used for checking the link status */
static uint8_t _link_state = LINK_STATE_DOWN;
#endif /* IS_USED(MODULE_STM32_ETH_LINK_UP) */

static void _debug_tx_descriptor_info(unsigned line)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("[stm32_eth:%u] TX descriptors:\n", line);
        for (unsigned i = 0; i < ETH_TX_DESCRIPTOR_COUNT; i++) {
            uint32_t status = tx_desc[i].status;
            DEBUG("    %s %u: OWN=%c, ES=%c, UF=%c, EC=%c, NC=%c, FS=%c, "
                  "LS=%c\n",
                  (tx_curr == tx_desc + i) ? "-->" : "   ",
                  i,
                  (status & TX_DESC_STAT_OWN) ? '1' : '0',
                  (status & TX_DESC_STAT_ES) ? '1' : '0',
                  (status & TX_DESC_STAT_UF) ? '1' : '0',
                  (status & TX_DESC_STAT_EC) ? '1' : '0',
                  (status & TX_DESC_STAT_NC) ? '1' : '0',
                  (status & TX_DESC_STAT_FS) ? '1' : '0',
                  (status & TX_DESC_STAT_LS) ? '1' : '0');
        }
    }
}

static inline uint32_t _len_from_rx_desc_status(uint32_t status)
{
    /* bits 16-29 contain the frame length including 4 B frame check sequence */
    return (status >> 16) & 0x3fff;
}

static void _debug_rx_descriptor_info(unsigned line)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("[stm32_eth:%u] RX descriptors:\n", line);
        for (unsigned i = 0; i < ETH_RX_DESCRIPTOR_COUNT; i++) {
            uint32_t status = rx_desc[i].status;
            DEBUG("    %s %u: OWN=%c, FS=%c, LS=%c, ES=%c, DE=%c, FL=%lu\n",
                  (rx_curr == rx_desc + i) ? "-->" : "   ",
                  i,
                  (status & RX_DESC_STAT_OWN) ? '1' : '0',
                  (status & RX_DESC_STAT_FS) ? '1' : '0',
                  (status & RX_DESC_STAT_LS) ? '1' : '0',
                  (status & RX_DESC_STAT_ES) ? '1' : '0',
                  (status & RX_DESC_STAT_DE) ? '1' : '0',
                  _len_from_rx_desc_status(status));
        }
    }
}

/**
 * @brief   Read or write a MII register
 *
 * @param[in]   reg     MII register to access
 * @param[in]   value   Value to write (ignored when @p write is `false`)
 * @param[in]   write   Whether to write (`true`) or read (`false`) to/from the
 *                      register
 *
 * @return  The value of the MII register accessed. (This should be equal to
 *          @p value, if @p write was `true`.)
 */
static uint16_t _mii_reg_transfer(unsigned reg, uint16_t value, bool write)
{
    unsigned tmp;
    const uint16_t phy_addr = eth_config.phy_addr;

    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}

    tmp = CLOCK_RANGE | ETH_MACMIIAR_MB
        | (((phy_addr & 0x1f) << 11) | ((reg & 0x1f) << 6));

    if (write) {
        tmp |= ETH_MACMIIAR_MW;
        ETH->MACMIIDR = value;
    }

    ETH->MACMIIAR = tmp;
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}

    return ETH->MACMIIDR;
}

static inline int16_t _mii_reg_read(uint8_t reg)
{
    return _mii_reg_transfer(reg, 0, false);
}

static inline void _mii_reg_write(uint8_t reg, uint16_t value)
{
    _mii_reg_transfer(reg, value, true);
}

static inline bool _get_link_status(void)
{
    return (_mii_reg_read(MII_BMSR) & MII_BMSR_LINK);
}

static void stm32_eth_get_addr(char *out)
{
    unsigned t;

    t = ETH->MACA0HR;
    out[5] = (t >> 8);
    out[4] = (t & 0xff);

    t = ETH->MACA0LR;
    out[3] = (t >> 24);
    out[2] = (t >> 16);
    out[1] = (t >> 8);
    out[0] = (t & 0xff);
}

/** Set the mac address. The peripheral supports up to 4 MACs but only one is
 * implemented */
static void stm32_eth_set_addr(const uint8_t *addr)
{
    ETH->MACA0HR &= 0xffff0000;
    ETH->MACA0HR |= (addr[5] << 8) | addr[4];
    ETH->MACA0LR = (addr[3] << 24) | (addr[2] << 16) | (addr[1] << 8) | addr[0];
}

/** Initialization of the DMA descriptors to be used */
static void _init_buffer(void)
{
    size_t i;
    for (i = 0; i < ETH_RX_DESCRIPTOR_COUNT; i++) {
        rx_desc[i].status = RX_DESC_STAT_OWN;
        rx_desc[i].control = RX_DESC_CTRL_RCH | (ETH_RX_BUFFER_SIZE & 0x0fff);
        rx_desc[i].buffer_addr = &rx_buffer[i][0];
        if ((i + 1) < ETH_RX_DESCRIPTOR_COUNT) {
            rx_desc[i].desc_next = &rx_desc[i + 1];
        }
    }
    rx_desc[i - 1].desc_next = &rx_desc[0];

    for (i = 0; i < ETH_TX_DESCRIPTOR_COUNT - 1; i++) {
        tx_desc[i].desc_next = &tx_desc[i + 1];
    }
    tx_desc[ETH_RX_DESCRIPTOR_COUNT - 1].desc_next = &tx_desc[0];

    rx_curr = &rx_desc[0];
    tx_curr = &tx_desc[0];

    ETH->DMARDLAR = (uintptr_t)rx_curr;
    ETH->DMATDLAR = (uintptr_t)tx_curr;
}

static int stm32_eth_set(netdev_t *dev, netopt_t opt,
                         const void *value, size_t max_len)
{
    int res = -1;

    switch (opt) {
    case NETOPT_ADDRESS:
        assert(max_len >= ETHERNET_ADDR_LEN);
        stm32_eth_set_addr(value);
        res = ETHERNET_ADDR_LEN;
        break;
    default:
        res = netdev_eth_set(dev, opt, value, max_len);
        break;
    }

    return res;
}

static int stm32_eth_get(netdev_t *dev, netopt_t opt,
                         void *value, size_t max_len)
{
    int res = -1;

    switch (opt) {
    case NETOPT_ADDRESS:
        assert(max_len >= ETHERNET_ADDR_LEN);
        stm32_eth_get_addr(value);
        res = ETHERNET_ADDR_LEN;
        break;
    case NETOPT_LINK:
        assert(max_len == sizeof(netopt_enable_t));
        {
            netopt_enable_t tmp = _get_link_status();
            memcpy(value, &tmp, sizeof(tmp));
        }
        res = sizeof(netopt_enable_t);
        break;
    default:
        res = netdev_eth_get(dev, opt, value, max_len);
        break;
    }

    return res;
}

#if IS_USED(MODULE_STM32_ETH_LINK_UP)
static void _timer_cb(void *arg)
{
    netdev_t *dev = (netdev_t *)arg;
    uint8_t state = LINK_STATE_DOWN;
    if (_get_link_status()) {
        state = LINK_STATE_UP;
    }

    if ((_link_state & LINK_STATE) != state) {
        /* link state changed, notify upper layer */
        _link_state = state;
        dev->event_callback(dev, NETDEV_EVENT_ISR);
    }

    xtimer_set(&_link_status_timer, STM32_ETH_LINK_UP_TIMEOUT_US);
}
#endif /* IS_USED(MODULE_STM32_ETH_LINK_UP) */

static bool _phy_can_negotiate(void)
{
    return (_mii_reg_read(MII_BMSR) & MII_BMSR_HAS_AN);
}

#if IS_USED(MODULE_STM32_ETH_AUTO)
static void _complete_auto_negotiation(void)
{
    /* first, wait until auto-negotiation really has completed */
    uint16_t bmsr;

    do {
        bmsr = _mii_reg_read(MII_BMSR);
        if (!(bmsr & MII_BMSR_LINK)) {
            /* disconnected during auto-negotiation */
            return;
        }
    } while (!(bmsr & MII_BMSR_AN_DONE));
    DEBUG("[stm32_eth] PHY auto-negotiation completed, PHY link up\n");
    /* Get current MACCR state without speed config */
    uint32_t maccr = ETH->MACCR & ~(ETH_MACCR_FES | ETH_MACCR_DM);
    /* stupidly, there is seemingly no way to get current connection speed
     * and duplex mode. But we can deduce it from our advertised capabilities
     * and the link partner abilities */
    uint16_t adv = _mii_reg_read(MII_ADVERTISE);
    uint16_t lpa = _mii_reg_read(MII_LPA);

    if ((adv & MII_ADVERTISE_100) && (lpa & MII_LPA_100)) {
        /* 100 Mbps */
        maccr |= ETH_MACCR_FES;
        if ((adv & MII_ADVERTISE_100_F) && (lpa & MII_LPA_100_F)) {
            /* full duplex */
            maccr |= ETH_MACCR_DM;
        }
    }
    else if ((adv & MII_ADVERTISE_10_F) && (lpa & MII_LPA_10_F)) {
        /* full duplex */
        maccr |= ETH_MACCR_DM;
    }
    DEBUG("[stm32_eth] %s Mbps %s duplex \n",
          (maccr & ETH_MACCR_FES) ? "100" : "10",
          (maccr & ETH_MACCR_DM) ? "full" : "half");
    ETH->MACCR = maccr;
}
#endif /* IS_USED(MODULE_STM32_ETH_AUTO) */

static void _setup_phy(void)
{
    DEBUG("[stm32_eth] Reset PHY\n");
    /* reset PHY */
    _mii_reg_write(MII_BMCR, MII_BMCR_RESET);
    /* wait till PHY reset is completed */
    while (MII_BMCR_RESET & _mii_reg_read(MII_BMCR)) {}

    /* check if auto-negotiation is enabled and supported */
    if (IS_USED(MODULE_STM32_ETH_AUTO) && _phy_can_negotiate()) {
        _mii_reg_write(MII_BMCR, MII_BMCR_AN_ENABLE);
        DEBUG("[stm32_eth] Enabled auto-negotiation\n");
        /* We'll continue link setup once auto-negotiation is done */
        return;
    }

    /* Get current MACCR state without speed config */
    uint32_t maccr = ETH->MACCR & ~(ETH_MACCR_FES | ETH_MACCR_DM);
    DEBUG("[stm32_eth] No PHY auto-negotiation disabled or unsupported\n");
    /* disable auto-negotiation and force manually configured speed to be
     * used */
    _mii_reg_write(MII_BMCR, eth_config.speed);
    /* configure MACCR to match PHY speed */
    if (eth_config.speed & MII_BMCR_FULL_DPLX) {
        maccr |= ETH_MACCR_DM;
    }
    if (eth_config.speed & MII_BMCR_SPEED_100) {
        maccr |= ETH_MACCR_FES;
    }
    DEBUG("[stm32_eth] %s Mbps %s duplex \n",
          (maccr & ETH_MACCR_FES) ? "100" : "10",
          (maccr & ETH_MACCR_DM) ? "full" : "half");
    /* Apply new duplex & speed configuration in MAC */
    ETH->MACCR = maccr;
}

static int stm32_eth_init(netdev_t *netdev)
{
#if IS_USED(MODULE_STM32_ETH_LINK_UP)
    _link_status_timer.callback = _timer_cb;
    _link_status_timer.arg = netdev;
    xtimer_set(&_link_status_timer, STM32_ETH_LINK_UP_TIMEOUT_US);
#else
    (void)netdev;
#endif
    /* enable APB2 clock */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* select RMII if necessary */
    if (eth_config.mode == RMII) {
        SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;
    }

    /* initialize GPIO */
    for (int i = 0; i < (int) eth_config.mode; i++) {
        gpio_init(eth_config.pins[i], GPIO_OUT);
        gpio_init_af(eth_config.pins[i], GPIO_AF11);
    }

    /* enable all clocks */
    RCC->AHB1ENR |= (RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN |
                     RCC_AHB1ENR_ETHMACRXEN | RCC_AHB1ENR_ETHMACPTPEN);

    /* reset the peripheral */
    RCC->AHB1RSTR |= RCC_AHB1RSTR_ETHMACRST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_ETHMACRST;

    /* software reset */
    ETH->DMABMR |= ETH_DMABMR_SR;
    while (ETH->DMABMR & ETH_DMABMR_SR) {}

    /* set the clock divider */
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}
    ETH->MACMIIAR = CLOCK_RANGE;

    /* ROD  = Don't receive own frames in half-duplex mode
     * IPCO = Drop IPv4 packets carrying TCP/UDP/ICMP when checksum is invalid
     * APCS = Do not pass padding and CRC fields to application (CRC is checked
     *        by hardware already) */
    ETH->MACCR |= ETH_MACCR_ROD | ETH_MACCR_IPCO | ETH_MACCR_APCS;

    /* pass all */
    //ETH->MACFFR |= ETH_MACFFR_RA;
    /* pass on perfect filter match and pass all multicast address matches */
    ETH->MACFFR |= ETH_MACFFR_PAM;

    /* store forward */
    ETH->DMAOMR |= (ETH_DMAOMR_RSF | ETH_DMAOMR_TSF | ETH_DMAOMR_OSF);

    /* configure DMA */
    ETH->DMABMR = ETH_DMABMR_DA | ETH_DMABMR_AAB | ETH_DMABMR_FB
                | ETH_DMABMR_RDP_32Beat | ETH_DMABMR_PBL_32Beat
                | ETH_DMABMR_EDE;

    if (eth_config.addr[0] != 0) {
        stm32_eth_set_addr(eth_config.addr);
    }
    else {
        eui48_t hwaddr;
        luid_get_eui48(&hwaddr);
        stm32_eth_set_addr(hwaddr.uint8);
    }

    _init_buffer();

    NVIC_EnableIRQ(ETH_IRQn);
    ETH->DMAIER |= ETH_DMAIER_NISE | ETH_DMAIER_TIE | ETH_DMAIER_RIE;

    /* enable transmitter and receiver */
    ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
    /* flush transmit FIFO */
    ETH->DMAOMR |= ETH_DMAOMR_FTF;
    /* wait for FIFO flushing to complete */
    while (ETH->DMAOMR & ETH_DMAOMR_FTF) { }

    /* enable DMA TX and RX */
    ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;

    _setup_phy();

    return 0;
}

static int stm32_eth_send(netdev_t *netdev, const struct iolist *iolist)
{
    (void)netdev;
    netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    unsigned bytes_to_send = iolist_size(iolist);
    /* Input must not be bigger than maximum allowed frame length */
    assert(bytes_to_send <= ETHERNET_FRAME_LEN);
    /* This API is not thread safe, check that no other thread is sending */
    assert(!(tx_desc[0].status & TX_DESC_STAT_OWN));
    /* We cannot send more chunks than allocated descriptors */
    assert(iolist_count(iolist) <= ETH_TX_DESCRIPTOR_COUNT);

    for (unsigned i = 0; iolist; iolist = iolist->iol_next, i++) {
        tx_curr->control = iolist->iol_len;
        tx_curr->buffer_addr = iolist->iol_base;
        uint32_t status = TX_DESC_STAT_IC | TX_DESC_STAT_TCH | TX_DESC_STAT_CIC
                          | TX_DESC_STAT_OWN;
        if (!i) {
            /* fist chunk */
            status |= TX_DESC_STAT_FS;
        }
        if (!iolist->iol_next) {
            /* last chunk */
            status |= TX_DESC_STAT_LS;
        }
        tx_curr->status = status;
        tx_curr = tx_curr->desc_next;
    }

    /* start TX */
    ETH->DMATPDR = 0;
    /* await completion */
    DEBUG("[stm32_eth] Started to send %u B via DMA\n", bytes_to_send);
    mutex_lock(&stm32_eth_tx_completed);
    DEBUG("[stm32_eth] TX completed\n");

    /* Error check */
    unsigned i = 0;
    _debug_tx_descriptor_info(__LINE__);
    while (1) {
        uint32_t status = tx_desc[i].status;
        /* The Error Summary (ES) bit is set, if any error during TX occurred */
        if (status & TX_DESC_STAT_ES) {
            /* TODO: Report better event to reflect error */
            netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
            return -1;
        }
        if (status & TX_DESC_STAT_LS) {
            break;
        }
        i++;
    }
    netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    return (int)bytes_to_send;
}

static int get_rx_frame_size(void)
{
    _debug_rx_descriptor_info(__LINE__);
    edma_desc_t *i = rx_curr;
    uint32_t status;
    while (1) {
        /* Wait until DMA gave up control over descriptor */
        if ((status = i->status) & RX_DESC_STAT_OWN) {
            DEBUG("[stm32_eth] RX not completed (spurious interrupt?)\n");
            return -EAGAIN;
        }
        if (status & RX_DESC_STAT_DE) {
            DEBUG("[stm32_eth] Overflow during RX\n");
            return -EOVERFLOW;
        }
        if (status & RX_DESC_STAT_ES) {
            DEBUG("[stm32_eth] Error during RX\n");
            return -EIO;
        }
        if (status & RX_DESC_STAT_LS) {
            break;
        }
        i = i->desc_next;
    }

    return _len_from_rx_desc_status(status) - ETHERNET_FCS_LEN;
}

static void drop_frame_and_update_rx_curr(void)
{
    while (1) {
        uint32_t old_status = rx_curr->status;
        /* hand over old descriptor to DMA */
        rx_curr->status = RX_DESC_STAT_OWN;
        rx_curr = rx_curr->desc_next;
        if (old_status & (RX_DESC_STAT_LS | RX_DESC_STAT_ES)) {
            /* reached either last DMA descriptor of frame or error ==> done */
            return;
        }
    }
}

static void handle_lost_rx_irqs(void)
{
    edma_desc_t *iter = rx_curr;
    while (1) {
        uint32_t status = iter->status;
        if (status & RX_DESC_STAT_OWN) {
            break;
        }
        if (status & RX_DESC_STAT_LS) {
            DEBUG("[stm32_eth] Lost RX IRQ, sending event to upper layer\n");
            /* we use the ISR event for this, as the upper layer calls recv()
             * right away on an NETDEV_EVENT_RX_COMPLETE. Because there could be
             * potentially quite a lot of received frames in the queue, we might
             * risk a stack overflow if we would send an
             * NETDEV_EVENT_RX_COMPLETE
             */
            netdev_trigger_event_isr(_netdev);
            break;
        }
        iter = iter->desc_next;
    }
}

static int stm32_eth_recv(netdev_t *netdev, void *buf, size_t max_len,
                          void *info)
{
    (void)info;
    (void)netdev;
    char *data = buf;
    /* Determine the size of received frame. The frame might span multiple
     * DMA buffers */
    int size = get_rx_frame_size();

    if (size < 0) {
        if (size != -EAGAIN) {
            DEBUG("[stm32_eth] Dropping frame due to error\n");
            drop_frame_and_update_rx_curr();
        }
        return size;
    }

    if (!buf) {
        if (max_len) {
            DEBUG("[stm32_eth] Dropping frame as requested by upper layer\n");
            drop_frame_and_update_rx_curr();
        }
        return size;
    }

    if (max_len < (size_t)size) {
        DEBUG("[stm32_eth] Buffer provided by upper layer is too small\n");
        drop_frame_and_update_rx_curr();
        return -ENOBUFS;
    }

    size_t remain = size;
    while (remain) {
        size_t chunk = MIN(remain, ETH_RX_BUFFER_SIZE);
        memcpy(data, rx_curr->buffer_addr, chunk);
        data += chunk;
        remain -= chunk;
        /* Hand over descriptor to DMA */
        rx_curr->status = RX_DESC_STAT_OWN;
        rx_curr = rx_curr->desc_next;
    }

    if ((size + ETHERNET_FCS_LEN - 1) % ETH_RX_BUFFER_SIZE < ETHERNET_FCS_LEN) {
        /* one additional rx descriptor was needed only for the FCS, hand that
         * back to the DMA as well */
        rx_curr->status = RX_DESC_STAT_OWN;
        rx_curr = rx_curr->desc_next;
    }

    _debug_rx_descriptor_info(__LINE__);
    handle_lost_rx_irqs();
    return size;
}

void stm32_eth_isr_eth_wkup(void)
{
    cortexm_isr_end();
}

static void stm32_eth_isr(netdev_t *netdev)
{
#if IS_USED(MODULE_STM32_ETH_LINK_UP)
    switch (_link_state) {
    case LINK_STATE_UP:
        DEBUG("[stm32_eth] Link UP\n");
        if (IS_USED(MODULE_STM32_ETH_AUTO)) {
            /* Complete auto-negotiation of the link */
            _complete_auto_negotiation();
        }
        netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);
        _link_state = LINK_STATE_NOTIFIED_UP;
        return;
    case LINK_STATE_DOWN:
        DEBUG("[stm32_eth] Link DOWN\n");
        netdev->event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
        _link_state = LINK_STATE_NOTIFIED_DOWN;
        return;
    default:
        break;
    }
#endif

    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

void isr_eth(void)
{
    unsigned tmp = ETH->DMASR;

    if ((tmp & ETH_DMASR_TS)) {
        ETH->DMASR = ETH_DMASR_NIS | ETH_DMASR_TS;
        DEBUG("isr_eth: TX completed\n");
        mutex_unlock(&stm32_eth_tx_completed);
    }

    if ((tmp & ETH_DMASR_RS)) {
        ETH->DMASR = ETH_DMASR_NIS | ETH_DMASR_RS;
        DEBUG("isr_eth: RX completed\n");
        if (_netdev) {
            netdev_trigger_event_isr(_netdev);
        }
    }

    cortexm_isr_end();
}

static const netdev_driver_t netdev_driver_stm32f4eth = {
    .send = stm32_eth_send,
    .recv = stm32_eth_recv,
    .init = stm32_eth_init,
    .isr = stm32_eth_isr,
    .get = stm32_eth_get,
    .set = stm32_eth_set,
};

void stm32_eth_netdev_setup(netdev_t *netdev)
{
    _netdev = netdev;
    netdev->driver = &netdev_driver_stm32f4eth;
}
