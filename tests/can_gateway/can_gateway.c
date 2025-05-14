/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @author  Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#include <stdbool.h>
#include "can_gateway.h"
#include "ps.h"

#if GATEWAY_OPERATING_MODE
#include "lwip.h"
#include "lwip/netif.h"
#include "net/ipv4/addr.h"
#include "net/sock/udp.h"
#include "net/af.h"
#include "net/ipv4.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Maximum number of CAN-filters per channel */
#define FILTER_NUM 14
#define TRX_MODE TRX_HIGH_SPEED_MODE

#define CAN_START_RECV  0x400

/* Must be 0, otherwise receive would stop after given time */
#define CAN_RECV_TIMEOUT 0

#define THREAD_STACKSIZE  1024
#define RECEIVE_THREAD_MSG_QUEUE_SIZE 8
#define SOCK_INBUF_SIZE         256

/* Maximum number of ID's in list */
#define ID_LIST_COUNT 30

#define ETH_FLAGS 0x00
#define ETH_MULTICAST 0x01

uint16_t list_counter = 0;
struct id can_id_list[ID_LIST_COUNT];

netif_t *netif_iface;

kernel_pid_t can_receive_pid[CAN_DLL_NUMOF];
char can_thread_stack[CAN_DLL_NUMOF][THREAD_STACKSIZE];
int can_thread_busy[CAN_DLL_NUMOF];
conn_can_raw_t conn[CAN_DLL_NUMOF];
struct can_filter can_filters[CAN_DLL_NUMOF][FILTER_NUM];

static bool server_running;
static uint8_t sock_inbuf[SOCK_INBUF_SIZE];
static sock_udp_t ip_sock;
static char ip_stack[THREAD_STACKSIZE];

#ifndef GATEWAY_OPERATING_MODE
#define SOCK_IP_EP_ANY  SOCK_IPV6_EP_ANY
static char ethernet_stack[THREAD_STACKSIZE];
#else
#define SOCK_IP_EP_ANY  SOCK_IPV4_EP_ANY
#endif

#ifdef MODULE_NCV7356
#include "ncv7356.h"
ncv7356_trx_t ncv7356 = { .trx.driver = &ncv7356_driver,
                          .mode0_pin = NCV7356_MODE0_PIN,
                          .mode1_pin = NCV7356_MODE1_PIN
};
#endif

static can_trx_t *devs[] = {
#ifdef MODULE_NCV7356
    (can_trx_t*) &ncv7356,
#endif
};

static void _print_usage(void)
{
    puts("can_gateway send ifnum can_id\
          [B1 [B2 [B3 [B4 [B5 [B6 [B7 [B8]]]]]]]]");
    puts("can_gateway add_id [CAN ID] [Dst. Port] [Dst. IP] [Eth send]\
          [CAN Interface] [Src CAN Interface]");
    puts("can_gateway set_bitrate [iface] [bitrate] [sample_point]");
    puts("can_gateway start_can_recv");
    puts("can_gateway close");
    puts("can_gateway delete_list");
}

int _get_can_id(uint32_t can_id)
{
    if (list_counter == 0) {
        DEBUG("_get_can_id: No list found!\n");
        return -1;
    }

    int res = -1;

    for (size_t i = 0; i < list_counter; i++) {
        if (can_id_list[i].can_id == can_id) {
            res = i;
            break;
        }
    }

    return res;
}

static void _close_can_connection(void)
{
    for (size_t i = 0; i < CAN_DLL_NUMOF; i++) {
        conn_can_raw_close(&conn[i]);
        can_thread_busy[i] = 0;
    }
}

static int _set_can_bitrate(size_t iface, uint32_t bitrate, uint32_t sample_point)
{
    int ret;

    if (iface >= CAN_DLL_NUMOF) {
        DEBUG("_set_can_bitrate: Wrong interface\n");
        return -1;
    }

    ret = raw_can_set_bitrate(iface, bitrate, sample_point);
    if (ret < 0) {
        DEBUG("_set_can_bitrate: Error when setting bitrate: res=%d\n", ret);
        return -1;
    }
    else if (ret == 1) {
        DEBUG("_set_can_bitrate: Bitrate/sample_point cannot\
               be reached but the bitrate is set\n");
    }

    DEBUG("_set_can_bitrate: Bittimings successfully set\n");

    return 0;
}

