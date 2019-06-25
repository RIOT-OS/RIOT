/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_hifive1b
 * @{
 *
 * @file
 * @brief       Support for the SiFive HiFive1b RISC-V board
 *
 * @author      Ken Rabold, JP Bonn
 *
 * @}
 */

#include <stdio.h>
#include <errno.h>

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "vendor/prci_driver.h"

/*
 * Configure the memory mapped flash for faster throughput
 * to minimize interrupt latency on an I-Cache miss and refill
 * from flash.  Alternatively (and faster) the interrupt
 * routine could be put in SRAM.

 * The flash chip on the HiFive1b is the ISSI 25LP03D
 * http://www.issi.com/WW/pdf/25LP-WP032D.pdf
 * The maximum frequency it can run at is 115MHz in
 * "Fast Read Dual I/O" mode.
 * #define MAX_FLASH_FREQ 115000000
 *
 * FYI - Like the FE310-G000, the G002 has problems with reading flash
 * faster than 50MHz
 */
#define MAX_FLASH_FREQ 50000000

/*
 * CPU max is 320MHz+ according to datasheet but
 * the relationship between cpu clock and spi clock is determined
 * by SCKDIV.  Given we're trying to achieve maximum I-cache refill
 * for the flash we let MAX_FLASH_FREQ dictate the CPU clock.
 */
#define CPU_DESIRED_FREQ 320000000

/*
 * The relationship between the input clock and SCK is given
 * by the following formula (Fin is processor/tile-link clock):
 *    Fsck = Fin/(2(div + 1))
 */
#define SCKDIV ((CPU_DESIRED_FREQ - 1) / (MAX_FLASH_FREQ * 2))

/* This should work for any reasonable cpu clock value. */
#define SCKDIV_SAFE 3

/*
 * By default the SPI FFMT initialized as:
 *  cmd_en = 1
 *  addr_len = 3
 *  cmd_code = 3
 *  all other fields = 0
 */

void board_init_clock(void)
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

__attribute__ ((section (".ramfunc")))
void board_init_flash(void)
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

void board_init(void)
{
    /* Initialize CPU and clocks */
    cpu_init();
    board_init_clock();
  //  board_init_flash();

    /* Configure pin muxing for UART0 */
    GPIO_REG(GPIO_OUTPUT_VAL) |= IOF0_UART0_MASK;
    GPIO_REG(GPIO_OUTPUT_EN) |= IOF0_UART0_MASK;
    GPIO_REG(GPIO_IOF_SEL) &= ~IOF0_UART0_MASK;
    GPIO_REG(GPIO_IOF_EN) |= IOF0_UART0_MASK;

    /* Configure GPIOs for LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    /* Turn all the LEDs off */
    LED0_OFF;
    LED1_OFF;
    LED2_OFF;

    /* Initialize newlib-nano library stubs */
    nanostubs_init();
}
