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
#include "shell.h"

#include "opendefs.h"
#include "04-TRAN/udp.h"
#include "cross-layers/openqueue.h"

udp_resource_desc_t uinject_vars;

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

extern int udp_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
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
