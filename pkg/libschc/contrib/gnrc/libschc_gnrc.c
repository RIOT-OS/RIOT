/*
 * Copyright (C) 2020 Bart Moons <bamoons.moons@ugent.be>
 * Copyright (C) 2022 Freie Universität Berlin
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

#include "od.h"
#include "net/gnrc.h"
#include "sched.h"
#include "thread.h"
#include "utlist.h"
#include "ztimer.h"

#include "net/ethernet/hdr.h"
#include "net/gnrc/netif.h"
#include "net/loramac.h"
#include "net/lorawan/hdr.h"

#include "schc.h"
#include "compressor.h"
#include "fragmenter.h"

#include "net/gnrc/schc.h"

#define ENABLE_DEBUG            0
#include "debug.h"

#define GNRC_SCHC_TIMER_CB_MSG  0x02FA
#define MAX_PACKET_LENGTH       128

static_assert(
    IS_ACTIVE(CONFIG_GNRC_NETIF_LORAWAN_NETIF_HDR),
    "SCHC assumes LoRaWAN port to be encoded in GNRC netif header "
    "(CONFIG_GNRC_NETIF_LORAWAN_NETIF_HDR = 1)"
);

typedef struct {
    void (*callback)(void *arg);
    void *arg;
} _gc_callback_t;

static uint32_t _device_id_pool = 0;
static kernel_pid_t _pid = KERNEL_PID_UNDEF;

#if ENABLE_DEBUG
static char _stack[GNRC_SCHC_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_SCHC_STACK_SIZE];
#endif

static void _gc_ztimer_cb(void *arg);

static _gc_callback_t _gc_callback;
static schc_bitarray_t schc_bitbuff;
static schc_fragmentation_t schc_conn;
static uint8_t message_buffer[MAX_PACKET_LENGTH] = { 0 };
static ztimer_t _gc_timer;

/* handles GNRC_NETAPI_MSG_TYPE_RCV commands */
static void _receive(gnrc_pktsnip_t *pkt);
/* handles GNRC_NETAPI_MSG_TYPE_SND commands */
static void _send(gnrc_pktsnip_t *pkt);
/* Main event loop for SCHC */
static void *_event_loop(void *args);

static uint32_t _get_device_id(gnrc_netif_t *netif)
{
    assert(netif != NULL);
    return netif->schc.dev_id;
}

static gnrc_netif_t *_get_netif(uint32_t device_id)
{
    gnrc_netif_t *ptr = NULL;

    assert(device_id > 0);
    while ((ptr = gnrc_netif_iter(NULL))) {
        /* TODO make usage with non-lorawan devices possible */
        if (ptr->schc.dev_id == device_id) {
            return ptr;
        }
    }
    return NULL;
}

kernel_pid_t libschc_gnrc_init(void)
{
    if (_pid > KERNEL_PID_UNDEF) {
        return _pid;
    }

    _pid = thread_create(_stack, sizeof(_stack), GNRC_SCHC_PRIO,
                         THREAD_CREATE_STACKTEST, _event_loop, NULL, "schc");

    return _pid;
}

void libschc_gnrc_netif_init(gnrc_netif_t *netif)
{
    if (!gnrc_netif_is_schc(netif)) {
        return;
    }
    if (!netif->schc.dev_id) {
        netif->schc.dev_id = ++_device_id_pool;
    }
    netif->flags |= GNRC_NETIF_FLAGS_SCHC;
    switch (netif->device_type) {
#ifdef MODULE_GNRC_SCHC_ETH
    case NETDEV_TYPE_ETHERNET:
        netif->schc.upstream_mode = NO_ACK;
        break;
#endif
#ifdef MODULE_GNRC_LORAWAN
    case NETDEV_TYPE_LORA:
        /* see https://datatracker.ietf.org/doc/html/rfc9011#section-5.6.2 */
        netif->schc.upstream_mode = ACK_ON_ERROR;
#ifdef MODULE_GNRC_NETIF_IPV6
        netif->ipv6.mtu = 1280;
#endif
        break;
#endif
    default:
        netif->schc.upstream_mode = NO_ACK;
#ifdef MODULE_GNRC_NETIF_IPV6
        netif->ipv6.mtu = 1280;
#endif
        break;
    }
}

