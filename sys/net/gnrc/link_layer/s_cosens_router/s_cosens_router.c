/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 * @file    s_cosens_router.c
 * @brief   Implementation of the S-CoSenS MAC/RDC protocol
 *          for router/PAN-coordinator nodes under RIOT OS
 * @details Implementation of the S-CoSenS MAC/radio duty cycling protocol;
 *          designed by Bilel Nefzi and Ye-Qiong Song, see:
 *          B. Nefzi PhD thesis: "Mécanismes auto-adaptatifs pour la
 *          gestion de la Qualité de Service dans les réseaux de capteurs
 *          sans-fil", chapter 5, Networking and Internet Architecture,
 *          Institut National Polytechnique de Lorraine (INPL), 2011.
 *          (document in French language)
 * @author  Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#include <errno.h>

#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "xtimer.h"
#include "net/gnrc/s_cosens_router.h"
#include "net/gnrc/csma_sender.h"
#include "net/gnrc.h"
#include "net/ieee802154.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif


/****************************************************************************/
/*    Constants/config. parameters of the S-CoSenS protocol's MAC driver    */
/****************************************************************************/

#define MAX_OUTGOING_PACKETS_NUMBER   (16)

#define MAC_MAX_FRAME_RETRIES          (7)

#define SUBFRAME_DURATION_MICROSEC   (100000UL)

#define MIN_WP_DURATION_MICROSEC      (50000UL)
#define MAX_WP_DURATION_MICROSEC     (100000UL)

#define DEFAULT_WP_DURATION_MICROSEC \
          ((MIN_WP_DURATION_MICROSEC + MAX_WP_DURATION_MICROSEC) >> 1)


#define ALPHA_PERCENT   1


#define OFF_PERIOD_IN_SLEEP_PERIOD_MICROSEC   (1000UL)
#define ON_PERIOD_IN_SLEEP_PERIOD_MICROSEC     (100UL)


/****************************************************************************/
/*          The following constants are normally not to be changed          */
/*                (unless you change the protocol internals)                */
/****************************************************************************/

#define S_COSENS_BEACON_SIZE   (17)


/****************************************************************************/
/*           Variables of the S-CoSenS protocol's MAC/RDC driver            */
/****************************************************************************/


/* underlying radio driver */
gnrc_netdev_t *dev = NULL;


/* S-CoSenS thread ID */
static volatile kernel_pid_t s_cosens_tid;

/* internal message types for S-CoSenS */
typedef enum {
    S_COSENS_SWITCH_PERIOD = 1977,
    S_COSENS_SEND_BEACON,
    S_COSENS_TOGGLE_RADIO,
    S_COSENS_EMIT_PACKET,
} s_cosens_msg_t;

/* period in which we currently are */
static volatile enum {
  INIT_PERIOD,
  SLEEP_PERIOD,
  WAIT_PERIOD,
  TRANSMIT_PERIOD,
  INVALID_PERIOD = -1
} s_cosens_router_period;

/* timer managing the period changes */
static xtimer_t period_change_timer;

/* timer managing the transceuver toggling during sleeping period */
static xtimer_t sleep_on_off_timer;

/* outgoing paquets' queue */
static gnrc_pktsnip_t outgoing_queue[MAX_OUTGOING_PACKETS_NUMBER];
static gnrc_pktsnip_t * volatile first_outgoing_packet;
/* occupation rate of the outgoing queue: useful for debugging purposes */
unsigned int outgoing_packets_queue_length = 0;
unsigned int max_out_queue_length = 0;
uint32_t total_out_queue_length = 0;
uint32_t elapsed_cycles = 0;


/* start time of the latest transmission period */
static uint32_t tp_start;

/* duration of the current cycle's sleeping period */
static uint32_t sp_us;
/* duration of the current cycle's waiting period */
static uint32_t wp_us;

/* parameters for computing waiting periods durations */
static uint32_t tp_mean_duration;          /* mean duration of tx periods */
static int packets_number_mean;            /* mean number of txed packets */

