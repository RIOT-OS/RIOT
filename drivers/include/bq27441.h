/*
 * Copyright 2018, Anatoliy Atanasov, Iliyan Stoyanov All rights reserved.
 *
 * This file is port of https://github.com/sparkfun/Battery_Babysitter code
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bq27441 BQ27441
 * @ingroup     drivers_sensors
 * @brief       Default parameters for the BQ27441 fuel gauge for single-cell Li-Ion batteries
 * @{
 *
 * @file
 * @brief       Default parameters for the BQ27441 fuel gauge
 *
 * @author      Anatoliy Atanasov <anatoliy@6lowpan.io>
 * @author      Iliyan Stoyanov <iliyan@6lowpan.io>
 */
#ifndef BQ27441_H
#define BQ27441_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph_conf.h"
#include "periph/i2c.h"

/**
 * @brief Default I2C address of the BQ27441-G1A
 */
#define BQ72441_I2C_ADDRESS 0x55

/**
 * @brief Parameters for the bq27441_get_current() function, to specify which current to read
 * @{
 */
typedef enum {
	AVG,  /**< Average Current (DEFAULT) */
	STBY, /**< Standby Current */
	MAX   /**< Mx Current */
} current_measure;
/** @} */

/**
 * @brief Parameters for the bq27441_get_capacity() function, to specify which current to read
 * @{
 */
typedef enum {
	REMAIN,     /**< Remaining Capacity (DEFAULT) */
	FULL,       /**< Full Capacity */
	AVAIL,      /**< Available Capacity */
	AVAIL_FULL, /**< Full Available Capacity */
	REMAIN_F,   /**< Remaining Capacity Filtered */
	REMAIN_UF,  /**< Remaining Capacity Unfiltered */
	FULL_F,     /**< Full Capacity Filtered */
	FULL_UF,    /**< Full Capacity Unfiltered */
	DESIGN      /**< Design Capacity */
} capacity_measure;
/** @} */

/**
 * @brief Parameters for the bq27441_get_soc() function
 * @{
 */
typedef enum {
	FILTERED,  /**< State of Charge Filtered (DEFAULT) */
	UNFILTERED /**< State of Charge Unfiltered */
} soc_measure;
/** @} */

/**
 * @brief Parameters for the bq27441_get_soh() function
 * @{
 */
typedef enum {
	PERCENT,  /**< State of Health Percentage (DEFAULT) */
	SOH_STAT  /**< State of Health Status Bits */
} soh_measure;
/** @} */

/**
 * @brief Parameters for the bq27441_get_temperature() function
 * @{
 */
typedef enum {
	BATTERY,      /**< Battery Temperature (DEFAULT) */
	INTERNAL_TEMP /**< Internal IC Temperature */
} temp_measure;
/** @} */

/**
 * @brief Parameters for the bq27441_set_gpout_function() function
 * @{
 */
typedef enum {
	SOC_INT, /**< Set GPOUT to SOC_INT functionality */
	BAT_LOW  /**< Set GPOUT to BAT_LOW functionality */
} gpout_function;
/** @} */

/**
 * @brief Parameter struct for driver initialization
 * @{
 */
typedef struct {
    gpio_t alarm_pin;           /**< Pin which is connected to the interrupt pin of the sensor */
    i2c_t bus;                  /**< I2C bus to use */
    uint8_t addr;               /**< I2C Address of the fuel gauge */
} bq27441_param_t;
/** @} */

/**
 * @brief Typedef for the Callback function
 * @details A function of this type will be called when an Interrupt is triggered on low RSOC or Voltage
 * @param[in]  arg Additional Arguments that will be passed to the function
 */
typedef void (*bq27441_cb_t)(void *arg);

/**
 * @brief Device descriptor for the fuel gauge
 * @details This struct will hold all information and configuration for the sensor
 * @{
 */
typedef struct {
    i2c_t bus;                  /**< I2C bus to use */
    uint8_t addr;               /**< I2C Address of the fuel gauge */
    bool sealFlag;              /**< Identify that IC was previously sealed */
    bool userConfigControl;     /**< Identify that user has control over */
    bq27441_param_t param;      /**< param struct with static settings etc*/
    bq27441_cb_t cb;            /**< callback method*/
    void *arg;                  /**< additional arguments for the callback method*/
} bq27441_t;
/** @} */

