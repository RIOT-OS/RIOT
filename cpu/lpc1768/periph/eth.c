/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lpc1768
 *
 * @{
 * @file
 * @brief       Low-level ethernet driver implementation
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 * @}
 */

#include <errno.h>
#include <string.h>

#include "cpu.h"
#include "config.h"
#include "mii.h"
#include "net/ethernet.h"
#include "net/netdev.h"
#include "periph/eth.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "vendor/LPC17xx.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#define ENABLE_TRACE 0
#include "debug.h"

#if ENABLE_TRACE
#  define TRACE(...)      DEBUG(__VA_ARGS__)
#  define TRACE_PUTS(...) DEBUG_PUTS(__VA_ARGS__)
#else
#  define TRACE(...)
#  define TRACE_PUTS(...)
#endif

/**
 * @brief   Per-buffer length (one MTU + headroom)
 */
#define LPC1768_ETH_BUF_LEN     (1536U)

/* SC.PCONP */
#define PCONP_PCENET            (1UL << 30)

/* MAC1 register bits */
#define MAC1_RECEIVE_ENABLE     (1UL << 0)
#define MAC1_PASS_ALL_RX        (1UL << 1)
#define MAC1_RX_FLOWCTRL        (1UL << 2)
#define MAC1_TX_FLOWCTRL        (1UL << 3)
#define MAC1_LOOPBACK           (1UL << 4)
#define MAC1_RESET_TX           (1UL << 8)
#define MAC1_RESET_MCS_TX       (1UL << 9)
#define MAC1_RESET_RX           (1UL << 10)
#define MAC1_RESET_MCS_RX       (1UL << 11)
#define MAC1_SIMUL_RESET        (1UL << 14)
#define MAC1_SOFT_RESET         (1UL << 15)

/* MAC2 register bits */
#define MAC2_FULL_DUPLEX        (1UL << 0)
#define MAC2_FRAME_LEN_CHECK    (1UL << 1)
#define MAC2_HUGE_FRAME_EN      (1UL << 2)
#define MAC2_DELAYED_CRC        (1UL << 3)
#define MAC2_CRC_EN             (1UL << 4)
#define MAC2_PAD_CRC_EN         (1UL << 5)
#define MAC2_VLAN_PAD_EN        (1UL << 6)
#define MAC2_AUTO_DETECT_PAD_EN (1UL << 7)
#define MAC2_PURE_PREAMBLE_ENF  (1UL << 8)
#define MAC2_LONG_PREAMBLE_ENF  (1UL << 9)
#define MAC2_NO_BACKOFF         (1UL << 12)
#define MAC2_BP_NO_BACKOFF      (1UL << 13)
#define MAC2_EXCESS_DEFER       (1UL << 14)

/* SUPP register bits */
#define SUPP_SPEED              (1UL << 8)

/* MCFG register bits */
#define MCFG_RESET_MIIM         (1UL << 15)

/* MCMD register bits */
#define MCMD_READ               (1UL << 0)
#define MCMD_SCAN               (1UL << 1)

/* MIND register bits */
#define MIND_BUSY               (1UL << 0)

/* COMMAND register bits */
#define COMMAND_RX_ENABLE       (1UL << 0)
#define COMMAND_TX_ENABLE       (1UL << 1)
#define COMMAND_REG_RESET       (1UL << 3)
#define COMMAND_TX_RESET        (1UL << 4)
#define COMMAND_RX_RESET        (1UL << 5)
#define COMMAND_PASS_RUNT_FRAME (1UL << 6)
#define COMMAND_PASS_RX_FILTER  (1UL << 7)
#define COMMAND_RMII            (1UL << 9)
#define COMMAND_FULL_DUPLEX     (1UL << 10)

