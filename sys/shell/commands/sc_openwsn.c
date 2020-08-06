/*
 * Copyright 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command implementation for OpenWSN
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "fmt.h"
#include "net/ipv6/addr.h"

#include "openwsn.h"
#include "opendefs.h"
#include "scheduler.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/idmanager.h"
#include "cross-layers/packetfunctions.h"

#include "02a-MAClow/IEEE802154E.h"
#include "02b-MAChigh/neighbors.h"
#include "02b-MAChigh/sixtop.h"
#include "02b-MAChigh/msf.h"
#ifdef MODULE_OPENWSN_IPV6
#include "03b-IPv6/icmpv6rpl.h"
#endif
#ifdef MODULE_OPENWSN_CJOIN
#include "cjoin.h"
#endif

extern idmanager_vars_t idmanager_vars;
extern neighbors_vars_t neighbors_vars;
extern openqueue_vars_t openqueue_vars;
extern schedule_vars_t schedule_vars;
extern scheduler_dbg_t scheduler_dbg;

#ifdef MODULE_OPENWSN_IPV6
extern icmpv6rpl_vars_t icmpv6rpl_vars;
#endif

char addr_str[IPV6_ADDR_MAX_STR_LEN];

static const struct {
    char *name;
    int id;
} components[] = {
    { "?", COMPONENT_NULL },
    { "openwsn", COMPONENT_OPENWSN },
    { "idmanager", COMPONENT_IDMANAGER },
    { "openqueue", COMPONENT_OPENQUEUE },
    { "openserial", COMPONENT_OPENSERIAL },
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
    { "icmpv6echo", COMPONENT_ICMPv6ECHO },
    { "icmpv6router", COMPONENT_ICMPv6ROUTER },
    { "icmpv6rpl", COMPONENT_ICMPv6RPL },
    { "udp", COMPONENT_OPENUDP },
    { "coap", COMPONENT_OPENCOAP },
    { "cjoin", COMPONENT_CJOIN },
    { "oscore", COMPONENT_OSCORE },
    { "c6t", COMPONENT_C6T },
    { "uinject", COMPONENT_UINJECT },
};

/* verbatim copy of gnrc_netif_addr_to_str*/
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

char *_get_component(int id)
{
    for (unsigned i = 0; i < ARRAY_SIZE(components); i++) {
        if (id == components[i].id) {
            return components[i].name;
        }
    }
    return NULL;
}

int _openwsn_ifconfig(char *arg)
{
    (void)arg;

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

#ifdef MODULE_OPENWSN_IPV6
    ipv6_addr_to_str(addr_str, (ipv6_addr_t *)temp_my128bID.addr_128b,
                     sizeof(addr_str));
    printf("\t\tinet6 addr: %s\n", addr_str);
    printf("\n");
#endif

    printf("\t\tIEEE802154E sync: %i\n", ieee154e_isSynch());
#ifdef MODULE_OPENWSN_CJOIN
    printf("\t\t6TiSCH joined: %i\n", cjoin_getIsJoined());
#endif
    printf("\n");

#ifdef MODULE_OPENWSN_IPV6
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
#endif

    return 0;
}

static int _neighbours_cmd(char *arg)
{
    (void)arg;

    for (int i = 0; i < MAXNUMNEIGHBORS; i++) {
        _array_2_string(neighbors_vars.neighbors[i].addr_64b.addr_64b, 8,
                        addr_str);
        if (memcmp(addr_str, "00:00:00:00:00:00:00:00", 8) != 0) {
            printf("%02i. %s\n", i, addr_str);
        }
    }
    return 0;
}

static void _print_cell_usage(void) {
    puts("Usage:");
    puts("\tcell list: show all active cell");
    puts("\tcell add <slot_offset> <channel_offset> <adv|tx|rx> [<address>]: add cell directly to schedule");
    puts("\tcell rmv <slot_offset> <channel_offset> <adv|tx|rx> [<address>]: remove cell directly in schedule ");
}

