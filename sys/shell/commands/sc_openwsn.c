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
#include "net/ieee802154.h"
#include "net/ipv6/addr.h"
#include "net/l2util.h"
#include "net/netif.h"

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
#include "03b-IPv6/icmpv6rpl.h"
#include "cjoin.h"

#define IEEE802154_LONG_ADDRESS_LEN_STR_MAX \
    (sizeof("00:00:00:00:00:00:00:00"))

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
    { "udp", COMPONENT_UDP },
    { "sock-udp", COMPONENT_SOCK_TO_UDP },
    { "udp-sock", COMPONENT_UDP_TO_SOCK },
    { "coap", COMPONENT_OPENCOAP },
    { "cjoin", COMPONENT_CJOIN },
    { "oscore", COMPONENT_OSCORE },
    { "c6t", COMPONENT_C6T },
    { "uinject", COMPONENT_UINJECT },
};

static char *_get_component(int id)
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

    open_addr_t *addr;
    /* Use shared buffer for IEEE802154_LONG_ADDRES and IPV6_ADDR str
       conversion */
#if IS_USED(MODULE_OPENWSN_IPV6)
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
#else
    char addr_str[IEEE802154_LONG_ADDRESS_LEN_STR_MAX];
#endif

    printf("Iface  %d  ", openwsn_get_pid());

    addr = idmanager_getMyID(ADDR_16B);
    printf("\tHWaddr: %s  ",
           l2util_addr_to_str(addr->addr_16b, sizeof(addr->addr_16b),
                             addr_str));

    addr = idmanager_getMyID(ADDR_PANID);
    printf("NID: %s\n\n",
           l2util_addr_to_str(addr->panid, sizeof(addr->panid),
                             addr_str));

    addr = idmanager_getMyID(ADDR_64B);
    printf("\t\tLong HWaddr: %s\n",
           l2util_addr_to_str(addr->addr_64b, sizeof(addr->addr_64b),
                             addr_str));

    if (IS_USED(MODULE_OPENWSN_IPV6)) {
        /* Can't recover ADDR_128B directly, recover ADDR_64B
           and ADDR_PREFIX to construct ADDR_128B */
        open_addr_t addr_ipv6;
        addr = idmanager_getMyID(ADDR_64B);
        memcpy(&addr_ipv6.addr_128b[8], addr->addr_64b,
               sizeof(addr->addr_64b));
        addr = idmanager_getMyID(ADDR_PREFIX);
        memcpy(&addr_ipv6.addr_128b[0], addr->prefix,
               sizeof(addr->prefix));
        ipv6_addr_to_str(addr_str, (ipv6_addr_t *)addr_ipv6.addr_128b,
                         IPV6_ADDR_MAX_STR_LEN);
        printf("\t\tinet6 addr: %s\n\n", addr_str);
    }

    printf("\t\tIEEE802154E sync: %i\n", ieee154e_isSynch());
    if (IS_USED(MODULE_OPENWSN_CJOIN)) {
        printf("\t\t6TiSCH joined: %i\n", cjoin_getIsJoined());
    }
    puts("");

    if (IS_USED(MODULE_OPENWSN_IPV6)) {
        uint8_t index;

        if (icmpv6rpl_getPreferredParentIndex(
                &index) || idmanager_getIsDAGroot()) {
            open_addr_t neighbor;
            uint8_t addr_ipv6[IPV6_ADDR_BIT_LEN / 8];
            printf("\t\tRPL rank: %i\n", icmpv6rpl_getMyDAGrank());
            if (idmanager_getIsDAGroot()) {
                puts("\t\tRPL parent: Node is DAG root");
            }
            else {
                icmpv6rpl_getPreferredParentEui64(&neighbor);
                printf("\t\tRPL parent: %s\n",
                       l2util_addr_to_str(neighbor.addr_64b,
                                         sizeof(neighbor.addr_64b),
                                         addr_str));
            }
            printf("\t\tRPL children:\n");
            for (uint8_t i = 0; i < MAXNUMNEIGHBORS; i++) {
                if (neighbors_isNeighborWithHigherDAGrank(i)) {
                    neighbors_getNeighborEui64(&neighbor, ADDR_64B, i);
                    printf("\t\t\t%s\n",
                           l2util_addr_to_str(neighbor.addr_64b,
                                             sizeof(neighbor.addr_64b),
                                             addr_str));
                }
            }

            icmpv6rpl_getRPLDODAGid(addr_ipv6);
            ipv6_addr_to_str(addr_str, (ipv6_addr_t *)addr_ipv6,
                             IPV6_ADDR_MAX_STR_LEN);
            printf("\t\tRPL DODAG ID: %16s\n", addr_str);
        }
        else {
            puts("\t\tNO RPL parent");
        }
    }

    return 0;
}

