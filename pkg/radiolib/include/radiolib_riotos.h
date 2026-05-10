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

#pragma once

#include "Hal.h"
#include "periph/gpio.h"
#include "periph/spi.h"

/*!
  \class RiotHal
  \brief RIOT-OS hardware abstraction layer for RadioLib.
*/
class RiotHal : public RadioLibHal {
public:
    /*!
      \brief Constructor.
      \param bus  RIOT SPI bus (e.g. SPI_DEV(2)).
      \param clk  SPI clock speed (e.g. SPI_CLK_5MHZ).
    */
    RiotHal(spi_t bus, spi_clk_t clk);

    void pinMode(uint32_t pin, uint32_t mode) override;
    void digitalWrite(uint32_t pin, uint32_t value) override;
    uint32_t digitalRead(uint32_t pin) override;
    void attachInterrupt(uint32_t interruptNum, void (*interruptCb)(void), uint32_t mode) override;
    void detachInterrupt(uint32_t interruptNum) override;
    void delay(RadioLibTime_t ms) override;
    void delayMicroseconds(RadioLibTime_t us) override;
    RadioLibTime_t millis() override;
    RadioLibTime_t micros() override;
    long pulseIn(uint32_t pin, uint32_t state, RadioLibTime_t timeout) override;
    void spiBegin() override;
    void spiBeginTransaction() override;
    void spiTransfer(uint8_t* out, size_t len, uint8_t* in) override;
    void spiEndTransaction() override;
    void spiEnd() override;

private:
    spi_t _bus;
    spi_clk_t _clk;
};

/** @} */
