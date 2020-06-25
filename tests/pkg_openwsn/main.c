/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       OpenWSN test application
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "od.h"
#include "fmt.h"
#include "shell.h"
#include "openwsn.h"
#include "net/ipv6/addr.h"

#include "opendefs.h"
#include "scheduler.h"
#include "02a-MAClow/IEEE802154E.h"
#include "02b-MAChigh/neighbors.h"
#include "03b-IPv6/icmpv6rpl.h"
#include "04-TRAN/openudp.h"
#include "cjoin.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/idmanager.h"
#include "cross-layers/packetfunctions.h"

extern idmanager_vars_t idmanager_vars;
extern icmpv6rpl_vars_t icmpv6rpl_vars;
extern neighbors_vars_t neighbors_vars;
extern openqueue_vars_t openqueue_vars;
extern schedule_vars_t schedule_vars;
extern scheduler_dbg_t scheduler_dbg;

udp_resource_desc_t uinject_vars;
char addr_str[IPV6_ADDR_MAX_STR_LEN];

void uinject_sendDone(OpenQueueEntry_t *msg, owerror_t error)
{
    (void)error;

    printf("msg.l2_sendDoneError: %x\n", msg->l2_sendDoneError);
    openqueue_freePacketBuffer(msg);
    puts("Send success");
}

void uinject_receive(OpenQueueEntry_t *pkt)
{
    printf("Received %i bytes on port %i\n", (int)pkt->length,
           pkt->l4_destination_port);
    od_hex_dump(pkt->payload, pkt->length, OD_WIDTH_DEFAULT);
    openqueue_freePacketBuffer(pkt);
}

void uinject_init(void)
{
    uinject_vars.port = WKP_UDP_INJECT;
    uinject_vars.callbackReceive = &uinject_receive;
    uinject_vars.callbackSendDone = &uinject_sendDone;
    openudp_register(&uinject_vars);
}

char *_array_2_string(const uint8_t *addr, size_t addr_len, char *out)
{
    char *res = out;

    assert((out != NULL) && ((addr != NULL) || (addr_len == 0U)));
    out[0] = '\0';
    for (size_t i = 0; i < addr_len; i++) {
        out += fmt_byte_hex((out), *(addr++));
        *(out++) = (i == (addr_len - 1)) ? '\0' : ':';
    }
    return res;
}

static int ifconfig_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    open_addr_t *hwaddr;
    open_addr_t temp_my128bID;

    memcpy(&temp_my128bID.addr_128b[0], &idmanager_vars.myPrefix.prefix, 8);
    memcpy(&temp_my128bID.addr_128b[8], &idmanager_vars.my64bID.addr_64b, 8);

    printf("Iface  %d  ", openwsn_get_pid());
    hwaddr = idmanager_getMyID(ADDR_16B);
    printf("\tHWaddr: %s  ", _array_2_string(hwaddr->addr_16b, 2, addr_str));
    hwaddr = idmanager_getMyID(ADDR_PANID);
    printf("NID: %s\n", _array_2_string(hwaddr->panid, 2, addr_str));
    printf("\n");

    hwaddr = idmanager_getMyID(ADDR_64B);
    printf("\t\tLong HWaddr: %s\n", _array_2_string(hwaddr->addr_64b, 8, addr_str));

    ipv6_addr_to_str(addr_str, (ipv6_addr_t *)temp_my128bID.addr_128b,
                     sizeof(addr_str));
    printf("\t\tinet6 addr: %s\n", addr_str);
    printf("\n");


    printf("\t\tIEEE802154E sync: %i\n", ieee154e_isSynch());
    printf("\t\t6TiSCH joined: %i\n", cjoin_getIsJoined());
    printf("\n");

    if (idmanager_vars.isDAGroot) {
        puts("\t\tNode is DAG root");
    }
    else {
        if (icmpv6rpl_vars.haveParent) {
            printf("\t\tRPL rank: %i\n", icmpv6rpl_vars.myDAGrank);
            printf("\t\tRPL parent: %s\n", \
                   _array_2_string(neighbors_vars.neighbors[icmpv6rpl_vars.
                                                            ParentIndex].
                                   addr_64b.addr_64b, 8, addr_str));
            printf("\t\tRPL children:\n");
            for (uint8_t i = 0; i < MAXNUMNEIGHBORS; i++) {
                if ((neighbors_isNeighborWithHigherDAGrank(i)) == true) {
                    printf("\t\t\t%s\n", \
                        _array_2_string(neighbors_vars.neighbors[i].
                        addr_64b.addr_64b, 8, addr_str));
                }
            }
            ipv6_addr_to_str(addr_str,
                             (ipv6_addr_t *)icmpv6rpl_vars.dao.DODAGID,
                             sizeof(addr_str));
            printf("\t\tRPL DODAG ID: %16s\n", addr_str);
        }
        else {
            puts("\t\tNO RPL parent");
        }
    }
    return 0;
}

static int nc_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (int i = 0; i < MAXNUMNEIGHBORS; i++) {
        _array_2_string(neighbors_vars.neighbors[i].addr_64b.addr_64b, 8,
                        addr_str);
        if (memcmp(addr_str, "00:00:00:00:00:00:00:00", 8) != 0) {
            printf("%02i. %s\n", i, addr_str);
        }
    }
    return 0;
}

