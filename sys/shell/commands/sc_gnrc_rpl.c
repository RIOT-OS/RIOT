/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sys_shell_commands.h
 * @{
 *
 * @file
 *
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include <string.h>
#include <stdio.h>
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/rpl/dodag.h"
#include "utlist.h"
#include "trickle.h"

int _gnrc_rpl_init(char *arg)
{
    gnrc_ipv6_netif_t *entry = NULL;
    kernel_pid_t iface_pid = (kernel_pid_t) atoi(arg);
    entry = gnrc_ipv6_netif_get(iface_pid);

    if (entry == NULL) {
        puts("unknown interface specified");
        return 1;
    }

    gnrc_rpl_init(iface_pid);
    printf("successfully initialized RPL on interface %d\n", iface_pid);
    return 0;
}

int _gnrc_rpl_dodag_root(char *arg1, char *arg2)
{
    uint8_t instance_id = (uint8_t) atoi(arg1);
    ipv6_addr_t dodag_id;

    if (ipv6_addr_from_str(&dodag_id, arg2) == NULL) {
        puts("<dodag_id> must be a valid IPv6 address");
        return 1;
    }

    gnrc_rpl_dodag_t *dodag = NULL;
    dodag = gnrc_rpl_root_init(instance_id, &dodag_id);
    if (dodag == NULL) {
        char addr_str[IPV6_ADDR_MAX_STR_LEN];
        printf("error: could not add DODAG (%s) to instance (%d)\n",
                ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)), instance_id);
        return 1;
    }

    printf("successfully added a new RPL DODAG\n");
    return 0;
}

int _gnrc_rpl_instance_remove(char *arg1)
{
    uint8_t instance_id = 0;
    gnrc_rpl_instance_t *inst;

    instance_id = (uint8_t) atoi(arg1);
    if (instance_id == 0) {
        puts("error: <instance_id> must be a positive number greater than zero");
        return 1;
    }

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        printf("error: could not find the instance (%d)\n", instance_id);
        return 1;
    }

    if (gnrc_rpl_instance_remove(inst) == false) {
        printf("error: could not remove instance (%d)\n", instance_id);
        return 1;
    }

    printf("success: removed instance (%d)\n", instance_id);
    return 0;
}

int _gnrc_rpl_dodag_remove(char *arg1, char *arg2)
{
    uint8_t instance_id = 0;
    ipv6_addr_t dodag_id;
    gnrc_rpl_instance_t *inst;
    gnrc_rpl_dodag_t *dodag = NULL;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    instance_id = (uint8_t) atoi(arg1);
    if (instance_id == 0) {
        puts("error: <instance_id> must be a positive number greater than zero");
        return 1;
    }

    if (ipv6_addr_from_str(&dodag_id, arg2) == NULL) {
        puts("error: <dodag_id> must be a valid IPv6 address");
        return 1;
    }

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        puts("error: could not find the <instance_id>");
        return 1;
    }

    if ((dodag = gnrc_rpl_dodag_get(inst, &dodag_id)) == NULL) {
        puts("error: <dodag_id> does not exist for the given <instance_id>");
        return 1;
    }

    if (gnrc_rpl_dodag_remove(dodag) == false) {
        printf("error: could not remove DODAG (%s) from instance (%d)\n",
                ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)),
                instance_id);
        return 1;
    }

    printf("success: removed DODAG (%s) from instance (%d)\n",
            ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)),
            instance_id);
    return 0;
}

int _gnrc_rpl_trickle_reset(char *arg1, char *arg2)
{
    uint8_t instance_id = 0;
    ipv6_addr_t dodag_id;
    gnrc_rpl_instance_t *inst;
    gnrc_rpl_dodag_t *dodag = NULL;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    instance_id = (uint8_t) atoi(arg1);
    if (instance_id == 0) {
        puts("error: <instance_id> must be a positive number greater than zero");
        return 1;
    }

    if (ipv6_addr_from_str(&dodag_id, arg2) == NULL) {
        puts("error: <dodag_id> must be a valid IPv6 address");
        return 1;
    }

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        puts("error: could not find the <instance_id>");
        return 1;
    }

    if ((dodag = gnrc_rpl_dodag_get(inst, &dodag_id)) == NULL) {
        puts("error: <dodag_id> does not exist for the given <instance_id>");
        return 1;
    }

    trickle_reset_timer(&dodag->trickle);

    printf("success: resetted trickle timer of DODAG (%s) from instance (%d)\n",
            ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)),
            instance_id);
    return 0;
}

