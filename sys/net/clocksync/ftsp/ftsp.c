/**
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup ftsp
 * @{
 * @file    ftsp.c
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
#include "random.h"
#include "transceiver.h"

#include "clocksync/ftsp.h"
#include "../common/send_helper.h"
#include "../common/receive_helper.h"
#include "sixlowpan/dispatch_values.h"
#include "gtimer.h"

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include <debug.h>

#ifdef MODULE_CC110X_NG
#define FTSP_CALIBRATION_OFFSET ((uint32_t) 2220)

#elif MODULE_NATIVENET
#define FTSP_CALIBRATION_OFFSET ((uint32_t) 1500)

#else
#warning "Transceiver not supported by FTSP!"
#define FTSP_CALIBRATION_OFFSET ((uint32_t) 0) // unknown delay
#endif

/* Protocol parameters */
#define FTSP_PREFERRED_ROOT (1) // node with id==1 will become root
#define FTSP_BEACON_INTERVAL (30 * 1000 * 1000) // beacon send interval
#define FTSP_MAX_SYNC_POINT_AGE (20 * 60 * 1000 * 1000) // max age for reg. table entry
#define FTSP_RATE_CALC_THRESHOLD (3) // at least 3 entries needed before rate is calculated
#define FTSP_MAX_ENTRIES (8) // number of entries in the regression table
#define FTSP_ENTRY_VALID_LIMIT (4) // number of entries to become synchronized
#define FTSP_ENTRY_SEND_LIMIT (3) // number of entries to send sync messages
#define FTSP_ROOT_TIMEOUT (3) // time to declare itself the root if no msg was received (in sync periods)
#define FTSP_IGNORE_ROOT_MSG (4) // after becoming the root ignore other roots messages (in send period)
#define FTSP_ENTRY_THROWOUT_LIMIT (300) // if time sync error is bigger than this clear the table
#define FTSP_SANE_OFFSET_CHECK (1)
#define FTSP_SANE_SYNC_OFFSET_THRESHOLD ((int64_t)1 * 1000 * 1000) // 1 min in us
#define FTSP_SANE_USYNC_OFFSET_THRESHOLD ((int64_t)31536 * 1000 * 1000 * 1000) // 1 year in us

/* easy to read status flags */
#define FTSP_OK (1)
#define FTSP_ERR (0)
#define FTSP_ENTRY_EMPTY (0)
#define FTSP_ENTRY_FULL (1)

#define FTSP_BEACON_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define FTSP_CYCLIC_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define FTSP_BEACON_BUFFER_SIZE (64)

/* threads */
static void *beacon_thread(void *arg);
static void *cyclic_driver_thread(void *arg);

static void send_beacon(void);
static void linear_regression(void);
static void clear_table(void);
static void add_new_entry(ftsp_beacon_t *beacon, gtimer_timeval_t *toa);
/* Removes the oldest not free entry of the table
   If the table is empty, nothing happens */
static void remove_last_entry(void);
static uint16_t get_transceiver_addr(void);

static int beacon_pid = KERNEL_PID_UNDEF;
static int cyclic_driver_pid = KERNEL_PID_UNDEF;

/* protocol state variables */
static uint32_t beacon_interval = FTSP_BEACON_INTERVAL;
static uint32_t transmission_delay = FTSP_CALIBRATION_OFFSET;
static bool pause_protocol = true;
static uint16_t root_id = UINT16_MAX;
static uint16_t node_id = 0, seq_num;
static uint8_t table_entries, heart_beats, num_errors;
static int64_t offset;
static float rate;
ftsp_table_item_t table[FTSP_MAX_ENTRIES];
#ifdef FTSP_SANE_OFFSET_CHECK
static uint64_t last_global = 0;
#endif

static char ftsp_beacon_stack[FTSP_BEACON_STACK_SIZE];
static char ftsp_cyclic_stack[FTSP_CYCLIC_STACK_SIZE];
char ftsp_beacon_buffer[FTSP_BEACON_BUFFER_SIZE] = { 0 };

