/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_mpfs
 * @{
 *
 * @file
 * @brief           Polarfire CPU initialization
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */
#include "stdio_uart.h"
#include "periph/init.h"
#include "irq_arch.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

extern void __libc_init_array(void);

void cpu_init(void)
{
    /* Common RISC-V initialization */
    riscv_init();

    mss_config_clk_rst(MSS_PERIPH_CFM, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);

    /* Initialize all GPIOs banks */
    mss_config_clk_rst(MSS_PERIPH_GPIO0, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);
    mss_config_clk_rst(MSS_PERIPH_GPIO1, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);
    mss_config_clk_rst(MSS_PERIPH_GPIO2, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);

    SYSREG->GPIO_INTERRUPT_FAB_CR = 0xFFFFFFFFUL;

    stdio_init();
    periph_init();
}
