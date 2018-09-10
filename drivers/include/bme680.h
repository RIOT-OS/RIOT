/**
 * Copyright (C) 2018 Mesotic SAS
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 *
 */

#ifndef BME680_H_
#define BME680_H_

/** CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif

/* Header includes */
#include "assert.h"
#include "periph/i2c.h"
#include "bme680_internals.h"


/**
 * @brief BME680 error code enum
 */
typedef enum {
	BME680_OK = 0,
	BME680_ERR,
	BME680_COM_FAIL,
	BME680_BAD_ID,
	BME680_NO_DATA,
} bme680_ret_t;

/**
 * @brief BME680 parameters structure
 */
typedef struct {
    i2c_t i2c_dev;                      /**< I2C device which is used */
    uint8_t i2c_addr;                   /**< I2C address */
} bme680_params_t;

/**
 * @brief Structure to hold the Calibration data
 */
typedef struct {
	uint16_t par_h1; 			/**< Calibrated humidity data */
	uint16_t par_h2; 			/**< Calibrated humidity data */
	int8_t par_h3; 				/**< Calibrated humidity data */
	int8_t par_h4; 				/**< Calibrated humidity data */
	int8_t par_h5; 				/**< Calibrated humidity data */
	uint8_t par_h6; 			/**< Calibrated humidity data */
	int8_t par_h7; 				/**< Calibrated humidity data */
	int8_t par_gh1; 			/**< Calibrated gas data */
	int16_t par_gh2;			/**< Calibrated gas data */
	int8_t par_gh3;				/**< Calibrated gas data */
	uint16_t par_t1; 			/**< Calibrated temperature data */
	int16_t par_t2;				/**< Calibrated temperature data */
	int8_t par_t3;				/**< Calibrated temperature data */
	uint16_t par_p1; 			/**< Calibrated pressure data */
	int16_t par_p2;				/**< Calibrated pressure data */
	int8_t par_p3;				/**< Calibrated pressure data */
	int16_t par_p4;				/**< Calibrated pressure data */
	int16_t par_p5;				/**< Calibrated pressure data */
	int8_t par_p6;				/**< Calibrated pressure data */
	int8_t par_p7;				/**< Calibrated pressure data */
	int16_t par_p8;				/**< Calibrated pressure data */
	int16_t par_p9;				/**< Calibrated pressure data */
	uint8_t par_p10;			/**< Calibrated pressure data */
	int32_t t_fine; 			/**< t_fine size */
	uint8_t res_heat_range; 	/**< Heater resistance range */
	int8_t res_heat_val; 		/**< Heater resistance value */
	int8_t range_sw_err; 		/**< Error range */
} bme680_calibration_t;

/**
 * @brief   Device descriptor for the BME680 sensor
 */
typedef struct {
    bme680_params_t* params;            /**< Device Parameters */
    bme680_calibration_t calib;   		/**< Calibration Data */
	uint8_t power_mode;                 /**< Sensor power modes */
	int8_t amb_temp;					/**< Ambient temperature */
	bme680_tph_sett_t tph_sett;    		/**< Sensor settings */
	bme680_gas_sett_t gas_sett;    		/**< Gas Sensor settings */
} bme680_t;

/* Function prototype declarations */

/**
 *  @brief This API is the entry point.
 *  It reads the chip-id and calibration data from the sensor.
 *
 *  @param[in] dev			Structure instance of bme680_dev
 *  @param[in] params		Structure
 *
 *  @return 				BME680_OK when success
 *  @return 				-BME680_COM_FAIL when I2C communication fails
 * 	@return					-BME680_BAD_ID when ID is wrong
 *  @return
 */
int8_t bme680_init(bme680_t *dev, const bme680_params_t *params);

/**
 * @brief This API writes the given data to the register address
 * of the sensor.
 *
 * @param[in] dev 	 	Structure instance of bme680_dev.
 * @param[in] reg_addr  Register address from where the data to be written.
 * @param[in] reg_data 	Pointer to data buffer which is to be written
 * in the sensor.
 * @param[in] len 		Number of bytes of data to write.
 *
 * @return 				BME680_OK when success
 * @return 				-BME680_COM_FAIL when I2C communication fails
 */
int8_t bme680_set_regs(bme680_t *dev, const uint8_t reg_addr, 
                       const uint8_t *reg_data, uint8_t len);