/* RxFilterCtrl bits */
#define RFC_ACCEPT_UNICAST      (1UL << 0)
#define RFC_ACCEPT_BROADCAST    (1UL << 1)
#define RFC_ACCEPT_MULTICAST    (1UL << 2)
#define RFC_ACCEPT_PERFECT      (1UL << 5)
#define RFC_MAGIC_PACKET_WOL    (1UL << 12)
#define RFC_RX_FILTER_WOL       (1UL << 13)

/* IntStatus / IntEnable bits */
#define INT_RX_OVERRUN          (1UL << 0)
#define INT_RX_ERROR            (1UL << 1)
#define INT_RX_FINISHED         (1UL << 2)
#define INT_RX_DONE             (1UL << 3)
#define INT_TX_UNDERRUN         (1UL << 4)
#define INT_TX_ERROR            (1UL << 5)
#define INT_TX_FINISHED         (1UL << 6)
#define INT_TX_DONE             (1UL << 7)

/* Status register bits */
#define STATUS_RX_ACTIVE        (1UL << 0)
#define STATUS_TX_ACTIVE        (1UL << 1)

/* TX descriptor control word */
#define TX_CTRL_SIZE_MASK       (0x7FFUL)
#define TX_CTRL_OVERRIDE        (1UL << 26)
#define TX_CTRL_HUGE            (1UL << 27)
#define TX_CTRL_PAD             (1UL << 28)
#define TX_CTRL_CRC             (1UL << 29)
#define TX_CTRL_LAST            (1UL << 30)
#define TX_CTRL_INT             (1UL << 31)

/* TX status info word */
#define TX_INFO_COLLISION_CNT_MASK  (0xFUL << 21)
#define TX_INFO_DEFER               (1UL << 25)
#define TX_INFO_EXCESSIVE_DEFER     (1UL << 26)
#define TX_INFO_EXCESSIVE_COLLISION (1UL << 27)
#define TX_INFO_LATE_COLLISION      (1UL << 28)
#define TX_INFO_UNDERRUN            (1UL << 29)
#define TX_INFO_NO_DESCRIPTOR       (1UL << 30)
#define TX_INFO_ERROR               (1UL << 31)

/* RX descriptor control word */
#define RX_CTRL_SIZE_MASK       (0x7FFUL)
#define RX_CTRL_INT             (1UL << 31)

/* RX status info word */
#define RX_INFO_SIZE_MASK       (0x7FFUL)
#define RX_INFO_CONTROL         (1UL << 18)
#define RX_INFO_VLAN            (1UL << 19)
#define RX_INFO_FAIL_FILTER     (1UL << 20)
#define RX_INFO_MULTICAST       (1UL << 21)
#define RX_INFO_BROADCAST       (1UL << 22)
#define RX_INFO_CRC_ERROR       (1UL << 23)
#define RX_INFO_SYMBOL_ERROR    (1UL << 24)
#define RX_INFO_LENGTH_ERROR    (1UL << 25)
#define RX_INFO_RANGE_ERROR     (1UL << 26)
#define RX_INFO_ALIGNMENT_ERROR (1UL << 27)
#define RX_INFO_OVERRUN         (1UL << 28)
#define RX_INFO_NO_DESCRIPTOR   (1UL << 29)
#define RX_INFO_LAST_FLAG       (1UL << 30)
#define RX_INFO_ERROR           (1UL << 31)

typedef struct {
    uint32_t packet;
    uint32_t control;
} desc_t;

typedef struct {
    uint32_t info;
    uint32_t hash_crc;
} rx_status_t;

typedef struct {
    uint32_t info;
} tx_status_t;

static desc_t _rx_desc[CONFIG_LPC1768_ETH_RX_BUF_NUMOF]
__attribute__((section(".eth_ram"), aligned(8)));
static rx_status_t _rx_stat[CONFIG_LPC1768_ETH_RX_BUF_NUMOF]
__attribute__((section(".eth_ram"), aligned(8)));
static uint8_t _rx_buf[CONFIG_LPC1768_ETH_RX_BUF_NUMOF][LPC1768_ETH_BUF_LEN]
__attribute__((section(".eth_ram"), aligned(8)));

