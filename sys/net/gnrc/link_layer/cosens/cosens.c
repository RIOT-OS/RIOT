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
 * @file    cosens.c
 * @brief   Implementation of the CoSenS MAC protocol for RIOT OS
 * @details Implementation of the CoSenS MAC protocol;
 *          designed by Bilel Nefzi and Ye-Qiong Song, see:
 *          Ad Hoc Networks 10 (2012) 680-695.
 * @author  Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#include <errno.h>

#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "xtimer.h"
#include "net/gnrc/cosens.h"
#include "net/gnrc/csma_sender.h"
#include "net/gnrc.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif


/****************************************************************************/
/*     Constants/config. parameters of the CoSenS protocol's MAC driver     */
/****************************************************************************/

#define MAX_OUTGOING_PACKETS_NUMBER   (16)

#define MAC_MAX_FRAME_RETRIES          (7)

#define MIN_WP_DURATION_MICROSEC      (1000UL)
#define MAX_WP_DURATION_MICROSEC     (70000UL)

#define DEFAULT_WP_DURATION_MICROSEC \
          ((MIN_WP_DURATION_MICROSEC + MAX_WP_DURATION_MICROSEC) >> 1)

#define DEFAULT_BACKOFF_DURATION_MICROSEC \
          (((1 << MAC_MIN_BE_DEFAULT) - 1) * A_UNIT_BACKOFF_PERIOD_MICROSEC)


#define ALPHA_PERCENT   1


/****************************************************************************/
/*              Variables of the CoSenS protocol's MAC driver               */
/****************************************************************************/


/* underlying radio driver */
gnrc_netdev_t *dev = NULL;


/* CoSenS thread ID */
static volatile kernel_pid_t cosens_tid;


/* internal message types for CoSenS */
typedef enum {
    COSENS_SWITCH_PERIOD = 1977,
    COSENS_EMIT_PACKET,
} cosens_msg_t;

/* period in which we currently are */
typedef enum {
    WAIT_PERIOD,
    TRANSMIT_PERIOD,
    INVALID_PERIOD = -1
} cosens_period_t;
static volatile cosens_period_t cosens_current_period;

/* timer managing the period changes */
static xtimer_t period_change_timer;

/* outgoing paquets' queue */
static gnrc_pktsnip_t outgoing_queue[MAX_OUTGOING_PACKETS_NUMBER];
static gnrc_pktsnip_t * volatile first_outgoing_packet;

/* duration of the latest waiting period */
static uint32_t wp_duration;

/* start time of the latest transmission period */
static uint32_t tp_start;

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

/* number of SFDs detected */
static uint32_t sfd_count = 0;


/****************************************************************************/
/*                         CoSenS utility functions                         */
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

static inline void send_cosens_message(cosens_msg_t msg_type, uint32_t param)
{
    msg_t new_msg;
    new_msg.type = msg_type;
    new_msg.content.value = param;
    int res = msg_try_send(&new_msg, cosens_tid);
    if (res < 1) {
        core_panic(PANIC_GENERAL_ERROR,
                   "Couldn't send message to CoSenS thread");
    }
}


/****************************************************************************/
/*                    CoSenS period-management functions                    */
/****************************************************************************/

/* executing transmission period
   i.e.: handle the emission of outgoing packets, one after another */
static void do_transmit(void)
{
    /* outcome of the radio transmission */
    int result;
    /* next packet to send */
    gnrc_pktsnip_t *curr_pkt = first_outgoing_packet;

    /* if all transmissions have been done, switch to waiting mode */
    if (curr_pkt == NULL) {
        send_cosens_message(COSENS_SWITCH_PERIOD, WAIT_PERIOD);
        return;
    }

    DEBUG("CoSenS: now emitting packet...\n");
    if (!in_burst_mode) {
        /* if next transmission must use the CSMA/CA method... */
        result = csma_ca_send(dev, curr_pkt);
    } else {
        /* we go directly here if transmission can be done in burst mode */
        result = cca_send(dev, curr_pkt);
    }

    /* check how transmission went */
    if (((unsigned int) result) == curr_pkt->size) {
        /* success */
        DEBUG("CoSenS: packet successfully transmitted.\n");
        /* remove the packet successfully sent from queue
           and free the packet descriptor... */
        pop_packet_from_queue();
        /* ... increase number of transmitted packets... */
        latest_tx_packets_number++;
        /* ... reset counter of attempted transmissions... */
        num_tx = 0;
        /* ... can switch to "faster" burst mode... */
        in_burst_mode = true;
        DEBUG("CoSenS: switching to burst transmission mode.\n");

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
            DEBUG("CoSenS: UNABLE TO EMIT PACKET"
                  " AFTER %d ATTEMPTS!\n", num_tx);
            DEBUG("CoSenS: GIVING UP TRANSMISSION\n");
            /* remove the failed packet from queue
               and free its corresponding slot */
            pop_packet_from_queue();
            num_tx = 0;
        }

    }

    /* start next transmission */
    send_cosens_message(COSENS_EMIT_PACKET, 0L);
}

