/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32
 *
 * @{
 * @file
 * @brief       Low-level ethernet driver implementation
 *
 * @note        This driver does not rely on an EMLIB abstraction for the
 *              ethernet peripheral, as it does not exist.
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 * @}
 */

#include <errno.h>
#include <string.h>

#include "cpu.h"
#include "mii.h"
#include "net/ethernet.h"
#include "net/netdev.h"
#include "periph/eth.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "ztimer.h"

#include "em_cmu.h"
#include "em_device.h"

#define ENABLE_DEBUG 0
#define ENABLE_TRACE 0
#include "debug.h"

#if ENABLE_TRACE
#  define TRACE(...) DEBUG(__VA_ARGS__)
#  define TRACE_PUTS(...) DEBUG_PUTS(__VA_ARGS__)
#else
#  define TRACE(...)
#  define TRACE_PUTS(...)
#endif

/**
 * @brief   Per-buffer length (one MTU + headroom)
 */
#define EFM32_ETH_BUF_LEN           (1536U)

/* NETWORKCFG bits not provided as named constants in the vendor header */
#define NETWORKCFG_CAF              (1UL << 4) /* Copy All Frames (promisc) */

/* GEM RX descriptor word 0 (addr) bits */
#define RX_ADDR_OWN                 (1UL << 0)
#define RX_ADDR_WRAP                (1UL << 1)
#define RX_ADDR_MASK                (~0x3UL)

/* GEM RX descriptor word 1 (status) bits */
#define RX_STATUS_LEN_MASK          (0x1FFFUL)
#define RX_STATUS_SOF               (1UL << 14)
#define RX_STATUS_EOF               (1UL << 15)

/* GEM TX descriptor word 1 (status) bits */
#define TX_STATUS_LEN_MASK          (0x3FFFUL)
#define TX_STATUS_LAST              (1UL << 15)
#define TX_STATUS_LATE_COLLISION    (1UL << 26) /* late collision, frame aborted */
#define TX_STATUS_AHB_ERROR         (1UL << 27) /* frame corruption (AHB error) */
#define TX_STATUS_RETRY_EXCEEDED    (1UL << 29) /* collision retry limit reached */
#define TX_STATUS_WRAP              (1UL << 30)
#define TX_STATUS_USED              (1UL << 31)

/* PHY management register fields */
#define PHYMNGMNT_WRITE10           (0x2UL << 16)
#define PHYMNGMNT_OP_WRITE          (0x1UL << 28)
#define PHYMNGMNT_OP_READ           (0x2UL << 28)
#define PHYMNGMNT_HEADER            (0x1UL << 30)

typedef struct {
    uint32_t addr;
    uint32_t status;
} gem_desc_t;

static gem_desc_t _rx_desc[CONFIG_EFM32_ETH_RX_BUF_NUMOF]
__attribute__((aligned(8)));
static gem_desc_t _tx_desc[CONFIG_EFM32_ETH_TX_BUF_NUMOF]
__attribute__((aligned(8)));
static uint8_t _rx_buf[CONFIG_EFM32_ETH_RX_BUF_NUMOF][EFM32_ETH_BUF_LEN]
__attribute__((aligned(8)));
static uint8_t _tx_buf[CONFIG_EFM32_ETH_TX_BUF_NUMOF][EFM32_ETH_BUF_LEN]
__attribute__((aligned(8)));

static unsigned _rx_idx;
static unsigned _tx_idx;
static int _tx_len;

static bool _mii_wait_idle(void)
{
    uint32_t start = ztimer_now(ZTIMER_USEC);

    while (!(ETH->NETWORKSTATUS & ETH_NETWORKSTATUS_MANDONE)) {
        if (ztimer_now(ZTIMER_USEC) - start > CONFIG_EFM32_ETH_PHY_TIMEOUT_US) {
            DEBUG_PUTS("[eth] _mii_wait_idle: timeout");
            return false;
        }
    }

    return true;
}

