/*
 * SPDX-FileCopyrightText: 2019 University of Applied Sciences Emden / Leer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_ph_oem pH OEM sensor device driver
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for Atlas Scientific pH OEM sensor with SMBus/I2C interface
 *
 * The Atlas Scientific pH OEM sensor can be used with or without the interrupt
 * pin. Per default this pin is mapped to @ref GPIO_UNDEF if not otherwise defined
 * in your makefile.
 *
 * If you use an electrical isolation for most accurate readings
 * e.g. with the ADM3260, keep in mind that its not recommended to use the
 * interrupt pin without also isolating it somehow. The preferred method,
 * if not using an isolation on the interrupt line, would be polling. In this case
 * leave the interrupt pin undefined.
 *
 * The Sensor has no integrated temperature sensor and for the highest possible
 * precision it requires another device to provide the temperature for error
 * compensation.
 *
 * Once the pH OEM is powered on it will be ready to receive commands and take
 * readings after 1ms.
 *
 * @note This driver provides @ref drivers_saul capabilities.
 * Reading (@ref saul_driver_t.read) from the device returns the current pH value.
 * Writing (@ref saul_driver_t.write) a temperature value in celsius to the
 * device sets the temperature compensation. A valid temperature range is
 * 1 - 20000 (0.01 °C  to  200.0 °C)
 *
 * @note Communication is done using SMBus/I2C protocol at speeds
 * of 10-100 kHz. Set your board I2C speed to @ref I2C_SPEED_LOW or
 * @ref I2C_SPEED_NORMAL
 *
 * @{
 *
 * @file
 * @brief       Device driver for Atlas Scientific pH OEM Sensor with SMBus/I2C interface

 * @author      Igor Knippenberg <igor.knippenberg@gmail.com>
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"

/**
 * @brief   Named return values
 */
typedef enum {
    PH_OEM_OK                   =  0,   /**< Everything was fine */
    PH_OEM_NODEV                = -1,   /**< No device found on the bus */
    PH_OEM_READ_ERR             = -2,   /**< Reading to device failed*/
    PH_OEM_WRITE_ERR            = -3,   /**< Writing to device failed */
    PH_OEM_NOT_PH               = -4,   /**< Not an Atlas Scientific pH OEM device */
    PH_OEM_INTERRUPT_GPIO_UNDEF = -5,   /**< Interrupt pin is @ref GPIO_UNDEF */
    PH_OEM_GPIO_INIT_ERR        = -6,   /**< Error while initializing GPIO PIN */
    PH_OEM_TEMP_OUT_OF_RANGE    = -7    /**< Temperature is out of range */
} ph_oem_named_returns_t;

/**
 * @brief   LED state values
 */
typedef enum {
    PH_OEM_LED_ON   = 0x01, /**< LED on state */
    PH_OEM_LED_OFF  = 0x00, /**< LED off state */
} ph_oem_led_state_t;

/**
 * @brief   Device state values
 */
typedef enum {
    PH_OEM_TAKE_READINGS    = 0x01, /**< Device active state */
    PH_OEM_STOP_READINGS    = 0x00, /**< Device hibernate state */
} ph_oem_device_state_t;
/**
 * @brief   Interrupt pin option values
 */
typedef enum {
    PH_OEM_IRQ_RISING   = 0x02, /**< Pin high on new reading (manually reset) */
    PH_OEM_IRQ_FALLING  = 0x04, /**< Pin low on new reading (manually reset) */
    PH_OEM_IRQ_BOTH     = 0x08, /**< Invert state on new reading (automatically reset) */
} ph_oem_irq_option_t;

/**
 * @brief   Calibration option values
 */
typedef enum {
    PH_OEM_CALIBRATE_LOW_POINT  = 0x02,     /**< Low point calibration option */
    PH_OEM_CALIBRATE_MID_POINT  = 0x03,     /**< Mid point calibration option */
    PH_OEM_CALIBRATE_HIGH_POINT = 0x04,     /**< High point calibration option */
} ph_oem_calibration_option_t;

/**
 * @brief   pH OEM sensor params
 */
typedef struct ph_oem_params {
    i2c_t i2c;                      /**< I2C device the sensor is connected to */
    uint8_t addr;                   /**< the slave address of the sensor on the I2C bus */
    gpio_t interrupt_pin;           /**< interrupt pin (@ref GPIO_UNDEF if not defined) */
    gpio_mode_t gpio_mode;          /**< gpio mode of the interrupt pin */
    ph_oem_irq_option_t irq_option; /**< behavior of the interrupt pin, disabled by default */
} ph_oem_params_t;

/**
 * @brief   pH OEM interrupt pin callback
 */