int _gnrc_rpl_trickle_stop(char *arg1, char *arg2)
{
    uint8_t instance_id = 0;
    ipv6_addr_t dodag_id;
    gnrc_rpl_instance_t *inst;
    gnrc_rpl_dodag_t *dodag = NULL;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    instance_id = (uint8_t) atoi(arg1);
    if (instance_id == 0) {
        puts("error: <instance_id> must be a positive number greater than zero");
        return 1;
    }

    if (ipv6_addr_from_str(&dodag_id, arg2) == NULL) {
        puts("error: <dodag_id> must be a valid IPv6 address");
        return 1;
    }

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        puts("error: could not find the <instance_id>");
        return 1;
    }

    if ((dodag = gnrc_rpl_dodag_get(inst, &dodag_id)) == NULL) {
        puts("error: <dodag_id> does not exist for the given <instance_id>");
        return 1;
    }

    trickle_stop(&dodag->trickle);

    printf("success: stopped trickle timer of DODAG (%s) from instance (%d)\n",
            ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)),
            instance_id);
    return 0;
}

int _gnrc_rpl_trickle_start(char *arg1, char *arg2)
{
    uint8_t instance_id = 0;
    ipv6_addr_t dodag_id;
    gnrc_rpl_instance_t *inst;
    gnrc_rpl_dodag_t *dodag = NULL;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    instance_id = (uint8_t) atoi(arg1);
    if (instance_id == 0) {
        puts("error: <instance_id> must be a positive number greater than zero");
        return 1;
    }

    if (ipv6_addr_from_str(&dodag_id, arg2) == NULL) {
        puts("error: <dodag_id> must be a valid IPv6 address");
        return 1;
    }

    if ((inst = gnrc_rpl_instance_get(instance_id)) == NULL) {
        puts("error: could not find the <instance_id>");
        return 1;
    }

    if ((dodag = gnrc_rpl_dodag_get(inst, &dodag_id)) == NULL) {
        puts("error: <dodag_id> does not exist for the given <instance_id>");
        return 1;
    }

    trickle_start(gnrc_rpl_pid, &dodag->trickle, GNRC_RPL_MSG_TYPE_TRICKLE_INTERVAL,
                  GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min),
                  dodag->dio_interval_doubl, dodag->dio_redun);

    printf("success: started trickle timer of DODAG (%s) from instance (%d)\n",
            ipv6_addr_to_str(addr_str, &dodag_id, sizeof(addr_str)),
            instance_id);
    return 0;
}

int _gnrc_rpl_send_dis(void)
{
    ipv6_addr_t all_RPL_nodes = GNRC_RPL_ALL_NODES_ADDR;
    gnrc_rpl_send_DIS(NULL, &all_RPL_nodes);

    puts("success: send a DIS\n");
    return 0;
}

