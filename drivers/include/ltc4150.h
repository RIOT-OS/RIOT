/*
 * Copyright 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ltc4150 LTC4150 coulomb counter
 * @ingroup     drivers_sensors
 * @brief       Driver for the Linear Tech LTC4150 Coulomb Counter
 *              (a.k.a. battery gauge sensor or power consumption sensor)
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
 * In case a battery is used the POL pin connected to another GPIO. This allows
 * to distinguish between charge drawn from the battery and charge transferred
 * into the battery (used to load it).
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
 *
 * @{
 *
 * @file
 * @brief       LTC4150 coulomb counter
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef LTC4150_H
#define LTC4150_H

#include <stdint.h>

#include "mutex.h"
#include "periph/gpio.h"

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
 * @brief Enumeration of directions in which the charge can be transferred
 */
typedef enum {
    LTC4150_CHARGE,             /**< The battery is charged */
    LTC4150_DISCHARGE,          /**< Charge is drawn from the battery */
} ltc4150_dir_t;

/**
 * @brief LTC4150 coulomb counter
 */
typedef struct ltc4150_dev ltc4150_dev_t;

/**
 * @brief Interface to allow recording of the drawn current in a user defined
 *        resolution
 *
 * @note  Keep in mind that the data recording may be performed by the CPU of
 *        the system to monitor - thus keep power consumption for the recording
 *        low!
 *
 * The LTC4150 driver will only track total charge transferred (separately for
 * charging in discharging direction). However, there are use cases that
 * required more precise data recording, e.g. a rolling average of the last
 * minute. This interface allows application developers to implement the ideal
 * trade-off between RAM, ROM and runtime overhead for the data recording and
 * the level of information they require.
 */
typedef struct {
    /**
     * @brief   Function to call on every pulse received from the LTC4150
     * @warning This function is called in interrupt context
     *
     * @param[in] dev       The device the pulse was received from
     * @param[in] dir       Direction in which the charge is transferred
     * @param[in] now_usec  The system time the pulse was received in µs
     * @param[in] arg       (Optional) argument for this callback
     */
    void (*pulse)(ltc4150_dev_t *dev, ltc4150_dir_t dir, uint64_t now_usec, void *arg);
    /**
     * @brief   Function to call upon driver initialization or reset
     *
     * @see ltc4150_init
     * @see ltc4150_reset_counters
     *
     * @param[in] dev       The LTC4150 device to monitor
     * @param[in] now_usec  The system time the pulse was received in µs
     * @param[in] arg       (Optional) argument for this callback
     */
    void (*reset)(ltc4150_dev_t *dev, uint64_t now_usec, void *arg);
} ltc4150_recorder_t;

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
     * Set this pin to `GPIO_UNDEF` to treat every pulse as discharging. This
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
    /**
     * @brief `NULL` or a `NULL`-terminated array of data recorders
     * @pre   If not `NULL`, the last element of the array must be `NULL`
     */
    const ltc4150_recorder_t **recorders;
    /**
     * @brief `NULL` or an array of the user defined data for each recorder
     * @pre   If @see ltc4150_params_t::recorders is not `NULL`, this must point
     *        to an array of `void`-Pointers of the same length.
     * @note  Unlike @see ltc4150_param_t::callback, this array does not need to
     *        be `NULL`-terminated
     */
    void **recorder_data;
} ltc4150_params_t;

/**
 * @brief LTC4150 coulomb counter
 */
struct ltc4150_dev {
    ltc4150_params_t params;    /**< Parameter of the LTC4150 coulomb counter */
    uint32_t start_sec;         /**< Time stamp when started counting */
    uint32_t last_update_sec;   /**< Time stamp of last pulse */
    uint32_t charged;           /**< # of pulses for charging (POL=high) */
    uint32_t discharged;        /**< # of pulses for discharging (POL=low) */
};

/**
 * @brief Data structure used by @ref ltc4150_last_minute
 */
