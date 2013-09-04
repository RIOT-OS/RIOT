/**
 * Generic transceiver module as an interface to NIC driver.
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
#ifdef MODULE_CC110X
#include "cc1100-interface.h"
#endif

#ifdef MODULE_CC110X_NG
#include "cc110x_ng.h"
#endif

#ifdef MODULE_CC2420
#include "cc2420.h"
#endif

#ifdef MODULE_MC1322X
#include "mc1322x.h"
#include "maca.h"
#include "maca_packet.h"
#endif

#ifdef MODULE_NATIVENET
#include "nativenet.h"
#include "nativenet_internal.h"
#endif

#ifdef MODULE_AT86RF231
#include <at86rf231.h>
#endif

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#undef TRANSCEIVER_STACK_SIZE
#define TRANSCEIVER_STACK_SIZE      (KERNEL_CONF_STACKSIZE_PRINTF)
#endif
#include "debug.h"

/*------------------------------------------------------------------------------------*/
/* used transceiver types */
transceiver_type_t transceivers = TRANSCEIVER_NONE;

/* registered upper layer threads */
registered_t reg[TRANSCEIVER_MAX_REGISTERED];

/* packet buffers */
radio_packet_t transceiver_buffer[TRANSCEIVER_BUFFER_SIZE];
uint8_t data_buffer[TRANSCEIVER_BUFFER_SIZE * PAYLOAD_SIZE];

/* message buffer */
msg_t msg_buffer[TRANSCEIVER_MSG_BUFFER_SIZE];

uint32_t response; ///< response bytes for messages to upper layer threads

int transceiver_pid = -1; ///< the transceiver thread's pid

static volatile uint8_t rx_buffer_pos = 0;
static volatile uint8_t transceiver_buffer_pos = 0;

#ifdef MODULE_CC110X
    void *cc1100_payload;
    int cc1100_payload_size;
    packet_info_t *cc1100_packet_info;
    uint8_t cc1100_pkt[CC1100_MAX_DATA_LENGTH];
#endif


/* transceiver stack */
char transceiver_stack[TRANSCEIVER_STACK_SIZE];

/*------------------------------------------------------------------------------------*/
/* function prototypes */
static void run(void);
static void receive_packet(uint16_t type, uint8_t pos);
#ifdef MODULE_CC110X_NG
static void receive_cc110x_packet(radio_packet_t *trans_p);
#endif
#ifdef MODULE_CC110X
void cc1100_packet_monitor(void *payload, int payload_size, protocol_t protocol, packet_info_t *packet_info);
void receive_cc1100_packet(radio_packet_t *trans_p);
#endif
#ifdef MODULE_CC2420
static void receive_cc2420_packet(radio_packet_t *trans_p);
#endif
#ifdef MODULE_NATIVENET
static void receive_nativenet_packet(radio_packet_t *trans_p);
#endif
#ifdef MODULE_AT86RF231
void receive_at86rf231_packet(radio_packet_t *trans_p);
#endif
static uint8_t send_packet(transceiver_type_t t, void *pkt);
static int16_t get_channel(transceiver_type_t t);
static int16_t set_channel(transceiver_type_t t, void *channel);
static int16_t get_address(transceiver_type_t t);
static int16_t set_address(transceiver_type_t t, void *address);
static uint16_t get_pan(transceiver_type_t t);
static uint16_t set_pan(transceiver_type_t t, void *pan);

static void set_monitor(transceiver_type_t t, void *mode);
static void powerdown(transceiver_type_t t);
static void switch_to_rx(transceiver_type_t t);

#ifdef DBG_IGNORE
static void ignore_add(transceiver_type_t t, void *address);
#endif