/* switching to transmission period */
static void begin_transmit(void)
{
    DEBUG("CoSenS: switching to transmission period...\n");
    /* switch to transmission period */
    cosens_current_period = TRANSMIT_PERIOD;
    tp_start = xtimer_now();

    /* reset the counter of transmitted packets */
    latest_tx_packets_number = 0;

    /* TX always begin in "safe" CSMA/CA mode */
    in_burst_mode = false;

    num_tx = 0;

    send_cosens_message(COSENS_EMIT_PACKET, 0L);
}

/* switching to waiting period */
static void begin_wait(void)
{
    DEBUG("CoSenS: switching to waiting period...\n");

    /* calculates the duration of the upcoming waiting period */
    if (tp_start == 0) {
        tp_start = xtimer_now();
    }
    uint32_t tp_duration = xtimer_now() - tp_start;

    if (tp_mean_duration == 0) {
        tp_mean_duration = DEFAULT_WP_DURATION_MICROSEC;
    } else {
        tp_mean_duration = ( ((100 - ALPHA_PERCENT) * tp_mean_duration)
                                  + (ALPHA_PERCENT  * tp_duration)
                            / 100);
    }

    if (packets_number_mean == 0) {
        packets_number_mean = latest_tx_packets_number;
    } else {
        packets_number_mean = ( ((100 - ALPHA_PERCENT) * packets_number_mean)
                                     + (ALPHA_PERCENT  * latest_tx_packets_number)
                               / 100);
    }
    /* ensure the mean doesn't fall under ONE packet */
    if (packets_number_mean <= 1) {
        packets_number_mean = 1;
    }

    wp_duration = (packets_number_mean - 1) * A_UNIT_BACKOFF_PERIOD_MICROSEC
                + tp_mean_duration;
    if (wp_duration < MIN_WP_DURATION_MICROSEC) {
        wp_duration = MIN_WP_DURATION_MICROSEC;
    }
    if (wp_duration > MAX_WP_DURATION_MICROSEC) {
        wp_duration = MAX_WP_DURATION_MICROSEC;
    }

    DEBUG("CoSenS: next waiting period will be %"PRIu32" us long.\n",
          wp_duration);

    /* sets the timer that will switch to transmission period */
    static msg_t msg_go_tp = {
        .type = COSENS_SWITCH_PERIOD,
        .content.value = TRANSMIT_PERIOD
    };
    xtimer_set_msg(&period_change_timer,
                   wp_duration,
                   &msg_go_tp,
                   cosens_tid);

    /* actually switch to waiting period */
    cosens_current_period = WAIT_PERIOD;
}


