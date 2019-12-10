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
 * @brief       Implementation of the CPU initialization for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include "cpu.h"
#include "periph/init.h"

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks, peripheral
 */
void cpu_init(void)
{
    /* Initialize IRQs */
    irq_init();

    /* Initialize clock */
    clock_init();

    /* Initialize newlib-nano library stubs */
    nanostubs_init();

    /* Initialize static peripheral */
    periph_init();
}

/*
 * By default the SPI FFMT initialized as:
 *  cmd_en = 1
 *  addr_len = 3
 *  cmd_code = 3
 *  all other fields = 0
 */

__attribute__ ((section (".ramfunc")))
void init_flash(void)
{
    /* Update the QSPI interface to adjust to the CPU speed
     * This function needs to execute from the RAM
     * when the QSPI interface is being reconfigured because the flash
     * can't be accessed during this time
     */

    /* Disable SPI flash mode */
    SPI0_REG(SPI_REG_FCTRL) &= ~SPI_FCTRL_EN;

    /* Enable QPI mode by sending command to flash */
    SPI0_REG(SPI_REG_TXFIFO) = 0x35;

    /* begin{code-style-ignore} */
    SPI0_REG(SPI_REG_FFMT) =               /* setup "Fast Read Quad I/O (QPI mode)"         */
        SPI_INSN_CMD_EN         |          /* Enable memory-mapped flash                    */
        SPI_INSN_ADDR_LEN(3)    |          /* 25LP03D read commands have 3 address bytes    */
        SPI_INSN_PAD_CNT(6)     |          /* 25LP03D Table 6.11 Read Dummy Cycles = 6      */
        SPI_INSN_CMD_PROTO(SPI_PROTO_Q) |  /* 25LP03D Table 8.1 "Instruction                */
        SPI_INSN_ADDR_PROTO(SPI_PROTO_Q) | /*  Set" shows mode for cmd, addr, and           */
        SPI_INSN_DATA_PROTO(SPI_PROTO_Q) | /*  data protocol for given instruction          */
        SPI_INSN_CMD_CODE(0xEB) |          /* Set the instruction to "Fast Read Quad I/O"   */
        SPI_INSN_PAD_CODE(0x00);           /* Dummy cycle sends 0 value bits                */
    /* end{code-style-ignore} */

    /* Re-enable SPI flash mode */
    SPI0_REG(SPI_REG_FCTRL) |= SPI_FCTRL_EN;

    /* Adjust the SPI clk divider for to boost flash speed */
  //  SPI0_REG(SPI_REG_SCKDIV) = SCKDIV;
}