uint8_t ftsp_mac_buf[PAYLOAD_SIZE];

mutex_t ftsp_mutex;

void ftsp_init(void)
{
    mutex_init(&ftsp_mutex);

    rate = 1.0;
    clear_table();
    heart_beats = 0;
    num_errors = 0;
    table_entries = 0;
    offset = 0;
    seq_num = 0;

    beacon_pid = thread_create(ftsp_beacon_stack, FTSP_BEACON_STACK_SIZE,
                               PRIORITY_MAIN - 2, CREATE_STACKTEST, beacon_thread, NULL, "ftsp_beacon");

    clocksync_common_init_recv();

    puts("FTSP initialized");
}

static void *beacon_thread(void *arg)
{
    (void) arg;

    while (1) {
        thread_sleep();
        DEBUG("_ftsp_beacon_thread locking mutex\n");
        mutex_lock(&ftsp_mutex);
        memset(ftsp_beacon_buffer, 0, sizeof(ftsp_beacon_t));

        if (!pause_protocol) {
            send_beacon();
        }

        mutex_unlock(&ftsp_mutex);
        DEBUG("_ftsp_beacon_thread: mutex unlocked\n");
    }

    return NULL;
}

static void *cyclic_driver_thread(void *arg)
{
    (void) arg;
    genrand_init((uint32_t) node_id);
    uint32_t random_wait = (100 + genrand_uint32() % FTSP_BEACON_INTERVAL);
    vtimer_usleep(random_wait);

    while (1) {
        vtimer_usleep(beacon_interval);

        if (!pause_protocol) {
            DEBUG("_ftsp_cyclic_driver_thread: waking sending thread up");
            thread_wakeup(beacon_pid);
        }
    }

    return NULL;
}

static void send_beacon(void)
{
    DEBUG("send_beacon\n");
    gtimer_timeval_t now;
    ftsp_beacon_t *ftsp_beacon = (ftsp_beacon_t *) ftsp_beacon_buffer;
    gtimer_sync_now(&now);

    if ((root_id != 0xFFFF)) {
        if ((table_entries < FTSP_ENTRY_SEND_LIMIT) && (root_id != node_id)) {
            heart_beats++;
        }
        else {
            gtimer_sync_now(&now);
#if FTSP_SANE_OFFSET_CHECK

            if (now.global > last_global + FTSP_SANE_USYNC_OFFSET_THRESHOLD) {
                DEBUG("send_beacon: trying to send abnormal high value");
                return;
            }

            last_global = now.global;
#endif
            ftsp_beacon->dispatch_marker = FTSP_PROTOCOL_DISPATCH;
            ftsp_beacon->id = node_id;
            ftsp_beacon->global = now.global + transmission_delay;
            ftsp_beacon->root = root_id;
            ftsp_beacon->seq_number = seq_num;

            clocksync_common_send_broadcast(ftsp_mac_buf, ftsp_beacon,
                                            sizeof(ftsp_beacon_t));

            heart_beats++;

            if (root_id == node_id) {
                seq_num++;
            }
        }
    }
}