/* number of outgoing packets transmitted during latest transmission period */
static volatile int latest_tx_packets_number;


/* flag indicating whether we can transmit in burst (fast) mode,
   or whether we must use classical CSMA/CA method. */
static bool in_burst_mode = false;
/* number of attempted transmission for the packet currently emitted;
   used in CSMA/CA mode */
static int num_tx = 0;


/* statistics on packets TX issue */
volatile unsigned long stat_tx_cancelled = 0;
volatile unsigned long stat_tx_failed = 0;
volatile unsigned long stat_tx_success = 0;
volatile unsigned long stat_success_retry[8] = {0, 0, 0, 0, 0, 0, 0, 0};


/* number of SFDs detected */
static uint32_t sfd_count = 0;


/****************************************************************************/
/*                        S-CoSenS utility functions                        */
/****************************************************************************/

/* enqueue a packet for transmission */
static bool push_out_packet(gnrc_pktsnip_t * in_pkt)
{
    /* look for an empty out_pkt_t descriptor */
    for (int i = 0; i < MAX_OUTGOING_PACKETS_NUMBER; i++) {
        if (outgoing_queue[i].data == NULL) {
            /* found: initialize the packet descriptor accordingly... */
            outgoing_queue[i].users = in_pkt->users++;
            outgoing_queue[i].next = NULL;
            outgoing_queue[i].data = in_pkt->data;
            outgoing_queue[i].size = in_pkt->size;
            outgoing_queue[i].type = GNRC_NETTYPE_UNDEF;
            /* ... and put it at the end of the list */
            if (first_outgoing_packet == NULL) {
                first_outgoing_packet = &(outgoing_queue[i]);
            } else {
                gnrc_pktsnip_t *node = first_outgoing_packet;
                while (node->next != NULL) {
                    node = node->next;
                }
                node->next = &(outgoing_queue[i]);
            }
            return true;
        }
    }
    /* if we arrive here, there is no more slots
       available for outgoing packets */
    return false;
}

/* free a slot of the outgoing packets queue */
static void pop_packet_from_queue(void)
{
    if (first_outgoing_packet == NULL) {
        /* no enqueued packet */
        return;
    }

    gnrc_pktsnip_t *node = first_outgoing_packet;

    first_outgoing_packet = node->next;

    node->users = 0;
    node->next = NULL;
    node->data = NULL;
    node->size = 0;
    node->type = GNRC_NETTYPE_UNDEF;
}

static inline void stop_radio(void)
{
    /* set the transceiver state to sleep (low power) mode */
    static const netopt_state_t on = NETOPT_STATE_SLEEP;
    int res = dev->driver->set(dev,
                               NETOPT_STATE,
                               (void *) &on,
                               sizeof(netopt_state_t));
    if (res < 0) {
        core_panic(PANIC_ASSERT_FAIL,
                   "Unable to stop (put to sleep) radio transceiver");
    }
}

static inline void start_radio(void)
{
    /* get the transceiver state out of sleep (low power) mode */
    static const netopt_state_t on = NETOPT_STATE_IDLE;
    int res = dev->driver->set(dev,
                               NETOPT_STATE,
                               (void *) &on,
                               sizeof(netopt_state_t));
    if (res < 0) {
        core_panic(PANIC_ASSERT_FAIL,
                   "Unable to start (get out of sleep) radio transceiver");
    }
}

static inline void send_s_cosens_message(s_cosens_msg_t msg_type,
                                         uint32_t param)
{
    msg_t new_msg;
    new_msg.type = msg_type;
    new_msg.content.value = param;
    int res = msg_try_send(&new_msg, s_cosens_tid);
    if (res < 1) {
        core_panic(res, "Couldn't send message to S-CoSenS thread");
    }
}


/****************************************************************************/
/*                   S-CoSenS period-management functions                   */
/****************************************************************************/

