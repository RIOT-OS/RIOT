/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef LC709203F_H
#define LC709203F_H

/**
 * @defgroup    drivers_lc709203f LC709203F
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the LC709203F Battery Fuel Gauge
 * @{
 *
 * @file
 * @brief       Device driver interface for the LC709203F Battery Fuel Gauge
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"

/**
 * @brief Current direction modes
 * @details For more details please see the datasheet (http://www.onsemi.com/pub/Collateral/LC709203F-D.PDF)
 * @{
 */
typedef enum {
    AUTO_MODE       = 0x0,
    CHARGE_MODE     = 0x1,
    DISCHARGE_MODE  = 0xffff
} lc709203f_current_direction_t;
/** @} */

/**
 * @brief Battery Profile Options
 * @details For more details please see the datasheet (http://www.onsemi.com/pub/Collateral/LC709203F-D.PDF)
 * @{
 */
typedef enum {
    BAT_PROFILE_1   = 0,
    BAT_PROFILE_2   = 1
} lc709203f_battery_profile_t;
/** @} */

/**
 * @brief Power mode choices
 * @details For more details please see the datasheet (http://www.onsemi.com/pub/Collateral/LC709203F-D.PDF)
 * @{
 */
typedef enum {
    OPERATIONAL_MODE    = 1,
    SLEEP_MODE          = 2
} lc709203f_power_mode_t;
/** @} */

/**
 * @brief Temperature obtaining mode options
 * @details For more details please see the datasheet (http://www.onsemi.com/pub/Collateral/LC709203F-D.PDF)
 * @{
 */
typedef enum {
    I2C_MODE        = 0,
    THERMISTOR_MODE = 1
} lc709203f_temp_obtaining_mode_t;
/** @} */

/**
 * @brief Typedef for the Callback function
 * @details A function of this type will be called when an Interrupt is triggered on low RSOC or Voltage
 * @param[in]  arg Additional Arguments that will be passed to the function
 */
typedef void (*lc709203f_cb_t)(void *arg);

/**
 * @brief Parameter struct for driver initialization
 * @{
 */
typedef struct {
    gpio_t alarm_pin;           /**< Pin which is connected to the interrupt pin of the sensor */
    i2c_t bus;                  /**< I2C bus to use */
    uint8_t addr;               /**< I2C Address of the fuel gauge */
} lc709203f_params_t;
/** @} */

enum {
    LC709203F_OK                =  0,       /**< all went as expected */
    LC709203F_NOI2C             = -1,       /**< error using the I2C bus */
    LC709203F_CELL_TEMP_INVALID = -2        /**< Cell temp invalid */
};

/**
 * @brief Device descriptor for the fuel gauge
 * @details This struct will hold all information and configuration for the sensor
 * @{
 */
typedef struct {
    i2c_t bus;                  /**< I2C bus to use */
    uint8_t addr;               /**< I2C Address of fuel gauge */
    uint8_t scale;              /**< scale for returned values */
    lc709203f_params_t params;  /**< param struct with static settings etc*/
    lc709203f_cb_t cb;          /**< callback method*/
    void *arg;                  /**< additional arguments for the callback method*/
} lc709203f_t;
/** @} */

/**
 * @brief Register Addresses
 * @details For more information on the registers and its functions see the datasheet
 * @{
 */
#define LC709203F_REG_CELL_VOLTAGE      0x09        /**< Register Cell Voltage */
#define LC709203F_REG_RSOC              0x0d        /**< Register RSOC (Relative State of Charge) */
#define LC709203F_REG_ITE               0x0f        /**< Register ITE (Indicator To Empty) */
#define LC709203F_REG_ID                0x11        /**< Register IC Version */
#define LC709203F_REG_CELL_TEMP         0x08        /**< Register Cell Temperature */
#define LC709203F_REG_STATUS            0x16        /**< Register Status Bit */
#define LC709203F_REG_POWER_MODE        0x15        /**< Register IC Power Mode */
#define LC709203F_REG_ALARM_VOLTAGE     0x14        /**< Register Alarm Low Cell Voltage */
#define LC709203F_REG_ALARM_RSOC        0x13        /**< Register Alarm Low RSOC */
#define LC709203F_REG_CHANGE_PARAMETER  0x12        /**< Register Change of the parameter */
#define LC709203F_REG_APT               0x0c        /**< Register APT (Adjustment Pack Thermistor) */
#define LC709203F_REG_APA               0x0b        /**< Register APA (Adjustment Pack Application) */
#define LC709203F_REG_CURRENT_DIRECTION 0x0a        /**< Register Current Direction */
#define LC709203F_REG_THERMISTOR        0x06        /**< Register Thermistor B */
#define LC709203F_REG_BEFORE_RSOC       0x04        /**< Register Before RSOC */
#define LC709203F_REG_INITIAL_RSOC      0x07        /**< Register Initial RSOC */
/** @} */

#define LC709203F_CRC_POLYNOMIAL        0x07        /**< CRC Polynomial */

/**
 * @brief initializes the sensor and i2c
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] *params   pointer to lc709203f_param_t struct containing the interrupt pin and callback
 *
 * @return              LC709203F_OK on a working initialization
 * @return              LC709203F_NOI2C on undefined i2c device given in periph_conf
 */
int lc709203f_init(lc709203f_t *dev, const lc709203f_params_t *params);

/**
 * @brief  reads battery voltage from Sensor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              voltage in mV
 */
int16_t lc709203f_get_voltage(const lc709203f_t *dev);

