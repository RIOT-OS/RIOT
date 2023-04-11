/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Bus statistic collection for I2C
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdio.h>
#include <string.h>

#include "bus_stats.h"
#include "cpu.h"
#include "irq.h"
#include "kernel_defines.h"
#include "mutex.h"
#include "mutex.h"
#include "periph/i2c.h"
#include "periph_conf.h"
#include "time_units.h"
#include "xfa.h"
#include "ztimer.h"

extern void __real_i2c_acquire(i2c_t bus);
extern void __real_i2c_release(i2c_t bus);
extern int __real_i2c_read_byte(i2c_t dev, uint16_t addr, void *data, uint8_t flags);
extern int __real_i2c_read_bytes(i2c_t dev, uint16_t addr,
                                 void *data, size_t len, uint8_t flags);
extern int __real_i2c_write_byte(i2c_t dev, uint16_t addr, uint8_t data, uint8_t flags);
extern int __real_i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                                  size_t len, uint8_t flags);

static bus_stats_i2c_t *stats[I2C_NUMOF];
static mutex_t data_mutex[I2C_NUMOF];
static uint32_t transaction_sizes[I2C_NUMOF];

XFA_INIT(bus_stats_i2c_t, bus_stats_i2c_xfa);

static void _transaction_end(i2c_t bus)
{
    bus_stats_t *s = &stats[bus]->stats;
    uint32_t now = ztimer_now(ZTIMER_USEC);
    uint32_t transaction_time_us = now - s->last_access_us;
    s->last_access_us = now;
    bus_stats_histogram_add(s->transaction_size_histogram, transaction_sizes[bus]);
    bus_stats_histogram_add(s->time_us_of_transactions_histogram,
                            transaction_time_us);
    s->time_of_use_us += transaction_time_us;
    stats[bus] = NULL;
}

static bus_stats_t *_get_stats(i2c_t bus, uint16_t addr)
{
    if ((stats[bus] != NULL) && (stats[bus]->endpoint.addr == addr)) {
        return &stats[bus]->stats;
    }

    if (stats[bus] != NULL) {
        /* talking to different slave now, transaction implicitly done */
        _transaction_end(bus);
    }

    for (size_t idx = 0; idx < bus_stats_i2c_numof(); idx++) {
        bus_stats_i2c_t *item = &bus_stats_i2c_xfa[idx];
        if ((item->endpoint.bus == bus) && (item->endpoint.addr == addr)) {
            /* found I2C device to talk to, implicit new transaction */
            stats[bus] = item;
            item->stats.transaction_numof++;
            uint32_t now = ztimer_now(ZTIMER_USEC);
            bus_stats_t *s = &stats[bus]->stats;
            if (s->transaction_numof > 0) {
                uint32_t idle_time = (now - s->last_access_us) / US_PER_MS;
                bus_stats_histogram_add(s->time_ms_between_transactions_histogram,
                                        idle_time);
            }
            s->last_access_us = now;
            transaction_sizes[bus] = 0;
            return &stats[bus]->stats;
        }
    }

    return NULL;
}

void __wrap_i2c_acquire(i2c_t bus)
{
    __real_i2c_acquire(bus);
    mutex_lock(&data_mutex[bus]);
}

void __wrap_i2c_release(i2c_t bus)
{
    if (stats[bus]) {
        _transaction_end(bus);
    }
    mutex_unlock(&data_mutex[bus]);
    __real_i2c_release(bus);
}

void bus_stats_i2c_get(bus_stats_t *dest, size_t idx)
{
    bus_stats_i2c_t *item = &bus_stats_i2c_xfa[idx];
    mutex_lock(&data_mutex[item->endpoint.bus]);
    *dest = item->stats;
    mutex_unlock(&data_mutex[item->endpoint.bus]);
}

void bus_stats_i2c_clear(size_t idx)
{
    bus_stats_i2c_t *item = &bus_stats_i2c_xfa[idx];
    mutex_lock(&data_mutex[item->endpoint.bus]);
    item->stats = (bus_stats_t){ 0 };
    item->stats.last_clear_us = ztimer_now(ZTIMER_USEC);
    mutex_unlock(&data_mutex[item->endpoint.bus]);
}

int __wrap_i2c_read_byte(i2c_t dev, uint16_t addr, void *data, uint8_t flags)
{
    int res = __real_i2c_read_byte(dev, addr, data, flags);
    bus_stats_t *s = _get_stats(dev, addr);
    if (s) {
        s->transaction_size_histogram[0]++;
        transaction_sizes[dev]++;
    }
    return res;
}

int __wrap_i2c_read_bytes(i2c_t dev, uint16_t addr,
                          void *data, size_t len, uint8_t flags)
{
    int res = __real_i2c_read_bytes(dev, addr, data, len, flags);

    bus_stats_t *s = _get_stats(dev, addr);
    if (s && (len > 0)) {
        bus_stats_histogram_add(s->transfer_size_histogram, len);
        transaction_sizes[dev] += len;
    }

    return res;
}

int __wrap_i2c_write_byte(i2c_t dev, uint16_t addr, uint8_t data, uint8_t flags)
{
    int res = __real_i2c_write_byte(dev, addr, data, flags);
    bus_stats_t *s = _get_stats(dev, addr);
    if (s) {
        s->transaction_size_histogram[0]++;
        transaction_sizes[dev]++;
    }
    return res;
}

int __wrap_i2c_write_bytes(i2c_t dev, uint16_t addr,
                           const void *data, size_t len, uint8_t flags)
{
    int res = __real_i2c_write_bytes(dev, addr, data, len, flags);

    bus_stats_t *s = _get_stats(dev, addr);
    if (s && (len > 0)) {
        bus_stats_histogram_add(s->transfer_size_histogram, len);
        transaction_sizes[dev] += len;
    }

    return res;
}