/*
 * Callback to handle the end of a fragmentation sequence
 */
static void end_tx(schc_fragmentation_t *conn) {
    DEBUG("end_tx() callback for device %" PRIu32 "\n", conn->device_id);
    schc_reset(conn);
}

/*
 * Callback to handle the end of a fragmentation sequence
 * may be used to forward packet to IP network
 */
static void end_rx(schc_fragmentation_t *conn) {
    DEBUG("schc: end_rx callback for device %" PRIu32 "\n", conn->device_id);

    schc_bitarray_t rcv_buf;
    conn->bit_arr = &rcv_buf;

    /* what is the length of the complete packet? */
    uint16_t compressed_packetlen = get_mbuf_len(conn);
    gnrc_pktsnip_t *schc = gnrc_pktbuf_add(NULL, NULL, MAX_PACKET_LENGTH, GNRC_NETTYPE_SCHC);
    if (!schc) {
        DEBUG("schc: unable to allocated compressed packet arena\n");
        schc_reset(conn);
        return;
    }
    memset(schc->data, 0, schc->size);
    mbuf_copy(conn, schc->data);       /* copy to a local buffer */
    rcv_buf.ptr = schc->data;

    gnrc_pktsnip_t *ip6 = gnrc_pktbuf_add(NULL, NULL, MAX_PACKET_LENGTH, GNRC_NETTYPE_IPV6);
    if (!ip6) {
        DEBUG("schc: unable to allocated decompressed packet\n");
        gnrc_pktbuf_release(schc);
        schc_reset(conn);
        return;
    }
    memset(ip6->data, 0, ip6->size);
    uint16_t new_packet_len = schc_decompress(
        &rcv_buf, ip6->data, conn->device_id, compressed_packetlen, DOWN
    );
    /* we don't need the decompressed snip anymore in any case, so release it */
    gnrc_pktbuf_release(schc);
    if (gnrc_pktbuf_realloc_data(ip6, new_packet_len) < 0) {
        DEBUG("schc: unable to shrink decompressed packet to final size\n");
        gnrc_pktbuf_release(ip6);
        schc_reset(conn);
        return;
    }
    if (ENABLE_DEBUG && IS_USED(MODULE_OD)) {
        DEBUG("schc: decompressed packet\n");
        od_hex_dump(ip6->data, ip6->size, OD_WIDTH_DEFAULT);
    }

    schc_reset(conn);

    if (!gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6,
                                      GNRC_NETREG_DEMUX_CTX_ALL, ip6)) {
        DEBUG("schc: No receivers for this packet found\n");
        gnrc_pktbuf_release(ip6);
    }
}

static void _gc_ztimer_cb(void *arg)
{
    msg_t msg = { .type = GNRC_SCHC_TIMER_CB_MSG, .content = { .ptr = arg } };

    msg_try_send(&msg, _pid);
}

static void set_schc_timer(schc_fragmentation_t *conn, void (*callback)(void *conn),
                           uint32_t delay, void *arg) {
    DEBUG("schc: set timer (%" PRIu32 " msec) for device %" PRIu32 " \n", delay,
          conn->device_id);

    conn->timer_ctx = &_gc_timer;
    /* indirect callback via _gc_callback object to get out of ISR context */
    _gc_callback.callback = callback;
    _gc_callback.arg = arg;
    _gc_timer.callback = _gc_ztimer_cb;
    _gc_timer.arg = &_gc_callback;
    ztimer_set(ZTIMER_MSEC, &_gc_timer, delay);
}

static int _get_deviid(uint32_t device_id, uint8_t *iid)
{
    gnrc_netif_t *netif = _get_netif(device_id);
    assert(netif != NULL);

    if (netif_get_opt(&netif->netif, NETOPT_IPV6_IID, 0, iid, 8U) < 0) {
        return -1;
    }
    return 0;
}