/* executing transmission period
   i.e.: handle the emission of outgoing packets, one after another */
static void do_transmit(void)
{
    /* outcome of the radio transmission */
    int result;
    /* next packet to send */
    gnrc_pktsnip_t *curr_pkt = first_outgoing_packet;

    /* if all transmissions have been done, begin a new cycle */
    if (curr_pkt == NULL) {
        DEBUG("s_cosens_router: no (more) packet to send.\n");
        send_s_cosens_message(S_COSENS_SWITCH_PERIOD, INIT_PERIOD);
        return;
    }

    DEBUG("s_cosens_router: now emitting packet...\n");
    if (!in_burst_mode) {
        /* if next transmission must use the CSMA/CA method... */
        result = csma_ca_send(dev, curr_pkt);
    } else {
        /* we go directly here if transmission can be done in burst mode */
        result = cca_send(dev, curr_pkt);
    }

    /* check how transmission went */
    if (result == curr_pkt->size) {
        /* success */
        DEBUG("s_cosens_router: packet successfully transmitted.\n");
        /* remove the packet successfully sent from queue
           and free the packet descriptor... */
        pop_packet_from_queue();
        /* ... reset counter of attempted transmissions... */
        num_tx = 0;
        /* ... can switch to "faster" burst mode... */
        in_burst_mode = true;
        DEBUG("s_cosens_router: switching to burst transmission mode.\n");

    } else {
        if (result < 0) {
            in_burst_mode = false;
            bool fatal;
            char errmsg[80];
            switch (result) {
            case -EBUSY:
                DEBUG("Radio medium busy; will try again...\n");
                fatal = false;
            case -ENODEV:
                sprintf(errmsg, "Invalid radio device!");
                fatal = true;
                break;
            case -ENOMSG:
                sprintf(errmsg, "Invalid packet to send!");
                fatal = true;
                break;
            case -EOVERFLOW:
                sprintf(errmsg, "Packet too long (%d bytes, max = 127)!",
                        curr_pkt->size);
                fatal = true;
                break;
            case -ECANCELED:
            default:
                sprintf(errmsg, "Internal driver error!");
                fatal = true;
                break;
            }
            /* halt system in case of fatal error */
            if (fatal) {
                core_panic(PANIC_ASSERT_FAIL, errmsg);
                // we won't go past here!!
            }
        }

        num_tx++;
        /* too much failed attempts: give up! */
        if (num_tx > MAC_MAX_FRAME_RETRIES) {
            DEBUG("s_cosens_router: UNABLE TO EMIT PACKET"
                  " AFTER %d ATTEMPTS!\n", num_tx);
            DEBUG("s_cosens_router: GIVING UP TRANSMISSION\n");
            /* remove the failed packet from queue
               and free its corresponding slot */
            pop_packet_from_queue();
            num_tx = 0;
        }

    }

    /* start next transmission */
    send_s_cosens_message(S_COSENS_EMIT_PACKET, 0L);
}

/* switching to transmission period */
static void begin_transmit(void)
{
    DEBUG("s_cosens_router: switching to transmission period...\n");

    /* turn radio transceiver on to send packets */
    start_radio();

    /* switch to transmission period */
    s_cosens_router_period = TRANSMIT_PERIOD;
    tp_start = xtimer_now();

    /* reset the counter of transmitted packets */
    latest_tx_packets_number = 0;

    /* TX always begin in "safe" CSMA/CA mode */
    in_burst_mode = false;

    num_tx = 0;

    /* send first outgoing packet ASAP */
    send_s_cosens_message(S_COSENS_EMIT_PACKET, 0L);
}

/* switching to waiting period */
static void begin_wait(void)
{
    DEBUG("s_cosens_router: switching to waiting period...\n");

    /* we are now in waiting period */
    s_cosens_router_period = WAIT_PERIOD;

    /* turn radio transceiver on to receive packets */
    start_radio();

    /* sets the timer that will switch to transmission period */
    static msg_t msg_go_tp = {
        .type = S_COSENS_SWITCH_PERIOD,
        .content.value = TRANSMIT_PERIOD
    };
    xtimer_set_msg(&period_change_timer,
                   wp_us,
                   &msg_go_tp,
                   s_cosens_tid);
}


