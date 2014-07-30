/**
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup pulsesync
 * @{
 * @file    pulsesync.c
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
#include "sixlowpan/mac.h"
#include "sixlowpan/dispatch_values.h"
#include "random.h"
#include "transceiver.h"


#include "clocksync/pulsesync.h"
#include "gtimer.h"

//#include "x64toa.h"

#ifdef MODULE_CC110X_NG
#define PULSESYNC_CALIBRATION_OFFSET ((uint32_t) 2300)

#elif MODULE_NATIVENET
#define PULSESYNC_CALIBRATION_OFFSET ((uint32_t) 1500)

#else
#warning "Transceiver not supported by PulseSync!"
#define PULSESYNC_CALIBRATION_OFFSET ((uint32_t) 0) // unknown delay
#endif

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include <debug.h>

// Protocol parameters
#define PULSESYNC_PREFERRED_ROOT (1) // node with id==1 will become root
#define PULSESYNC_BEACON_INTERVAL (5 * 1000 * 1000) // in us

#define PULSESYNC_BEACON_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define PULSESYNC_CYCLIC_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define PULSESYNC_BEACON_BUFFER_SIZE (64)

static void *root_beacon_thread(void *arg);
static void *cyclic_driver_thread(void *arg);
static void send_beacon(uint64_t local, int64_t offset, float skew);

static int beacon_thread_id = 0;
static int driver_thread_id = 0;
static uint32_t beacon_interval = PULSESYNC_BEACON_INTERVAL;
static uint32_t transmission_delay = PULSESYNC_CALIBRATION_OFFSET;
static bool protocol_pause = true;

static uint16_t root_id = UINT16_MAX;
static uint16_t node_id = 0;

static char beacon_stack[PULSESYNC_BEACON_STACK_SIZE];
static char cyclic_stack[PULSESYNC_CYCLIC_STACK_SIZE];
char pulsesync_beacon_buffer[PULSESYNC_BEACON_BUFFER_SIZE] =
{ 0 };

static int8_t i, free_item, oldest_item;
static uint64_t local_average, age, oldest_time;
static uint8_t num_entries, table_entries, heart_beats, num_errors, seq_num;
static int64_t offset_average, time_error, a, b;
static float skew;
static table_item table[PULSESYNC_MAX_ENTRIES];

static void clear_table(void);
static void add_new_entry(pulsesync_beacon_t *beacon, gtimer_timeval_t *toa);
static void calculate_conversion(void);
static uint16_t get_transceiver_addr(void);
/*
#ifdef DEBUG_ENABLED
static void print_beacon(pulsesync_beacon_t *beacon);
#endif
*/

mutex_t pulsesync_mutex;

void pulsesync_init(void)
{
    mutex_init(&pulsesync_mutex);

    skew = 0.0;
    local_average = 0;
    offset_average = 0;
    clear_table();
    heart_beats = 0;
    num_errors = 0;

    puts("PULSESYNC initialized");
}

void pulsesync_mac_read(uint8_t *frame_payload, uint16_t src,
        gtimer_timeval_t *toa)
{
    DEBUG("pulsesync_mac_read");
    mutex_lock(&pulsesync_mutex);
    pulsesync_beacon_t *beacon = (pulsesync_beacon_t *) frame_payload;

    if (protocol_pause || node_id == root_id || beacon->seq_number < seq_num)
    {
        // if paused or beacon is not new -> ignore
        mutex_unlock(&pulsesync_mutex);
        return;
    }

    add_new_entry(beacon, toa);
    calculate_conversion();

    gtimer_timeval_t now;
    gtimer_sync_now(&now);
    int64_t local_diff = (int64_t) table[free_item].local_time - local_average;

    int64_t offset_diff = skew * local_diff;

    int64_t offset_estimated = offset_average + offset_diff;

    int64_t offset_now = (int64_t) now.global - (int64_t) now.local;

    int64_t offset = offset_estimated - offset_now;

    gtimer_sync_set_global_offset(offset);
    gtimer_sync_set_relative_rate(skew);
    gtimer_sync_now(&now);
    if (node_id != root_id)
    {
        // forward this beacon
        send_beacon(now.local, (int64_t)now.global - (int64_t)now.local, now.rate);
    }

    mutex_unlock(&pulsesync_mutex);
}

void pulsesync_set_beacon_delay(uint32_t delay_in_sec)
{
    beacon_interval = delay_in_sec * 1000 * 1000;
}

void pulsesync_set_prop_time(uint32_t us)
{
    transmission_delay = us;
}

