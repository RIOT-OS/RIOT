/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#ifdef MODULE_PMP_NOEXEC_RAM
#include "pmp.h"
#endif

#ifdef MODULE_PUF_SRAM
#include "puf_sram.h"

extern unsigned _sheap;

void riscv_puf_sram_init(void)
{
    puf_sram_init((uint8_t *)&_sheap, SEED_RAM_LEN);
}
#endif /* MODULE_PUF_SRAM */

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
#ifdef MODULE_PMP_NOEXEC_RAM
    /* This marks the (main) RAM region as non
     * executable. Using PMP entry 0.
     */
    write_pmpaddr(0, make_napot(CPU_RAM_BASE, CPU_RAM_SIZE));

    /* Lock & select NAPOT, only allow write and read */
    set_pmpcfg(0, PMP_L | PMP_NAPOT | PMP_W | PMP_R);
#endif
}