typedef struct {
    uint32_t last_rotate_sec;   /**< Time stamp of the last ring "rotation" */
    /**
     * @brief Pulses in charging direction recorded in the last minute
     */
    uint16_t charged;
    /**
     * @brief Pulses in discharging direction recorded in the last minute
     */
    uint16_t discharged;
    /**
     * @brief Ring-buffer to store charge information in 10 sec resolution
     */
    uint8_t buf_charged[7];
    /**
     * @brief As above, but in discharging direction
     */
    uint8_t buf_discharged[7];
    uint8_t ring_pos;           /**< Position in the ring buffer */
} ltc4150_last_minute_data_t;

/**
 * @brief Records the charge transferred within the last minute using
 */
extern const ltc4150_recorder_t ltc4150_last_minute;

/**
 * @brief             Initialize the LTC4150 driver
 *
 * @param  dev        Device to initialize
 * @param  params     Information on how the LTC4150 is connected
 *
 * @retval  0         Success
 * @retval -EINVAL    Called with invalid argument(s)
 * @retval -EIO       IO failure (`gpio_init()`/`gpio_init_int()` failed)
 */
int ltc4150_init(ltc4150_dev_t *dev, const ltc4150_params_t *params);

/**
 * @brief             Clear current counters of the given LTC4150 device
 * @param dev         The LTC4150 device to clear current counters from
 *
 * @retval 0          Success
 * @retval -EINVAL    Called with an invalid argument
 */
int ltc4150_reset_counters(ltc4150_dev_t *dev);

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
 * @brief                  Get the measured charge since boot or last reset in
 *                         millicoulomb
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
int ltc4150_charge(ltc4150_dev_t *dev, uint32_t *charged, uint32_t *discharged);

/**
 * @brief             Get the average current drawn in E-01 milliampere
 *
 * This will return the average current drawn since boot or last reset until the
 * last pulse from the LTC4150 was received. The value might thus be a bit
 * outdated (0.8 seconds for the breakout board and a current of 100mA, 79
 * seconds for a current of 1mA).
 *
 * @param dev         The LTC4150 device to read data from
 * @param[out] dest   Store the average current drawn in E-01 milliampere here
 *
 * @retval 0          Success
 * @retval -EINVAL    Called with an invalid argument
 * @retval -EAGAIN    Called before enough data samples have been acquired.
 *                    (Wait for at least one second or one pulse from the
                      LTC4150, whichever takes longer.)
 */
int ltc4150_avg_current(ltc4150_dev_t *dev, int16_t *dest);

/**
 * @brief                  Get the measured charge in the last minute
 *
 * @param dev              The LTC4150 device to read data from
 * @param data             The data recorded by @ref ltc4150_last_minute
 * @param[out] charged     The charge transferred in charging direction
 * @param[out] discharged  The charge transferred in discharging direction
 *
 * @retval 0                Success
 * @retval -EINVAL          Called with an invalid argument
 *
 * @warning The returned data may be outdated up to ten seconds
 *
 * Passing `NULL` for `charged` or `discharged` is allowed, if only one
 * information is of interest.
 */
int ltc4150_last_minute_charge(ltc4150_dev_t *dev,
                               ltc4150_last_minute_data_t *data,
                               uint32_t *charged, uint32_t *discharged);

/**
 * @brief Convert the raw data (# pulses) acquired by the LTC4150 device to
 *        charge information in millicoulomb
 * @note  This function will make writing data recorders (see
 *        @ref ltc4150_recorder_t) easier, but is not intended for end users
 *
 * @param dev                 LTC4150 device the data was received from
 * @param[out] charged        Charge in charging direction is stored here
 * @param[out] discharged     Charge in discharging direction is stored here
 * @param[in] raw_charged     Number of pulses in charging direction
 * @param[in] raw_discharged  Number of pulses in discharging direction
 */
void ltc4150_pulses2c(const ltc4150_dev_t *dev,
                      uint32_t *charged, uint32_t *discharged,
                      uint32_t raw_charged,
                      uint32_t raw_discharged);
#ifdef __cplusplus
}
#endif

#endif /* LTC4150_H */
/** @} */
