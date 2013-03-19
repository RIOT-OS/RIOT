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
#include <vtimer.h>
#include <thread.h>
#include <transceiver.h>

#include "sys/net/sixlowpan/sixlowmac.h"
#include "sys/net/sixlowpan/ieee802154_frame.h"
#include "rpl_structs.h"
#include "rpl_dodag.h"

#define MS 10000

int etx_beacon_pid = 0;
int etx_radio_pid = 0;
int etx_update_pid = 0;

//Buffer
char etx_beacon_buf[ETX_BEACON_STACKSIZE] = { 0 };
char etx_radio_buf[ETX_RADIO_STACKSIZE] = { 0 };
char etx_update_buf[ETX_UPDT_STACKSIZE] = { 0 };

uint8_t etx_send_buf[ETX_BUF_SIZE] = { 0 };
uint8_t etx_rec_buf[ETX_BUF_SIZE] = { 0 };

/*
 * The counter for the current 'round'. An ETX beacon is sent every ETX_INTERVAL
 * u-seconds and a node computes the ETX value by comparing the the received
 * probes vs the expected probes from a neighbor every ETX_ROUND intervals.
 */
static uint8_t rounds = 0;

//Message queue for radio
msg_t msg_que[ETX_RCV_BUFFER_SIZE] = { 0 };

/*
 * This could (and should) be done differently, once the RPL implementation
 * deals with candidate neighbors in another way than just defining that every
 * possible neighbor we hear from is a parent.
 * Right now, we need to keep track of the ETX values of other nodes without
 * needing them to be in our parent array, so we have another array here in
 * which we put all necessary info for up to RPL_MAX_CANDIDATE_NEIGHBORS
 * candidates.
 */
//Candidate array for candidate<==>parent matching
//Throws a warning, but this is the correct way to initialize!
static rpl_candidate_neighbor_t candidates[RPL_MAX_CANDIDATE_NEIGHBORS] = { 0 };

//Transceiver command for sending ETX probes
transceiver_command_t tcmd;

//Message to send probes with
msg_t mesg;

//RPL-address
static ipv6_addr_t * own_address;

static etx_probe_t * get_etx_send_buf(void) {
    return ((etx_probe_t *) &(etx_send_buf[0]));
}
static etx_probe_t * get_etx_rec_buf(void) {
    return ((etx_probe_t *) &(etx_rec_buf[0]));
}


void show_candidates(void) {
    //TODO delete
    /*
     for (int i = 0; i < RPL_MAX_CANDIDATE_NEIGHBORS; i++) {
     printf("Candidates Addr:%d\n"
     "\t cur_etx:%f\n"
     "\t packets_rx:%d\n"
     "\t used:%d\n", candidates[i].addr.uint8[ETX_IPV6_LAST_BYTE],
     candidates[i].cur_etx, candidates[i].packets_rx,
     candidates[i].used);
     }
     */
    rpl_candidate_neighbor_t * candidate;
    rpl_candidate_neighbor_t *end;

    for (candidate = &candidates[0], end = candidates
            + RPL_MAX_CANDIDATE_NEIGHBORS; candidate < end;
            candidate++) {
        printf("Candidates Addr:%d\n"
                "\t cur_etx:%f\n"
                "\t packets_rx:%d\n"
                "\t used:%d\n", candidate->addr.uint8[ETX_IPV6_LAST_BYTE],
                candidate->cur_etx, candidate->packets_rx,
                candidate->used);
    }
}

