/**
 * Shell commands for transceiver module
 *
 * Copyright (C) 2013 Ludwig Ortmann.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_transceiver.c
 * @brief   provides shell commands to configure the transceiver
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "transceiver.h"
#include "msg.h"

#if   defined( MODULE_CC110X )
#include "cc110x.h"
#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_CC1100

#elif defined( MODULE_CC110X_NG )
#include "cc110x_ng.h"
#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_CC1100

#elif defined( MODULE_CC2420 )
#include "cc2420.h"
#define TEXT_SIZE           CC2420_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_CC2420

#elif defined( MODULE_AT86RF231 )
#include "at86rf231.h"
#define TEXT_SIZE           AT86RF231_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_AT86RF231

#elif defined( MODULE_NATIVENET )
#include "nativenet.h"
#define TEXT_SIZE           NATIVE_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_NATIVE
#endif


/* checked for type safety */
void _transceiver_get_set_address_handler(char *addr)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    radio_address_t a;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;

    if (strlen(addr) > 5) {
        a = atoi(addr + 5);
        printf("[transceiver] trying to set address %"PRIu16"\n", a);
        mesg.type = SET_ADDRESS;
    }
    else {
        mesg.type = GET_ADDRESS;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[transceiver] got address: %"PRIu16"\n", a);
}

/* checked for type safety */
void _transceiver_get_set_channel_handler(char *chan)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    int32_t c;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &c;
    mesg.content.ptr = (char *) &tcmd;

    if (strlen(chan) > 5) {
        c = atoi(chan + 5);
        printf("[transceiver] Trying to set channel %"PRIi32"\n", c);
        mesg.type = SET_CHANNEL;
    }
    else {
        mesg.type = GET_CHANNEL;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    if (c == -1) {
        puts("[transceiver] Error setting/getting channel");
    }
    else {
        printf("[transceiver] Got channel: %"PRIi32"\n", c);
    }
}

void _transceiver_send_handler(char *pkt)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    char text_msg[TEXT_SIZE];

    radio_packet_t p;
    int8_t response;
    radio_address_t addr;
    char *tok;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
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
            printf("[transceiver] Sending packet of length %"PRIu16" to %"PRIu16": %s\n", p.length, p.dst, (char*) p.data);
            msg_send_receive(&mesg, &mesg, transceiver_pid);
            response = mesg.content.value;
            printf("[transceiver] Packet sent: %"PRIi8"\n", response);
            return;
        }
    }

    puts("Usage:\ttxtsnd <ADDR> <MSG>");
}

/* checked for type safety */
void _transceiver_monitor_handler(char *mode)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    uint8_t m;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &m;
    mesg.content.ptr = (char *) &tcmd;

    if (strlen(mode) > 8) {
        m = atoi(mode + 8);
        printf("Setting monitor mode: %"PRIu8"\n", m);
        mesg.type = SET_MONITOR;
        msg_send(&mesg, transceiver_pid, 1);
    }
    else {
        puts("Usage:\nmonitor <MODE>");
    }
}

/* checked for type safety */
void _transceiver_get_set_pan_handler(char *pan) {
    transceiver_command_t tcmd;
    msg_t mesg;
    int32_t p;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &p;
    mesg.content.ptr = (char*) &tcmd;
    if (strlen(pan) > 4) {
        p = atoi(pan+4);
        printf("[transceiver] Trying to set pan %"PRIi32"\n", p);
        mesg.type = SET_PAN;
    }
    else {
        mesg.type = GET_PAN;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    if (p == -1) {
        puts("[transceiver] Error setting/getting pan");
    }
    else {
        printf("[transceiver] Got pan: %"PRIi32"\n", p);
    }
}

/* checked for type safety */
#ifdef DBG_IGNORE
void _transceiver_set_ignore_handler(char *addr)
{
    transceiver_command_t tcmd;
    msg_t mesg;
    radio_address_t a;
    int16_t response;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &a;
    mesg.content.ptr = (char*) &tcmd;

    if (strlen(addr) > 4) {
        a = atoi(addr + 4);
        printf("[transceiver] trying to add address %"PRIu16" to the ignore list \n", a);
        mesg.type = DBG_IGN;
        msg_send_receive(&mesg, &mesg, transceiver_pid);
        response = a;
        if (response == -1) {
            printf("Error: ignore list full\n");
        }
        else {
            printf("Success (added at index %"PRIi16").\n", response);
        }
    }
    else {
        puts("Usage:\nign <address>");
    }
}
#endif
