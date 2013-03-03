/*
 * etx_beaconing.c
 *
 *  Created on: Feb 26, 2013
 *      Author: stephan
 */
#include "etx_beaconing.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <hwtimer.h>
#include <thread.h>
#include <transceiver.h>

#define MS 10000

unsigned int etx_beacon_pid;
unsigned int etx_radio_pid;

//Buffer
char etx_beacon_buf[ETX_BEACON_STACKSIZE];
char etx_radio_buf[ETX_BEACON_STACKSIZE];

uint8_t etx_send_buf[ETX_BUF_SIZE];
uint8_t etx_rec_buf[ETX_BUF_SIZE];

//Message queue for radio
msg_t msg_q[ETX_RCV_BUFFER_SIZE];

//Transceiver command for sending ETX probes
transceiver_command_t tcmd;

//Message to send probes with
msg_t mesg;

static etx_probe_t * get_etx_send_buf(void) {
    return ((etx_probe_t *) &(etx_send_buf[0]));
}
static etx_probe_t * get_etx_rec_buf(void) {
    return ((etx_probe_t *) &(etx_rec_buf[0]));
}

void init_etx_beaconing(void) {

    //set code
    puts("INit of etx beaconing");
    etx_send_buf[0] = ETX_BEACON;

    etx_beacon_pid = thread_create(etx_beacon_buf, ETX_BEACON_STACKSIZE,
            PRIORITY_MAIN - 1, CREATE_STACKTEST,
            etx_beacon, "etx_beacon");

    etx_radio_pid = thread_create(etx_radio_buf, ETX_BEACON_STACKSIZE,
            PRIORITY_MAIN - 1, CREATE_STACKTEST,
            etx_radio, "etx_radio");

    //register at transceiver
    transceiver_register(TRANSCEIVER_CC1100, etx_radio_pid);
}

void etx_beacon(void) {
    /*
     * Sends a message every second +-10% jitter.
     * A correcting variable keeps this timer from drifting off from the
     * 1 second interval.
     */
    radio_packet_t p;

    uint8_t test = 0; //TODO delete, change to true or sth.
    uint8_t jittercorrection = 10;
    uint8_t jitter = (uint8_t) (rand() % 21);

    while (test < 10) {

        //Prepare the ETX packet
        mesg.type = SND_PKT;
        mesg.content.ptr = (char*) &tcmd;

        tcmd.transceivers = TRANSCEIVER_CC1100;
        tcmd.data = &p;
        p.length = (get_etx_send_buf())->length + 2; //opt.+lngth+variable (addr,pkt_count) pairs
        p.dst = 0;
        p.data = (uint8_t *) &etx_send_buf;
        msg_send(&mesg, transceiver_pid, 1);

        test++;

        hwtimer_wait(HWTIMER_TICKS(80*MS + jittercorrection*MS + jitter*MS));

        /*
         * The jitter correction is needed to stay at a base interval of 1 sec.
         * between the wakeups. It takes the old jittervalue in account and
         * modifies the time to wait accordingly.
         */
        jittercorrection = 20 - jitter;

        //the jitter is a number between 0 and 20
        jitter = (uint8_t) (rand() % 21);

    }
}

void handle_etx_beacon(void) {
    /*
     * Handle the ETX probe that has been received and update all infos.
     * For now, just print a nice output of what I got, so i see it works as
     * intended.
     */

    etx_probe_t * probe = get_etx_rec_buf();

    printf("ETX beacon package received with following values:\n"
            "\tPackage Option:%x\n"
            "\tPackage Length:%u\n\n", probe->code, probe->length);

    for (uint8_t i = 0; i < probe->length / 3; i++) {
        printf("\tIPv6 Short Addr:%u%u\n"
                "\tPackets f. Addr:%u\n\n", probe->data[i * 3],
                probe->data[i * 3 + 1], probe->data[i * 3 + 2]);
    }

}

void etx_radio(void) {
    msg_t m;
    radio_packet_t *p;
    etx_probe_t *etx_pkt;
    msg_init_queue(msg_q, ETX_RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t*) m.content.ptr;
            etx_pkt = (etx_probe_t *) p->data;

            if (etx_pkt->code == ETX_BEACON) {
                //copy to receive buffer
                memcpy(etx_rec_buf, p->data, etx_pkt->length + 2);
                handle_etx_beacon();
            }
            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            //packet is not for me
        }
    }
}
