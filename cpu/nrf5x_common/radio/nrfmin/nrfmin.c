/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_nrf5x_nrfmin
 * @{
 *
 * @file
 * @brief       Implementation of the nrfmin radio driver for nRF51 radios
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <errno.h>

#include "cpu.h"
#include "mutex.h"
#include "assert.h"

#include "periph_conf.h"
#include "periph/cpuid.h"

#include "nrfmin.h"
#include "net/netdev.h"

#ifdef MODULE_GNRC_SIXLOWPAN
#include "net/gnrc/nettype.h"
#endif

#define ENABLE_DEBUG            (0)
#include "debug.h"

/**
 * @brief   Driver specific device configuration
 * @{
 */
#define CONF_MODE               RADIO_MODE_MODE_Nrf_1Mbit
#define CONF_LEN                (8U)
#define CONF_S0                 (0U)
#define CONF_S1                 (0U)
#define CONF_STATLEN            (0U)
#define CONF_BASE_ADDR_LEN      (4U)
#define CONF_ENDIAN             RADIO_PCNF1_ENDIAN_Big
#define CONF_WHITENING          RADIO_PCNF1_WHITEEN_Disabled
#define CONF_CRC_LEN            (2U)
#define CONF_CRC_POLY           (0x11021)
#define CONF_CRC_INIT           (0xf0f0f0)
/** @} */

/**
 * @brief   Driver specific address configuration
 * @{
 */
#define CONF_ADDR_PREFIX0       (0xe7e7e7e7)
#define CONF_ADDR_BASE          (0xe7e70000)
#define CONF_ADDR_BCAST         (CONF_ADDR_BASE | NRFMIN_ADDR_BCAST)
/** @} */

/**
 * @brief   We define a pseudo NID for compliance to 6LoWPAN
 */
#define CONF_PSEUDO_NID         (0xaffe)

/**
 * @brief   Driver specific (interrupt) events (not all of them used currently)
 * @{
 */
#define ISR_EVENT_RX_START      (0x0001)
#define ISR_EVENT_RX_DONE       (0x0002)
#define ISR_EVENT_TX_START      (0x0004)
#define ISR_EVENT_TX_DONE       (0x0008)
#define ISR_EVENT_WRONG_CHKSUM  (0x0010)
/** @} */

/**
 * @brief   Possible internal device states
 */
typedef enum {
    STATE_OFF,                  /**< device is powered off */
    STATE_IDLE,                 /**< device is in idle mode */
    STATE_RX,                   /**< device is in receive mode */
    STATE_TX,                   /**< device is transmitting data */
} state_t;


/**
 * @brief   Since there can only be 1 nrfmin device, we allocate it right here
 */
netdev_t nrfmin_dev;

/**
 * @brief   For faster lookup we remember our own 16-bit address
 */
static uint16_t my_addr;

/**
 * @brief   We need to keep track of the radio state in SW (-> PAN ID 20)
 *
 * See nRF51822 PAN ID 20: RADIO State Register is not functional.
 */
static volatile state_t state = STATE_OFF;

/**
 * @brief   We also remember the 'long-term' state, so we can resume after TX
 */
static volatile state_t target_state = STATE_OFF;

/**
 * @brief   When sending out data, the data needs to be in one continuous memory
 *          region. So we need to buffer outgoing data on the driver level.
 */
static nrfmin_pkt_t tx_buf;

/**
 * @brief   As the device is memory mapped, we need some space to save incoming
 *          data to.
 *
 * @todo    Improve the RX buffering to at least use double buffering
 */
static nrfmin_pkt_t rx_buf;

/**
 * @brief   While we listen for incoming data, we lock the RX buffer
 */
static volatile uint8_t rx_lock = 0;

/**
 * @brief   Set radio into idle (DISABLED) state
 */
static void go_idle(void)
{
    /* set device into basic disabled state */
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0) {}
    /* also release any existing lock on the RX buffer */
    rx_lock = 0;
    state = STATE_IDLE;
}

/**
 * @brief   Set radio into the target state as defined by `target_state`
 *
 * Trick here is, that the driver can go back to it's previous state after a
 * send operation, so it can differentiate if the driver was in DISABLED or in
 * RX mode before the send process had started.
 */