static int _get_appiid(uint32_t device_id, uint8_t *iid)
{
    gnrc_netif_t *netif = _get_netif(device_id);
    assert(netif != NULL);

    gnrc_netif_acquire(netif);
    switch (netif->device_type) {
#if IS_USED(MODULE_GNRC_NETIF_LORAWAN)
    case NETDEV_TYPE_LORA:
        memcpy(iid, netif->lorawan.joineui, sizeof(netif->lorawan.joineui));
        break;
#endif
    default:
        (void)iid;
        return -ENOTSUP;
    }
    gnrc_netif_release(netif);
    return 0;
}

static uint32_t _get_duty_cycle(gnrc_netif_t *netif, size_t payload_len)
{
    switch (netif->device_type) {
#if IS_USED(MODULE_GNRC_SCHC_ETH)
    case NETDEV_TYPE_ETHERNET:
        return 1000;
#endif
#if IS_USED(MODULE_GNRC_NETIF_LORAWAN)
    case NETDEV_TYPE_LORA: {
        int res;
        uint8_t cr;

        res = netif->dev->driver->get(netif->dev, NETOPT_CODING_RATE, &cr, sizeof(cr));
        if (res < 0) {
            DEBUG("schc: Unable to get code rate from device\n");
            return 0;
        }
        /* pkt_len = payload_len + LoRaWAN header + MIC + FPort */
        return (lora_time_on_air(payload_len + sizeof(lorawan_hdr_t) + 4 + 1,
                                 /* round up to milliseconds */
                                 netif->lorawan.datarate, cr) + (US_PER_MS - 1)) / US_PER_MS;
   }
#endif
    default:
        DEBUG("schc: Unknown device type\n");
        return 0;
    }
}

static uint32_t _upstream_timeout(gnrc_netif_t *netif)
{
    switch (netif->device_type) {
#if IS_USED(MODULE_GNRC_SCHC_ETH)
    case NETDEV_TYPE_ETHERNET:
        return 1000;
#endif
#if IS_USED(MODULE_GNRC_NETIF_LORAWAN)
    case NETDEV_TYPE_LORA:
        /* see https://datatracker.ietf.org/doc/html/rfc9011#section-5.6.2 */
        return 12 * SEC_PER_HOUR * MS_PER_SEC;
#endif
    default:
        return 15000;
    }
}

static uint8_t send_callback(uint8_t* data, uint16_t length, uint32_t device_id) {
    uint8_t dst[GNRC_NETIF_L2ADDR_MAXLEN];
    gnrc_netif_t *netif;
    uint8_t dst_len = 0;

    DEBUG("schc: transmitting packet with length %" PRIu16 " for device %" PRIu32 "\n",
          length, device_id);

    (void)_get_deviid; /* TODO will be needed for compression and decompression */
    (void)_get_appiid; /* TODO will be needed for compression and decompression */
    netif = _get_netif(device_id);
    assert(netif != NULL);

    switch (netif->device_type) {
#ifdef MODULE_GNRC_SCHC_ETH
    case NETDEV_TYPE_ETHERNET:
        memset(dst, 0xff, ETHERNET_ADDR_LEN);
        dst_len = ETHERNET_ADDR_LEN;
        break;
#endif
#ifdef MODULE_GNRC_NETIF_LORAWAN
    case NETDEV_TYPE_LORA:
        dst[0] = data[0];
        dst_len = sizeof(dst[0]);
        data++;
        length--;
        break;
#endif
    default:
        break;
    }
    gnrc_pktsnip_t* schc = gnrc_pktbuf_add(NULL, data, length, GNRC_NETTYPE_SCHC);

    if (dst_len > 0U) {
        gnrc_pktsnip_t *netif_hdr;

        netif_hdr = gnrc_netif_hdr_build(NULL, 0, dst, dst_len);
        gnrc_netif_hdr_set_netif(netif_hdr->data, netif);

        LL_PREPEND(schc, netif_hdr);
    }

    if (gnrc_netif_send(netif, schc) < 1) {
        DEBUG("schc: unable to send %p over interface %u\n", (void *)schc,
              netif->pid);
        schc_reset(&schc_conn);
        gnrc_pktbuf_release(schc);
    }

    DEBUG("schc: sent %d bytes.\n", length);

    return 1;
}