static int _cell_list_cmd(char *arg)
{
    (void)arg;

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

static int _cell_manage_cmd(int argc, char **argv)
{
    open_addr_t addr;
    memset(&addr, 0, sizeof(addr));
    int res;

    if(argc < 5) {
        _print_cell_usage();
        return -1;
    }

    if(argc == 6) {
        addr.type = ADDR_64B;
        fmt_hex_bytes(addr.addr_64b, argv[5]);
    }
    else {
        addr.type = ADDR_ANYCAST;
    }

    bool add = strcmp(argv[1], "add") == 0 ? true : false;

    cellType_t type;

    if(strcmp(argv[4], "adv") == 0) {
        type = CELLTYPE_TXRX;
    }
    else if(strcmp(argv[4], "tx") == 0) {
        type = CELLTYPE_TX;
    }
    else if(strcmp(argv[4], "rx") == 0) {
        type = CELLTYPE_RX;
    }
    else {
        _print_cell_usage();
        return -1;
    }

    if(add) { 
        res = schedule_addActiveSlot(atoi(argv[2]), type, true, false,
                                     atoi(argv[3]), &addr);
    }
    else {
        res = schedule_removeActiveSlot(atoi(argv[2]), type, true,
                                        &addr);
    }

    if(res == 0) {
        puts("Successfully set link");
        return 0;
    }
    else {
        puts("Something went wrong (duplicate link?)");
        return -1;
    }
}

static int _cell_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_cell_usage();
        return 1;
    }

    if (!strcmp(argv[1], "list")) {
        return _cell_list_cmd(NULL);
    }
    
    if (!strcmp(argv[1], "add") || !strcmp(argv[1], "rmv")) {
        return _cell_manage_cmd(argc, argv);
    }

    _print_cell_usage();
    return 1;
}

static void _print_6top_usage(void) {
    puts("Usage:");
    puts("\t6top clear [<neighbour>]: request neighbour to clear all cells");
    puts("\t6top add <num> <adv|tx|rx> [<neighbour>]: request parent to add num cells");
}

static int _6top_manage_cmd(int argc, char **argv)
{
    cellInfo_ht celllist_add[CELLLIST_MAX_LEN];
    cellInfo_ht celllist_delete[CELLLIST_MAX_LEN];
    open_addr_t neigh;
    uint8_t cell_options;
    uint8_t code = IANA_6TOP_CMD_NONE;
    uint8_t num;
    bool add;

    if(argc < 4) {
        _print_6top_usage();
        return -1;
    }

    if(argc == 5) {
        neigh.type = ADDR_64B;
        fmt_hex_bytes(neigh.addr_64b, argv[4]);
    }
    else {
        if(IS_USED(MODULE_OPENWSN_IPV6)){
            if (!icmpv6rpl_getPreferredParentEui64(&neigh)) {
                printf("Error: no preferred parent\n");
                return -1;
            }
            printf("Found preferred parent\n");
        }
        else {
            printf("Error: a neighbour address must be supplied\n");
            return -1;
        }
    }

    if(!strcmp(argv[3], "adv")) {
        cell_options = CELLOPTIONS_TX | CELLOPTIONS_RX | \
                       CELLOPTIONS_SHARED;
    }
    else if(!strcmp(argv[3], "tx")) {
        cell_options = CELLOPTIONS_TX;
    }
    else if(!strcmp(argv[3], "rx")) {
        cell_options = CELLOPTIONS_RX;
    }
    else {
        _print_6top_usage();
        return -1;
    }

    num = atoi(argv[2]);
    add = strcmp(argv[1], "add") == 0 ? true : false;
    if (add) {
        code = IANA_6TOP_CMD_ADD;
        if (!msf_candidateAddCellList(celllist_add, num)) {
            printf("Error: can't add that many cells\n");
            return -1;
        }
    }
    else {
        code = IANA_6TOP_CMD_DELETE;
        if (!msf_candidateRemoveCellList(celllist_delete, &neigh, num,
                                         cell_options)) {
            puts("Error: can't remove the specifide cells");
            return -1;
        }
    }

    int ret = sixtop_request(code, &neigh, num, cell_options, celllist_add,
                             celllist_delete, IANA_6TISCH_SFID_MSF,
                             0, 0);
    if (ret) {
        puts("Error: 6top request failed");
        return 0;
    }
    else {
        puts("6top request succeeded");
        return -1;
    }
}

