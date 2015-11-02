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
 * @file    s_cosens_node.c
 * @brief   Implementation of the S-CoSenS MAC/RDC protocol
 *          for simple/leaf nodes under RIOT OS
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
#include "net/gnrc/s_cosens_node.h"
#include "net/gnrc/csma_sender.h"
#include "net/gnrc.h"

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
    S_COSENS_EMIT_PACKET,
} s_cosens_msg_t;

/* period in which we currently are */
static volatile enum {
  IDLE_PERIOD,
  WAIT_PERIOD,
  SLEEP_PERIOD,
  TRANSMIT_PERIOD,
  END_TRANSMIT_PERIOD,
  INVALID_PERIOD = -1
} s_cosens_node_period;

/* timer managing the period changes */
static xtimer_t period_change_timer;

/* outgoing paquets' queue */
static gnrc_pktsnip_t outgoing_queue[MAX_OUTGOING_PACKETS_NUMBER];
static gnrc_pktsnip_t * volatile first_outgoing_packet;
/* occupation rate of the outgoing queue: useful for debugging purposes */
unsigned int outgoing_packets_queue_length = 0;
unsigned int max_out_queue_length = 0;
uint32_t total_out_queue_length = 0;
uint32_t elapsed_cycles = 0;


/* Address of the PAN router (found in beacon packets) */
static volatile uint16_t router_id;

/* Length, in microseconds, for the next sleeping period
    and the next transmission period for S-CoSens nodes
    (given by the PAN router in the latest beacon) */
static volatile uint32_t sleep_us;
static volatile uint32_t tx_us;

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


static inline bool beacon_in_buf(void *buf)  // TODO adapt to all header sizes!
{
    /* check if the packet in the given buffer is a S-CoSens beacon */
    uint8_t *p = buf;
    return (p[8] == 0xee);
}

