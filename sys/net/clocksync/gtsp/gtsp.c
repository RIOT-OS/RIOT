/**
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup gtsp
 * @{
 * @file    gtsp.c
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author  Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "thread.h"
#include "timex.h"
#include "vtimer.h"
#include "mutex.h"
#include "ieee802154_frame.h"
#include "net_if.h"
#include "random.h"
#include "transceiver.h"

#include "clocksync/gtsp.h"
#include "../common/send_helper.h"
#include "../common/receive_helper.h"
#include "gtimer.h"
#include "sixlowpan/dispatch_values.h"

#ifdef MODULE_CC110X_NG
#define GTSP_CALIBRATION_OFFSET ((uint32_t) 2220)

#elif MODULE_NATIVENET
#define GTSP_CALIBRATION_OFFSET ((uint32_t) 1500)

#else
#warning "Transceiver not supported by GTSP!"
#define GTSP_CALIBRATION_OFFSET ((uint32_t) 0) // unknown delay
#endif

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include <debug.h>

// Protocol parameters
#define GTSP_MAX_NEIGHBORS (10)
#define GTSP_BEACON_INTERVAL (30 * 1000 * 1000)  // in us
#define GTSP_JUMP_THRESHOLD (10) // in us
#define GTSP_MOVING_ALPHA 0.9
#define GTSP_SANE_OFFSET_CHECK (1)
#define GTSP_SANE_OFFSET_THRESHOLD ((int64_t)3145 * 10 * 1000 * 1000 * 1000) // 1 year in us

#define GTSP_BEACON_STACK_SIZE (KERNEL_CONF_STACKSIZE_MAIN)
#define GTSP_CYCLIC_STACK_SIZE (KERNEL_CONF_STACKSIZE_MAIN)
#define GTSP_BEACON_BUFFER_SIZE (64)

/* prototypes for threads */
static void *beacon_thread(void *arg);
static void *cyclic_driver_thread(void *arg);
//
static void send_beacon(void);
static uint16_t get_transceiver_addr(void);

/* neighbor table */
static gtsp_sync_point_t *neighbor_add(uint16_t addr,
                                       gtsp_sync_point_t *sync_point);
static gtsp_sync_point_t *neighbor_get(uint16_t addr);

static int beacon_pid = KERNEL_PID_UNDEF;
static int cyclic_driver_pid = KERNEL_PID_UNDEF;
static uint32_t beacon_interval = GTSP_BEACON_INTERVAL;
static uint32_t transmission_delay = GTSP_CALIBRATION_OFFSET;
static bool pause_protocol = true;
static uint32_t jump_threshold = GTSP_JUMP_THRESHOLD;
static bool jumped = false;
static uint32_t neighbor_count = 0;

char gtsp_beacon_stack[GTSP_BEACON_STACK_SIZE];
char gtsp_cyclic_stack[GTSP_CYCLIC_STACK_SIZE];
char gtsp_beacon_buffer[GTSP_BEACON_BUFFER_SIZE] =
{ 0 };

static gtsp_sync_point_t gtsp_neighbor_table[GTSP_MAX_NEIGHBORS] = {
    { 0 }
};

uint8_t gtsp_mac_buf[PAYLOAD_SIZE];

mutex_t gtsp_mutex;

void gtsp_init(void)
{
    mutex_init(&gtsp_mutex);

    for (int i = 0; i < GTSP_MAX_NEIGHBORS; i++) {
        gtsp_neighbor_table[i].src = 0;
    }

    beacon_pid = thread_create(gtsp_beacon_stack, GTSP_BEACON_STACK_SIZE,
                               PRIORITY_MAIN - 2, CREATE_STACKTEST, beacon_thread, NULL, "gtsp_beacon");

    cyclic_driver_pid = thread_create(gtsp_cyclic_stack, GTSP_CYCLIC_STACK_SIZE,
                                      PRIORITY_MAIN - 2,
                                      CREATE_STACKTEST, cyclic_driver_thread, NULL, "gtsp_cyclic_driver");

    clocksync_common_init_recv();
    puts("GTSP initialized");
}

static void *beacon_thread(void *arg)
{
    (void) arg;

    while (1) {
        thread_sleep();
        DEBUG("_gtsp_beacon_thread locking mutex\n");
        mutex_lock(&gtsp_mutex);
        memset(gtsp_beacon_buffer, 0, sizeof(gtsp_beacon_t));

        if (!pause_protocol) {
            send_beacon();
        }

        mutex_unlock(&gtsp_mutex);
        DEBUG("_gtsp_beacon_thread: mutex unlocked\n");
    }

    return NULL;
}

