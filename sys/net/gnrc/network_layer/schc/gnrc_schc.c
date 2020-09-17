/*
 * Copyright (C) 2020 Bart Moons <bamoons.moons@ugent.be>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include "kernel_types.h"
#include "net/gnrc.h"
#include "thread.h"
#include "utlist.h"

#include "net/gnrc/schc.h"
#include "net/gnrc/netif.h"

#include "schc.h"
#include "compressor.h"
#include "fragmenter.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define GNRC_SCHC_FRAG_CB_MSG	(0x02FA)
#define MAX_PACKET_LENGTH		128

static kernel_pid_t _pid = KERNEL_PID_UNDEF;

#if ENABLE_DEBUG
static char _stack[GNRC_SCHC_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_SCHC_STACK_SIZE];
#endif

schc_bitarray_t schc_bitbuff;
schc_fragmentation_t schc_conn;
gnrc_netif_t *netif;
uint8_t message_buffer[MAX_PACKET_LENGTH] = { 0 };
static xtimer_t _gc_timer;

/* handles GNRC_NETAPI_MSG_TYPE_RCV commands */
static void _receive(gnrc_pktsnip_t *pkt);
/* handles GNRC_NETAPI_MSG_TYPE_SND commands */
static void _send(gnrc_pktsnip_t *pkt);
/* Main event loop for SCHC */
static void *_event_loop(void *args);

kernel_pid_t gnrc_schc_init(void)
{
    if (_pid > KERNEL_PID_UNDEF) {
        return _pid;
    }

    _pid = thread_create(_stack, sizeof(_stack), GNRC_SCHC_PRIO,
                         THREAD_CREATE_STACKTEST, _event_loop, NULL, "schc");

    return _pid;
}

kernel_pid_t gnrc_schc_get_pid(void)
{
    return _pid;
}

/*
 * Callback to handle the end of a fragmentation sequence
 */
static void end_tx(void) {
	DEBUG("end_tx() callback \n");
	xtimer_remove(&_gc_timer);
}

/*
 * Callback to handle the end of a fragmentation sequence
 * may be used to forward packet to IP network
 */
static void end_rx(schc_fragmentation_t *conn) {
	DEBUG("schc: end_rx callback for device %d\n", (int) conn->device_id);

	schc_bitarray_t rcv_buf;
	conn->bit_arr = &rcv_buf;

	// what is the length of the complete packet?
	uint16_t compressed_packetlen = get_mbuf_len(conn);
	uint8_t mbuf_buffer[MAX_PACKET_LENGTH] = { 0 };
	mbuf_copy(conn, mbuf_buffer); // copy to a local buffer
	rcv_buf.ptr = mbuf_buffer;

	uint16_t new_packet_len = schc_decompress(&rcv_buf, message_buffer,
			conn->device_id, compressed_packetlen, UP);

	gnrc_pktsnip_t *ip6 = gnrc_pktbuf_add(NULL, message_buffer, new_packet_len, GNRC_NETTYPE_IPV6);

	schc_reset(conn);
	xtimer_remove(&_gc_timer);

    if (!gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6,
                                      GNRC_NETREG_DEMUX_CTX_ALL, ip6)) {
        DEBUG("schc: No receivers for this packet found\n");
        gnrc_pktbuf_release(ip6);
    }
}

static void set_schc_timer(int8_t (*callback)(schc_fragmentation_t *conn), uint32_t device_id,
		uint32_t delay, void *arg) {
	DEBUG("schc: set timer (%d msec) for device %d \n", (int ) delay,
			(int ) device_id);

	xtimer_callback_t cb = (xtimer_callback_t) (callback);
	_gc_timer.callback = cb;
	_gc_timer.arg = arg;

	xtimer_set64(&_gc_timer, (delay * 1000));
}

