/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf54l
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation for the nRF54L family,
 *              based on the SPIM peripheral
 *
 * Compared to the older nRF families, the SPIM peripheral of the nRF54L
 * moved the EasyDMA buffer registers into dedicated DMA register groups and
 * replaced the FREQUENCY register with a prescaler, hence the dedicated
 * driver implementation.
 *
 * Unlike on the nRF52/nRF53/nRF9160, the UARTE/SPIM/TWIM instances sharing
 * one SERIALxx peripheral are not multiplexed at runtime: each SERIALxx
 * instance must be used for a single function in the board configuration.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "compiler_hints.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_cpu.h"

/**
 * @brief   Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

/**
 * @brief   Array with a busy mutex for each SPI device, used to block the
 *          thread until the transfer is done
 */
static mutex_t busy[SPI_NUMOF];

static uint8_t _mbuf[SPI_NUMOF][CONFIG_SPI_MBUF_SIZE];

/* use an enum to count the number of SPI ISR macro names defined by the
 * board */
enum {
#ifdef SPI_0_ISR
    SPI_0_ISR_NUM,
#endif
#ifdef SPI_1_ISR
    SPI_1_ISR_NUM,
#endif
    SPI_ISR_NUMOF,
};

static inline NRF_SPIM_Type *dev(spi_t bus)
{
    return (NRF_SPIM_Type *)spi_config[bus].dev;
}

static inline bool _in_ram(const uint8_t *data)
{
    return ((uint32_t)data & RAM_MASK);
}

static IRQn_Type _irqn(spi_t bus)
{
    if (dev(bus) == NRF_SPIM00) {
        return SERIAL00_IRQn;
    }
    else if (dev(bus) == NRF_SPIM20) {
        return SERIAL20_IRQn;
    }
    else if (dev(bus) == NRF_SPIM21) {
        return SERIAL21_IRQn;
    }
    else if (dev(bus) == NRF_SPIM22) {
        return SERIAL22_IRQn;
    }
    else if (dev(bus) == NRF_SPIM30) {
        return SERIAL30_IRQn;
    }
    assert(false);
    UNREACHABLE();
}

void spi_init(spi_t bus)
{
    /* ensure the ISR names have been defined as needed */
    static_assert(SPI_NUMOF == SPI_ISR_NUMOF, "Define(s) of SPI ISR name(s) missing");

    assert(bus < SPI_NUMOF);

    /* initialize mutex */
    mutex_init_locked(&busy[bus]);
    /* initialize pins */
    spi_init_pins(bus);
}

int spi_init_with_gpio_mode(spi_t bus, const spi_gpio_mode_t* mode)
{
    assert(bus < SPI_NUMOF);

    int ret = 0;

    /* pins that are not connected on a given bus (e.g. MISO on a TX only
     * bus) are configured as GPIO_UNDEF and must be skipped */
    if (gpio_is_valid(spi_config[bus].mosi)) {
        ret += gpio_init(spi_config[bus].mosi, mode->mosi);
    }

    if (gpio_is_valid(spi_config[bus].miso)) {
        ret += gpio_init(spi_config[bus].miso, mode->miso);
    }

    if (gpio_is_valid(spi_config[bus].sclk)) {
        ret += gpio_init(spi_config[bus].sclk, mode->sclk);
    }

    return ret;
}

void spi_init_pins(spi_t bus)
{
    const spi_gpio_mode_t gpio_modes = {
        .mosi = GPIO_OUT,
        .sclk = GPIO_OUT,
        .miso = GPIO_IN,
    };
    spi_init_with_gpio_mode(bus, &gpio_modes);

    /* select pins for the SPI device */
    dev(bus)->PSEL.SCK = spi_config[bus].sclk;
    dev(bus)->PSEL.MOSI = spi_config[bus].mosi;
    dev(bus)->PSEL.MISO = spi_config[bus].miso;
    /* The RIOT SPI API manages the chip select line in software (via the cs
     * argument of spi_acquire()/spi_transfer*()), so the SPIM hardware chip
     * select and the DCX line are intentionally left unconnected. */
    dev(bus)->PSEL.CSN = 0xffffffff;
    dev(bus)->PSEL.DCX = 0xffffffff;

    NVIC_EnableIRQ(_irqn(bus));
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);

    mutex_lock(&locks[bus]);

    /* configure bus, the clk value holds the prescaler divisor */
    dev(bus)->CONFIG = mode;
    dev(bus)->PRESCALER = clk;
    /* enable the bus */
    dev(bus)->ENABLE = SPIM_ENABLE_ENABLE_Enabled;
}

void spi_release(spi_t bus)
{
    /* power off everything */
    dev(bus)->ENABLE = 0;

    mutex_unlock(&locks[bus]);
}

static size_t _transfer(spi_t bus, const uint8_t *out_buf, uint8_t *in_buf,
                        size_t remaining_len)
{
    size_t transfer_len = remaining_len > UINT16_MAX ? UINT16_MAX : remaining_len;
    const uint8_t *out_mbuf = out_buf;

    /* Copy the out buffer in case it resides in the RRAM, EasyDMA only works
     * from RAM */
    if (out_buf && !_in_ram(out_buf)) {
        transfer_len = transfer_len > CONFIG_SPI_MBUF_SIZE
                     ? CONFIG_SPI_MBUF_SIZE : transfer_len;
        memcpy(_mbuf[bus], out_buf, transfer_len);
        out_mbuf = _mbuf[bus];
    }

    dev(bus)->DMA.TX.PTR = (uint32_t)out_mbuf;
    dev(bus)->DMA.RX.PTR = (uint32_t)in_buf;

    dev(bus)->DMA.TX.MAXCNT = (out_buf) ? transfer_len : 0;
    dev(bus)->DMA.RX.MAXCNT = (in_buf) ? transfer_len : 0;

    /* clear any spurious END events */
    dev(bus)->EVENTS_END = 0;
    dev(bus)->TASKS_START = 1;
    return transfer_len;
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    /* enable IRQ */
    dev(bus)->INTENSET = SPIM_INTENSET_END_Msk;

    do {
        size_t transfer_len = _transfer(bus, out_buf, in_buf, len);
        /* block until the irq releases the mutex, then lock it again for the
         * next transfer */
        mutex_lock(&busy[bus]);
        out_buf += out_buf ? transfer_len : 0;
        in_buf += in_buf ? transfer_len : 0;
        len -= transfer_len;
    } while (len);

    /* disable IRQ */
    dev(bus)->INTENCLR = SPIM_INTENCLR_END_Msk;

    if ((cs != SPI_CS_UNDEF) && (!cont)) {
        gpio_set((gpio_t)cs);
    }
}

static void irq_handler(spi_t bus)
{
    dev(bus)->EVENTS_END = 0;
    mutex_unlock(&busy[bus]);

    cortexm_isr_end();
}

#ifdef SPI_0_ISR
void SPI_0_ISR(void)
{
    irq_handler(SPI_DEV(0));
}
#endif

#ifdef SPI_1_ISR
void SPI_1_ISR(void)
{
    irq_handler(SPI_DEV(1));
}
#endif