static void goto_target_state(void)
{
    go_idle();

    if ((target_state == STATE_RX) && (rx_buf.pkt.hdr.len == 0)) {
        /* set receive buffer and our own address */
        rx_lock = 1;
        NRF_RADIO->PACKETPTR = (uint32_t)(&rx_buf);
        NRF_RADIO->BASE0 = (CONF_ADDR_BASE | my_addr);
        /* goto RX mode */
        NRF_RADIO->EVENTS_READY = 0;
        NRF_RADIO->TASKS_RXEN = 1;
        while (NRF_RADIO->EVENTS_READY == 0) {}
        state = STATE_RX;
    }

    if (target_state == STATE_OFF) {
        NRF_RADIO->POWER = 0;
        state = STATE_OFF;
    }
}

void nrfmin_setup(void)
{
    nrfmin_dev.driver = &nrfmin_netdev;
    nrfmin_dev.event_callback = NULL;
    nrfmin_dev.context = NULL;
}

uint16_t nrfmin_get_addr(void)
{
    return my_addr;
}

void nrfmin_get_iid(uint16_t *iid)
{
    iid[0] = 0;
    iid[1] = 0xff00;
    iid[2] = 0x00fe;
    iid[3] = my_addr;
}

uint16_t nrfmin_get_channel(void)
{
    return (uint16_t)(NRF_RADIO->FREQUENCY >> 2);
}

netopt_state_t nrfmin_get_state(void)
{
    switch (state) {
        case STATE_OFF:  return NETOPT_STATE_OFF;
        case STATE_IDLE: return NETOPT_STATE_SLEEP;
        case STATE_RX:   return NETOPT_STATE_IDLE;
        case STATE_TX:   return NETOPT_STATE_TX;
        default:         return NETOPT_STATE_RESET;     /* should never show */
    }
}

int16_t nrfmin_get_txpower(void)
{
    int8_t p = (int8_t)NRF_RADIO->TXPOWER;
    if (p < 0) {
        return (int16_t)(0xff00 | p);
    }
    return (int16_t)p;
}

void nrfmin_set_addr(uint16_t addr)
{
    my_addr = addr;
    goto_target_state();
}

int nrfmin_set_channel(uint16_t chan)
{
    if (chan > NRFMIN_CHAN_MAX) {
        return -EOVERFLOW;
    }

    NRF_RADIO->FREQUENCY = (chan << 2);
    goto_target_state();

    return sizeof(uint16_t);
}

void nrfmin_set_txpower(int16_t power)
{
    if (power > 2) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos4dBm;
    }
    else if (power > -2) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm;
    }
    else if (power > -6) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg4dBm;
    }
    else if (power > -10) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg8dBm;
    }
    else if (power > -14) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg12dBm;
    }
    else if (power > -18) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg16dBm;
    }
    else if (power > -25) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg20dBm;
    }
    else {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg30dBm;
    }
}

int nrfmin_set_state(netopt_state_t val)
{
    /* make sure radio is turned on and no transmission is in progress */
    NRF_RADIO->POWER = 1;

    switch (val) {
        case NETOPT_STATE_OFF:
            target_state = STATE_OFF;
            break;
        case NETOPT_STATE_SLEEP:
            target_state = STATE_IDLE;
            break;
        case NETOPT_STATE_IDLE:
            target_state = STATE_RX;
            break;
        default:
            return -ENOTSUP;
    }

    goto_target_state();

    return sizeof(netopt_state_t);
}


/**
 * @brief   Radio interrupt routine
 */
void isr_radio(void)
{
    if (NRF_RADIO->EVENTS_END == 1) {
        NRF_RADIO->EVENTS_END = 0;
        /* did we just send or receive something? */
        if (state == STATE_RX) {
            /* drop packet on invalid CRC */
            if ((NRF_RADIO->CRCSTATUS != 1) || !(nrfmin_dev.event_callback)) {
                rx_buf.pkt.hdr.len = 0;
                NRF_RADIO->TASKS_START = 1;
            }
            else {
                rx_lock = 0;
                nrfmin_dev.event_callback(&nrfmin_dev, NETDEV_EVENT_ISR);
            }
        }
        else if (state == STATE_TX) {
            goto_target_state();
        }
    }

    cortexm_isr_end();
}

