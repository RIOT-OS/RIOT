/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_max313xx  MAX313xx I2C RTC driver
 * @ingroup     drivers_sensors
 * @brief       Driver for the MAX313xx I2C real-time clock family
 * @{
 *
 * The driver can be used directly in an application or through the
 * @ref sys_walltime module by adding one of the following lines to your
 * application's Makefile (depending on the device you have):
 * ```makefile
 * USEMODULE += walltime_impl_max31331
 * USEMODULE += walltime_impl_max31343
 * ```
 *
 * ## Implementation Status and Supported Devices
 * The driver does not handle interrupts from the real-time clock. The
 * application has to configure the appropriate pin with @ref gpio_init_int
 * and provide a callback.
 *
 * Furthermore there is no support for the built-in User Storage Memory.
 *
 * Only ALARM1 is supported, ALARM2 has limited capabilities compared to
 * ALARM1. The Countdown Timer is also not supported yet.
 *
 * The driver has support for the MAX31331 and MAX31343 devices, although not
 * all features of each device is implemented.
 *
 * @file
 * @brief       Driver interface for the MAX313xx I2C real-time clock
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 */

#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "periph/i2c.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device descriptor for the MAX313xx RTC.
 *
 * Holds the runtime state of a MAX313xx device instance.
 * Currently this only contains the I2C bus, but may be extended
 * in the future.
 */
typedef struct {
    i2c_t i2c;     /**< I2C bus */
} max313xx_t;

/**
 * @brief Square-wave output frequency selection.
 *
 * Selects the output frequency of the SQW pin.
 * The exact mapping is defined by the MAX31343 datasheet.
 *
 * @note Only supported on the MAX31343.
 */
typedef enum {
    MAX313xx_SQW_1HZ = 0,
    MAX313xx_SQW_2HZ = 1,
    MAX313xx_SQW_4HZ = 2,
    MAX313xx_SQW_8HZ = 3,
    MAX313xx_SQW_16HZ = 4,
    MAX313xx_SQW_32HZ = 5,
} max313xx_sqw_freq_t;

/**
 * @brief Automatic temperature conversion interval.
 *
 * Defines the interval for automatic temperature measurements
 * when temperature AUTOMODE is enabled.
 *
 * The values correspond to the TTSINT field (TS_Config[5:3]).
 *
 * @note Only supported on the MAX31343.
 */
typedef enum {
    MAX313xx_TTSINT_1S   = 0x0, /**< update temperature every 1s   */
    MAX313xx_TTSINT_2S   = 0x1, /**< update temperature every 2s   */
    MAX313xx_TTSINT_4S   = 0x2, /**< update temperature every 4s   */
    MAX313xx_TTSINT_8S   = 0x3, /**< update temperature every 8s   */
    MAX313xx_TTSINT_16S  = 0x4, /**< update temperature every 16s  */
    MAX313xx_TTSINT_32S  = 0x5, /**< update temperature every 32s  */
    MAX313xx_TTSINT_64S  = 0x6, /**< update temperature every 64s  */
    MAX313xx_TTSINT_128S = 0x7, /**< update temperature every 128s */
} max313xx_ttsint_t;

/**
 * @brief Trickle charger resistor selection.
 *
 * Selects the series resistor in the trickle charging path.
 * Corresponds to TRICKLE bits [1:2] for the MAX31331 and to
 * D_TRICKLE bits [1:0] for the MAX31343.
 */
typedef enum {
    MAX313xx_TRICKLE_RES_3K  = 0x0U,  /**< 3 kOhm  */
    MAX313xx_TRICKLE_RES_6K  = 0x2U,  /**< 6 kOhm  */
    MAX313xx_TRICKLE_RES_11K = 0x3U,  /**< 11 kOhm */
} max313xx_trickle_res_t;

/**
 * @brief Configuration parameters for MAX313xx initialization.
 */
typedef struct {
    i2c_t i2c;                              /**< I2C bus the device is connected to */
} max313xx_params_t;

/**
 * @brief Initialize MAX313xx device
 *
 * This function initializes the device and checks the Oscillator Stop Flag (OSF).
 * If OSF is set (indicating the oscillator was stopped, e.g., after power loss),
 * the current time may be invalid and should be set using max313xx_set_time().
 * The OSF flag is automatically cleared when the time registers are written.
 *
 * @note After power-on or if the oscillator was stopped, the caller should
 *       check if the time is valid and set it if necessary.
 *
 * @param[out] dev     device descriptor
 * @param[in]  params  device parameters
 *
 * @retval  0        Success
 * @retval -EINVAL   Invalid argument (NULL pointer)
 * @retval -EIO      I2C communication error
 * @retval -ENODATA  Oscillator was stopped; time is invalid.
 *                   Call max313xx_set_time() before using max313xx_get_time().
 */
int max313xx_init(max313xx_t *dev, const max313xx_params_t *params);

/**
 * @brief Read current time from device
 *
 * @param[in]  dev   device descriptor
 * @param[out] time  time structure to fill
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error or invalid time read from device
 */
int max313xx_get_time(const max313xx_t *dev, struct tm *time);

/**
 * @brief Set current time on device
 *
 * @note The new time is set after one second. Reading back the time immediately
 *       will result in a wrong time.
 *
 * @param[in] dev   device descriptor
 * @param[in] time  time structure to set
 *
 * @retval  0       Success
 * @retval -ERANGE  Time values are out of supported range (year must be 2000-2099)
 * @retval -EIO     I2C communication error
 */
int max313xx_set_time(const max313xx_t *dev, const struct tm *time);

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
int max313xx_poweron(const max313xx_t *dev);

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
int max313xx_poweroff(const max313xx_t *dev);

/**
 * @brief Set alarm time registers.
 *
 * Writes the alarm time to the device. The alarm interrupt (A1IE) is
 * disabled before writing and must be explicitly re-enabled afterwards
 * using max313xx_set_alarm_int(), as required by the datasheet.
 *
 * @note Per datasheet requirement, the alarm interrupt (A1IE) must not be
 *       enabled until at least 1 second after calling this function.
 *       Use max313xx_set_alarm_int() after the required delay.
 *
 * @param[in] dev   device descriptor
 * @param[in] time  alarm time to store
 *
 * @retval  0       Success
 * @retval -ERANGE  Time values are out of supported range (year must be 2000-2099)
 * @retval -EIO     I2C communication error
 */
int max313xx_set_alarm(const max313xx_t *dev, const struct tm *time);

/**
 * @brief Get the currently configured alarm time.
 *
 * @note This function does not check the masking bits. If masking bits are set,
 *       not all of the configured time parameters might be considered by
 *       the RTC for a match.
 *
 * @param[in]  dev   device descriptor
 * @param[out] time  receives the stored alarm time
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max313xx_get_alarm(const max313xx_t *dev, struct tm *time);

/**
 * @brief Enable or disable the alarm interrupt.
 *
 * Controls the alarm interrupt enable bit (A1IE) in the interrupt enable
 * register. Disabling the alarm also clears the alarm flag (A1F).
 *
 * @note When enabling the alarm after max313xx_set_alarm(), wait at least
 *       1 second as required by the datasheet before calling this function.
 *
 * @param[in] dev    device descriptor
 * @param[in] enable true to enable alarm interrupt, false to disable
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max313xx_set_alarm_int(const max313xx_t *dev, bool enable);

/**
 * @brief Configure the square-wave (SQW) output frequency.
 *
 * This function enables and configures the SQW output according
 * to the selected frequency.
 *
 * @note Only supported on the MAX31343.
 *
 * @param[in] dev   device descriptor
 * @param[in] freq  square-wave frequency selection
 *
 * @retval  0       Success
 * @retval -ERANGE  Invalid frequency value
 * @retval -EIO     I2C communication error
 * @retval -ENOTSUP RTC variant does not have a square wave output
 */
int max313xx_set_sqw(const max313xx_t *dev, max313xx_sqw_freq_t freq);

/**
 * @brief Read temperature in centi-degrees Celsius (°C * 100)
 *
 * Example: 84.75°C -> 8475
 *
 * @note Only supported on the MAX31343.
 *
 * @param[in]  dev        device descriptor
 * @param[out] temp_centi temperature in centi-degC
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 * @retval -ENOTSUP RTC variant does not have a temperature sensor
 */
int max313xx_get_temp(const max313xx_t *dev, int16_t *temp_centi);

/**
 * @brief Enable the trickle charger.
 *
 * The trickle charger can be used to slowly charge a supercapacitor or
 * rechargeable backup battery connected to VBAT. The charging current is
 * determined by the selected diode path and series resistor:
 * `I = (VCC - V_diode - V_BAT) / R`
 *
 * @param[in] dev    device descriptor
 * @param[in] diode  diode path selection
 * @param[in] res    series resistor selection
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max313xx_trickle_charge_enable(const max313xx_t *dev, bool diode,
                                   max313xx_trickle_res_t res);

/**
 * @brief Disable the trickle charger.
 *
 * @param[in] dev  device descriptor
 *
 * @retval  0       Success
 * @retval -EIO     I2C communication error
 */
int max313xx_trickle_charge_disable(const max313xx_t *dev);

/**
 * @brief Configure automatic temperature conversion mode and interval.
 *
 * @note Only supported on the MAX31343.
 *
 * @param[in] dev      device descriptor
 * @param[in] enable   true to set AUTOMODE=1, false to set AUTOMODE=0
 * @param[in] ttsint   value written to TS_Config[5:3]
 *
 * @retval  0       Success
 * @retval -ERANGE  Invalid ttsint value
 * @retval -EIO     I2C communication error
 * @retval -ENOTSUP RTC variant does not have a temperature sensor
 */
int max313xx_temp_set_automode(const max313xx_t *dev, bool enable, max313xx_ttsint_t ttsint);

/** @} */

#ifdef __cplusplus
}
#endif