/* message to handle next radio toggling */
static msg_t msg_radio_action = {
    .type = S_COSENS_TOGGLE_RADIO,
    .content.value = 0L
};

/* toggle radio transceiver on/off during sleep period */
static void toggle_radio_transceiver(void)
{
    if (s_cosens_router_period != SLEEP_PERIOD) {
        /* if we are already out of sleeping period, cancel */
        return;
    }

    /* get the transceiver state */
    netopt_state_t state;
    int res = dev->driver->get(dev,
                               NETOPT_STATE,
                               (void *) &state,
                               sizeof(netopt_state_t));
    if (res < 0) {
        core_panic(PANIC_GENERAL_ERROR,
                   "Unable to get radio transceiver's state");
    }

    /* shall we turn rado on or off? */
    switch (state) {
    case NETOPT_STATE_SLEEP:
        /* turn on radio for a short time */
        start_radio();
        xtimer_set_msg(&sleep_on_off_timer,
                       ON_PERIOD_IN_SLEEP_PERIOD_MICROSEC,
                       &msg_radio_action,
                       s_cosens_tid);
        break;
    case NETOPT_STATE_IDLE:
        /* nothing detected on radio medium, turn off for a "long" time */
        stop_radio();
        xtimer_set_msg(&sleep_on_off_timer,
                       OFF_PERIOD_IN_SLEEP_PERIOD_MICROSEC,
                       &msg_radio_action,
                       s_cosens_tid);
        break;
    default:
        /* radio transceiver is processing something:
           wait a little and try again */
        xtimer_set_msg(&sleep_on_off_timer,
                       ON_PERIOD_IN_SLEEP_PERIOD_MICROSEC,
                       &msg_radio_action,
                       s_cosens_tid);
    }
}

/* switching to sleeping period */
static void begin_sleep(void)
{
    DEBUG("s_cosens_router: switching to sleeping period...\n");

    /* we are now in waiting period */
    s_cosens_router_period = SLEEP_PERIOD;

    /* turn radio transceiver on to receive packets */
    start_radio();

    /* sets the timer that will switch to waiting period */
    static msg_t msg_go_tp = {
        .type = S_COSENS_SWITCH_PERIOD,
        .content.value = WAIT_PERIOD
    };
    xtimer_set_msg(&period_change_timer,
                   sp_us,
                   &msg_go_tp,
                   s_cosens_tid);

    /* first check of incoming packet in a "long" time */
    xtimer_set_msg(&sleep_on_off_timer,
                   OFF_PERIOD_IN_SLEEP_PERIOD_MICROSEC,
                   &msg_radio_action,
                   s_cosens_tid);
}


/* broadcast the adequate beacon packet to the PAN --
   called by S-CoSenS thread function */