static int _neighbors_cmd(char *arg)
{
    (void)arg;

    char hwaddr_str[IEEE802154_LONG_ADDRESS_LEN_STR_MAX];
    open_addr_t neighbor;

    for (int i = 0; i < MAXNUMNEIGHBORS; i++) {
        neighbors_getNeighborEui64(&neighbor, ADDR_64B, i);
        l2util_addr_to_str(neighbor.addr_64b, sizeof(neighbor.addr_64b),
                          hwaddr_str);
        if (memcmp(hwaddr_str, "00:00:00:00:00:00:00:00",
                   IEEE802154_LONG_ADDRESS_LEN_STR_MAX)) {
            printf("%02i. %s\n", i, hwaddr_str);
        }
    }

    return 0;
}

static void _print_cell_usage(void)
{
    puts("Usage:");
    puts("\tcell list: show all active cell");
    puts("\tcell add <slot_offset> <channel_offset> <adv|tx|rx>"
         " [<address>]: add cell directly to schedule");
    puts("\tcell rmv <slot_offset> <channel_offset> <adv|tx|rx>"
         " [<address>]: remove cell directly in schedule ");
}

static int _cell_list_cmd(char *arg)
{
    (void)arg;

    extern schedule_vars_t schedule_vars;
    char hwaddr_str[IEEE802154_LONG_ADDRESS_LEN_STR_MAX];

    for (int i = 0; i < MAXACTIVESLOTS; i++) {
        switch (schedule_vars.scheduleBuf[i].type) {
        case CELLTYPE_TX:
            printf("neigh: %s, slot: %03i, chan: %02i, type: TX\n",
                   l2util_addr_to_str(
                       schedule_vars.scheduleBuf[i].neighbor.addr_64b,
                       IEEE802154_LONG_ADDRESS_LEN, hwaddr_str),
                   schedule_vars.scheduleBuf[i].slotOffset,
                   schedule_vars.scheduleBuf[i].channelOffset);
            break;
        case CELLTYPE_RX:
            printf("slot: %03i, chan: %02i, type: RX\n",
                   schedule_vars.scheduleBuf[i].slotOffset,
                   schedule_vars.scheduleBuf[i].channelOffset);
            break;
        case CELLTYPE_TXRX:
            printf("neigh: %s, slot: %03i, chan: %02i, type: RXTX\n",
                   l2util_addr_to_str(
                       schedule_vars.scheduleBuf[i].neighbor.addr_64b,
                       IEEE802154_LONG_ADDRESS_LEN, hwaddr_str),
                   schedule_vars.scheduleBuf[i].slotOffset,
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
    cellType_t type;
    int res;

    memset(&addr, 0, sizeof(addr));

    if (argc < 5) {
        _print_cell_usage();
        return -1;
    }

    if (argc == 6) {
        addr.type = ADDR_64B;
        size_t len = l2util_addr_from_str(argv[5], addr.addr_64b);
        if (len == 0) {
            puts("Error: invalid address");
            return -1;
        }
    }
    else {
        addr.type = ADDR_ANYCAST;
    }

    if (!strcmp(argv[4], "adv")) {
        type = CELLTYPE_TXRX;
    }
    else if (!strcmp(argv[4], "tx")) {
        type = CELLTYPE_TX;
    }
    else if (!strcmp(argv[4], "rx")) {
        type = CELLTYPE_RX;
    }
    else {
        _print_cell_usage();
        return -1;
    }

    if (!strcmp(argv[1], "add")) {
        res =
            schedule_addActiveSlot(atoi(argv[2]), type, true, false,
                                   atoi(argv[3]), &addr);
    }
    else {
        res = schedule_removeActiveSlot(atoi(argv[2]), type, true, &addr);
    }

    if (res == 0) {
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
        return -1;
    }

    if (!strcmp(argv[1], "list")) {
        return _cell_list_cmd(NULL);
    }

    if (!strcmp(argv[1], "add") || !strcmp(argv[1], "rmv")) {
        return _cell_manage_cmd(argc, argv);
    }

    _print_cell_usage();
    return -1;
}

static void _print_6top_usage(void)
{
    puts("Usage:");
    puts("\t6top clear [<neighbor>]:"
         " request neighbor to clear all cells");
    puts("\t6top add <num> <adv|tx|rx> [<neighbor>]:"
         " request parent to add num cells");
    puts("\t6top rmv <num> <adv|tx|rx> [<neighbor>]:"
         " request parent to remove num cells");
    puts("\t6top rel <num> <adv|tx|rx> [<neighbor>]:"
         " request parent to relocate num cells");
}

static int _6top_manage_cmd(int argc, char **argv)
{
    cellInfo_ht cells_add[CELLLIST_MAX_LEN];
    cellInfo_ht cells_rmv[CELLLIST_MAX_LEN];
    open_addr_t neigh;
    uint8_t cell_options;
    uint8_t code = IANA_6TOP_CMD_NONE;
    uint8_t num;

    if (argc < 4) {
        puts("Error: not enough args");
        _print_6top_usage();
        return -1;
    }

    if (argc == 5) {
        neigh.type = ADDR_64B;
        size_t len = l2util_addr_from_str(argv[4], neigh.addr_64b);
        if (len == 0) {
            puts("Error: invalid address");
            return -1;
        }
    }
    else {
        if (IS_USED(MODULE_OPENWSN_IPV6)) {
            if (!icmpv6rpl_getPreferredParentEui64(&neigh)) {
                puts("Error: no preferred parent");
                return -1;
            }
        }
        else {
            puts("Error: a neighbor address must be supplied");
            return -1;
        }
    }

    if (!strcmp(argv[3], "adv")) {
        cell_options = CELLOPTIONS_TX | CELLOPTIONS_RX | CELLOPTIONS_SHARED;
    }
    else if (!strcmp(argv[3], "tx")) {
        cell_options = CELLOPTIONS_TX;
    }
    else if (!strcmp(argv[3], "rx")) {
        cell_options = CELLOPTIONS_RX;
    }
    else {
        puts("Error: invalid cell option");
        return -1;
    }

    num = atoi(argv[2]);

    if (!strcmp(argv[1], "add")) {
        code = IANA_6TOP_CMD_ADD;
        if (!msf_candidateAddCellList(cells_add, num)) {
            puts("Error: can't add that many cells");
            return -1;
        }
    }
    else if (!strcmp(argv[1], "rmv")) {
        code = IANA_6TOP_CMD_DELETE;
        if (!msf_candidateRemoveCellList(cells_rmv, &neigh, num,
                                         cell_options)) {
            puts("Error: can't remove the specified cells");
            return -1;
        }
    }
    else if (!strcmp(argv[1], "rel")) {
        code = IANA_6TOP_CMD_RELOCATE;
        if (!schedule_getCellsToBeRelocated(&neigh, cells_rmv) ||
            !msf_candidateAddCellList(cells_add, num)) {
            puts("Error: failed to get cells to relocate");
            return -1;
        }
    }
    else {
        puts("Error: unknown 6top command");
        return -1;
    }

    int ret = sixtop_request(code, &neigh, num, cell_options, cells_add,
                             cells_rmv, IANA_6TISCH_SFID_MSF, 0, 0);

    if (ret) {
        puts("Error: 6top request failed");
        return -1;
    }

    return 0;
}

static int _6top_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_6top_usage();
        return -1;
    }

    if (!strcmp(argv[1], "clear")) {

        open_addr_t neighbor;

        if (argc == 3) {
            neighbor.type = ADDR_64B;
            size_t len = l2util_addr_from_str(argv[2], neighbor.addr_64b);
            if (len == 0) {
                puts("Error: invalid address");
                return -1;
            }
        }
        else {
            if (IS_USED(MODULE_OPENWSN_IPV6)) {
                if (!icmpv6rpl_getPreferredParentEui64(&neighbor)) {
                    puts("Error: no preferred parent");
                    return -1;
                }
            }
            else {
                puts("Error: a neighbor address must be supplied");
                return -1;
            }
        }
        int ret = sixtop_request(IANA_6TOP_CMD_CLEAR, &neighbor, 0, 0,
                                 NULL, NULL, IANA_6TISCH_SFID_MSF, 0, 0);
        if (ret) {
            puts("Error: 6top request failed");
            return -1;
        }
    }

    if (!strcmp(argv[1], "add") || !strcmp(argv[1], "rmv") ||
        !strcmp(argv[1], "rel")) {
        return _6top_manage_cmd(argc, argv);
    }

    _print_6top_usage();
    return -1;
}

static void _print_queue_usage(void)
{
    puts("Usage:");
    puts("\tqueue list: show all Openqueue entries");
    puts("\tqueue rmv <creator>: remove all entries"
         " from <creator> in queue");
}

static int _queue_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_queue_usage();
        return -1;
    }

    if (!strcmp(argv[1], "list")) {
        extern openqueue_vars_t openqueue_vars;
        bool empty_queue = true;

        for (uint8_t i = 0; i < QUEUELENGTH; i++) {
            if (openqueue_vars.queue[i].creator ||
                openqueue_vars.queue[i].owner) {
                uint8_t creator = openqueue_vars.queue[i].creator;
                uint8_t owner = openqueue_vars.queue[i].owner;
                printf("Creator: %.9s [%d], ",
                       _get_component(creator), creator);
                printf("Owner: %.9s [%d]\n",
                       _get_component(owner), owner);
                empty_queue = false;
            }
        }
        if (empty_queue) {
            puts("Queue is empty");
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
            puts("Error: invalid input value\n");
            return -1;
        }
        else {
            printf("Removing entries created by: %.9s [%d]\n",
                   _get_component(creator), creator);
            openqueue_removeAllCreatedBy(creator);
            return 0;
        }
    }

    _print_queue_usage();
    return -1;
}

