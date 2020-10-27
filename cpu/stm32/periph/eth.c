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
#include <errno.h>
#include <string.h>

#include "bitarithm.h"
#include "iolist.h"
#include "luid.h"
#include "mutex.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if IS_USED(MODULE_STM32_ETH_LINK_UP)
#include "xtimer.h"

#define STM32_ETH_LINK_UP_TIMEOUT_US    (1UL * US_PER_SEC)

static xtimer_t _link_status_timer;
#endif

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
#define ETH_RX_DESCRIPTOR_COUNT (6U)
#endif
#ifndef ETH_TX_DESCRIPTOR_COUNT
#define ETH_TX_DESCRIPTOR_COUNT (8U)
#endif
#ifndef ETH_RX_BUFFER_SIZE
#define ETH_RX_BUFFER_SIZE  (256U)
#endif

#define LINK_STATE_DOWN         (0x00)
#define LINK_STATE_UP           (0x01)
#define LINK_STATE_NOTIFIED_UP  (0x02)

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
#endif

/** Read or write a phy register, to write the register ETH_MACMIIAR_MW is to
 * be passed as the higher nibble of the value */
static unsigned _rw_phy(unsigned addr, unsigned reg, unsigned value)
{
    unsigned tmp;

    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}
    DEBUG("[stm32_eth] rw_phy %x (%x): %x\n", addr, reg, value);

    tmp = (ETH->MACMIIAR & ETH_MACMIIAR_CR) | ETH_MACMIIAR_MB;
    tmp |= (((addr & 0x1f) << 11) | ((reg & 0x1f) << 6));
    tmp |= (value >> 16);

    ETH->MACMIIDR = (value & 0xffff);
    ETH->MACMIIAR = tmp;
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}

    DEBUG("[stm32_eth] %lx\n", ETH->MACMIIDR);
    return (ETH->MACMIIDR & 0x0000ffff);
}

static inline int32_t _phy_read(uint16_t addr, uint8_t reg)
{
    return _rw_phy(addr, reg, 0);
}

static inline void _phy_write(uint16_t addr, uint8_t reg, uint16_t value)
{
    _rw_phy(addr, reg, (value & 0xffff) | (ETH_MACMIIAR_MW << 16));
}

static inline bool _get_link_status(void)
{
    return (_phy_read(0, PHY_BSMR) & BSMR_LINK_STATUS);
}

static void stm32_eth_get_mac(char *out)
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
static void stm32_eth_set_mac(const char *mac)
{
    ETH->MACA0HR &= 0xffff0000;
    ETH->MACA0HR |= ((mac[5] << 8) | mac[4]);
    ETH->MACA0LR = ((mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0]);
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
        stm32_eth_set_mac((char *)value);
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
        stm32_eth_get_mac((char *)value);
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
    if (_get_link_status()) {
        _link_state = LINK_STATE_UP;
        dev->event_callback(dev, NETDEV_EVENT_ISR);
    }
    else {
        _link_state = LINK_STATE_DOWN;
        xtimer_set(&_link_status_timer, STM32_ETH_LINK_UP_TIMEOUT_US);
    }
}
#endif

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

    /* configure the PHY (standard for all PHY's) */
    /* if there's no PHY, this has no effect */
    _phy_write(eth_config.phy_addr, PHY_BMCR, BMCR_RESET);

    /* speed from conf */
    ETH->MACCR |= (ETH_MACCR_ROD | ETH_MACCR_IPCO | ETH_MACCR_APCS |
                   ((eth_config.speed & 0x0100) << 3) |
                   ((eth_config.speed & 0x2000) << 1));

    /* pass all */
    //ETH->MACFFR |= ETH_MACFFR_RA;
    /* pass on perfect filter match and pass all multicast address matches */
    ETH->MACFFR |= ETH_MACFFR_PAM;

    /* store forward */
    ETH->DMAOMR |= (ETH_DMAOMR_RSF | ETH_DMAOMR_TSF | ETH_DMAOMR_OSF);

    /* configure DMA */
    ETH->DMABMR = (ETH_DMABMR_DA | ETH_DMABMR_AAB | ETH_DMABMR_FB |
                   ETH_DMABMR_RDP_32Beat | ETH_DMABMR_PBL_32Beat | ETH_DMABMR_EDE);

    if(eth_config.mac[0] != 0) {
      stm32_eth_set_mac(eth_config.mac);
    }
    else {
      eui48_t hwaddr;
      luid_get_eui48(&hwaddr);
      stm32_eth_set_mac((const char *)hwaddr.uint8);
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

    /* configure speed, do it at the end so the PHY had time to
     * reset */
    _phy_write(eth_config.phy_addr, PHY_BMCR, eth_config.speed);

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
    while (1) {
        uint32_t status = tx_desc[i].status;
        DEBUG("TX desc %u status: ES=%c, UF=%c, EC=%c, NC=%c, FS=%c, LS=%c\n",
              i,
              (status & TX_DESC_STAT_ES) ? '1' : '0',
              (status & TX_DESC_STAT_UF) ? '1' : '0',
              (status & TX_DESC_STAT_EC) ? '1' : '0',
              (status & TX_DESC_STAT_NC) ? '1' : '0',
              (status & TX_DESC_STAT_FS) ? '1' : '0',
              (status & TX_DESC_STAT_LS) ? '1' : '0');
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
    edma_desc_t *i = rx_curr;
    uint32_t status;
    while (1) {
        /* Wait until DMA gave up control over descriptor */
        if ((status = i->status) & RX_DESC_STAT_OWN) {
            DEBUG("[stm32_eth] RX not completed (spurious interrupt?)\n");
            return -EAGAIN;
        }
        DEBUG("[stm32_eth] get_rx_frame_size(): FS=%c, LS=%c, ES=%c, DE=%c, FL=%lu\n",
              (status & RX_DESC_STAT_FS) ? '1' : '0',
              (status & RX_DESC_STAT_LS) ? '1' : '0',
              (status & RX_DESC_STAT_ES) ? '1' : '0',
              (status & RX_DESC_STAT_DE) ? '1' : '0',
              ((status >> 16) & 0x3fff) - ETHERNET_FCS_LEN);
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

    /* bits 16-29 contain the frame length including 4 B frame check sequence */
    return ((status >> 16) & 0x3fff) - ETHERNET_FCS_LEN;
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
            DEBUG("[stm32_eth] Lost RX IRQ, sending event to upper layer now\n");
            /* we use the ISR event for this, as the upper layer calls recv()
             * right away on an NETDEV_EVENT_RX_COMPLETE. Because there could be
             * potentially quite a lot of received frames in the queue, we might
             * risk a stack overflow if we would send an NETDEV_EVENT_RX_COMPLETE
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
        netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);
        _link_state = LINK_STATE_NOTIFIED_UP;
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