/*------------------------------------------------------------------------------------*/
/* Transceiver init */
void transceiver_init(transceiver_type_t t)
{
    uint8_t i;

    if (transceiver_pid >= 0) {
        /* do not re-initialize an already running transceiver */
        return;
    }

    /* Initializing transceiver buffer and data buffer */
    memset(transceiver_buffer, 0, TRANSCEIVER_BUFFER_SIZE * sizeof(radio_packet_t));
    memset(data_buffer, 0, TRANSCEIVER_BUFFER_SIZE * PAYLOAD_SIZE);

    for (i = 0; i < TRANSCEIVER_MAX_REGISTERED; i++) {
        reg[i].transceivers = TRANSCEIVER_NONE;
        reg[i].pid          = 0;
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
int transceiver_start(void)
{
    transceiver_pid = thread_create(transceiver_stack, TRANSCEIVER_STACK_SIZE, PRIORITY_MAIN - 3, CREATE_STACKTEST, run, "Transceiver");

    if (transceiver_pid < 0) {
        puts("Error creating transceiver thread");
    }
#ifdef MODULE_CC110X_NG
    else if (transceivers & TRANSCEIVER_CC1100) {
        DEBUG("transceiver: Transceiver started for CC1100\n");
        cc110x_init(transceiver_pid);
    }
#endif
#ifdef MODULE_CC110X
    else if (transceivers & TRANSCEIVER_CC1100) {
        DEBUG("transceiver: Transceiver started for CC1100\n");
        cc1100_init();
        cc1100_set_packet_monitor(cc1100_packet_monitor);
    }
#endif
#ifdef MODULE_CC2420
    else if(transceivers & TRANSCEIVER_CC2420) {
        DEBUG("transceiver: Transceiver started for CC2420\n");
        cc2420_init(transceiver_pid);
    }
#endif
#ifdef MODULE_AT86RF231
    else if(transceivers & TRANSCEIVER_AT86RF231) {
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
uint8_t transceiver_register(transceiver_type_t t, int pid)
{
    uint8_t i;

    for (i = 0; ((reg[i].pid != pid) &&
                (i < TRANSCEIVER_MAX_REGISTERED) &&
                (reg[i].transceivers != TRANSCEIVER_NONE)); i++);

    if (i >= TRANSCEIVER_MAX_REGISTERED) {
        return ENOMEM;
    }
    else {
        reg[i].transceivers |= t;
        reg[i].pid         = pid;
        DEBUG("transceiver: Thread %i registered for %i\n", reg[i].pid, reg[i].transceivers);
        return 1;
    }
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/*
 * @brief The main thread run, receiving and processing messages in an infinite
 * loop
 */
void run(void)
{
    msg_t m;
    transceiver_command_t *cmd;

    msg_init_queue(msg_buffer, TRANSCEIVER_MSG_BUFFER_SIZE);

    while (1) {
        DEBUG("transceiver: Waiting for next message\n");
        msg_receive(&m);
        /* only makes sense for messages for upper layers */
        cmd = (transceiver_command_t *) m.content.ptr;
        DEBUG("transceiver: Transceiver: Message received\n");

        switch(m.type) {
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
                *((int16_t *) cmd->data) = get_channel(cmd->transceivers);
                msg_reply(&m, &m);
                break;

            case SET_CHANNEL:
                *((int16_t *) cmd->data) = set_channel(cmd->transceivers, cmd->data);
                msg_reply(&m, &m);
                break;

            case GET_ADDRESS:
                *((int16_t *) cmd->data) = get_address(cmd->transceivers);
                msg_reply(&m, &m);
                break;

            case SET_ADDRESS:
                *((int16_t *) cmd->data) = set_address(cmd->transceivers, cmd->data);
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
                *((int16_t*) cmd->data) = get_pan(cmd->transceivers);
                msg_reply(&m, &m);
                break;
            case SET_PAN:
                *((int16_t*) cmd->data) = set_pan(cmd->transceivers, cmd->data);
                msg_reply(&m, &m);
                break;
#ifdef DBG_IGNORE

            case DBG_IGN:
                printf("Transceiver PID: %i (%p), rx_buffer_next: %u\n", transceiver_pid, &transceiver_pid, rx_buffer_next);
                ignore_add(cmd->transceivers, cmd->data);
                break;
#endif

            default:
                DEBUG("transceiver: Unknown message received\n");
                break;
        }
    }
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
    uint8_t i = 0;
    transceiver_type_t t;
    rx_buffer_pos = pos;
    msg_t m;

    DEBUG("Packet received\n");

    switch(type) {
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
    }
    /* copy packet and handle it */
    else {
        radio_packet_t *trans_p = &(transceiver_buffer[transceiver_buffer_pos]);
        m.type = PKT_PENDING;

        /* pass a null pointer if a packet from a undefined transceiver is
         * received */
        if (type == RCV_PKT_CC1100) {
#ifdef MODULE_CC110X_NG
            receive_cc110x_packet(trans_p); 
#elif MODULE_CC110X
            receive_cc1100_packet(trans_p);
#else
            trans_p = NULL;
#endif
        } 
        else if (type == RCV_PKT_MC1322X) {
#ifdef MODULE_MC1322X
            receive_mc1322x_packet(trans_p);
#else
            trans_p = NULL;
#endif
        }
        else if (type == RCV_PKT_CC2420) {
#ifdef MODULE_CC2420
            receive_cc2420_packet(trans_p);
#else
            trans_p = NULL;
#endif
        }
        else if (type == RCV_PKT_AT86RF231) {
#ifdef MODULE_AT86RF231
            receive_at86rf231_packet(trans_p);
#else
            trans_p = NULL;
        }
#endif
        else if (type == RCV_PKT_NATIVE) {
#ifdef MODULE_NATIVENET
            receive_nativenet_packet(trans_p);
#else
            trans_p = NULL;
#endif
        }
        else {
            puts("Invalid transceiver type");
            return;
        }
    }

    /* finally notify waiting upper layers
     * this is done non-blocking, so packets can get lost */
    i = 0;

    while (reg[i].transceivers != TRANSCEIVER_NONE) {
        if (reg[i].transceivers & t) {
            m.content.ptr = (char *) & (transceiver_buffer[transceiver_buffer_pos]);
            DEBUG("transceiver: Notify thread %i\n", reg[i].pid);

            if (msg_send(&m, reg[i].pid, false) && (m.type != ENOBUFFER)) {
                transceiver_buffer[transceiver_buffer_pos].processing++;
            }
        }

        i++;
    }
}

#ifdef MODULE_CC110X_NG
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
    memcpy((void *)&(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), p.data, CC1100_MAX_DATA_LENGTH);
    eINT();

    trans_p->data = (uint8_t *)&(data_buffer[transceiver_buffer_pos * CC1100_MAX_DATA_LENGTH]);
    DEBUG("transceiver: Packet %p (%p) was from %hu to %hu, size: %u\n", trans_p, trans_p->data, trans_p->src, trans_p->dst, trans_p->length);
}
#endif

#ifdef MODULE_CC110X
void receive_cc1100_packet(radio_packet_t *trans_p)
{
    dINT();
    trans_p->src = cc1100_packet_info->source;
    trans_p->dst = cc1100_packet_info->destination;
    trans_p->rssi = cc1100_packet_info->rssi;
    trans_p->lqi = cc1100_packet_info->lqi;
    trans_p->length = cc1100_payload_size;
    memcpy((void *)&(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), cc1100_payload, CC1100_MAX_DATA_LENGTH);
    eINT();

    trans_p->data = (uint8_t *)&(data_buffer[transceiver_buffer_pos * CC1100_MAX_DATA_LENGTH]);
}
#endif


#ifdef MODULE_CC2420
void receive_cc2420_packet(radio_packet_t *trans_p) {
    DEBUG("transceiver: Handling CC2420 packet\n");
    dINT();
    cc2420_packet_t p = cc2420_rx_buffer[rx_buffer_pos];
    trans_p->src = (uint16_t)((p.frame.src_addr[1] << 8) | p.frame.src_addr[0]);
    trans_p->dst = (uint16_t)((p.frame.dest_addr[1] << 8)| p.frame.dest_addr[0]);
    trans_p->rssi = p.rssi;
    trans_p->lqi = p.lqi;
    trans_p->length = p.frame.payload_len;
    memcpy((void*) &(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), p.frame.payload, CC2420_MAX_DATA_LENGTH);
    eINT();

    DEBUG("transceiver: Packet %p was from %u to %u, size: %u\n", trans_p, trans_p->src, trans_p->dst, trans_p->length);
    trans_p->data = (uint8_t*) &(data_buffer[transceiver_buffer_pos * CC2420_MAX_DATA_LENGTH]);
    DEBUG("transceiver: Content: %s\n", trans_p->data);
}
#endif

#ifdef MODULE_MC1322X
void receive_mc1322x_packet(radio_packet_t *trans_p) {
    maca_packet_t* maca_pkt;
    dINT();
    maca_pkt = maca_get_rx_packet ();
    trans_p->lqi = maca_pkt->lqi;
    trans_p->length = maca_pkt->length;
    memcpy((void*) &(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), maca_pkt->data, MACA_MAX_PAYLOAD_SIZE);
    maca_free_packet( maca_pkt );
    eINT();
    
    trans_p->data = (uint8_t*) &(data_buffer[transceiver_buffer_pos * MACA_MAX_PAYLOAD_SIZE]);
}
#endif

 
#ifdef MODULE_NATIVENET
void receive_nativenet_packet(radio_packet_t *trans_p) {
    unsigned state;
    radio_packet_t *p = &_nativenet_rx_buffer[rx_buffer_pos].packet;

    /* disable interrupts while copying packet */
    state = disableIRQ();

    DEBUG("Handling nativenet packet\n");

    memcpy(trans_p, p, sizeof(radio_packet_t));
    memcpy(&(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), p->data, p->length);
    trans_p->data =  (uint8_t*) &(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]);

    DEBUG("Packet %p was from %"PRIu16" to %"PRIu16", size: %"PRIu8"\n", trans_p, trans_p->src, trans_p->dst, trans_p->length);

    /* reset interrupts */
    restoreIRQ(state);
}
#endif

#ifdef MODULE_AT86RF231
void receive_at86rf231_packet(radio_packet_t *trans_p) {
    DEBUG("Handling AT86RF231 packet\n");
    dINT();
    at86rf231_packet_t p = at86rf231_rx_buffer[rx_buffer_pos];
    trans_p->src = (uint16_t)((p.frame.src_addr[1] << 8) | p.frame.src_addr[0]);
    trans_p->dst = (uint16_t)((p.frame.dest_addr[1] << 8)| p.frame.dest_addr[0]);
    trans_p->rssi = p.rssi;
    trans_p->lqi = p.lqi;
    trans_p->length = p.frame.payload_len;
    memcpy((void*) &(data_buffer[transceiver_buffer_pos * PAYLOAD_SIZE]), p.frame.payload, AT86RF231_MAX_DATA_LENGTH);
    eINT();

    DEBUG("Packet %p was from %u to %u, size: %u\n", trans_p, trans_p->src, trans_p->dst, trans_p->length);
    trans_p->data = (uint8_t*) &(data_buffer[transceiver_buffer_pos * AT86RF231_MAX_DATA_LENGTH]);
    DEBUG("Content: %s\n", trans_p->data);
}
#endif
/*------------------------------------------------------------------------------------*/
/*
 * @brief Sends a radio packet to the receiver
 *
 * @param t     The transceiver device
 * @param pkt   Generic pointer to the packet
 *
 * @return 1 on success, 0 otherwise
 */
static uint8_t send_packet(transceiver_type_t t, void *pkt)
{
    uint8_t res = 0;
#ifdef MODULE_CC110X
    int snd_ret;
#endif
    radio_packet_t p = *((radio_packet_t *)pkt);

#ifdef MODULE_CC110X_NG
    cc110x_packet_t cc110x_pkt;
#endif
#ifdef MODULE_MC1322X
    maca_packet_t* maca_pkt = maca_get_free_packet();
#endif

#ifdef MODULE_CC2420
    cc2420_packet_t cc2420_pkt;
#endif

#ifdef MODULE_AT86RF231
    at86rf231_packet_t at86rf231_pkt;
#endif

    switch (t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_NG
            DEBUG("transceiver: Send packet to %" PRIu16 "\n", p.dst);
            cc110x_pkt.length = p.length + CC1100_HEADER_LENGTH;
            cc110x_pkt.address = p.dst;
            cc110x_pkt.flags = 0;
            memcpy(cc110x_pkt.data, p.data, p.length);
            res = cc110x_send(&cc110x_pkt);
#elif MODULE_CC110X
            memcpy(cc1100_pkt, p.data, p.length);

            if ((snd_ret = cc1100_send_csmaca(p.dst, 4, 0, (char *) cc1100_pkt, p.length)) < 0) {
                DEBUG("transceiver: snd_ret (%u) = %i\n", p.length, snd_ret);
                res = 0;
            }
            else {
                res = 1;
            }
#else
            puts("Unknown transceiver");
#endif
            break;
#ifdef MODULE_CC2420
        case TRANSCEIVER_CC2420:
            cc2420_pkt.frame.payload_len = p.length;
            cc2420_pkt.frame.dest_addr[1] = (uint8_t)(p.dst >> 8);
            cc2420_pkt.frame.dest_addr[0] = (uint8_t)(p.dst & 0xFF);
            cc2420_pkt.frame.dest_pan_id = cc2420_get_pan();
            cc2420_pkt.frame.fcf.dest_addr_m = 2;
            cc2420_pkt.frame.fcf.src_addr_m = 2;
            cc2420_pkt.frame.fcf.ack_req = 0;
            cc2420_pkt.frame.fcf.sec_enb = 0;
            cc2420_pkt.frame.fcf.frame_type = 1;
            cc2420_pkt.frame.fcf.frame_pend = 0;
            cc2420_pkt.frame.payload = p.data;
            res = cc2420_send(&cc2420_pkt);
            break;
#endif
#ifdef MODULE_MC1322X
        case TRANSCEIVER_MC1322X:
            maca_pkt->length = p.length;
            memcpy(maca_pkt->data, p.data, p.length);
            maca_set_tx_packet( maca_pkt );
            res = 1;
            break;
#endif
#ifdef MODULE_NATIVENET
        case TRANSCEIVER_NATIVE:
            res = nativenet_send(&p);
            break;
#endif
#ifdef MODULE_AT86RF231
        case TRANSCEIVER_AT86RF231:
            at86rf231_pkt.frame.payload_len = p.length;
            at86rf231_pkt.frame.dest_addr[1] = (uint8_t)(p.dst >> 8);
            at86rf231_pkt.frame.dest_addr[0] = (uint8_t)(p.dst & 0xFF);
            at86rf231_pkt.frame.dest_pan_id = at86rf231_get_pan();
            at86rf231_pkt.frame.fcf.dest_addr_m = 2;
            at86rf231_pkt.frame.fcf.src_addr_m = 2;
            at86rf231_pkt.frame.fcf.ack_req = 0;
            at86rf231_pkt.frame.fcf.sec_enb = 0;
            at86rf231_pkt.frame.fcf.frame_type = 1;
            at86rf231_pkt.frame.fcf.frame_pend = 0;
            at86rf231_pkt.frame.payload = p.data;
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
static int16_t set_channel(transceiver_type_t t, void *channel)
{
    uint8_t c = *((uint8_t *)channel);

    switch(t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_NG
            return cc110x_set_channel(c);
#elif MODULE_CC110X
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
            return c; ///< TODO: should be changed! implement get channel
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
static int16_t get_channel(transceiver_type_t t)
{
    switch(t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_NG
            return cc110x_get_channel();
#elif MODULE_CC110X
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
 * @brief Sets the pan for the CC2420 transceiver device
 *
 * @param t         The transceiver device
 * @param channel   The channel to be set
 *
 * @return The pan AFTER calling the set command, -1 on error
 */
static uint16_t set_pan(transceiver_type_t t, void *pan) {
    uint16_t c = *((uint16_t*) pan);
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
        default:
            /* get rid of compiler warning about unused variable */
            (void) c;
            return -1;
    }
}

/*
 * @brief Get the pan for the cc2420 transceiver device
 *
 * @param t     The transceiver device
 *
 * @return The current pan of the transceiver, -1 on error
 */
static uint16_t get_pan(transceiver_type_t t) {
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
 * @return  The configured address of the device, -1 on error
 */
static int16_t get_address(transceiver_type_t t)
{
    switch(t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_NG
            return cc110x_get_address();
#elif MODULE_CC110X
            return cc1100_get_address();
#else
            return -1;
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
            return -1;
    }
}

/*
 * @brief Set the address of the transceiver device
 *
 * @param t         The transceiver device
 * @param address   Generic pointer to the address to set
 *
 * @return  The new radio address of the device
 */
static int16_t set_address(transceiver_type_t t, void *address)
{
    radio_address_t addr = *((radio_address_t *)address);

    switch(t) {
        case TRANSCEIVER_CC1100:
#ifdef MODULE_CC110X_NG
            return cc110x_set_address(addr);
#elif MODULE_CC110X
            return cc1100_set_address(addr);
#else
            return -1;
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
            return -1;
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
    switch(t) {
#ifdef MODULE_CC110X_NG
        case TRANSCEIVER_CC1100:
            cc110x_set_monitor(*((uint8_t *)mode));
            break;
#endif
#ifdef MODULE_CC2420
        case TRANSCEIVER_CC2420:
            cc2420_set_monitor(*((uint8_t*) mode));
            break;
#endif
#ifdef MODULE_NATIVENET
        case TRANSCEIVER_NATIVE:
            nativenet_set_monitor(*((uint8_t*) mode));
            break;
#endif
#ifdef MODULE_AT86RF231
        case TRANSCEIVER_AT86RF231:
            at86rf231_set_monitor(*((uint8_t*) mode));
#endif
        default:
            break;
    }
}

#ifdef MODULE_CC110X
void cc1100_packet_monitor(void *payload, int payload_size, protocol_t protocol, packet_info_t *packet_info)
{
    cc1100_payload = payload;
    cc1100_payload_size = payload_size - 3;
    cc1100_packet_info = packet_info;
    receive_packet(TRANSCEIVER_CC1100, 0);
}
#endif

/*------------------------------------------------------------------------------------*/
static void powerdown(transceiver_type_t t)
{
    switch(t) {
#ifdef MODULE_CC110X_NG
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
    switch(t) {
#ifdef MODULE_CC110X_NG
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
static void ignore_add(transceiver_type_t t, void *address)
{
    radio_address_t addr = *((radio_address_t *)address);

    switch(t) {
        case TRANSCEIVER_CC1100:
            cc110x_add_ignored(addr);
            break;

        default:
            break;
    }
}
#endif
