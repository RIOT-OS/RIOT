/*
 * SPDX-FileCopyrightText: 2026 Jakob Müller <ja.mueller@tuhh.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    drivers_max31343  MAX31343 I2C RTC driver
 * @ingroup     drivers_sensors
 * @brief       Driver for the MAX31343 I2C real-time clock with integrated MEMS oscillator
 * @{
 *
 * @file
 * @brief       Driver interface for the MAX31343 I2C real-time clock
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 */

#pragma once

#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "periph/i2c.h"
#ifndef USE_INTERNAL_RTC
#include "periph/rtc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device descriptor for the MAX31343 RTC.
 *
 * Holds the runtime state of a MAX31343 device instance.
 * Currently this only contains the I2C bus, but may be extended
 * in the future.
 */
typedef struct {
    i2c_t i2c;     /**< I2C bus */
} max31343_t;

/**
 * @brief Square-wave output frequency selection.
 *
 * Selects the output frequency of the SQW pin.
 * The exact mapping is defined by the MAX31343 datasheet.
 */
typedef enum {
    MAX31343_SQW_1HZ = 0,
    MAX31343_SQW_2HZ = 1,
    MAX31343_SQW_4HZ = 2,
    MAX31343_SQW_8HZ = 3,
    MAX31343_SQW_16HZ = 4,
    MAX31343_SQW_32HZ = 5,
} max31343_sqw_freq_t;

/**
 * @brief Automatic temperature conversion interval.
 *
 * Defines the interval for automatic temperature measurements
 * when temperature AUTOMODE is enabled.
 *
 * The values correspond to the TTSINT field (TS_Config[5:3]).
 */
typedef enum {
    MAX31343_TTSINT_1S   = 0x0, /**< update temperature every 1 s   */
    MAX31343_TTSINT_2S   = 0x1, /**< update temperature every 2 s   */
    MAX31343_TTSINT_4S   = 0x2, /**< update temperature every 4 s   */
    MAX31343_TTSINT_8S   = 0x3, /**< update temperature every 8 s   */
    MAX31343_TTSINT_16S  = 0x4, /**< update temperature every 16 s  */
    MAX31343_TTSINT_32S  = 0x5, /**< update temperature every 32 s  */
    MAX31343_TTSINT_64S  = 0x6, /**< update temperature every 64 s  */
    MAX31343_TTSINT_128S = 0x7, /**< update temperature every 128 s */
} max31343_ttsint_t;

/**
 * @brief Trickle charger resistor selection.
 *
 * Selects the series resistor in the trickle charging path.
 * Corresponds to D_TRICKLE bits [1:0].
 */
typedef enum {
    MAX31343_TRICKLE_RES_3K  = 0x0U,  /**< 3 kΩ  */
    MAX31343_TRICKLE_RES_6K  = 0x2U,  /**< 6 kΩ  */
    MAX31343_TRICKLE_RES_11K = 0x3U,  /**< 11 kΩ */
} max31343_trickle_res_t;

/**
 * @brief Trickle charger diode path selection.
 *
 * Selects whether an additional diode is inserted in the charging path.
 * Corresponds to D_TRICKLE bit 2.
 *
 * - SCHOTTKY:      Schottky diode only.
 * - PLUS_SCHOTTKY: Additional diode in series with a Schottky diode.
 */
typedef enum {
    MAX31343_TRICKLE_DIODE_SCHOTTKY      = 0U, /**< Schottky diode only          */
    MAX31343_TRICKLE_DIODE_PLUS_SCHOTTKY = 1U, /**< Diode + Schottky diode path  */
} max31343_trickle_diode_t;

/**
 * @brief Configuration parameters for MAX31343 initialization.
 *
 * These parameters are applied once during max31343_init().
 * Runtime changes can be made using the individual setter functions.
 */
typedef struct {
    i2c_t i2c;                              /**< I2C bus the device is connected to */
    max31343_sqw_freq_t sqw_freq;           /**< SQW output frequency */
    bool trickle_enable;                    /**< enable trickle charger on init */
    max31343_trickle_diode_t trickle_diode; /**< trickle charger diode path */
    max31343_trickle_res_t trickle_res;     /**< trickle charger series resistor */
} max31343_params_t;

/**
 * @brief Initialize MAX31343 device
 *
 * This function initializes the device and checks the Oscillator Stop Flag (OSF).
 * If OSF is set (indicating the oscillator was stopped, e.g., after power loss),
 * the current time may be invalid and should be set using max31343_set_time().
 * The OSF flag is automatically cleared when the time registers are written.
 *
 * @param[out] dev     device descriptor
 * @param[in]  params  device parameters
 *
 * @retval  0        Success
 * @retval -EINVAL   Invalid argument (NULL pointer)
 * @retval -EIO      I2C communication error
 * @retval -ENODATA  Oscillator was stopped; time is invalid.
 *                   Call max31343_set_time() before using max31343_get_time().
 *
 * @note After power-on or if the oscillator was stopped, the caller should
 *       check if the time is valid and set it if necessary.
 */
