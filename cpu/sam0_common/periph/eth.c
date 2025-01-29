/*
 * Copyright (C) 2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @{
 *
 * @file
 * @brief       Low-level Ethernet driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "iolist.h"
#include "mii.h"
#include "net/eui48.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"

#include "periph/gpio.h"

#include "sam0_eth_netdev.h"

#define ENABLE_DEBUG 0
#include "debug.h"
#include "log.h"

#include <string.h>

/* Internal helpers */
#define PHY_READ_OP 0x02
#define PHY_WRITE_OP 0x01

/* Internal RX/TX descriptors */
#define DESC_RX_ADDR_OWNSHP    1
#define DESC_RX_ADDR_WRAP      2
#define DESC_RX_ADDR_ADDR_MASK 0xFFFFFFFC

#define DESC_RX_STATUS_FRAME_LEN_MASK 0x1FFF

#define DESC_RX_STATUS_FCS_MASK     (1ul << 13)
#define DESC_RX_STATUS_STA_FRAME    (1ul << 14)
#define DESC_RX_STATUS_END_FRAME    (1ul << 15)
#define DESC_RX_STATUS_CFI          (1ul << 16)
#define DESC_RX_STATUS_VLAN_PRIO    (1ul << 17) || (1ul << 18) || (1ul << 19)
#define DESC_RX_STATUS_PRIO_TAG     (1ul << 20)
#define DESC_RX_STATUS_VLAN_TAG     (1ul << 21)
#define DESC_RX_STATUS_TYPE_ID      (1ul << 22) || (1ul << 23)
#define DESC_RX_STATUS_CHKSUM       (1ul << 24)
#define DESC_RX_STATUS_SPEC_ADDR    (1ul << 25) || (1ul << 26)
#define DESC_RX_STATUS_SPEC_TAG     (1ul << 27)
#define DESC_RX_STATUS_UNIHASH      (1ul << 29)
#define DESC_RX_STATUS_MULTIHASH    (1ul << 30)
#define DESC_RX_STATUS_BROADCAST    (1ul << 31)

#define DESC_TX_STATUS_LEN_MASK     0x3FFF
#define DESC_TX_STATUS_LAST_BUF     (1ul << 15)
#define DESC_TX_STATUS_CRC          (1ul << 16)
#define DESC_TX_STATUS_CHKSUM       (1ul << 20) || (1ul << 21) || (1ul << 22)
#define DESC_TX_STATUS_LATE_COL     (1ul << 26)
#define DESC_TX_STATUS_TX_ERROR     (1ul << 27)
#define DESC_TX_STATUS_RETRY        (1ul << 29)
#define DESC_TX_STATUS_WRAP         (1ul << 30)
#define DESC_TX_STATUS_USED         (1ul << 31)

struct eth_buf_desc {
    uint32_t address;
    uint32_t status;
};

/* GMAC buffer descriptors */
#define GMAC_DESC_ALIGNMENT 8
#define GMAC_BUF_ALIGNMENT  32
static struct eth_buf_desc rx_desc[ETH_RX_BUFFER_COUNT] __attribute__((aligned(GMAC_DESC_ALIGNMENT)));
static struct eth_buf_desc tx_desc[ETH_TX_BUFFER_COUNT] __attribute__((aligned(GMAC_DESC_ALIGNMENT)));

static struct eth_buf_desc *rx_curr;
static struct eth_buf_desc *tx_curr;

/* Declare our own indexes to point to a RX/TX buffer descriptor.
   GMAC IP have its own indexes on its side */
static uint8_t  tx_idx;
static uint8_t  rx_idx;

static uint8_t  rx_buf[ETH_RX_BUFFER_COUNT][ETH_RX_BUFFER_SIZE] __attribute__((aligned(GMAC_BUF_ALIGNMENT)));
static uint8_t  tx_buf[ETH_TX_BUFFER_COUNT][ETH_TX_BUFFER_SIZE] __attribute__((aligned(GMAC_BUF_ALIGNMENT)));
extern sam0_eth_netdev_t _sam0_eth_dev;