static void send_beacon(void)
{
    /* contents of the beacon packet */
    uint8_t beacon_buf[S_COSENS_BEACON_SIZE];

    /* get PAN and router address */
    uint16_t pan, addr;
    int res = dev->driver->get(dev,
                               NETOPT_NID,
                               (void *) &pan,
                               sizeof(uint16_t));
    if (res <= 0) {
        core_panic(PANIC_GENERAL_ERROR,
                   "Cannot get PAN ID from device");
        /* won't go past here */
    }
    res = dev->driver->get(dev,
                           NETOPT_ADDRESS,
                           (void *) &addr,
                           sizeof(uint16_t));
    if (res <= 0) {
        core_panic(PANIC_GENERAL_ERROR,
                   "Cannot get router address from device");
        /* won't go past here */
    }

    /* build the IEEE 802.15.4 header of the beacon */
        /* a S-CoSenS "beacon" is not a standard 802.15.4 beacon:
           it's a special, short kind of IEEE 802.15.4 data packet,
           without security, ACK request or "frame pending" signal,
           for intra-PAN communication */
    beacon_buf[0] = IEEE802154_FCF_TYPE_DATA |
                    IEEE802154_FCF_PAN_COMP;
        /* we use short (intra-PAN) addresses, and
           use the very first version of FCF */
    beacon_buf[1] = IEEE802154_FCF_DST_ADDR_SHORT |
                    IEEE802154_FCF_VERS_V0 |
                    IEEE802154_FCF_SRC_ADDR_SHORT;
        /* PAN ID */
    beacon_buf[2] = (pan & 0xff);
    beacon_buf[3] = (pan >> 8);
        /* Destination address ! broadcast */
    beacon_buf[4] = 0xff;
    beacon_buf[5] = 0xff;
        /* Short source address, of this router */
    beacon_buf[6] = (addr & 0xff);
    beacon_buf[7] = (addr >> 8);
    /* build the beacon packet's payload */
    beacon_buf[8] = 0xee;                 /* identifies a "beacon" for S-CoSens */
    /* next sleep period duration in us */
    beacon_buf[9] = (sp_us & 0xff);
    beacon_buf[10] = (sp_us >> 8);
    beacon_buf[11] = (sp_us >> 16);
    beacon_buf[12] = (sp_us >> 24);
    /* next waiting period duration in us */
    beacon_buf[13] = (wp_us & 0xff);
    beacon_buf[14] = (wp_us >> 8);
    beacon_buf[15] = (wp_us >> 16);
    beacon_buf[16] = (wp_us >> 24);


    /* prepare the beacon packet */
    DEBUG("s_cosens_router: preparing beacon packet... ");
    gnrc_pktsnip_t *beacon = gnrc_pktbuf_add(NULL,
                                             beacon_buf,
                                             S_COSENS_BEACON_SIZE,
                                             GNRC_NETTYPE_UNDEF);
    if (beacon == NULL) {
        core_panic(PANIC_GENERAL_ERROR,
                   "s_cosens_router: cannot reserve space for beacon packet");
        // we won't go past here!!
    }

    /* send the prepared beacon packet */
    DEBUG("s_cosens_router: broadcasting beacon packet... ");
    int result = cca_send(dev, beacon);

    if (result > 0) {
        /* OK: begin sleep period ASAP */
        DEBUG("Done.\n");
        send_s_cosens_message(S_COSENS_SWITCH_PERIOD, SLEEP_PERIOD);
        return;
    } else if (result == -EBUSY) {
        /* medium busy/collision: backoff for a while, then retry */
        DEBUG("radio channel busy!\n");
        static msg_t msg_wait_beacon = {
            .type = S_COSENS_SEND_BEACON,
            .content.value = 0L
        };
        xtimer_set_msg(&sleep_on_off_timer,
                       A_UNIT_BACKOFF_PERIOD_MICROSEC,
                       &msg_wait_beacon,
                       s_cosens_tid);
        return;
    } else {
        /* fatal transmission error */
        core_panic(result, "s_cosens_router: FATAL! ERROR OCCURED DURING"
                           " BEACON TRANSMISSION (RADIO FAILURE)! ***");
        // we won't go past here!!
    }

    /* release slot from packet buffers */
    gnrc_pktbuf_release(beacon);
}

/* initialize a new S-CoSenS duty cycle, by computing period durations --
   called by S-CoSenS thread function */
