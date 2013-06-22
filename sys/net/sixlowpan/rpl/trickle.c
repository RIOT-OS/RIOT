/**
 * Trickle implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
#include <math.h>
#include "trickle.h"
#include "sys/net/sixlowpan/rpl/rpl.h"

char timer_over_buf[TRICKLE_TIMER_STACKSIZE];
char interval_over_buf[TRICKLE_INTERVAL_STACKSIZE];
char dao_delay_over_buf[DAO_DELAY_STACKSIZE];
char routing_table_buf[RT_STACKSIZE];
int timer_over_pid;
int interval_over_pid;
int dao_delay_over_pid;
int rt_timer_over_pid;

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
    vtimer_set_wakeup(&trickle_t_timer, t_time, timer_over_pid);
    vtimer_set_wakeup(&trickle_I_timer, I_time, interval_over_pid);

}

void init_trickle(void)
{
    /* Create threads */
    ack_received = true;
    timer_over_pid = thread_create(timer_over_buf, TRICKLE_TIMER_STACKSIZE,
                                   PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                   trickle_timer_over, "trickle_timer_over");

    interval_over_pid = thread_create(interval_over_buf, TRICKLE_INTERVAL_STACKSIZE,
                                      PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                      trickle_interval_over, "trickle_interval_over");
    dao_delay_over_pid = thread_create(dao_delay_over_buf, DAO_DELAY_STACKSIZE,
                                       PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                       dao_delay_over, "dao_delay_over");
    rt_timer_over_pid = thread_create(routing_table_buf, RT_STACKSIZE,
                                      PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                      rt_timer_over, "rt_timer_over");

}

void start_trickle(uint8_t DIOIntMin, uint8_t DIOIntDoubl,
                   uint8_t DIORedundancyConstant)
{
    c = 0;
    k = DIORedundancyConstant;
    Imin = pow(2, DIOIntMin);
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
    vtimer_set_wakeup(&trickle_t_timer, t_time, timer_over_pid);
    vtimer_set_wakeup(&trickle_I_timer, I_time, interval_over_pid);
}

void trickle_increment_counter(void)
{
    /* call this function, when received DIO message */
    c++;
}

void trickle_timer_over(void)
{
    ipv6_addr_t mcast;
    ipv6_set_all_nds_mcast_addr(&mcast);

    while(1) {
        thread_sleep();

        /* Laut RPL Spezifikation soll k=0 wie k= Unendlich behandelt werden, also immer gesendet werden */
        if((c < k) || (k == 0)) {
            send_DIO(&mcast);
        }
    }
}

void trickle_interval_over(void)
{
    while(1) {
        thread_sleep();
        I = I * 2;
        printf("TRICKLE new Interval %u\n", I);

        if(I == 0) {
            puts("[WARNING] Interval was 0");

            if(Imax == 0) {
                puts("[WARNING] Imax == 0");
            }

            I = (Imin << Imax);
        }

        if(I > (Imin << Imax)) {
            I = (Imin << Imax);
        }

        c = 0;
        t = (I / 2) + (rand() % (I - (I / 2) + 1));
        /* start timer */
        t_time = timex_set(0, t * 1000);
        timex_normalize(&t_time);
        I_time = timex_set(0, I * 1000);
        timex_normalize(&I_time);

        if(vtimer_set_wakeup(&trickle_t_timer, t_time, timer_over_pid) != 0) {
            puts("[ERROR] setting Wakeup");
        }

        if(vtimer_set_wakeup(&trickle_I_timer, I_time, interval_over_pid) != 0) {
            puts("[ERROR] setting Wakeup");
        }
    }

}

void delay_dao(void)
{
    dao_time = timex_set(DEFAULT_DAO_DELAY, 0);
    dao_counter = 0;
    ack_received = false;
    vtimer_remove(&dao_timer);
    vtimer_set_wakeup(&dao_timer, dao_time, dao_delay_over_pid);
}

/* This function is used for regular update of the routes. The Timer can be overwritten, as the normal delay_dao function gets called */
void long_delay_dao(void)
{
    dao_time = timex_set(REGULAR_DAO_INTERVAL, 0);
    dao_counter = 0;
    ack_received = false;
    vtimer_remove(&dao_timer);
    vtimer_set_wakeup(&dao_timer, dao_time, dao_delay_over_pid);
}

void dao_delay_over(void)
{
    while(1) {
        thread_sleep();

        if((ack_received == false) && (dao_counter < DAO_SEND_RETRIES)) {
            dao_counter++;
            send_DAO(NULL, 0, true, 0);
            dao_time = timex_set(DEFAULT_WAIT_FOR_DAO_ACK, 0);
            vtimer_set_wakeup(&dao_timer, dao_time, dao_delay_over_pid);
        }
        else if(ack_received == false) {
            long_delay_dao();
        }
    }
}

void dao_ack_received()
{
    ack_received = true;
    long_delay_dao();
}

void rt_timer_over(void)
{
    rpl_routing_entry_t *rt;

    while(1) {
        rpl_dodag_t *my_dodag = rpl_get_my_dodag();

        if(my_dodag != NULL) {
            rt = rpl_get_routing_table();

            for(uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
                if(rt[i].used) {
                    if(rt[i].lifetime <= 1) {
                        memset(&rt[i], 0, sizeof(rt[i]));
                    }
                    else {
                        rt[i].lifetime--;
                    }
                }
            }

            /* Parent is NULL for root too */
            if(my_dodag->my_preferred_parent != NULL) {
                if(my_dodag->my_preferred_parent->lifetime <= 1) {
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
}
