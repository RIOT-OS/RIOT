/*
 * ETX beaconing implementation
 *
 * Copyright (C) 2013 Stephan Arndt <arndtste@zedat.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     rpl
 * @{
 * @file
 * @brief       ETX-beaconing implementation
 *
 * Implementation for ETX-based beaconing.
 *
 * @author  Stephan Arndt <arndtste@zedat.fu-berlin.de>
 * @}
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mutex.h"
#include "hwtimer.h"
#include "vtimer.h"
#include "thread.h"
#include "transceiver.h"

#include "sixlowpan/ip.h"
#include "ieee802154_frame.h"
#include "etx_beaconing.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ENABLE_DEBUG
#define ETX_BEACON_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT)
#define ETX_RADIO_STACKSIZE     (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT)
#define ETX_CLOCK_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#else
#define ETX_BEACON_STACKSIZE    (THREAD_STACKSIZE_MAIN)
#define ETX_RADIO_STACKSIZE     (THREAD_STACKSIZE_MAIN)
#define ETX_CLOCK_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#endif

/* prototytpes */
static uint8_t etx_count_packet_tx(etx_neighbor_t *candidate);
static void etx_set_packets_received(void);
static bool etx_equal_id(ipv6_addr_t *id1, ipv6_addr_t *id2);

static void *etx_beacon(void *);
static void *etx_clock(void *);
static void *etx_radio(void *);

/* Buffer */
static char etx_beacon_buf[ETX_BEACON_STACKSIZE];
static char etx_radio_buf[ETX_RADIO_STACKSIZE];
static char etx_clock_buf[ETX_CLOCK_STACKSIZE];

static uint8_t etx_send_buf[ETX_BUF_SIZE];
static uint8_t etx_rec_buf[ETX_BUF_SIZE];

/* PIDs */
static kernel_pid_t etx_beacon_pid = KERNEL_PID_UNDEF;
static kernel_pid_t etx_radio_pid = KERNEL_PID_UNDEF;
static kernel_pid_t etx_clock_pid = KERNEL_PID_UNDEF;

/* Message queue for radio */
static msg_t msg_que[ETX_RCV_QUEUE_SIZE];

/*
 * The counter for the current 'round'. An ETX beacon is sent every ETX_INTERVAL
 * u-seconds and a node computes the ETX value by comparing the the received
 * probes vs the expected probes from a neighbor every ETX_ROUND intervals.
 */
static uint8_t cur_round;

/*
 * If we have not yet reached WINDOW intervals, won't calculate the ETX just yet
 */
static char reached_window;

/*
 * This could (and should) be done differently, once the RPL implementation
 * deals with candidate neighbors in another way than just defining that every
 * possible neighbor we hear from is a parent.
 * Right now, we need to keep track of the ETX values of other nodes without
 * needing them to be in our parent array, so we have another array here in
 * which we put all necessary info for up to ETX_MAX_CANDIDATE_NEIHGBORS
 * candidates.
 */
/* Candidate array */
static etx_neighbor_t candidates[ETX_MAX_CANDIDATE_NEIGHBORS];

/*
 * Each time we send a beacon packet we need to reset some values for the
 * current 'round' (a round being the time between each sent beacon packet).
 *
 * In this time, no packet may be handled, otherwise it could assume values
 * from the last round to count for this round.
 */
static mutex_t etx_mutex = MUTEX_INIT;

static ipv6_addr_t *own_address;

static etx_probe_t *etx_get_send_buf(void)
{
    return ((etx_probe_t *) &(etx_send_buf[0]));
}

static etx_probe_t *etx_get_rec_buf(void)
{
    return ((etx_probe_t *) &(etx_rec_buf[0]));
}

void etx_show_candidates(void)
{
    etx_neighbor_t *candidate;
    etx_neighbor_t *end;

    for (candidate = &candidates[0], end = candidates
                                           + ETX_MAX_CANDIDATE_NEIGHBORS; candidate < end;
         candidate++) {
        if (candidate->used == 0) {
            break;
        }

        printf("Candidates Addr:%d\n"
               "\t cur_etx:%f\n"
               "\t packets_rx:%d\n"
               "\t packets_tx:%d\n"
               "\t used:%d\n", candidate->addr.uint8[ETX_IPV6_LAST_BYTE],
               candidate->cur_etx, candidate->packets_rx,
               etx_count_packet_tx(candidate),
               candidate->used);
    }
}