void ftsp_mac_read(uint8_t *frame_payload, uint16_t src, gtimer_timeval_t *toa)
{
    (void) src;
    DEBUG("ftsp_mac_read");
    ftsp_beacon_t *beacon = (ftsp_beacon_t *) frame_payload;
    mutex_lock(&ftsp_mutex);

    if (pause_protocol) {
        mutex_unlock(&ftsp_mutex);
        return;
    }

    if ((beacon->root < root_id)
        && !((heart_beats < FTSP_IGNORE_ROOT_MSG) && (root_id == node_id))) {
        root_id = beacon->root;
        seq_num = beacon->seq_number;
    }
    else {
        if ((root_id == beacon->root)
            && ((int8_t)(beacon->seq_number - seq_num) > 0)) {
            seq_num = beacon->seq_number;
        }
        else {
            DEBUG("not (beacon->root < _ftsp_root_id) [...] and not (_ftsp_root_id == beacon->root)");
            mutex_unlock(&ftsp_mutex);
            return;
        }
    }

    if (root_id < node_id) {
        heart_beats = 0;
    }

    add_new_entry(beacon, toa);
    linear_regression();
    int64_t est_global = offset + ((int64_t) toa->local) * (rate);
    int64_t offset_global = est_global - (int64_t) toa->global;

#if FTSP_SANE_OFFSET_CHECK

    if (ftsp_is_synced()) {
        if (offset_global > FTSP_SANE_SYNC_OFFSET_THRESHOLD
            || offset_global < -FTSP_SANE_SYNC_OFFSET_THRESHOLD) {
            DEBUG("ftsp_mac_read: offset calculation yielded abnormal high value");
            DEBUG("ftsp_mac_read: skipping offending beacon");
            remove_last_entry();
            num_errors++;
            mutex_unlock(&ftsp_mutex);
            return;
        }
    }
    else {
        if (offset_global > FTSP_SANE_USYNC_OFFSET_THRESHOLD
            || offset_global < -FTSP_SANE_USYNC_OFFSET_THRESHOLD) {
            DEBUG("ftsp_mac_read: offset calculation yielded abnormal high value");
            DEBUG("ftsp_mac_read: skipping offending beacon");
            remove_last_entry();
            num_errors++;
            mutex_unlock(&ftsp_mutex);
            return;
        }
    }

#endif /* FTSP_SANE_OFFSET_CHECK */

    offset = offset_global;

    gtimer_sync_set_global_offset(offset_global);

    if (table_entries >= FTSP_RATE_CALC_THRESHOLD) {
        gtimer_sync_set_relative_rate(rate - 1);
    }

    mutex_unlock(&ftsp_mutex);
}

void ftsp_set_beacon_delay(uint32_t delay_in_sec)
{
    beacon_interval = delay_in_sec * 1000 * 1000;
}

void ftsp_set_prop_time(uint32_t us)
{
    transmission_delay = us;
}

void ftsp_pause(void)
{
    pause_protocol = true;
    DEBUG("FTSP disabled");
}

void ftsp_resume(void)
{
    DEBUG("ftsp: resume");
    node_id = get_transceiver_addr();

    if (node_id == FTSP_PREFERRED_ROOT) {
        root_id = node_id;
    }
    else {
        root_id = 0xFFFF;
    }

    rate = 1.0;
    clear_table();
    heart_beats = 0;
    num_errors = 0;

    pause_protocol = false;

    if (cyclic_driver_pid == 0) {
        cyclic_driver_pid = thread_create(ftsp_cyclic_stack,
                                          FTSP_CYCLIC_STACK_SIZE,
                                          PRIORITY_MAIN - 2,
                                          CREATE_STACKTEST, cyclic_driver_thread, NULL, "ftsp_cyclic_driver");
    }
}

void ftsp_driver_timestamp(uint8_t *ieee802154_frame, uint8_t frame_length)
{
    if (ieee802154_frame[0] == FTSP_PROTOCOL_DISPATCH) {
        gtimer_timeval_t now;
        ieee802154_frame_t frame;
        uint8_t hdrlen = ieee802154_frame_read(ieee802154_frame, &frame, frame_length);
        ftsp_beacon_t *beacon = (ftsp_beacon_t *) frame.payload;
        gtimer_sync_now(&now);
#if FTSP_SANE_OFFSET_CHECK

        if (now.global > last_global + FTSP_SANE_USYNC_OFFSET_THRESHOLD) {
            DEBUG("send_beacon: trying to send abnormal high value");
            return;
        }

#endif
        beacon->global = now.global + transmission_delay;
        memcpy(ieee802154_frame + hdrlen, beacon, sizeof(ftsp_beacon_t));
    }
}

int ftsp_is_synced(void)
{
    if ((table_entries >= FTSP_ENTRY_VALID_LIMIT) || (root_id == node_id)) {
        return FTSP_OK;
    }
    else {
        return FTSP_ERR;
    }
}

