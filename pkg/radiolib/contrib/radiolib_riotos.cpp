/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Baptiste Le Duc <baptiste.leduc@etik.com>
 */

#include "radiolib_riotos.h"
#include "ztimer.h"

static void _irq_handler(void *arg)
{
    ((void (*)(void))arg)();
}

RiotHal::RiotHal(spi_t bus, spi_clk_t clk)
    : RadioLibHal(GPIO_IN, GPIO_OUT, 0, 1, GPIO_RISING, GPIO_FALLING),
      _bus(bus), _clk(clk) {}

void RiotHal::pinMode(uint32_t pin, uint32_t mode)
{
    if (pin == RADIOLIB_NC) {
        return;
    }
    gpio_init((gpio_t)pin, (gpio_mode_t)mode);
}

void RiotHal::digitalWrite(uint32_t pin, uint32_t value)
{
    if (pin == RADIOLIB_NC) {
        return;
    }
    gpio_write((gpio_t)pin, (bool)value);
}

uint32_t RiotHal::digitalRead(uint32_t pin)
{
    if (pin == RADIOLIB_NC) {
        return 0;
    }
    return gpio_read((gpio_t)pin);
}

void RiotHal::attachInterrupt(uint32_t interruptNum, void (*interruptCb)(void), uint32_t mode)
{
    if (interruptNum == RADIOLIB_NC) {
        return;
    }
    gpio_init_int((gpio_t)interruptNum, GPIO_IN, (gpio_flank_t)mode,
                  _irq_handler, (void *)interruptCb);
}

void RiotHal::detachInterrupt(uint32_t interruptNum)
{
    if (interruptNum == RADIOLIB_NC) {
        return;
    }
    gpio_irq_disable((gpio_t)interruptNum);
}

void RiotHal::delay(RadioLibTime_t ms)
{
    ztimer_sleep(ZTIMER_MSEC, ms);
}

void RiotHal::delayMicroseconds(RadioLibTime_t us)
{
    ztimer_sleep(ZTIMER_USEC, us);
}

RadioLibTime_t RiotHal::millis(void)
{
    return ztimer_now(ZTIMER_MSEC);
}

RadioLibTime_t RiotHal::micros(void)
{
    return ztimer_now(ZTIMER_USEC);
}

long RiotHal::pulseIn(uint32_t pin, uint32_t state, RadioLibTime_t timeout)
{
    (void)pin;
    (void)state;
    (void)timeout;
    return 0;
}

void RiotHal::spiBegin(void) {}

void RiotHal::spiBeginTransaction(void)
{
    spi_acquire(_bus, SPI_CS_UNDEF, SPI_MODE_0, _clk);
}

void RiotHal::spiTransfer(uint8_t *out, size_t len, uint8_t *in)
{
    spi_transfer_bytes(_bus, SPI_CS_UNDEF, false, out, in, len);
}

void RiotHal::spiEndTransaction(void)
{
    spi_release(_bus);
}

void RiotHal::spiEnd(void) {}

/** @} */