void remove_timer_entry(schc_fragmentation_t *conn) {
    DEBUG("remove_timer_entry(): remove timer entry for device with id %" PRIu32 "\n",
          conn->device_id);
    if (conn->timer_ctx) {
        ztimer_remove(ZTIMER_MSEC, conn->timer_ctx);
        conn->timer_ctx = NULL;
    }
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *netif_snip;
    gnrc_netif_t *netif;
    DEBUG("schc: received packet with length %d \n", pkt->size);

    if ((netif_snip = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF)) == NULL) {
        DEBUG("schc: Received packet has no netif header\n");
        gnrc_pktbuf_release(pkt);
        return;
    } else {
        netif = gnrc_netif_hdr_get_netif(netif_snip->data);
    }

    if (IS_USED(MODULE_GNRC_NETIF_LORAWAN) && (netif->device_type == NETDEV_TYPE_LORA)) {
        gnrc_netif_hdr_t *netif_hdr = netif_snip->data;
        gnrc_pktsnip_t *tmp;
        uint8_t *data;

        if (netif_hdr->dst_l2addr_len != 1) {
            DEBUG("schc: Unexpected FPort length\n");
            gnrc_pktbuf_release(pkt);
            return;
        }

        tmp = gnrc_pktbuf_start_write(pkt);
        if (!tmp) {
            DEBUG("schc: Unable to write protect packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = tmp;
        if (gnrc_pktbuf_realloc_data(pkt, pkt->size + 1)) {
            DEBUG("schc: Unable resize payload to prepend FPort\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        data = pkt->data;

        for (int i = (pkt->size - 2); i >= 0; i--) {
            data[i + 1] = data[i];
        }
        data[0] = *gnrc_netif_hdr_get_dst_addr(netif_hdr);
    }

    uint32_t device_id = _get_device_id(netif);

    if (device_id == 0) {
        DEBUG("schc: can not associate netif %d to a device ID\n", netif->pid);
        gnrc_pktbuf_release(pkt);
        return;
    }
    /* get active connection and set the correct rule for this connection */
    schc_fragmentation_t *conn = schc_input((uint8_t*) pkt->data, pkt->size, &schc_conn, device_id);

    if ((conn != NULL) && (conn != &schc_conn)) {
        /* if returned value is receiving_conn: acknowledgement is received, which is handled
         * by the library */
        conn->post_timer_task = &set_schc_timer;
        conn->dc = _upstream_timeout(netif);    /* retransmission timer: used for timeouts */

        if ((conn->fragmentation_rule == NULL) ||
            (conn->fragmentation_rule->mode == NOT_FRAGMENTED)) { /* packet was not fragmented */
            DEBUG("schc: unfragmented packet received\n");
            end_rx(conn);
            schc_reset(conn);
        } else {
            int ret = schc_reassemble(conn);
            if (ret && conn->fragmentation_rule->mode == NO_ACK){
                /* use the connection to reassemble */
                DEBUG("schc: No ACKs to wait for, ending reception\n");
                end_rx(conn);   /* final packet arrived */
                schc_reset(conn);
            }
        }
    }

    /* release original headers and payload */
    gnrc_pktbuf_release(pkt);
}

static void _send(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *netif_snip;
    gnrc_pktsnip_t *ip6;

    if ((netif_snip = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF)) == NULL) {
        DEBUG("schc: Sending packet has no netif header\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    gnrc_netif_t *netif = gnrc_netif_hdr_get_netif(netif_snip->data);

    if (netif == NULL) {
        DEBUG("schc: Can not get SCHC specific interface information.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

#ifdef MODULE_GNRC_NETTYPE_IPV6
    if ((ip6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6)) == NULL) {
        DEBUG("schc: Sending packet has no IPv6 header\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#else
    /* just assume next snip to be IPv6 snip */
    ip6 = netif_snip->next;
#endif

    /* merge layers in ip6 packet */
    gnrc_pktbuf_merge(ip6);

    /* compress packet */
    uint32_t device_id = _get_device_id(netif);
    if (device_id == 0) {
        DEBUG("schc: unable to get device ID\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    if (schc_conn.device_id) {
        /* TODO allow for multiple conns with different network interfaces*/
        DEBUG("schc: already transmitting\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    schc_conn.device_id = device_id;        /* the device id of the connection */
    memset(message_buffer, 0, sizeof(message_buffer));
    schc_bitbuff.ptr = (uint8_t*) (message_buffer);     /* deep copy */
    schc_bitbuff.len = 0;

    if (ENABLE_DEBUG && IS_USED(MODULE_OD)) {
        DEBUG("schc: compressing packet\n");
        od_hex_dump(ip6->data, ip6->size, OD_WIDTH_DEFAULT);
    }
    struct schc_compression_rule_t* comp_rule;
    comp_rule = schc_compress(ip6->data, ip6->size, &schc_bitbuff, device_id, UP);

    if ((comp_rule == NULL) && (schc_bitbuff.len == 0)) {
        DEBUG("schc: compression step failed.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    struct schc_fragmentation_rule_t *frag_rule = NULL;
    /* TODO make usage with non-lorawan devices possible */
    reliability_mode upstream_mode = netif->schc.upstream_mode;
    frag_rule = get_fragmentation_rule_by_reliability_mode(upstream_mode, device_id);

    if (!frag_rule) {
        DEBUG("schc: no appropriate rule for fragmentation mode %d and device %" PRIu32 "\n",
              upstream_mode, device_id);
        gnrc_pktbuf_release(pkt);
        return;
    }
    if (netif_get_opt(&netif->netif, NETOPT_MAX_PDU_SIZE, 0,
                      &schc_conn.mtu, sizeof(schc_conn.mtu)) < 0) {
        DEBUG("schc: error getting link-layer PDU\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    if (schc_conn.mtu > MAX_MTU_LENGTH) {
        DEBUG("schc: reducing MTU to MAX_MTU_LENGTH\n");
        schc_conn.mtu = MAX_MTU_LENGTH;
    }
    gnrc_netif_acquire(netif);
    schc_conn.dc = _get_duty_cycle(netif, schc_conn.mtu);
    if (schc_conn.dc == 0) {
        DEBUG("schc: unable to calculate duty-cycle\n");
        gnrc_netif_release(netif);
        gnrc_pktbuf_release(pkt);
        return;
    }
    gnrc_netif_release(netif);
    switch (netif->device_type) {
#if IS_USED(MODULE_GNRC_NETIF_LORAWAN)
    case NETDEV_TYPE_LORA:
        schc_conn.mtu++;    /* rule ID goes into FPort field, so we have one byte more of PDU */
#endif
    default:
        break;
    }
    DEBUG("schc: sending with mtu=%" PRIu16 ",dc=%" PRIu32 "\n", schc_conn.mtu, schc_conn.dc);
    schc_conn.bit_arr = &schc_bitbuff;
    schc_conn.fragmentation_rule = frag_rule;

    /* release original headers and payload */
    gnrc_pktbuf_release(pkt);

    int res = schc_fragment(&schc_conn);    /* start fragmentation loop */
    if (res == -2) {
        DEBUG("schc: No fragmentation needed\n");
        end_tx(&schc_conn);
    }
}

static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_SCHC_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            thread_getpid());

    (void)args;
    msg_init_queue(msg_q, GNRC_SCHC_MSG_QUEUE_SIZE);

    /* register interest in all SCHC packets */
    gnrc_netreg_register(GNRC_NETTYPE_SCHC, &me_reg);

    /* preinitialize ACK */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    schc_fragmenter_init(&schc_conn);

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
        case GNRC_SCHC_TIMER_CB_MSG: {
            DEBUG("schc: timer event received\n");
            _gc_callback_t *gc_callback = msg.content.ptr;
            struct schc_fragmentation_t *conn = gc_callback->arg;
            conn->timer_ctx = NULL;
            gc_callback->callback(conn);
            break;
        }
        default:
            DEBUG("schc: operation not supported\n");
            break;
        }
    }

    return NULL;
}
