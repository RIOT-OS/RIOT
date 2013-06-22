/**
 * Shell commands for cc110x_ng driver
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_cc110x_ng.c
 * @brief   provides shell commands to configure cc110x_ng driver 
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <transceiver.h>
#include <cc110x_ng.h>
#include <msg.h>
#include <inttypes.h>

#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH

char text_msg[TEXT_SIZE];
msg_t mesg;
transceiver_command_t tcmd;

void _cc110x_ng_get_set_address_handler(char *addr)
{
    int16_t a;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;
    a = atoi(addr + 5);

    if(strlen(addr) > 5) {
        printf("[cc110x] Trying to set address %i\n", a);
        mesg.type = SET_ADDRESS;
    }
    else {
        mesg.type = GET_ADDRESS;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[cc110x] Got address: %i\n", a);
}

void _cc110x_ng_get_set_channel_handler(char *chan)
{
    int16_t c;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &c;
    mesg.content.ptr = (char *) &tcmd;
    c = atoi(chan + 5);

    if(strlen(chan) > 5) {
        printf("[cc110x] Trying to set channel %i\n", c);
        mesg.type = SET_CHANNEL;
    }
    else {
        mesg.type = GET_CHANNEL;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[cc110x] Got channel: %i\n", c);
}

void _cc110x_ng_send_handler(char *pkt)
{
    radio_packet_t p;
    uint32_t response;
    uint16_t addr;
    char *tok;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;

    tok = strtok(pkt + 7, " ");

    if(tok) {
        addr = atoi(tok);
        tok = strtok(NULL, " ");

        if(tok) {
            memset(text_msg, 0, TEXT_SIZE);
            memcpy(text_msg, tok, strlen(tok));
            p.data = (uint8_t *) text_msg;
            p.length = strlen(text_msg) + 1;
            p.dst = addr;
            mesg.type = SND_PKT;
            mesg.content.ptr = (char *) &tcmd;
            printf("[cc110x] Sending packet of length %u to %hu: %s\n", p.length, p.dst, (char *) p.data);
            msg_send_receive(&mesg, &mesg, transceiver_pid);
            response = mesg.content.value;
            printf("[cc110x] Packet sent: %" PRIu32 "\n", response);
            return;
        }
    }

    puts("Usage:\ttxtsnd <ADDR> <MSG>");
}

void _cc110x_ng_monitor_handler(char *mode)
{
    unsigned int m;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &m;
    mesg.content.ptr = (char *) &tcmd;
    m = atoi(mode + 8);

    if(strlen(mode) > 8) {
        printf("Setting monitor mode: %u\n", m);
        mesg.type = SET_MONITOR;
        msg_send(&mesg, transceiver_pid, 1);
    }
    else {
        puts("Usage:\nmonitor <MODE>");
    }
}
