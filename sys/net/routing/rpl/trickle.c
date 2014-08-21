/**
 * Trickle implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup rpl
 * @{
 * @file    trickle.c
 * @brief   Trickle implementation
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "inttypes.h"
#include "trickle.h"
#include "rpl.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* thread stacks */
static char timer_over_buf[TRICKLE_TIMER_STACKSIZE];
static char interval_over_buf[TRICKLE_INTERVAL_STACKSIZE];
static char dao_delay_over_buf[DAO_DELAY_STACKSIZE];
static char routing_table_buf[RT_STACKSIZE];

kernel_pid_t timer_over_pid = KERNEL_PID_UNDEF;
kernel_pid_t interval_over_pid = KERNEL_PID_UNDEF;
kernel_pid_t dao_delay_over_pid = KERNEL_PID_UNDEF;
kernel_pid_t rt_timer_over_pid = KERNEL_PID_UNDEF;

bool ack_received;
uint8_t dao_counter;

uint8_t k;
uint32_t Imin;
uint8_t Imax;
uint32_t I;
uint32_t t;
uint16_t c;
vtimer_t trickle_t_timer;
vtimer_t trickle_I_timer;
vtimer_t dao_timer;
vtimer_t rt_timer;
timex_t t_time;
timex_t I_time;
timex_t dao_time;
timex_t rt_time;

static void *trickle_timer_over(void *arg);
static void *trickle_interval_over(void *arg);
static void *dao_delay_over(void *arg);
static void *rt_timer_over(void *arg);

void reset_trickletimer(void)
{
    I = Imin;
    c = 0;
    /* start timer */
    t = (I / 2) + (rand() % (I - (I / 2) + 1));
    t_time = timex_set(0, t * 1000);
    I_time = timex_set(0, I * 1000);
    timex_normalize(&t_time);
    timex_normalize(&I_time);
    vtimer_remove(&trickle_t_timer);
    vtimer_remove(&trickle_I_timer);
    vtimer_set_wakeup(&trickle_t_timer, t_time, VTIMER_RELATIVE, timer_over_pid, NULL);
    vtimer_set_wakeup(&trickle_I_timer, I_time, VTIMER_RELATIVE, interval_over_pid, NULL);

}

void init_trickle(void)
{
    /* Create threads */
    ack_received = true;
    dao_counter = 0;
    timer_over_pid = thread_create(timer_over_buf, TRICKLE_TIMER_STACKSIZE,
                                   PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                   trickle_timer_over, NULL, "trickle_timer_over");

    interval_over_pid = thread_create(interval_over_buf, TRICKLE_INTERVAL_STACKSIZE,
                                      PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                      trickle_interval_over, NULL, "trickle_interval_over");
    dao_delay_over_pid = thread_create(dao_delay_over_buf, DAO_DELAY_STACKSIZE,
                                       PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                       dao_delay_over, NULL, "dao_delay_over");
    rt_timer_over_pid = thread_create(routing_table_buf, RT_STACKSIZE,
                                      PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                      rt_timer_over, NULL, "rt_timer_over");
}

void start_trickle(uint8_t DIOIntMin, uint8_t DIOIntDoubl,
                   uint8_t DIORedundancyConstant)
{
    c = 0;
    k = DIORedundancyConstant;
    Imin = (1 << DIOIntMin);
    Imax = DIOIntDoubl;
    /* Eigentlich laut Spezifikation erste Bestimmung von I wie auskommentiert: */
    /* I = Imin + ( rand() % ( (Imin << Imax) - Imin + 1 ) ); */
    I = Imin + (rand() % (4 * Imin)) ;

    t = (I / 2) + (rand() % (I - (I / 2) + 1));
    t_time = timex_set(0, t * 1000);
    timex_normalize(&t_time);
    I_time = timex_set(0, I * 1000);
    timex_normalize(&I_time);
    vtimer_remove(&trickle_t_timer);
    vtimer_remove(&trickle_I_timer);
    vtimer_set_wakeup(&trickle_t_timer, t_time, VTIMER_RELATIVE, timer_over_pid, NULL);
    vtimer_set_wakeup(&trickle_I_timer, I_time, VTIMER_RELATIVE, interval_over_pid, NULL);
}

void trickle_increment_counter(void)
{
    /* call this function, when received DIO message */
    c++;
}