static desc_t _tx_desc[CONFIG_LPC1768_ETH_TX_BUF_NUMOF]
__attribute__((section(".eth_ram"), aligned(8)));
static tx_status_t _tx_stat[CONFIG_LPC1768_ETH_TX_BUF_NUMOF]
__attribute__((section(".eth_ram"), aligned(8)));
static uint8_t _tx_buf[CONFIG_LPC1768_ETH_TX_BUF_NUMOF][LPC1768_ETH_BUF_LEN]
__attribute__((section(".eth_ram"), aligned(8)));

static uint8_t _last_tx_idx;
static size_t _last_tx_len;

static void _filter_error_frames(void)
{
    /* cannot use RX_INFO_ERROR, because that includes RX_INFO_RANGE_ERROR
     * which is not an error, but only an indication that the frame exceeds
     * 1500 bytes */
    const uint32_t error_mask = RX_INFO_CRC_ERROR | RX_INFO_SYMBOL_ERROR | RX_INFO_ALIGNMENT_ERROR
                              | RX_INFO_OVERRUN | RX_INFO_NO_DESCRIPTOR | RX_INFO_FAIL_FILTER;

    uint32_t produce = LPC_EMAC->RxProduceIndex;
    uint32_t consume = LPC_EMAC->RxConsumeIndex;

    /* drain any error frames sitting at the head of the ring */
    while (produce != consume) {
        uint32_t info_word = _rx_stat[consume].info;

        if (!(info_word & error_mask)) {
            /* valid frame found */
            return;
        }

        consume = (consume + 1) % CONFIG_LPC1768_ETH_RX_BUF_NUMOF;
        LPC_EMAC->RxConsumeIndex = consume;
    }
}

static bool _mii_wait_idle(void)
{
    uint32_t start = ztimer_now(ZTIMER_USEC);

    while (LPC_EMAC->MIND & MIND_BUSY) {
        if (ztimer_now(ZTIMER_USEC) - start > CONFIG_LPC1768_ETH_PHY_TIMEOUT_US) {
            DEBUG_PUTS("[eth] _mii_wait_idle: timeout");
            return false;
        }
    }

    return true;
}

static uint16_t _mii_read(uint8_t reg)
{
    LPC_EMAC->MADR = ((uint32_t)eth_config.phy_addr << 8) | (reg & 0x1F);
    LPC_EMAC->MCMD = MCMD_READ;

    if (!_mii_wait_idle()) {
        LPC_EMAC->MCMD = 0;
        return 0xFFFF;
    }

    LPC_EMAC->MCMD = 0;

    uint16_t val = LPC_EMAC->MRDD;

    TRACE("[eth] _mii_read: reg=0x%02x val=0x%04x\n", reg, val);

    return val;
}

static void _mii_write(uint8_t reg, uint16_t data)
{
    TRACE("[eth] _mii_write: reg=0x%02x val=0x%04x\n", reg, data);

    LPC_EMAC->MADR = ((uint32_t)eth_config.phy_addr << 8) | (reg & 0x1F);
    LPC_EMAC->MWTD = data;

    _mii_wait_idle();
}

static int _phy_reset(void)
{
    _mii_write(MII_BMCR, MII_BMCR_RESET);

    for (unsigned i = 0; i < 1000; i++) {
        if (!(_mii_read(MII_BMCR) & MII_BMCR_RESET)) {
            return 0;
        }

        ztimer_sleep(ZTIMER_MSEC, 1);
    }

    DEBUG_PUTS("[eth] _phy_reset: timed out");

    return -ENODEV;
}

static int _phy_detect(void)
{
    uint16_t bmsr = _mii_read(MII_BMSR);

    if (bmsr == 0x0000 || bmsr == 0xFFFF) {
        DEBUG_PUTS("[eth] _phy_detect: PHY not detected");
        return -EIO;
    }

    DEBUG("[eth] _phy_detect: PHY detected (BMSR=0x%04x)\n", (unsigned)bmsr);

    return 0;
}

