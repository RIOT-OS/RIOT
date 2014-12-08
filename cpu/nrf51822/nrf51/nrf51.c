/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_nrf51
 * @{
 *
 * @file
 * @brief       Implementation of the NRF51 radio driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "mutex.h"
#include "thread.h"
#include "sched.h"
#include "kernel_types.h"
#include "nrf51.h"
#include "periph_conf.h"

#ifdef MODULE_TRANSCEIVER
#include "transceiver.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief Change the radios internal state to DISABLED
 */
static void _switch_to_disabled(void);

/**
 * @brief Put the radio into receiving state
 */
static void _switch_to_rx(void);

/**
 * @brief Transmission buffer
 */
static uint8_t tx_buf[NRF51_CONF_MAX_PAYLOAD_LENGTH + 1];

/**
 * @brief Receiving buffers
 */
nrf51_packet_t nrf51_rx_buf[NRF51_RX_BUFSIZE];

/**
 * @brief Pointer to the currently used receive buffer
 */
static volatile uint8_t rx_buf_next = 0;

/**
 * @brief The current state of the radio
 */
static volatile nrf51_state_t state = NRF51_STATE_OFF;

/**
 * @brief The currently configured radio address
 */
static volatile uint16_t own_addr = NRF51_CONF_ADDR_OWN;

/**
 * @brief Pointer to a registered receive callback
 */
static nrf51_receive_cb_t recv_cb = NULL;

/**
 * @brief Save the transceivers PID in case the radio is used with the transceiver
 */
#ifdef MODULE_TRANSCEIVER
volatile kernel_pid_t transceiver_pid;
#endif


#ifdef MODULE_TRANSCEIVER
void nrf51_init_transceiver(kernel_pid_t trans_pid)
{
    DEBUG("nrf51: init_transceiver()\n");
    transceiver_pid = trans_pid;
    nrf51_init();
    _switch_to_rx();
}
#endif

int nrf51_init(void)
{
    DEBUG("nrf51: init()\n");

    /* power on the NRFs radio */
    NRF_RADIO->POWER = 1;

    /* configure the basic (default) radio parameters */
    NRF_RADIO->TXPOWER = NRF51_CONF_TXPOWER;
    NRF_RADIO->FREQUENCY = NRF51_CONF_CHANNEL;
    NRF_RADIO->MODE = RADIO_MODE_MODE_Nrf_2Mbit;

    /* pre-configure radio addresses */
    NRF_RADIO->PREFIX0 = NRF51_CONF_PREFIX0;
    NRF_RADIO->PREFIX1 = NRF51_CONF_PREFIX1;
    NRF_RADIO->BASE0   = NRF51_CONF_BASE0 | NRF51_CONF_ADDR_OWN;
    NRF_RADIO->BASE1   = NRF51_CONF_BASE1 | NRF51_CONF_ADDR_BROADCAST;
    NRF_RADIO->TXADDRESS = 0x00UL;      /* always send from address 0 */
    NRF_RADIO->RXADDRESSES = 0x03UL;    /* listen to logical addresses 0 (own) and 1 (broadcast) */

    /* configure data fields and packet length whitening and endianess */
    NRF_RADIO->PCNF0 = (NRF51_CONF_S1 << RADIO_PCNF0_S1LEN_Pos) |
                       (NRF51_CONF_S0 << RADIO_PCNF0_S0LEN_Pos) |
                       (NRF51_CONF_LEN << RADIO_PCNF0_LFLEN_Pos);
    NRF_RADIO->PCNF1 = (NRF51_CONF_WHITENING << RADIO_PCNF1_WHITEEN_Pos) |
                       (NRF51_CONF_ENDIAN << RADIO_PCNF1_ENDIAN_Pos) |
                       (NRF51_CONF_BASE_ADDR_LEN << RADIO_PCNF1_BALEN_Pos) |
                       (NRF51_CONF_STATLEN << RADIO_PCNF1_STATLEN_Pos) |
                       (NRF51_CONF_MAX_PAYLOAD_LENGTH << RADIO_PCNF1_MAXLEN_Pos);

    /* set shortcuts for more efficient transfer */
    NRF_RADIO->SHORTS = (1 << RADIO_SHORTS_READY_START_Pos);

    /* configure CRC unit */
    NRF_RADIO->CRCCNF = NRF51_CONF_CRC_LEN;
    NRF_RADIO->CRCPOLY = NRF51_CONF_CRC_POLY;
    NRF_RADIO->CRCINIT = 0xf0f0f0;

    /* enable interrupts */
    NVIC_SetPriority(RADIO_IRQn, RADIO_IRQ_PRIO);
    NVIC_EnableIRQ(RADIO_IRQn);
    return 0;
}

int nrf51_register_receive_cb(nrf51_receive_cb_t cb)
{
    /* test if no other callback is already registered */
    if (recv_cb) {
        return -1;
    }
    recv_cb = cb;
    return 0;
}

int nrf51_unregister_receive_cb(nrf51_receive_cb_t cb)
{
    (void)cb;

    recv_cb = 0;
    return 0;
}

int nrf51_set_state(nrf51_state_t new_state)
{
    nrf51_state_t old_state = state;

    switch (new_state) {
        case NRF51_STATE_OFF:
            _switch_to_disabled();
            break;
        case NRF51_STATE_RX:
            _switch_to_rx();
            break;
        default:
            return -1;
    }
    return old_state;
}

nrf51_state_t nrf51_get_state(void)
{
    return state;
}

int nrf51_send(uint16_t addr, uint8_t *data, uint8_t length)
{
    nrf51_state_t old_state = state;

    if (length > NRF51_CONF_MAX_PAYLOAD_LENGTH) {
        DEBUG("nrf51 TX: payload too large, dropping package\n");
        return 0;
    }

    /* prepare TX buffer */
    tx_buf[0] = length;
    memcpy(&tx_buf[1], data, length);

    DEBUG("nrf51 TX: sending %i byte to addr %i\n", tx_buf[0], addr);

    /* point radio to the packet that it has to send */
    NRF_RADIO->PACKETPTR = (uint32_t)tx_buf;
    /* switch back to disabled in case we are in RX state */
    if (state == NRF51_STATE_RX) {
        _switch_to_disabled();
    }
    /* configure shorts */
    NRF_RADIO->SHORTS |= (1 << RADIO_SHORTS_END_DISABLE_Pos);
    /* set destination address */
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= addr;
    /* send out packet */
    DEBUG("nrf51 STATE: TX\n");
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_TXEN = 1;
    while(NRF_RADIO->EVENTS_DISABLED == 0);
    DEBUG("nrf51 STATE: OFF\n");

    /* if radio was receiving before, go back into RX state */
    if (old_state == NRF51_STATE_RX) {
        _switch_to_rx();
    }

    return (int)length;
}

int nrf51_set_address(uint16_t address)
{
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= address;
    own_addr = address;
    return 0;
}

uint16_t nrf51_get_address(void)
{
    return own_addr;
}

int nrf51_set_broadcast_address(uint16_t address)
{
    NRF_RADIO->BASE1 &= ~(0xffff);
    NRF_RADIO->BASE1 |= address;
    return 0;
}

uint16_t nrf51_get_broadcast_address(void)
{
    return (uint16_t)(NRF_RADIO->BASE1 & 0xffff);
}

int nrf51_set_channel(uint8_t channel)
{
    uint8_t chan = channel & 0x3f;
    if (chan == channel) {
        if (state == NRF51_STATE_RX) {
            _switch_to_disabled();
        }
        NRF_RADIO->FREQUENCY = chan;
        if (state == NRF51_STATE_RX) {
            _switch_to_rx();
        }
        return (int)chan;
    }
    return -1;
}

int nrf51_get_channel(void)
{
    return (uint8_t)(0x3f & NRF_RADIO->FREQUENCY);
}

int nrf51_set_txpower(nrf51_txpower_t power)
{
    NRF_RADIO->TXPOWER = (power & 0xff);
    return 0;
}

nrf51_txpower_t nrf51_get_txpower(void)
{
    return (NRF_RADIO->TXPOWER & 0xff);
}

void nrf51_poweron(void)
{
    NRF_RADIO->POWER = 1;
}

void nrf51_poweroff(void)
{
    if (state == NRF51_STATE_RX) {
        _switch_to_disabled();
    }
    NRF_RADIO->POWER = 0;
}

static void _switch_to_disabled(void)
{
    /* disable END interrupt */
    NRF_RADIO->INTENCLR = (1 << RADIO_INTENSET_END_Pos);
    /* switch into DISABLED mode and wait until we are there */
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0);
    state = NRF51_STATE_OFF;
    DEBUG("nrf51 STATE: OFF\n");
}

