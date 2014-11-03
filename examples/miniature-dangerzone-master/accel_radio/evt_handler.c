/*
 * Copyright (C) 2014 Freie Universität Berlin
 * 
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cebit_sensor
 * @{
 *
 * @file        evt_handler.c
 * @brief       CeBIT 2014 demo application - sensor node
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "vtimer.h"
#include "board.h"

#include "transceiver.h"
#include "evt_handler.h"
#include "events.h"

#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH

static uint8_t sequ_no = 0;
static uint8_t evt_no = 0;

void send_event(evt_t event);

void send(void)
{
    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    radio_packet_t p;

    transceiver_command_t tcmd;
    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;

    char text_msg[TEXT_SIZE];
    memset(text_msg, 0, TEXT_SIZE);

    p.data = (uint8_t *) text_msg;
    p.length = strlen(text_msg) + 1;
    p.dst = 0; 

    msg_t mesg;
    mesg.type = SND_PKT;
    mesg.content.ptr = (char *) &tcmd;

    printf("[transceiver] Sending packet of length %" PRIu16 " to %" PRIu16 ": %s\n", p.length, p.dst, (char*) p.data);
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    int8_t response = mesg.content.value;
    printf("[transceiver] Packet sent: %" PRIi8 "\n", response);
}



void evt_handler_ok(void)
{
    puts("EVENT: all good");
    send();
    LED_GREEN_ON;
    LED_RED_OFF;
    // send status ok to actuator nodes
    send_event(CONFIRM);
}

void evt_handler_warn(void)
{
    puts("EVENT: warning");
    send();
    LED_GREEN_ON;
    LED_RED_ON;
    // send status warning to actuator nodes
    send_event(WARN);
}

void evt_handler_alarm(void)
{
    puts("EVENT: alarm");
    send();
    LED_GREEN_OFF;
    LED_RED_ON;
    // send alarm event to actuator nodes
    send_event(ALARM);
}

void send_event(evt_t event)
{
    char cmd[3];
    if (event != CONFIRM) {         // if CONFIRM reuse old evt_no
        ++evt_no;
    }
    cmd[0] = (char)event;      // id
    cmd[1] = (char)evt_no;   // data
    cmd[2] = (char)sequ_no++;  // sequence number
    printf("send id:%i, data:%i, sequ:%i\n", cmd[0], cmd[1], cmd[2]);
}