static int _can_send(const struct can_frame *frame, int8_t iface)
{

    if ((iface >= CAN_DLL_NUMOF) || (iface < 0)) {
        DEBUG("_can_send: Wrong interface\n");
        return -1;
    }

    conn_can_raw_t conn;
    conn_can_raw_create(&conn, NULL, 0, iface, 0);
    int ret = conn_can_raw_send(&conn, frame, 0);
    if (ret < 0) {
        DEBUG("_can_send: Error when trying to send\n");
        return -1;
    }

    DEBUG("_can_send: Msg sent over CAN\n");

    return 0;
}

static uint32_t _set_someip_hdr_length(uint8_t dlc)
{
    return SOMEIP_HDR_LENGTH + (uint32_t)dlc;
}

static void _conv_can_to_someip(const struct can_frame *frame, someip_hdr_t *hdr, uint8_t *data)
{
    hdr->message_id.service_id = htons((0x1FFF0000 & frame->can_id) >> 16);
    hdr->message_id.method_id = htons((0x0000FFFF & frame->can_id));

    hdr->length = htonl(_set_someip_hdr_length(frame->can_dlc));

    hdr->request_id.client_id = SOMEIP_CLIENT_ID;
    hdr->request_id.session_id = SOMEIP_SESSION_ID;

    hdr->protocol_version = SOMEIP_PROTOCOL_VERSION;
    hdr->interface_version = SOMEIP_INTERFACE_VERSION;
    hdr->msg_type = SOMEIP_MESSAGE_TYPE;
    hdr->return_code = SOMEIP_RETURN_CODE;

    for (size_t i = 0; i < frame->can_dlc; i++) {
        data[i] = frame->data[i];
    }
}

static int _conv_someip_to_can(someip_hdr_t *hdr, uint8_t *payload)
{
    struct can_frame frame;

    if (hdr->protocol_version != SOMEIP_PROTOCOL_VERSION) {
        DEBUG("_conv_someip_to_can: Wrong protocol version!\n");
        return -1;
    }

    if (htonl(hdr->length) < SOMEIP_HDR_LENGTH ||
        htonl(hdr->length) > (SOMEIP_HDR_LENGTH + CAN_MAX_DLEN)) {
        DEBUG("_conv_someip_to_can: Wrong SOME/IP length\n");
        return -1;
    }

    frame.can_id = htons(hdr->message_id.service_id) << 16 |
                   htons(hdr->message_id.method_id);

    if (frame.can_id > CAN_EFF_MASK) {
        DEBUG("_conv_someip_to_can: ID greater than allowed\n");
        return -1;
    }

    int id_pos = _get_can_id(frame.can_id);
    if (id_pos == -1) {
        DEBUG("_conv_someip_to_can: No matching ID found\n");
        return -1;
    }

    if (frame.can_id > CAN_SFF_MASK) {
        frame.can_id = frame.can_id | CAN_EFF_FLAG;
    }

    if ((htonl(hdr->length) - SOMEIP_HDR_LENGTH) > CAN_MAX_DLEN) {
        DEBUG("_conv_someip_to_can: Wrong can_dlc size!\n");
        return -1;
    }
    frame.can_dlc = htonl(hdr->length) - SOMEIP_HDR_LENGTH;

    for (size_t i = 0; i < frame.can_dlc; i++) {
        frame.data[i] = payload[i];
    }

    _can_send(&frame, can_id_list[id_pos].interface_information.dst_ifnum);

    return 0;
}