static void *trickle_timer_over(void *arg)
{
    (void) arg;

    ipv6_addr_t mcast;
    ipv6_addr_set_all_nodes_addr(&mcast);

    while (1) {
        thread_sleep();

        /* Handle k=0 like k=infinity (according to RFC6206, section 6.5) */
        if ((c < k) || (k == 0)) {
            send_DIO(&mcast);
        }
    }

    return NULL;
}

static void *trickle_interval_over(void *arg)
{
    (void) arg;

    while (1) {
        thread_sleep();
        I = I * 2;
        DEBUG("TRICKLE new Interval %" PRIu32 "\n", I);

        if (I == 0) {
            puts("[WARNING] Interval was 0");

            if (Imax == 0) {
                puts("[WARNING] Imax == 0");
            }

            I = (Imin << Imax);
        }

        if (I > (Imin << Imax)) {
            I = (Imin << Imax);
        }

        c = 0;
        t = (I / 2) + (rand() % (I - (I / 2) + 1));
        /* start timer */
        t_time = timex_set(0, t * 1000);
        timex_normalize(&t_time);
        I_time = timex_set(0, I * 1000);
        timex_normalize(&I_time);

        vtimer_remove(&trickle_t_timer);

        if (vtimer_set_wakeup(&trickle_t_timer, t_time, VTIMER_RELATIVE, timer_over_pid, NULL) != 0) {
            puts("[ERROR] setting Wakeup");
        }

        vtimer_remove(&trickle_I_timer);

        if (vtimer_set_wakeup(&trickle_I_timer, I_time, VTIMER_RELATIVE, interval_over_pid, NULL) != 0) {
            puts("[ERROR] setting Wakeup");
        }
    }

    return NULL;
}

void delay_dao(void)
{
    dao_time = timex_set(DEFAULT_DAO_DELAY, 0);
    dao_counter = 0;
    ack_received = false;
    vtimer_remove(&dao_timer);
    vtimer_set_wakeup(&dao_timer, dao_time, VTIMER_RELATIVE, dao_delay_over_pid, NULL);
}

/* This function is used for regular update of the routes. The Timer can be overwritten, as the normal delay_dao function gets called */
void long_delay_dao(void)
{
    dao_time = timex_set(REGULAR_DAO_INTERVAL, 0);
    dao_counter = 0;
    ack_received = false;
    vtimer_remove(&dao_timer);
    vtimer_set_wakeup(&dao_timer, dao_time, VTIMER_RELATIVE, dao_delay_over_pid, NULL);
}

static void *dao_delay_over(void *arg)
{
    (void) arg;
    while (1) {
        thread_sleep();

        if ((ack_received == false) && (dao_counter < DAO_SEND_RETRIES)) {
            dao_counter++;
            send_DAO(NULL, 0, true, 0);
            dao_time = timex_set(DEFAULT_WAIT_FOR_DAO_ACK, 0);
            vtimer_remove(&dao_timer);
            vtimer_set_wakeup(&dao_timer, dao_time, VTIMER_RELATIVE, dao_delay_over_pid, NULL);
        }
        else if (ack_received == false) {
            long_delay_dao();
        }
    }
    return NULL;
}

void dao_ack_received(void)
{
    ack_received = true;
    long_delay_dao();
}

static void *rt_timer_over(void *arg)
{
    (void) arg;

    rpl_routing_entry_t *rt;

    while (1) {
        rpl_dodag_t *my_dodag = rpl_get_my_dodag();

        if (my_dodag != NULL) {
            rt = rpl_get_routing_table();

            for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
                if (rt[i].used) {
                    if (rt[i].lifetime <= 1) {
                        memset(&rt[i], 0, sizeof(rt[i]));
                    }
                    else {
                        rt[i].lifetime--;
                    }
                }
            }

            /* Parent is NULL for root too */
            if (my_dodag->my_preferred_parent != NULL) {
                if (my_dodag->my_preferred_parent->lifetime <= 1) {
                    puts("parent lifetime timeout");
                    rpl_parent_update(NULL);
                }
                else {
                    my_dodag->my_preferred_parent->lifetime--;
                }
            }
        }

        /* Wake up every second */
        vtimer_usleep(1000000);
    }

    return NULL;
}