/**
 * @brief       Initializes I2C and verifies communication with the BQ27441. Must be called before using any other functions.
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   *params     pointer to bq27441_params_t struct containing the interrupt pin and callback
 *
 * @return                  true if communication was successful.
*/
bool bq27441_init(bq27441_t *dev, const bq27441_param_t *params);

/**
 * @brief       Configures the design capacity of the connected battery.
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   capacity    capacity of battery (unsigned 16-bit value)
 *
 * @return                  true if capacity successfully set.
*/
bool bq27441_set_capacity(bq27441_t *dev, uint16_t capacity);

/**
 * @brief       Reads and returns the battery voltage
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      battery voltage in mV
*/
uint16_t bq27441_get_voltage(const bq27441_t *dev);

/**
 * @brief       Reads and returns the specified current measurement
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   type    enum specifying current value to be read
 *
 * @return              specified current measurement in mA. >0 indicates charging.
*/
int16_t bq27441_get_current(const bq27441_t *dev, current_measure type);

/**
 * @brief       Reads and returns the specified capacity measurement
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   type    enum specifying capacity value to be read
 *
 * @return              specified capacity measurement in mAh.
*/
uint16_t bq27441_get_capacity(const bq27441_t *dev, capacity_measure type);

/**
 * @brief       Reads and returns measured average power
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      average power in mAh. >0 indicates charging.
*/
int16_t bq27441_get_power(const bq27441_t *dev);

/**
 * @brief       Reads and returns specified state of charge measurement
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   type    enum specifying filtered or unfiltered measurement
 *
 * @return              specified state of charge measurement in %
*/
uint16_t bq27441_get_soc(const bq27441_t *dev, soc_measure type);

/**
 * @brief       Reads and returns specified state of health measurement
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   type    enum specifying filtered or unfiltered measurement
 *
 * @return              specified state of health measurement in %, or status bits
*/
uint8_t bq27441_get_soh(const bq27441_t *dev, soh_measure type);

/**
 * @brief       Reads and returns specified temperature measurement
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   type    enum specifying internal or battery measurement
 *
 * @return              specified temperature measurement in degrees C
*/
uint16_t bq27441_get_temperature(const bq27441_t *dev, temp_measure type);

/**
 * @brief       Get GPOUT polarity setting (active-high or active-low)
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true if active-high, false if active-low
*/
bool bq27441_get_gpout_polarity(const bq27441_t *dev);

/**
 * @brief       Set GPOUT polarity to active-high or active-low
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   activeHigh  is true if active-high, false if active-low
 *
 * @return                  true on success
*/
bool bq27441_set_gpout_polarity(bq27441_t *dev, bool activeHigh);

/**
 * @brief       Get GPOUT function (BAT_LOW or SOC_INT)
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true if BAT_LOW or false if SOC_INT
*/
bool bq27441_get_gpout_function(const bq27441_t *dev);

/**
 * @brief       Set GPOUT function to BAT_LOW or SOC_INT
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   function    should be either BAT_LOW or SOC_INT
 *
 * @return                  true on success
*/
bool bq27441_set_gpout_function(bq27441_t *dev, gpout_function function);

/**
 * @brief       Get SOC1_Set Threshold - threshold to set the alert flag
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      state of charge value between 0 and 100%
*/
uint8_t bq27441_get_soc1_set_threshold(bq27441_t *dev);

/**
 * @brief       Get SOC1_Clear Threshold - threshold to clear the alert flag
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      state of charge value between 0 and 100%
*/
uint8_t bq27441_get_soc1_clear_threshold(bq27441_t *dev);

/**
 * @brief       Set the SOC1 set and clear thresholds to a percentage
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   set     percentage between 0 and 100
 * @param[in]   clear   percentage between 0 and 100. clear > set.
 *
 * @return              true on success
*/
bool bq27441_set_soc1_thresholds(bq27441_t *dev, uint8_t set, uint8_t clear);