static void _init_pins(void)
{
    /* the ethernet peripheral has fixed alternative function mappings on
     * P1.0 - P1.17, two bits per pin, selecting function 1. */
    const uint32_t pinsel2 =
        (1U << (0  * 2)) |  /* P1.0  ENET_TXD0 */
        (1U << (1  * 2)) |  /* P1.1  ENET_TXD1 */
        (1U << (4  * 2)) |  /* P1.4  ENET_TX_EN */
        (1U << (8  * 2)) |  /* P1.8  ENET_CRS */
        (1U << (9  * 2)) |  /* P1.9  ENET_RXD0 */
        (1U << (10 * 2)) |  /* P1.10 ENET_RXD1 */
        (1U << (14 * 2)) |  /* P1.14 ENET_RX_ER */
        (1U << (15 * 2));   /* P1.15 ENET_REF_CLK */
    const uint32_t pinsel3 =
        (1U << (0  * 2)) |  /* P1.16 ENET_MDC */
        (1U << (1  * 2));   /* P1.17 ENET_MDIO */

    LPC_PINCON->PINSEL2 |=  pinsel2;
    LPC_PINCON->PINSEL2 &= ~(pinsel2 << 1);
    LPC_PINCON->PINSEL3 |=  pinsel3;
    LPC_PINCON->PINSEL3 &= ~(pinsel3 << 1);

    /* enable the PHY if configured */
    if (gpio_is_valid(eth_config.phy_en_pin)) {
        gpio_init(eth_config.phy_en_pin, GPIO_OUT);
        gpio_set(eth_config.phy_en_pin);
    }

    /* reset the PHY if configured */
    if (gpio_is_valid(eth_config.phy_rst_pin)) {
        gpio_init(eth_config.phy_rst_pin, GPIO_OUT);

        gpio_clear(eth_config.phy_rst_pin);
        ztimer_sleep(ZTIMER_MSEC, 10);
        gpio_set(eth_config.phy_rst_pin);
        ztimer_sleep(ZTIMER_MSEC, 10);
    }
}

static void _init_clock(void)
{
    /* the MDC must be <= 2.5 MHz (IEEE 802.3), so select the smallest divider
     * whose resulting frequency does not exceed that limit at the current core
     * clock. */
#if CLOCK_CORECLOCK <= (4U * 2500000U)
    LPC_EMAC->MCFG = (0x1UL << 2);
#elif CLOCK_CORECLOCK <= (6U * 2500000U)
    LPC_EMAC->MCFG = (0x2UL << 2);
#elif CLOCK_CORECLOCK <= (8U * 2500000U)
    LPC_EMAC->MCFG = (0x3UL << 2);
#elif CLOCK_CORECLOCK <= (10U * 2500000U)
    LPC_EMAC->MCFG = (0x4UL << 2);
#elif CLOCK_CORECLOCK <= (14U * 2500000U)
    LPC_EMAC->MCFG = (0x5UL << 2);
#elif CLOCK_CORECLOCK <= (20U * 2500000U)
    LPC_EMAC->MCFG = (0x6UL << 2);
#elif CLOCK_CORECLOCK <= (28U * 2500000U)
    LPC_EMAC->MCFG = (0x7UL << 2);
#elif CLOCK_CORECLOCK <= (36U * 2500000U)
    LPC_EMAC->MCFG = (0x8UL << 2);
#elif CLOCK_CORECLOCK <= (40U * 2500000U)
    LPC_EMAC->MCFG = (0x9UL << 2);
#elif CLOCK_CORECLOCK <= (44U * 2500000U)
    LPC_EMAC->MCFG = (0xAUL << 2);
#elif CLOCK_CORECLOCK <= (48U * 2500000U)
    LPC_EMAC->MCFG = (0xBUL << 2);
#elif CLOCK_CORECLOCK <= (52U * 2500000U)
    LPC_EMAC->MCFG = (0xCUL << 2);
#elif CLOCK_CORECLOCK <= (56U * 2500000U)
    LPC_EMAC->MCFG = (0xDUL << 2);
#elif CLOCK_CORECLOCK <= (60U * 2500000U)
    LPC_EMAC->MCFG = (0xEUL << 2);
#elif CLOCK_CORECLOCK <= (64U * 2500000U)
    LPC_EMAC->MCFG = (0xFUL << 2);
#else
#  error "CLOCK_CORECLOCK too high for EMAC MDC (> 160 MHz)"
#endif

    /* wait for clock to stabilize */
    ztimer_sleep(ZTIMER_MSEC, 10);
}

