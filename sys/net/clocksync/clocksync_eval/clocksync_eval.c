/**
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup clocksync_eval
 * @{
 * @file    clocksync_eval.c
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author  Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "vtimer.h"
#include "mutex.h"
#include "ieee802154_frame.h"
#include "net_if.h"
#include "sixlowpan/dispatch_values.h"
#include "random.h"
#include "msg.h"
#include "transceiver.h"
#include "hwtimer.h"

#include "gtimer.h"
#include "clocksync/clocksync_eval.h"
#include "../common/send_helper.h"
#include "../common/receive_helper.h"

#ifdef MODULE_GTSP
#include "clocksync/gtsp.h"
#endif
#ifdef MODULE_FTSP
#include "clocksync/ftsp.h"
#endif
#ifdef MODULE_PULSESYNC
#include "clocksync/pulsesync.h"
#endif
#ifdef MODULE_CC110X_NG
/* for radio statistics */
#include "cc110x_ng/cc110x-interface.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Protocol parameters */
#define CLOCKSYNC_EVAL_BEACON_INTERVAL (10 * 1000 * 1000) /* default to 10s */
#define CLOCKSYNC_EVAL_HEARTBEAT_INTERVAL (1 * 1000 * 1000) /* default to 1s */


#define CLOCKSYNC_EVAL_BEACON_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define CLOCKSYNC_EVAL_HEARTBEAT_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define CLOCKSYNC_EVAL_CYCLIC_BEACON_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define CLOCKSYNC_EVAL_BEACON_BUFFER_SIZE (64)

/* prototypes for threads */
static void *beacon_send_thread(void *arg);
static void *cyclic_driver_thread(void *arg);

static void send_beacon(void);
static uint16_t get_transceiver_addr(void);

static uint32_t beacon_counter = 0;
static int beacon_pid = KERNEL_PID_UNDEF;
static uint32_t beacon_interval = CLOCKSYNC_EVAL_BEACON_INTERVAL;
static uint32_t heartbeat_interval = CLOCKSYNC_EVAL_HEARTBEAT_INTERVAL;
static uint32_t beacon_interval_jitter = 5000;
static uint32_t beacon_interval_lower = 5000;
static uint16_t node_id = 1;

static bool pause_protocol = true;
static bool heartbeat_pause = false;

char clocksync_eval_beacon_stack[CLOCKSYNC_EVAL_BEACON_STACK_SIZE];
char clocksync_eval_heartbeat_stack[CLOCKSYNC_EVAL_HEARTBEAT_STACK_SIZE];
char clocksync_eval_cyclic_beacon_stack[CLOCKSYNC_EVAL_CYCLIC_BEACON_STACK_SIZE];
char clocksync_eval_beacon_buffer[CLOCKSYNC_EVAL_BEACON_BUFFER_SIZE] = { 0 };

uint8_t clocksync_eval_mac_buf[PAYLOAD_SIZE];

mutex_t clocksync_eval_mutex;

void clocksync_eval_init(void)
{
    mutex_init(&clocksync_eval_mutex);

    beacon_pid = thread_create(clocksync_eval_beacon_stack,
                               CLOCKSYNC_EVAL_BEACON_STACK_SIZE, PRIORITY_MAIN - 2,
                               CREATE_STACKTEST|CREATE_SLEEPING,
                               beacon_send_thread, NULL, "clocksync_eval_beacon");

    thread_create(clocksync_eval_cyclic_beacon_stack,
                  CLOCKSYNC_EVAL_CYCLIC_BEACON_STACK_SIZE,
                  PRIORITY_MAIN - 2, CREATE_STACKTEST,
                  cyclic_driver_thread, NULL,
                  "clocksync_eval_driver_beacon");

    clocksync_common_init_recv();
}

void clocksync_eval_mac_read(uint8_t *payload, uint16_t src,
                             gtimer_timeval_t *gtimer_toa)
{
    mutex_lock(&clocksync_eval_mutex);
    DEBUG("clocksync_eval_mac_read\n");
    clocksync_eval_beacon_t *beacon = (clocksync_eval_beacon_t *) payload;
    printf("#et, a: %" PRIu16 ",", src);
    printf(" c: %"PRIu32",", beacon->counter);
    printf(" tl: %" PRIu64",", gtimer_toa->local);
    printf(" tg: %" PRIu64 "\n", gtimer_toa->global);
    mutex_unlock(&clocksync_eval_mutex);
}

