/**
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
#include "sixlowpan/mac.h"
#include "sixlowpan/dispatch_values.h"
#include "random.h"
#include "transceiver.h"

#include "clocksync/ftsp.h"
#include "gtimer.h"
//#include "x64toa.h"

#ifdef MODULE_CC110X_NG
#define FTSP_CALIBRATION_OFFSET ((uint32_t) 2300)

#elif MODULE_NATIVENET
#define FTSP_CALIBRATION_OFFSET ((uint32_t) 1500)

#else
#warning "Transceiver not supported by FTSP!"
#define FTSP_CALIBRATION_OFFSET ((uint32_t) 0) // unknown delay
#endif

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include <debug.h>

// Protocol parameters
#define FTSP_PREFERRED_ROOT (1) // node with id==1 will become root
#define FTSP_BEACON_INTERVAL (5 * 1000 * 1000) // in us


#define FTSP_BEACON_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define FTSP_CYCLIC_STACK_SIZE (KERNEL_CONF_STACKSIZE_PRINTF_FLOAT)
#define FTSP_BEACON_BUFFER_SIZE (64)

// threads
static void *beacon_thread(void *arg);
static void *cyclic_driver_thread(void *arg);

static void send_beacon(void);

static int beacon_pid = 0;
static int cyclic_driver_pid = 0;
static uint32_t beacon_interval = FTSP_BEACON_INTERVAL;
static uint32_t transmission_delay = FTSP_CALIBRATION_OFFSET;
static bool pause_protocol = true;

static uint16_t root_id = UINT16_MAX;
static uint16_t node_id = 0;

char ftsp_beacon_stack[FTSP_BEACON_STACK_SIZE];
char ftsp_cyclic_stack[FTSP_CYCLIC_STACK_SIZE];
char ftsp_beacon_buffer[FTSP_BEACON_BUFFER_SIZE] =
{ 0 };

static int8_t i, free_item, oldest_item;
static uint64_t local_average, age, oldest_time;
static uint8_t num_entries, table_entries, heart_beats, num_errors, seq_num;
static int64_t offset_average, time_error, a, b;
static int64_t local_sum, offset_sum;
static float skew;
static table_item table[FTSP_MAX_ENTRIES];

static void clear_table(void);
static void add_new_entry(ftsp_beacon_t *beacon, gtimer_timeval_t *toa);
static void calculate_conversion(void);
static uint16_t get_transceiver_addr(void);
/*
#ifdef DEBUG_ENABLED
static void print_beacon(ftsp_beacon_t *beacon);
#endif
*/

mutex_t ftsp_mutex;

void ftsp_init(void)
{
    mutex_init(&ftsp_mutex);

    skew = 0.0;
    local_average = 0;
    offset_average = 0;
    clear_table();
    heart_beats = 0;
    num_errors = 0;

    beacon_pid = thread_create(ftsp_beacon_stack, FTSP_BEACON_STACK_SIZE,
    PRIORITY_MAIN - 2, CREATE_STACKTEST, beacon_thread, NULL, "ftsp_beacon");

    puts("FTSP initialized");
}

