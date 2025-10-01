/*
 * Copyright (C) 2018       HAW Hamburg
 * Copyright (C) 2015–2017  Cenk Gündoğan <mail-github@cgundogan.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#include <string.h>
#include <stdio.h>

#include "net/gnrc/netif.h"
#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/dodag.h"
#include "net/gnrc/rpl/structs.h"
#include "shell.h"
#include "trickle.h"
#include "utlist.h"
#ifdef MODULE_GNRC_RPL_P2P
#include "net/gnrc/rpl/p2p.h"
#include "net/gnrc/rpl/p2p_dodag.h"
#include "net/gnrc/rpl/p2p_structs.h"
#endif

int _gnrc_rpl_init(char *arg)
{
    kernel_pid_t iface_pid = atoi(arg);
    if (gnrc_netif_get_by_pid(iface_pid) == NULL) {
        printf("unknown interface specified\n");
        return 1;
    }

    gnrc_rpl_init(iface_pid);
    printf("successfully initialized RPL on interface %d\n", iface_pid);
    return 0;
}

int _gnrc_rpl_dodag_root(char *arg1, char *arg2)
{
    uint8_t instance_id = atoi(arg1);
    ipv6_addr_t dodag_id;

    if (ipv6_addr_from_str(&dodag_id, arg2) == NULL) {
        printf("error: <dodag_id> must be a valid IPv6 address\n");
        return 1;
    }

    gnrc_rpl_instance_t *inst = gnrc_rpl_root_init(instance_id, &dodag_id, false, false);
    if (inst == NULL) {
        char addr_str[IPV6_ADDR_MAX_STR_LEN];
        printf("error: could not add DODAG (%s) to instance (%d)\n",
                ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)), instance_id);
        return 1;
    }

    printf("successfully added a new RPL DODAG\n");
    return 0;
}

#ifdef MODULE_GNRC_RPL_P2P
int _gnrc_rpl_find(char *arg1, char *arg2)
{
    uint8_t instance_id = atoi(arg1);
    ipv6_addr_t dodag_id;
    ipv6_addr_t target;

    if (ipv6_addr_from_str(&dodag_id, arg1) == NULL) {
        printf("<dodag_id> must be a valid IPv6 address\n");
        return 1;
    }

    if (ipv6_addr_from_str(&target, arg2) == NULL) {
        printf("<target> must be a valid IPv6 address\n");
        return 1;
    }

    if (gnrc_rpl_p2p_root_init(0, &dodag_id, &target, true) == NULL) {
        char addr_str[IPV6_ADDR_MAX_STR_LEN];
        printf("error: could not add DODAG (%s) to instance (%d)\n",
                ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)), instance_id);
        return 1;
    }

    printf("successfully initiated a P2P-RPL Route Discovery\n");
    return 0;
}
#endif

int _gnrc_rpl_instance_remove(char *arg1)
{
    uint8_t instance_id = atoi(arg1);
    gnrc_rpl_instance_t *inst;

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        printf("error: could not find the instance (%d)\n", instance_id);
        return 1;
    }

    gnrc_rpl_instance_remove(inst);

    printf("success: removed instance (%d)\n", instance_id);
    return 0;
}

int _gnrc_rpl_trickle_reset(char *arg1)
{
    uint8_t instance_id = atoi(arg1);
    gnrc_rpl_instance_t *inst;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        printf("error: could not find the <instance_id>\n");
        return 1;
    }

    trickle_reset_timer(&(inst->dodag.trickle));

    printf("success: reset trickle timer of DODAG (%s) from instance (%d)\n",
            ipv6_addr_to_str(addr_str, &(inst->dodag.dodag_id), sizeof(addr_str)),
            instance_id);
    return 0;
}

int _gnrc_rpl_trickle_stop(char *arg1)
{
    uint8_t instance_id = atoi(arg1);
    gnrc_rpl_instance_t *inst;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        printf("error: could not find the <instance_id>\n");
        return 1;
    }
    trickle_stop(&(inst->dodag.trickle));

    printf("success: stopped trickle timer of DODAG (%s) from instance (%d)\n",
            ipv6_addr_to_str(addr_str, &(inst->dodag.dodag_id), sizeof(addr_str)),
            instance_id);
    return 0;
}

int _gnrc_rpl_trickle_start(char *arg1)
{
    uint8_t instance_id = atoi(arg1);
    gnrc_rpl_instance_t *inst;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        printf("error: could not find the <instance_id>\n");
        return 1;
    }

    trickle_start(gnrc_rpl_pid, &(inst->dodag.trickle), GNRC_RPL_MSG_TYPE_TRICKLE_MSG,
                  (1 << inst->dodag.dio_min), inst->dodag.dio_interval_doubl,
                  inst->dodag.dio_redun);

    printf("success: started trickle timer of DODAG (%s) from instance (%d)\n",
            ipv6_addr_to_str(addr_str, &(inst->dodag.dodag_id), sizeof(addr_str)),
            instance_id);
    return 0;
}

int _gnrc_rpl_send_dis_w_sol_opt(char* VID, char* version, char* instance, char* dodag)
{
    uint8_t VID_flags = atoi(VID);
    uint8_t version_number = atoi(version);
    uint8_t instance_id = atoi(instance);

    gnrc_rpl_internal_opt_dis_solicited_t sol;
    sol.type = GNRC_RPL_OPT_SOLICITED_INFO;
    sol.length = GNRC_RPL_DIS_SOLICITED_INFO_LENGTH;
    sol.VID_flags = htons(VID_flags);
    sol.version_number = version_number;
    sol.instance_id = instance_id;

    if (ipv6_addr_from_str(&sol.dodag_id, dodag))
    {
        gnrc_rpl_internal_opt_t* opt[] = {(gnrc_rpl_internal_opt_t*)&sol};
        gnrc_rpl_send_DIS(NULL, (ipv6_addr_t *) &ipv6_addr_all_rpl_nodes, opt, 1);
        printf("success: send a DIS with SOL option\n\n");
    }
    return 0;
}

int _gnrc_rpl_send_dis(void)
{
    gnrc_rpl_send_DIS(NULL, (ipv6_addr_t *) &ipv6_addr_all_rpl_nodes, NULL, 0);

    printf("success: send a DIS\n\n");
    return 0;
}

#ifdef MODULE_NETSTATS_RPL
static void _print_stats_block(netstats_rpl_block_t *block, const char *name)
{
    /* In the following we need to sync with the RPL thread via disabling IRQs
     * to avoid reading corrupted data. The simpler strategy would be to
     * disable IRQs during the whole printing (so in _stats()_, but stdio could
     * be via a slow UART and, hence, have severe impact on the real time
     * capabilities of the system. The second and simplest strategy would be to
     * memcpy the whole netstats_rpl_t on to the stack with IRQs disabled and
     * print the stack copy with IRQs re-enabled. However, that structure is
     * 128 B in size, so that we would easily provoke a stack-overflow.
     *
     * Our strategy instead is to read the data four values at a time with
     * IRQs disabled, and print them with IRQs re-enabled. The disadvantage is
     * that stats may get updated while printing one group of values. However,
     * the stats are grouped such that closely related values are read together.
     * Hence, related metrics will always refer to the same state of the stats.
     */
    unsigned irq_state = irq_disable();
    uint32_t rx_ucast = block->rx_ucast_count;
    uint32_t tx_ucast = block->tx_ucast_count;
    uint32_t rx_mcast = block->rx_mcast_count;
    uint32_t tx_mcast = block->tx_mcast_count;
    irq_restore(irq_state);
    printf("%7s #packets: %10" PRIu32 " / %-10" PRIu32 "  %10" PRIu32 " / %-10" PRIu32 "\n",
           name, rx_ucast, tx_ucast, rx_mcast, tx_mcast);

    irq_state = irq_disable();
    rx_ucast = block->rx_ucast_bytes;
    tx_ucast = block->tx_ucast_bytes;
    rx_mcast = block->rx_mcast_bytes;
    tx_mcast = block->tx_mcast_bytes;
    irq_restore(irq_state);
    printf("%7s   #bytes: %10" PRIu32 " / %-10" PRIu32 "  %10" PRIu32 " / %-10" PRIu32 "\n",
           name, rx_ucast, tx_ucast, rx_mcast, tx_mcast);
}