static bool _is_sleeping;

/* Flush our reception buffers and reset reception internal mechanism,
   this function may be call from ISR context */
void sam0_clear_rx_buffers(void)
{
    for (int i=0; i<ETH_RX_BUFFER_COUNT; i++) {
        rx_desc[i].address &= ~DESC_RX_ADDR_OWNSHP;
    }
    rx_idx = 0;
    rx_curr = rx_desc;
    GMAC->RBQB.reg = (uint32_t) rx_desc;
}

static void _enable_clock(void)
{
    /* Enable GMAC clocks */
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_GMAC;
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_GMAC;
}

static void _disable_clock(void)
{
    /* Disable GMAC clocks */
    MCLK->AHBMASK.reg &= ~MCLK_AHBMASK_GMAC;
    MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_GMAC;
}

unsigned sam0_read_phy(uint8_t phy, uint8_t addr)
{
    if (_is_sleeping) {
        return 0;
    }

    GMAC->MAN.reg = GMAC_MAN_REGA(addr) | GMAC_MAN_PHYA(phy)
                  | GMAC_MAN_CLTTO      | GMAC_MAN_WTN(0x2)
                  | GMAC_MAN_OP(PHY_READ_OP);

    /* Wait for operation completion */
    while (!(GMAC->NSR.reg & GMAC_NSR_IDLE)) {}
    /* return content of shift register */
    return GMAC->MAN.reg & GMAC_MAN_DATA_Msk;
}

void sam0_write_phy(uint8_t phy, uint8_t addr, uint16_t data)
{
    if (_is_sleeping) {
        return;
    }

    GMAC->MAN.reg = GMAC_MAN_REGA(addr) | GMAC_MAN_PHYA(phy)
                  | GMAC_MAN_WTN(0x2)   | GMAC_MAN_OP(PHY_WRITE_OP)
                  | GMAC_MAN_CLTTO      | GMAC_MAN_DATA(data);

    /* Wait for operation completion */
    while (!(GMAC->NSR.reg & GMAC_NSR_IDLE)) {}
}

void sam0_eth_poweron(void)
{
    _enable_clock();
    sam0_clear_rx_buffers();

    /* enable PHY */
    gpio_set(sam_gmac_config[0].rst_pin);

    /* if the PHY is not idle, it's likely broken */
    if (!(GMAC->NSR.reg & GMAC_NSR_IDLE)) {
        DEBUG_PUTS("sam0_eth: PHY not IDLE, likely broken.");
        return;
    }

    _is_sleeping = false;
    while (MII_BMCR_RESET & sam0_read_phy(0, MII_BMCR)) {}
}

void sam0_eth_poweroff(void)
{
    /* disable PHY */
    gpio_clear(sam_gmac_config[0].rst_pin);

    _is_sleeping = true;
    _disable_clock();
}

static void _init_desc_buf(void)
{
    int i;
    /* Initialize RX buffer descriptors */
    for (i=0; i < ETH_RX_BUFFER_COUNT; i++) {
        rx_desc[i].address = ((uint32_t) (rx_buf[i]) & DESC_RX_ADDR_ADDR_MASK);
    }
    /* Set WRAP flag to indicate last buffer */
    rx_desc[i-1].address |= DESC_RX_ADDR_WRAP;
    rx_curr = &rx_desc[0];
    /* Initialize TX buffer descriptors */
    for (i=0; i < ETH_TX_BUFFER_COUNT; i++) {
        tx_desc[i].address = (uint32_t) tx_buf[i];
    }
    /* Set WRAP flag to indicate last buffer */
    tx_desc[i-1].status |= DESC_TX_STATUS_WRAP;
    tx_curr = &tx_desc[0];
    /* Setup buffers index */
    rx_idx = 0;
    tx_idx = 0;
    /* Store RX buffer descriptor list */
    GMAC->RBQB.reg = (uint32_t) rx_desc;
    /* Store TX buffer descriptor list */
    GMAC->TBQB.reg = (uint32_t) tx_desc;
}