static int _6top_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_6top_usage();
        return 1;
    }

    if (!strcmp(argv[1], "clear")) {

        open_addr_t neigh;

        if(argc == 3) {
            neigh.type = ADDR_64B;
            fmt_hex_bytes(neigh.addr_64b, argv[2]);
        }
        else {
            if(IS_USED(MODULE_OPENWSN_IPV6)){
                if (!icmpv6rpl_getPreferredParentEui64(&neigh)) {
                    puts("Error: no preferred parent");
                    return -1;
                }
            }
            else {
                puts("Error: a neighbour address must be supplied");
                return -1;
            }
        }
        int ret = sixtop_request(IANA_6TOP_CMD_CLEAR, &neigh, 0, 0, NULL,
                                 NULL, IANA_6TISCH_SFID_MSF, 0, 0);
        if (ret) {
            puts("Error: 6top request failed");
            return 0;
        }
        else {
            puts("6top request succeeded");
            return -1;
        }
    }

    if (!strcmp(argv[1], "add") || !strcmp(argv[1], "rmv")) {
        return _6top_manage_cmd(argc, argv);
    }

    _print_6top_usage();
    return 1;
}

static void _print_queue_usage(void) {
    puts("Usage:");
    puts("\tqueue list: show all Openqueue entries");
    puts("\tqueue rmv <creator>: remove all entries from <creator> in queue");
}

static int _queue_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_queue_usage();
        return 1;
    }

    if (!strcmp(argv[1], "list")) {
        bool queue = 1;

        for (uint8_t i = 0; i < QUEUELENGTH; i++) {
            if (openqueue_vars.queue[i].creator || openqueue_vars.queue[i].owner) {
                queue = 0;
                uint8_t creator = openqueue_vars.queue[i].creator;
                uint8_t owner = openqueue_vars.queue[i].owner;
                printf("Creator: %.9s [%d], ", _get_component(creator), creator);
                printf("Owner: %.9s [%d]\n", _get_component(owner), owner);
            }
        }
        if (queue) {
            puts("openqueue empty");
        }
        return 0;
    }

    if (!strcmp(argv[1], "rmv")) {
        if (argc != 3) {
            _print_queue_usage();
            return -1;
        }

        uint8_t creator = atoi(argv[2]);

        if (creator == 0) {
            printf("Error: invalid input value\n");
            return 1;
        }
        else {
            printf("Removing entries created by: %.9s [%d]\n", \
                   _get_component(creator), creator);
            openqueue_removeAllCreatedBy(creator);
            return 0;
        }
    }

    _print_queue_usage();
    return 1;
}

static int _scheduler_cmd(char *arg)
{
    (void) arg;

    printf("Current tasks:%i\n", scheduler_dbg.numTasksCur);
    printf("Max tasks:    %i\n", scheduler_dbg.numTasksMax);

    return 0;
}

static void _print_usage(void) {
    puts("Usage:");
    puts("\topenwsn neigh: show neighbor table");
    puts("\topenwsn queue: Openqueue management commands");
    puts("\topenwsn cell: cell management commands (directly on schedule)");
    puts("\topenwsn 6top: 6top request commands");
    puts("\topenwsn sched: show openos scheduler information");
}

int _openwsn_handler(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage();
        return 1;
    }

    if (!strcmp(argv[1], "neigh")) {
        return _neighbours_cmd(NULL);
    }

    if (!strcmp(argv[1], "queue")) {
        return _queue_cmd(argc - 1, &argv[1]);
    }

    if (!strcmp(argv[1], "cell")) {
        if (ieee154e_isSynch() == FALSE) {
            puts("Error: node is not synched");
            return -1;
        }
        return _cell_cmd(argc - 1, &argv[1]);
    }

    if (!strcmp(argv[1], "sched")) {
        return _scheduler_cmd(NULL);
    }

    if (!strcmp(argv[1], "6top")) {
        if (ieee154e_isSynch() == FALSE) {
            puts("Error: node is not synched");
            return -1;
        }
        return _6top_cmd(argc - 1, &argv[1]);
    }

    _print_usage();
    return 1;
}