void etx_init_beaconing(ipv6_addr_t *address)
{
    own_address = address;
    //set code
    DEBUGF("ETX BEACON INIT");
    etx_send_buf[0] = ETX_PKT_OPTVAL;

    etx_beacon_pid = thread_create(etx_beacon_buf, sizeof(etx_beacon_buf),
                                   THREAD_PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                   etx_beacon, NULL, "etx_beacon");

    etx_radio_pid = thread_create(etx_radio_buf, sizeof(etx_radio_buf),
                                  THREAD_PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                  etx_radio, NULL, "etx_radio");

    etx_clock_pid = thread_create(etx_clock_buf, sizeof(etx_clock_buf),
                                  THREAD_PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                  etx_clock, NULL, "etx_clock");
    //register at transceiver
    transceiver_register(TRANSCEIVER_CC1100, etx_radio_pid);
    DEBUG("...[DONE]\n");
}

static void *etx_beacon(void *arg)
{
    (void) arg;

    /*
     * Sends a message every ETX_INTERVAL +/- a jitter-value (default is 10%) .
     * A correcting variable is needed to stay at a base interval of
     * ETX_INTERVAL between the wakeups. It takes the old jittervalue in account
     * and modifies the time to wait accordingly.
     */
    etx_probe_t *packet = etx_get_send_buf();

    while (true) {
        thread_sleep();
        mutex_lock(&etx_mutex);
        //Build etx packet
        uint8_t p_length = 0;

        for (uint8_t i = 0; i < ETX_BEST_CANDIDATES; i++) {
            if (candidates[i].used != 0) {
                packet->data[i * ETX_TUPLE_SIZE] =
                    candidates[i].addr.uint8[ETX_IPV6_LAST_BYTE];
                packet->data[i * ETX_TUPLE_SIZE + ETX_PKT_REC_OFFSET] =
                    etx_count_packet_tx(&candidates[i]);
                p_length = p_length + ETX_PKT_HDR_LEN;
            }
        }

        packet->length = p_length;
        /* will be send broadcast, so if_id and destination address will be
         * ignored (see documentation)
         */
        sixlowpan_mac_send_ieee802154_frame(0, NULL, 8, &etx_send_buf[0],
                                            ETX_DATA_MAXLEN + ETX_PKT_HDR_LEN, 1);
        DEBUG("sent beacon!\n");
        etx_set_packets_received();
        cur_round++;

        if (cur_round == ETX_WINDOW) {
            if (reached_window != 1) {
                //first round is through
                reached_window = 1;
            }

            cur_round = 0;
        }

        mutex_unlock(&etx_mutex);
    }

    return NULL;
}

etx_neighbor_t *etx_find_candidate(ipv6_addr_t *address)
{
    /*
     * find the candidate with address address and returns it, or returns NULL
     * if no candidate having this address was found.
     */
    for (uint8_t i = 0; i < ETX_MAX_CANDIDATE_NEIGHBORS; i++) {
        if (candidates[i].used
            && (etx_equal_id(&candidates[i].addr, address))) {
            return &candidates[i];
        }
    }

    return NULL ;
}

static void *etx_clock(void *arg)
{
    (void) arg;

    /*
     * Manages the etx_beacon thread to wake up every full second +- jitter
     */

    /*
     * The jittercorrection and jitter variables keep usecond values divided
     * through 1000 to fit into uint8 variables.
     *
     * That is why they are multiplied by 1000 when used for hwtimer_wait.
     */
    uint8_t jittercorrection = ETX_DEF_JIT_CORRECT;
    uint8_t jitter = (uint8_t)(rand() % ETX_JITTER_MOD);

    while (true) {
        thread_wakeup(etx_beacon_pid);

        /*
         * Vtimer is buggy, but I seem to have no hwtimers left, so using this
         * for now.
         */
        vtimer_usleep(
            ((ETX_INTERVAL - ETX_MAX_JITTER)*MS) + jittercorrection * MS + jitter * MS - ETX_CLOCK_ADJUST);

        //hwtimer_wait(
        //        HWTIMER_TICKS(((ETX_INTERVAL - ETX_MAX_JITTER)*MS) + jittercorrection*MS + jitter*MS - ETX_CLOCK_ADJUST));

        jittercorrection = (ETX_MAX_JITTER) - jitter;
        jitter = (uint8_t)(rand() % ETX_JITTER_MOD);
    }

    return NULL;
}

