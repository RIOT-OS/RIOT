/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_lwmac
 * @file
 * @brief       Implementation of the LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @}
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <kernel_types.h>
#include <lpm.h>
#include <msg.h>
#include <thread.h>
#include <timex.h>
#include <periph/rtt.h>
#include <net/gnrc.h>
#include <net/gnrc/lwmac/lwmac.h>
#include <net/gnrc/lwmac/packet_queue.h>

#include "include/tx_state_machine.h"
#include "include/rx_state_machine.h"
#include "include/lwmac_internal.h"
#include "include/lwmac_types.h"
#include "include/timeout.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define LOG_LEVEL LOG_WARNING
#include "log.h"

#undef LOG_ERROR
#undef LOG_WARNING
#undef LOG_INFO
#undef LOG_DEBUG

#define LOG_ERROR(...) LOG(LOG_ERROR, "ERROR: [lwmac] " __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_WARNING, "WARNING: [lwmac] " __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_INFO, "[lwmac] " __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_DEBUG, "[lwmac] " __VA_ARGS__)

/******************************************************************************/

/* Internal state of lwMAC */
static lwmac_t lwmac = LWMAC_INIT;

/******************************************************************************/

static bool lwmac_update(void);
static void lwmac_set_state(lwmac_state_t newstate);
static void lwmac_schedule_update(void);
static bool lwmac_needs_update(void);
static void rtt_handler(uint32_t event);

/******************************************************************************/

// TODO: Don't use global variables
inline void lwmac_schedule_update(void)
{
    lwmac.needs_rescheduling = true;
}

/******************************************************************************/

// TODO: Don't use global variables
inline bool lwmac_needs_update(void)
{
    return lwmac.needs_rescheduling;
}

/******************************************************************************/

// TODO: Don't use global variables
void lwmac_set_state(lwmac_state_t newstate)
{
    lwmac_state_t oldstate = lwmac.state;

    if(newstate == oldstate)
        return;

    if(newstate >= STATE_COUNT) {
        LOG_ERROR("Trying to set invalid state %u\n", newstate);
        return;
    }

    /* Already change state, but might be reverted to oldstate when needed */
    lwmac.state = newstate;

    /* Actions when leaving old state */
    switch(oldstate)
    {
    case RECEIVING:
    case TRANSMITTING:
        /* Enable duty cycling again */
        rtt_handler(LWMAC_EVENT_RTT_RESUME);
        break;

    case SLEEPING:
        lwmac_clear_timeout(&lwmac, TIMEOUT_WAKEUP_PERIOD);
        break;

    default:
        break;
    }

    /* Actions when entering new state */
    switch(newstate)
    {
    /*********************** Operation states *********************************/
    case LISTENING:
        _set_netdev_state(&lwmac, NETOPT_STATE_IDLE);
        break;

    case SLEEPING:
        /* Put transceiver to sleep */
        _set_netdev_state(&lwmac, NETOPT_STATE_SLEEP);
        /* We may have come here through RTT handler, so timeout may still be active */
        lwmac_clear_timeout(&lwmac, TIMEOUT_WAKEUP_PERIOD);
        /* Return immediately, so no rescheduling */
        return;

    /* Trying to send data */
    case TRANSMITTING:
        rtt_handler(LWMAC_EVENT_RTT_PAUSE); /* No duty cycling while RXing */
        _set_netdev_state(&lwmac, NETOPT_STATE_IDLE);  /* Power up netdev */
        break;

    /* Receiving incoming data */
    case RECEIVING:
        rtt_handler(LWMAC_EVENT_RTT_PAUSE); /* No duty cycling while TXing */
        _set_netdev_state(&lwmac, NETOPT_STATE_IDLE);  /* Power up netdev */
        break;

    case STOPPED:
        _set_netdev_state(&lwmac, NETOPT_STATE_OFF);
        break;

    /*********************** Control states ***********************************/
    case START:
        rtt_handler(LWMAC_EVENT_RTT_START);
        lwmac_set_state(LISTENING);
        break;

    case STOP:
        rtt_handler(LWMAC_EVENT_RTT_STOP);
        lwmac_set_state(STOPPED);
        break;

    case RESET:
        LOG_WARNING("Reset not yet implemented\n");
        lwmac_set_state(STOP);
        lwmac_set_state(START);
        break;

    /**************************************************************************/
    default:
        LOG_DEBUG("No actions for entering state %u\n", newstate);
        return;
    }

    lwmac_schedule_update();
}