void pulsesync_pause(void)
{
    protocol_pause = true;
    DEBUG("PULSESYNC disabled");
}

void pulsesync_resume(void)
{
    node_id = get_transceiver_addr();
    if (node_id == PULSESYNC_PREFERRED_ROOT)
    {
        //_pulsesync_root_timeout = 0;
        root_id = node_id;
    }
    else
    {
        root_id = 0xFFFF;
    }
    skew = 0.0;
    local_average = 0;
    offset_average = 0;
    clear_table();
    heart_beats = 0;
    num_errors = 0;

    protocol_pause = false;

    if (beacon_thread_id == 0 && (root_id == node_id))
    {
        beacon_thread_id = thread_create(beacon_stack,
        PULSESYNC_BEACON_STACK_SIZE,
        PRIORITY_MAIN - 2, CREATE_STACKTEST, root_beacon_thread, NULL,
                "pulsesync_beacon");
    }

    if (driver_thread_id == 0 && (root_id == node_id))
    {
        driver_thread_id = thread_create(cyclic_stack,
        PULSESYNC_CYCLIC_STACK_SIZE,
        PRIORITY_MAIN - 2,
        CREATE_STACKTEST, cyclic_driver_thread, NULL, "pulsesync_cyclic_driver");
    }

    DEBUG("PULSESYNC enabled");
}

void pulsesync_driver_timestamp(uint8_t *ieee802154_frame, uint8_t frame_length)
{

    if (ieee802154_frame[0] == PULSESYNC_PROTOCOL_DISPATCH)
    {
        gtimer_timeval_t now;
        ieee802154_frame_t frame;
        uint8_t hdrlen = ieee802154_frame_read(ieee802154_frame, &frame,
                frame_length);
        pulsesync_beacon_t *beacon = (pulsesync_beacon_t *) frame.payload;
        gtimer_sync_now(&now);
        beacon->local = now.local + transmission_delay;
        beacon->offset = (int64_t) now.global - (int64_t) now.local;
        beacon->relative_rate = now.rate;
        memcpy(ieee802154_frame + hdrlen, beacon, sizeof(pulsesync_beacon_t));
    }

}

int pulsesync_is_synced(void)
{
    if ((num_entries >= PULSESYNC_ENTRY_VALID_LIMIT) || (root_id == node_id))
        return PULSESYNC_OK;
    else
        return PULSESYNC_ERR;
}

static void *root_beacon_thread(void *arg)
{
    while (1)
    {
        thread_sleep();
        DEBUG("root_beacon_thread locking mutex\n");
        mutex_lock(&pulsesync_mutex);
        memset(pulsesync_beacon_buffer, 0, sizeof(pulsesync_beacon_t));
        if (!protocol_pause)
        {
            gtimer_timeval_t now;
            gtimer_sync_now(&now);
            send_beacon(now.local, 0, 0.0f);
        }
        mutex_unlock(&pulsesync_mutex);
        DEBUG("root_beacon_thread: mutex unlocked\n");
    }
    return NULL;
}

static void send_beacon(uint64_t local, int64_t offset, float skew)
{
    DEBUG("_pulsesync_send_beacon\n");
    gtimer_timeval_t now;
    pulsesync_beacon_t *pulsesync_beacon =
            (pulsesync_beacon_t *) pulsesync_beacon_buffer;

    gtimer_sync_now(&now);
    pulsesync_beacon->dispatch_marker = PULSESYNC_PROTOCOL_DISPATCH;
    pulsesync_beacon->id = node_id;
    // TODO: do we need to add the transmission delay to the offset?
    pulsesync_beacon->offset = offset;
    pulsesync_beacon->local = local + transmission_delay;
    pulsesync_beacon->relative_rate = skew;
    pulsesync_beacon->root = root_id;
    pulsesync_beacon->seq_number = seq_num;
#ifdef DEBUG_ENABLED
    print_beacon(pulsesync_beacon);
#endif
    sixlowpan_mac_send_ieee802154_frame(0, NULL, 8, pulsesync_beacon_buffer,
            sizeof(pulsesync_beacon_t), 1);
    seq_num++;
}

static void *cyclic_driver_thread(void *arg)
{
    genrand_init((uint32_t) node_id);
    uint32_t random_wait = (100 + genrand_uint32() % PULSESYNC_BEACON_INTERVAL);
    vtimer_usleep(random_wait);

    while (1)
    {
        vtimer_usleep(beacon_interval);
        if (!protocol_pause)
        {
            DEBUG("_pulsesync_cyclic_driver_thread: waking sending thread up");
            thread_wakeup(beacon_thread_id);
        }
    }
    return NULL;
}

