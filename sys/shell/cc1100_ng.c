#include <stdio.h>
#include <string.h>
#include <transceiver.h>
#include <cc1100_ng.h>
#include <msg.h>

#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH

char text_msg[TEXT_SIZE];
msg mesg;
transceiver_command_t tcmd;

void _cc1100_ng_get_set_address_handler(char *addr) {
    int16_t a;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &a;
    mesg.content.ptr = (char*) &tcmd;
    if (sscanf(addr, "addr %hi", &a) > 0) {
        printf("[cc1100] Trying to set address %i\n", a);
        mesg.type = SET_ADDRESS;
    }
    else {
        mesg.type = GET_ADDRESS;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[cc1100] Got address: %i\n", a);
}

void _cc1100_ng_get_set_channel_handler(char *chan) {
    int16_t c;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &c;
    mesg.content.ptr = (char*) &tcmd;
    if (sscanf(chan, "chan %hi", &c) > 0) {
        printf("[cc1100] Trying to set channel %i\n", c);
        mesg.type = SET_CHANNEL;
    }
    else {
        mesg.type = GET_CHANNEL;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[cc1100] Got channel: %i\n", c);
}

void _cc1100_ng_send_handler(char *pkt) {
    radio_packet_t p;
    uint32_t response;
    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;
    uint16_t addr;

    if (sscanf(pkt, "txtsnd %hu %s", &(addr), text_msg) == 2) {
        p.data = (uint8_t*) text_msg;
        p.length = strlen(text_msg);
        p.dst = addr;
        mesg.type = SND_PKT;
        mesg.content.ptr = (char*) &tcmd;
        printf("[cc1100] Sending packet of length %u to %hu: %s\n", p.length, p.dst, (char*) p.data);
        msg_send_receive(&mesg, &mesg, transceiver_pid);
        response = mesg.content.value;
        printf("[cc1100] Packet sent: %lu\n", response);
    }
    else {
        puts("Usage:\ttsnd <ADDR> <MSG>");
    }
}