/**
 * @brief       Get SOCF_Set Threshold - threshold to set the alert flag
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      state of charge value between 0 and 100%
*/
uint8_t bq27441_get_socf_set_threshold(bq27441_t *dev);

/**
 * @brief       Get SOCF_Clear Threshold - threshold to clear the alert flag
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      state of charge value between 0 and 100%
*/
uint8_t bq27441_get_socf_clear_threshold(bq27441_t *dev);

/**
 * @brief       Set the SOCF set and clear thresholds to a percentage
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   set     percentage between 0 and 100
 * @param[in]   clear   percentage between 0 and 100. clear > set.
 *
 * @return              true on success
*/
bool bq27441_set_socf_thresholds(bq27441_t *dev, uint8_t set, uint8_t clear);

/**
 * @brief       Check if the SOC1 flag is set in flags()
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true if flag is set
*/
bool bq27441_get_soc1_flag(const bq27441_t *dev);

/**
 * @brief       Check if the SOCF flag is set in flags()
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true if flag is set
*/
bool bq27441_get_socf_flag(const bq27441_t *dev);

/**
 * @brief       Get the SOC_INT interval delta
 *s
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      interval percentage value between 1 and 100
*/
uint8_t bq27441_get_soc_int_delta(bq27441_t *dev);

/**
 * @brief       Set the SOC_INT interval delta to a value between 1 and 100
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   delta   interval percentage value between 1 and 100
 *
 * @return              true on success
*/
bool bq27441_set_soc_int_delta(bq27441_t *dev, uint8_t delta);

/**
 * @brief       Pulse the GPOUT pin - must be in SOC_INT mode
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true on success
*/
bool bq27441_pulse_gpout(const bq27441_t *dev);

/**
 * @brief       Read the device type - should be 0x0421
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return 16-bit value read from DEVICE_TYPE subcommand
*/
uint16_t bq27441_get_device_type(const bq27441_t *dev);

/**
 * @brief       Enter configuration mode - set userControl if calling from an Arduino sketch and you want control over when to exitConfig.
 *
 * @param[in]   *dev            pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   userControl     true if the Arduino sketch is handling entering and exiting config mode (should be false in library calls).
 *
 * @return                      true on success
*/
bool bq27441_enter_config_mode(bq27441_t *dev, bool userControl);

/**
 * @brief       Exit configuration mode with the option to perform a resimulation
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   resim   true if resimulation should be performed after exiting
 *
 * @return              true on success
*/
bool bq27441_exit_config_mode(const bq27441_t *dev, bool resim);

/**
 * @brief       Read the flags() command
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      16-bit representation of flags() command register
*/
uint16_t bq27441_get_flags(const bq27441_t *dev);

/**
 * @brief       Read the CONTROL_STATUS subcommand of control()
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      16-bit representation of CONTROL_STATUS subcommand
*/
uint16_t bq27441_get_status(const bq27441_t *dev);

/**
 * @brief       Check if the BQ27441-G1A is sealed or not.
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true if the chip is sealed
*/
bool _bq27441_sealed(const bq27441_t *dev);

/**
 * @brief       Seal the BQ27441-G1A
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return true on success
*/
bool _bq27441_seal(const bq27441_t *dev);

/**
 * @brief       UNseal the BQ27441-G1A
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true on success
*/
bool _bq27441_unseal(const bq27441_t *dev);

/**
 * @brief       Read the 16-bit opConfig register from extended data
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      opConfig register contents
*/
uint16_t _bq27441_get_opConfig(const bq27441_t *dev);

/**
 * @brief       Write the 16-bit opConfig register in extended data
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   value   16-bit value for opConfig
 *
 * @return              true on success
*/
bool _bq27441_write_op_config(bq27441_t *dev, uint16_t value);

/**
 * @brief       Issue a soft-reset to the BQ27441-G1A
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true on success
*/
bool _bq27441_soft_reset(const bq27441_t *dev);

/**
 * @brief       Read a 16-bit command word from the BQ27441-G1A
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   subAddress  command to be read from
 *
 * @return                  16-bit value of the command's contents
*/
uint16_t _bq27441_read_word(const bq27441_t *dev, uint16_t subAddress);

