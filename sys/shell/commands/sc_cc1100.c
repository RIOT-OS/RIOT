/**
 * Shell commands for cc110x driver
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_cc1100.c
 * @brief   provides shell commands to configure cc110x driver 
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cc1100-interface.h>

#ifdef MODULE_CC110X
#include <msg.h>
#include <transceiver.h>

#ifdef MODULE_TRANSCEIVER
#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH

char text_msg[TEXT_SIZE];
msg_t mesg;
transceiver_command_t tcmd;

void _cc1100_get_set_address_handler(char *addr)
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

void _cc1100_get_set_channel_handler(char *chan)
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

void _cc1100_send_handler(char *pkt)
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
            /*    if (sscanf(pkt, "txtsnd %hu %s", &(addr), text_msg) == 2) {*/
            p.data = (uint8_t *) text_msg;
            p.length = strlen(text_msg) + 1;
            p.dst = addr;
            mesg.type = SND_PKT;
            mesg.content.ptr = (char *) &tcmd;
            printf("[cc110x] Sending packet of length %u to %hu: %s\n", p.length, p.dst, (char *) p.data);
            msg_send_receive(&mesg, &mesg, transceiver_pid);
            response = mesg.content.value;
            printf("[cc110x] Packet sent: %lu\n", response);
            return;
        }
    }

    puts("Usage:\ttxtsnd <ADDR> <MSG>");
}

#else
void _cc110x_get_set_address_handler(char *addr)
{
    int16_t a;

    a = atoi(addr + 5);

    if(strlen(addr) > 5) {
        printf("[cc110x] Setting address %i ... ", a);
        cc1100_set_address((radio_address_t)a);

        if(cc1100_get_address() == (radio_address_t)a) {
            puts("[OK]");
        }
        else {
            puts("Error!");
        }
    }
    else {
        printf("[cc1100] Got address: %i\n", cc1100_get_address());
    }
}

void _cc110x_get_set_channel_handler(char *addr)
{
    int16_t a;

    a = atoi(addr + 5);

    if(strlen(addr) > 5) {
        printf("[cc110x] Setting channel %i...", a);
        cc1100_set_channel(a);

        if(cc1100_get_channel() == a) {
            puts("OK");
        }
        else {
            puts("Error!");
        }
    }
    else {
        printf("[cc1100] Got address: %i\n", cc1100_get_channel());
    }
}
#endif
#endif
