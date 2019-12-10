/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the clock initialization for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"

#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "vendor/prci_driver.h"

void clock_init(void)
{
    /* In case we are executing from QSPI, (which is quite likely) we need to
     * set the QSPI clock divider appropriately before boosting the clock
     * frequency. PRCI_set_hfrosctrim_for_f_cpu() tries multiple clocks
     * so choose a safe value that should work for all frequencies.
     */
    SPI0_REG(SPI_REG_SCKDIV) = SCKDIV_SAFE;

    /* Note: The range is limited to ~100MHz and depends on PLL settings */
    PRCI_set_hfrosctrim_for_f_cpu(CPU_DESIRED_FREQ, PRCI_FREQ_UNDERSHOOT);

    /* begin{code-style-ignore} */
    SPI0_REG(SPI_REG_FFMT) =               /* setup "Fast Read Dual I/O"                             */
        SPI_INSN_CMD_EN         |          /* Enable memory-mapped flash                    */
        SPI_INSN_ADDR_LEN(3)    |          /* 25LP03D read commands have 3 address bytes    */
        SPI_INSN_PAD_CNT(4)     |          /* 25LP03D Table 6.11 Read Dummy Cycles = 4      */
        SPI_INSN_CMD_PROTO(SPI_PROTO_S) |  /* 25LP03D Table 8.1 "Instruction                */
        SPI_INSN_ADDR_PROTO(SPI_PROTO_D) | /*  Set" shows mode for cmd, addr, and           */
        SPI_INSN_DATA_PROTO(SPI_PROTO_D) | /*  data protocol for given instruction          */
        SPI_INSN_CMD_CODE(0xBB) |          /* Set the instruction to "Fast Read Dual I/O"   */
        SPI_INSN_PAD_CODE(0x00);           /* Dummy cycle sends 0 value bits                */
    /* end{code-style-ignore} */

    SPI0_REG(SPI_REG_SCKDIV) = SCKDIV;
}