#if SCHEDULER_DEBUG_ENABLE
static int _scheduler_cmd(char *arg)
{
    (void)arg;
    extern scheduler_dbg_t scheduler_dbg;

    printf("Current tasks: %i\n", scheduler_dbg.numTasksCur);
    printf("Max tasks:     %i\n", scheduler_dbg.numTasksMax);

    return 0;
}
#endif

static void _print_usage(void)
{
    puts("Usage:");
    puts("\topenwsn neigh: show neighbor table");
    puts("\topenwsn queue: Openqueue management commands");
    puts("\topenwsn cell: cell management commands");
    puts("\topenwsn 6top: 6top request commands");
#if SCHEDULER_DEBUG_ENABLE
    puts("\topenwsn sched: show openos scheduler information");
#endif
}

int _openwsn_handler(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage();
        return -1;
    }

    if (!strcmp(argv[1], "neigh")) {
        return _neighbors_cmd(NULL);
    }

    if (!strcmp(argv[1], "queue")) {
        return _queue_cmd(argc - 1, &argv[1]);
    }

    if (!strcmp(argv[1], "cell")) {
        if (!ieee154e_isSynch()) {
            puts("Error: node is not synchronized");
            return -1;
        }
        return _cell_cmd(argc - 1, &argv[1]);
    }

#if SCHEDULER_DEBUG_ENABLE
    if (!strcmp(argv[1], "sched")) {
        return _scheduler_cmd(NULL);
    }
#endif

    if (!strcmp(argv[1], "6top")) {
        if (!ieee154e_isSynch()) {
            puts("Error: node is not synchronized");
            return -1;
        }
        return _6top_cmd(argc - 1, &argv[1]);
    }

    _print_usage();
    return -1;
}
