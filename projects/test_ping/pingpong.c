//include thy header!
#include <pingpong.h>

//standard stuff
#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include <thread.h>
#include <msg.h>
#include <swtimer.h>

//net stuff
#include <transceiver.h>

char stack_stop[XS_STACK];

static uint64_t start = 0;
static uint64_t end = 0;
static float rtt = 0;
static bool cont_ping = false;
static uint16_t time = 0;
static int wait_pid = 0;
static uint16_t sequence = 0;

radio_packet_t p;
ping_packet_t ping_packet;
transceiver_command_t tcmd;
msg_t mesg;

// see header for documentation
void stop_after_time() {
    vtimer_usleep(time * SECOND);
    stop();
}

// see header for documentation
void stop() {
    cont_ping = false;
}

// see header for documentation
void pingpong(uint8_t addr, uint16_t duration) {
    cont_ping = true;
    time = duration;

    if (time) {
        //A time has been given after which pings should not be sent anymore
        thread_create(stack_stop, XS_STACK, PRIORITY_MAIN - 2, CREATE_STACKTEST,
                stop_after_time, "showstopper");
    }
    while (cont_ping) {
        start = swtimer_now();
        wait_pid = thread_getpid();
        send_ping(addr);

        wait_pong();
        //savestats();
    }
    //We're done, print endstats
    //printendstats();
}

// see header for documentation
void wait_pong() {
    //Either sleep 2 seconds or get waken up in between
    vtimer_usleep(2 * SECOND);
    calc_rtt();
    if (rtt > 2000) {
        //this means that the pong to this ping wasn't received
        print_failure();
    } else {
        print_success();
    }
}

// see header for documentation
void broadcast_without_ack(uint16_t duration) {
    time = duration;

    if (time) {
        //A time has been given after which pings should not be sent anymore
        thread_create(stack_stop, XS_STACK, PRIORITY_MAIN - 2, 0,
                stop_after_time, "showstopper");
    }

    while (cont_ping) {
        start = swtimer_now();
        send_broadcast();
        vtimer_usleep(500 * 1000);
    }
}

// see header for documentation
void ping_incoming(uint8_t src) {
    puts("Ping in, sending ack out");
    send_ack(src);
}

// see header for documentation
void ack_incoming() {
    thread_wakeup(wait_pid);
    puts("Ping got acked");
}

// see header for documentation
void broadcast_incoming() {
    puts("got some broadcasted stuff");
}

// see header for documentation
void print_success() {
    printf("%s%f%s\n", "time=", rtt, "ms");
}

// see header for documentation
void print_failure() {
    puts("Time for ping exceeded");
}

// see header for documentation
void send_ping(uint8_t addr) {

    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;
    p.length = sizeof(ping_packet_t);
    p.dst = addr;

    puts("creating packet..");
    sequence++;
    ping_packet.seq_nr = sequence;
    ping_packet.type = PING;

    puts("sending ping..");
    printf("pingpacket address: %p", &ping_packet);
    printf("pingpacket seqnr: %d", ping_packet.seq_nr);
    printf("pingpacket type: %d\n", ping_packet.type);
    thread_print_all();
    p.data = (uint8_t *) &ping_packet;
    msg_send(&mesg, transceiver_pid, 1);
    puts("sent");

}

// see header for documentation
void send_ack(uint8_t addr) {
    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;
    p.length = sizeof(ping_packet_t);
    p.dst = addr;

    puts("creating packet..");
    sequence++;
    ping_packet.seq_nr = sequence;
    ping_packet.type = PING_ACK;

    puts("sending ack..");
    p.data = (uint8_t *) &ping_packet;
    msg_send(&mesg, transceiver_pid, 1);
    puts("sent");
}

// see header for documentation
void send_broadcast() {
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

// see header for documentation
void calc_rtt() {
    end = swtimer_now();
    rtt = ((float) end - (float) start) / 1000;
}
