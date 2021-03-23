/*
 * Copyright (C) 2020, Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_riscv_common
 * @{
 *
 * @file        cpu.c
 * @brief       Common CPU initialization for RISC-V rv32i architecture
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include "cpu.h"
#include "cpu_conf_common.h"
#include "periph_cpu_common.h"

void riscv_fpu_init(void)
{
    /* Enable FPU if present */
    if (read_csr(misa) & (1 << ('F' - 'A'))) {
        write_csr(mstatus, MSTATUS_FS); /* allow FPU instructions without trapping */
        write_csr(fcsr, 0);             /* initialize rounding mode, undefined at reset */
    }
}

void riscv_init(void)
{
    riscv_fpu_init();
    riscv_irq_init();
}