static uint8_t send_callback(uint8_t* data, uint16_t length, uint32_t device_id) {
	DEBUG("schc: transmitting packet with length %d for device %d \n", (int) length, (int) device_id);

	gnrc_pktsnip_t* schc = gnrc_pktbuf_add(NULL, data, length, GNRC_NETTYPE_SCHC);

	if(netif == NULL) {
        netif = gnrc_netif_iter(NULL);
	}

	/* ethernet broacast */
	uint8_t dst[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	gnrc_pktsnip_t *netif_hdr;
	netif_hdr = gnrc_netif_hdr_build(netif->l2addr, 6, dst, 6);
	gnrc_netif_hdr_set_netif(netif_hdr->data, netif);

	LL_PREPEND(schc, netif_hdr);

    gnrc_netif_hdr_t *hdr = schc->data;

	if (gnrc_netif_send(gnrc_netif_get_by_pid(hdr->if_pid), schc) < 1) {
		DEBUG("schc: unable to send %p over interface %u\n", (void* )schc,
				hdr->if_pid);
		gnrc_pktbuf_release(schc);
	}

    DEBUG("schc: sent %d bytes.\n", length);

	return 1;
}

void remove_timer_entry(uint32_t device_id) {
	DEBUG("remove_timer_entry(): remove timer entry for device with id %d \n", (int) device_id);
	xtimer_remove(&_gc_timer);
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    pkt->type = GNRC_NETTYPE_SCHC;
    DEBUG("schc: received packet with length %d \n", pkt->size);

	if ((pkt == NULL) || (pkt->size < sizeof(gnrc_netif_hdr_t))) {
        DEBUG("schc: Received packet has no netif header\n");
        netif = gnrc_netif_iter(NULL);
    } else {
        netif = gnrc_netif_hdr_get_netif(pkt->data);
    }

	uint32_t device_id = 0x01;
	schc_fragmentation_t *conn = schc_input((uint8_t*) pkt->data, pkt->size,
			&schc_conn, device_id); // get active connection and set the correct rule for this connection

	if (conn != &schc_conn) { // if returned value is receiving_conn: acknowledgement is received, which is handled by the library
		conn->post_timer_task = &set_schc_timer;
		conn->dc = 15000; // retransmission timer: used for timeouts

		if (conn->schc_rule->mode == NOT_FRAGMENTED) { // packet was not fragmented
			end_rx(conn);
		} else {
			int ret = schc_reassemble(conn);
			if(ret && conn->schc_rule->mode == NO_ACK){ // use the connection to reassemble
				end_rx(conn); // final packet arrived
			}
		}
	}

	/* release original headers and payload */
    gnrc_pktbuf_release(pkt);
}

static void _send(gnrc_pktsnip_t *pkt)
{
	if ((pkt == NULL) || (pkt->size < sizeof(gnrc_netif_hdr_t))) {
        DEBUG("schc: Sending packet has no netif header\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    netif = gnrc_netif_hdr_get_netif(pkt->data);

    if (netif == NULL) {
        DEBUG("schc: Can not get SCHC specific interface information.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

#ifdef MODULE_GNRC_IPV6
    if ((pkt->next == NULL) || (pkt->next->type != GNRC_NETTYPE_IPV6)) {
        DEBUG("schc: Sending packet has no IPv6 header\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#endif

	/* merge layers in ip6 packet */
	gnrc_pktsnip_t *ip6 = pkt->next;
	gnrc_pktbuf_merge(ip6);

	/* compress packet */
	uint8_t device_id = 0x01;
	schc_bitbuff.ptr = (uint8_t*) (message_buffer); // deep copy

	struct schc_rule_t* schc_rule;
	schc_rule = schc_compress(ip6->data, ip6->size, &schc_bitbuff, device_id, UP);

	schc_conn.mtu = 242; // network driver MTU todo get from netif
	schc_conn.dc = 5000; // 5 seconds duty cycle
	schc_conn.device_id = device_id; // the device id of the connection
	schc_conn.bit_arr = &schc_bitbuff;
	schc_conn.schc_rule = schc_rule;
	schc_conn.RULE_SIZE = RULE_SIZE_BITS;
	schc_conn.MODE = ACK_ALWAYS;

	/* todo integrate empty rules */
	if (schc_rule == NULL) {
        DEBUG("schc: Can not retrieve rule.\n");
		return;
	}

	/* release original headers and payload */
    gnrc_pktbuf_release(pkt);

	schc_fragment(&schc_conn); // start fragmentation loop
}


static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_SCHC_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            sched_active_pid);

    (void)args;
    msg_init_queue(msg_q, GNRC_SCHC_MSG_QUEUE_SIZE);

    /* register interest in all SCHC packets */
    gnrc_netreg_register(GNRC_NETTYPE_SCHC, &me_reg);

    /* preinitialize ACK */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

	schc_fragmenter_init(&schc_conn, &send_callback, &end_rx,
			&remove_timer_entry);

	schc_conn.send = &send_callback;
	schc_conn.end_tx = &end_tx;
	schc_conn.end_rx = &end_rx;
	schc_conn.remove_timer_entry = &remove_timer_entry;
	schc_conn.post_timer_task = &set_schc_timer;

	/* start event loop */
	while (1) {
		DEBUG("schc: waiting for incoming message.\n");
		msg_receive(&msg);

		switch (msg.type) {
		case GNRC_NETAPI_MSG_TYPE_RCV:
			DEBUG("schc: GNRC_NETDEV_MSG_TYPE_RCV received\n");
			_receive(msg.content.ptr);
			break;

		case GNRC_NETAPI_MSG_TYPE_SND:
			DEBUG("schc: GNRC_NETDEV_MSG_TYPE_SND received\n");
			_send(msg.content.ptr);
			break;

		case GNRC_NETAPI_MSG_TYPE_GET:
		case GNRC_NETAPI_MSG_TYPE_SET:
			DEBUG("schc: reply to unsupported get/set\n");
			reply.content.value = -ENOTSUP;
			msg_reply(&msg, &reply);
			break;
        case GNRC_SCHC_FRAG_CB_MSG:
            DEBUG("schc: fragmentation event received\n");
            break;
		default:
			DEBUG("schc: operation not supported\n");
			break;
		}
	}

    return NULL;
}