static void *_ip_receive_thread(void *args)
{
    (void)args;
    sock_udp_ep_t server_addr = SOCK_IP_EP_ANY;
    msg_t ip_msg_queue[RECEIVE_THREAD_MSG_QUEUE_SIZE];

    msg_init_queue(ip_msg_queue, RECEIVE_THREAD_MSG_QUEUE_SIZE);

    #ifdef GATEWAY_OPERATING_MODE
    struct netif *iface = netif_list;
    IP4_ADDR(&iface->ip_addr, 192, 168, 1, 11);
    #endif

    server_addr.port = SERVER_PORT;
    if (sock_udp_create(&ip_sock, &server_addr, NULL, 0) < 0) {
        DEBUG("_ip_receive_thread: Error creating UDP server\n");
        return NULL;
    }
    server_running = true;
    DEBUG("_ip_receive_thread: Server created and recv on port (%d) started\n",
          server_addr.port);

    while (1) {
        sock_udp_ep_t remote;

        if (sock_udp_recv(&ip_sock, sock_inbuf, sizeof(sock_inbuf),
                                 SOCK_NO_TIMEOUT, &remote) < 0) {
            DEBUG("_ip_receive_thread: Error on receive\n");
        } else {
            DEBUG("_ip_receive_thread: Message received\n");
            someip_hdr_t *hdr = (someip_hdr_t *)sock_inbuf;
            uint8_t *payload = sock_inbuf + sizeof(*hdr);
            _conv_someip_to_can(hdr, payload);
        }
    }
    return NULL;
}

#ifndef GATEWAY_OPERATING_MODE
static int _ip_send(const struct can_frame *frame, uint16_t dst_port, ipv6_addr_t ip_addr)
#else
static int _ip_send(const struct can_frame *frame, uint16_t dst_port, ipv4_addr_t ip_addr)
#endif
{
    someip_hdr_t hdr;
    uint8_t data[8];
    sock_udp_ep_t remote;

    _conv_can_to_someip(frame, &hdr, data);

    uint8_t sock_outbuf[SOMEIP_FULL_HDR_SIZE + frame->can_dlc];

    memcpy(sock_outbuf, (uint8_t *) &hdr, sizeof(someip_hdr_t));
    if (frame->can_dlc != 0) {
        memcpy(sock_outbuf + sizeof(someip_hdr_t),
        (uint8_t *)&data, sizeof(data));
    }

    #ifndef GATEWAY_OPERATING_MODE
    remote.family = AF_INET6;
    memcpy(&remote.addr.ipv6, &ip_addr, sizeof(ipv6_addr_t));
    #else
    remote.family = AF_INET;
    memcpy(&remote.addr.ipv4, &ip_addr, sizeof(ipv4_addr_t));
    #endif

    remote.netif = SOCK_ADDR_ANY_NETIF;
    remote.port = dst_port;

    if (!server_running) {
        DEBUG("_ip_send: Server is not active\n");
        return -1;
    }

    if (sock_udp_send(&ip_sock, &sock_outbuf,
        (SOMEIP_FULL_HDR_SIZE + frame->can_dlc), &remote) < 0) {
        DEBUG("_ip_send: Error sending message\n");
        sock_udp_close(&ip_sock);
        return -1;
    }

    DEBUG("_ip_send: Msg sent over ip\n");

    return 0;
}

#ifndef GATEWAY_OPERATING_MODE
static int _ethernet_send(const struct can_frame *frame)
{
    gnrc_pktsnip_t *pkt, *hdr;
    gnrc_netif_hdr_t *nethdr;
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];

    /* Set hwaddr and multicast */
    hwaddr[5] = (frame->can_id & 0x000000FF);
    hwaddr[4] = (frame->can_id & 0x0000FF00) >> 8;
    hwaddr[3] = (frame->can_id & 0x00FF0000) >> 16;
    hwaddr[2] = (frame->can_id & 0x1F000000) >> 24;
    hwaddr[1] = 0x00;
    hwaddr[0] = ETH_MULTICAST;

    size_t frame_size = frame->can_dlc + 1;

    uint8_t data[frame_size];

    data[0] = frame->can_dlc;

    for (size_t i = 0; i < frame->can_dlc; i++) {
        data[i + 1] = frame->data[i];
    }

    /* put packet together */
    pkt = gnrc_pktbuf_add(NULL, data, frame_size, GNRC_NETTYPE_CUSTOM);
    if (pkt == NULL) {
        DEBUG("_ethernet_send: packet buffer full\n");
        return -1;
    }

    hdr = gnrc_netif_hdr_build(NULL, 0, hwaddr, sizeof(hwaddr));
    if (hdr == NULL) {
        DEBUG("_ethernet_send: packet buffer full\n");
        gnrc_pktbuf_release(pkt);
        return -1;
    }
    LL_PREPEND(pkt, hdr);
    nethdr = (gnrc_netif_hdr_t *)hdr->data;
    nethdr->flags = ETH_FLAGS;

    if (gnrc_netapi_send(((gnrc_netif_t *)netif_iface)->pid, pkt) < 1) {
        DEBUG("_ethernet_send: unable to send\n");
        gnrc_pktbuf_release(pkt);
        return -1;
    }

    DEBUG("_ethernet_send: Msg sent over eth\n");

    return 0;
}
#endif