static int nrfmin_send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;

    assert(iolist);
    if (state == STATE_OFF) {
        return -ENETDOWN;
    }

    /* wait for any ongoing transmission to finish and go into idle state */
    while (state == STATE_TX) {}
    go_idle();

    /* copy packet data into the transmit buffer */
    int pos = 0;
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if ((pos + iol->iol_len) > NRFMIN_PKT_MAX) {
            DEBUG("[nrfmin] send: unable to do so, packet is too large!\n");
            return -EOVERFLOW;
        }
        memcpy(&tx_buf.raw[pos], iol->iol_base, iol->iol_len);
        pos += iol->iol_len;
    }

    /* set output buffer and destination address */
    nrfmin_hdr_t *hdr = (nrfmin_hdr_t *)iolist->iol_base;
    NRF_RADIO->PACKETPTR = (uint32_t)(&tx_buf);
    NRF_RADIO->BASE0 = (CONF_ADDR_BASE | hdr->dst_addr);

    /* trigger the actual transmission */
    DEBUG("[nrfmin] send: putting %i byte into the ether\n", (int)hdr->len);
    NRF_RADIO->EVENTS_READY = 0;
    NRF_RADIO->TASKS_TXEN = 1;
    while (NRF_RADIO->EVENTS_READY == 0) {}
    state = STATE_TX;

    return (int)pos;
}

static int nrfmin_recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    (void)dev;
    (void)info;

    if (state == STATE_OFF) {
        return -ENETDOWN;
    }

    unsigned pktlen = rx_buf.pkt.hdr.len;

    /* check if packet data is readable */
    if (rx_lock || (pktlen == 0)) {
        DEBUG("[nrfmin] recv: no packet data available\n");
        return 0;
    }

    if (buf == NULL) {
        if (len > 0) {
            /* drop packet */
            DEBUG("[nrfmin] recv: dropping packet of length %i\n", pktlen);
            rx_buf.pkt.hdr.len = 0;
            goto_target_state();
        }
    }
    else {
        DEBUG("[nrfmin] recv: reading packet of length %i\n", pktlen);

        pktlen = (len < pktlen) ? len : pktlen;
        memcpy(buf, rx_buf.raw, pktlen);
        rx_buf.pkt.hdr.len = 0;
        goto_target_state();
    }

    return pktlen;
}

static int nrfmin_init(netdev_t *dev)
{
    (void)dev;
    uint8_t cpuid[CPUID_LEN];

    /* check given device descriptor */
    assert(dev);

    /* initialize our own address from the CPU ID */
    my_addr = 0;
    cpuid_get(cpuid);
    for (unsigned i = 0; i < CPUID_LEN; i++) {
        my_addr ^= cpuid[i] << (8 * (i & 0x01));
    }

    /* power on the NRFs radio */
    NRF_RADIO->POWER = 1;
    /* load driver specific configuration */
    NRF_RADIO->MODE = CONF_MODE;
    /* configure variable parameters to default values */
    NRF_RADIO->TXPOWER = NRFMIN_TXPOWER_DEFAULT;
    NRF_RADIO->FREQUENCY = NRFMIN_CHAN_DEFAULT;
    /* pre-configure radio addresses */
    NRF_RADIO->PREFIX0 = CONF_ADDR_PREFIX0;
    NRF_RADIO->BASE0   = (CONF_ADDR_BASE | my_addr);
    NRF_RADIO->BASE1   = CONF_ADDR_BCAST;
    /* always send from logical address 0 */
    NRF_RADIO->TXADDRESS = 0x00UL;
    /* and listen to logical addresses 0 and 1 */
    /* workaround errata nrf52832 3.41 [143] */
    NRF_RADIO->RXADDRESSES = 0x10003UL;
    /* configure data fields and packet length whitening and endianness */
    NRF_RADIO->PCNF0 = ((CONF_S1 << RADIO_PCNF0_S1LEN_Pos) |
                        (CONF_S0 << RADIO_PCNF0_S0LEN_Pos) |
                        (CONF_LEN << RADIO_PCNF0_LFLEN_Pos));
    NRF_RADIO->PCNF1 = ((CONF_WHITENING << RADIO_PCNF1_WHITEEN_Pos) |
                        (CONF_ENDIAN << RADIO_PCNF1_ENDIAN_Pos) |
                        (CONF_BASE_ADDR_LEN << RADIO_PCNF1_BALEN_Pos) |
                        (CONF_STATLEN << RADIO_PCNF1_STATLEN_Pos) |
                        (NRFMIN_PKT_MAX << RADIO_PCNF1_MAXLEN_Pos));
    /* configure the CRC unit, we skip the address field as this seems to lead
     * to wrong checksum calculation on nRF52 devices in some cases */
    NRF_RADIO->CRCCNF = CONF_CRC_LEN | RADIO_CRCCNF_SKIPADDR_Msk;
    NRF_RADIO->CRCPOLY = CONF_CRC_POLY;
    NRF_RADIO->CRCINIT = CONF_CRC_INIT;
    /* set shortcuts for more efficient transfer */
    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk;
    /* enable interrupts */
    NVIC_EnableIRQ(RADIO_IRQn);
    /* enable END interrupt */
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk;
    /* put device in receive mode */
    target_state = STATE_RX;
    goto_target_state();

    DEBUG("[nrfmin] initialization successful\n");

    return 0;
}

