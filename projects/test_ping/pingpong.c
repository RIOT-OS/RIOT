//include thy header!
#include <pingpong.h>

//standard stuff
#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include <thread.h>
#include <msg.h>

//net stuff
#include <transceiver.h>

static uint16_t sequence = 0;

radio_packet_t p;
ping_packet_t ping_packet;
transceiver_command_t tcmd;
msg_t mesg;

// see header for documentation
void broadcast_without_ack(uint16_t duration) {
    uint8_t counter  = 0;
    uint16_t seconds = 10;

    puts("Setting up broadcast");
    if (duration > 0) {
        seconds = duration;
    }

    while(counter < seconds){
        puts("Setting up packet");
        counter = counter + 1;
        send_broadcast();
        vtimer_usleep(1 * SECOND);
    }
}

// see header for documentation
void broadcast_incoming() {
    puts("got some broadcasted stuff");
}

// see header for documentation
void send_broadcast() {
    puts("Preparing broadcast ping");
    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;
    p.length = sizeof(ping_packet_t);
    p.dst = 0;

    puts("creating packet..");
    sequence++;
    ping_packet.seq_nr = sequence;
    ping_packet.type = PING_BCST;

    puts("sending broadcast..");
    p.data = (uint8_t *) &ping_packet;
    msg_send(&mesg, transceiver_pid, 1);
    puts("sent");
}