typedef void (*ph_oem_interrupt_pin_cb_t)(void *);

/**
 * @brief   pH OEM device descriptor
 */
typedef struct ph_oem {
    ph_oem_params_t params;         /**< device driver configuration */
    ph_oem_interrupt_pin_cb_t cb;   /**< interrupt pin callback */
    void *arg;                      /**< interrupt pin callback param */
} ph_oem_t;

/**
 * @brief   Initialize a pH OEM sensor
 *
 * @param[in,out]   dev      device descriptor
 * @param[in]       params   device configuration
 *
 * @retval  PH_OEM_OK       on success
 * @retval  PH_OEM_NODEV    if no device is found on the bus
 * @retval  PH_OEM_NOT_PH   if the device found at the address is not a pH OEM device
 */
int ph_oem_init(ph_oem_t *dev, const ph_oem_params_t *params);

/**
 * @brief   Sets a new address to the pH OEM device by unlocking the
 *          @ref PH_OEM_REG_UNLOCK register and  writing a new address to
 *          the @ref PH_OEM_REG_ADDRESS register.
 *          The device address will also be updated in the device descriptor so
 *          it is still usable.
 *
 *          Settings are retained in the sensor if the power is cut.
 *
 *          The address in the device descriptor will reverse to the default
 *          address you provided through PH_OEM_PARAM_ADDR after the
 *          microcontroller restarts
 *
 * @param[in] dev   device descriptor
 * @param[in] addr  new address for the device. Range: 0x01 - 0x7f
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 */
int ph_oem_set_i2c_address(ph_oem_t *dev, uint8_t addr);

/**
 * @brief   Enable the pH OEM interrupt pin if @ref ph_oem_params_t.interrupt_pin
 *          is defined.
 *          @note @ref ph_oem_reset_interrupt_pin needs to be called in the
 *          callback if you use @ref PH_OEM_IRQ_FALLING or @ref PH_OEM_IRQ_RISING
 *
 *          @note Provide the PH_OEM_PARAM_INTERRUPT_OPTION flag in your
 *          makefile. Valid options see: @ref ph_oem_irq_option_t.
 *          The default is @ref PH_OEM_IRQ_BOTH.
 *
 *          @note Also provide the @ref gpio_mode_t as a CFLAG in your makefile.
 *          Most likely @ref GPIO_IN. If the pin is to sensitive use
 *          @ref GPIO_IN_PU for @ref PH_OEM_IRQ_FALLING or
 *          @ref GPIO_IN_PD for @ref PH_OEM_IRQ_RISING and
 *          @ref PH_OEM_IRQ_BOTH. The default is @ref GPIO_IN_PD
 *
 *
 * @param[in] dev       device descriptor
 * @param[in] cb        callback called when the pH OEM interrupt pin fires
 * @param[in] arg       callback argument
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 * @return @ref PH_OEM_INTERRUPT_GPIO_UNDEF if the interrupt pin is undefined
 * @return @ref PH_OEM_GPIO_INIT_ERR if initializing the interrupt gpio pin failed
 */
int ph_oem_enable_interrupt(ph_oem_t *dev, ph_oem_interrupt_pin_cb_t cb,
                            void *arg);

/**
 * @brief   The interrupt pin will not auto reset on option @ref PH_OEM_IRQ_RISING
 *          and @ref PH_OEM_IRQ_FALLING after interrupt fires,
 *          so call this function again to reset the pin state.
 *
 * @note    The interrupt settings are not retained if the power is cut,
 *          so you have to call this function again after powering on the device.
 *
 * @param[in] dev    device descriptor
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 */
int ph_oem_reset_interrupt_pin(const ph_oem_t *dev);

/**
 * @brief   Set the LED state of the pH OEM sensor by writing to the
 *          @ref PH_OEM_REG_LED register
 *
 * @param[in] dev       device descriptor
 * @param[in] state     @ref ph_oem_led_state_t
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 */
int ph_oem_set_led_state(const ph_oem_t *dev, ph_oem_led_state_t state);

/**
 * @brief   Sets the device state (active/hibernate) of the pH OEM sensor by
 *          writing to the @ref PH_OEM_REG_HIBERNATE register.
 *
 *          @note Once the device has been woken up it will continuously take
 *          readings every 420ms. Waking the device is the only way to take a
 *          reading. Hibernating the device is the only way to stop taking readings.
 *
 * @param[in] dev   device descriptor
 * @param[in] state @ref ph_oem_device_state_t
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 */
int ph_oem_set_device_state(const ph_oem_t *dev, ph_oem_device_state_t state);