static void nrfmin_isr(netdev_t *dev)
{
    if (nrfmin_dev.event_callback) {
        nrfmin_dev.event_callback(dev, NETDEV_EVENT_RX_COMPLETE);
    }
}

static int nrfmin_get(netdev_t *dev, netopt_t opt, void *val, size_t max_len)
{
    (void)dev;
    (void)max_len;

    switch (opt) {
        case NETOPT_CHANNEL:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)val) = nrfmin_get_channel();
            return sizeof(uint16_t);
        case NETOPT_ADDRESS:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)val) = nrfmin_get_addr();
            return sizeof(uint16_t);
        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = nrfmin_get_state();
            return sizeof(netopt_state_t);
        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *((int16_t *)val) = nrfmin_get_txpower();
            return sizeof(int16_t);
        case NETOPT_MAX_PDU_SIZE:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)val) = NRFMIN_PAYLOAD_MAX;
            return sizeof(uint16_t);
        case NETOPT_ADDR_LEN:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)val) = 2;
            return sizeof(uint16_t);
        case NETOPT_NID:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t*)val) = CONF_PSEUDO_NID;
            return sizeof(uint16_t);
#ifdef MODULE_GNRC_SIXLOWPAN
        case NETOPT_PROTO:
            assert(max_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)val) = GNRC_NETTYPE_SIXLOWPAN;
            return sizeof(gnrc_nettype_t);
#endif
        case NETOPT_DEVICE_TYPE:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)val) = NETDEV_TYPE_NRFMIN;
            return sizeof(uint16_t);
        case NETOPT_IPV6_IID:
            assert(max_len >= sizeof(uint64_t));
            nrfmin_get_iid((uint16_t *)val);
            return sizeof(uint64_t);
        default:
            return -ENOTSUP;
    }
}

static int nrfmin_set(netdev_t *dev, netopt_t opt, const void *val, size_t len)
{
    (void)dev;
    (void)len;

    switch (opt) {
        case NETOPT_CHANNEL:
            assert(len == sizeof(uint16_t));
            return nrfmin_set_channel(*((const uint16_t *)val));
        case NETOPT_ADDRESS:
            assert(len == sizeof(uint16_t));
            nrfmin_set_addr(*((const uint16_t *)val));
            return sizeof(uint16_t);
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(len == sizeof(uint16_t));
            if (*((const uint16_t *)val) != 2) {
                return -EAFNOSUPPORT;
            }
            return sizeof(uint16_t);
        case NETOPT_STATE:
            assert(len == sizeof(netopt_state_t));
            return nrfmin_set_state(*((const netopt_state_t *)val));
        case NETOPT_TX_POWER:
            assert(len == sizeof(int16_t));
            nrfmin_set_txpower(*((const int16_t *)val));
            return sizeof(int16_t);
        default:
            return -ENOTSUP;
    }
}

/**
 * @brief   Export of the netdev interface
 */
const netdev_driver_t nrfmin_netdev = {
    .send = nrfmin_send,
    .recv = nrfmin_recv,
    .init = nrfmin_init,
    .isr  = nrfmin_isr,
    .get  = nrfmin_get,
    .set  = nrfmin_set
};