/**
 * @brief  reads battery state of charge from Sensor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              rsoc in %
 */
int16_t lc709203f_get_rsoc(const lc709203f_t *dev);

/**
 * @brief  reads battery indicator to empty from Sensor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              ite in 0.1%
 */
int16_t lc709203f_get_ite(const lc709203f_t *dev);

/**
 * @brief  reads Sensor id
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              id value
 */
int16_t lc709203f_get_id(const lc709203f_t *dev);

/**
 * @brief  reads cell temperature from sensor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              temp in 0.1°C steps
 */
int16_t lc709203f_get_cell_temp(const lc709203f_t *dev);

/**
 * @brief  reads status bit register (Temperature obtaining mode)
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              THERMISTOR_MODE Thermistor mode
 * @return              I2C_MODE I2C Mode
 */
lc709203f_temp_obtaining_mode_t lc709203f_get_status_bit(const lc709203f_t *dev);

/**
 * @brief  reads IC Power Mode register
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              OPERATIONAL_MODE Operational Mode
 * @return              SLEEP_MODE Sleep mode
 */
lc709203f_power_mode_t lc709203f_get_power_mode(const lc709203f_t *dev);

/**
 * @brief  reads Alarm Low Cell Voltage Register
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              0 Disable
 * @return              Threshold value in 1mV steps
 */
int16_t lc709203f_get_alarm_low_voltage(const lc709203f_t *dev);

/**
 * @brief  reads Alarm Low RSOC Register
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              0 Disable
 * @return              Threshold value in %
 */
int16_t lc709203f_get_alarm_low_rsoc(const lc709203f_t *dev);

/**
 * @brief  reads Change of the Parameter register, Battery Profile selection
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              Battery profile number (1 or 0)
 */
int16_t lc709203f_get_change_of_parameter(const lc709203f_t *dev);

/**
 * @brief  reads APT (Adjustment Pack Thermistor)
 * value to adjust temperature measurement delay timing.
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              value to adjust temperature measurement delay timing.
 */
int16_t lc709203f_get_apt(const lc709203f_t *dev);

/**
 * @brief  reads APA (Adjustment Pack Application) in 1mOhm steps
 * value to adjust temperature measurement delay timing.
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              parasitic impedance
 */
int16_t lc709203f_get_apa(const lc709203f_t *dev);

/**
 * @brief  reads Current Direction Register
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              AUTO_MODE Auto mode
 * @return              CHARGE_MODE charge mode
 * @return              DISCHARGE_MODE Discharge mode
 */
lc709203f_current_direction_t lc709203f_get_current_direction(const lc709203f_t *dev);

/**
 * @brief  reads Thermistor-B register (Thermistor B constant to be measured)
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              B Value in 1K steps
 */
int16_t lc709203f_get_thermistor_b(const lc709203f_t *dev);

/**
 * @brief  Executes RSOC initialization with sampled maximum voltage
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 */
void lc709203f_set_rsoc_before(const lc709203f_t *dev);

/**
 * @brief  Sets B constant of the Thermistor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     Bconstant of thermistor
 */
void lc709203f_set_thermistor_b(const lc709203f_t *dev, const unsigned int value);

/**
 * @brief  Executes RSOC initialization
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 */
void lc709203f_set_rsoc_initial(const lc709203f_t *dev);

/**
 * @brief  Sets cell temperature in I2C-mode
 *
 * @param[in] *dev		pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value		temp in 0.1K 0xAAC=0°C
 *
 * @return              LC709203F_CELL_TEMP_INVALID Invalid cell temp

 */
int8_t lc709203f_set_cell_temp(const lc709203f_t *dev, const unsigned int value);

/**
 * @brief  Sets current direction
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] direction current direction, AUTO_MODE, CHARGE_MODE, DISCHARGE_MODE
 */
void lc709203f_set_current_direction(const lc709203f_t *dev, const lc709203f_current_direction_t direction);

/**
 * @brief  Sets parasitic impedance (adjustment pack application)
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     impedance in 1mOhm steps
 */
void lc709203f_set_apa(const lc709203f_t *dev, const uint8_t value);

/**
 * @brief  Sets value to adjust temperature measurement delay timing (adjusmtent pack thermistor)
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     adjustment value
 */
void lc709203f_set_apt(const lc709203f_t *dev, const unsigned int value);

/**
 * @brief  Sets battery profile
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     profile (enum) BAT_PROFILE_1 BAT_PROFILE_2
 */
void lc709203f_set_change_of_parameter(const lc709203f_t *dev, const lc709203f_battery_profile_t value);

/**
 * @brief  Sets threshold for low rsoc alert
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     alert threshold (0-100%)
 */
void lc709203f_set_alarm_low_rsoc(const lc709203f_t *dev, const uint8_t value);

/**
 * @brief  Sets threshold for low cell voltage alert
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     alert threshold in 1mV steps
 */
void lc709203f_set_alarm_low_cell_voltage(const lc709203f_t *dev, const unsigned int value);

/**
 * @brief  Sets power mode
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     Power mode (enum) OPERATIONAL_MODE, SLEEP_MODE
 */
void lc709203f_set_power_mode(const lc709203f_t *dev, const lc709203f_power_mode_t value);

/**
 * @brief  Sets temperature obtaining method
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     method to be used (enum) I2C_MODE, THERMISTOR_MODE
 */
void lc709203f_set_status_bit(const lc709203f_t *dev, const lc709203f_temp_obtaining_mode_t value);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LC709203F_H */
