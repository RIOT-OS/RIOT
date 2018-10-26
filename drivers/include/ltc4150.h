/*
 * Copyright 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ltc4150 LTC4150 coulomb counter
 * @ingroup     drivers_sensors
 * @brief       Driver for LTC4150 coulomb counter
 * @{
 *
 * @file
 * @brief       LTC4150 coulomb counter
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * # Wiring the LTC4150
 * Hint: M Grusin thankfully created an
 * [open hardware breakout board](https://cdn.sparkfun.com/datasheets/BreakoutBoards/LTC4150_BOB_v10.pdf).
 * As a result, virtually all LTC4150 breakout boards are using this schematic.
 * Whenever this documentation refers to a breakout board, this open hardware
 * board is meant. Of course, this driver works with the "bare" LTC4150 as well.
 *
 * Please note that this driver works interrupt driven and does not clear the
 * signal. Thus, the /CLR and /INT pins on the LTC4150 need to be connected
 * (in case of the breakout board: close solder jumper SJ1), so that the signal
 * is automatically cleared.
 *
 * Hint: The breakout board uses external pull up resistors on /INT, POL and
 * /SHDN. Therefore /SHDN can be left unconnected and no internal pull ups are
 * required for /INT and POL. In case your board uses 3.3V logic the solder
 * jumpers SJ2 and SJ3 have to be closed, in case of 5V they have to remain
 * open. Connect the VIO pin to the logic level, GND to ground, IN+ and IN- to
 * the power supply and use OUT+ and OUT- to power your board.
 *
 * In the easiest case only the /INT pin needs to be connected to a GPIO,
 * and (in case of external pull ups) /SHDN and POL can be left unconnected.
 * The GPIO /INT is connected to support for interrupts, /SHDN and POL
 * (if connected) do not require interrupt support.
 *
 * In case a battery is used, the module `ltc4150_bidirectional` should also be
 * added and the POL pin connected to another GPIO. This allows to distinguish
 * between charge drawn from the battery and charge transferred into the battery
 * (used to load it).
 *
 * In case support to power off the LTC4150 is desired, the /SHDN pin needs to
 * be connected to a third GPIO.
 *
 * # Things to keep in mind
 * The LTC4150 creates pulses with a frequency depending on the current drawn.
 * Thus, more interrupts need to be handled when more current is drawn, which
 * in turn increases system load (and power consumption). The interrupt service
 * routing is quite short and even when used outside of specification less than
 * 20 ticks per second will occur. Hence, this effect should hopefully be
 * negligible.
 */

#ifndef LTC4150_H
#define LTC4150_H

#include <mutex.h>
#include <periph/gpio.h>
#include <ringbuffer.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration flags of the LTC4150 coulomb counter
 */
enum {
    /**
     * @brief External pull on the /INT pin is present
     */
    LTC4150_INT_EXT_PULL_UP = 0x01,
    /**
     * @brief External pull on the /POL pin is present
     */
    LTC4150_POL_EXT_PULL_UP = 0x02,
    /**
     * @brief External pull on the /INT *and* the /POL pin is present
     */
    LTC4150_EXT_PULL_UP = LTC4150_INT_EXT_PULL_UP | LTC4150_POL_EXT_PULL_UP,
};

/**
 * @brief Parameters required to set up the LTC4150 coulomb counter
 */
typedef struct {
    /**
     * @brief Pin going LOW every time a specific charge is drawn, labeled INT
     */
    gpio_t interrupt;
    /**
     * @brief Pin indicating (dis-)charging, labeled POL
     *
     * Set this pin to `GPIO_UNDEF` to tread every pulse as discharging. This
     * pin is pulled low by the LTC4150 in case the battery is discharging.
     */
    gpio_t polarity;
    /**
     * @brief Pin to power off the LTC4150 coulomb counter, labeled SHDN
     *
     * Set this pin to `GPIO_UNDEF` if the SHDN pin is not connected to the MCU
     */
    gpio_t shutdown;
    /**
     * @brief Pulse per ampere hour of charge
     *
     * pulses = 3600 * 32.55 * R
     *
     * Where R is the resistance (in Ohm) between the SENSE+ and SENSE- pins.
     * E.g. the MSBA2 has 0.390 Ohm (==> 45700 pulses), while most breakout
     * boards for the LTC4150 have 0.050 Ohm (==> 5859 pulses).
     */
    uint16_t pulses_per_ah;
    /**
     * @brief Configuration flags controlling if inter pull ups are required
     *
     * Most [breakout boards](https://cdn.sparkfun.com/datasheets/BreakoutBoards/LTC4150_BOB_v10.pdf)
     * and the MSBA2 board use external pull up resistors, so no internal pull
     * ups are required. Clear the flags to use internal pull ups instead.
     */
    uint16_t flags;
} ltc4150_params_t;