int _stats(void)
{
   printf(  "Statistics        (ucast) RX / TX                  RX / TX (mcast)\n");
    _print_stats_block(&gnrc_rpl_netstats.dio, "DIO");
    _print_stats_block(&gnrc_rpl_netstats.dis, "DIS");
    _print_stats_block(&gnrc_rpl_netstats.dao, "DAO");
    _print_stats_block(&gnrc_rpl_netstats.dao_ack, "DAO-ACK");
    return 0;
}
#endif

int _gnrc_rpl_dodag_show(void)
{
    if (gnrc_rpl_pid == KERNEL_PID_UNDEF) {
        printf("RPL not initializied\n");
        return 1;
    }

    printf("instance table:\t");
    for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        if (gnrc_rpl_instances[i].state == 0) {
            printf("[ ]");
        }
        else {
            printf("[X]");
        }
        putchar('\t');
    }

    putchar('\n');

    printf("parent table:\t");
    for (uint8_t i = 0; i < GNRC_RPL_PARENTS_NUMOF; ++i) {
        if (gnrc_rpl_parents[i].state == 0) {
            printf("[ ]");
        }
        else {
            printf("[X]");
        }
        putchar('\t');
    }

    putchar('\n');

#ifdef MODULE_GNRC_RPL_P2P
    printf("p2p-rpl table:\t");
    for (int8_t i = 0; i < GNRC_RPL_P2P_EXTS_NUMOF; ++i) {
        if (gnrc_rpl_p2p_exts[i].state == 0) {
            printf("[ ]");
        }
        else {
            printf("[X]");
        }
        putchar('\t');
    }
