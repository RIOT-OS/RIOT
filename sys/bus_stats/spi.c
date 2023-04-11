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
 * @brief   Bus statistic collection for SPI
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
#include "periph/spi.h"
#include "periph_conf.h"
#include "time_units.h"
#include "xfa.h"
#include "ztimer.h"

/* the common spi_transfer_reg() and spi_transfer_regs() implementation calls
 * spi_transfer_bytes() and will indirectly be monitored */
#ifndef PERIPH_SPI_NEEDS_TRANSFER_REG
#warning "SPI driver doesn't use common spi_transfer_reg() implementation, these calls will not be monitored"
#endif

#ifndef PERIPH_SPI_NEEDS_TRANSFER_REGS
#warning "SPI driver doesn't use common spi_transfer_regs() implementation, these calls will not be monitored"
#endif

extern void __real_spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode,
                               spi_clk_t clk);
extern void __real_spi_release(spi_t bus);
extern void __real_spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                                      const void *out, void *in, size_t len);
extern uint8_t __real_spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont,
                                        uint8_t out);

static bus_stats_t *stats[SPI_NUMOF];
static uint16_t transaction_sizes[SPI_NUMOF];
static mutex_t data_mutex[SPI_NUMOF];

XFA_INIT(bus_stats_spi_t, bus_stats_spi_xfa);

void __wrap_spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    __real_spi_acquire(bus, cs, mode, clk);
    for (size_t idx = 0; idx < bus_stats_spi_numof(); idx++) {
        bus_stats_spi_t *item = &bus_stats_spi_xfa[idx];
        if ((item->endpoint.bus == bus) && (item->endpoint.cs == cs)) {
            mutex_lock(&data_mutex[bus]);
            stats[bus] = &item->stats;
            transaction_sizes[bus] = 0;
            uint32_t now = ztimer_now(ZTIMER_USEC);
            if (stats[bus]->transaction_numof > 0) {
                uint32_t idle_ms = (now - stats[bus]->last_access_us) / US_PER_MS;
                bus_stats_histogram_add(stats[bus]->time_ms_between_transactions_histogram,
                                        idle_ms);
            }
            stats[bus]->last_access_us = now;
            return;
        }
    }
}

void __wrap_spi_release(spi_t bus)
{
    if (stats[bus]) {
        uint32_t now = ztimer_now(ZTIMER_USEC);
        uint32_t transaction_time_us = now - stats[bus]->last_access_us;
        stats[bus]->time_of_use_us += transaction_time_us;
        stats[bus]->transaction_numof++;
        bus_stats_histogram_add(stats[bus]->transaction_size_histogram, transaction_sizes[bus]);
        bus_stats_histogram_add(stats[bus]->time_us_of_transactions_histogram,
                                transaction_time_us);
        stats[bus]->last_access_us = now;
        stats[bus] = NULL;
        mutex_unlock(&data_mutex[bus]);
    }
    __real_spi_release(bus);
}

void bus_stats_spi_get(bus_stats_t *dest, size_t idx)
{
    bus_stats_spi_t *item = &bus_stats_spi_xfa[idx];
    mutex_lock(&data_mutex[item->endpoint.bus]);
    *dest = item->stats;
    mutex_unlock(&data_mutex[item->endpoint.bus]);
}

void bus_stats_spi_clear(size_t idx)
{
    bus_stats_spi_t *item = &bus_stats_spi_xfa[idx];
    mutex_lock(&data_mutex[item->endpoint.bus]);
    item->stats = (bus_stats_t){ 0 };
    item->stats.last_clear_us = ztimer_now(ZTIMER_USEC);
    mutex_unlock(&data_mutex[item->endpoint.bus]);
}

void __wrap_spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                               const void *out, void *in, size_t len)
{
    __real_spi_transfer_bytes(bus, cs, cont, out, in, len);
    if (stats[bus]) {
        bus_stats_histogram_add(stats[bus]->transfer_size_histogram, len);
        transaction_sizes[bus] += len;
    }
}

uint8_t __wrap_spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont,
                                 uint8_t out)
{
#ifdef PERIPH_SPI_NEEDS_TRANSFER_BYTE
    uint8_t in;
    __wrap_spi_transfer_bytes(bus, cs, cont, &out, &in, 1);
    return in;
#else
    __real_spi_transfer_byte(bus, cs, cont, out);
    if (stats[bus]) {
        stats[bus]->transfer_size_histogram[0]++;
        transaction_sizes[bus]++;
    }
#endif
}
