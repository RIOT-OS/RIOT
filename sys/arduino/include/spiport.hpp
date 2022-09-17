/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_arduino_api
 * @{
 *
 * @file
 * @brief       Definition of the Arduino 'SPI' interface
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef SPIPORT_H
#define SPIPORT_H

#include "arduino_board.h"
#include "byteorder.h"
#include "periph/spi.h"
#include "rmutex.h"

/**
 * @name    Arduino compatible SPI modes
 * @{
 */
#define SPI_MODE0       (0)     /**< CPOL=0, CPHA=0 */
#define SPI_MODE1       (1)     /**< CPOL=0, CPHA=1 */
#define SPI_MODE2       (2)     /**< CPOL=1, CPHA=0 */
#define SPI_MODE3       (3)     /**< CPOL=1, CPHA=1 */
/** @} */

/**
 * @name    Arduino compatible SPI frequency selection via clock divider
 *
 * This API assumes the library was targeting a 16 MHz Arduino. It will choose
 * the SPI clock frequency matching the requested frequency best, but never
 * a frequency greater than the one it would have on Arduinos.
 * @{
 */
#define SPI_CLOCK_DIV2      (0) /**< Best match for 8 MHz: 5 MHz */
#define SPI_CLOCK_DIV4      (1) /**< Best match for 4 MHz: 1 MHz */
#define SPI_CLOCK_DIV8      (1) /**< Best match for 2 MHz: 1 MHz */
#define SPI_CLOCK_DIV16     (1) /**< Best match for 1 MHz: 1 MHz */
#define SPI_CLOCK_DIV32     (2) /**< Best match for 500 kHz: 400 kHz */
#define SPI_CLOCK_DIV64     (3) /**< Best match for 250 kHZ: 100 kHz */
#define SPI_CLOCK_DIV128    (3) /**< Best match for 125 kHz: 100 kHz */
/** @} */

/**
 * @name    Arduino compatible bit order values for SPI
 * @{
 */
#define MSBFIRST        (1)     /**< most significant bit first */
/** @} */

/**
 * @brief   Arduino SPI configuration interface
 */
class SPISettings {
private:
    spi_mode_t mode;
    spi_clk_t clock;

public:
    /**
     * @brief   Create a new SPI settings instance
     *
     * @param   clock_hz    SPI clock in Hz to use
     * @param   bitOrder    Has to be `MSBFIST`, for compatibility only
     * @param   dataMode    SPI mode to use
     *
     * The RIOT SPI clock frequency best matching @p clock will be chosen,
     * but the frequency will never by greater than what is given in @p clock,
     * unless @p clock_hz is is lower than 100kHz, which is the lowest clock
     * frequency that will be used.
     */
    SPISettings(uint32_t clock_hz, uint8_t bitOrder, uint8_t dataMode);

    /**
     * @brief   Create a new SPI settings instance with default settings
     */
    SPISettings() : SPISettings(4000000, MSBFIRST, SPI_MODE0) { }

    friend class SPIClass;
};

/**
 * @brief   Arduino SPI interface
 *
 * @warning Wrap all SPI transfers in `SPI.beginTransaction()` and `SPI.endTransaction()`
 *
 * The official Arduino SPI-API allows to use SPI transfers without having to
 * call `SPI.beginTransaction()`, but discourages to do so. The RIOT API does
 * not provide this feature, instead a call to `SPI.beginTransaction()` is
 * mandatory. However, most Arduino code already does this and the remaining
 * code should be fixed anyway.
 */
class SPIClass {
private:
    spi_t spi_dev;
    SPISettings settings;
    bool is_transaction;
    rmutex_t mut;

public:
    /**
     * @brief   Create a new SPI interface instance
     * @param   spi_dev     The RIOT SPI device to use under the hood
     */
    explicit SPIClass(spi_t spi_dev);

    /**
     * @brief   Create a new SPI interface instance for SPI device 0
     * @param   uc_pinMISO  Ignored, for compatibility only
     * @param   uc_pinSCK   Ignored, for compatibility only
     * @param   uc_pinMOSI  Ignored, for compatibility only
     * @param   uc_pinSS    Ignored, for compatibility only
     * @param   uc_mux      Ignored, for compatibility only
     */
    SPIClass(uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI,
             uint8_t uc_pinSS, uint8_t uc_mux) : SPIClass(SPI_DEV(0))
    {
        (void)uc_pinMISO;
        (void)uc_pinSCK;
        (void)uc_pinMOSI;
        (void)uc_pinSS;
        (void)uc_mux;
    }

    /**
     * @brief   Transfer a single byte of data
     * @param[in]   data        Byte to send
     * @return  The received byte
     */
    uint8_t transfer(uint8_t data)
    {
        transfer(&data, sizeof(data));
        return data;
    }

    /**
     * @brief   Transfer two bytes of data
     * @param[in]       data        The two bytes to send
     * @return  The received two bytes
     *
     * Arduino is sending the most significant byte first, if the SPI interface
     * is configured to send the most significant bit first. If the least
     * significant bit is send first, Arduino will also send the least
     * significant byte first.
     *
     * This wrapper currently only supports sending the most significant bit
     * first over the wire, so this function will also always send the most
     * significant byte first.
     */
    uint16_t transfer16(uint16_t data)
    {
        data = htons(data);
        transfer(&data, sizeof(data));
        return ntohs(data);
    }

    /**
     * @brief   Transfer data
     * @param[inout]    buf     Buffer containing the data to send, received
     *                          data will be written here
     * @param[in]       count   Number of bytes to send
     */
    void transfer(void *buf, size_t count);

    /**
     * @brief   Doesn't do anything, for compatibility only
     */
    void begin() { }

    /**
     * @brief   Doesn't do anything, for compatibility only
     */
    void end() { }

    /**
     * @brief   Acquires the SPI interface and applies the given settings
     * @param[in]   settings    Settings to apply
     */
    void beginTransaction(SPISettings settings);

    /**
     * @brief   Releases the SPI interface
     */
    void endTransaction();

    /**
     * @brief   Sets the bit order to the given value
     *
     * @details Currently only most significant bit first is supported, as
     *          practically no hardware exists using lsbfirst. An assertion
     *          is triggered if lsbfirst is requested.
     * @deprecated  This function is deprecated in the official Arduino API,
     *              so it is a good idea to not use it. In RIOT, this function
     *              is not yet scheduled for removal to allow using Arduino
     *              libraries using it.
     */
    void setBitOrder(uint8_t order);

    /**
     * @brief   Sets the SPI mode (clock phase and polarity)
     *
     * @deprecated  This function is deprecated in the official Arduino API,
     *              so it is a good idea to not use it. In RIOT, this function
     *              is not yet scheduled for removal to allow using Arduino
     *              libraries using it.
     */
    void setDataMode(uint8_t mode);

    /**
     * @brief   Sets the SPI clock in an archaic manner
     *
     * @deprecated  This function is deprecated in the official Arduino API,
     *              so it is a good idea to not use it. In RIOT, this function
     *              is not yet scheduled for removal to allow using Arduino
     *              libraries using it.
     */
    void setClockDivider(uint8_t divider);
};

/**
 * @brief: Instance of the SPI interface as required by the Arduino API
 */
extern SPIClass SPI;

#endif /* SPIPORT_H */
/** @} */