static void *cyclic_driver_thread(void *arg)
{
    (void) arg;

    uint16_t _gtsp_id = get_transceiver_addr();
    genrand_init((uint32_t) _gtsp_id);
    uint32_t random_wait = (100 + genrand_uint32() % GTSP_BEACON_INTERVAL);
    vtimer_usleep(random_wait);

    while (1) {
        vtimer_usleep(beacon_interval);

        if (!pause_protocol) {
            DEBUG("_gtsp_cyclic_driver_thread: waking sending thread up");
            thread_wakeup(beacon_pid);
        }
    }

    return NULL;
}

static void send_beacon(void)
{
    DEBUG("_gtsp_send_beacon\n");
    gtimer_timeval_t now;
    gtsp_beacon_t *gtsp_beacon = (gtsp_beacon_t *) gtsp_beacon_buffer;
    gtimer_sync_now(&now);
    gtsp_beacon->dispatch_marker = GTSP_PROTOCOL_DISPATCH;
    gtsp_beacon->local = now.local + transmission_delay;
    gtsp_beacon->global = now.global + transmission_delay;
    gtsp_beacon->relative_rate = now.rate;
    clocksync_common_send_broadcast(gtsp_mac_buf, gtsp_beacon, sizeof(gtsp_beacon_t));
}

static float compute_rate(void)
{
    DEBUG("gtsp_compute_rate\n");
    float avg_rate = gtimer_sync_get_relative_rate();
    int64_t sum_offset = 0;
    int offset_count = 0;
    int neighbor_count = 0;

    for (uint32_t i = 0; i < GTSP_MAX_NEIGHBORS; i++) {
        gtsp_sync_point_t *beacon = (gtsp_sync_point_t *) &gtsp_neighbor_table[i];

        if (beacon->src == 0) {
            continue;
        }

        int64_t offset = (int64_t) beacon->remote_global
                         - (int64_t) beacon->local_global;

        neighbor_count++;
        avg_rate += beacon->relative_rate;

        if (offset > -jump_threshold) {
            /* neighbor is ahead in time */
            sum_offset += offset;
            offset_count++;
        }
    }

    /* check for negative time offsets */
    if (offset_count > 0 && !jumped && sum_offset >= 0) {
        int64_t correction = sum_offset / (offset_count + 1);

        if (correction < jump_threshold) {
            gtimer_sync_set_global_offset(correction);
        }
    }

    jumped = false;
    avg_rate /= (neighbor_count + 1);

    /* safeguard: ensures sane clock rates in the presence of byzantine faults */
    if (avg_rate > 0.00005) {
        avg_rate = 0.00005;
    }
    else if (avg_rate < -0.00005) {
        avg_rate = -0.00005;
    }

    return avg_rate;
}