void sam0_eth_set_mac(const eui48_t *mac)
{
    GMAC->Sa[0].SAT.reg = ((mac->uint8[5] << 8)  | mac->uint8[4]);
    GMAC->Sa[0].SAB.reg = ((mac->uint8[3] << 24) | (mac->uint8[2] << 16)
                        |  (mac->uint8[1] << 8)  | mac->uint8[0]);
}

void sam0_eth_get_mac(eui48_t *out)
{
    out->uint8[5] = (GMAC->Sa[0].SAT.reg >> 8);
    out->uint8[4] = (GMAC->Sa[0].SAT.reg);
    out->uint8[3] = (GMAC->Sa[0].SAB.reg >> 24);
    out->uint8[2] = (GMAC->Sa[0].SAB.reg >> 16);
    out->uint8[1] = (GMAC->Sa[0].SAB.reg >> 8);
    out->uint8[0] = (GMAC->Sa[0].SAB.reg);
}

int sam0_eth_send(const struct iolist *iolist)
{
    unsigned tx_len = 0;
    tx_curr = &tx_desc[tx_idx];

    if (_is_sleeping) {
        return -ENETDOWN;
    }

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy ((uint32_t*)(tx_curr->address + tx_len), iol->iol_base, iol->iol_len);
            tx_len += iol->iol_len;
        }
    }

    /* Clear and set the frame size */
    tx_curr->status = (tx_len & DESC_TX_STATUS_LEN_MASK)
    /* Indicate this is the last buffer and the frame is ready */
                    | DESC_TX_STATUS_LAST_BUF;
    /* Prepare next buffer index */
    if (++tx_idx == ETH_TX_BUFFER_COUNT) {
        /* Set WRAP flag to indicate last buffer */
        tx_curr->status |= DESC_TX_STATUS_WRAP;
        tx_idx = 0;
    }
    __DMB();

    /* Start transmission */
    GMAC->NCR.reg |= GMAC_NCR_TSTART;
    /* Set the next buffer */
    tx_curr = &tx_desc[tx_idx];

    return 0;
}

unsigned _sam0_eth_get_last_len(void)
{
    unsigned idx = tx_idx ? tx_idx - 1 : ETH_TX_BUFFER_COUNT - 1;
    return tx_desc[idx].status & DESC_TX_STATUS_LEN_MASK;
}

static int _try_receive(char* data, unsigned max_len, int block)
{
    (void)block;
    unsigned rxlen = 0;
    uint16_t idx = rx_idx;
    uint8_t tmp = ETH_RX_BUFFER_COUNT;

    /* Check if the current rx descriptor contains the beginning of
       a new frame, iterates over all our RX buffers if not.
       If there is no new frame (because we may have flush our RX
       buffers due to BNA interrupt), return an error to netdev so
       we can move forward */
    do {
        if ((rx_curr->address & DESC_RX_ADDR_OWNSHP)
            && (rx_curr->status & DESC_RX_STATUS_STA_FRAME)) {
            break;
        }
        else {
            idx = (idx+1) % ETH_RX_BUFFER_COUNT;
            rx_curr = &rx_desc[idx];
            tmp--;
        }
    } while (tmp > 0);

    if (tmp == 0) {
        return -ENOBUFS;
    }

    for (unsigned cpt=0; cpt < ETH_RX_BUFFER_COUNT; cpt++) {
        /* Get the length of the received frame */
        unsigned len = (rx_curr->status & DESC_RX_STATUS_FRAME_LEN_MASK);

        /* Only copy data if the stack requested it, otherwise return the length
           of the next frame if available */
        if (max_len) {
            /* If buffer available, copy data into it */
            if (data)  {
                /* If provided buffer is smaller than the received frame,
                drop it as netdev request */
                if (rxlen + len > max_len) {
                    return -ENOBUFS;
                }
                memcpy(&data[rxlen], rx_buf[idx], len);
            }
            /* Tell the GMAC IP that we don't need this frame anymore  */
            rx_curr->address &= ~DESC_RX_ADDR_OWNSHP;
        }

        rxlen += len;

        if (rx_curr->status & DESC_RX_STATUS_END_FRAME) {
            /* We reach the end of frame, leave the loop */
            break;
        }
        /* Prepare next buffer */
        idx = (idx + 1) % ETH_RX_BUFFER_COUNT;
        rx_curr = &rx_desc[idx];

    }
    /* restore the previous index if packets were not released */
    if (!max_len) {
       rx_curr = &rx_desc[rx_idx];
    }
    /* Point to the next buffer as GMAC IP will likely used it
       to store the next frame */
    else {
       rx_idx = (idx+1) % ETH_RX_BUFFER_COUNT;
       rx_curr = &rx_desc[rx_idx];
    }

    /* If provided buffer is smaller than the received frame,
       drop it as netdev request */
    if (data != NULL && rxlen > max_len) {
       return -ENOBUFS;
   }

    return rxlen;
}