double etx_get_metric(ipv6_addr_t *address)
{
    etx_neighbor_t *candidate = etx_find_candidate(address);

    if (candidate != NULL) {
        if (etx_count_packet_tx(candidate) > 0) {
            //this means the current etx_value is not outdated
            return candidate->cur_etx;
        }
        else {
            //The last time I received a packet is too long ago to give a
            //good estimate of the etx value
            return 0;
        }
    }

    return 0;
}

etx_neighbor_t *etx_add_candidate(ipv6_addr_t *address)
{
    DEBUG("add candidate\n");
    /*
     * Pre-Condition:   etx_add_candidate should only be called when the
     *                  candidate is not yet in the list.
     *                  Otherwise the candidate will be added a second time,
     *                  leading to unknown behavior.
     *
     *      Check if there is still enough space to add this candidate
     *
     *      a)
     *          Space is available:
     *              Add candidate
     *
     *      b)
     *          Space is not available:
     *              ignore new candidate
     *              This shouldn't really happen though, since we have enough
     *              place in the array.
     *
     * Returns the pointer to the candidate if it was added, or a NULL-pointer
     * otherwise.
     */
    etx_neighbor_t *candidate;
    etx_neighbor_t *end;

    for (candidate = &candidates[0], end = candidates
                                           + ETX_MAX_CANDIDATE_NEIGHBORS; candidate < end;
         candidate++) {
        if (candidate->used) {
            //skip
            continue;
        }
        else {
            //We still have a free place add the new candidate
            memset(candidate, 0, sizeof(*candidate));
            candidate->addr = *address;
            candidate->cur_etx = 0;
            candidate->packets_rx = 0;
            candidate->used = 1;
            return candidate;
        }
    }

    return NULL ;
}

void etx_handle_beacon(ipv6_addr_t *candidate_address)
{
    /*
     * Handle the ETX probe that has been received and update all infos.
     * If the candidate address is unknown, try to add it to my struct.
     */

    DEBUG(
        "ETX beacon package received with following values:\n"
        "\tPackage Option:%x\n"
        "\t   Data Length:%u\n"
        "\tSource Address:%d\n\n", etx_rec_buf[ETX_PKT_OPT], etx_rec_buf[ETX_PKT_LEN],
        candidate_address->uint8[ETX_IPV6_LAST_BYTE]);

    etx_neighbor_t *candidate = etx_find_candidate(candidate_address);

    if (candidate == NULL) {
        //Candidate was not found in my list, I should add it
        candidate = etx_add_candidate(candidate_address);

        if (candidate == NULL) {
            DEBUGF("[ERROR] Candidate could not get added\n");
            DEBUG("Increase the constant ETX_MAX_CANDIDATE_NEIHGBORS\n");
            return;
        }
    }

    //I have received 1 packet from this candidate in this round
    //This value will be reset by etx_update to 0
    candidate->tx_cur_round = 1;

    // If i find my address in this probe, update the packet_rx value for
    // this candidate.
    etx_probe_t *rec_pkt = etx_get_rec_buf();

    for (uint8_t i = 0; i < rec_pkt->length / ETX_TUPLE_SIZE; i++) {
        DEBUG("\tIPv6 short Addr:%u\n"
              "\tPackets f. Addr:%u\n\n", rec_pkt->data[i * ETX_TUPLE_SIZE],
              rec_pkt->data[i * ETX_TUPLE_SIZE + ETX_PKT_REC_OFFSET]);

        if (rec_pkt->data[i * ETX_TUPLE_SIZE]
            == own_address->uint8[ETX_IPV6_LAST_BYTE]) {

            candidate->packets_rx = rec_pkt->data[i * ETX_TUPLE_SIZE
                                                  + ETX_PKT_REC_OFFSET];
        }
    }

    //Last, update the ETX value for this candidate
    etx_update(candidate);
}

