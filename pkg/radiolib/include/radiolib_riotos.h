/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @{
 *
 * @file
 * @author  Baptiste Le Duc <baptiste.leduc@etik.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "Hal.h"

/**
  \class RiotHal
  \brief RIOT-OS hardware abstraction layer for RadioLib.
*/
class RiotHal : public RadioLibHal {
public:
    /**
      \brief Constructor.
      \param bus  RIOT SPI bus (e.g. SPI_DEV(2)).
      \param clk  SPI clock speed (e.g. SPI_CLK_5MHZ).
    */
    RiotHal(spi_t bus, spi_clk_t clk);

    /**
      \brief Set pin mode (input/output).
      \param pin  GPIO pin number.
      \param mode Pin mode (RadioLib constant).
    */
    void pinMode(uint32_t pin, uint32_t mode) override;

    /**
      \brief Write digital value to pin.
      \param pin   GPIO pin number.
      \param value Value to write (0 or 1).
    */
    void digitalWrite(uint32_t pin, uint32_t value) override;

    /**
      \brief Read digital value from pin.
      \param pin  GPIO pin number.
      \returns    Current pin level (0 or 1).
    */
    uint32_t digitalRead(uint32_t pin) override;

    /**
      \brief Attach interrupt handler to pin.
      \param interruptNum  GPIO pin number.
      \param interruptCb   Callback invoked on interrupt.
      \param mode          Trigger mode (RadioLib constant).
    */
    void attachInterrupt(uint32_t interruptNum, void (*interruptCb)(void), uint32_t mode) override;

    /**
      \brief Detach interrupt handler from pin.
      \param interruptNum  GPIO pin number.
    */
    void detachInterrupt(uint32_t interruptNum) override;

    /**
      \brief Block for given number of milliseconds.
      \param ms  Duration in milliseconds.
    */
    void delay(RadioLibTime_t ms) override;

    /**
      \brief Block for given number of microseconds.
      \param us  Duration in microseconds.
    */
    void delayMicroseconds(RadioLibTime_t us) override;

    /**
      \brief Return elapsed time in milliseconds.
      \returns  Milliseconds since boot.
    */
    RadioLibTime_t millis() override;

    /**
      \brief Return elapsed time in microseconds.
      \returns  Microseconds since boot.
    */
    RadioLibTime_t micros() override;

    /**
      \brief Measure pulse length on pin (not supported, returns 0).
      \param pin      GPIO pin number.
      \param state    Pulse level to measure.
      \param timeout  Maximum wait time in microseconds.
      \returns        Pulse duration in microseconds, or 0 if unsupported.
    */
    long pulseIn(uint32_t pin, uint32_t state, RadioLibTime_t timeout) override;

    /**
      \brief Initialize SPI bus (no-op; bus initialized at board level).
    */
    void spiBegin() override;

    /**
      \brief Acquire SPI bus for a transaction.
    */
    void spiBeginTransaction() override;

    /**
      \brief Transfer bytes over SPI.
      \param out  TX buffer (may be NULL).
      \param len  Number of bytes to transfer.
      \param in   RX buffer (may be NULL).
    */
    void spiTransfer(uint8_t* out, size_t len, uint8_t* in) override;

    /**
      \brief Release SPI bus after a transaction.
    */
    void spiEndTransaction() override;

    /**
      \brief De-initialize SPI bus (no-op).
    */
    void spiEnd() override;

private:
    spi_t _bus;
    spi_clk_t _clk;
};

#endif /* __cplusplus */

/** @} */