void gtsp_mac_read(uint8_t *frame_payload, uint16_t src, gtimer_timeval_t *toa)
{
    DEBUG("gtsp_mac_read");
    mutex_lock(&gtsp_mutex);
    gtsp_sync_point_t new_sync_point;
    gtsp_sync_point_t *sync_point;

    float relative_rate = 0.0f;
    gtsp_beacon_t gtsp_beacon;

    memcpy(&gtsp_beacon, frame_payload, sizeof(gtsp_beacon_t));

    if (pause_protocol) {
        /* don't accept beacons if gtsp is paused */
        mutex_unlock(&gtsp_mutex);
        return;
    }

    /* check for previously received beacons from the same node */
    DEBUG("gtsp_mac_read: Looking up src address: %" PRIu16 "\n", src);

    if (NULL != (sync_point = neighbor_get(src))) {
        DEBUG("gtsp_mac_read: _gtsp_buffer_lookup success!");
        /* calculate local time difference between beacons */
        int64_t delta_local = toa->local - sync_point->local_local;
        /* calculate estimate of local remote time between beacons */
        int64_t delta_remote = +(int64_t) gtsp_beacon.local
                               - (int64_t) sync_point->remote_local
                               + ((int64_t) gtsp_beacon.local
                                  - (int64_t) sync_point->remote_local)
                               * gtsp_beacon.relative_rate;
        /* estimate rate of the local clock relative to the rate of the remote clock */
        float current_rate = (delta_remote - delta_local) / (float) delta_local;
        /* use moving average filter in order to smoothen out short term rate fluctuations */
        relative_rate = GTSP_MOVING_ALPHA * sync_point->relative_rate
                        + (1 - GTSP_MOVING_ALPHA) * current_rate;
    }
    else {
        /* add a new sync_point to the table */
        new_sync_point.src = src;
        sync_point = neighbor_add(src, &new_sync_point);
    }

    int64_t offset = (int64_t) gtsp_beacon.global - (int64_t) toa->global;
#if GTSP_SANE_OFFSET_CHECK

    if (offset > GTSP_SANE_OFFSET_THRESHOLD
        || offset < -GTSP_SANE_OFFSET_THRESHOLD) {
        DEBUG("gtsp_mac_read: offset calculation yielded abnormal high value");
        DEBUG("gtsp_mac_read: skipping offending beacon");
        memset(sync_point, 0, sizeof(gtsp_sync_point_t));
        mutex_unlock(&gtsp_mutex);
        return;
    }

#endif /* GTSP_SANE_OFFSET_CHECK */

    /* store the received and calculated data in the sync point */
    sync_point->local_local = toa->local;
    sync_point->local_global = toa->global;
    sync_point->remote_local = gtsp_beacon.local;
    sync_point->remote_global = gtsp_beacon.global;
    sync_point->remote_rate = gtsp_beacon.relative_rate;
    sync_point->relative_rate = relative_rate;

    /* if our clock is to far behind jump to remote clock value */
    if (offset > jump_threshold) {
        jumped = true;
        gtimer_sync_set_global_offset(offset);
    }

    DEBUG("gtsp_mac_read: compute_rate");
    /* compute new relative clock rate based on new sync point */
    float avg_rate = compute_rate();
    gtimer_sync_set_relative_rate(avg_rate);

    mutex_unlock(&gtsp_mutex);
    DEBUG("gtsp_mac_read: mutex unlocked");
}

void gtsp_set_beacon_delay(uint32_t delay_in_sec)
{
    DEBUG("gtsp_set_beacon_delay: %"PRIu32"\n", delay_in_sec);
    beacon_interval = delay_in_sec * 1000 * 1000;
}

void gtsp_set_prop_time(uint32_t us)
{
    DEBUG("gtsp_set_prop_time: %"PRIu32"\n", us);
    transmission_delay = us;
}

void gtsp_pause(void)
{
    pause_protocol = true;
    DEBUG("GTSP disabled");
}

void gtsp_resume(void)
{
    pause_protocol = false;

    if (cyclic_driver_pid == 0) {
        cyclic_driver_pid = thread_create(gtsp_cyclic_stack,
                                          GTSP_CYCLIC_STACK_SIZE,
                                          PRIORITY_MAIN - 2,
                                          CREATE_STACKTEST, cyclic_driver_thread, NULL, "gtsp_cyclic_driver");
    }

    DEBUG("GTSP enabled with calibration offset: %" PRIu32 "\n", transmission_delay);
}

void gtsp_driver_timestamp(uint8_t *ieee802154_frame, uint8_t frame_length)
{
    if (ieee802154_frame[0] == GTSP_PROTOCOL_DISPATCH) {
        gtimer_timeval_t now;
        ieee802154_frame_t frame;
        uint8_t hdrlen = ieee802154_frame_read(ieee802154_frame, &frame,
                                               frame_length);
        gtsp_beacon_t *beacon = (gtsp_beacon_t *) frame.payload;
        gtimer_sync_now(&now);
        beacon->local = now.local + transmission_delay;
        beacon->global = now.global + transmission_delay;
        memcpy(ieee802154_frame + hdrlen, beacon, sizeof(gtsp_beacon_t));
    }
}

static gtsp_sync_point_t *neighbor_add(uint16_t addr,
                                       gtsp_sync_point_t *sync_point)
{
    gtsp_sync_point_t *sp;

    if (NULL == (sp = neighbor_get(addr))) {
        sp = &gtsp_neighbor_table[neighbor_count];

        if (neighbor_count == GTSP_MAX_NEIGHBORS - 1) {
            neighbor_count = 0;
        }
        else {
            neighbor_count++;
        }
    }

    memcpy(sp, sync_point, sizeof(gtsp_sync_point_t));
    return sp;
}

static gtsp_sync_point_t *neighbor_get(uint16_t addr)
{
    for (int i = 0; i < GTSP_MAX_NEIGHBORS; i++) {
        gtsp_sync_point_t *sync_point = &gtsp_neighbor_table[i];

        if (sync_point->src == addr) {
            return sync_point;
        }
    }

    return NULL;
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