static void _init_descriptors(void)
{
    memset(_rx_desc, 0, sizeof(_rx_desc));
    memset(_rx_stat, 0, sizeof(_rx_stat));
    memset(_tx_desc, 0, sizeof(_tx_desc));
    memset(_tx_stat, 0, sizeof(_tx_stat));

    for (unsigned i = 0; i < CONFIG_LPC1768_ETH_RX_BUF_NUMOF; i++) {
        _rx_desc[i].packet = (uint32_t)_rx_buf[i];
        _rx_desc[i].control = RX_CTRL_INT | (LPC1768_ETH_BUF_LEN - 1);
    }

    for (unsigned i = 0; i < CONFIG_LPC1768_ETH_TX_BUF_NUMOF; i++) {
        _tx_desc[i].packet = (uint32_t)_tx_buf[i];
    }

    LPC_EMAC->RxDescriptor = (uint32_t)_rx_desc;
    LPC_EMAC->RxStatus = (uint32_t)_rx_stat;
    LPC_EMAC->RxDescriptorNumber = CONFIG_LPC1768_ETH_RX_BUF_NUMOF - 1;
    LPC_EMAC->RxConsumeIndex = 0;

    LPC_EMAC->TxDescriptor = (uint32_t)_tx_desc;
    LPC_EMAC->TxStatus = (uint32_t)_tx_stat;
    LPC_EMAC->TxDescriptorNumber = CONFIG_LPC1768_ETH_TX_BUF_NUMOF - 1;
    LPC_EMAC->TxProduceIndex = 0;

    _last_tx_idx = ARRAY_SIZE(_tx_desc);
    _last_tx_len = 0;
}

int lpc1768_eth_init(const uint8_t *mac)
{
    DEBUG_PUTS("[eth] lpc1768_eth_init: initializing peripheral");

    /* enable clock to peripheral */
    LPC_SC->PCONP |= PCONP_PCENET;

    _init_pins();

    LPC_EMAC->MAC1 = MAC1_RESET_TX | MAC1_RESET_MCS_TX | MAC1_RESET_RX | MAC1_RESET_MCS_RX
                   | MAC1_SIMUL_RESET | MAC1_SOFT_RESET;
    LPC_EMAC->Command = COMMAND_REG_RESET | COMMAND_TX_RESET | COMMAND_RX_RESET;

    ztimer_sleep(ZTIMER_MSEC, 10);

    LPC_EMAC->MAC1 = 0;
    LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_CRC_EN;
    LPC_EMAC->MAXF = LPC1768_ETH_BUF_LEN;
    LPC_EMAC->IPGR = 0xC12; /* default inter-packet gap: 96-bit times */
    LPC_EMAC->CLRT = 0x370; /* default collision window / retry: 24 + 36 = 60 bit times */

    LPC_EMAC->Command = COMMAND_RMII;

    _init_clock();

    /* reset the MII interface */
    LPC_EMAC->MCFG |= MCFG_RESET_MIIM;
    LPC_EMAC->MCFG &= ~MCFG_RESET_MIIM;

    /* reset the phy */
    int res = _phy_reset();

    if (res < 0) {
        return res;
    }

    res = _phy_detect();

    if (res < 0) {
        return res;
    }

    /* configure MAC address */
    lpc1768_eth_set_mac(mac);

    /* configure default (fallback) link speed */
    lpc1768_eth_set_link_speed(eth_config.speed);

    _init_descriptors();

    LPC_EMAC->RxFilterCtrl = RFC_ACCEPT_UNICAST | RFC_ACCEPT_BROADCAST | RFC_ACCEPT_PERFECT;

    /* configure interrupts */
    LPC_EMAC->IntClear = 0xFFFFFFFFUL;
    LPC_EMAC->IntEnable = INT_RX_DONE | INT_RX_OVERRUN | INT_RX_ERROR
                        | INT_TX_DONE | INT_TX_UNDERRUN | INT_TX_ERROR;

    NVIC_ClearPendingIRQ(ENET_IRQn);
    NVIC_EnableIRQ(ENET_IRQn);

    /* start processing */
    LPC_EMAC->Command |= COMMAND_RX_ENABLE | COMMAND_TX_ENABLE;
    LPC_EMAC->MAC1 |= MAC1_RECEIVE_ENABLE;

    return 0;
}