static void linear_regression(void)
{
    DEBUG("linear_regression");
    int64_t sum_local = 0, sum_global = 0, covariance = 0,
            sum_local_squared = 0;

    if (table_entries == 0) {
        return;
    }

    for (uint8_t i = 0; i < FTSP_MAX_ENTRIES; i++) {
        if (table[i].state == FTSP_ENTRY_FULL) {
            sum_local += table[i].local;
            sum_global += table[i].global;
            sum_local_squared += table[i].local * table[i].local;
            covariance += table[i].local * table[i].global;
        }
    }

    if (table_entries > 1) {
        rate = (covariance - (sum_local * sum_global) / table_entries);
        rate /= (sum_local_squared - ((sum_local * sum_local) / table_entries));
    }
    else {
        rate = 1.0;
    }

    offset = (sum_global - rate * sum_local) / table_entries;

    DEBUG("FTSP linear_regression calculated: num_entries=%u, is_synced=%u\n",
          table_entries, ftsp_is_synced());
}

/* XXX: This function not only adds an entry but also removes old entries. */
static void add_new_entry(ftsp_beacon_t *beacon, gtimer_timeval_t *toa)
{
    int8_t free_item = -1;
    uint8_t oldest_item = 0;
    uint64_t oldest_time = UINT64_MAX;
    uint64_t limit_age = toa->local - FTSP_MAX_SYNC_POINT_AGE;

    /* check for unsigned wrap around */
    if (toa->local < FTSP_MAX_SYNC_POINT_AGE) {
        limit_age = 0;
    }

    table_entries = 0;

    int64_t time_error = (int64_t)(beacon->global - toa->global);

    if (ftsp_is_synced() == FTSP_OK) {
        if ((time_error > FTSP_ENTRY_THROWOUT_LIMIT)
            || (-time_error > FTSP_ENTRY_THROWOUT_LIMIT)) {
            DEBUG("ftsp: error large; new root elected?\n");

            if (++num_errors > 3) {
                DEBUG("ftsp: number of errors to high clearing table\n");
                clear_table();
            }
        }
        else {
            num_errors = 0;
        }
    }
    else {
        DEBUG("ftsp: not synced (yet)\n");
    }

    int del_because_old = 0;

    for (uint8_t i = 0; i < FTSP_MAX_ENTRIES; i++) {
        if (table[i].local < limit_age) {
            table[i].state = FTSP_ENTRY_EMPTY;
            del_because_old++;
        }

        if (table[i].state == FTSP_ENTRY_EMPTY) {
            free_item = i;
        }
        else {
            table_entries++;
        }

        if (oldest_time > table[i].local) {
            oldest_time = table[i].local;
            oldest_item = i;
        }
    }

    if (free_item < 0) {
        free_item = oldest_item;
    }
    else {
        table_entries++;
    }

    table[free_item].state = FTSP_ENTRY_FULL;
    table[free_item].local = toa->local;
    table[free_item].global = beacon->global;
}

static void remove_last_entry(void)
{
    int8_t newest_item = -1;
    uint64_t newest_age = 0;

    for (uint8_t i = 0; i < FTSP_MAX_ENTRIES; i++) {
        if (table[i].state == FTSP_ENTRY_FULL && newest_age < table[i].local) {
            newest_age = table[i].local;
            newest_item = i;
        }
    }

    /* table was empty, nothing to do */
    if (newest_item == -1) {
        return;
    }

    table[newest_item].state = FTSP_ENTRY_EMPTY;
    table_entries--;
}

static void clear_table(void)
{
    for (uint8_t i = 0; i < FTSP_MAX_ENTRIES; i++) {
        table[i].state = FTSP_ENTRY_EMPTY;
    }

    table_entries = 0;
}

static uint16_t get_transceiver_addr(void)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    radio_address_t a;

    if (transceiver_pid < 0) {
        puts("ftsp: Transceiver not initialized");
        return 1;
    }

    tcmd.transceivers = TRANSCEIVER_DEFAULT;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;
    mesg.type = GET_ADDRESS;

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    return a;
}