static uint16_t _mii_read(uint8_t reg)
{
    ETH->PHYMNGMNT = PHYMNGMNT_HEADER | PHYMNGMNT_OP_READ | PHYMNGMNT_WRITE10
                     | ((uint32_t)eth_config.phy_addr << 23)
                     | ((uint32_t)(reg & 0x1F) << 18);

    if (!_mii_wait_idle()) {
        return 0xFFFF;
    }

    uint16_t val = (uint16_t)(ETH->PHYMNGMNT & 0xFFFF);

    TRACE("[eth] _mii_read: reg=0x%02x val=0x%04x\n", reg, val);

    return val;
}

static void _mii_write(uint8_t reg, uint16_t data)
{
    TRACE("[eth] _mii_write: reg=0x%02x val=0x%04x\n", reg, data);

    ETH->PHYMNGMNT = PHYMNGMNT_HEADER | PHYMNGMNT_OP_WRITE | PHYMNGMNT_WRITE10
                     | ((uint32_t)eth_config.phy_addr << 23)
                     | ((uint32_t)(reg & 0x1F) << 18)
                     | data;

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

static void _init_clock(void)
{
    CMU_ClockEnable(cmuClock_ETH, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

    /* select HFXO as CMU_CLK2 source and route to PHY as RMII reference clock */
    CMU->CTRL |= CMU_CTRL_CLKOUTSEL2_HFXO;

    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_CMU_CLK2_PORT(5),
                    AF_CMU_CLK2_PIN(5), gpioModePushPull, 0);

    uint32_t temp = CMU->ROUTELOC0 & ~_CMU_ROUTELOC0_CLKOUT2LOC_MASK;
    CMU->ROUTELOC0 = temp | CMU_ROUTELOC0_CLKOUT2LOC_LOC5;
    CMU->ROUTEPEN |= CMU_ROUTEPEN_CLKOUT2PEN;
}

static void _init_pins(void)
{
    uint8_t rmii_loc = (eth_config.routeloc1 & _ETH_ROUTELOC1_RMIILOC_MASK)
                       >> _ETH_ROUTELOC1_RMIILOC_SHIFT;
    uint8_t mdio_loc = (eth_config.routeloc1 & _ETH_ROUTELOC1_MDIOLOC_MASK)
                       >> _ETH_ROUTELOC1_MDIOLOC_SHIFT;

    ETH->CTRL = ETH_CTRL_GBLCLKEN | ETH_CTRL_MIISEL_RMII;

    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_RMIITXD0_PORT(rmii_loc),
                    AF_ETH_RMIITXD0_PIN(rmii_loc), gpioModePushPull, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_RMIITXD1_PORT(rmii_loc),
                    AF_ETH_RMIITXD1_PIN(rmii_loc), gpioModePushPull, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_RMIITXEN_PORT(rmii_loc),
                    AF_ETH_RMIITXEN_PIN(rmii_loc), gpioModePushPull, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_RMIIRXD0_PORT(rmii_loc),
                    AF_ETH_RMIIRXD0_PIN(rmii_loc), gpioModeInput, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_RMIIRXD1_PORT(rmii_loc),
                    AF_ETH_RMIIRXD1_PIN(rmii_loc), gpioModeInput, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_RMIICRSDV_PORT(rmii_loc),
                    AF_ETH_RMIICRSDV_PIN(rmii_loc), gpioModeInput, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_RMIIRXER_PORT(rmii_loc),
                    AF_ETH_RMIIRXER_PIN(rmii_loc), gpioModeInput, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_MDIO_PORT(mdio_loc),
                    AF_ETH_MDIO_PIN(mdio_loc), gpioModePushPull, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)AF_ETH_MDC_PORT(mdio_loc),
                    AF_ETH_MDC_PIN(mdio_loc), gpioModePushPull, 0);

    ETH->ROUTELOC1 = eth_config.routeloc1;
    ETH->ROUTEPEN = ETH_ROUTEPEN_RMIIPEN | ETH_ROUTEPEN_MDIOPEN;

    /* enable the PHY if configured */
    if (gpio_is_valid(eth_config.phy_en_pin)) {
        gpio_init(eth_config.phy_en_pin, GPIO_OUT);
        gpio_set(eth_config.phy_en_pin);
        ztimer_sleep(ZTIMER_MSEC, 10);
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

static void _init_descriptors(void)
{
    memset(_rx_desc, 0, sizeof(_rx_desc));
    memset(_tx_desc, 0, sizeof(_tx_desc));

    for (unsigned i = 0; i < CONFIG_EFM32_ETH_RX_BUF_NUMOF; i++) {
        uint32_t addr = ((uint32_t)_rx_buf[i]) & RX_ADDR_MASK;

        if (i == CONFIG_EFM32_ETH_RX_BUF_NUMOF - 1) {
            addr |= RX_ADDR_WRAP;
        }

        _rx_desc[i].addr = addr;
    }

    for (unsigned i = 0; i < CONFIG_EFM32_ETH_TX_BUF_NUMOF; i++) {
        uint32_t status = TX_STATUS_USED;

        if (i == CONFIG_EFM32_ETH_TX_BUF_NUMOF - 1) {
            status |= TX_STATUS_WRAP;
        }

        _tx_desc[i].addr = (uint32_t)_tx_buf[i];
        _tx_desc[i].status = status;
    }

    _rx_idx = 0;
    _tx_idx = 0;

    ETH->RXQPTR = (uint32_t)_rx_desc;
    ETH->TXQPTR = (uint32_t)_tx_desc;
}

int efm32_eth_init(const uint8_t *mac)
{
    DEBUG_PUTS("[eth] efm32_eth_init: initializing peripheral");

    _init_clock();
    _init_pins();
    _init_descriptors();

    ETH->NETWORKCTRL = ETH_NETWORKCTRL_CLRALLSTATSREGS;
    ETH->NETWORKCFG = ETH_NETWORKCFG_FCSREMOVE | ETH_NETWORKCFG_RX1536BYTEFRAMES |
                      ETH_NETWORKCFG_MDCCLKDIV_DIVBY32;

    ETH->DMACFG = (ETH->DMACFG & ~_ETH_DMACFG_RXBUFSIZE_MASK) | ((EFM32_ETH_BUF_LEN / 64U) <<
    _ETH_DMACFG_RXBUFSIZE_SHIFT) | ETH_DMACFG_FRCDISCARDONERR;

    /* configure MAC address (other three addresses are not used) */
    efm32_eth_set_mac(mac);

    ETH->SPECADDR2BOTTOM = 0;
    ETH->SPECADDR2TOP = 0;
    ETH->SPECADDR3BOTTOM = 0;
    ETH->SPECADDR3TOP = 0;
    ETH->SPECADDR4BOTTOM = 0;
    ETH->SPECADDR4TOP = 0;

    /* initialize the PHY */
    ETH->NETWORKCTRL = ETH_NETWORKCTRL_MANPORTEN;

    int res = _phy_reset();

    if (res < 0) {
        return res;
    }

    res = _phy_detect();

    if (res < 0) {
        return res;
    }

    /* configure default (fallback) link speed */
    efm32_eth_set_link_speed(eth_config.speed);

    /* configure interrupts */
    ETH->IFCR = 0xFFFFFFFFUL;
    ETH->IENS = ETH_IENS_RXCMPLT | ETH_IENS_RXOVERRUN
              | ETH_IENS_TXCMPLT | ETH_IENS_TXUNDERRUN;
    NVIC_EnableIRQ(ETH_IRQn);

    /* enable RX and TX */
    ETH->NETWORKCTRL = ETH_NETWORKCTRL_MANPORTEN | ETH_NETWORKCTRL_ENBRX | ETH_NETWORKCTRL_ENBTX;

    return 0;
}

int efm32_eth_send(const iolist_t *iolist)
{
    unsigned idx = _tx_idx;

    TRACE("[eth] efm32_eth_send: idx=%u\n", idx);

    if (!(_tx_desc[idx].status & TX_STATUS_USED)) {
        DEBUG_PUTS("[eth] efm32_eth_send: TX descriptor busy");
        return -EBUSY;
    }

    uint8_t *dst = _tx_buf[idx];
    size_t len = 0;

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (len + iol->iol_len > EFM32_ETH_BUF_LEN) {
            DEBUG_PUTS("[eth] efm32_eth_send: frame exceeds buffer");
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

    uint32_t status = (len & TX_STATUS_LEN_MASK) | TX_STATUS_LAST;

    if (idx == CONFIG_EFM32_ETH_TX_BUF_NUMOF - 1) {
        status |= TX_STATUS_WRAP;
    }

    /* clearing USED hands the buffer back to the MAC */
    _tx_desc[idx].status = status;
    __DMB();

    /* remember the queued length for efm32_eth_tx_status() */
    _tx_len = (int)len;

    _tx_idx = (idx + 1) % CONFIG_EFM32_ETH_TX_BUF_NUMOF;

    ETH->NETWORKCTRL |= ETH_NETWORKCTRL_TXSTRT;

    return (int)len;
}

int efm32_eth_recv(void *buf, size_t max_len)
{
    unsigned idx = _rx_idx;

    TRACE("[eth] efm32_eth_recv: idx=%u\n", idx);

    if (!(_rx_desc[idx].addr & RX_ADDR_OWN)) {
        DEBUG_PUTS("[eth] efm32_eth_recv: ring empty");
        return -ENODATA;
    }

    int frame_len = (int)(_rx_desc[idx].status & RX_STATUS_LEN_MASK);

    if (buf == NULL && max_len == 0) {
        DEBUG("[eth] efm32_eth_recv: frame length %d bytes\n", frame_len);
        return frame_len;
    }

    if (buf != NULL) {
        size_t copy = (frame_len > (int)max_len) ? max_len : (size_t)frame_len;
        memcpy(buf, _rx_buf[idx], copy);
    }

    /* hand the descriptor back to the MAC, preserving WRAP */
    _rx_desc[idx].status = 0;
    _rx_desc[idx].addr &= ~RX_ADDR_OWN;
    __DMB();

    _rx_idx = (idx + 1) % CONFIG_EFM32_ETH_RX_BUF_NUMOF;

    return frame_len;
}

void efm32_eth_get_mac(uint8_t out[6])
{
    uint32_t bottom = ETH->SPECADDR1BOTTOM;
    uint32_t top = ETH->SPECADDR1TOP;

    out[0] = (uint8_t)(bottom & 0xFF);
    out[1] = (uint8_t)(bottom >> 8);
    out[2] = (uint8_t)(bottom >> 16);
    out[3] = (uint8_t)(bottom >> 24);
    out[4] = (uint8_t)(top & 0xFF);
    out[5] = (uint8_t)(top >> 8);
}

void efm32_eth_set_mac(const uint8_t mac[6])
{
    DEBUG("[eth] efm32_eth_set_mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    ETH->SPECADDR1BOTTOM = ((uint32_t)mac[3] << 24) | ((uint32_t)mac[2] << 16)
                           | ((uint32_t)mac[1] << 8) | mac[0];
    ETH->SPECADDR1TOP = ((uint32_t)mac[5] << 8) | mac[4];
}

bool efm32_eth_link_up(void)
{
    return (_mii_read(MII_BMSR) & MII_BMSR_LINK) != 0;
}

bool efm32_eth_rx_pending(void)
{
    return (_rx_desc[_rx_idx].addr & RX_ADDR_OWN) != 0;
}

int efm32_eth_tx_status(void)
{
    /* descriptor used by the most recent efm32_eth_send() */
    unsigned idx = (_tx_idx + CONFIG_EFM32_ETH_TX_BUF_NUMOF - 1)
                   % CONFIG_EFM32_ETH_TX_BUF_NUMOF;

    uint32_t status = _tx_desc[idx].status;

    /* the frame is still in flight until the MAC hands the descriptor back by
     * setting the USED bit */
    if (!(status & TX_STATUS_USED)) {
        return -EAGAIN;
    }

    /* a collision aborted the frame, report as busy so the upper layer may
     * retry */
    if (status & (TX_STATUS_LATE_COLLISION | TX_STATUS_RETRY_EXCEEDED)) {
        DEBUG_PUTS("[eth] efm32_eth_tx_status: collision");
        return -EBUSY;
    }

    /* any other error (underrun, excessive defer, no descriptor) is fatal for
     * this frame */
    if (status & TX_STATUS_AHB_ERROR) {
        DEBUG_PUTS("[eth] efm32_eth_tx_status: AHB error");
        return -EIO;
    }

    return _tx_len;
}

bool efm32_eth_get_promiscuous(void)
{
    return (ETH->NETWORKCFG & NETWORKCFG_CAF) != 0;
}

void efm32_eth_set_promiscuous(bool enable)
{
    if (enable) {
        ETH->NETWORKCFG |= NETWORKCFG_CAF;
    }
    else {
        ETH->NETWORKCFG &= ~NETWORKCFG_CAF;
    }
}

void efm32_eth_set_link_speed(uint16_t speed)
{
    DEBUG("[eth] efm32_eth_set_link_speed: %s Mbps %s duplex\n",
          (speed & MII_BMCR_SPEED_100) ? "100" : "10",
          (speed & MII_BMCR_FULL_DPLX) ? "full" : "half");

    uint32_t cfg = ETH->NETWORKCFG;

    if (speed & MII_BMCR_SPEED_100) {
        cfg |= ETH_NETWORKCFG_SPEED;
    }
    else {
        cfg &= ~ETH_NETWORKCFG_SPEED;
    }

    if (speed & MII_BMCR_FULL_DPLX) {
        cfg |= ETH_NETWORKCFG_FULLDUPLEX;
    }
    else {
        cfg &= ~ETH_NETWORKCFG_FULLDUPLEX;
    }

    ETH->NETWORKCFG = cfg;
}

int efm32_eth_start_auto_negotiation(void)
{
    DEBUG_PUTS("[eth] efm32_eth_start_auto_negotiation: starting auto-negotiation");

    if (!(_mii_read(MII_BMSR) & MII_BMSR_HAS_AN)) {
        DEBUG_PUTS("[eth] efm32_eth_start_auto_negotiation: not supported by PHY");
        return -EOPNOTSUPP;
    }

    _mii_write(MII_ADVERTISE,
               MII_ADVERTISE_10_F | MII_ADVERTISE_10_H | MII_ADVERTISE_100_F | MII_ADVERTISE_100_H);
    _mii_write(MII_BMCR, MII_BMCR_AN_ENABLE | MII_BMCR_AN_RESTART);

    return 0;
}

void efm32_eth_complete_auto_negotiation(void)
{
    uint32_t deadline = ztimer_now(ZTIMER_MSEC) + CONFIG_EFM32_ETH_AN_TIMEOUT_MS;
    uint16_t bmsr;

    /* wait until AN has completed or the link drops */
    do {
        bmsr = _mii_read(MII_BMSR);

        if (!(bmsr & MII_BMSR_LINK)) {
            DEBUG_PUTS("[eth] efm32_eth_complete_auto_negotiation: link dropped");
            return;
        }

        if (ztimer_now(ZTIMER_MSEC) >= deadline) {
            DEBUG_PUTS("[eth] efm32_eth_complete_auto_negotiation: timed out");
            return;
        }

        ztimer_sleep(ZTIMER_MSEC, 1);
    } while (!(bmsr & MII_BMSR_AN_DONE));

    uint16_t adv = _mii_read(MII_ADVERTISE);
    uint16_t lpa = _mii_read(MII_LPA);

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
            "[eth] efm32_eth_complete_auto_negotiation: defaulting to configured link speed");
        return;
    }

    efm32_eth_set_link_speed(speed);
}

void isr_eth(void)
{
    uint32_t status = ETH->IFCR;

    ETH->IFCR = status;

    /* deliver ISR event to netdev */
    extern netdev_t *efm32_eth_netdev;

    if (efm32_eth_netdev != NULL) {
        if (status & (ETH_IFCR_RXCMPLT | ETH_IFCR_RXOVERRUN)) {
            netdev_trigger_event_isr(efm32_eth_netdev);
        }
        if (status & (ETH_IFCR_TXCMPLT | ETH_IFCR_TXUNDERRUN)) {
            efm32_eth_netdev->event_callback(efm32_eth_netdev, NETDEV_EVENT_TX_COMPLETE);
        }
    }

    cortexm_isr_end();
}
