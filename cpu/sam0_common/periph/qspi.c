/*
 * Copyright (c) 2020 ML!PA Consulting GmbH
 * Copyright (c) 2019 Ha Thach and Dean Miller for Adafruit Industries LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @{
 *
 * @file        qspi.c
 * @brief       Low-level QSPI peripheral implementation
 *              based on Adafruit_QSPI.cpp
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "periph/qspi.h"
#include "macros/units.h"
#include "mutex.h"

/* QSPI peripheral pins are fixed */
#ifdef CPU_COMMON_SAMD5X
#define QSPI_PIN_CLK    GPIO_PIN(PB, 10)
#define QSPI_PIN_CS     GPIO_PIN(PB, 11)
#define QSPI_PIN_DATA_0 GPIO_PIN(PA,  8)
#define QSPI_PIN_DATA_1 GPIO_PIN(PA,  9)
#define QSPI_PIN_DATA_2 GPIO_PIN(PA, 10)
#define QSPI_PIN_DATA_3 GPIO_PIN(PA, 11)
#define QSPI_MUX        GPIO_MUX_H
#endif

static mutex_t _qspi_mutex = MUTEX_INIT;
static uint32_t _qspi_addrlen = QSPI_INSTRFRAME_ADDRLEN_24BITS;

/**
 * @brief Run a single QSPI instruction.
 *
 * @param command  instruction code
 * @param iframe   iframe register value (configured by caller according to command
 *                 code)
 * @param addr     The address to read or write from. If the instruction doesn't
 *                 require an address, this parameter is meaningless.
 * @param buffer   pointer to the data to be written or stored depending on the type
 *                 is Read or Write
 * @param size     the number of bytes to read or write.
 */
static void _run_instruction(uint8_t command, uint32_t iframe, uint32_t addr,
                             uint8_t *buffer, uint32_t size)
{
    QSPI->INSTRCTRL.bit.INSTR = command;
    QSPI->INSTRFRAME.reg = iframe;

    /* Dummy read of INSTRFRAME needed to synchronize.
     * See Instruction Transmission Flow Diagram, figure 37.9, page 995
     * and Example 4, page 998, section 37.6.8.5.
     */
    QSPI->INSTRFRAME.reg;

    if (buffer && size) {
        void *qspi_mem = (uint8_t *)(QSPI_AHB + addr);
        const uint32_t tfr_type = iframe & QSPI_INSTRFRAME_TFRTYPE_Msk;

        if ((tfr_type == QSPI_INSTRFRAME_TFRTYPE_READ) ||
            (tfr_type == QSPI_INSTRFRAME_TFRTYPE_READMEMORY)) {
            memcpy(buffer, qspi_mem, size);
        } else {
            memcpy(qspi_mem, buffer, size);
        }
    }

    QSPI->CTRLA.reg = QSPI_CTRLA_ENABLE | QSPI_CTRLA_LASTXFER;

    while (!QSPI->INTFLAG.bit.INSTREND) {
        thread_yield();
    }

    QSPI->INTFLAG.reg = QSPI_INTFLAG_INSTREND;
}

ssize_t qspi_cmd_read(qspi_t bus, uint8_t command, void *response, size_t len)
{
    (void)bus;

    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI
                    | _qspi_addrlen
                    | QSPI_INSTRFRAME_TFRTYPE_READ
                    | QSPI_INSTRFRAME_INSTREN
                    | (response != NULL ? QSPI_INSTRFRAME_DATAEN : 0);

    samd0_cache_disable();
    _run_instruction(command, iframe, 0, response, len);
    samd0_cache_enable();

    return len;
}

ssize_t qspi_cmd_write(qspi_t bus, uint8_t command, const void *data, size_t len)
{
    (void)bus;

    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI
                    | _qspi_addrlen
                    | QSPI_INSTRFRAME_TFRTYPE_WRITE
                    | QSPI_INSTRFRAME_INSTREN
                    | (data != NULL ? QSPI_INSTRFRAME_DATAEN : 0);

    samd0_cache_disable();
    _run_instruction(command, iframe, 0, (void *)data, len);
    samd0_cache_enable();

    return len;
}

ssize_t qspi_read(qspi_t bus, uint32_t addr, void *data, size_t len)
{
    (void)bus;

    /* Command 0x6B 1 line address, 4 line Data
     * Quad output mode, read memory type
     */
    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT
                    | _qspi_addrlen
                    | QSPI_INSTRFRAME_TFRTYPE_READMEMORY
                    | QSPI_INSTRFRAME_INSTREN | QSPI_INSTRFRAME_ADDREN
                    | QSPI_INSTRFRAME_DATAEN  | QSPI_INSTRFRAME_DUMMYLEN(8);

    samd0_cache_disable();
    _run_instruction(SFLASH_CMD_QUAD_READ, iframe, addr, data, len);
    samd0_cache_enable();

    return len;
}

