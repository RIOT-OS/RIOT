/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf51_ble
 * @{
 *
 * @file
 * @brief       Implementation of the NRF51822 minimal BLE radio driver
 *
 * @author      Jan Wagner <mail@jwagner.eu>
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
#include "net/gnrc.h"
#include "hwtimer.h"
#include "net/ble_ll.h"

#define ENABLE_DEBUG		(0)
#include "debug.h"

/**
 * @brief   Driver specific device configuration
 * @{
 */
#define CONF_MODE               (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos)
#define CONF_PAYLOAD_LEN        (250U)
#define CONF_LEN                (8U)
#define CONF_S0                 (1U)
#define CONF_S1                 (0U)
#define CONF_STATLEN            (0U)
#define CONF_BASE_ADDR_LEN      (3U)
#define CONF_ENDIAN             RADIO_PCNF1_ENDIAN_Little
#define CONF_WHITENING          RADIO_PCNF1_WHITEEN_Enabled
#define CONF_CRC_LEN            (3U)
#define CONF_CRC_POLY           (0x00065b)
#define CONF_CRC_INIT           (0x555555)
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
 * @brief   Possible internal device states
 */
typedef enum {
    STATE_OFF,              /**< device is powered off */
    STATE_IDLE,             /**< device is in idle mode */
    STATE_RX,               /**< device is in receive mode */
    STATE_TX,               /**< device is transmitting data */
} state_t;

/**
 * @brief   Pointer to the MAC layer event callback
 */
static gnrc_netdev_t *_netdev = NULL;

/**
 * @brief   Current state of the device
 */
static volatile state_t _state = STATE_OFF;

/**
 * @brief   Hold the state before sending to return to it afterwards
 */
static state_t _tx_prestate;

/**
 * @brief    Double receive buffers
 */
static ble_adv_pkt_t _rx_buf[2];

/**
 * @brief Transmission buffer
 */
static uint8_t _tx_buf[CONF_PAYLOAD_LEN];

/**
 * @brief   BLE Address of the device
 */
static uint32_t _access_addr = BLE_DEFAULT_ACCESS_ADDR;

/**
 * @brief   Pointer to the free receive buffer
 */
static volatile int _rx_next = 0;



/* Switch the BLE radio state to idle */
static void _switch_to_idle(void)
{
    DEBUG("nrf51_ble: switch_to_idle\n");

    /* switch to idle state */
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_DISABLE = 1;

    while (NRF_RADIO->EVENTS_DISABLED == 0);

    _state = STATE_IDLE;
}

/* Switch the BLE radio state to receive */
static void _switch_to_rx(void)
{
    DEBUG("nrf51_ble: switch_to_rx\n");

    /* set pointer to receive buffer */
    NRF_RADIO->PACKETPTR = (uint32_t) & (_rx_buf[_rx_next]);

    /* switch int RX mode */
    NRF_RADIO->TASKS_RXEN = 1;
    _state = STATE_RX;
}

/* Get the BLE Access Address of the interface */
int _get_access_address(uint8_t *val, size_t max_len)
{
    /* check parameters */
    if (max_len < BLE_ACCESS_ADDR_LEN) {
        return -EOVERFLOW;
    }

    /* get address */
    val[0] = (uint8_t)(_access_addr >> 24);
    val[1] = (uint8_t)(_access_addr >> 16);
    val[2] = (uint8_t)(_access_addr >> 8);
    val[3] = (uint8_t)(_access_addr);
    return BLE_ACCESS_ADDR_LEN;
}

/* Set the BLE Access Address of the interface */
int _set_access_address(uint8_t *val, size_t len)
{
    int is_rx = 0;

    /* check parameters */
    if (len != BLE_ACCESS_ADDR_LEN) {
        return -EINVAL;
    }

    /* keep track of state */
    while (_state == STATE_TX);

    if (_state == STATE_RX) {
        is_rx = 1;
        _switch_to_idle();
    }

    /* set BLE access address */
    _access_addr = (((uint32_t)val[0]) << 24) | (((uint32_t)val[1]) << 16) |
                   (((uint32_t)val[2]) << 8) | val[3];

    NRF_RADIO->PREFIX0 = ((_access_addr >> 24) & 0x000000FF);
    NRF_RADIO->BASE0   = ((_access_addr << 8) & 0xFFFFFF00);

    /* restore original state */
    if (is_rx) {
        _switch_to_rx();
    }

    return BLE_ACCESS_ADDR_LEN;
}

/* Get the BLE radio state */
int _get_state(uint8_t *val, size_t max_len)
{
    netopt_state_t state;

    /* check parameters */
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

/* Set the BLE radio state */
int _set_state(uint8_t *val, size_t len)
{
    netopt_state_t state;

    /* check parameters */
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

/* Get the current BLE channel number of the interface */
int _get_channel(uint8_t *val, size_t max_len)
{
    /* check parameters */
    if (max_len < 2) {
        return -EOVERFLOW;
    }

    /* get channel */
    val[0] = freq_to_chan[0x3f & NRF_RADIO->FREQUENCY];
    val[1] = 0;
    return 2;
}

/* Set the current BLE channel number of the interface */
int _set_channel(uint8_t *val, size_t len)
{
    int is_rx = 0;

    /* check parameter */
    if (len != 2 || val[0] > BLE_MAX_CHANNELS) {
        return -EINVAL;
    }

    /* remember state */
    while (_state == STATE_TX);

    if (_state == STATE_RX) {
        is_rx = 1;
        _switch_to_idle();
    }

    /* set channel */
    NRF_RADIO->FREQUENCY = chan_to_freq[val[0]];
    NRF_RADIO->DATAWHITEIV = val[0];

    /* restore state */
    if (is_rx) {
        _switch_to_rx();
    }

    return 2;
}

/* Set the TX power of the BLE interface */
int _get_txpower(uint8_t *val, size_t len)
{
    /* check parameters */
    if (len < 2) {
        return -EINVAL;
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

/* Set the TX power of the BLE interface */
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

/* BLE interface interrupt routine */
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
            NRF_RADIO->PACKETPTR = (uint32_t) & (_rx_buf[_rx_next]);
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

/* Initialize BLE interface */
int nrf51_ble_init(gnrc_netdev_t *dev)
{
    /* check given device descriptor */
    if (dev == NULL) {
        return -ENODEV;
    }

    /* set initial values */
    dev->driver = &ble_driver;
    dev->event_cb = NULL;
    dev->mac_pid = KERNEL_PID_UNDEF;
    /* keep a pointer for future reference */
    _netdev = dev;

    /* power on the NRFs radio */
    NRF_RADIO->POWER = 1;
    /* load driver specific configuration */
    NRF_RADIO->MODE = CONF_MODE;
    /* configure variable parameters to default values */
    NRF_RADIO->TXPOWER = BLE_DEFAULT_TXPOWER;
    NRF_RADIO->FREQUENCY = chan_to_freq[BLE_DEFAULT_CHANNEL];

    /* pre-configure radio addresses */
    NRF_RADIO->PREFIX0 = ((_access_addr >> 24) & 0x000000FF);
    NRF_RADIO->BASE0   = ((_access_addr << 8) & 0xFFFFFF00);

    NRF_RADIO->TXADDRESS = 0x00UL;      /* always send from address 0 */
    NRF_RADIO->RXADDRESSES = 0x01UL;    /* listen to addresses 0 and 1 */

    /* configure data fields and packet length whitening and endianess */
    NRF_RADIO->PCNF0 = (CONF_S1 << RADIO_PCNF0_S1LEN_Pos) |
                       (CONF_S0 << RADIO_PCNF0_S0LEN_Pos) |
                       (CONF_LEN << RADIO_PCNF0_LFLEN_Pos);

    NRF_RADIO->PCNF1 = (CONF_WHITENING << RADIO_PCNF1_WHITEEN_Pos) |
                       (CONF_ENDIAN << RADIO_PCNF1_ENDIAN_Pos) |
                       (CONF_BASE_ADDR_LEN << RADIO_PCNF1_BALEN_Pos) |
                       //(CONF_STATLEN << RADIO_PCNF1_STATLEN_Pos) |
                       (CONF_PAYLOAD_LEN << RADIO_PCNF1_MAXLEN_Pos);
    /* configure CRC unit */
    NRF_RADIO->CRCCNF = RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos |
                        RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos;

    NRF_RADIO->CRCPOLY = CONF_CRC_POLY;
    NRF_RADIO->CRCINIT = CONF_CRC_INIT;
    NRF_RADIO->DATAWHITEIV = BLE_DEFAULT_CHANNEL;

    /* set shortcuts for more efficient transfer */
    NRF_RADIO->SHORTS = (1 << RADIO_SHORTS_READY_START_Pos);
    /* enable interrupts */
    NVIC_EnableIRQ(RADIO_IRQn);
    /* enable END interrupt */
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->INTENSET = (1 << RADIO_INTENSET_END_Pos);

    /* put device in receive mode */
    _switch_to_rx();

    return 0;
}

/* BLE interface low-level receive function */
static void _receive_data(void)
{
    ble_adv_pkt_t *data;
    gnrc_pktsnip_t *pkt;
    uint16_t pkt_size;

    /* only read data if we have somewhere to send it to */
    if (_netdev->event_cb == NULL) {
        DEBUG("nrf51_ble: receive_data (return)\n");
        return;
    }

    /* get pointer to RX data buffer */
    data = &(_rx_buf[_rx_next ^ 1]);

    pkt_size = BLE_ADV_HDR_LEN + data->header.length;

    /* allocate and fill payload */
    pkt = gnrc_pktbuf_add(NULL, data, pkt_size, GNRC_NETTYPE_UNDEF);

    if (pkt == NULL) {
        DEBUG("nrf51_ble: Error allocating packet payload on RX\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* pass on the received packet */
    _netdev->event_cb(NETDEV_EVENT_RX_COMPLETE, pkt);
}

/* BLE interface low-level send function */
int _send(gnrc_netdev_t *dev, gnrc_pktsnip_t *pkt)
{
    (void)dev;
    size_t size;

    /* check packet */
    if (pkt == NULL) {
        DEBUG("nrf51_ble: Error sending packet: packet incomplete\n");
        return -ENOMSG;
    }

    /* check if payload is withing length bounds */
    size = gnrc_pkt_len(pkt);

    if (size > CONF_PAYLOAD_LEN) {
        gnrc_pktbuf_release(pkt);
        DEBUG("nrf51_ble: Error sending packet: invalid packet length\n");
        return -EOVERFLOW;
    }

    memcpy(&(_tx_buf), pkt->data, size);

    /* wait for any ongoing transmission to finish */
    while (_state == STATE_TX);

    /* save old state and switch to idle if applicable */
    _tx_prestate = _state;

    if (_tx_prestate == STATE_RX) {
        _switch_to_idle();
    }

    /* set packet pointer to TX buffer and write destination address */
    NRF_RADIO->PACKETPTR = (uint32_t)(&_tx_buf);

    /* start transmission */
    _state = STATE_TX;
    NRF_RADIO->TASKS_TXEN = 1;

    /* release packet */
    gnrc_pktbuf_release(pkt);
    return (int)size;
}

/* Add gnrc_netdev_event callback function to gnrc_netdev */
int _add_event_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    if (dev->event_cb != NULL) {
        return -ENOBUFS;
    }

    dev->event_cb = cb;
    return 0;
}

/* Remove gnrc_netdev_event callback function from gnrc_netdev */
int _rem_event_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    if (dev->event_cb == cb) {
        dev->event_cb = NULL;
        return 0;
    }

    return -ENOENT;
}

/* Get netopt parameter value from current BLE interface configuration */
int _get(gnrc_netdev_t *dev, netopt_t opt,
         void *value, size_t max_len)
{
    (void)dev;

    switch (opt) {
        case NETOPT_BLE_ACCESS_ADDRESS:
            return _get_access_address(value, max_len);

        case NETOPT_CHANNEL:
            return _get_channel(value, max_len);

        case NETOPT_TX_POWER:
            return _get_txpower(value, max_len);

        case NETOPT_STATE:
            return _get_state(value, max_len);

        default:
            return -ENOTSUP;
    }
}

/* Set netopt parameter value in current BLE interface configuration */
int _set(gnrc_netdev_t *dev, netopt_t opt,
         void *value, size_t value_len)
{
    (void)dev;

    switch (opt) {
        case NETOPT_BLE_ACCESS_ADDRESS:
            return _set_access_address(value, value_len);

        case NETOPT_CHANNEL:
            return _set_channel(value, value_len);

        case NETOPT_TX_POWER:
            return _set_txpower(value, value_len);

        case NETOPT_STATE:
            return _set_state(value, value_len);

        default:
            return -ENOTSUP;
    }
}

/* Event handler */
void _isr_event(gnrc_netdev_t *dev, uint32_t event_type)
{
    switch (event_type) {
        case ISR_EVENT_RX_DONE:
             DEBUG("nrf51_ble; isr_event -> ISR_EVENT_RX_DONE\n");
            _receive_data();
            break;

        default:
            /* do nothing */
            return;
    }
}

/* Mapping of netdev interface */
const gnrc_netdev_driver_t ble_driver = {
    .send_data = _send,
    .add_event_callback = _add_event_cb,
    .rem_event_callback = _rem_event_cb,
    .get = _get,
    .set = _set,
    .isr_event = _isr_event,
};
