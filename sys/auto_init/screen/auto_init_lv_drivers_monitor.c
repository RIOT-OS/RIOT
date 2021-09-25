/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes monitor display device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stddef.h>

#include "log.h"

#include "disp_dev.h"

#include "display/monitor.h"
#include "display/monitor_disp_dev.h"

static disp_dev_reg_t disp_dev_entry;
static monitor_t _monitor;

void auto_init_monitor(void)
{
    LOG_DEBUG("[auto_init_screen] initializing SDL monitor\n");

    disp_dev_entry.dev = (disp_dev_t *)&_monitor;
    disp_dev_entry.screen_id = MONITOR_PARAM_SCREEN_ID;
    disp_dev_entry.dev->driver = &monitor_disp_dev_driver;

    /* add to disp_dev registry */
    disp_dev_reg_add(&(disp_dev_entry));
}