int max31343_init(max31343_t *dev, const max31343_params_t *params);

/**
 * @brief Read current time from device
 *
 * @param[in]  dev   device descriptor
 * @param[out] time  time structure to fill
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error or invalid time read from device
 */
int max31343_get_time(const max31343_t *dev, struct tm *time);

/**
 * @brief Set current time on device
 *
 * @param[in] dev   device descriptor
 * @param[in] time  time structure to set
 *
 * @retval  0       Success
 * @retval -ERANGE  Time values are out of supported range (year must be 2000-2099)
 * @retval -EIO     I2C communication error
 */
int max31343_set_time(const max31343_t *dev, const struct tm *time);

/**
 * @brief Enable RTC oscillator (power on timekeeping).
 *
 * Sets ENOSC bit in RTC_CFG1.
 *
 * @param[in] dev device descriptor
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max31343_poweron(const max31343_t *dev);

/**
 * @brief Disable RTC oscillator (stop timekeeping).
 *
 * Clears ENOSC bit in RTC_CFG1.
 *
 * @param[in] dev device descriptor
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max31343_poweroff(const max31343_t *dev);

/**
 * @brief Set alarm time registers.
 *
 * Writes the alarm time to the device and disables the alarm interrupt (A1IE).
 * The alarm interrupt is NOT automatically re-enabled by this function.
 *
 * @param[in] dev   device descriptor
 * @param[in] time  alarm time to store
 *
 * @retval  0       Success
 * @retval -ERANGE  Time values are out of supported range (year must be 2000-2099)
 * @retval -EIO     I2C communication error
 *
 * @note Per datasheet requirement, the alarm interrupt (A1IE) must not be
 *       enabled until at least 1 second after calling this function.
 *       Use max31343_alarm_enable() after the required delay.
 */
int max31343_set_alarm(const max31343_t *dev, const struct tm *time);

/**
 * @brief Get the currently configured alarm time.
 *
 * @param[in]  dev   device descriptor
 * @param[out] time  receives the stored alarm time
 *
 * @retval  0       Success
 * @retval -ENOENT  No alarm is set (mask bits are active)
 * @retval -EIO     I2C communication error
 */
int max31343_get_alarm(const max31343_t *dev, struct tm *time);

/**
 * @brief Enable or disable the alarm interrupt.
 *
 * Controls the alarm interrupt enable bit (A1IE) in the interrupt enable register.
 *
 * @param[in] dev    device descriptor
 * @param[in] enable true to enable alarm interrupt, false to disable
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 *
 * @note When enabling the alarm after max31343_set_alarm(), wait at least
 *       1 second as required by the datasheet before calling this function.
 */
int max31343_alarm_enable(const max31343_t *dev, bool enable);

/**
 * @brief Clear the alarm.
 *
 * Disables the alarm interrupt (A1IE) and reads the status register
 * to clear the alarm flag (A1F).
 *
 * @param[in] dev device descriptor
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max31343_clear_alarm(const max31343_t *dev);

/**
 * @brief Configure the square-wave (SQW) output frequency.
 *
 * This function enables and configures the SQW output according
 * to the selected frequency.
 *
 * @param[in] dev   device descriptor
 * @param[in] freq  square-wave frequency selection
 *
 * @retval  0       Success
 * @retval -ERANGE  Invalid frequency value
 * @retval -EIO     I2C communication error
 */
int max31343_set_sqw(const max31343_t *dev, max31343_sqw_freq_t freq);

/**
 * @brief Read temperature in centi-degrees Celsius (°C * 100)
 *
 * Example: 84.75°C -> 8475
 *
 * @param[in]  dev        device descriptor
 * @param[out] temp_centi temperature in centi-degC
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max31343_get_temp(const max31343_t *dev, int16_t *temp_centi);

/**
 * @brief Enable the trickle charger.
 *
 * The trickle charger can be used to slowly charge a supercapacitor or
 * rechargeable backup battery connected to VBAT. The charging current is
 * determined by the selected diode path and series resistor:
 * I = (VCC - V_diode - V_BAT) / R
 *
 * @param[in] dev    device descriptor
 * @param[in] diode  diode path selection
 * @param[in] res    series resistor selection
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max31343_trickle_charge_enable(const max31343_t *dev,
                                   max31343_trickle_diode_t diode,
                                   max31343_trickle_res_t res);

/**
 * @brief Disable the trickle charger.
 *
 * @param[in] dev  device descriptor
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max31343_trickle_charge_disable(const max31343_t *dev);

/**
 * @brief Configure automatic temperature conversion mode and interval.
 *
 * @param[in] dev      device descriptor
 * @param[in] enable   true to set AUTOMODE=1, false to set AUTOMODE=0
 * @param[in] ttsint   value written to TS_Config[5:3]
 *
 * @retval  0       Success
 * @retval -ERANGE  Invalid ttsint value
 * @retval -EIO     I2C communication error
 */
int max31343_temp_set_automode(const max31343_t *dev, bool enable, max31343_ttsint_t ttsint);

/** @} */

#ifdef __cplusplus
}
#endif
