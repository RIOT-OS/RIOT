/**
 * Generic transceiver module as an interface to NIC driver.
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup transceiver
 * @{
 * @file    transceiver.c
 * @brief   Providing a generic interface to the driver implementation for any supported network device.
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#include "thread.h"
#include "msg.h"
#include "irq.h"

#include "radio/types.h"

#include "transceiver.h"

/* supported transceivers */
#ifdef MODULE_CC110X_LEGACY_CSMA
#include "cc110x_legacy_csma.h"
#endif

#ifdef MODULE_CC110X_LEGACY
#include "cc110x_legacy.h"
#endif

#ifdef MODULE_CC2420
#include "cc2420.h"
#include "ieee802154_frame.h"
#endif

#ifdef MODULE_MC1322X
#include "mc1322x.h"
#include "maca.h"
#include "maca_packet.h"
#include "ieee802154_frame.h"
#endif

#ifdef MODULE_NATIVENET
#include "nativenet.h"
#include "nativenet_internal.h"
#endif

#ifdef MODULE_AT86RF231
#include "at86rf231.h"
#include "ieee802154_frame.h"
#endif

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
//#define DEBUG_ENABLED
#undef TRANSCEIVER_STACK_SIZE
#define TRANSCEIVER_STACK_SIZE      (1024)
#endif
#include "debug.h"

/*------------------------------------------------------------------------------------*/
/* used transceiver types */
transceiver_type_t transceivers = TRANSCEIVER_NONE;

/* registered upper layer threads */
registered_t reg[TRANSCEIVER_MAX_REGISTERED];

/* packet buffers */
#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
ieee802154_packet_t transceiver_buffer[TRANSCEIVER_BUFFER_SIZE];
#else
radio_packet_t transceiver_buffer[TRANSCEIVER_BUFFER_SIZE];
#endif
uint8_t data_buffer[TRANSCEIVER_BUFFER_SIZE * PAYLOAD_SIZE];

/* message buffer */
msg_t msg_buffer[TRANSCEIVER_MSG_BUFFER_SIZE];

uint32_t response; ///< response bytes for messages to upper layer threads

volatile kernel_pid_t transceiver_pid = KERNEL_PID_UNDEF; ///< the transceiver thread's pid

static volatile uint8_t rx_buffer_pos = 0;
static volatile uint8_t transceiver_buffer_pos = 0;

#ifdef MODULE_CC110X_LEGACY_CSMA
void *cc1100_payload;
int cc1100_payload_size;
packet_info_t *cc1100_packet_info;
uint8_t cc1100_pkt[CC1100_MAX_DATA_LENGTH];
#endif


/* transceiver stack */
char transceiver_stack[TRANSCEIVER_STACK_SIZE];

/*------------------------------------------------------------------------------------*/
/* function prototypes */
static void *run(void *arg);
static void receive_packet(uint16_t type, uint8_t pos);
#ifdef MODULE_CC110X_LEGACY
static void receive_cc110x_packet(radio_packet_t *trans_p);
#endif
#ifdef MODULE_CC110X_LEGACY_CSMA
void cc1100_packet_monitor(void *payload, int payload_size, protocol_t protocol, packet_info_t *packet_info);
void receive_cc1100_packet(radio_packet_t *trans_p);
#endif
#ifdef MODULE_CC2420
static void receive_cc2420_packet(ieee802154_packet_t *trans_p);
#endif
#ifdef MODULE_NATIVENET
static void receive_nativenet_packet(radio_packet_t *trans_p);
#endif
#ifdef MODULE_MC1322X
void receive_mc1322x_packet(ieee802154_packet_t *trans_p);
#endif
#ifdef MODULE_AT86RF231
void receive_at86rf231_packet(ieee802154_packet_t *trans_p);
#endif
static int8_t send_packet(transceiver_type_t t, void *pkt);
static int32_t get_channel(transceiver_type_t t);
static int32_t set_channel(transceiver_type_t t, void *channel);
static radio_address_t get_address(transceiver_type_t t);
static radio_address_t set_address(transceiver_type_t t, void *address);
static transceiver_eui64_t get_long_addr(transceiver_type_t t);
static transceiver_eui64_t set_long_addr(transceiver_type_t t,
        void *address);
static int32_t get_pan(transceiver_type_t t);
static int32_t set_pan(transceiver_type_t t, void *pan);

static void set_monitor(transceiver_type_t t, void *mode);
static void powerdown(transceiver_type_t t);
static void switch_to_rx(transceiver_type_t t);

#ifdef DBG_IGNORE
static int16_t ignore_add(transceiver_type_t transceiver, void *address);

radio_address_t transceiver_ignored_addr[TRANSCEIVER_MAX_IGNORED_ADDR];
#endif