static void _switch_to_rx(void)
{
    /* set pointer to receive buffer */
    NRF_RADIO->PACKETPTR = (uint32_t)&nrf51_rx_buf[rx_buf_next];
    /* set address */
    NRF_RADIO->BASE0 &= ~(0xffff);
    NRF_RADIO->BASE0 |= own_addr;
    /* configure shorts */
    NRF_RADIO->SHORTS &= ~(1 << RADIO_SHORTS_END_DISABLE_Pos);
    /* enable END interrupt */
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->INTENSET = (1 << RADIO_INTENSET_END_Pos);
    /* switch int RX mode */
    NRF_RADIO->TASKS_RXEN = 1;
    state = NRF51_STATE_RX;
    DEBUG("nrf51 STATE: RX\n");
}

void isr_radio(void)
{
    DEBUG("nrf51 ISR\n");
    if (NRF_RADIO->EVENTS_END == 1) {
        NRF_RADIO->EVENTS_END = 0;
        if (NRF_RADIO->CRCSTATUS != 1)
        {
            DEBUG("nrf51 RX: CRC failed - dropping package\n");
        }
        else {
            DEBUG("nrf51 RX: CRC ok, got %i byte\n", nrf51_rx_buf[rx_buf_next].length);
#ifdef MODULE_TRANSCEIVER
            /* notify transceiver thread if any */
            if (transceiver_pid != KERNEL_PID_UNDEF) {
                msg_t m;
                m.type = (uint16_t) RCV_PKT_NRF51;
                m.content.value = (uint16_t)rx_buf_next;
                msg_send(&m, transceiver_pid);
                DEBUG("nrf51 RX: send message to tranceiver\n");
            }
#else
            if (recv_cb) {
                recv_cb((uint8_t *)&nrf51_rx_buf[rx_buf_next].payload,
                        nrf51_rx_buf[rx_buf_next].length);
            }
#endif
            if (++rx_buf_next == NRF51_RX_BUFSIZE) {
                rx_buf_next = 0;
            }
            NRF_RADIO->PACKETPTR = (uint32_t)&nrf51_rx_buf[rx_buf_next];
        }
        /* and here we go again */
        NRF_RADIO->TASKS_START = 1;
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