void etx_init_beaconing(ipv6_addr_t * address) {
    own_address = address;
    //set code
    printf("ETX BEACON INIT");
    etx_send_buf[0] = ETX_BEACON;

    thread_print_all();
    etx_beacon_pid = thread_create(etx_beacon_buf, ETX_BEACON_STACKSIZE,
            PRIORITY_MAIN - 1, CREATE_STACKTEST,
            etx_beacon, "etx_beacon");

    etx_radio_pid = thread_create(etx_radio_buf, ETX_RADIO_STACKSIZE,
            PRIORITY_MAIN - 1, CREATE_STACKTEST,
            etx_radio, "etx_radio");

    /*
     * Maybe this should not be in a seperate thread resource-wise, but the
     * motive was to delay beacon-sending as little as possible, as this would
     * derail beaconing from the intended ETX_INTERVAL.
     */

    etx_update_pid = thread_create(etx_update_buf, ETX_UPDT_STACKSIZE,
            PRIORITY_MAIN - 1, CREATE_STACKTEST,
            etx_update, "etx_update");

//register at transceiver
    transceiver_register(TRANSCEIVER_CC1100, etx_radio_pid);
    printf("...[DONE]\n");
}

void etx_beacon(void) {
    /*
     * TODO use ETX_INTERVAL variable instead of fixed value
     * Sends a message every ETX_INTERVAL +/- a small jitter-value.
     * A correcting variable is needed to stay at a base interval of
     * ETX_INTERVAL between the wakeups. It takes the old jittervalue in account
     * and modifies the time to wait accordingly.
     */
    etx_probe_t *etx_p = get_etx_send_buf();
    uint8_t jittercorrection = ETX_DEF_JIT_CORRECT;
    uint8_t jitter = (uint8_t) (rand() % ETX_JITTER_MOD);

    uint8_t p_length = 0;

    ieee_802154_long_t empty_addr = { 0 };

    while (true) {

        if (rounds == ETX_ROUNDS) {
            //calculate the ETX values and update the parents
            thread_wakeup(etx_update_pid);
            rounds = 1;
        } else {
            rounds++;
        }

        //Build etx packet
        p_length = 0;
        for (uint8_t i = 0; i < RPL_MAX_CANDIDATE_NEIGHBORS; i++) {
            if (candidates[i].used != 0) {
                etx_p->data[i * ETX_TUPLE_SIZE] = candidates[i].addr.uint8[ETX_IPV6_LAST_BYTE];
                etx_p->data[i * ETX_TUPLE_SIZE + ETX_PKT_REC_OFFSET] = candidates[i].packets_rx;
                p_length = p_length + ETX_PKT_HDR_LEN;
            }
        }
        etx_p->length = p_length;

        send_ieee802154_frame(&empty_addr, &etx_send_buf[0],
                get_etx_send_buf()->length + ETX_PKT_HDR_LEN, 1);
        puts("sent beacon!");
        //vtimer_usleep(80 * MS + jittercorrection * MS + jitter * MS);
        /// TODO once vtimer works as intended, replace the hwtimer here with
        /// the vtimer. Right now vtimer bugs, so we have hwtimer here.
        /// hangs after 38 runthroughs (roughly 1 minute, a bit more)
        hwtimer_wait(HWTIMER_TICKS(80*MS + jittercorrection*MS + jitter*MS));

        jittercorrection = 20 - jitter;

        //the jitter is a value between 0 and 20
        jitter = (uint8_t) (rand() % ETX_JITTER_MOD);
    }
}

rpl_candidate_neighbor_t * etx_find_candidate(ipv6_addr_t * address) {
    /*
     * find the candidate with address address and returns it, or returns NULL
     * if no candidate having this address was found.
     */
    //for (uint8_t i = 0; i < RPL_MAX_CANDIDATE_NEIGHBORS; i++) {
    //    if (candidates[i].used
    //            && (rpl_equal_id(&candidates[i].addr, address))) {
    //        return &candidates[i];
    //    }
    //}
    return NULL ;
}

double etx_get_metric(ipv6_addr_t * address) {
    rpl_candidate_neighbor_t * candidate = etx_find_candidate(address);
    if (candidate != NULL ) {
        return candidate->packets_rx / (double) ETX_ROUNDS;
    }
    return 0;
}