int lpc1768_eth_send(const iolist_t *iolist)
{
    uint32_t produce = LPC_EMAC->TxProduceIndex;
    uint32_t consume = LPC_EMAC->TxConsumeIndex;

    /* check for a free descriptor */
    uint32_t next = (produce + 1) % CONFIG_LPC1768_ETH_TX_BUF_NUMOF;

    if (next == consume) {
        DEBUG_PUTS("[eth] lpc1768_eth_send: TX descriptor busy");
        return -EBUSY;
    }

    /* copy the data into the descriptor's buffer */
    uint8_t *dst = _tx_buf[produce];
    size_t len = 0;

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (len + iol->iol_len > LPC1768_ETH_BUF_LEN) {
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy(dst + len, iol->iol_base, iol->iol_len);
            len += iol->iol_len;
        }
    }

    if (len < ETHERNET_MIN_LEN) {
        memset(dst + len, 0, ETHERNET_MIN_LEN - len);
        len = ETHERNET_MIN_LEN;
    }

    /* start transmission */
    _tx_desc[produce].control = ((len - 1) & TX_CTRL_SIZE_MASK) | TX_CTRL_LAST | TX_CTRL_INT
                              | TX_CTRL_PAD | TX_CTRL_CRC;
    __DMB();

    /* store the index and length for status checking after transmission */
    _last_tx_idx = produce;
    _last_tx_len = len;

    LPC_EMAC->TxProduceIndex = next;

    return (int)len;
}

int lpc1768_eth_recv(void *buf, size_t max_len)
{
    /* filter error frames first */
    _filter_error_frames();

    /* check if a frame is available */
    uint32_t produce = LPC_EMAC->RxProduceIndex;
    uint32_t consume = LPC_EMAC->RxConsumeIndex;

    TRACE("[eth] lpc1768_eth_recv: produce=%lu consume=%lu\n", produce, consume);

    if (produce == consume) {
        DEBUG_PUTS("[eth] lpc1768_eth_recv: no frame to receive");
        return -ENODATA;
    }

    /* process the received frame */
    uint32_t info_word = _rx_stat[consume].info;
    int len = (int)((info_word & RX_INFO_SIZE_MASK) + 1);

    DEBUG("[eth] lpc1768_eth_recv: frame length %d bytes\n", len);

    if (buf == NULL && max_len == 0) {
        return len;
    }

    if (buf != NULL) {
        size_t copy = (len > (int)max_len) ? max_len : (size_t)len;
        memcpy(buf, _rx_buf[consume], copy);
    }

    /* mark as available */
    uint32_t next = (consume + 1) % CONFIG_LPC1768_ETH_RX_BUF_NUMOF;
    LPC_EMAC->RxConsumeIndex = next;

    return len;
}

