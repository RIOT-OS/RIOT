/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Ryan Kurte <ryankurte@gmail.com>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @author      Christian Amsüss <c@amsuess.com>
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "mutex.h"

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_usart.h"

/* DIV_UP is division which rounds up instead of down */
#define SPI_DIV_UP(a, b)    (((a) + ((b) - 1)) / (b))

static mutex_t spi_lock[SPI_NUMOF];

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* initialize lock */
    mutex_init(&spi_lock[bus]);

    /* initialize pins */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    /* configure the pins */
    gpio_init(spi_config[bus].clk_pin, GPIO_OUT);
    gpio_init(spi_config[bus].mosi_pin, GPIO_OUT);
    gpio_init(spi_config[bus].miso_pin, GPIO_IN_PD);
}

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;

    /* bound divider from 2 to 65536 */
    if (freq > (CLOCK_CORECLOCK - 1) / 2) {
        freq = (CLOCK_CORECLOCK - 1) / 2;
    }
    assert(freq >= SPI_DIV_UP(CLOCK_CORECLOCK, 65536));

    /* =================================================================
     * from USART_BaudrateSyncSet() of the gecko SDK
     */
    uint32_t ref_freq, clkdiv;

    /* Prevent dividing by 0. */
    EFM_ASSERT(freq);

    /*
     * CLKDIV in synchronous mode is given by:
     *
     * CLKDIV = 256 * (fHFPERCLK/(2 * br) - 1)
     */

    /* HFPERCLK/HFPERBCLK used to clock all USART/UART peripheral modules. */
#if defined(_SILICON_LABS_32B_SERIES_2)
    ref_freq = CMU_ClockFreqGet(cmuClock_PCLK);
#else
#if defined(_CMU_HFPERPRESCB_MASK)
    if (dev == USART2) {
        ref_freq = CMU_ClockFreqGet(cmuClock_HFPERB);
    }
    else {
        ref_freq = CMU_ClockFreqGet(cmuClock_HFPER);
    }
#else
    ref_freq = CMU_ClockFreqGet(cmuClock_HFPER);
#endif
#endif

    clkdiv = (ref_freq - 1) / (2 * freq);
    clkdiv = clkdiv << 8;

    /* Verify that resulting clock divider is within limits. */
    EFM_ASSERT(!(clkdiv & ~_USART_CLKDIV_MASK));

    /* =================================================================
     * 
     * The manual say that `the clock division factor have a 15-bit
     * integral part and a 5-bit fractional part` but only the integral
     * part was used in the gecko SDK.
     */

    /* br = fHFPERCLK/(2 x (1 + USARTn_CLKDIV / 256)) */
    return SPI_DIV_UP(ref_freq, (2 * (1 + (clkdiv >> 8))));
}

uint32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    return clk;
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);

    mutex_lock(&spi_lock[bus]);

    /* power on spi bus */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(spi_config[bus].cmu, true);

    USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

    init.baudrate = (uint32_t)clk;
    init.clockMode = (USART_ClockMode_TypeDef)mode;
    init.msbf = true;

    USART_InitSync(spi_config[bus].dev, &init);

    /* configure pin functions */
#if defined(_SILICON_LABS_32B_SERIES_0)
    spi_config[bus].dev->ROUTE = (spi_config[bus].loc |
                                  USART_ROUTE_RXPEN |
                                  USART_ROUTE_TXPEN |
                                  USART_ROUTE_CLKPEN);
#elif defined(_SILICON_LABS_32B_SERIES_1)
    spi_config[bus].dev->ROUTELOC0 = spi_config[bus].loc;
    spi_config[bus].dev->ROUTEPEN = (USART_ROUTEPEN_RXPEN |
                                     USART_ROUTEPEN_TXPEN |
                                     USART_ROUTEPEN_CLKPEN);
#endif
}

void spi_release(spi_t bus)
{
    /* power off spi bus */
    CMU_ClockEnable(spi_config[bus].cmu, false);

    mutex_unlock(&spi_lock[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    uint8_t *out_buf = (uint8_t *)out;
    uint8_t *in_buf = (uint8_t *)in;

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t ret = USART_SpiTransfer(spi_config[bus].dev,
                                        out != NULL ? out_buf[i] : 0);

        if (in != NULL) {
            in_buf[i] = ret;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}