/****************************************************************************/
/*            Generic event-management function of CoSenS thread            */
/****************************************************************************/

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
static void _cosens_event_cb(gnrc_netdev_event_t event, void *data)
{
    gnrc_pktsnip_t *pkt;

    DEBUG("cosens: event triggered -> %i\n", event);

    /* CoSenS managing events from radio */
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
            DEBUG("cosens: unable to forward packet of type %i\n",
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
/*                      Main function of CoSenS thread                      */
/****************************************************************************/

/**
 * @brief   Startup code and event loop of the CoSenS MAC layer
 *
 * @param[in] args          expects a pointer to the underlying netdev device
 *
 * @return                  never returns
 */
static void * _cosens_thread_func(void * args)
{
    char errmsg[80];

    gnrc_netreg_entry_t regentry;
    gnrc_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[GNRC_COSENS_MSG_QUEUE_SIZE];

    DEBUG("cosens is initializing... \n");
    dev = (gnrc_netdev_t *)args;
    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, GNRC_COSENS_MSG_QUEUE_SIZE);
    /* save the PID to the device descriptor and register the device */
    dev->mac_pid = thread_getpid();
    gnrc_netif_add(dev->mac_pid);
    /* register the event callback with the device driver */
    dev->driver->add_event_callback(dev, _cosens_event_cb);

    /* intialize the outgoing packets' queue */
    for (int i = 0; i < MAX_OUTGOING_PACKETS_NUMBER; i++) {
        outgoing_queue[i].next = NULL;
        outgoing_queue[i].data = NULL;
        outgoing_queue[i].size = 0;
    }
    first_outgoing_packet = NULL;

    /* register our thread to receive the wanted events from the netstack */
    regentry.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    regentry.pid = thread_getpid();
    /* we want the raw 802.15.4 frames => GNRC_NETTYPE_UNDEF */
    res = gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &regentry);
    if (res < 0) {
        core_panic(PANIC_ASSERT_FAIL,
                   "Could not register CoSenS MAC thread to netreg");
    }

    /* start the event loop */
    DEBUG("cosens: going ON...\n");
    while (1) {

        /* The very first period is a waiting period */
        begin_wait();

        DEBUG("cosens: waiting for incoming messages\n");
        msg_receive(&msg);

        switch (msg.type) {
            /*---- dispatch CoSenS-specific messages ----*/

            /* Switching from a CoSenS period to another */
            case COSENS_SWITCH_PERIOD:
                switch (msg.content.value) {
                case WAIT_PERIOD:
                    begin_wait();
                    break;
                case TRANSMIT_PERIOD:
                    begin_transmit();
                    break;
                default:
                    sprintf(errmsg,
                            "cosens: Unknown period %" PRIu32 "\n",
                            msg.content.value);
                    core_panic(PANIC_ASSERT_FAIL, errmsg);
                    /* won't go past here */
                }
                break;

            /* Launch the transmission of a enqueued packet */
            case COSENS_EMIT_PACKET:
                do_transmit();
                break;

            /*---- dispatch NETDEV and NETAPI messages ----*/

            /* Generic NETDEV (radio) event */
            case GNRC_NETDEV_MSG_TYPE_EVENT:
                DEBUG("cosens: GNRC_NETDEV_MSG_TYPE_EVENT received\n");
                dev->driver->isr_event(dev, msg.content.value);
                break;

            /* Enqueue packet for sending */
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("cosens: GNRC_NETAPI_MSG_TYPE_SND received\n");
                bool ok = push_out_packet((gnrc_pktsnip_t *) msg.content.ptr);
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t) ok;
                msg_reply(&msg, &reply);
                break;

            /* get specific option */
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("cosens: GNRC_NETAPI_MSG_TYPE_SET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* set option for device driver */
                res = dev->driver->set(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("cosens: response of netdev->set: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;

            /* set specific option */
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("cosens: GNRC_NETAPI_MSG_TYPE_GET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* get option from device driver */
                res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("cosens: response of netdev->get: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;

            /* unknown message: crash the program */
            default:
                sprintf(errmsg,
                        "cosens: Unknown command %" PRIu16 "\n",
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
/*   Initialization function exported by the CoSenS protocol's MAC driver   */
/****************************************************************************/

kernel_pid_t gnrc_cosens_init(char *stack,
                              int stacksize,
                              char priority,
                              const char *name,
                              gnrc_netdev_t *dev)
{
    /* define the CSMA/CA parameters */
    set_csma_mac_min_be(COSENS_MAC_MIN_BE);
    set_csma_mac_max_be(COSENS_MAC_MAX_BE);
    set_csma_mac_max_csma_backoffs(COSENS_MAC_MAX_CSMA_BACKOFFS);

    /* check if given netdev device is defined and the driver is set */
    if (dev == NULL || dev->driver == NULL) {
        return -ENODEV;
    }

    /* create new CoSenS thread */
    cosens_tid = thread_create(stack,
                               stacksize,
                               priority,
                               THREAD_CREATE_STACKTEST,
                               _cosens_thread_func,
                               (void *)dev,
                               name);
    if (cosens_tid <= 0) {
        return -EINVAL;
    }

    return cosens_tid;
}
