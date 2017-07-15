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

#include "od.h"
#include "shell.h"
#include "openwsn.h"
#include "net/ipv6/addr.h"

#include "opendefs.h"
#include "02a-MAClow/IEEE802154E.h"
#include "03b-IPv6/icmpv6rpl.h"
#include "04-TRAN/openudp.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/idmanager.h"
#include "cross-layers/packetfunctions.h"

extern idmanager_vars_t idmanager_vars;
extern icmpv6rpl_vars_t icmpv6rpl_vars;

udp_resource_desc_t uinject_vars;

void uinject_sendDone(OpenQueueEntry_t *msg, owerror_t error)
{
    openqueue_freePacketBuffer(msg);
    puts("Send success");
}

void uinject_receive(OpenQueueEntry_t *pkt)
{
    printf("Received %i bytes on port %i\n", (int)pkt->length, pkt->l4_destination_port);
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

static int ifconfig_cmd(int argc, char **argv)
{
    if (idmanager_vars.isDAGroot) {
        puts("Node is DAG root");
    }
    open_addr_t temp_my128bID;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    memcpy(&temp_my128bID.addr_128b[0], &idmanager_vars.myPrefix.prefix, 8);
    memcpy(&temp_my128bID.addr_128b[8], &idmanager_vars.my64bID.addr_64b, 8);
    ipv6_addr_to_str(addr_str, (ipv6_addr_t *)temp_my128bID.addr_128b, sizeof(addr_str));
    printf("inet6 %s\n", addr_str);
    printf("RPL rank: %i\n", icmpv6rpl_vars.myDAGrank);
    return 0;
}

static int rpl_cmd(int argc, char **argv)
{
    /* TODO allow other prefixes via shell ?!? */
    uint8_t temp[8] = { 0xbb, 0xbb, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00 };
    open_addr_t myPrefix;

    myPrefix.type = ADDR_PREFIX;
    memcpy(myPrefix.prefix, &temp, sizeof(myPrefix.prefix));
    idmanager_setMyID(&myPrefix);
    icmpv6rpl_init();
    idmanager_setIsDAGroot(TRUE);

    return 0;
}

extern int udp_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "ifconfig", "Shows assigned IPv6 addresses", ifconfig_cmd },
    { "udp", "Send UDP messages and listen for messages on UDP port", udp_cmd },
    { "rplroot", "Set node as RPL DODAG root node", rpl_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("OpenWSN UDP test");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    openwsn_bootstrap();

    uinject_init();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
