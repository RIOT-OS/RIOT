/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Functions to communicate with the CC1100/CC1101 transceiver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef CC110X_COMMUNICATION_H
#define CC110X_COMMUNICATION_H

#include "periph/gpio.h"
#include "periph/spi.h"
#include "cc110x.h"
#include "cc110x_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Acquire the SPI interface of the transceiver
 *
 * @pre     When first acquiring the device either after boot or after having put
 *          the device to sleep mode, use @ref cc110x_power_on_and_acquire
 *          instead. Subsequently, this function should be used (it is faster).
 */
static inline void cc110x_acquire(cc110x_t *dev)
{
    spi_acquire(dev->params.spi, dev->params.cs, SPI_MODE_0, dev->params.spi_clk);
}

/**
 * @brief   Release the SPI interface of the transceiver
 */
static inline void cc110x_release(cc110x_t *dev)
{
    spi_release(dev->params.spi);
}

/**
 * @brief   Read a single configuration/status register from the transceiver
 *
 * @param   dev     Device descriptor of the transceiver to read the register
 *                  from
 * @param   addr    Address of the register to read
 * @param   dest    Where to store the received register content
 *
 * @return  The received status byte
 *
 * @pre     @p dest points to one byte of writeable memory
 * @warning Race condition: SPI access to status registers can occur while
 *          their content is changed, resulting in corrupted data being
 *          retrieved. @ref cc110x_read_reliable provides reliable access
 *          to status registers and should be used to read the TXBYTES,
 *          RXBYTES, MARCSTATE, LQI, RSSI, WORTIME1 and WORTIME0 status
 *          registers. (See Silicon Errata from 2015 at pages 4ff.)
 *          (In IDLE state LQI and RSSI can be read safely using this
 *          function.)
 * @warning The received status byte is occasionally corrupted. (See
 *          Silicon Errata from 2015 at pages 4ff.) Use @ref cc110x_status
 *          To get the status byte in a reliable way.
 */
uint8_t cc110x_read(cc110x_t *dev, uint8_t addr, uint8_t *dest);

/**
 * @brief   Read a single status register from the transceiver reliable
 *
 * This function has more overhead than @ref cc110x_read, but it is the only
 * reliable way to access frequently updated status registers.
 *
 * @param   dev     Device descriptor of the transceiver to read the register
 *                  from
 * @param   addr    Address of the register to read
 * @param   dest    Where to store the received register content
 *
 * @return  The received status byte
 *
 * @pre     @p dest points to one byte of writeable memory
 * @warning The received status byte is occasionally corrupted. (See
 *          Silicon Errata from 2015 at pages 4ff.) Use @ref cc110x_status
 *          To get the status byte in a reliable way.
 */
uint8_t cc110x_read_reliable(cc110x_t *dev, uint8_t addr, uint8_t *dest);

/**
 * @brief   Write to a single configuration register on the transceiver
 *
 * @param   dev     Device descriptor of the transceiver to write byte to
 * @param   addr    Address of the register to write to
 * @param   data    Data to write
 *
 * @return  The received status byte
 *
 * @pre     @p addr <= 0x2e (@ref CC110X_REG_TEST0)
 * @warning Writing to status registers is impossible (==> precondition)
 * @warning The received status byte is occasionally corrupted. (See
 *          Silicon Errata from 2015 at pages 4ff.) Use @ref cc110x_status
 *          To get the status byte in a reliable way.
 */
uint8_t cc110x_write(cc110x_t *dev, uint8_t addr, uint8_t data);

/**
 * @brief   Burst-read a bunch of configuration registers from the transceiver
 *
 * @param   dev     Device descriptor of the transceiver to read from
 * @param   addr    Address to start reading from
 * @param   dest    Destination buffer to store the received data to
 * @param   len     Number of bytes to read starting from @p addr
 *
 * @return  The received status byte
 *
 * @pre     @p dest points to a pre-allocated buffer of >= @p len bytes
 * @pre     @p addr + @p len <= 0x2e (@ref CC110X_REG_TEST0)
 * @warning Burst read access from status registers is impossible
 *          (==> second precondition)
 * @warning The received status byte is occasionally corrupted. (See
 *          Silicon Errata from 2015 at pages 4ff.) Use @ref cc110x_status
 *          To get the status byte in a reliable way.
 */
uint8_t cc110x_burst_read(cc110x_t *dev, uint8_t addr, void *dest, size_t len);

/**
 * @brief   Burst-write to a bunch of configuration registers on the transceiver
 *
 * @param   dev     Device descriptor of the transceiver to write
 * @param   addr    Address to start writing to
 * @param   src     Buffer holding the configuration to write
 * @param   len     Number of registers to write to
 *
 * @return  The received status byte
 *
 * @pre     @p src points to @p len bytes of readable memory
 * @pre     @p addr + @p len <= 0x2e (@ref CC110X_REG_TEST0)
 * @warning Writes to status registers is impossible
 *          (==> second precondition)
 * @warning The received status byte is occasionally corrupted. (See
 *          Silicon Errata from 2015 at pages 4ff.) Use @ref cc110x_status
 *          to get the status byte in a reliable way.
 */
uint8_t cc110x_burst_write(cc110x_t *dev, uint8_t addr,
                           const void *src, size_t len);

/**
 * @brief   Send a command to the transceiver
 *
 * @param   dev     Device descriptor of the transceiver to send the command to
 * @param   cmd     Command to send
 *
 * @return  The received status byte
 *
 * @warning The received status byte is occasionally corrupted. (See
 *          Silicon Errata from 2015 at pages 4ff.) Use
 *          @ref cc110x_status to get the status byte in a reliable
 *          way.
 */
uint8_t cc110x_cmd(cc110x_t *dev, uint8_t cmd);

/**
 * @brief   Get the transceivers status byte in a reliable way
 *
 * @param   dev     Device descriptor of the transceiver to get the status from
 *
 * @return  The received status byte
 */
uint8_t cc110x_status(cc110x_t *dev);

/**
 * @brief   Wakes up the transceiver from "Sleep" or "Crystal oscillator off"
 *          state and waits until the crystal has stabilized
 *
 * Thus function clears the CS pin, which triggers a transition from the "Sleep"
 * or "Crystal oscillator off" states (see Figure 13 on page 28 in the data
 * sheet).
 *
 * If the crystal was off (only in above mentioned states), the MCU must wait
 * for the transceiver to become ready *before* any SPI transfer is initiated.
 * In all other states, CS pin can be pulled low and SPI transfer can start
 * right away (see section 10 on page 29 in the data sheet). This driver will
 * never disable the transceivers crystal, so this function has to be called
 * only once when the transceiver is powered on.
 *
 * The transceiver will signal that it is available by pulling the MISO pin
 * low (section 10 on page 29 in the data sheet), which does not take longer
 * than 150 microseconds (see Table 22 on page 30 in the data sheet). Instead
 * of messing with the SPI interface, this driver simply waits for this upper
 * bound, as suggested in the note below Table 22 on page 30 in the data sheet.
 *
 * @pre     The device was not acquired and in low power mode
 * @post    The device is in IDLE mode and acquired
 *
 * @retval  0       Success
 * @retval  -EIO    Couldn't pull the CS pin down (@ref cc110x_params_t::cs)
 */
int cc110x_power_on_and_acquire(cc110x_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* CC110X_COMMUNICATION_H */
/** @} */
