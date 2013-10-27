#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <transceiver.h>
#include <cc2420.h>
#include <msg.h>

#define TEXT_SIZE           CC2420_MAX_DATA_LENGTH

char text_msg[TEXT_SIZE];
msg_t mesg;
transceiver_command_t tcmd;

void _cc2420_get_set_address_handler(char *addr) {
    uint16_t a;

    tcmd.transceivers = TRANSCEIVER_CC2420;
    tcmd.data = &a;
    mesg.content.ptr = (char*) &tcmd;
    if (strlen(addr) > 5) {
        a = atoi(addr+5);
        printf("[cc2420] Trying to set address %i\n", a);
        mesg.type = SET_ADDRESS;
    }
    else {
        mesg.type = GET_ADDRESS;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[cc2420] Got address: %i\n", a);
}

void _cc2420_get_set_channel_handler(char *chan) {
    int16_t c;

    tcmd.transceivers = TRANSCEIVER_CC2420;
    tcmd.data = &c;
    mesg.content.ptr = (char*) &tcmd;
    if (strlen(chan) > 5) {
        c = atoi(chan+5);
        printf("[cc2420] Trying to set channel %i\n", c);
        mesg.type = SET_CHANNEL;
    }
    else {
        mesg.type = GET_CHANNEL;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[cc2420] Got channel: %i\n", c);
}

void _cc2420_get_set_pan_handler(char *pan) {
    uint16_t p;

    tcmd.transceivers = TRANSCEIVER_CC2420;
    tcmd.data = &p;
    mesg.content.ptr = (char*) &tcmd;
    if (strlen(pan) > 4) {
        p = atoi(pan+4);
        printf("[cc2420] Trying to set pan %i\n", p);
        mesg.type = SET_PAN;
    }
    else {
        mesg.type = GET_PAN;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[cc2420] Got pan: %i\n", p);
}

void _cc2420_send_handler(char *pkt) {
    radio_packet_t p;
    uint32_t response;
    uint16_t addr;
    char *tok;

    tcmd.transceivers = TRANSCEIVER_CC2420;
    tcmd.data = &p;

    tok = strtok(pkt+7, " ");
    if (tok) {
        addr = atoi(tok);
        tok = strtok(NULL, " ");
        if (tok) {
            memset(text_msg, 0, TEXT_SIZE);
            memcpy(text_msg, tok, strlen(tok));
    /*    if (sscanf(pkt, "txtsnd %hu %s", &(addr), text_msg) == 2) {*/
            p.data = (uint8_t*) text_msg;
            p.length = strlen(text_msg) + 1;
            p.dst = addr;
            mesg.type = SND_PKT;
            mesg.content.ptr = (char*) &tcmd;
            printf("[cc2420] Sending packet of length %u to %u: %s\n", p.length, p.dst, (char*) p.data);
            msg_send_receive(&mesg, &mesg, transceiver_pid);
            response = mesg.content.value;
            printf("[cc2420] Packet sent: %lu\n", response);
            return;
        }
    }
    puts("Usage:\ttxtsnd <ADDR> <MSG>");
}

void _cc2420_monitor_handler(char *mode) {
    unsigned int m;

    tcmd.transceivers = TRANSCEIVER_CC2420;
    tcmd.data = &m;
    mesg.content.ptr = (char*) &tcmd;
    if (strlen(mode) > 8) {
        m = atoi(mode+8);
        printf("Setting monitor mode: %u\n", m);
        mesg.type = SET_MONITOR;
        msg_send(&mesg, transceiver_pid, 1);
    }
    else {
        puts("Usage:\nmonitor <MODE>");
    }
}