static void *beacon_thread(void *arg)
{
    (void) arg;
    while (1)
    {
        thread_sleep();
        DEBUG("_ftsp_beacon_thread locking mutex\n");
        mutex_lock(&ftsp_mutex);
        memset(ftsp_beacon_buffer, 0, sizeof(ftsp_beacon_t));
        if (!pause_protocol)
        {
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

    while (1)
    {
        vtimer_usleep(beacon_interval);
        if (!pause_protocol)
        {
            DEBUG("_ftsp_cyclic_driver_thread: waking sending thread up");
            thread_wakeup(beacon_pid);
        }
    }
    return NULL;
}

static void send_beacon(void)
{
    DEBUG("_ftsp_send_beacon\n");
    gtimer_timeval_t now;
    ftsp_beacon_t *ftsp_beacon = (ftsp_beacon_t *) ftsp_beacon_buffer;
    gtimer_sync_now(&now);
    if ((root_id != 0xFFFF))
    {
        if (root_id == node_id)
        {
            if ((long) (now.local - local_average) >= 0x20000000)
            {
                local_average = now.local;
                offset_average = now.global - now.local;
            }
        }

        //      else
        //      {
        //          if (heart_beats >= ROOT_TIMEOUT)
        //          {
        //              PRINTF("FTSP: root timeout, declaring myself the root\n");
        //              heart_beats = 0;
        //              _ftsp_root_id = _ftsp_id;
        //              ++seq_num;
        //          }
        //      }

        if ((num_entries < FTSP_ENTRY_SEND_LIMIT) && (root_id != node_id))
        {
            ++heart_beats;
        }
        else
        {
            gtimer_sync_now(&now);
            ftsp_beacon->dispatch_marker = FTSP_PROTOCOL_DISPATCH;
            ftsp_beacon->id = node_id;
            // TODO: do we need to add the transmission delay to the offset?
            ftsp_beacon->offset = (int64_t) now.global - (int64_t) now.local;
            ftsp_beacon->local = now.local + transmission_delay;
            ftsp_beacon->relative_rate = now.rate;
            ftsp_beacon->root = root_id;
            ftsp_beacon->seq_number = seq_num;
#ifdef DEBUG_ENABLED
            /*
            print_beacon(ftsp_beacon);
            */
#endif
            sixlowpan_mac_send_ieee802154_frame(0, NULL, 8, ftsp_beacon_buffer,
                    sizeof(ftsp_beacon_t), 1);

            ++heart_beats;

            if (root_id == node_id)
                ++seq_num;
        }
    }
}

void ftsp_mac_read(uint8_t *frame_payload, uint16_t src, gtimer_timeval_t *toa)
{
    (void) src;
    DEBUG("ftsp_mac_read");
    mutex_lock(&ftsp_mutex);
    if (pause_protocol)
    {
        mutex_unlock(&ftsp_mutex);
        return;
    }

    ftsp_beacon_t *beacon = (ftsp_beacon_t *) frame_payload;

    if ((beacon->root < root_id)
            && !((heart_beats < FTSP_IGNORE_ROOT_MSG) && (root_id == node_id)))
    {
        root_id = beacon->root;
        seq_num = beacon->seq_number;
    }
    else
    {
        if ((root_id == beacon->root)
                && ((int8_t) (beacon->seq_number - seq_num) > 0))
        {
            seq_num = beacon->seq_number;
        }
        else
        {
            DEBUG(
                    "not (beacon->root < _ftsp_root_id) [...] and not (_ftsp_root_id == beacon->root)");
            mutex_unlock(&ftsp_mutex);
            return;
        }
    }

    if (root_id < node_id)
        heart_beats = 0;

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
    node_id = get_transceiver_addr();
    if (node_id == FTSP_PREFERRED_ROOT)
    {
        //_ftsp_root_timeout = 0;
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

    pause_protocol = false;
    if (cyclic_driver_pid == 0)
    {
        cyclic_driver_pid = thread_create(ftsp_cyclic_stack,
        FTSP_CYCLIC_STACK_SIZE,
        PRIORITY_MAIN - 2,
        CREATE_STACKTEST, cyclic_driver_thread, NULL, "ftsp_cyclic_driver");
    }

    DEBUG("FTSP enabled");
}

void ftsp_driver_timestamp(uint8_t *ieee802154_frame, uint8_t frame_length)
{

    if (ieee802154_frame[0] == FTSP_PROTOCOL_DISPATCH)
    {
        gtimer_timeval_t now;
        ieee802154_frame_t frame;
        uint8_t hdrlen = ieee802154_frame_read(ieee802154_frame, &frame,
                frame_length);
        ftsp_beacon_t *beacon = (ftsp_beacon_t *) frame.payload;
        gtimer_sync_now(&now);
        beacon->local = now.local + transmission_delay;
        beacon->offset = (int64_t) now.global - (int64_t) now.local;
        beacon->relative_rate = now.rate;
        memcpy(ieee802154_frame + hdrlen, beacon, sizeof(ftsp_beacon_t));
    }

}

int ftsp_is_synced(void)
{
    if ((num_entries >= FTSP_ENTRY_VALID_LIMIT) || (root_id == node_id))
        return FTSP_OK;
    else
        return FTSP_ERR;
}

static void calculate_conversion(void)
{
    DEBUG("calculate_conversion");
    for (i = 0; (i < FTSP_MAX_ENTRIES) && (table[i].state != FTSP_ENTRY_FULL); ++i)
        ;

    if (i >= FTSP_MAX_ENTRIES)
        return;   // table is empty

    local_average = table[i].local_time;
    offset_average = table[i].time_offset;

    local_sum = 0;
    offset_sum = 0;

    while (++i < FTSP_MAX_ENTRIES)
    {
        if (table[i].state == FTSP_ENTRY_FULL)
        {
            local_sum += (long) (table[i].local_time - local_average)
                    / table_entries;
            offset_sum += (long) (table[i].time_offset - offset_average)
                    / table_entries;
        }
    }

    local_average += local_sum;
    offset_average += offset_sum;

    local_sum = offset_sum = 0;
    for (i = 0; i < FTSP_MAX_ENTRIES; ++i)
    {
        if (table[i].state == FTSP_ENTRY_FULL)
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

    DEBUG("FTSP conversion calculated: num_entries=%u, is_synced=%u\n",
            num_entries, ftsp_is_synced());
}

static void add_new_entry(ftsp_beacon_t *beacon, gtimer_timeval_t *toa)
{
    DEBUG("calculate_conversion");
    free_item = -1;
    oldest_item = 0;
    age = 0;
    oldest_time = 0;

    table_entries = 0;

    time_error = (int64_t) (beacon->local + beacon->offset - toa->global);
    if (ftsp_is_synced() == FTSP_OK)
    {
        /*
#ifdef DEBUG_ENABLED
        char buf[60];
        DEBUG("FTSP synced, error %s\n", l2s(time_error, X64LL_SIGNED, buf));
#endif
    */
        if ((time_error > FTSP_ENTRY_THROWOUT_LIMIT)
                || (-time_error > FTSP_ENTRY_THROWOUT_LIMIT))
        {
            DEBUG("(big)\n");
            if (++num_errors > 3)
            {
                puts("FTSP: num_errors > 3 => clear_table()\n");
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
        DEBUG("FTSP not synced\n");
    }

    for (i = 0; i < FTSP_MAX_ENTRIES; ++i)
    {
        age = toa->local - table[i].local_time;

        if (age >= 0x7FFFFFFFL)
            table[i].state = FTSP_ENTRY_EMPTY;

        if (table[i].state == FTSP_ENTRY_EMPTY)
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

    table[free_item].state = FTSP_ENTRY_FULL;
    table[free_item].local_time = toa->local;
    table[free_item].time_offset = beacon->local + beacon->offset - toa->local;
}

static void clear_table(void)
{
    for (i = 0; i < FTSP_MAX_ENTRIES; ++i)
        table[i].state = FTSP_ENTRY_EMPTY;

    num_entries = 0;
}

#ifdef DEBUG_ENABLED
/*
static void print_beacon(ftsp_beacon_t *beacon)
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