/*------------------------------------------------------------------------------------*/
/* Transceiver init */
void transceiver_init(transceiver_type_t t)
{
    uint8_t i;

    if (transceiver_pid != KERNEL_PID_UNDEF) {
        /* do not re-initialize an already running transceiver */
        return;
    }

    /* Initializing transceiver buffer and data buffer */
    memset(transceiver_buffer, 0, sizeof(transceiver_buffer));
    memset(data_buffer, 0, TRANSCEIVER_BUFFER_SIZE * PAYLOAD_SIZE);
#ifdef DBG_IGNORE
    memset(transceiver_ignored_addr, 0, sizeof(transceiver_ignored_addr));
#endif

    for (i = 0; i < TRANSCEIVER_MAX_REGISTERED; i++) {
        reg[i].transceivers = TRANSCEIVER_NONE;
        reg[i].pid          = KERNEL_PID_UNDEF;
    }

    /* check if a non defined bit is set */
    if (t & ~(TRANSCEIVER_CC1100 | TRANSCEIVER_CC2420 | TRANSCEIVER_MC1322X | TRANSCEIVER_NATIVE | TRANSCEIVER_AT86RF231)) {
        puts("Invalid transceiver type");
    }
    else {
        transceivers |= t;
    }
}

/* Start the transceiver thread */
kernel_pid_t transceiver_start(void)
{
    transceiver_pid = thread_create(transceiver_stack, TRANSCEIVER_STACK_SIZE, PRIORITY_MAIN - 3, CREATE_STACKTEST, run, NULL, "Transceiver");

    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Error creating transceiver thread");
    }

#ifdef MODULE_CC110X_LEGACY
    else if (transceivers & TRANSCEIVER_CC1100) {
        DEBUG("transceiver: Transceiver started for CC1100\n");
        cc110x_init(transceiver_pid);
    }

#endif
#ifdef MODULE_CC110X_LEGACY_CSMA
    else if (transceivers & TRANSCEIVER_CC1100) {
        DEBUG("transceiver: Transceiver started for CC1100\n");
        cc1100_init();
        cc1100_set_packet_monitor(cc1100_packet_monitor);
    }

#endif
#ifdef MODULE_CC2420
    else if (transceivers & TRANSCEIVER_CC2420) {
        DEBUG("transceiver: Transceiver started for CC2420\n");
        cc2420_init(transceiver_pid);
    }

#endif
#ifdef MODULE_AT86RF231
    else if (transceivers & TRANSCEIVER_AT86RF231) {
        DEBUG("transceiver: Transceiver started for AT86RF231\n");
        at86rf231_init(transceiver_pid);
    }

#endif
#ifdef MODULE_MC1322X
    else if (transceivers & TRANSCEIVER_MC1322X) {
        maca_init();
    }

#endif
#ifdef MODULE_NATIVENET
    else if (transceivers & TRANSCEIVER_NATIVE) {
        nativenet_init(transceiver_pid);
    }

#endif
    return transceiver_pid;
}

/* Register an upper layer thread */
uint8_t transceiver_register(transceiver_type_t t, kernel_pid_t pid)
{
    int result = 0;
    unsigned state = disableIRQ();
    for (size_t i = 0; i < TRANSCEIVER_MAX_REGISTERED; i++) {
        if ((reg[i].pid == pid) || (reg[i].transceivers == TRANSCEIVER_NONE)) {
            reg[i].transceivers |= t;
            reg[i].pid = pid;
            DEBUG("transceiver: Thread %i registered for %i\n", reg[i].pid, reg[i].transceivers);
            restoreIRQ(state);
            result = 1;            
            break;
        }
    }
    restoreIRQ(state);
    return result;
}