/**
 * @brief   Starts a new reading by setting the device state to
 *          @ref PH_OEM_TAKE_READINGS.
 *
 * @note    If the @ref ph_oem_params_t.interrupt_pin is @ref GPIO_UNDEF
 *          this function will poll every 20ms till a reading is done (~420ms)
 *          and stop the device from taking further readings
 *
 * @param[in] dev   device descriptor
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 * @return @ref PH_OEM_READ_ERR if reading from the device failed
 */
int ph_oem_start_new_reading(const ph_oem_t *dev);

/**
 * @brief   Clears all calibrations previously done
 *
 * @param[in] dev   device descriptor
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 * @return @ref PH_OEM_READ_ERR if reading from the device failed
 */
int ph_oem_clear_calibration(const ph_oem_t *dev);

/**
 * @brief   Sets the @ref PH_OEM_REG_CALIBRATION_BASE register to the
 *          calibration_value which the pH OEM sensor will be
 *          calibrated to. Multiply the floating point calibration value of your
 *          solution by 1000 e.g. pH calibration solution => 7.002 * 1000 = 7002 = 0x00001B5A
 *
 *          The calibration value will be saved based on the given
 *          @ref ph_oem_calibration_option_t and retained after the power is cut.
 *
 * @note    Calibrating with @ref PH_OEM_CALIBRATE_MID_POINT will reset the
 *          previous calibrations.
 *          Always start with @ref PH_OEM_CALIBRATE_MID_POINT if you doing
 *          2 or 3 point calibration
 *
 * @param[in] dev                 device descriptor
 * @param[in] calibration_value   pH value multiplied by 1000 e.g 7,002 * 1000 = 7002
 * @param[in] option              @ref ph_oem_calibration_option_t
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 * @return @ref PH_OEM_READ_ERR if reading from the device failed
 */
int ph_oem_set_calibration(const ph_oem_t *dev, uint16_t calibration_value,
                           ph_oem_calibration_option_t option);

/**
 * @brief   Read the @ref PH_OEM_REG_CALIBRATION_CONFIRM register.
 *          After a calibration event has been successfully carried out, the
 *          calibration confirmation register will reflect what calibration has
 *          been done, by setting bits 0 - 2.
 *
 * @param[in]  dev                 device descriptor
 * @param[out] calibration_state   calibration state reflected by bits 0 - 2 <br>
 *                                 (0 = low, 1 = mid, 2 = high)
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_READ_ERR if reading from the device failed
 */
int ph_oem_read_calibration_state(const ph_oem_t *dev, uint16_t *calibration_state);

/**
 * @brief   Sets the @ref PH_OEM_REG_TEMP_COMPENSATION_BASE register to the
 *          temperature_compensation value which the pH OEM sensor will use
 *          to compensate the reading error.
 *          Multiply the floating point temperature value by 100
 *          e.g. temperature in degree Celsius = 34.26 * 100 = 3426
 *
 *  @note   The temperature compensation will not be retained if the power is cut.
 *
 * @param[in] dev                        device descriptor
 * @param[in] temperature_compensation   valid temperature range is
 *                                       1 - 20000 (0.01 °C  to  200.0 °C)
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_WRITE_ERR if writing to the device failed
 * @return @ref PH_OEM_TEMP_OUT_OF_RANGE if the temperature_compensation is not in
 *                                       the valid range
 */
int ph_oem_set_compensation(const ph_oem_t *dev,
                            uint16_t temperature_compensation);

/**
 * @brief   Reads the @ref PH_OEM_REG_TEMP_CONFIRMATION_BASE register to verify
 *          the temperature compensation value that was used to take the pH
 *          reading is set to the correct temperature.
 *
 * @param[in]  dev                       device descriptor
 * @param[out] temperature_compensation  raw temperature compensation value. <br>
 *                                       Divide by 100 for floating point <br>
 *                                       e.g 3426 / 100 = 34.26
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_READ_ERR if reading from the device failed
 */
int ph_oem_read_compensation(const ph_oem_t *dev,
                             uint16_t *temperature_compensation);

/**
 * @brief   Reads the @ref PH_OEM_REG_PH_READING_BASE register to get the current
 *          pH reading.
 *
 * @param[in]  dev        device descriptor
 * @param[out] ph_value   raw pH value <br>
 *                        divide by 1000 for floating point <br>
 *                        e.g 8347 / 1000 = 8.347
 *
 * @return @ref PH_OEM_OK on success
 * @return @ref PH_OEM_READ_ERR if reading from the device failed
 */
int ph_oem_read_ph(const ph_oem_t *dev, uint16_t *ph_value);

#ifdef __cplusplus
}
#endif

/** @} */