/**
 * @brief This API reads the data from the given register address of the sensor.
 * 
 * @param[in] dev 		 	Structure instance of bme680_dev.
 * @param[in] reg_addr 	 	Register address from where the data to be read
 * @param[out] reg_data  	Pointer to data buffer to store the read data.
 * @param[in] len 		 	No of bytes of data to be read.
 *
 * @return 				BME680_OK when success
 * @return 				-BME680_COM_FAIL when I2C communication fails
 */
int8_t bme680_get_regs(bme680_t *dev, uint8_t reg_addr, uint8_t *reg_data, 
                       uint16_t len);

/**
 * @brief This API performs the soft reset of the sensor.
 *
 * @param[in] dev   	Structure instance of bme680_dev.
 *
 * @return 				BME680_OK when success
 * @return 				-BME680_COM_FAIL when I2C communication fails
 */
int8_t bme680_soft_reset(bme680_t *dev);

/**
 * @brief This API is used to set the power mode of the sensor.
 *
 * @param[in] dev   Structure instance of bme680_dev
 *
 * @return 				BME680_OK when success
 * @return 				-BME680_COM_FAIL when I2C communication fails
 */
int8_t bme680_set_sensor_mode(bme680_t *dev, uint8_t mode);

/**
 * @brief This API is used to get the power mode of the sensor.
 *
 * @param[in] dev  Structure instance of bme680_dev
 *
 * @return 				BME680_OK when success
 * @return 				-BME680_COM_FAIL when I2C communication fails
 */
int8_t bme680_get_sensor_mode(bme680_t *dev);

/**
 * @brief This API is used to set the profile duration of the sensor.
 *
 * @param[in] dev	     Structure instance of bme680_dev.
 * @param[in] duration   Duration of the measurement in ms.
 *
 */
void bme680_set_profile_dur(bme680_t *dev, uint16_t duration);

/**
 * @brief This API is used to get the profile duration of the sensor.
 *
 * @param[in] dev	     Structure instance of bme680_dev.
 * @param[in] duration   Duration of the measurement in ms.
 *
 */
void bme680_get_profile_dur(bme680_t *dev, uint32_t *duration);

/**
 * @brief This API reads the pressure, temperature and humidity and gas data
 * from the sensor, compensates the data and store it in the bme680_data
 * structure instance passed by the user.
 *
 * @param[in] dev    Structure instance of bme680_dev.
 * @param[out] data  Structure instance to hold the data.

 *
 * @return 				BME680_OK when success
 * @return 				-BME680_COM_FAIL when I2C communication fails
 * @return 				BME680_NO_DATA when no new data are presents
 */
int8_t bme680_get_sensor_data(bme680_t *dev, bme680_data_t *data);

/**
 * @brief This API is used to set the oversampling, filter and T,P,H, gas 
 * selection settings in the sensor.
 *
 * @param[in] dev   	Structure instance of bme680_dev.
 * @param[in] flags   	Variable used to select the settings which
 * are to be set in the sensor.
 *
 *	 Macros	                   |  Functionality
 *-----------------------------|--------------------------------------------
 *	BME680_OST_SEL             |    To set temperature oversampling.
 *	BME680_OSP_SEL             |    To set pressure oversampling.
 *	BME680_OSH_SEL             |    To set humidity oversampling.
 *	BME680_GAS_MEAS_SEL        |    To set gas measurement setting.
 *	BME680_FILTER_SEL          |    To set filter setting.
 *	BME680_HCNTRL_SEL          |    To set humidity control setting.
 *	BME680_RUN_GAS_SEL         |    To set run gas setting.
 *	BME680_NBCONV_SEL          |    To set NB conversion setting.
 *	BME680_GAS_SENSOR_SEL      |    To set all gas sensor related settings
 *
 * @note : Below are the macros to be used by the user for selecting the
 * desired settings. User can do OR operation of these macros for configuring
 * multiple settings.
 *
 * @return 				BME680_OK when success
 * @return 				-BME680_COM_FAIL when I2C communication fails
 */
int8_t bme680_set_sensor_settings(bme680_t *dev, uint16_t flags);

/**
 * @brief This API is used to get the oversampling, filter and T,P,H, gas 
 * selection settings in the sensor.
 *
 * @param[in] dev  		 Structure instance of bme680_dev.
 * @param[in] flags		 Variable used to select the settings which
 * are to be get from the sensor.
 *
 * @return 				BME680_OK when success
 * @return 				-BME680_COM_FAIL when I2C communication fails
 */
int8_t bme680_get_sensor_settings(bme680_t *dev);
#ifdef __cplusplus
}
#endif

#endif /* BME680_H_ */
/** @}*/
