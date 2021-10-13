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
#include "periph_conf.h"

#include "vendor/riscv_csr.h"

#include "stdio_uart.h"

/*
 * Configure the memory mapped flash for faster throughput
 * to minimize interrupt latency on an I-Cache miss and refill
 * from flash.  Alternatively (and faster) the interrupt
 * routine could be put in SRAM.  The linker script supports
 * code in SRAM using the ".hotcode" section.
 * The flash chip on the HiFive1 is the ISSI 25LP128
 * http://www.issi.com/WW/pdf/IS25LP128.pdf
 * The maximum frequency it can run at is 133MHz in
 * "Fast Read Dual I/O" mode.
 * Note the updated data sheet:
 * https://static.dev.sifive.com/SiFive-FE310-G000-datasheet-v1.0.4.pdf
 * states "Address and write data using DQ[3] for transmission will not
 * function properly."  This rules out QPI for the XIP memory mapped flash.
 * #define MAX_FLASH_FREQ 133000000
 * On forum SiFive says "safe" operation would be 40MHz.  50MHz seems to work
 * fine.
 */
#define MAX_FLASH_FREQ 50000000

/* This should work for any reasonable cpu clock value. */
#define SCKDIV_SAFE 3

/*
 * By default the SPI FFMT initialized as:
 *  cmd_en = 1
 *  addr_len = 3
 *  cmd_code = 3
 *  all other fields = 0
 */
void flash_init(void)
{
    /* In case we are executing from QSPI, (which is quite likely) we need to
     * set the QSPI clock divider appropriately before boosting the clock
     * frequency.
     */
    SPI0_REG(SPI_REG_SCKDIV) = SCKDIV_SAFE;

    /* begin{code-style-ignore} */
    SPI0_REG(SPI_REG_FFMT) =               /* setup "Fast Read Dual I/O" 1-1-2              */
        SPI_INSN_CMD_EN         |          /* Enable memory-mapped flash                    */
        SPI_INSN_ADDR_LEN(3)    |          /* 25LP128 read commands have 3 address bytes    */
        SPI_INSN_PAD_CNT(4)     |          /* 25LP128 Table 6.9 Read Dummy Cycles P4,P3=0,0 */
        SPI_INSN_CMD_PROTO(SPI_PROTO_S) |  /* 25LP128 Table 8.1 "Instruction                */
        SPI_INSN_ADDR_PROTO(SPI_PROTO_D) | /*  Set" shows mode for cmd, addr, and           */
        SPI_INSN_DATA_PROTO(SPI_PROTO_D) | /*  data protocol for given instruction          */
        SPI_INSN_CMD_CODE(0xbb) |          /* Set the instruction to "Fast Read Dual I/O"   */
        SPI_INSN_PAD_CODE(0x00);           /* Dummy cycle sends 0 value bits                */
    /* end{code-style-ignore} */

    /*
     * The relationship between the input clock and SCK is given
     * by the following formula (Fin is processor/tile-link clock):
     *    Fsck = Fin/(2(div + 1))
     */
    uint32_t freq = cpu_freq();
    uint32_t sckdiv = (freq - 1) / (MAX_FLASH_FREQ * 2);

    if (sckdiv > SCKDIV_SAFE) {
        SPI0_REG(SPI_REG_SCKDIV) = sckdiv;
    }
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks, peripheral
 */
void cpu_init(void)
{
    /* Initialize clock */
    fe310_clock_init();

#if USE_CLOCK_HFROSC_PLL
    /* Initialize flash memory, only when using the PLL: in this
       case the CPU core clock can be configured to be so fast that the SPI
       flash frequency needs to be adjusted accordingly. */
    flash_init();
#endif

    /* Common RISC-V initialization */
    riscv_init();

    /* Initialize stdio */
    stdio_init();

    /* Initialize static peripheral */
    periph_init();
}