/* Unregister an upper layer thread */
uint8_t transceiver_unregister(transceiver_type_t t, kernel_pid_t pid)
{
    int result = 0;
    unsigned state = disableIRQ();
    for (size_t i = 0; i < TRANSCEIVER_MAX_REGISTERED; ++i) {
        if (reg[i].pid == pid) {
            reg[i].transceivers &= ~t;
            restoreIRQ(state);
            result = 1;
            break;
        }
    }
    restoreIRQ(state);
    return result;
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/*
 * @brief The main thread run, receiving and processing messages in an infinite
 * loop
 */
static void *run(void *arg)
{
    (void) arg;

    msg_init_queue(msg_buffer, TRANSCEIVER_MSG_BUFFER_SIZE);

    while (1) {
        DEBUG("transceiver: Waiting for next message\n");
        msg_t m;
        msg_receive(&m);
        /* only makes sense for messages for upper layers */
        transceiver_command_t *cmd = (transceiver_command_t *) m.content.ptr;
        DEBUG("transceiver: Transceiver: Message received, type: %02X\n", m.type);
        switch (m.type) {
            case RCV_PKT_CC1020:
            case RCV_PKT_CC1100:
            case RCV_PKT_CC2420:
            case RCV_PKT_MC1322X:
            case RCV_PKT_NATIVE:
            case RCV_PKT_AT86RF231:
                receive_packet(m.type, m.content.value);
                break;

            case SND_PKT:
                response = send_packet(cmd->transceivers, cmd->data);
                m.content.value = response;
                msg_reply(&m, &m);
                break;

            case GET_CHANNEL:
                *((int32_t *) cmd->data) = get_channel(cmd->transceivers);
                msg_reply(&m, &m);
                break;

            case SET_CHANNEL:
                *((int32_t *) cmd->data) = set_channel(cmd->transceivers, cmd->data);
                msg_reply(&m, &m);
                break;

            case GET_ADDRESS:
                *((radio_address_t *) cmd->data) = get_address(cmd->transceivers);
                msg_reply(&m, &m);
                break;

            case SET_ADDRESS:
                *((radio_address_t *) cmd->data) = set_address(cmd->transceivers, cmd->data);
                msg_reply(&m, &m);
                break;

            case GET_LONG_ADDR:
                *((transceiver_eui64_t *) cmd->data) = get_long_addr(cmd->transceivers);
                msg_reply(&m, &m);
                break;

            case SET_LONG_ADDR:
                *((transceiver_eui64_t *) cmd->data) = set_long_addr(cmd->transceivers, cmd->data);
                msg_reply(&m, &m);
                break;

            case SET_MONITOR:
                set_monitor(cmd->transceivers, cmd->data);
                break;

            case POWERDOWN:
                powerdown(cmd->transceivers);
                break;

            case SWITCH_RX:
                switch_to_rx(cmd->transceivers);
                break;

            case GET_PAN:
                *((int32_t *) cmd->data) = get_pan(cmd->transceivers);
                msg_reply(&m, &m);
                break;

            case SET_PAN:                
                *((int32_t *) cmd->data) = set_pan(cmd->transceivers, cmd->data);
                msg_reply(&m, &m);
                break;

#ifdef DBG_IGNORE
            case DBG_IGN:
                *((int16_t *) cmd->data) = ignore_add(cmd->transceivers, cmd->data);
                msg_reply(&m, &m);
                break;
#endif

            default:
                DEBUG("transceiver: Unknown message received\n");
                break;
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------------*/
/*
 * @brief Processes a packet received by any transceiver device
 *
 * @param type  The message type to determine which device has received the
 * packet
 * @param pos   The current device driver's buffer position
 */
static void receive_packet(uint16_t type, uint8_t pos)
{
    size_t i = 0;
    transceiver_type_t t;
    rx_buffer_pos = pos;
    msg_t m;

    DEBUG("Packet received\n");

    switch (type) {
        case RCV_PKT_CC1020:
            t = TRANSCEIVER_CC1020;
            break;

        case RCV_PKT_CC1100:
            t = TRANSCEIVER_CC1100;
            break;

        case RCV_PKT_CC2420:
            t = TRANSCEIVER_CC2420;
            break;

        case RCV_PKT_MC1322X:
            t = TRANSCEIVER_MC1322X;
            break;

        case RCV_PKT_NATIVE:
            t = TRANSCEIVER_NATIVE;
            break;

        case RCV_PKT_AT86RF231:
            t = TRANSCEIVER_AT86RF231;
            break;

        default:
            t = TRANSCEIVER_NONE;
            break;
    }

    /* search first free position in transceiver buffer */
    for (i = 0; (i < TRANSCEIVER_BUFFER_SIZE) && (transceiver_buffer[transceiver_buffer_pos].processing); i++) {
        if (++transceiver_buffer_pos == TRANSCEIVER_BUFFER_SIZE) {
            transceiver_buffer_pos = 0;
        }
    }

    /* no buffer left */
    if (i >= TRANSCEIVER_BUFFER_SIZE) {
        /* inform upper layers of lost packet */
        m.type = ENOBUFFER;
        m.content.value = t;
        DEBUGF("transceiver: buffer size exceeded, dropping packet\n");
    }
    /* copy packet and handle it */
    else {
        m.type = PKT_PENDING;

        /* pass a null pointer if a packet from a undefined transceiver is
         * received */
        if (type == RCV_PKT_CC1100) {
#ifdef MODULE_CC110X_LEGACY
            radio_packet_t *trans_p = &(transceiver_buffer[transceiver_buffer_pos]);
            receive_cc110x_packet(trans_p);
#elif MODULE_CC110X_LEGACY_CSMA
            radio_packet_t *trans_p = &(transceiver_buffer[transceiver_buffer_pos]);
            receive_cc1100_packet(trans_p);
#endif
        }
        else if (type == RCV_PKT_MC1322X) {
#ifdef MODULE_MC1322X
            ieee802154_packet_t *trans_p = &(transceiver_buffer[transceiver_buffer_pos]);
            receive_mc1322x_packet(trans_p);
#endif
        }
        else if (type == RCV_PKT_CC2420) {
#ifdef MODULE_CC2420
            ieee802154_packet_t *trans_p = &(transceiver_buffer[transceiver_buffer_pos]);
            receive_cc2420_packet(trans_p);
#endif
        }
        else if (type == RCV_PKT_AT86RF231) {
#ifdef MODULE_AT86RF231
            ieee802154_packet_t *trans_p = &(transceiver_buffer[transceiver_buffer_pos]);
            receive_at86rf231_packet(trans_p);
#endif
        }
        else if (type == RCV_PKT_NATIVE) {
#ifdef MODULE_NATIVENET
            radio_packet_t *trans_p = &(transceiver_buffer[transceiver_buffer_pos]);
            receive_nativenet_packet(trans_p);
#endif
        }
        else {
            puts("Invalid transceiver type");
            return;
        }

#ifdef DBG_IGNORE

        for (size_t j = 0; (j < TRANSCEIVER_MAX_IGNORED_ADDR) && (transceiver_ignored_addr[j]); j++) {
            DEBUG("check if source (%u) is ignored -> %u\n", transceiver_buffer[transceiver_buffer_pos].src, transceiver_ignored_addr[j]);

            if (transceiver_buffer[transceiver_buffer_pos].src == transceiver_ignored_addr[j]) {
                DEBUG("ignored packet from %" PRIu16 "\n", transceiver_buffer[transceiver_buffer_pos].src);
                return;
            }
        }

#endif
    }

    /* finally notify waiting upper layers
     * this is done non-blocking, so packets can get lost */
    i = 0;

    while (reg[i].transceivers != TRANSCEIVER_NONE) {
        if (reg[i].transceivers & t) {
            m.content.ptr = (char *) &(transceiver_buffer[transceiver_buffer_pos]);
            DEBUG("transceiver: Notify thread %" PRIkernel_pid "\n", reg[i].pid);

            if (msg_try_send(&m, reg[i].pid) && (m.type != ENOBUFFER)) {
                transceiver_buffer[transceiver_buffer_pos].processing++;
            }
            else {
                DEBUGF("transceiver: failed to notify upper layer.\n");
            }
        }

        i++;
    }
}

#ifdef MODULE_CC110X_LEGACY
/*
 * @brief process packets from CC1100
 *
 * @param trans_p   The current entry in the transceiver buffer
 */
static void receive_cc110x_packet(radio_packet_t *trans_p)
{
    DEBUG("transceiver: Handling CC1100 packet\n");
    /* disable interrupts while copying packet */
    dINT();
    cc110x_packet_t p = cc110x_rx_buffer[rx_buffer_pos].packet;

    trans_p->src = p.phy_src;
    trans_p->dst = p.address;
    trans_p->rssi = cc110x_rx_buffer[rx_buffer_pos].rssi;
    trans_p->lqi = cc110x_rx_buffer[rx_buffer_pos].lqi;
    trans_p->length = p.length - CC1100_HEADER_LENGTH;
    memcpy((void *) &(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), p.data, CC1100_MAX_DATA_LENGTH);
    eINT();

    trans_p->data = (uint8_t *) &(data_buffer[transceiver_buffer_pos * CC1100_MAX_DATA_LENGTH]);
    DEBUG("transceiver: Packet %p (%p) was from %hu to %hu, size: %u\n", trans_p, trans_p->data, trans_p->src, trans_p->dst, trans_p->length);
}
#endif

#ifdef MODULE_CC110X_LEGACY_CSMA
void receive_cc1100_packet(radio_packet_t *trans_p)
{
    dINT();
    trans_p->src = cc1100_packet_info->source;
    trans_p->dst = cc1100_packet_info->destination;
    trans_p->rssi = cc1100_packet_info->rssi;
    trans_p->lqi = cc1100_packet_info->lqi;
    trans_p->length = cc1100_payload_size;
    memcpy((void *) &(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), cc1100_payload, CC1100_MAX_DATA_LENGTH);
    eINT();

    trans_p->data = (uint8_t *) &(data_buffer[transceiver_buffer_pos * CC1100_MAX_DATA_LENGTH]);
    DEBUG("transceiver: Packet %p (%p) was from %hu to %hu, size: %u\n", trans_p, trans_p->data, trans_p->src, trans_p->dst, trans_p->length);
}
#endif


#ifdef MODULE_CC2420
void receive_cc2420_packet(ieee802154_packet_t *trans_p)
{
    DEBUG("transceiver: Handling CC2420 packet\n");
    dINT();
    cc2420_packet_t *p = &cc2420_rx_buffer[rx_buffer_pos];
    trans_p->length = p->length;
    memcpy(&trans_p->frame, &p->frame, p->length);
    trans_p->rssi = p->rssi;
    trans_p->crc = p->crc;
    trans_p->lqi = p->lqi;
    memcpy(&data_buffer[transceiver_buffer_pos * CC2420_MAX_DATA_LENGTH],
           p->frame.payload, p->frame.payload_len);
    trans_p->frame.payload = (uint8_t *) & (data_buffer[transceiver_buffer_pos * CC2420_MAX_DATA_LENGTH]);
    trans_p->frame.payload_len = p->frame.payload_len;
    eINT();

#ifdef DEBUG_ENABLED

    if (trans_p->frame.fcf.dest_addr_m == IEEE_802154_SHORT_ADDR_M) {
        if (trans_p->frame.fcf.src_addr_m == IEEE_802154_SHORT_ADDR_M) {
            DEBUG("Packet %p was from %" PRIu16 " to %" PRIu16 ", size: %u\n", trans_p, *((uint16_t *) &trans_p->frame.src_addr[0]), *((uint16_t *) &trans_p->frame.dest_addr), trans_p->frame.payload_len);
        }
        else if (trans_p->frame.fcf.src_addr_m == IEEE_802154_LONG_ADDR_M) {
            DEBUG("Packet %p was from %016" PRIx64 " to %" PRIu16 ", size: %u\n", trans_p, *((uint64_t *) &trans_p->frame.src_addr[0]), *((uint16_t *) &trans_p->frame.dest_addr), trans_p->frame.payload_len);

        }
        else {
            DEBUG("Illegal source address mode: %d\n", trans_p->frame.fcf.src_addr_m);
            return;
        }
    }
    else if (trans_p->frame.fcf.dest_addr_m == IEEE_802154_LONG_ADDR_M) {
        if (trans_p->frame.fcf.src_addr_m == IEEE_802154_SHORT_ADDR_M) {
            DEBUG("Packet %p was from %" PRIu16 " to %016" PRIx64 ", size: %u\n", trans_p, *((uint16_t *) &trans_p->frame.src_addr[0]), *((uint64_t *) &trans_p->frame.dest_addr), trans_p->frame.payload_len);
        }
        else if (trans_p->frame.fcf.src_addr_m == IEEE_802154_LONG_ADDR_M) {
            DEBUG("Packet %p was from %016" PRIx64 " to %016" PRIx64 ", size: %u\n", trans_p, *((uint64_t *) &trans_p->frame.src_addr[0]), *((uint16_t *) &trans_p->frame.dest_addr), trans_p->frame.payload_len);

        }
        else {
            DEBUG("Illegal source address mode: %d\n", trans_p->frame.fcf.src_addr_m);
            return;
        }
    }
    else {
        DEBUG("Illegal destination address mode: %d\n", trans_p->frame.fcf.src_addr_m);
        return;
    }
#endif
    DEBUG("transceiver: Content: %s\n", trans_p->frame.payload);
}
#endif

#ifdef MODULE_MC1322X
void receive_mc1322x_packet(ieee802154_packet_t *trans_p)
{
    maca_packet_t *maca_pkt;
    dINT();
    maca_pkt = maca_get_rx_packet();
    trans_p->lqi = maca_pkt->lqi;
    trans_p->length = maca_pkt->length;
    memcpy((void *) &(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), maca_pkt->data, MACA_MAX_PAYLOAD_SIZE);
    maca_free_packet(maca_pkt);
    eINT();

    trans_p->frame.payload = (uint8_t *) &(data_buffer[transceiver_buffer_pos * MACA_MAX_PAYLOAD_SIZE]);
}
#endif


#ifdef MODULE_NATIVENET
void receive_nativenet_packet(radio_packet_t *trans_p)
{
    unsigned state;
    radio_packet_t *p = &_nativenet_rx_buffer[rx_buffer_pos].packet;

    /* disable interrupts while copying packet */
    state = disableIRQ();

    DEBUG("Handling nativenet packet\n");

    memcpy(trans_p, p, sizeof(radio_packet_t));
    memcpy(&(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), p->data, p->length);
    trans_p->data = (uint8_t *) &(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]);

    DEBUG("Packet %p was from %" PRIu16 " to %" PRIu16 ", size: %" PRIu8 "\n", trans_p, trans_p->src, trans_p->dst, trans_p->length);

    /* reset interrupts */
    restoreIRQ(state);
}
#endif

#ifdef MODULE_AT86RF231
void receive_at86rf231_packet(ieee802154_packet_t *trans_p)
{
    DEBUG("Handling AT86RF231 packet\n");
    dINT();
    at86rf231_packet_t *p = &at86rf231_rx_buffer[rx_buffer_pos];
    trans_p->length = p->length;
    trans_p->rssi = p->rssi;
    trans_p->crc = p->crc;
    trans_p->lqi = p->lqi;
    memcpy(&trans_p->frame, &p->frame, sizeof(p->frame));//p->length);
    memcpy(&data_buffer[transceiver_buffer_pos * AT86RF231_MAX_DATA_LENGTH], p->frame.payload,
           p->frame.payload_len);
    trans_p->frame.payload = (uint8_t *) & (data_buffer[transceiver_buffer_pos * AT86RF231_MAX_DATA_LENGTH]);
    trans_p->frame.payload_len = p->frame.payload_len;
    eINT();

#ifdef DEBUG_ENABLED

    if (trans_p->frame.fcf.dest_addr_m == IEEE_802154_SHORT_ADDR_M) {
        if (trans_p->frame.fcf.src_addr_m == IEEE_802154_SHORT_ADDR_M) {
            DEBUG("Packet %p was from %" PRIu16 " to %" PRIu16 ", size: %u\n", trans_p, *((uint16_t *) &trans_p->frame.src_addr[0]), *((uint16_t *) &trans_p->frame.dest_addr), trans_p->frame.payload_len);
        }
        else {
            DEBUG("Packet %p was from %016" PRIx64 " to %" PRIu16 ", size: %u\n", trans_p, *((uint64_t *) &trans_p->frame.src_addr[0]), *((uint16_t *) &trans_p->frame.dest_addr), trans_p->frame.payload_len);

        }
    }
    else {
        if (trans_p->frame.fcf.src_addr_m == IEEE_802154_SHORT_ADDR_M) {
            DEBUG("Packet %p was from %" PRIu16 " to %016" PRIx64 ", size: %u\n", trans_p, *((uint16_t *) &trans_p->frame.src_addr[0]), *((uint64_t *) &trans_p->frame.dest_addr), trans_p->frame.payload_len);
        }
        else {
            DEBUG("Packet %p was from %016" PRIx64 " to %016" PRIx64 ", size: %u\n", trans_p, *((uint64_t *) &trans_p->frame.src_addr[0]), *((uint16_t *) &trans_p->frame.dest_addr), trans_p->frame.payload_len);

        }
    }

#endif
    DEBUG("Content: %s\n", trans_p->frame.payload);
}
#endif
/*------------------------------------------------------------------------------------*/
/*
 * @brief Sends a radio packet to the receiver
 *
 * @param t     The transceiver device
 * @param pkt   Generic pointer to the packet (use ieee802154_packet_t for
 *              AT86RF231, CC2420, and MC1322X)
 *
 * @return A negative value if operation failed, 0 or the number of bytes sent otherwise.
 */
static int8_t send_packet(transceiver_type_t t, void *pkt)
{
    int8_t res = -1;
#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
    ieee802154_packet_t *p = (ieee802154_packet_t *)pkt;
    DEBUG("transceiver: Send packet to ");
#ifdef DEBUG_ENABLED

    for (size_t i = 0; i < 8; i++) {
        printf("%02x ", p->frame.dest_addr[i]);
    }

    printf("\n");
#endif
#else
    radio_packet_t *p = (radio_packet_t *)pkt;
    DEBUG("transceiver: Send packet to %" PRIu16 "\n", p->dst);
    for (size_t i = 0; i < p->length; i++) {
        DEBUG("%02x ", p->data[i]);
    }

    DEBUG("\n");
#endif

#ifdef MODULE_CC110X_LEGACY
    cc110x_packet_t cc110x_pkt;
#endif
#ifdef MODULE_MC1322X
    maca_packet_t *maca_pkt = maca_get_free_packet();
#endif

#ifdef MODULE_CC2420
    cc2420_packet_t cc2420_pkt;
#endif

#ifdef MODULE_AT86RF231
    at86rf231_packet_t at86rf231_pkt;
#endif

    switch (t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_LEGACY
            cc110x_pkt.length = p->length + CC1100_HEADER_LENGTH;
            cc110x_pkt.address = p->dst;
            cc110x_pkt.flags = 0;
            memcpy(cc110x_pkt.data, p->data, p->length);
            res = cc110x_send(&cc110x_pkt);
#elif MODULE_CC110X_LEGACY_CSMA
            memcpy(cc1100_pkt, p->data, p->length);

            res = cc1100_send_csmaca(p->dst, 4, 0, (char *) cc1100_pkt, p->length);
            DEBUG("transceiver: snd_ret (%u) = %i\n", p->length, res);
#else
            puts("Unknown transceiver");
#endif
            break;
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            memcpy(&cc2420_pkt.frame, &p->frame, sizeof(ieee802154_frame_t));
            cc2420_pkt.length = p->frame.payload_len + IEEE_802154_FCS_LEN;
            res = cc2420_send(&cc2420_pkt);
            break;
#endif
#ifdef MODULE_MC1322X

        case TRANSCEIVER_MC1322X:
            maca_pkt->length = p->length;
            memcpy(maca_pkt->data, &p->frame, p->length);
            maca_set_tx_packet(maca_pkt);
            res = 1;
            break;
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            res = nativenet_send(p);
            break;
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            memcpy(&at86rf231_pkt.frame, &p->frame, sizeof(ieee802154_frame_t));
            at86rf231_pkt.length = p->frame.payload_len + IEEE_802154_FCS_LEN;
            res = at86rf231_send(&at86rf231_pkt);
            break;
#endif

        default:
            puts("Unknown transceiver");
            break;
    }

    return res;
}

/*------------------------------------------------------------------------------------*/
/*
 * @brief Sets the radio channel for any transceiver device
 *
 * @param t         The transceiver device
 * @param channel   The channel to be set
 *
 * @return The radio channel AFTER calling the set command, -1 on error
 */
static int32_t set_channel(transceiver_type_t t, void *channel)
{
    /* cppcheck: c is read depending on enabled modules */
    /* cppcheck-suppress unreadVariable */
    uint8_t c = *((uint8_t *)channel);

    switch (t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_LEGACY
            return cc110x_set_channel(c);
#elif MODULE_CC110X_LEGACY_CSMA
            return cc1100_set_channel(c);
#else
            return -1;
#endif
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            return cc2420_set_channel(c);
#endif
#ifdef MODULE_MC1322X

        case TRANSCEIVER_MC1322X:
            maca_set_channel(c);
            return c; ///< TODO: should be changed!implement get channel
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            return nativenet_set_channel(c);
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            return at86rf231_set_channel(c);
#endif

        default:
            return -1;
    }
}

/*
 * @brief Get the radio channel of any transceiver device
 *
 * @param t     The transceiver device
 *
 * @return The current radio channel of the transceiver, -1 on error
 */
static int32_t get_channel(transceiver_type_t t)
{
    switch (t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_LEGACY
            return cc110x_get_channel();
#elif MODULE_CC110X_LEGACY_CSMA
            return cc1100_get_channel();
#else
            return -1;
#endif
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            return cc2420_get_channel();
#endif
#ifdef MODULE_MC1322X

        case TRANSCEIVER_MC1322X:
            ///< TODO:implement return maca_get_channel();
            return -1;
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            return nativenet_get_channel();
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            return at86rf231_get_channel();
#endif

        default:
            return -1;
    }
}

/*------------------------------------------------------------------------------------*/
/*
 * @brief Sets the PAN for any transceiver device
 *
 * @param t         The transceiver device
 * @param pan       The PAN to be set
 *
 * @return The PAN AFTER calling the set command, -1 on error
 */
static int32_t set_pan(transceiver_type_t t, void *pan)
{
    uint16_t c = *((uint16_t *) pan);

    switch (t) {
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            return cc2420_set_pan(c);
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            return nativenet_set_pan(c);
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            return at86rf231_set_pan(c);
#endif
#ifdef MODULE_MC1322X

        case TRANSCEIVER_MC1322X:
            return maca_set_pan(c);
#endif

        default:
            /* get rid of compiler warning about unused variable */
            (void) c;
            return -1;
    }
}

/*
 * @brief Get the pan of any transceiver device
 *
 * @param t     The transceiver device
 *
 * @return The current pan of the transceiver, -1 on error
 */
static int32_t get_pan(transceiver_type_t t)
{
    switch (t) {
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            return cc2420_get_pan();
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            return nativenet_get_pan();
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            return at86rf231_get_pan();
#endif
#ifdef MODULE_MC1322X

        case TRANSCEIVER_MC1322X:
            return maca_get_pan();
#endif

        default:
            return -1;
    }
}
/*------------------------------------------------------------------------------------*/
/*
 * @brief Get the current address of transceiver device
 *
 * @param t     The transceiver device
 *
 * @return  The configured address of the device
 *
 * TODO: define error behavior
 */
static radio_address_t get_address(transceiver_type_t t)
{
    switch (t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_LEGACY
            return cc110x_get_address();
#elif MODULE_CC110X_LEGACY_CSMA
            return cc1100_get_address();
#else
            return 0; /* XXX see TODO above */
#endif
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            return cc2420_get_address();
#endif
#ifdef MODULE_MC1322X

        case TRANSCEIVER_MC1322X:
            return maca_get_address();
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            return nativenet_get_address();
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            return at86rf231_get_address();
#endif

        default:
            return 0; /* XXX see TODO above */
    }
}

/*
 * @brief Set the address of the transceiver device
 *
 * @param t         The transceiver device
 * @param address   Generic pointer to the address to set
 *
 * @return  The new radio address of the device
 *
 * TODO: define error behavior
 */
static radio_address_t set_address(transceiver_type_t t, void *address)
{
    /* cppcheck: addr is read depending on enabled modules */
    /* cppcheck-suppress unreadVariable */
    radio_address_t addr = *((radio_address_t *)address);

    switch (t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_LEGACY
            return cc110x_set_address(addr);
#elif MODULE_CC110X_LEGACY_CSMA
            return cc1100_set_address(addr);
#else
            return 0; /* XXX see TODO above */
#endif
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            return cc2420_set_address(addr);
#endif
#ifdef MODULE_MC1322X

        case TRANSCEIVER_MC1322X:
            return maca_set_address(addr);
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            return nativenet_set_address(addr);
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            return at86rf231_set_address(addr);
#endif

        default:
            return 0; /* XXX see TODO above */
    }
}

/*
 * @brief Get the current long address of transceiver device
 *
 * @param t     The transceiver device
 *
 * @return  The configured long address of the device, 0 on error
 */
static transceiver_eui64_t get_long_addr(transceiver_type_t t)
{
    switch (t) {
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            return cc2420_get_address_long();
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            return at86rf231_get_address_long();
#endif

        default:
            return 0;
    }
}

/*
 * @brief Set the long address of the transceiver device
 *
 * @param t         The transceiver device
 * @param address   Generic pointer to the long address to set
 *
 * @return  The new long radio address of the device, 0 on error
 */
static transceiver_eui64_t set_long_addr(transceiver_type_t t, void *address)
{
    uint64_t addr = *((uint64_t *)address);

    switch (t) {
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            return cc2420_set_address_long(addr);
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            return at86rf231_set_address_long(addr);
#endif

        default:
            (void) addr;
            return 0;
    }
}

/*
 * @brief Set the transceiver device into monitor mode (disabling address check)
 *
 * @param t         The transceiver device
 * @param mode      1 for enabling monitor mode, 0 for enabling address check
 */
static void set_monitor(transceiver_type_t t, void *mode)
{
    (void) mode;

    switch (t) {
#ifdef MODULE_CC110X_LEGACY

        case TRANSCEIVER_CC1100:
            cc110x_set_monitor(*((uint8_t *)mode));
            break;
#endif
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:
            cc2420_set_monitor(*((uint8_t *) mode));
            break;
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            nativenet_set_monitor(*((uint8_t *) mode));
            break;
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            at86rf231_set_monitor(*((uint8_t *) mode));
#endif

        default:
            break;
    }
}

#ifdef MODULE_CC110X_LEGACY_CSMA
void cc1100_packet_monitor(void *payload, int payload_size, protocol_t protocol, packet_info_t *packet_info)
{
    (void) protocol;

    cc1100_payload = payload;
    cc1100_payload_size = payload_size - 3;
    cc1100_packet_info = packet_info;
    receive_packet(TRANSCEIVER_CC1100, 0);
}
#endif

/*------------------------------------------------------------------------------------*/
static void powerdown(transceiver_type_t t)
{
    switch (t) {
#ifdef MODULE_CC110X_LEGACY

        case TRANSCEIVER_CC1100:
            cc110x_switch_to_pwd();
            break;
#endif
#ifdef MODULE_MC1322X

        case TRANSCEIVER_MC1322X:
            maca_off();
            break;
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            nativenet_powerdown();
            break;
#endif

        default:
            break;
    }
}

/*------------------------------------------------------------------------------------*/
static void switch_to_rx(transceiver_type_t t)
{
    switch (t) {
#ifdef MODULE_CC110X_LEGACY

        case TRANSCEIVER_CC1100:
            cc110x_switch_to_rx();
            break;
#endif
#ifdef MODULE_CC2420

        case TRANSCEIVER_CC2420:

            cc2420_switch_to_rx();
            break;
#endif
#ifdef MODULE_NATIVENET

        case TRANSCEIVER_NATIVE:
            nativenet_switch_to_rx();
            break;
#endif
#ifdef MODULE_AT86RF231

        case TRANSCEIVER_AT86RF231:
            at86rf231_switch_to_rx();
#endif

        default:
            break;
    }
}

#ifdef DBG_IGNORE
static int16_t ignore_add(transceiver_type_t transceiver, void *address)
{
    (void) transceiver;
    radio_address_t addr = *((radio_address_t *)address);

    for (size_t i = 0; i < TRANSCEIVER_MAX_IGNORED_ADDR; i++) {
        if (transceiver_ignored_addr[i] == 0) {
            transceiver_ignored_addr[i] = addr;
            DEBUG("addr %u will be ignored (%u)\n", addr, i);
            return i;
        }
    }

    return -1;
}
#endif