#ifndef GATEWAY_OPERATING_MODE
static int _fill_can_data(void *snip, struct can_frame *frame)
{
    unsigned char *data = (unsigned char *)snip;

    frame->can_dlc = data[0];

    if (frame->can_dlc > 8) {
        DEBUG("_fill_can_data: Wrong dlc size\n");
        return -1;
    }

    for (size_t i = 0; i < frame->can_dlc; i++) {
        frame->data[i] = data[i + 1];
    }

    return 0;
}
#endif

#ifndef GATEWAY_OPERATING_MODE
static int _conv_eth_frame_to_can(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *snip = pkt;
    uint8_t hwaddr[6];

    struct can_frame frame;

    while (snip != NULL) {
        if (snip->type == GNRC_NETTYPE_CUSTOM) {
            if (_fill_can_data(snip->data, &frame) != 0) {
                return -1;
            }
        } else if (snip->type == GNRC_NETTYPE_NETIF) {
            gnrc_netif_hdr_t *data_pointer = (gnrc_netif_hdr_t *)snip->data;
            memcpy(hwaddr, gnrc_netif_hdr_get_dst_addr(snip->data),
                    data_pointer->dst_l2addr_len);
        }
        snip = snip->next;
    }

    frame.can_id = hwaddr[2] << 24 | hwaddr[3] << 16 |
                   hwaddr[4] << 8 | hwaddr[5];

    int id_pos = _get_can_id(frame.can_id);
    if (id_pos == -1) {
        DEBUG("_conv_eth_frame_to_can: No matching ID found\n");
        gnrc_pktbuf_release(pkt);
        return -1;
    }

    if (frame.can_id > CAN_SFF_MASK) {
        frame.can_id = frame.can_id | CAN_EFF_FLAG;
    }

    _can_send(&frame, can_id_list[id_pos].interface_information.dst_ifnum);
    gnrc_pktbuf_release(pkt);

    return 0;
}
#endif

#ifndef GATEWAY_OPERATING_MODE
void *_eth_receive_thread(void *args)
{
    (void)args;
    msg_t msg, msg_queue[RECEIVE_THREAD_MSG_QUEUE_SIZE];

    msg_init_queue(msg_queue, RECEIVE_THREAD_MSG_QUEUE_SIZE);

    DEBUG("_eth_receive_thread: launching eth_receive_thread\n");

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("_eth_receive_thread: Msg received over eth\n");
                _conv_eth_frame_to_can(msg.content.ptr);
                break;
            default:
                DEBUG("_eth_receive_thread: received something unexpected\n");
                break;
        }
    }
    return NULL;
}
#endif

static int _start_can_receive(void)
{
    for (size_t i = 0; i < CAN_DLL_NUMOF; i++) {
        if (can_thread_busy[i]) {
            DEBUG("_start_can_receive: Thread already in use\n");
            continue;
        }

        uint8_t filter_count = 0;

        for (size_t entry = 0; entry < list_counter; entry++) {
            if (can_id_list[entry].interface_information.src_ifnum == i) {
                if (can_id_list[entry].can_id > CAN_SFF_MASK) {
                    can_filters[i][filter_count].can_id = can_id_list[entry].can_id | CAN_EFF_FLAG;
                } else {
                    can_filters[i][filter_count].can_id = can_id_list[entry].can_id;
                }
                can_filters[i][filter_count].can_mask = 0xffffffff;
                filter_count ++;
                if (filter_count >= FILTER_NUM) {
                    DEBUG("_start_can_receive: Max filter number for\
                           interface %d reached\n", i);
                    break;
                }
            }
        }

        if (filter_count == 0) {
            DEBUG("_start_can_receive: Nothing to do for interface %d\n", i);
            continue;
        }

        int res = conn_can_raw_create(&conn[i], can_filters[i],
                                  filter_count, i, 0);
        if (res < 0) {
            DEBUG("_start_can_receive: Error when setting filters\n");
            return -1;
        }
        can_thread_busy[i] = 1;

        msg_t msg;
        msg.type = CAN_START_RECV;
        msg.content.value = CAN_RECV_TIMEOUT;

        msg_send(&msg, can_receive_pid[i]);
        DEBUG("_start_can_receive: Receive on thread %d started\n", i);
    }

    return 0;
}

