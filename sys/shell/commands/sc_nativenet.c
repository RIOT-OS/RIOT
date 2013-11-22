/**
 * Shell commands for native transceiver
 *
 * Copyright (C) 2013 Ludwig Ortmann.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_nativenet.c
 * @brief   provides shell commands to configure nativenet transceiver 
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "transceiver.h"
#include "nativenet.h"
#include "msg.h"

#define TEXT_SIZE           (255) /* XXX: this aint enough for everybody */

void _nativenet_get_set_address_handler(char *addr)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    uint16_t a;

    tcmd.transceivers = TRANSCEIVER_NATIVE;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;
    a = atoi(addr + 5);

    if (strlen(addr) > 5) {
        printf("[nativenet] trying to set address %"PRIu16"\n", a);
        mesg.type = SET_ADDRESS;
    }
    else {
        mesg.type = GET_ADDRESS;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[nativenet] got address: %"PRIu16"\n", a);
}

void _nativenet_get_set_channel_handler(char *chan)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    uint8_t c;

    tcmd.transceivers = TRANSCEIVER_NATIVE;
    tcmd.data = &c;
    mesg.content.ptr = (char *) &tcmd;
    c = atoi(chan + 5);

    if (strlen(chan) > 5) {
        printf("[nativenet] Trying to set channel %"PRIu8"\n", c);
        mesg.type = SET_CHANNEL;
    }
    else {
        mesg.type = GET_CHANNEL;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[nativenet] Got channel: %"PRIu8"\n", c);
}

void _nativenet_send_handler(char *pkt)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    char text_msg[TEXT_SIZE];

    radio_packet_t p;
    uint32_t response;
    uint16_t addr;
    char *tok;

    tcmd.transceivers = TRANSCEIVER_NATIVE;
    tcmd.data = &p;

    tok = strtok(pkt + 7, " ");

    if (tok) {
        addr = atoi(tok);
        tok = strtok(NULL, " ");

        if (tok) {
            memset(text_msg, 0, TEXT_SIZE);
            memcpy(text_msg, tok, strlen(tok));
            p.data = (uint8_t *) text_msg;
            p.length = strlen(text_msg) + 1;
            p.dst = addr;
            mesg.type = SND_PKT;
            mesg.content.ptr = (char *)&tcmd;
            printf("[nativenet] Sending packet of length %"PRIu16" to %"PRIu16": %s\n", p.length, p.dst, (char*) p.data);
            msg_send_receive(&mesg, &mesg, transceiver_pid);
            response = mesg.content.value;
            printf("[nativenet] Packet sent: %"PRIu32"\n", response);
            return;
        }
    }

    puts("Usage:\ttxtsnd <ADDR> <MSG>");
}

void _nativenet_monitor_handler(char *mode)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    uint8_t m;

    tcmd.transceivers = TRANSCEIVER_NATIVE;
    tcmd.data = &m;
    mesg.content.ptr = (char *) &tcmd;
    m = atoi(mode + 8);

    if (strlen(mode) > 8) {
        printf("Setting monitor mode: %"PRIu8"\n", m);
        mesg.type = SET_MONITOR;
        msg_send(&mesg, transceiver_pid, 1);
    }
    else {
        puts("Usage:\nmonitor <MODE>");
    }
}