static void calculate_conversion(void)
{
    DEBUG("calculate_conversion");
    for (i = 0;
            (i < PULSESYNC_MAX_ENTRIES)
                    && (table[i].state != PULSESYNC_ENTRY_FULL); ++i)
        ;

    if (i >= PULSESYNC_MAX_ENTRIES)
        return;   // table is empty

    local_average = table[i].local_time;
    offset_average = table[i].time_offset;

    int64_t local_sum = 0;
    int64_t offset_sum = 0;

    while (++i < PULSESYNC_MAX_ENTRIES)
    {
        if (table[i].state == PULSESYNC_ENTRY_FULL)
        {
            local_sum += table[i].local_time - local_average;
            offset_sum += table[i].time_offset - offset_average;
        }
    }

    local_average += local_sum / table_entries;
    offset_average += offset_sum / table_entries;

    local_sum = offset_sum = 0;
    for (i = 0; i < PULSESYNC_MAX_ENTRIES; ++i)
    {
        if (table[i].state == PULSESYNC_ENTRY_FULL)
        {
            a = table[i].local_time - local_average;
            b = table[i].time_offset - offset_average;

            local_sum += (long long) a * a;
            offset_sum += (long long) a * b;
        }
    }

    if (local_sum != 0)
        skew = (float) offset_sum / (float) local_sum;

    num_entries = table_entries;

    DEBUG("PULSESYNC conversion calculated: num_entries=%u, is_synced=%u\n",
            num_entries, pulsesync_is_synced());
}

static void add_new_entry(pulsesync_beacon_t *beacon, gtimer_timeval_t *toa)
{
    DEBUG("add_new_entry");

    free_item = -1;
    oldest_item = 0;
    age = 0;
    oldest_time = 0;

    table_entries = 0;

    time_error = (int64_t) (beacon->local + beacon->offset - toa->global);
    if (pulsesync_is_synced() == PULSESYNC_OK)
    {
        /*
#ifdef DEBUG_ENABLED
        char buf[60];
        printf("PULSESYNC synced, error %s\n", l2s(time_error, X64LL_SIGNED, buf));
#endif
*/
        if ((time_error > PULSESYNC_ENTRY_THROWOUT_LIMIT)
                || (-time_error > PULSESYNC_ENTRY_THROWOUT_LIMIT))
        {
            DEBUG("(big)\n");
            if (++num_errors > 3)
            {
                DEBUG("PULSESYNC: num_errors > 3 => clear_table()\n");
                clear_table();
            }
        }
        else
        {
            DEBUG("(small)\n");
            num_errors = 0;
        }
    }
    else
    {
        DEBUG("PULSESYNC not synced\n");
    }

    for (i = 0; i < PULSESYNC_MAX_ENTRIES; ++i)
    {
        age = toa->local - table[i].local_time;

        if (age >= 0x7FFFFFFFL)
            table[i].state = PULSESYNC_ENTRY_EMPTY;

        if (table[i].state == PULSESYNC_ENTRY_EMPTY)
            free_item = i;
        else
            ++table_entries;

        if (age >= oldest_time)
        {
            oldest_time = age;
            oldest_item = i;
        }
    }

    if (free_item < 0)
        free_item = oldest_item;
    else
        ++table_entries;

    table[free_item].state = PULSESYNC_ENTRY_FULL;
    table[free_item].local_time = toa->local;
    table[free_item].time_offset = beacon->local + beacon->offset - toa->local;
}

static void clear_table(void)
{
    for (i = 0; i < PULSESYNC_MAX_ENTRIES; ++i)
        table[i].state = PULSESYNC_ENTRY_EMPTY;

    num_entries = 0;
}

#ifdef DEBUG_ENABLED
/*
static void print_beacon(pulsesync_beacon_t *beacon)
{
    char buf[66];
    printf("----\nbeacon: \n");
    printf("\t id: %"PRIu16"\n", beacon->id);
    printf("\t root: %"PRIu16"\n", beacon->root);
    printf("\t seq_number: %"PRIu16"\n", beacon->seq_number);
    printf("\t local: %s\n", l2s(beacon->local, X64LL_SIGNED, buf));
    printf("\t offset: %s\n", l2s(beacon->offset, X64LL_SIGNED, buf));
    printf("\t relative_rate: %"PRId32"\n----\n",
            (int32_t) (beacon->relative_rate * 1000 * 1000 * 1000));
}
*/
#endif /* DEBUG_ENABLED */

static uint16_t get_transceiver_addr(void)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    radio_address_t a;

    if (transceiver_pid < 0)
    {
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