static void *_can_receive_thread(void *args)
{
    int thread_nb = (int)args;
    struct can_frame frame;
    msg_t msg, msg_queue[RECEIVE_THREAD_MSG_QUEUE_SIZE];

    msg_init_queue(msg_queue, RECEIVE_THREAD_MSG_QUEUE_SIZE);

    DEBUG("_can_receive_thread: launching can_receive_thread\n");

    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
        case CAN_START_RECV:
        {
            int ret;
            while ((ret = conn_can_raw_recv(&conn[thread_nb],
                    &frame, msg.content.value))
                   == sizeof(struct can_frame)) {
                DEBUG("%d: %-8s %" PRIx32 "  [%x] ",
                       thread_nb,
                       raw_can_get_name_by_ifnum(conn[thread_nb].ifnum),
                       (frame.can_id & CAN_EFF_MASK), frame.can_dlc);
                for (int i = 0; i < frame.can_dlc; i++) {
                    DEBUG(" %02X", frame.data[i]);
                }
                DEBUG("\n");

                if (frame.can_id & CAN_EFF_FLAG) {
                    frame.can_id = frame.can_id & CAN_EFF_MASK;
                }

                int id_pos = _get_can_id(frame.can_id);
                if (id_pos == -1) {
                    DEBUG("_can_receive_thread: No matching ID found\n");
                    continue;
                } else  {
                    #ifndef GATEWAY_OPERATING_MODE
                    if (can_id_list[id_pos].interface_information.dst_port &&
                        !ipv6_addr_is_unspecified(&can_id_list[id_pos].interface_information.ipv6_addr)) {
                        _ip_send(&frame,
                                 can_id_list[id_pos].interface_information.dst_port,
                                 can_id_list[id_pos].interface_information.ipv6_addr);
                    }

                    if (can_id_list[id_pos].interface_information.eth_send) {
                        _ethernet_send(&frame);
                    }
                    #else
                    if (can_id_list[id_pos].interface_information.dst_port &&
                        (&can_id_list[id_pos].interface_information.ipv4_addr != NULL)) {
                        _ip_send(&frame,
                                 can_id_list[id_pos].interface_information.dst_port,
                                 can_id_list[id_pos].interface_information.ipv4_addr);
                    }
                    #endif

                    if (can_id_list[id_pos].interface_information.dst_ifnum >= 0) {
                        _can_send(&frame,
                                  can_id_list[id_pos].interface_information.dst_ifnum);
                    }
                }
            }
            DEBUG("%d: recv terminated: ret=%d\n", thread_nb, ret);
            conn_can_raw_close(&conn[thread_nb]);
            can_thread_busy[thread_nb] = 0;
            break;
        }
        default:
            DEBUG("%d: _receive_thread: received unknown message\n", thread_nb);
            break;
        }
    }
    return NULL;
}

kernel_pid_t init_gateway(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(devs); i++) {
        int res = can_trx_init(devs[i]);
        if (res < 0) {
            DEBUG("init_gateway: Error when initializing trx: %d\n", res);
            return -1;
        }
        res = can_trx_set_mode(devs[i], TRX_MODE);
        if (res < 0) {
            DEBUG("init_gateway: Error when setting mode: %d\n", res);
            return -1;
        }
    }

    DEBUG("init_gateway: Trx successfully initialized\n");

    for (int i = 0; i < CAN_DLL_NUMOF; i++) {
        _set_can_bitrate(i, CAN_BITRATE, CAN_SAMPLE_POINT);

        can_receive_pid[i] = thread_create(can_thread_stack[i],
                                           THREAD_STACKSIZE,
                                           THREAD_PRIORITY_MAIN - 1,
                                           THREAD_CREATE_STACKTEST,
                                           _can_receive_thread,
                                           (void*)i, "can_receive_thread");
    }

    if (thread_create(ip_stack, sizeof(ip_stack), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, _ip_receive_thread, NULL,
                      "ip_receive_thread") <= KERNEL_PID_UNDEF) {
        DEBUG("init_gateway: Error when starting the socket\n");
        return -1;
    }

    #ifndef GATEWAY_OPERATING_MODE
    kernel_pid_t eth_receive = thread_create(ethernet_stack,
                                             sizeof(ethernet_stack),
                                             THREAD_PRIORITY_MAIN - 1,
                                             THREAD_CREATE_STACKTEST,
                                             _eth_receive_thread, NULL,
                                             "eth_receive_thread");

    netif_iface = netif_iter(NULL);

    return eth_receive;
    #else
    return 0;
    #endif
}