/******************************************************************************/

/* Main state machine. Call whenever something happens */
// TODO: Don't use global variables
bool lwmac_update(void)
{
    lwmac.needs_rescheduling = false;

    switch(lwmac.state)
    {
    case SLEEPING:

        /* If a packet is scheduled, no other (possible earlier) packet can be
         * sent before the first one is handled, even no broadcast
         */
        if( !lwmac_timeout_is_running(&lwmac, TIMEOUT_WAIT_FOR_DEST_WAKEUP) ) {

            /* Check if there are broadcasts to send and transmit immediately */
            if(packet_queue_length(&(lwmac.tx.neighbours[0].queue)) > 0) {
                lwmac.tx.current_neighbour = &(lwmac.tx.neighbours[0]);
                lwmac_set_state(TRANSMITTING);
                break;
            }

            lwmac_tx_neighbour_t* neighbour = _next_tx_neighbour(&lwmac);

            if(neighbour != NULL) {
                /* Offset in microseconds when the earliest (phase) destination
                 * node wakes up that we have packets for. */
                int time_until_tx = RTT_TICKS_TO_US(_ticks_until_phase(neighbour->phase));

                /* If there's not enough time to prepare a WR to catch the phase
                 * postpone to next interval */
                if (time_until_tx < LWMAC_WR_PREPARATION_US) {
                    time_until_tx += LWMAC_WAKEUP_INTERVAL_US;
                }

                time_until_tx -= LWMAC_WR_PREPARATION_US;
                lwmac_set_timeout(&lwmac, TIMEOUT_WAIT_FOR_DEST_WAKEUP, time_until_tx);

                /* Register neighbour to be the next */
                lwmac.tx.current_neighbour = neighbour;

                /* Stop dutycycling, we're preparing to send. This prevents the
                 * timeout arriving late, so that the destination phase would
                 * be missed. */
                // TODO: bad for power savings
                rtt_handler(LWMAC_EVENT_RTT_PAUSE);
            } else {
                /* LOG_WARNING("Nothing to send, why did we get called?\n"); */
            }
        } else {
            if(lwmac_timeout_is_expired(&lwmac, TIMEOUT_WAIT_FOR_DEST_WAKEUP)) {
                LOG_DEBUG("Got timeout for dest wakeup, ticks: %"PRIu32"\n", rtt_get_counter());
                lwmac_set_state(TRANSMITTING);
            } else {
                /* LOG_DEBUG("Nothing to do, why did we get called?\n"); */
            }
        }
        break;

    case LISTENING:
        /* Set timeout for if there's no successful rx transaction that will
         * change state to SLEEPING. */
        if(!lwmac_timeout_is_running(&lwmac, TIMEOUT_WAKEUP_PERIOD)) {
            lwmac_set_timeout(&lwmac, TIMEOUT_WAKEUP_PERIOD, LWMAC_WAKEUP_DURATION_US);
        } else if(lwmac_timeout_is_expired(&lwmac, TIMEOUT_WAKEUP_PERIOD)) {
            /* Dispatch first as there still may be broadcast packets. */
            _dispatch(lwmac.rx.dispatch_buffer);
            lwmac_set_state(SLEEPING);
        }

        if(packet_queue_length(&lwmac.rx.queue) > 0) {
            lwmac_set_state(RECEIVING);
        }
        break;

    case RECEIVING:
    {
        lwmac_rx_state_t state_rx = lwmac.rx.state;

        switch(state_rx)
        {
        case RX_STATE_STOPPED:
        {
            lwmac_rx_start(&lwmac);
            lwmac_rx_update(&lwmac);
            break;
        }
        case RX_STATE_FAILED:
            /* This may happen frequently because we'll receive WA from
             * every node in range. */
            LOG_DEBUG("Reception was NOT successful\n");
            lwmac_rx_stop(&lwmac);
            /* Restart */
            lwmac_set_state(LISTENING);
            break;
        case RX_STATE_SUCCESSFUL:
            LOG_INFO("Reception was successful\n");
            lwmac_rx_stop(&lwmac);
            /* Dispatch received packets, timing is not critical anymore */
            _dispatch(lwmac.rx.dispatch_buffer);
            /* Go back to sleep after successful transaction */
            lwmac_set_state(SLEEPING);
            break;
        default:
            lwmac_rx_update(&lwmac);
        }

        /* If state has changed, reschedule main state machine */
        if(state_rx != lwmac.rx.state)
        {
            lwmac_schedule_update();
        }
        break;
    }
    case TRANSMITTING:
    {
        char* tx_success = "";
        lwmac_tx_state_t state_tx = lwmac.tx.state;

        switch(state_tx)
        {
        case TX_STATE_STOPPED:
        {
            gnrc_pktsnip_t* pkt;

//            assert(lwmac.tx.current_neighbour);

            if( (pkt = packet_queue_pop(&lwmac.tx.current_neighbour->queue)) )
            {
                lwmac_tx_start(&lwmac, pkt, lwmac.tx.current_neighbour);
                lwmac_tx_update(&lwmac);
            } else {
                /* Shouldn't happen, but never observed this case */
                int id = (lwmac.tx.current_neighbour - lwmac.tx.neighbours);
                id /= sizeof(lwmac.tx.current_neighbour);
                LOG_ERROR("Packet from neighbour's queue (#%d) invalid\n", id);
                lwmac_schedule_update();
            }
            break;
        }

        case TX_STATE_FAILED:
            tx_success = "NOT ";
            /* Intended fall-through, TX packet will therefore be dropped. No
             * automatic resending here, we did our best.
             */
        case TX_STATE_SUCCESSFUL:
            if(lwmac.tx.current_neighbour == &(lwmac.tx.neighbours[0])) {
                LOG_INFO("Broadcast transmission done\n");
            } else {
                LOG_INFO("Transmission was %ssuccessful (%"PRIu32" WRs sent)\n",
                         tx_success, lwmac.tx.wr_sent);
            }
            lwmac_tx_stop(&lwmac);
            lwmac_set_state(SLEEPING);
            break;
        default:
            lwmac_tx_update(&lwmac);
        }

        /* If state has changed, reschedule main state machine */
        if(state_tx != lwmac.tx.state)
        {
            lwmac_schedule_update();
        }
        break;
    }
    default:
        LOG_DEBUG("No actions in state %u\n", lwmac.state);
    }

    return lwmac.needs_rescheduling;
}