int sam0_eth_receive_blocking(char *data, unsigned max_len)
{
    return _try_receive(data, max_len, 1);
}

bool sam0_eth_has_queued_pkt(void)
{
    return _try_receive(NULL, 0, 0) > 0;
}

int sam0_eth_init(void)
{
    /* Enable clocks */
    _enable_clock();
    /* Initialize GPIOs */
    gpio_init_mux(sam_gmac_config[0].refclk, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].txen, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].txd0, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].txd1, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].crsdv, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].rxd0, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].rxd1, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].rxer, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].mdc, GPIO_MUX_L);
    gpio_init_mux(sam_gmac_config[0].mdio, GPIO_MUX_L);
    /* PHY reset */
    gpio_init(sam_gmac_config[0].rst_pin, GPIO_OUT);
    gpio_clear(sam_gmac_config[0].rst_pin);

    /* reset buffers */
    memset(rx_buf, 0, sizeof(rx_buf));
    memset(tx_buf, 0, sizeof(tx_buf));
    memset(rx_desc, 0, sizeof(rx_desc));
    memset(tx_desc, 0, sizeof(tx_desc));

    /* Initialize buffers descriptor */
    _init_desc_buf();
    /* Disable RX and TX */
    GMAC->NCR.reg &= ~(GMAC_NCR_RXEN | GMAC_NCR_TXEN);
    /* Enable Port Management */
    GMAC->NCR.reg |= GMAC_NCR_MPE;
    /* TODO: Implements MII, default to RMII */
    GMAC->UR.reg = 0;
    /* disable all interrupts */
    GMAC->IDR.reg = 0xFFFFFFFF;
    /* clear flags */
    GMAC->RSR.reg = GMAC_RSR_HNO | GMAC_RSR_RXOVR | GMAC_RSR_REC | GMAC_RSR_BNA;
    GMAC->TSR.reg = 0xFFFF;
    /* Enable needed interrupts */
    GMAC->IER.reg = GMAC_IER_RCOMP
                  | GMAC_IER_TCOMP | GMAC_IER_TFC | GMAC_IER_RLEX;

    GMAC->NCFGR.reg = GMAC_NCFGR_MTIHEN
                    | GMAC_NCFGR_RXCOEN | GMAC_NCFGR_MAXFS | GMAC_NCFGR_CAF
                    | GMAC_NCFGR_LFERD | GMAC_NCFGR_RFCS | GMAC_NCFGR_CLK(3)
                    | GMAC_NCFGR_DBW(1);

    /* Enable all multicast addresses */
    GMAC->HRB.reg = 0xffffffff;
    GMAC->HRT.reg = 0xffffffff;

    /* Set DMA receive buffer size to 1536 bytes */
    GMAC->DCFGR.reg |= GMAC_DCFGR_DRBS(0x18);
    /* Enable PHY */
    gpio_set(sam_gmac_config[0].rst_pin);
    /* Enable IRQ */
    NVIC_EnableIRQ(GMAC_IRQn);
    /* Enable both receiver and transmitter */
    GMAC->NCR.reg |= GMAC_NCR_TXEN | GMAC_NCR_RXEN;

    return 0;
}