void etx_handle_beacon(ipv6_addr_t * candidate_address) {
    puts("handle_beacon");        //TODO del
    /*
     * Handle the ETX probe that has been received and update all infos.
     */

    etx_probe_t * probe = get_etx_rec_buf();

    //Todo delete once everything works well
    printf("ETX beacon package received with following values:\n"
            "\tPackage Option:%x\n"
            "\t   Data Length:%u\n"
            "\tSource Address:%d\n\n", probe->code, probe->length,
            candidate_address->uint8[ETX_IPV6_LAST_BYTE]);

    rpl_candidate_neighbor_t* candidate = NULL;

    for (uint8_t i = 0; i < probe->length / ETX_TUPLE_SIZE; i++) {
        //todo delete once everything works well
        printf("\tIPv6 short  Addr:%u\n"
                "\tPackets f. Addr:%u\n\n", probe->data[i * ETX_TUPLE_SIZE],
                probe->data[i * ETX_TUPLE_SIZE + ETX_PKT_REC_OFFSET]);

        // If i find my address in this probe, update the packet_rx value for
        // this candidate, if he is in my candidate list.
        if (probe->data[i * ETX_TUPLE_SIZE] == own_address->uint8[ETX_IPV6_LAST_BYTE]) {
            //candidate = etx_find_candidate(candidate_address);
            candidate = NULL;//TODO delete
            if (candidate != NULL ) {
                candidate->packets_rx = probe->data[i * ETX_TUPLE_SIZE + ETX_PKT_REC_OFFSET];
            }
        }
    }

}

void etx_radio(void) {
    msg_t m;
    radio_packet_t *p;

    ieee802154_frame_t frame;

    msg_init_queue(msg_que, ETX_RCV_BUFFER_SIZE);

    ipv6_addr_t ll_address;
    ipv6_addr_t candidate_addr;

    ipv6_set_ll_prefix(&ll_address);
    ipv6_get_saddr(&candidate_addr, &ll_address);

    while (1) {
        puts("radio");        //TODO del
        msg_receive(&m);
        puts("msg received");
        if (m.type == PKT_PENDING) {
            p = (radio_packet_t*) m.content.ptr;

            read_802154_frame(p->data, &frame, p->length);

            if (frame.payload[0] == ETX_BEACON) {
                //copy to receive buffer
                memcpy(etx_rec_buf, &frame.payload[0], frame.payload_len);

                //create IPv6 address from radio packet
                //we can do the cast here since rpl nodes can only have addr
                //up to 8 bits
                candidate_addr.uint8[ETX_IPV6_LAST_BYTE] = (uint8_t) p->src;
                //handle the beacon
                etx_handle_beacon(&candidate_addr);
            }

            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            //packet is not for me, whatever
        }
    }
}

void etx_update(void) {
    /*
     * Update the current ETX values, then
     * reset the received packet count for the candidates.
     *
     * This thread should never take very long, as it has to be done in less
     * than (ETX_INTERVAL - maximum jitter), otherwise the ETX calculation might
     * be faulty. TODO delete this once mutexes are in place
     *
     */
    rpl_candidate_neighbor_t * candidate;
    rpl_candidate_neighbor_t * end;

    while (true) {
        //good night
        thread_sleep();
        //Wait a tiny bit to allow for the last packet to be processed
        hwtimer_wait(HWTIMER_TICKS(200));

        for (candidate = &candidates[0], end = candidates
                + RPL_MAX_CANDIDATE_NEIGHBORS; candidate < end;
                candidate++) {

            if (candidate->used != 0) {
                //update its ETX-metric and packet count TODO mutex this
                candidate->cur_etx = candidate->packets_rx / (double) ETX_ROUNDS;
                candidate->packets_rx = 0;
                printf(
                        "Updated ETX Metric to %f for candidate used was on %d",
                        candidate->cur_etx, candidate->used);
            }
        }
    }
}
