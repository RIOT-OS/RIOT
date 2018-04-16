/*
 * Copyright (C) 2017 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_hifive
 * @{
 *
 * @file
 * @brief       Support for the SiFive HiFive1 RISC-V board
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
 * routine could be put in SRAM.  The linker script supports
 * code in SRAM using the ".hotcode" section.

 * The flash chip on the HiFive1 is the ISSI 25LP128
 * http://www.issi.com/WW/pdf/25LP128.pdf
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

/*
 * CPU max is 320MHz+ according to datasheet but
 * the relationship between cpu clock and spi clock is determined
 * by SCKDIV.  Given we're trying to achieve maximum I-cache refill
 * for the flash we let MAX_FLASH_FREQ dictate the CPU clock.
 */
#define CPU_DESIRED_FREQ 200000000

/*
 * The relationship between the input clock and SCK is given
 * by the following formula (Fin is processor/tile-link clock):
 *    Fsck = Fin/(2(div + 1))
 * FYI - For 320MHZ it seems to be tolerating a faster SPI clock (56MHz)
 */
#define SCKDIV ((CPU_DESIRED_FREQ - 1) / (MAX_FLASH_FREQ * 2))

/* This should work for any reasonable cpu clock value. */
#define SCKDIV_SAFE 3

/*
 * By default the SPI initialized as:
 * https://github.com/sifive/sifive-blocks/blob/master/src/main/scala/devices/spi/SPIFlash.scala
 * insn.cmd.en := Bool(true)
 * insn.cmd.code := Bits(0x03)
 * insn.cmd.proto := SPIProtocol.Single
 * insn.addr.len := UInt(3)
 * insn.addr.proto := SPIProtocol.Single
 * insn.pad.cnt := UInt(0)
 * insn.pad.code := Bits(0)
 * insn.data.proto := SPIProtocol.Single
 *
 * 25LP128 appears to left in post-reset default state. Boot code
 * does not modify it. We change the SPI configuration here.
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

     /* disable uncrustify *INDENT-OFF* */
    SPI0_REG(SPI_REG_FFMT) =               /* setup "Fast Read Dual I/O" 1-1-2              */
        SPI_INSN_CMD_EN         |          /* Enable memory-mapped flash                    */
        SPI_INSN_ADDR_LEN(3)    |          /* 25LP128 read commands have 3 address bytes    */
        SPI_INSN_PAD_CNT(4)     |          /* 25LP128 Table 6.9 Read Dummy Cycles P4,P3=0,0 */
        SPI_INSN_CMD_PROTO(SPI_PROTO_S) |  /* 25LP128 Table 8.1 "Instruction                */
        SPI_INSN_ADDR_PROTO(SPI_PROTO_D) | /*  Set" shows mode for cmd, addr, and           */
        SPI_INSN_DATA_PROTO(SPI_PROTO_D) | /*  data protocol for given instruction          */
        SPI_INSN_CMD_CODE(0xbb) |          /* Set the instruction to "Fast Read Dual I/O"   */
        SPI_INSN_PAD_CODE(0x00);           /* Dummy cycle sends 0 value bits                */
    /* *INDENT-ON* */

    SPI0_REG(SPI_REG_SCKDIV) = SCKDIV;
}

void board_init(void)
{
    /* Initialize CPU and clocks */
    cpu_init();
    board_init_clock();

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