/**
 * @brief       Read a 16-bit subcommand() from the BQ27441-G1A's control()
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   function    subcommand of control() to be read
 *
 * @return                  16-bit value of the subcommand's contents
*/
uint16_t _bq27441_read_control_word(const bq27441_t *dev, uint16_t function);

/**
 * @brief       Execute a subcommand() from the BQ27441-G1A's control()
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   function    subcommand of control() to be executed
 *
 * @return                  true on success
*/
bool _bq27441_execute_control_word(const bq27441_t *dev, uint16_t function);

/**
 * @brief       Issue a BlockDataControl() command to enable BlockData access
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true on success
*/
bool _bq27441_block_data_control(const bq27441_t *dev);

/**
 * @brief       Issue a DataClass() command to set the data class to be accessed
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   id  id number of the class
 *
 * @return          true on success
*/
bool _bq27441_block_data_class(const bq27441_t *dev, uint8_t id);

/**
 * @brief       Issue a DataBlock() command to set the data block to be accessed
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   offset  offset of the data block
 *
 * @return              true on success
*/
bool _bq27441_block_data_offset(const bq27441_t *dev, uint8_t offset);

/**
 * @brief       Read the current checksum using BlockDataCheckSum()
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      true on success
*/
uint8_t _bq27441_block_data_checksum(const bq27441_t *dev);

/**
 * @brief       Use BlockData() to read a byte from the loaded extended data
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   offset  offset of data block byte to be read
 *
 * @return              true on success
*/
uint8_t _bq27441_read_block_data(const bq27441_t *dev, uint8_t offset);

/**
 * @brief       Use BlockData() to write a byte to an offset of the loaded data
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   offset  position of the byte to be written
 * @param[in]   data    value to be written
 *
 * @return              true on success
*/
bool _bq27441_write_block_data(const bq27441_t *dev, uint8_t offset, uint8_t data);

/**
 * @brief       Read all 32 bytes of the loaded extended data and compute a checksum based on the values.
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 *
 * @return      8-bit checksum value calculated based on loaded data
*/
uint8_t _bq27441_compute_block_checksum(const bq27441_t *dev);

/**
 * @brief       Use the BlockDataCheckSum() command to write a checksum value
 *
 * @param[in]   *dev    pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   csum i  8-bit checksum to be written
 *
 * @return      true on success
*/
bool _bq27441_write_block_checksum(const bq27441_t *dev, uint8_t csum);

/**
 * @brief       Read a byte from extended data specifying a class ID and position offset
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   classID     id of the class to be read from
 * @param[in]   offset      byte position of the byte to be read
 *
 * @return                  8-bit value of specified data
*/
uint8_t _bq27441_read_extended_data(bq27441_t *dev, uint8_t classID, uint8_t offset);

/**
 * @brief       Write a specified number of bytes to extended data specifying a class ID, position offset.
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   classID     id of the class to be read from
 * @param[in]   offset      byte position of the byte to be read
 * @param[in]   data        data buffer to be written
 * @param[in]   len         number of bytes to be written
 *
 * @return                  true on success
*/
bool _bq27441_write_extended_data(bq27441_t *dev, uint8_t classID, uint8_t offset, uint8_t * data, uint8_t len);

/**
 * @brief       Read a specified number of bytes over I2C at a given subAddress
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   subAddress  8-bit address of the data to be read
 * @param[in]   dest        data buffer to be written to
 * @param[in]   count       number of bytes to be read
 *
 * @return                  true on success
*/
int16_t _bq27441_read_bytes_i2c(const bq27441_t *dev, uint8_t subAddress, uint8_t * dest, uint8_t count);

/**
 * @brief       Write a specified number of bytes over I2C to a given subAddress
 *
 * @param[in]   *dev        pointer to bq27441_t struct containing the i2c device and the address
 * @param[in]   subAddress  8-bit address of the data to be written to
 * @param[in]   src         data buffer to be written
 * @param[in]   count       number of bytes to be written
 *
 * @return true on success
*/
uint16_t _bq27441_write_bytes_i2c(const bq27441_t *dev, uint8_t subAddress, uint8_t * src, uint8_t count);

#endif /* BQ27441_H */
/** @} */
