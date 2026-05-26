/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes LTDC peripheral display device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stddef.h>

#include "log.h"

#include "disp_dev.h"

#include "periph_cpu.h"

static disp_dev_reg_t _disp_dev_reg;
static disp_dev_t _disp_dev;

extern const disp_dev_driver_t stm32_ltdc_disp_dev_driver;

void auto_init_periph_ltdc(void)
{
    LOG_DEBUG("[auto_init_screen] initializing LTDC periph\n");
    ltdc_init();

    _disp_dev_reg.dev = &_disp_dev;
    _disp_dev_reg.screen_id = 0;
    _disp_dev_reg.dev->driver = &stm32_ltdc_disp_dev_driver;

    /* add to disp_dev registry */
    disp_dev_reg_add(&_disp_dev_reg);
}
