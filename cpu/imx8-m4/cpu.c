/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_imx8-m4
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for IMX8-M4
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"
#include "stdio_base.h"
#include "clk_conf.h"

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{


    /* initialize the Cortex-M core */
    cortexm_init();

    /* Default clocks setup is already done by U-Boot initialization */
    /* TODO REWORK PARAMS TO USE DEFINE OR ENUM */
    ccm_set_root_clk(CCM_ROOT_CLK_CM4, 1, 1, 3);
    ccm_set_root_clk(CCM_ROOT_CLK_AHB, 1, 1, 1);

    /* Enable RDC clock peripheral */
    ccm_set_ccgr(CCM_CCGR_RDC, CLK_ENABLE_ALL_DOMAINS);
    ccm_en_root_clk(CCM_ROOT_CLK_RDC, 1);

    /* Set CM4F as master of RDC */
    RDC->MDA[1] = 0;
    
    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
}