/**
 * @brief Data structure storing the number of pulses for (dis-)charging
 *
 * In addition to the total number of pulses the accumulated pulses are stored
 * for:
 * - Every second in the last minute
 * - The last minute
 */
typedef struct {
    uint32_t total;         /**< Total number of pulses */
    uint8_t ringbuf[6];     /**< Ring buffer to allow O(1) update of the `last_min` */
    uint16_t last_min;      /**< Charge transferred in the last minute */
    uint8_t counter;        /**< Temporary counter used as source for the ring buffer */
} ltc4150_data_t;

/**
 * @brief LTC4150 coulomb counter
 */
typedef struct {
    ltc4150_params_t params;    /**< Parameter of the LTC4150 coulomb counter */
    uint32_t start_sec;         /**< Time stamp when started counting */
    uint32_t last_update_sec;   /**< Time stamp of last pulse */
#ifdef MODULE_LTC4150_BIDIRECTIONAL
    ltc4150_data_t charged;     /**< # of pulses for charging (POL=high) */
#endif
    ltc4150_data_t discharged;  /**< # of pulses for discharging (POL=low) */
    uint8_t position;           /**< Offset of the first element in the ring buf */
} ltc4150_dev_t;

/**
 * @brief             Initialize the LTC4150 driver
 *
 * @param  dev        Device to initialize
 * @param  params     Information on how the LTC4150 is conntected
 *
 * @retval  0         Success
 * @retval -EINVAL    Called with invalid argument(s)
 * @retval -EIO       IO failure (`gpio_init()`/`gpio_init_int()` failed)
 */
int ltc4150_init(ltc4150_dev_t *dev, const ltc4150_params_t *params);

/**
 * @brief             Disable the interrupt handler and turn the chip off
 *
 * @param  dev        Previously initialized device to power off
 *
 * @retval  0         Success
 * @retval -EINVAL    Called with invalid argument(s)
 *
 * The driver can be reinitialized to power on the LTC4150 chip again
 */
int ltc4150_shutdown(ltc4150_dev_t *dev);

/**
 * @brief                  Get the measured charge since boot in millicoulomb
 *
 * @param dev              The LTC4150 device to read data from
 * @param[out] charged     The charge transferred in charging direction
 * @param[out] discharged  The charge transferred in discharging direction
 *
 * @retval 0                Success
 * @retval -EINVAL          Called with an invalid argument
 *
 * Passing `NULL` for `charged` or `discharged` is allowed, if only one
 * information is of interest.
 */
int ltc4150_total_charge(ltc4150_dev_t *dev,
                         int32_t *charged, int32_t *discharged);

/**
 * @brief                  Get the accumulated charge of the last minute in
 *                         millicoulomb
 *
 * @param dev              The LTC4150 device to read data from
 * @param[out] charged     The charge transferred in charging direction
 * @param[out] discharged  The charge transferred in discharging direction
 *
 * @retval 0               Success
 * @retval -EINVAL         Called with an invalid argument
 *
 * Passing `NULL` for `charged` or `discharged` is allowed, if only one
 * information is of interest. Divide the result by 60 to get the average
 * current in milliampere.
 */
int ltc4150_last_minute_charge(ltc4150_dev_t *dev, int32_t *charged,
                               int32_t *discharged);

/**
 * @brief             Get the average current drawn in E-01 milliampere
 *
 * @param dev         The LTC4150 device to read data from
 * @param[out] dest   The result is stored here
 *
 * @retval 0          Success
 * @retval -EINVAL    Called with an invalid argument
 */
int ltc4150_avg_current(ltc4150_dev_t *dev, int16_t *dest);
#ifdef __cplusplus
}
#endif

#endif /* LTC4150_H */
/** @} */