static inline void parse_beacon_in_buf(void *buf)
{
    /* get the duration values from the S-CoSens beacon in the given buffer */
    uint8_t *p = buf;
    sleep_us = ((unsigned long)(p[12]) << 24)
             | ((unsigned long)(p[11]) << 16)
             | ((unsigned long)(p[10]) << 8)
             |  (unsigned long)(p[9]);
    tx_us = ((unsigned long)(p[16]) << 24)
          | ((unsigned long)(p[15]) << 16)
          | ((unsigned long)(p[14]) << 8)
          |  (unsigned long)(p[13]);
    router_id = ((unsigned int)(p[7]) << 8)
              |  (unsigned int)(p[6]);
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

/* duration allocated for transmission period is over */
static void end_transmit(void)
{
    /* if we are already out of TX period, do nothing */
    if(s_cosens_node_period != TRANSMIT_PERIOD) return;

    /* if all outgoing packets are transmitted ... */
    gnrc_pktsnip_t *curr_pkt = first_outgoing_packet;
    if (curr_pkt == NULL) {
        /* ... we can go to idle mode ... */
        DEBUG("s_cosens_node: no (more) packet to send.\n");
        send_s_cosens_message(S_COSENS_SWITCH_PERIOD, IDLE_PERIOD);
    } else {
        /* ... otherwise, begin a new duty cycle (wait for a beacon) */
        send_s_cosens_message(S_COSENS_SWITCH_PERIOD, WAIT_PERIOD);
    }
}

/* executing transmission period
   i.e.: handle the emission of outgoing packets, one after another */
static void do_transmit(void)
{
    /* outcome of the radio transmission */
    int result;
    /* next packet to send */
    gnrc_pktsnip_t *curr_pkt = first_outgoing_packet;

    /* if we are already out of TX period, do nothing */
    if (s_cosens_node_period != TRANSMIT_PERIOD) {
        return;
    }

    /* if all transmissions have been done, switch to waiting mode */
    if (curr_pkt == NULL) {
        DEBUG("s_cosens_node: no (more) packet to send.\n");
        send_s_cosens_message(S_COSENS_SWITCH_PERIOD, IDLE_PERIOD);
        return;
    }

    DEBUG("s_cosens_node: now emitting packet...\n");
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
        DEBUG("s_cosens_node: packet successfully transmitted.\n");
        /* remove the packet successfully sent from queue
           and free the packet descriptor... */
        pop_packet_from_queue();
        /* ... reset counter of attempted transmissions... */
        num_tx = 0;
        /* ... can switch to "faster" burst mode... */
        in_burst_mode = true;
        DEBUG("s_cosens_node: switching to burst transmission mode.\n");

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
            DEBUG("s_cosens_node: UNABLE TO EMIT PACKET"
                  " AFTER %d ATTEMPTS!\n", num_tx);
            DEBUG("s_cosens_node: GIVING UP TRANSMISSION\n");
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
    DEBUG("s_cosens_node: switching to transmission period...\n");

    /* turn radio transceiver on to send packets */
    start_radio();

    /* switch to transmission period */
    s_cosens_node_period = TRANSMIT_PERIOD;

    /* TX always begin in "safe" CSMA/CA mode */
    in_burst_mode = false;

    num_tx = 0;

    /* transmission period ends after the delay given by router */
    /* switch to transmission period after the appropriate delay */
    static msg_t msg_end_tp = {
        .type = S_COSENS_SWITCH_PERIOD,
        .content.value = END_TRANSMIT_PERIOD
    };
    xtimer_set_msg(&period_change_timer,
                   tx_us,
                   &msg_end_tp,
                   s_cosens_tid);

    /* send first outgoing packet ASAP */
    send_s_cosens_message(S_COSENS_EMIT_PACKET, 0L);
}

/* switching to sleeping period */
static void begin_sleep(void)
{
    DEBUG("s_cosens_node: switching to sleeping period...\n");

    /* turn radio transceiver off to spare energy */
    stop_radio();

    /* go into sleeping period */
    s_cosens_node_period = SLEEP_PERIOD;

    /* switch to transmission period after the appropriate delay */
    static msg_t msg_go_tp = {
        .type = S_COSENS_SWITCH_PERIOD,
        .content.value = TRANSMIT_PERIOD
    };
    xtimer_set_msg(&period_change_timer,
                   sleep_us,
                   &msg_go_tp,
                   s_cosens_tid);
}

/* switching to waiting period */
static void begin_wait(void)
{
    elapsed_cycles++;
    total_out_queue_length += outgoing_packets_queue_length;

    DEBUG("s_cosens_node: switching to waiting period...\n");

    /* turn radio transceiver on to receive packets */
    start_radio();

    /* go into waiting period */
    s_cosens_node_period = WAIT_PERIOD;
    /* a node stays in waiting period until a beacon is received */
}

/* switching to idle (inactive) period */
static void begin_idle(void)
{
    if (first_outgoing_packet != NULL) {
        /* if a packet has arrived while period change was processed,
           get back online */
        begin_wait();
        return;
    }

    DEBUG("s_cosens_node: going idle, ZZZ...\n");

    /* turn radio transceiver off to spare energy */
    stop_radio();

    /* go into idle period */
    s_cosens_node_period = IDLE_PERIOD;
    /* a node stays in idle period as long as there is no data to transmit */
}


/****************************************************************************/
/*  Event-management function of S-CoSenS thread: handles incoming beacons  */
/****************************************************************************/

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
static void _s_cosens_node_event_cb(gnrc_netdev_event_t event, void *data)
{
    gnrc_pktsnip_t *pkt;

    DEBUG("s_cosens_node: event triggered -> %i\n", event);
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

        /* check if it's an s-cosens beacon */
        void *buf = pkt->data;
        if (beacon_in_buf(buf)) {
            parse_beacon_in_buf(buf);
            /* an S-CoSenS beacon is not meant
               to be relayed anywhere else */
            break;
        }

        /* send the packet to everyone interested in it's type */
        if (!gnrc_netapi_dispatch_receive(pkt->type,
                                          GNRC_NETREG_DEMUX_CTX_ALL,
                                          pkt))
        {
            DEBUG("s_cosens_node: unable to forward packet of type %i\n",
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
    msg_t msg, reply, msg_queue[GNRC_S_COSENS_NODE_MSG_QUEUE_SIZE];

    DEBUG("s_cosens_node: initializing... \n");
    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, GNRC_S_COSENS_NODE_MSG_QUEUE_SIZE);
    /* save the PID to the device descriptor and register the device */
    dev->mac_pid = thread_getpid();
    gnrc_netif_add(dev->mac_pid);
    /* register the event callback with the device driver */
    dev->driver->add_event_callback(dev, _s_cosens_node_event_cb);

    /* intialize the outgoing packets' queue */
    for (int i = 0; i < MAX_OUTGOING_PACKETS_NUMBER; i++) {
        outgoing_queue[i].next = NULL;
        outgoing_queue[i].data = NULL;
        outgoing_queue[i].size = 0;
    }
    first_outgoing_packet = NULL;

    /* start the event loop */
    DEBUG("s-cosens-node: going ON...\n");
    while (1) {

        /* The very first period is an idle period */
        begin_idle();

        DEBUG("s_cosens_node: waiting for incoming messages\n");
        msg_receive(&msg);

        switch (msg.type) {
            /*---- dispatch S-CoSenS-specific messages ----*/

            /* Switching from a S-CoSenS period to another */
            case S_COSENS_SWITCH_PERIOD:
                switch (msg.content.value) {
                case IDLE_PERIOD:
                    begin_idle();
                    break;
                case WAIT_PERIOD:
                    begin_wait();
                    break;
                case SLEEP_PERIOD:
                    begin_sleep();
                    break;
                case TRANSMIT_PERIOD:
                    begin_transmit();
                    break;
                case END_TRANSMIT_PERIOD:
                    end_transmit();
                    break;
                default:
                    core_panic(msg.content.value,
                               "S-CoSenS tried to switch to an unknown period");
                }
                break;

            /* Launch the transmission of a enqueued packet */
            case S_COSENS_EMIT_PACKET:
                do_transmit();
                break;

            /*---- dispatch NETDEV and NETAPI messages ----*/

            /* Generic NETDEV (radio) event */
            case GNRC_NETDEV_MSG_TYPE_EVENT:
                DEBUG("s_cosens_node: GNRC_NETDEV_MSG_TYPE_EVENT received\n");
                dev->driver->isr_event(dev, msg.content.value);
                break;

            /* Enqueue packet for sending */
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("s_cosens_node: GNRC_NETAPI_MSG_TYPE_SND received\n");
                bool ok = push_out_packet((gnrc_pktsnip_t *) msg.content.ptr);
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t) ok;
                msg_reply(&msg, &reply);
                break;

            /* get specific option */
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("s_cosens_node: GNRC_NETAPI_MSG_TYPE_SET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* set option for device driver */
                res = dev->driver->set(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("s_cosens_node: response of netdev->set: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;

            /* set specific option */
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("s_cosens_node: GNRC_NETAPI_MSG_TYPE_GET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* get option from device driver */
                res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("s_cosens_node: response of netdev->get: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;

            /* unknown message: crash the program */
            default:
                sprintf(errmsg,
                        "s_cosens_node: Unknown command %" PRIu16 "\n",
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

kernel_pid_t gnrc_s_cosens_node_init(char *stack,
                                     int stacksize,
                                     char priority,
                                     const char *name,
                                     gnrc_netdev_t *dev)
{
    /* define the CSMA/CA parameters */
    set_csma_mac_min_be(S_COSENS_NODE_MAC_MIN_BE);
    set_csma_mac_max_be(S_COSENS_NODE_MAC_MAX_BE);
    set_csma_mac_max_csma_backoffs(S_COSENS_NODE_MAC_MAX_CSMA_BACKOFFS);

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