void clocksync_eval_set_beacon_interval(uint32_t lower_delay_in_ms,
                                        uint32_t jitter_in_ms)
{
    beacon_interval_jitter = jitter_in_ms;
    beacon_interval_lower = lower_delay_in_ms;
}

void clocksync_eval_set_heartbeat_interval(uint32_t delay_in_ms)
{
    heartbeat_interval = delay_in_ms * 1000;
}

void clocksync_eval_pause_sending(void)
{
    pause_protocol = true;
    DEBUG("clocksync_eval beacon sending off");
}

void clocksync_eval_resume_sending(void)
{
    genrand_init((uint32_t) get_transceiver_addr());
    node_id = get_transceiver_addr();

    pause_protocol = false;
    DEBUG("clocksync_eval beacon sending on");
}

void clocksync_eval_pause_heartbeat(void)
{
    heartbeat_pause = true;
    DEBUG("clocksync_eval heartbeat off");
}

void clocksync_eval_resume_heartbeat(void)
{
    node_id = get_transceiver_addr();
    heartbeat_pause = false;
    DEBUG("clocksync_eval heartbeat on");
}

static void *beacon_send_thread(void *arg)
{
    (void) arg;

    while (1) {
        if (!pause_protocol) {
            mutex_lock(&clocksync_eval_mutex);
            send_beacon();

            if (!heartbeat_pause) {
#ifdef GTIMER_USE_VTIMER
                timex_t now;
                vtimer_now(&now);
                uint64_t local;
                local = timex_uint64(now);
                printf("#eh,");
                printf(" a: %"PRIu16 ",", node_id);
                printf(" gl: %" PRIu64 ",", local);
                printf(" gg: %" PRIu64 ",", local);
                printf(" gr: 100");
#else
                gtimer_timeval_t now;
                gtimer_sync_now(&now);
                printf("#eh,");
                printf(" a: %"PRIu16 ",", node_id);
                printf(" gl: %" PRIu64 ",", now.local);
                printf(" gg: %" PRIu64 ",", now.global);
                /* display rate as integer; newlib's printf and floats do no play nice together */
                printf(" gr: %d", (int)(now.rate * 1000000000));
#endif
#ifdef MODULE_CC110X_NG
                printf(", pi: %"PRIu32, cc110x_statistic.packets_in);
                printf(", po: %"PRIu32, cc110x_statistic.raw_packets_out);
                printf(", cr: %"PRIu32, cc110x_statistic.packets_in_crc_fail);
                printf(", s: %"PRIu32, cc110x_statistic.packets_in_while_tx);
#endif
                printf("\n");

            }

            beacon_interval =
                (beacon_interval_lower
                 + genrand_uint32()
                 % beacon_interval_jitter)
                * 1000;
            DEBUG("beacon_send_thread: new beacon interval: %"PRIu32 "\n",
                  beacon_interval);
            mutex_unlock(&clocksync_eval_mutex);
        }
        DEBUG("beacon_send_thread: sleeping\n");
        thread_sleep();
        DEBUG("beacon_send_thread: woke up\n");
    }

    return NULL;
}

static void *cyclic_driver_thread(void *arg)
{
    (void) arg;

    while (1) {
        vtimer_usleep(beacon_interval);

        if (!pause_protocol) {
            DEBUG("cyclic_driver_thread: waking sender thread up\n");
            thread_wakeup(beacon_pid);
        }
    }

    return NULL;
}

static void send_beacon(void)
{
    puts("send_beacon");
    clocksync_eval_beacon_t *clocksync_eval_beacon =
        (clocksync_eval_beacon_t *) clocksync_eval_beacon_buffer;
    memset(clocksync_eval_beacon_buffer, 0, sizeof(clocksync_eval_beacon_t));
    clocksync_eval_beacon->dispatch_marker = CLOCKSYNC_EVAL_PROTOCOL_DISPATCH;
    clocksync_eval_beacon->counter = beacon_counter;

    clocksync_common_send_broadcast(clocksync_eval_mac_buf, clocksync_eval_beacon,
                                    sizeof(clocksync_eval_beacon_t));
    beacon_counter++;
}

static uint16_t get_transceiver_addr(void)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    radio_address_t a;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return 1;
    }

    tcmd.transceivers = TRANSCEIVER_DEFAULT;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;
    mesg.type = GET_ADDRESS;

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    return a;
}