/******************************************************************************/

static void rtt_cb(void* arg)
{
    msg_t msg;
    msg.content.value = ((uint32_t) arg ) & 0xffff;
    msg.type = LWMAC_EVENT_RTT_TYPE;
    msg_send(&msg, lwmac.pid);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

/******************************************************************************/

// TODO: Don't use global variables
void rtt_handler(uint32_t event)
{
    uint32_t alarm;
    switch(event & 0xffff)
    {
    case LWMAC_EVENT_RTT_WAKEUP_PENDING:
        lwmac.last_wakeup = rtt_get_alarm();
        alarm = _next_inphase_event(lwmac.last_wakeup, RTT_US_TO_TICKS(LWMAC_WAKEUP_DURATION_US));
        rtt_set_alarm(alarm, rtt_cb, (void*) LWMAC_EVENT_RTT_SLEEP_PENDING);
        lpm_prevent_sleep |= LWMAC_LPM_MASK;
        lwmac_set_state(LISTENING);
        break;

    case LWMAC_EVENT_RTT_SLEEP_PENDING:
        alarm = _next_inphase_event(lwmac.last_wakeup, RTT_US_TO_TICKS(LWMAC_WAKEUP_INTERVAL_US));
        rtt_set_alarm(alarm, rtt_cb, (void*) LWMAC_EVENT_RTT_WAKEUP_PENDING);
        lpm_prevent_sleep &= ~(LWMAC_LPM_MASK);
        lwmac_set_state(SLEEPING);
        break;

    /* Set initial wakeup alarm that starts the cycle */
    case LWMAC_EVENT_RTT_START:
        LOG_DEBUG("RTT: Initialize duty cycling\n");
        alarm = rtt_get_counter() + RTT_US_TO_TICKS(LWMAC_WAKEUP_DURATION_US);
        rtt_set_alarm(alarm, rtt_cb, (void*) LWMAC_EVENT_RTT_SLEEP_PENDING);
        lpm_prevent_sleep |= LWMAC_LPM_MASK;
        lwmac.dutycycling_active = true;
        break;

    case LWMAC_EVENT_RTT_STOP:
    case LWMAC_EVENT_RTT_PAUSE:
        rtt_clear_alarm();
        LOG_DEBUG("RTT: Stop duty cycling, now in state %u\n", lwmac.state);
        lpm_prevent_sleep |= LWMAC_LPM_MASK;
        lwmac.dutycycling_active = false;
        break;

    case LWMAC_EVENT_RTT_RESUME:
        LOG_DEBUG("RTT: Resume duty cycling\n");
        alarm = _next_inphase_event(lwmac.last_wakeup, RTT_US_TO_TICKS(LWMAC_WAKEUP_INTERVAL_US));
        rtt_set_alarm(alarm, rtt_cb, (void*) LWMAC_EVENT_RTT_WAKEUP_PENDING);
        lpm_prevent_sleep &= ~(LWMAC_LPM_MASK);
        lwmac.dutycycling_active = true;
        break;
    }
}

/******************************************************************************/

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
// TODO: Don't use global variables
static void _event_cb(gnrc_netdev_event_t event, void *data)
{
    switch(event)
    {
    case NETDEV_EVENT_RX_STARTED:
        LOG_DEBUG("NETDEV_EVENT_RX_STARTED\n");
        lwmac.rx_started = true;
        break;
    case NETDEV_EVENT_RX_COMPLETE:
    {
        LOG_DEBUG("NETDEV_EVENT_RX_COMPLETE\n");

        gnrc_pktsnip_t* pkt = (gnrc_pktsnip_t *) data;

        /* Prevent packet corruption when a packet is sent before the previous
         * received packet has been downloaded. This happens e.g. when a timeout
         * expires that causes the tx state machine to send a packet. When a
         * packet arrives after the timeout, the notification is queued but the
         * tx state machine continues to send and then destroys the received
         * packet in the frame buffer. After completion, the queued notification
         * will be handled a corrupted packet will be downloaded. Therefore
         * keep track that RX_STARTED is followed by RX_COMPLETE.
         *
         * TODO: transceivers might have 2 frame buffers, so make this optional
         */
        if(!lwmac.rx_started) {
            LOG_WARNING("Maybe sending kicked in and frame buffer is now corrupted\n");
            gnrc_pktbuf_release(pkt);
            lwmac.rx_started = false;
            break;
        }

        lwmac.rx_started = false;

        if(!packet_queue_push(&lwmac.rx.queue, pkt, 0))
        {
            LOG_ERROR("Can't push RX packet @ %p, memory full?\n", pkt);
            gnrc_pktbuf_release(pkt);
            break;
        }
        lwmac_schedule_update();
        break;
    }
    case NETDEV_EVENT_TX_STARTED:
        lwmac.tx_feedback = TX_FEEDBACK_UNDEF;
        lwmac.rx_started = false;
//        lwmac_schedule_update();
        break;
    case NETDEV_EVENT_TX_COMPLETE:
        lwmac.tx_feedback = TX_FEEDBACK_SUCCESS;
        lwmac.rx_started = false;
        lwmac_schedule_update();
        break;
    case NETDEV_EVENT_TX_NOACK:
        lwmac.tx_feedback = TX_FEEDBACK_NOACK;
        lwmac.rx_started = false;
        lwmac_schedule_update();
        break;
    case NETDEV_EVENT_TX_MEDIUM_BUSY:
        lwmac.tx_feedback = TX_FEEDBACK_BUSY;
        lwmac.rx_started = false;
        lwmac_schedule_update();
        break;

    default:
        LOG_WARNING("Unhandled netdev event: %u\n", event);
    }
}

/**
 * @brief   Startup code and event loop of the LWMAC layer
 *
 * @param[in] args          expects a pointer to the underlying netdev device
 *
 * @return                  never returns
 */
// TODO: Don't use global variables
static void *_lwmac_thread(void *args)
{
    gnrc_netdev_t* dev = lwmac.netdev = (gnrc_netdev_t *)args;
    gnrc_netapi_opt_t* opt;
    int res;
    msg_t msg, reply, msg_queue[LWMAC_IPC_MSG_QUEUE_SIZE];

    LOG_INFO("Starting lwMAC\n");

    /* RTT is used for scheduling wakeup */
    rtt_init();

    /* Store pid globally, so that IRQ can use it to send msg */
    lwmac.pid = thread_getpid();

    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, LWMAC_IPC_MSG_QUEUE_SIZE);
    /* save the PID to the device descriptor and register the device */
    dev->mac_pid = lwmac.pid;
    gnrc_netif_add(lwmac.pid);
    /* register the event callback with the device driver */
    dev->driver->add_event_callback(dev, _event_cb);

    /* Enable RX- and TX-started interrupts  */
    netopt_enable_t enable = NETOPT_ENABLE;
    dev->driver->set(dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

    /* Enable preloading, so packet will only be sent when netdev state will be
     * set to NETOPT_STATE_TX */
    dev->driver->set(dev, NETOPT_PRELOADING, &enable, sizeof(enable));

    /* Get own address from netdev */
    lwmac.l2_addr.len = dev->driver->get(dev, NETOPT_ADDRESS, &lwmac.l2_addr.addr, sizeof(lwmac.l2_addr.addr));
    assert(lwmac.l2_addr.len > 0);

    /* Initialize broadcast sequence number. This at least differs from board
     * to board */
    lwmac.tx.bcast_seqnr = lwmac.l2_addr.addr[0];

    /* Initialize receive packet queue */
    packet_queue_init(&lwmac.rx.queue,
                      lwmac.rx._queue_nodes,
                      (sizeof(lwmac.rx._queue_nodes) / sizeof(packet_queue_node_t)));

    /* First neighbour queue is supposed to be broadcast queue */
    int broadcast_queue_id = _alloc_neighbour(&lwmac);
    assert(broadcast_queue_id == 0);

    /* Setup broadcast tx queue */
    uint8_t broadcast_addr[] = {0xff, 0xff};
    _init_neighbour(_get_neighbour(&lwmac, 0), broadcast_addr, sizeof(broadcast_addr));

    /* Reset all timeouts just to be sure */
    lwmac_reset_timeouts(&lwmac);

    /* Start duty cycling */
    lwmac_set_state(START);

    /* start the event loop */
    while (1) {

        msg_receive(&msg);

        /* Handle NETDEV, NETAPI, RTT and TIMEOUT messages */
        switch (msg.type) {

        /* RTT raised an interrupt */
        case LWMAC_EVENT_RTT_TYPE:
            if(lwmac.dutycycling_active) {
                rtt_handler(msg.content.value);
                lwmac_schedule_update();
            } else {
                LOG_DEBUG("Ignoring late RTT event while dutycycling is off\n");
            }
            break;

        /* An lwmac timeout occured */
        case LWMAC_EVENT_TIMEOUT_TYPE:
        {
            lwmac_timeout_make_expire((lwmac_timeout_t*) msg.content.ptr);
            lwmac_schedule_update();
            break;
        }

        /* Transceiver raised an interrupt */
        case GNRC_NETDEV_MSG_TYPE_EVENT:
            LOG_DEBUG("GNRC_NETDEV_MSG_TYPE_EVENT received\n");
            /* Forward event back to driver */
            dev->driver->isr_event(dev, msg.content.value);
            break;

        /* TX: Queue for sending */
        case GNRC_NETAPI_MSG_TYPE_SND:
        {
            // TODO: how to announce failure to upper layers?

            LOG_DEBUG("GNRC_NETAPI_MSG_TYPE_SND received\n");
            gnrc_pktsnip_t* pkt = (gnrc_pktsnip_t*) msg.content.ptr;

            _queue_tx_packet(&lwmac, pkt);

            lwmac_schedule_update();
            break;
        }
        /* NETAPI set/get. Can't this be refactored away from here? */
        case GNRC_NETAPI_MSG_TYPE_SET:
        {
            LOG_DEBUG("GNRC_NETAPI_MSG_TYPE_SET received\n");
            opt = (gnrc_netapi_opt_t *)msg.content.ptr;

            /* Depending on option forward to NETDEV or handle here */
            switch(opt->opt)
            {
            /* Handle state change requests */
            case NETOPT_STATE:
            {
                netopt_state_t* state = (netopt_state_t*) opt->data;
                res = opt->data_len;
                switch(*state)
                {
                case NETOPT_STATE_OFF:
                    lwmac_set_state(STOP);
                    break;
                case NETOPT_STATE_IDLE:
                    lwmac_set_state(START);
                    break;
                case NETOPT_STATE_RESET:
                    lwmac_set_state(RESET);
                    break;
                default:
                    res = -EINVAL;
                    LOG_ERROR("NETAPI tries to set unsupported state %u\n",
                          *state);
                }
                lwmac_schedule_update();
                break;
            }
            /* Forward to netdev by default*/
            default:
                /* set option for device driver */
                res = dev->driver->set(dev, opt->opt, opt->data, opt->data_len);
                LOG_DEBUG("Response of netdev->set: %i\n", res);
            }

            /* send reply to calling thread */
            reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
            reply.content.value = (uint32_t)res;
            msg_reply(&msg, &reply);
            break;
        }
        case GNRC_NETAPI_MSG_TYPE_GET:
            /* TODO: filter out MAC layer options -> for now forward
                     everything to the device driver */
            LOG_DEBUG("GNRC_NETAPI_MSG_TYPE_GET received\n");
            /* read incoming options */
            opt = (gnrc_netapi_opt_t *)msg.content.ptr;
            /* get option from device driver */
            res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
            LOG_DEBUG("Response of netdev->get: %i\n", res);
            /* send reply to calling thread */
            reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
            reply.content.value = (uint32_t)res;
            msg_reply(&msg, &reply);
            break;

        default:
            LOG_ERROR("Unknown command %" PRIu16 "\n", msg.type);
            break;
        }

        /* Execute main state machine because something just happend*/
        while(lwmac_needs_update()) {
            lwmac_update();
        }
    }

    LOG_ERROR("terminated\n");

    /* never reached */
    return NULL;
}

kernel_pid_t gnrc_lwmac_init(char *stack, int stacksize, char priority,
                        const char *name, gnrc_netdev_t *dev)
{
    kernel_pid_t res;

    /* check if given netdev device is defined and the driver is set */
    if (dev == NULL || dev->driver == NULL) {
        LOG_ERROR("No netdev supplied or driver not set\n");
        return -ENODEV;
    }

    /* Prevent sleeping until first RTT alarm is set */
    lpm_prevent_sleep |= LWMAC_LPM_MASK;

    /* create new LWMAC thread */
    res = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
                         _lwmac_thread, (void *)dev, name);
    if (res <= 0) {
        LOG_ERROR("Couldn't create thread\n");
        lpm_prevent_sleep &= ~(LWMAC_LPM_MASK);
        return -EINVAL;
    }

    return res;
}