static void init_cycle(void)
{
    elapsed_cycles++;
    total_out_queue_length += outgoing_packets_queue_length;

    /* calculates the duration of the upcoming waiting period */
    if (tp_start == 0) {
        tp_start = xtimer_now();
    }
    uint32_t tp_duration = xtimer_now() - tp_start;

    DEBUG("s_cosens_router: *** beginning new duty cycle ***\n");
    s_cosens_router_period = INIT_PERIOD;

    /* get initial values */
    if (tp_mean_duration == 0) {
        tp_mean_duration = DEFAULT_WP_DURATION_MICROSEC;
    }
    if (packets_number_mean == 0) {
        packets_number_mean = latest_tx_packets_number;
    }

    /* empty cycles do not change computed means */
    if (latest_tx_packets_number > 0) {
        tp_mean_duration = ( ((100 - ALPHA_PERCENT) * tp_mean_duration)
                             + (ALPHA_PERCENT  * tp_duration) )
                           / 100;

        packets_number_mean = ( ((100 - ALPHA_PERCENT) * packets_number_mean)
                                     + (ALPHA_PERCENT  * latest_tx_packets_number) )
                              / 100;
    }
    /* ensure the mean doesn't fall under ONE packet */
    if (packets_number_mean <= 1) {
        packets_number_mean = 1;
    }

    DEBUG("tpmean = %lu, packmean = %lu \n",
          tp_mean_duration,
          packets_number_mean);

    wp_us = (packets_number_mean - 1) * A_UNIT_BACKOFF_PERIOD_MICROSEC
          + tp_mean_duration + 1;
    sp_us = SUBFRAME_DURATION_MICROSEC - wp_us - 1;

    DEBUG("s_cosens_router: sleeping period will be %lu us long.\n", sp_us);
    DEBUG("s_cosens_router: next waiting period will be %lu us long.\n", wp_us);

    /* (try to) send this beacon packet */
    send_s_cosens_message(S_COSENS_SEND_BEACON, 0L);
}


/****************************************************************************/
/*           Generic event-management function of S-CoSenS thread           */
/****************************************************************************/

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
static void _s_cosens_event_cb(gnrc_netdev_event_t event, void *data)
{
    gnrc_pktsnip_t *pkt;

    DEBUG("s-cosens-router: event triggered -> %i\n", event);
    /* S-CoSenS managing events from radio */
    switch (event) {

    case NETDEV_EVENT_RX_STARTED:
        /* SFD detected! */
        sfd_count++;
        break;

    case NETDEV_EVENT_RX_COMPLETE:
        /* packet successfully received */

        /* get pointer to the received packet */
        pkt = (gnrc_pktsnip_t *)data;
        /* send the packet to everyone interested in it's type */
        if (!gnrc_netapi_dispatch_receive(pkt->type,
                                          GNRC_NETREG_DEMUX_CTX_ALL,
                                          pkt))
        {
            DEBUG("s-cosens-router: unable to forward packet of type %i\n",
                  pkt->type);
            gnrc_pktbuf_release(pkt);
        }

        break;

    default:
        /* we don't care about other events */
        break;
    }
}


/****************************************************************************/
/*                     Main function of S-CoSenS thread                     */
/****************************************************************************/

/**
 * @brief   Startup code and event loop of the S-CoSenS node MAC/RDC layer
 *
 * @param[in] args          expects a pointer to the underlying netdev device
 *
 * @return                  never returns
 */