void lpc1768_eth_get_mac(uint8_t out[6])
{
    uint32_t sa0 = LPC_EMAC->SA0;
    uint32_t sa1 = LPC_EMAC->SA1;
    uint32_t sa2 = LPC_EMAC->SA2;

    out[0] = (uint8_t)(sa2 >> 8);
    out[1] = (uint8_t)(sa2 & 0xFF);
    out[2] = (uint8_t)(sa1 >> 8);
    out[3] = (uint8_t)(sa1 & 0xFF);
    out[4] = (uint8_t)(sa0 >> 8);
    out[5] = (uint8_t)(sa0 & 0xFF);
}

void lpc1768_eth_set_mac(const uint8_t mac[6])
{
    DEBUG("[eth] lpc1768_eth_set_mac: setting MAC address to %02x:%02x:%02x:%02x:%02x:%02x\n",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    LPC_EMAC->SA0 = ((uint32_t)mac[4] << 8) | mac[5];
    LPC_EMAC->SA1 = ((uint32_t)mac[2] << 8) | mac[3];
    LPC_EMAC->SA2 = ((uint32_t)mac[0] << 8) | mac[1];
}

void lpc1768_eth_set_link_speed(uint16_t speed)
{
    DEBUG("[eth] lpc1768_eth_set_link_speed: %s Mbps %s duplex\n",
          (speed & MII_BMCR_SPEED_100) ? "100" : "10",
          (speed & MII_BMCR_FULL_DPLX) ? "full" : "half");

    /* configure the link speed */
    if (speed & MII_BMCR_SPEED_100) {
        LPC_EMAC->SUPP |= SUPP_SPEED;
    }
    else {
        LPC_EMAC->SUPP &= ~SUPP_SPEED;
    }

    /* configure duplex */
    if (speed & MII_BMCR_FULL_DPLX) {
        LPC_EMAC->MAC2 |= MAC2_FULL_DUPLEX;
        LPC_EMAC->Command |= COMMAND_FULL_DUPLEX;
        LPC_EMAC->IPGT = 0x15UL; /* recommended full-duplex IPG */
    }
    else {
        LPC_EMAC->MAC2 &= ~MAC2_FULL_DUPLEX;
        LPC_EMAC->Command &= ~COMMAND_FULL_DUPLEX;
        LPC_EMAC->IPGT = 0x12UL; /* recommended half-duplex IPG */
    }
}

bool lpc1768_eth_link_up(void)
{
    return (_mii_read(MII_BMSR) & MII_BMSR_LINK) != 0;
}

bool lpc1768_eth_rx_pending(void)
{
    return LPC_EMAC->RxProduceIndex != LPC_EMAC->RxConsumeIndex;
}

int lpc1768_eth_tx_status(void)
{
    /* method must only be called if after a transmission */
    assert(_last_tx_idx < ARRAY_SIZE(_tx_desc));

    /* the frame is still in flight until the engine consumes its descriptor */
    if (LPC_EMAC->TxConsumeIndex == _last_tx_idx) {
        return -EAGAIN;
    }

    uint32_t info = _tx_stat[_last_tx_idx].info;

    /* a collision aborted the frame, report as busy so the upper layer may
     * retry */
    if (info & (TX_INFO_LATE_COLLISION | TX_INFO_EXCESSIVE_COLLISION)) {
        return -EBUSY;
    }

    /* any other error (underrun, excessive defer, no descriptor) is fatal for
     * this frame */
    if (info & TX_INFO_ERROR) {
        return -EIO;
    }

    return (int)_last_tx_len;
}

bool lpc1768_eth_get_promiscuous(void)
{
    return (LPC_EMAC->RxFilterCtrl & RFC_ACCEPT_PERFECT) == 0;
}

void lpc1768_eth_set_promiscuous(bool enable)
{
    if (enable) {
        LPC_EMAC->RxFilterCtrl = RFC_ACCEPT_UNICAST | RFC_ACCEPT_BROADCAST | RFC_ACCEPT_MULTICAST;
    }
    else {
        LPC_EMAC->RxFilterCtrl = RFC_ACCEPT_UNICAST | RFC_ACCEPT_BROADCAST | RFC_ACCEPT_PERFECT;
    }
}

int lpc1768_eth_start_auto_negotiation(void)
{
    DEBUG_PUTS("[eth] lpc1768_eth_start_auto_negotiation: starting auto-negotiation");

    /* check if auto-negotiation is supported */
    if (!(_mii_read(MII_BMSR) & MII_BMSR_HAS_AN)) {
        DEBUG_PUTS("[eth] lpc1768_start_auto_negotiation: not supported by PHY");
        return -EOPNOTSUPP;
    }

    /* advertise all capabilities and restart auto-negotiation */
    _mii_write(MII_ADVERTISE,
               MII_ADVERTISE_10_F | MII_ADVERTISE_10_H | MII_ADVERTISE_100_F | MII_ADVERTISE_100_H);
    _mii_write(MII_BMCR, MII_BMCR_AN_ENABLE | MII_BMCR_AN_RESTART);

    return 0;
}

void lpc1768_eth_complete_auto_negotiation(void)
{
    uint32_t deadline = ztimer_now(ZTIMER_MSEC) + CONFIG_LPC1768_ETH_AN_TIMEOUT_MS;
    uint16_t bmsr;

    /* wait until AN has completed or the link drops */
    do {
        bmsr = _mii_read(MII_BMSR);

        if (!(bmsr & MII_BMSR_LINK)) {
            DEBUG_PUTS("[eth] lpc1768_eth_complete_auto_negotiation: link dropped");
            return;
        }

        if (ztimer_now(ZTIMER_MSEC) >= deadline) {
            DEBUG_PUTS("[eth] lpc1768_eth_complete_auto_negotiation: timed out");
            return;
        }

        ztimer_sleep(ZTIMER_MSEC, 1);
    } while (!(bmsr & MII_BMSR_AN_DONE));

    uint16_t adv = _mii_read(MII_ADVERTISE);
    uint16_t lpa = _mii_read(MII_LPA);

    /* derive link speed from advertised and negotiated speeds */
    uint16_t speed;

    if ((adv & MII_ADVERTISE_100) && (lpa & MII_LPA_100)) {
        speed = MII_BMCR_SPEED_100;

        if ((adv & MII_ADVERTISE_100_F) && (lpa & MII_LPA_100_F)) {
            speed |= MII_BMCR_FULL_DPLX;
        }
    }
    else if ((adv & MII_ADVERTISE_10) && (lpa & MII_LPA_10)) {
        speed = MII_BMCR_SPEED_10;

        if ((adv & MII_ADVERTISE_10_F) && (lpa & MII_LPA_10_F)) {
            speed |= MII_BMCR_FULL_DPLX;
        }
    }
    else {
        DEBUG_PUTS(
            "[eth] lpc1768_eth_complete_auto_negotiation: defaulting to configured link speed");
        return;
    }

    lpc1768_eth_set_link_speed(speed);
}

void isr_enet(void)
{
    uint32_t status = LPC_EMAC->IntStatus;

    LPC_EMAC->IntClear = status;

    /* deliver ISR event to netdev */
    extern netdev_t *lpc1768_eth_netdev;

    if (lpc1768_eth_netdev != NULL) {
        if (status & (INT_RX_DONE | INT_RX_OVERRUN | INT_RX_ERROR)) {
            netdev_trigger_event_isr(lpc1768_eth_netdev);
        }
        if (status & (INT_TX_DONE | INT_TX_UNDERRUN | INT_TX_ERROR)) {
            lpc1768_eth_netdev->event_callback(lpc1768_eth_netdev, NETDEV_EVENT_TX_COMPLETE);
        }
    }

    cortexm_isr_end();
}