static void *etx_radio(void *arg)
{
    (void) arg;

    msg_t m;
    radio_packet_t *p;

    ieee802154_frame_t frame;

    msg_init_queue(msg_que, ETX_RCV_QUEUE_SIZE);

    ipv6_addr_t ll_address;
    ipv6_addr_t candidate_addr;

    ipv6_addr_set_link_local_prefix(&ll_address);
    ipv6_net_if_get_best_src_addr(&candidate_addr, &ll_address);

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t *) m.content.ptr;

            ieee802154_frame_read(p->data, &frame, p->length);

            if (frame.payload[0] == ETX_PKT_OPTVAL) {
                //copy to receive buffer
                memcpy(etx_rec_buf, &frame.payload[0], frame.payload_len);

                //create IPv6 address from radio packet
                //we can do the cast here since rpl nodes can only have addr
                //up to 8 bits
                candidate_addr.uint8[ETX_IPV6_LAST_BYTE] = (uint8_t) p->src;
                //handle the beacon
                mutex_lock(&etx_mutex);
                etx_handle_beacon(&candidate_addr);
                mutex_unlock(&etx_mutex);
            }

            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
            DEBUGF("Transceiver buffer full\n");
        }
        else {
            //packet is not for me, whatever
        }
    }

    return NULL;
}

void etx_update(etx_neighbor_t *candidate)
{
    DEBUG("update!\n");
    /*
     * Update the current ETX value of a candidate
     */
    double d_f;
    double d_r;

    if (reached_window != 1 || candidate == NULL) {
        //We will wait at least ETX_WINDOW beacons until we decide to
        //calculate an ETX value, so that we have a good estimate
        return;
    }

    /*
     * Calculate d_f (the forward PDR) from ME to this candidate.
     */
    d_f = candidate->packets_rx / (double) ETX_WINDOW;

    /*
     * Calculate d_r (the backwards PDR) from this candidate to ME
     */
    d_r = etx_count_packet_tx(candidate) / (double) ETX_WINDOW;

    /*
     * Calculate the current ETX value for my link to this candidate.
     */
    if (d_f * d_r != 0) {
        candidate->cur_etx = 1 / (d_f * d_r);
    }
    else {
        candidate->cur_etx = 0;
    }

    DEBUG(
        "Estimated ETX Metric  is %f for candidate w/ addr %d\n"
        "Estimated PDR_forward is %f\n"
        "Estimated PDR_backwrd is %f\n"
        "\n"
        "Received Packets: %d\n"
        "Sent Packets    : %d\n\n",
        candidate->cur_etx, candidate->addr.uint8[ETX_IPV6_LAST_BYTE],
        d_f, d_r, candidate->packets_rx, etx_count_packet_tx(candidate));
}

static uint8_t etx_count_packet_tx(etx_neighbor_t *candidate)
{
    /*
     *  Counts the number of packets that were received for this candidate
     *  in the last ETX_WINDOW intervals.
     */
    DEBUG("counting packets");
    uint8_t pkt_count = 0;
    DEBUG("[");

    for (uint8_t i = 0; i < ETX_WINDOW; i++) {
        if (i != cur_round) {
            pkt_count = pkt_count + candidate->packets_tx[i];
            DEBUG("%d", candidate->packets_tx[i]);

            if (i < ETX_WINDOW - 1) {
                DEBUG(",");
            }
        }
        else {
            //Check if I received something for the current round
            if (candidate->tx_cur_round == 0) {
                //Didn't receive a packet, zero the field and don't add
                candidate->packets_tx[i] = 0;
                DEBUG("%d!", candidate->packets_tx[i]);

                if (i < ETX_WINDOW - 1) {
                    DEBUG(",");
                }
            }
            else {
                //Add 1 and set field
                pkt_count = pkt_count + 1;
                candidate->packets_tx[i] = 1;
                DEBUG("%d!", candidate->packets_tx[i]);

                if (i < ETX_WINDOW - 1) {
                    DEBUG(",");
                }
            }
        }
    }

    DEBUG("]\n");
    return pkt_count;
}

static void etx_set_packets_received(void)
{
    /*
     * Set for all candidates if they received a packet this round or not
     */
    for (uint8_t i = 0; i < ETX_MAX_CANDIDATE_NEIGHBORS; i++) {
        if (candidates[i].used) {
            if (candidates[i].tx_cur_round != 0) {
                candidates[i].packets_tx[cur_round] = 1;
                candidates[i].tx_cur_round = 0;
            }
        }
    }
}

bool etx_equal_id(ipv6_addr_t *id1, ipv6_addr_t *id2)
{
    for (uint8_t i = 0; i < 4; i++) {
        if (id1->uint32[i] != id2->uint32[i]) {
            return false;
        }
    }

    return true;

}