static void * _s_cosens_node_thread_func(void * args)
{
    char errmsg[80];

    dev = (gnrc_netdev_t *)args;
    gnrc_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[GNRC_S_COSENS_ROUTER_MSG_QUEUE_SIZE];

    DEBUG("s_cosens_router: initializing... \n");
    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, GNRC_S_COSENS_ROUTER_MSG_QUEUE_SIZE);
    /* save the PID to the device descriptor and register the device */
    dev->mac_pid = thread_getpid();
    gnrc_netif_add(dev->mac_pid);
    /* register the event callback with the device driver */
    dev->driver->add_event_callback(dev, _s_cosens_event_cb);

    /* intialize the outgoing packets' queue */
    for (int i = 0; i < MAX_OUTGOING_PACKETS_NUMBER; i++) {
        outgoing_queue[i].next = NULL;
        outgoing_queue[i].data = NULL;
        outgoing_queue[i].size = 0;
    }
    first_outgoing_packet = NULL;

    /* start the event loop */
    DEBUG("s_cosens_router: going ON...\n");
    while (1) {

        /* We begin by a first cycle */
        init_cycle();

        DEBUG("s_cosens_router: waiting for incoming messages\n");
        msg_receive(&msg);

        switch (msg.type) {
            /*---- dispatch S-CoSenS-specific messages ----*/

            /* Switching from a S-CoSenS period to another */
            case S_COSENS_SWITCH_PERIOD:
                switch (msg.content.value) {
                case INIT_PERIOD:
                    init_cycle();
                    break;
                case SLEEP_PERIOD:
                    begin_sleep();
                    break;
                case WAIT_PERIOD:
                    begin_wait();
                    break;
                case TRANSMIT_PERIOD:
                    begin_transmit();
                    break;
                default:
                    core_panic(msg.content.value,
                               "S-CoSenS tried to switch to an unknown period");
                }
                break;

            /* Broadcast a beacon to the PAN */
            case S_COSENS_SEND_BEACON:
                send_beacon();
                break;

            /* Toggle radio on and off during sleeping period,
               to be able to receive data from other PANs */
            case S_COSENS_TOGGLE_RADIO:
                toggle_radio_transceiver();
                break;

            /* Launch the transmission of a enqueued packet */
            case S_COSENS_EMIT_PACKET:
                do_transmit();
                break;

            /*---- dispatch NETDEV and NETAPI messages ----*/

            /* Generic NETDEV (radio) event */
            case GNRC_NETDEV_MSG_TYPE_EVENT:
                DEBUG("s_cosens_router: GNRC_NETDEV_MSG_TYPE_EVENT received\n");
                dev->driver->isr_event(dev, msg.content.value);
                break;

            /* Enqueue packet for sending */
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("s_cosens_router: GNRC_NETAPI_MSG_TYPE_SND received\n");
                bool ok = push_out_packet((gnrc_pktsnip_t *) msg.content.ptr);
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t) ok;
                msg_reply(&msg, &reply);
                break;

            /* get specific option */
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("s_cosens_router: GNRC_NETAPI_MSG_TYPE_SET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* set option for device driver */
                res = dev->driver->set(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("s_cosens_router: response of netdev->set: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;

            /* set specific option */
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("s_cosens_router: GNRC_NETAPI_MSG_TYPE_GET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* get option from device driver */
                res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("s_cosens_router: response of netdev->get: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;

            /* unknown message: crash the program */
            default:
                sprintf(errmsg,
                        "s_cosens_router: Unknown command %" PRIu16 "\n",
                        msg.type);
                core_panic(PANIC_ASSERT_FAIL, errmsg);
                /* won't go past here */
                break;
        }
    }
    /* never reached */
    return NULL;
}


/****************************************************************************/
/*    Initialization function exported by the S-CoSenS protocol's driver    */
/****************************************************************************/

kernel_pid_t gnrc_s_cosens_router_init(char *stack,
                                       int stacksize,
                                       char priority,
                                       const char *name,
                                       gnrc_netdev_t *dev)
{
    /* define the CSMA/CA parameters */
    set_csma_mac_min_be(S_COSENS_ROUTER_MAC_MIN_BE);
    set_csma_mac_max_be(S_COSENS_ROUTER_MAC_MAX_BE);
    set_csma_mac_max_csma_backoffs(S_COSENS_ROUTER_MAC_MAX_CSMA_BACKOFFS);

    /* check if given netdev device is defined and the driver is set */
    if (dev == NULL || dev->driver == NULL) {
        return -ENODEV;
    }

    /* create the new S-CoSenS thread */
    s_cosens_tid = thread_create(stack,
                                 stacksize,
                                 priority,
                                 CREATE_STACKTEST,
                                 _s_cosens_node_thread_func,
                                 (void *)dev,
                                 name);
    if (s_cosens_tid <= 0) {
        return -EINVAL;
    }

    return s_cosens_tid;
}