void qspi_erase(qspi_t bus, uint32_t address, qspi_erase_size_t size)
{
    qspi_cmd_write(bus, SFLASH_CMD_WRITE_ENABLE, NULL, 0);

    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI
                    | _qspi_addrlen
                    | QSPI_INSTRFRAME_TFRTYPE_WRITE
                    | QSPI_INSTRFRAME_INSTREN | QSPI_INSTRFRAME_ADDREN;

    QSPI->INSTRADDR.reg = address;
    _run_instruction(size, iframe, address, NULL, 0);
}

ssize_t qspi_write(qspi_t bus, uint32_t addr, const void *data, size_t len)
{
    qspi_cmd_write(bus, SFLASH_CMD_WRITE_ENABLE, NULL, 0);

    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT
                    | _qspi_addrlen
                    | QSPI_INSTRFRAME_TFRTYPE_WRITEMEMORY
                    | QSPI_INSTRFRAME_INSTREN | QSPI_INSTRFRAME_ADDREN
                    | QSPI_INSTRFRAME_DATAEN;

    samd0_cache_disable();
    _run_instruction(SFLASH_CMD_QUAD_PAGE_PROGRAM, iframe, addr, (void *)data, len);
    samd0_cache_enable();

    return len;
}

void qspi_acquire(qspi_t bus)
{
    (void)bus;

    mutex_lock(&_qspi_mutex);

    QSPI->CTRLA.bit.ENABLE = 1;
}

void qspi_release(qspi_t bus)
{
    (void)bus;

    mutex_unlock(&_qspi_mutex);

    QSPI->CTRLA.bit.ENABLE = 0;
}

void qspi_init(qspi_t bus)
{
    (void)bus;

    /* enable QSPI clock */
    MCLK->AHBMASK.reg  |= MCLK_AHBMASK_QSPI;
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_QSPI;

    /* reset the peripheral */
    QSPI->CTRLA.bit.SWRST = 1;

    /* configure QSPI pins */
    gpio_init_mux(QSPI_PIN_CLK, QSPI_MUX);
    gpio_init_mux(QSPI_PIN_CS,  QSPI_MUX);
    gpio_init_mux(QSPI_PIN_DATA_0, QSPI_MUX);
    gpio_init_mux(QSPI_PIN_DATA_1, QSPI_MUX);
    gpio_init_mux(QSPI_PIN_DATA_2, QSPI_MUX);
    gpio_init_mux(QSPI_PIN_DATA_3, QSPI_MUX);

    /* start with low 4 MHz, Mode 0 */
    QSPI->BAUD.bit.BAUD = CLOCK_CORECLOCK / MHZ(4);
    QSPI->CTRLB.reg = QSPI_CTRLB_MODE_MEMORY   | QSPI_CTRLB_CSMODE_NORELOAD
                    | QSPI_CTRLB_DATALEN_8BITS | QSPI_CTRLB_CSMODE_LASTXFER;
}

void qspi_configure(qspi_t bus, qspi_mode_t mode, uint32_t flags, uint32_t clk_hz)
{
    (void)bus;

    assert(clk_hz <= CLOCK_CORECLOCK);
    /* TODO: support other modes */
    assert(QSPI_FLAG_BIT(flags) == QSPI_FLAG_4BIT);

    QSPI->BAUD.reg = QSPI_BAUD_BAUD((CLOCK_CORECLOCK + clk_hz - 1) / clk_hz)
                   | mode;

    /* make sure to keep the memory peripheral in sync */
    /* set addr len needs write enable                 */
    qspi_cmd_write(bus, SFLASH_CMD_WRITE_ENABLE, NULL, 0);

    if (flags & QSPI_FLAG_ADDR_32BIT) {
        qspi_cmd_write(bus, SFLASH_CMD_4_BYTE_ADDR, NULL, 0);
        _qspi_addrlen = QSPI_INSTRFRAME_ADDRLEN_32BITS;
    } else {
        qspi_cmd_write(bus, SFLASH_CMD_3_BYTE_ADDR, NULL, 0);
        _qspi_addrlen = QSPI_INSTRFRAME_ADDRLEN_24BITS;
    }
}
