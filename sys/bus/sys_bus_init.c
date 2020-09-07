/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_bus
 * @{
 *
 * @file
 * @brief       System Buses
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "sys/bus.h"

msg_bus_t _sys_bus[SYS_BUS_NUMOF];

void auto_init_sys_bus(void)
{
    for (unsigned i = 0; i < SYS_BUS_NUMOF; ++i) {
        msg_bus_init(&_sys_bus[i]);
    }
}
