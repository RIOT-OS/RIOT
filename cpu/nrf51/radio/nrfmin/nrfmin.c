/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_nrf51822_nrfmin
 * @{
 *
 * @file
 * @brief       Implementation of the nrfmin NRF51822 minimal radio driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "thread.h"
#include "sched.h"
#include "kernel.h"
#include "periph_conf.h"
#include "periph/cpuid.h"
#include "nrfmin.h"
#include "net/gnrc.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Driver specific device configuration
 * @{
 */
#define CONF_MODE               RADIO_MODE_MODE_Nrf_2Mbit
#define CONF_PAYLOAD_LEN        (250U)
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
#define CONF_ADDR_PREFIX0       (0xE7E7E7E7)
#define CONF_ADDR_BCAST         (0xffff)
/** @} */

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
 * @brief   Payload types to use in driver specific framed format
 *
 * We expect the radio to carry either raw link layer data (UNDEF) or network
 * layer data, so no need to map transport layer protocols etc...
 * @{
 */
#define NRFTYPE_UNDEF           (0x01)
#define NRFTYPE_SIXLOWPAN       (0x02)
#define NRFTYPE_IPV6            (0x03)
#define NRFTYPE_ICMPV6          (0x04)
/**
 * @}
 */

/**
 * @brief   Possible internal device states
 */
typedef enum {
    STATE_OFF,              /**< device is powered off */
    STATE_IDLE,             /**< device is in idle mode */
    STATE_RX,               /**< device is in receive mode */
    STATE_TX,               /**< device is transmitting data */
} state_t;

/**
 * @brief   In-memory structure of a nrfmin radio packet
 */
typedef struct __attribute__((packed)) {
    uint8_t length;                     /**< packet length */
    uint8_t src_addr[2];                /**< source address of the packet */
    uint8_t dst_addr[2];                /**< destination address */
    uint8_t proto;                      /**< protocol of payload */
    uint8_t payload[CONF_PAYLOAD_LEN];  /**< actual payload */
} packet_t;

/**
 * @brief   Pointer to the MAC layer event callback
 */
static gnrc_netdev_t *_netdev = NULL;

/**
 * @brief   Current state of the device
 */
static volatile state_t _state = STATE_OFF;

/**
 * @brief   Address of the device
 */
static uint16_t _addr;

/**
 * @brief   Transmission buffer
 */
static packet_t _tx_buf;

/**
 * @brief   Hold the state before sending to return to it afterwards
 */
static state_t _tx_prestate;

/**
 * @brief    Double receive buffers
 */
static packet_t _rx_buf[2];

/**
 * @brief   Pointer to the free receive buffer
 */
static volatile int _rx_next = 0;

/*
 * Create an internal mapping between NETTYPE and NRFTYPE
 */
static inline gnrc_nettype_t _nrftype_to_nettype(uint8_t nrftype)
{
    switch (nrftype) {
#ifdef MODULE_GNRC_SIXLOWPAN
        case NRFTYPE_SIXLOWPAN:
            return GNRC_NETTYPE_SIXLOWPAN;
#endif
#ifdef MODULE_GNRC_IPV6
        case NRFTYPE_IPV6:
            return GNRC_NETTYPE_IPV6;
#endif
#ifdef MODULE_GNRC_ICMPV6
        case NRFTYPE_ICMPV6:
            return GNRC_NETTYPE_ICMPV6;
#endif
        default:
            return GNRC_NETTYPE_UNDEF;
    }
}

static inline uint8_t _nettype_to_nrftype(gnrc_nettype_t nettype)
{
    switch (nettype) {
#ifdef MODULE_GNRC_SIXLOWPAN
        case GNRC_NETTYPE_SIXLOWPAN:
            return NRFTYPE_SIXLOWPAN;
#endif
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            return NRFTYPE_IPV6;
#endif
#ifdef MODULE_GNRC_ICMPV6
        case GNRC_NETTYPE_ICMPV6:
            return NRFTYPE_ICMPV6;
#endif
        default:
            return NRFTYPE_UNDEF;
    }
}

/*
 * Functions for controlling the radios state
 */
static void _switch_to_idle(void)
{
    /* witch to idle state */
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0);
    _state = STATE_IDLE;
}

static void _switch_to_rx(void)
{
    /* set pointer to receive buffer */
    NRF_RADIO->PACKETPTR = (uint32_t)&(_rx_buf[_rx_next]);
    /* set address */
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= _addr;
    /* switch int RX mode */
    NRF_RADIO->TASKS_RXEN = 1;
    _state = STATE_RX;
}

/*
 * Getter and Setter functions
 */
int _get_state(uint8_t *val, size_t max_len)
{
    netopt_state_t state;

    if (max_len < sizeof(netopt_state_t)) {
        return -EOVERFLOW;
    }
    switch (_state) {
        case STATE_OFF:
            state = NETOPT_STATE_OFF;
            break;
        case STATE_IDLE:
            state = NETOPT_STATE_SLEEP;
            break;
        case STATE_RX:
            state = NETOPT_STATE_IDLE;
            break;
        case STATE_TX:
            state = NETOPT_STATE_TX;
            break;
        default:
            return -ECANCELED;
    }
    memcpy(val, &state, sizeof(netopt_state_t));
    return sizeof(netopt_state_t);
}

int _set_state(uint8_t *val, size_t len)
{
    netopt_state_t state;

    if (len != sizeof(netopt_state_t)) {
        return -EINVAL;
    }
    /* get target state */
    memcpy(&state, val, len);
    /* switch to target state */
    switch (state) {
        case NETOPT_STATE_SLEEP:
            _switch_to_idle();
            break;
        case NETOPT_STATE_IDLE:
            _switch_to_rx();
            break;
        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

int _get_address(uint8_t *val, size_t max_len)
{
    /* check parameters */
    if (max_len < 2) {
        return -EOVERFLOW;
    }
    /* get address */
    val[0] = (uint8_t)(_addr >> 8);
    val[1] = (uint8_t)(_addr);
    return 2;
}

int _set_address(uint8_t *val, size_t len)
{
    int is_rx = 0;

    /* check parameters */
    if (len != 2) {
        return -EINVAL;
    }
    /* keep track of state */
    while (_state == STATE_TX);
    if (_state == STATE_RX) {
        is_rx = 1;
        _switch_to_idle();
    }
    /* set address */
    _addr = (((uint16_t)val[0]) << 8) | val[1];
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= _addr;
    /* restore old state */
    if (is_rx) {
        _switch_to_rx();
    }
    return 2;
}

int _get_channel(uint8_t *val, size_t max_len)
{
    /* check parameters */
    if (max_len < 2) {
        return -EOVERFLOW;
    }
    /* get channel */
    val[0] = (0x3f & NRF_RADIO->FREQUENCY);
    val[1] = 0;
    return 2;
}

int _set_channel(uint8_t *val, size_t len)
{
    int is_rx = 0;

    /* check parameter */
    if (len != 2 || val[0] > 0x3f) {
        return -EINVAL;
    }
    /* remember state */
    while (_state == STATE_TX);
    if (_state == STATE_RX) {
        is_rx = 1;
        _switch_to_idle();
    }
    /* set channel */
    NRF_RADIO->FREQUENCY = val[0];
    /* restore state */
    if (is_rx) {
        _switch_to_rx();
    }
    return 2;
}

int _get_pan(uint8_t *val, size_t max_len)
{
    /* check parameters */
    if (max_len < 2) {
        return -EOVERFLOW;
    }
    /* get PAN ID */
    val[0] = (uint8_t)((NRF_RADIO->BASE0 & 0x00ff0000) >> 16);
    val[1] = (uint8_t)((NRF_RADIO->BASE0 & 0xff000000) >> 24);
    return 2;
}

int _set_pan(uint8_t *val, size_t len)
{
    int is_rx = 0;
    uint32_t pan;

    /* check parameter */
    if (len != 2) {
        return -EINVAL;
    }
    /* remember state */
    while (_state == STATE_TX);
    if (_state == STATE_RX) {
        is_rx = 1;
        _switch_to_idle();
    }
    /* set new PAN ID */
    pan = ((uint32_t)val[1] << 24) | ((uint32_t)val[0] << 16);
    NRF_RADIO->BASE0   = pan | _addr;
    NRF_RADIO->BASE1   = pan | CONF_ADDR_BCAST;
    /* restore state */
    if (is_rx) {
        _switch_to_rx();
    }
    return 2;
}

int _get_txpower(uint8_t *val, size_t len)
{
    /* check parameters */
    if (len < 2) {
        return 0;
    }
    /* get value */
    val[0] = NRF_RADIO->TXPOWER;
    if (val[0] & 0x80) {
        val[1] = 0xff;
    }
    else {
        val[1] = 0x00;
    }
    return 2;
}

int _set_txpower(uint8_t *val, size_t len)
{
    int8_t power;

    /* check parameters */
    if (len < 2) {
        return -EINVAL;
    }
    /* get TX power value */
    power = (int8_t)val[0];

    if (power > 2) {
        power = 4;
    }
    else if (power > -2) {
        power = 0;
    }
    else if (power > -6) {
        power = -4;
    }
    else if (power > -10) {
        power = -8;
    }
    else if (power > -14) {
        power = -12;
    }
    else if (power > -18) {
        power = -16;
    }
    else {
        power = -20;
    }
    NRF_RADIO->TXPOWER = power;
    return 2;
}

/*
 * Radio interrupt routine
 */
void isr_radio(void)
{
    msg_t msg;

    if (NRF_RADIO->EVENTS_END == 1) {
        NRF_RADIO->EVENTS_END = 0;
        /* did we just send or receive something? */
        if (_state == STATE_RX) {
            /* drop packet on invalid CRC */
            if (NRF_RADIO->CRCSTATUS != 1) {
                return;
            }
            msg.type = GNRC_NETDEV_MSG_TYPE_EVENT;
            msg.content.value = ISR_EVENT_RX_DONE;
            msg_send_int(&msg, _netdev->mac_pid);
            /* switch buffer */
            _rx_next = _rx_next ^ 1;
            NRF_RADIO->PACKETPTR = (uint32_t)&(_rx_buf[_rx_next]);
            /* go back into receive mode */
            NRF_RADIO->TASKS_START = 1;
        }
        else if (_state == STATE_TX) {
            /* disable radio again */
            _switch_to_idle();
            /* if radio was receiving before, go back into RX state */
            if (_tx_prestate == STATE_RX) {
                _switch_to_rx();
            }
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

/*
 * Event handlers
 */
static void _receive_data(void)
{
    packet_t *data;
    gnrc_pktsnip_t *pkt_head;
    gnrc_pktsnip_t *pkt;
    gnrc_netif_hdr_t *hdr;
    gnrc_nettype_t nettype;

    /* only read data if we have somewhere to send it to */
    if (_netdev->event_cb == NULL) {
        return;
    }

    /* get pointer to RX data buffer */
    data = &(_rx_buf[_rx_next ^ 1]);

    /* allocate and fill netif header */
    pkt_head = gnrc_pktbuf_add(NULL, NULL, sizeof(gnrc_netif_hdr_t) + 4,
                               GNRC_NETTYPE_UNDEF);
    if (pkt_head == NULL) {
        DEBUG("nrfmin: Error allocating netif header on RX\n");
        return;
    }
    hdr = (gnrc_netif_hdr_t *)pkt_head->data;
    gnrc_netif_hdr_init(hdr, 2, 2);
    hdr->if_pid = _netdev->mac_pid;
    gnrc_netif_hdr_set_src_addr(hdr, data->src_addr, 2);
    gnrc_netif_hdr_set_dst_addr(hdr, data->dst_addr, 2);

    /* allocate and fill payload */
    nettype = _nrftype_to_nettype(data->proto);
    pkt = gnrc_pktbuf_add(pkt_head, data->payload, data->length - 6, nettype);
    if (pkt == NULL) {
        DEBUG("nrfmin: Error allocating packet payload on RX\n");
        gnrc_pktbuf_release(pkt_head);
        return;
    }

    /* pass on the received packet */
    _netdev->event_cb(NETDEV_EVENT_RX_COMPLETE, pkt);
}

/*
 * Public interface functions
 */
int nrfmin_init(gnrc_netdev_t *dev)
{
    uint8_t cpuid[CPUID_ID_LEN];
    uint8_t tmp;
    int i;

    /* check given device descriptor */
    if (dev == NULL) {
        return -ENODEV;
    }
    /* set initial values */
    dev->driver = &nrfmin_driver;
    dev->event_cb = NULL;
    dev->mac_pid = KERNEL_PID_UNDEF;
    /* keep a pointer for future reference */
    _netdev = dev;

    /* power on the NRFs radio */
    NRF_RADIO->POWER = 1;
    /* load driver specific configuration */
    NRF_RADIO->MODE = CONF_MODE;
    /* configure variable parameters to default values */
    NRF_RADIO->TXPOWER = NRFMIN_DEFAULT_TXPOWER;
    NRF_RADIO->FREQUENCY = NRFMIN_DEFAULT_CHANNEL;
    /* get default address from CPU ID */
    cpuid_get(cpuid);
    tmp = 0;
    for (i = 0; i < (CPUID_ID_LEN / 2); i++) {
        tmp ^= cpuid[i];
    }
    _addr = ((uint16_t)tmp) << 8;
    tmp = 0;
    for (; i < CPUID_ID_LEN; i++) {
        tmp ^= cpuid[i];
    }
    _addr |= tmp;
    /* pre-configure radio addresses */
    NRF_RADIO->PREFIX0 = CONF_ADDR_PREFIX0;
    NRF_RADIO->BASE0   = (NRFMIN_DEFAULT_PAN << 16) | _addr;
    NRF_RADIO->BASE1   = (NRFMIN_DEFAULT_PAN << 16) | CONF_ADDR_BCAST;
    NRF_RADIO->TXADDRESS = 0x00UL;      /* always send from address 0 */
    NRF_RADIO->RXADDRESSES = 0x03UL;    /* listen to addresses 0 and 1 */
    /* configure data fields and packet length whitening and endianess */
    NRF_RADIO->PCNF0 = (CONF_S1 << RADIO_PCNF0_S1LEN_Pos) |
                       (CONF_S0 << RADIO_PCNF0_S0LEN_Pos) |
                       (CONF_LEN << RADIO_PCNF0_LFLEN_Pos);
    NRF_RADIO->PCNF1 = (CONF_WHITENING << RADIO_PCNF1_WHITEEN_Pos) |
                       (CONF_ENDIAN << RADIO_PCNF1_ENDIAN_Pos) |
                       (CONF_BASE_ADDR_LEN << RADIO_PCNF1_BALEN_Pos) |
                       (CONF_STATLEN << RADIO_PCNF1_STATLEN_Pos) |
                       (CONF_PAYLOAD_LEN << RADIO_PCNF1_MAXLEN_Pos);
    /* configure CRC unit */
    NRF_RADIO->CRCCNF = CONF_CRC_LEN;
    NRF_RADIO->CRCPOLY = CONF_CRC_POLY;
    NRF_RADIO->CRCINIT = CONF_CRC_INIT;
    /* set shortcuts for more efficient transfer */
    NRF_RADIO->SHORTS = (1 << RADIO_SHORTS_READY_START_Pos);
    /* enable interrupts */
    NVIC_SetPriority(RADIO_IRQn, RADIO_IRQ_PRIO);
    NVIC_EnableIRQ(RADIO_IRQn);
    /* enable END interrupt */
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->INTENSET = (1 << RADIO_INTENSET_END_Pos);
    /* put device in receive mode */
    _switch_to_rx();
    return 0;
}

int _send(gnrc_netdev_t *dev, gnrc_pktsnip_t *pkt)
{
    (void)dev;
    size_t size;
    size_t pos = 0;
    uint8_t *dst_addr;
    gnrc_netif_hdr_t *hdr;
    gnrc_pktsnip_t *payload;

    /* check packet */
    if (pkt == NULL || pkt->next == NULL) {
        DEBUG("nrfmin: Error sending packet: packet incomplete\n");
        return -ENOMSG;
    }

    /* check if payload is withing length bounds */
    size = gnrc_pkt_len(pkt->next);
    if (size > CONF_PAYLOAD_LEN) {
        gnrc_pktbuf_release(pkt);
        DEBUG("nrfmin: Error sending packet: payload to large\n");
        return -EOVERFLOW;
    }
    /* get netif header and check address length */
    hdr = (gnrc_netif_hdr_t *)pkt->data;
    if (hdr->dst_l2addr_len != 2) {
        DEBUG("nrfmin: Error sending packet: dest address has invalid size\n");
        gnrc_pktbuf_release(pkt);
        return -ENOMSG;
    }
    dst_addr = gnrc_netif_hdr_get_dst_addr(hdr);

    DEBUG("nrfmin: Sending packet to %02x:%02x - size %u\n",
           dst_addr[0], dst_addr[1], size);

    /* wait for any ongoing transmission to finish */
    while (_state == STATE_TX);
    /* write data into TX buffer */
    payload = pkt->next;
    _tx_buf.length = 6 + size;
    _tx_buf.src_addr[0] = (uint8_t)(_addr >> 8);
    _tx_buf.src_addr[1] = (uint8_t)(_addr);
    _tx_buf.dst_addr[0] = dst_addr[0];
    _tx_buf.dst_addr[1] = dst_addr[1];
    _tx_buf.proto = _nettype_to_nrftype(payload->type);
    while (payload) {
        memcpy(&(_tx_buf.payload[pos]), payload->data, payload->size);
        pos += payload->size;
        payload = payload->next;
    }

    /* save old state and switch to idle if applicable */
    _tx_prestate = _state;
    if (_tx_prestate == STATE_RX) {
        _switch_to_idle();
    }
    /* set packet pointer to TX buffer and write destination address */
    NRF_RADIO->PACKETPTR = (uint32_t)(&_tx_buf);
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= ((((uint16_t)dst_addr[0]) << 8) | dst_addr[1]);
    /* start transmission */
    _state = STATE_TX;
    NRF_RADIO->TASKS_TXEN = 1;

    /* release packet */
    gnrc_pktbuf_release(pkt);
    return (int)size;
}

int _add_event_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    if (dev->event_cb != NULL) {
        return -ENOBUFS;
    }
    dev->event_cb = cb;
    return 0;
}

int _rem_event_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    if (dev->event_cb == cb) {
        dev->event_cb = NULL;
        return 0;
    }
    return -ENOENT;
}

int _get(gnrc_netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    (void)dev;

    switch (opt) {
        case NETOPT_ADDRESS:
            return _get_address(value, max_len);
        case NETOPT_CHANNEL:
            return _get_channel(value, max_len);
        case NETOPT_NID:
            return _get_pan(value, max_len);
        case NETOPT_TX_POWER:
            return _get_txpower(value, max_len);
        case NETOPT_STATE:
            return _get_state(value, max_len);
        default:
            return -ENOTSUP;
    }
}

int _set(gnrc_netdev_t *dev, netopt_t opt, void *value, size_t value_len)
{
    (void)dev;

    switch (opt) {
        case NETOPT_ADDRESS:
            return _set_address(value, value_len);
        case NETOPT_CHANNEL:
            return _set_channel(value, value_len);
        case NETOPT_NID:
            return _set_pan(value, value_len);
        case NETOPT_TX_POWER:
            return _set_txpower(value, value_len);
        case NETOPT_STATE:
            return _set_state(value, value_len);
        default:
            return -ENOTSUP;
    }
}

void _isr_event(gnrc_netdev_t *dev, uint32_t event_type)
{
    switch (event_type) {
        case ISR_EVENT_RX_DONE:
            _receive_data();
            break;
        default:
            /* do nothing */
            return;
    }
}

/*
 * Mapping of netdev interface
 */
const gnrc_netdev_driver_t nrfmin_driver = {
    .send_data = _send,
    .add_event_callback = _add_event_cb,
    .rem_event_callback = _rem_event_cb,
    .get = _get,
    .set = _set,
    .isr_event = _isr_event,
};
//