#endif

    putchar('\n');

    gnrc_rpl_dodag_t *dodag = NULL;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        if (gnrc_rpl_instances[i].state == 0) {
            continue;
        }

        dodag = &gnrc_rpl_instances[i].dodag;

        printf("instance [%d | Iface: %" PRIkernel_pid " | mop: %d | ocp: %d | mhri: %d | mri %d]\n",
                gnrc_rpl_instances[i].id, dodag->iface,
                gnrc_rpl_instances[i].mop, gnrc_rpl_instances[i].of->ocp,
                gnrc_rpl_instances[i].min_hop_rank_inc, gnrc_rpl_instances[i].max_rank_inc);

        printf("\tdodag [%s | R: %d | OP: %s | PIO: %s | "
               "TR(I=[%d,%d], k=%d, c=%d)]\n",
               ipv6_addr_to_str(addr_str, &dodag->dodag_id, sizeof(addr_str)),
               dodag->my_rank, (dodag->node_status == GNRC_RPL_LEAF_NODE ? "Leaf" : "Router"),
               ((dodag->dio_opts & GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO) ? "on" : "off"),
               (1 << dodag->dio_min), dodag->dio_interval_doubl, dodag->trickle.k,
               dodag->trickle.c);

#ifdef MODULE_GNRC_RPL_P2P
        if (dodag->instance->mop == GNRC_RPL_P2P_MOP) {
            gnrc_rpl_p2p_ext_t *p2p_ext = gnrc_rpl_p2p_ext_get(dodag);
            printf("\tP2P-Ext [%s | HBH: %s | R: %s | L: %us]\n",
                    ipv6_addr_to_str(addr_str, &p2p_ext->target, sizeof(addr_str)),
                    p2p_ext->hop_by_hop ? "True" : "False",
                    p2p_ext->reply ? "True" : "False", p2p_ext->lifetime_sec);
        }
#endif

        gnrc_rpl_parent_t *parent = NULL;
        LL_FOREACH(gnrc_rpl_instances[i].dodag.parents, parent) {
            printf("\t\tparent [addr: %s | rank: %d]\n",
                    ipv6_addr_to_str(addr_str, &parent->addr, sizeof(addr_str)),
                    parent->rank);
        }
    }
    return 0;
}

int _gnrc_rpl_operation(bool leaf, char *arg1)
{
    uint8_t instance_id = atoi(arg1);
    gnrc_rpl_instance_t *inst;

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        printf("error: could not find the instance (%d)\n", instance_id);
        return 1;
    }

    if (leaf) {
        gnrc_rpl_leaf_operation(&inst->dodag);
    }
    else {
        gnrc_rpl_router_operation(&inst->dodag);
    }

    printf("success: operate in instance (%d) as %s\n", instance_id, leaf ? "leaf" : "router");
    return 0;
}

int _gnrc_rpl_set_pio(char *inst_id, bool status)
{
    uint8_t instance_id = atoi(inst_id);
    gnrc_rpl_instance_t *inst;

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        printf("error: could not find the instance (%d)\n", instance_id);
        return 1;
    }

    gnrc_rpl_config_pio(&inst->dodag, status);

    printf("success: %sactivated PIO transmissions\n", status ? "" : "de");
    return 0;
}