int _gnrc_rpl_dodag_show(void)
{
    printf("instance table:\t");
    for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        if (gnrc_rpl_instances[i].state == 0) {
            printf("[ ]");
        }
        else {
            printf("[X]");
        }
        if (i < (GNRC_RPL_INSTANCES_NUMOF - 1)) {
            printf("\t");
        }
        else {
            printf("\n");
        }
    }

    printf("dodag table:\t");
    for (uint8_t i = 0; i < GNRC_RPL_DODAGS_NUMOF; ++i) {
        if (gnrc_rpl_dodags[i].state == 0) {
            printf("[ ]");
        }
        else {
            printf("[X]");
        }
        if (i < (GNRC_RPL_DODAGS_NUMOF - 1)) {
            putchar('\t');
        }
        else {
            printf("\n");
        }
    }

    printf("parent table:\t");
    for (uint8_t i = 0; i < GNRC_RPL_PARENTS_NUMOF; ++i) {
        if (gnrc_rpl_parents[i].state == 0) {
            printf("[ ]");
        }
        else {
            printf("[X]");
        }
        if (i < (GNRC_RPL_PARENTS_NUMOF - 1)) {
            putchar('\t');
        }
    }
    putchar('\n');
    putchar('\n');

    gnrc_rpl_dodag_t *dodag = NULL;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    timex_t now, tc, ti, cleanup;
    vtimer_now(&now);
    for (uint8_t i = 0; i < GNRC_RPL_INSTANCES_NUMOF; ++i) {
        if (gnrc_rpl_instances[i].state == 0) {
            continue;
        }
        printf("instance [%d | mop: %d | ocp: %d | mhri: %d | mri %d]\n", gnrc_rpl_instances[i].id,
                gnrc_rpl_instances[i].mop, gnrc_rpl_instances[i].of->ocp,
                gnrc_rpl_instances[i].min_hop_rank_inc, gnrc_rpl_instances[i].max_rank_inc);
        LL_FOREACH(gnrc_rpl_instances[i].dodags, dodag) {
            tc = timex_sub(dodag->trickle.msg_callback_timer.absolute, now);
            ti = timex_sub(dodag->trickle.msg_interval_timer.absolute, now);
            cleanup = timex_sub(dodag->cleanup_timer.absolute, now);
            timex_normalize(&tc);
            timex_normalize(&ti);
            timex_normalize(&cleanup);
            printf("\tdodag [%s | R: %d | CL: %" PRIu32 "s | \
TR(I=[%d,%d], k=%d, c=%d, TC=%" PRIu32 "s, TI=%" PRIu32 "s)]\n",
                    ipv6_addr_to_str(addr_str, &dodag->dodag_id, sizeof(addr_str)),
                    dodag->my_rank, ((int32_t) cleanup.seconds) > 0 ? cleanup.seconds : 0,
                    (1 << dodag->dio_min), dodag->dio_interval_doubl,
                    dodag->trickle.k, dodag->trickle.c,
                    ((int32_t) tc.seconds) > 0 ? tc.seconds : 0,
                    ((int32_t) ti.seconds) > 0 ? ti.seconds : 0);
            gnrc_rpl_parent_t *parent;
            LL_FOREACH(dodag->parents, parent) {
                printf("\t\tparent [addr: %s | rank: %d | lifetime: %" PRIu32 "s]\n",
                        ipv6_addr_to_str(addr_str, &parent->addr, sizeof(addr_str)),
                        parent->rank, (parent->lifetime.seconds - now.seconds));
            }
        }
    }
    return 0;
}

int _gnrc_rpl(int argc, char **argv)
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
        if (argc == 4) {
            return _gnrc_rpl_dodag_remove(argv[2], argv[3]);
        }
        else if (argc == 3) {
            return _gnrc_rpl_instance_remove(argv[2]);
        }
    }
    else if (strcmp(argv[1], "trickle") == 0) {
        if ((argc == 5) && (strcmp(argv[2], "reset") == 0)) {
            return _gnrc_rpl_trickle_reset(argv[3], argv[4]);
        }
        else if ((argc == 5) && (strcmp(argv[2], "stop") == 0)) {
            return _gnrc_rpl_trickle_stop(argv[3], argv[4]);
        }
        else if ((argc == 5) && (strcmp(argv[2], "start") == 0)) {
            return _gnrc_rpl_trickle_start(argv[3], argv[4]);
        }
    }
    else if (strcmp(argv[1], "send") == 0) {
        if ((argc == 3) && (strcmp(argv[2], "dis") == 0)) {
            return _gnrc_rpl_send_dis();
        }
    }

    printf("usage: %s [help|init|rm|root|show]\n", argv[0]);
    puts("* help\t\t\t\t\t\t- show usage");
    puts("* init <if_id>\t\t\t\t\t- initialize RPL on the given interface");
    puts("* trickle reset <instance_id> <dodag_id>\t- reset the trickle timer");
    puts("* trickle start <instance_id> <dodag_id>\t- start the trickle timer");
    puts("* trickle stop <instance_id> <dodag_id>\t\t- stop the trickle timer");
    puts("* rm <instance_id>\t\t\t\t- delete the given instance and all related dodags");
    puts("* rm <instance_id> <dodag_id>\t\t\t- delete the dodag in the given instance");
    puts("* root <instance_id> <dodag_id>\t\t\t- add a dodag to a new or existing instance");
    puts("* send dis\t\t\t\t\t- send a multicast DIS");
    puts("* show\t\t\t\t\t\t- show instance and dodag tables");
    return 0;
}
/**
 * @}
 */