static int _add_can_id(int argc, char **argv)
{
    if (argc < 8) {
        DEBUG("_add_can_id: usage: %s %s [CAN ID] [Port] [IP] [Eth send]\
              [Dst. Iface] [Src. Iface]\n", argv[0], argv[1]);
        return -1;
    }

    if (list_counter >= ID_LIST_COUNT) {
        DEBUG("_add_can_id: List is full!\n");
        return -1;
    } else {
        uint32_t can_id = strtoul(argv[2], NULL, 16);

        if (can_id > CAN_EFF_MASK) {
            DEBUG("_add_can_id: ID bigger than allowed\n");
            return -1;
        } else if (_get_can_id(can_id) >= 0) {
            DEBUG("_add_can_id: ID already in list\n");
            return -1;
        } else {
            can_id_list[list_counter].can_id = strtoul(argv[2], NULL, 16);
        }

        can_id_list[list_counter].interface_information.dst_port = atoi(argv[3]);
        #ifndef GATEWAY_OPERATING_MODE
        ipv6_addr_from_str(&can_id_list[list_counter].interface_information.ipv6_addr, argv[4]);
        can_id_list[list_counter].interface_information.eth_send = atoi(argv[5]);
        #else
        ipv4_addr_from_str(&can_id_list[list_counter].interface_information.ipv4_addr, argv[4]);
        #endif
        can_id_list[list_counter].interface_information.dst_ifnum = atoi(argv[6]);
        can_id_list[list_counter].interface_information.src_ifnum = atoi(argv[7]);
    }

    list_counter ++;

    DEBUG("_add_can_id: Entry added\n");

    return 0;
}

static int _delete_id_list(void)
{
    if (list_counter == 0) {
        DEBUG("_delete_id_list: No list found\n");
        return -1;
    }

    list_counter = 0;

    can_id_list[list_counter].can_id = 0;
    can_id_list[list_counter].interface_information.dst_port = 0;
    #ifndef GATEWAY_OPERATING_MODE
    ipv6_addr_from_str(&can_id_list[list_counter].interface_information.ipv6_addr, "0");
    can_id_list[list_counter].interface_information.eth_send = -1;
    #else
    ipv4_addr_from_str(&can_id_list[list_counter].interface_information.ipv4_addr, "0");
#endif
    can_id_list[list_counter].interface_information.dst_ifnum = -1;
    can_id_list[list_counter].interface_information.src_ifnum = -1;

    DEBUG("_delete_id_list: List deleted\n");
    return 0;
}

int can_gateway_handler(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage();
        return -1;
    }
    else if (strcmp(argv[1], "add_id") == 0) {
        _add_can_id(argc, argv);
    }
    else if (strcmp(argv[1], "set_bitrate") == 0) {
        if (argc < 4) {
            _print_usage();
            return -1;
        }
        return _set_can_bitrate(strtoul(argv[2], NULL, 0),
                                strtoul(argv[3], NULL, 0),
                                strtoul(argv[4], NULL, 0));
    }
    else if (strcmp(argv[1], "start_can_recv") == 0) {
        return _start_can_receive();
    }
    else if (strcmp(argv[1], "close") == 0) {
        _close_can_connection();
    }
    else if (strcmp(argv[1], "delete_list") == 0) {
        return _delete_id_list();
    }
    else {
        printf("unknown command: %s\n", argv[1]);
        return -1;
    }
    return 0;
}