static int _gnrc_rpl(int argc, char **argv)
{
    if ((argc < 2) || (strcmp(argv[1], "show") == 0)) {
        return _gnrc_rpl_dodag_show();
    }
    else if ((argc == 3) && strcmp(argv[1], "init") == 0) {
        return _gnrc_rpl_init(argv[2]);
    }
    else if ((argc == 4) && strcmp(argv[1], "root") == 0) {
        return _gnrc_rpl_dodag_root(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "rm") == 0) {
        if (argc == 3) {
            return _gnrc_rpl_instance_remove(argv[2]);
        }
    }
    else if (strcmp(argv[1], "trickle") == 0) {
        if ((argc == 4) && (strcmp(argv[2], "reset") == 0)) {
            return _gnrc_rpl_trickle_reset(argv[3]);
        }
        else if ((argc == 4) && (strcmp(argv[2], "stop") == 0)) {
            return _gnrc_rpl_trickle_stop(argv[3]);
        }
        else if ((argc == 4) && (strcmp(argv[2], "start") == 0)) {
            return _gnrc_rpl_trickle_start(argv[3]);
        }
    }
    else if (strcmp(argv[1], "send") == 0) {
        if ((argc == 3) && (strcmp(argv[2], "dis") == 0)) {
            return _gnrc_rpl_send_dis();
        }
        if ((argc == 7) && (strcmp(argv[2], "dis") == 0)) {
            return _gnrc_rpl_send_dis_w_sol_opt(argv[3], argv[4], argv[5], argv[6]);
        }
    }
    else if (strcmp(argv[1], "leaf") == 0) {
        if (argc == 3) {
            return _gnrc_rpl_operation(true, argv[2]);
        }
    }
    else if (strcmp(argv[1], "router") == 0) {
        if (argc == 3) {
            return _gnrc_rpl_operation(false, argv[2]);
        }
    }
    else if (strcmp(argv[1], "set") == 0) {
        if ((argc > 2) && !IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_PIO)) {
            if (strcmp(argv[2], "pio") == 0) {
                if ((argc == 5) && (strcmp(argv[3], "on") == 0)) {
                    return _gnrc_rpl_set_pio(argv[4], true);
                }
                else if ((argc == 5) && (strcmp(argv[3], "off") == 0)) {
                    return _gnrc_rpl_set_pio(argv[4], false);
                }
            }
        }
    }
#ifdef MODULE_GNRC_RPL_P2P
    else if (strcmp(argv[1], "find") == 0) {
        if (argc == 4) {
            return _gnrc_rpl_find(argv[2], argv[3]);
        }
    }
#endif
#ifdef MODULE_NETSTATS_RPL
    else if (strcmp(argv[1], "stats") == 0) {
        return _stats();
    }
#endif

#ifdef MODULE_GNRC_RPL_P2P
    printf("* find <dodag_id> <target>\t\t\t- initiate a P2P-RPL route discovery\n");
#endif
    printf("* help\t\t\t\t\t- show usage\n");
    printf("* init <if_id>\t\t\t\t- initialize RPL on the given interface\n");
    printf("* leaf <instance_id>\t\t\t- operate as leaf in the instance\n");
    printf("* trickle reset <instance_id>\t\t- reset the trickle timer\n");
    printf("* trickle start <instance_id>\t\t- start the trickle timer\n");
    printf("* trickle stop <instance_id>\t\t- stop the trickle timer\n");
    printf("* rm <instance_id>\t\t\t- delete the given instance and related dodag\n");
    printf("* root <inst_id> <dodag_id>\t\t- add a dodag to a new or existing instance\n");
    printf("* router <instance_id>\t\t\t- operate as router in the instance\n");
    printf("* send dis\t\t\t\t- send a multicast DIS\n");
    printf("* send dis <VID_flags> <version> <instance_id> <dodag_id> - send a multicast DIS with SOL option\n");

    if (!IS_ACTIVE(CONFIG_GNRC_RPL_WITHOUT_PIO)) {
        printf("* set pio <on/off> <instance_id>\t- (de-)activate PIO transmissions in DIOs\n");
    }

    printf("* show\t\t\t\t\t- show instance and dodag tables\n");
    return 0;
}

SHELL_COMMAND(rpl, "rpl configuration tool ('rpl help' for more information)", _gnrc_rpl);
/**
 * @}
 */