static const struct {
    char *name;
    int id;
} names[] = {
    { "?", COMPONENT_NULL },
    { "owsn", COMPONENT_OPENWSN },
    { "idmanager", COMPONENT_IDMANAGER },
    { "oqueue", COMPONENT_OPENQUEUE },
    { "oserial", COMPONENT_OPENSERIAL },
    { "pktfuncs", COMPONENT_PACKETFUNCTIONS },
    { "random", COMPONENT_RANDOM },
    { "radio", COMPONENT_RADIO },
    { "154", COMPONENT_IEEE802154 },
    { "154e", COMPONENT_IEEE802154E },
    { "6top2154e", COMPONENT_SIXTOP_TO_IEEE802154E },
    { "154e26top", COMPONENT_IEEE802154E_TO_SIXTOP },
    { "6top", COMPONENT_SIXTOP },
    { "neigh", COMPONENT_NEIGHBORS },
    { "sched", COMPONENT_SCHEDULE },
    { "6topres", COMPONENT_SIXTOP_RES },
    { "bridge", COMPONENT_OPENBRIDGE },
    { "iphc", COMPONENT_IPHC },
    { "frag", COMPONENT_FRAG },
    { "fwd", COMPONENT_FORWARDING },
    { "icmpv6", COMPONENT_ICMPv6 },
    { "icmpv6ech", COMPONENT_ICMPv6ECHO },
    { "icmpv6rtr", COMPONENT_ICMPv6ROUTER },
    { "icmpv6rpl", COMPONENT_ICMPv6RPL },
    { "udp", COMPONENT_OPENUDP },
    { "coap", COMPONENT_OPENCOAP },
    { "cjoin", COMPONENT_CJOIN },
    { "openoscoap", COMPONENT_OPENOSCOAP },
    { "c6t", COMPONENT_C6T },
    { "uinject", COMPONENT_UINJECT },
};

char *_get_name(int id)
{
    for (unsigned i = 0; i < ARRAY_SIZE(names); i++) {
        if (id == names[i].id) {
            return names[i].name;
        }
    }
    return NULL;
}

static int q_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    bool queue = 1;

    for (uint8_t i = 0; i < QUEUELENGTH; i++) {
        if (openqueue_vars.queue[i].creator || openqueue_vars.queue[i].owner) {
            queue = 0;
            uint8_t creator = openqueue_vars.queue[i].creator;
            uint8_t owner = openqueue_vars.queue[i].owner;
            printf("Creator: %.9s [%d], ", _get_name(creator), creator);
            printf("Owner: %.9s [%d]\n", _get_name(owner), owner);
        }
    }
    if (queue) {
        puts("openqueue empty");
    }
    return 0;
}

static int q_rmv(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [creator]\n", argv[0]);
        return 1;
    }

    uint8_t creator = atoi(argv[1]);
    if (creator == 0) {
        printf("error: invalid input value\n");
        return 1;
    }
    else {
        printf("Removing entries created by: %.9s [%d]\n", _get_name(creator), creator);
        openqueue_removeAllCreatedBy(creator);
    }
    return 0;
}

static int as_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (int i = 0; i < MAXACTIVESLOTS; i++) {
        switch (schedule_vars.scheduleBuf[i].type) {
            case CELLTYPE_TX:
                printf("neigh: %s, slot: %03i, chan: %02i, type: TX\n", \
                       _array_2_string(
                           schedule_vars.scheduleBuf[i].neighbor.addr_64b,
                           8, addr_str),
                       schedule_vars.scheduleBuf[i].slotOffset, \
                       schedule_vars.scheduleBuf[i].channelOffset);
                break;
            case CELLTYPE_RX:
                printf("slot: %03i, chan: %02i, type: RX\n", \
                       schedule_vars.scheduleBuf[i].slotOffset, \
                       schedule_vars.scheduleBuf[i].channelOffset);
                break;
            case CELLTYPE_TXRX:
                printf("neigh: %s, slot: %03i, chan: %02i, type: RXTX\n", \
                       _array_2_string(
                           schedule_vars.scheduleBuf[i].neighbor.addr_64b,
                           8, addr_str),
                       schedule_vars.scheduleBuf[i].slotOffset, \
                       schedule_vars.scheduleBuf[i].channelOffset);
                break;
            default:
                break;
        }
    }

    return 0;
}

static int sc_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* TODO allow other prefixes via shell ?!? */
    printf("Current tasks:%i\n", scheduler_dbg.numTasksCur);
    printf("Max tasks:    %i\n", scheduler_dbg.numTasksMax);

    return 0;
}

extern int udp_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "ifconfig", "Shows assigned IPv6 addresses", ifconfig_cmd },
    { "nc", "Shows neighbor table", nc_cmd },
    { "q", "Shows Openqueue", q_cmd },
    { "q-rmv", "Remove entries from creator in queue", q_rmv },
    { "as", "Shows active cells", as_cmd },
    { "sc", "Shows scheduler (openos) dbg states", sc_cmd },
    { "udp", "Send data over UDP and listen on UDP ports", udp_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("OpenWSN UDP test");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    uinject_init();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
