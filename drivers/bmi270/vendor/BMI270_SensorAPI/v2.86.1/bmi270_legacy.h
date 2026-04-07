/**
* Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file       bmi270_legacy.h
* @date       2023-05-03
* @version    v2.86.1
*
*/

/* BMI270_legacy offers same low-power features as in BMI160 */

/**
 * \ingroup bmi2xy
 * \defgroup bmi270_legacy BMI270_LEGACY
 * @brief Sensor driver for BMI270_LEGACY sensor
 */

#ifndef BMI270_LEGACY_H_
#define BMI270_LEGACY_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/

/*!             Header files
 ****************************************************************************/
#include "bmi2.h"

/***************************************************************************/

/*!               Macro definitions
 ****************************************************************************/

/*! @name BMI270_LEGACY chip identifier */
#define BMI270_LEGACY_CHIP_ID                         UINT8_C(0x24)

/*! @name BMI270_LEGACY feature input start addresses */
#define BMI270_LEGACY_CONFIG_ID_STRT_ADDR             UINT8_C(0x00)
#define BMI270_LEGACY_MAX_BURST_LEN_STRT_ADDR         UINT8_C(0x02)
#define BMI270_LEGACY_CRT_GYRO_SELF_TEST_STRT_ADDR    UINT8_C(0x03)
#define BMI270_LEGACY_ABORT_STRT_ADDR                 UINT8_C(0x03)
#define BMI270_LEGACY_AXIS_MAP_STRT_ADDR              UINT8_C(0x04)
#define BMI270_LEGACY_GYRO_SELF_OFF_STRT_ADDR         UINT8_C(0x05)
#define BMI270_LEGACY_NVM_PROG_PREP_STRT_ADDR         UINT8_C(0x05)
#define BMI270_LEGACY_ANY_MOT_STRT_ADDR               UINT8_C(0x06)
#define BMI270_LEGACY_NO_MOT_STRT_ADDR                UINT8_C(0x0A)
#define BMI270_LEGACY_ORIENT_STRT_ADDR                UINT8_C(0x00)
#define BMI270_LEGACY_HIGH_G_STRT_ADDR                UINT8_C(0x04)
#define BMI270_LEGACY_LOW_G_STRT_ADDR                 UINT8_C(0x0A)
#define BMI270_LEGACY_FLAT_STRT_ADDR                  UINT8_C(0x00)
#define BMI270_LEGACY_SIG_MOT_STRT_ADDR               UINT8_C(0x04)
#define BMI270_LEGACY_STEP_COUNT_STRT_ADDR            UINT8_C(0x00)
#define BMI270_LEGACY_GYRO_USERGAIN_UPDATE_STRT_ADDR  UINT8_C(0x04)
#define BMI270_LEGACY_TAP_DETECT_1_STRT_ADDR          UINT8_C(0x00)
#define BMI270_LEGACY_TAP_DETECT_2_STRT_ADDR          UINT8_C(0x00)

/*! @name BMI270_LEGACY feature output start addresses */
#define BMI270_LEGACY_STEP_CNT_OUT_STRT_ADDR          UINT8_C(0x00)
#define BMI270_LEGACY_STEP_ACT_OUT_STRT_ADDR          UINT8_C(0x04)
#define BMI270_LEGACY_ORIENT_HIGH_G_OUT_STRT_ADDR     UINT8_C(0x06)
#define BMI270_LEGACY_GYR_USER_GAIN_OUT_STRT_ADDR     UINT8_C(0x08)
#define BMI270_LEGACY_GYRO_CROSS_SENSE_STRT_ADDR      UINT8_C(0x0C)
#define BMI270_LEGACY_NVM_VFRM_OUT_STRT_ADDR          UINT8_C(0x0E)

/*! @name Defines maximum number of pages */
#define BMI270_LEGACY_MAX_PAGE_NUM                    UINT8_C(8)

/*! @name Defines maximum number of feature input configurations */
#define BMI270_LEGACY_MAX_FEAT_IN                     UINT8_C(23)

/*! @name Defines maximum number of feature outputs */
#define BMI270_LEGACY_MAX_FEAT_OUT                    UINT8_C(8)

/*! @name Mask definitions for feature interrupt status bits */
#define BMI270_LEGACY_SIG_MOT_STATUS_MASK             UINT8_C(0x01)
#define BMI270_LEGACY_STEP_CNT_STATUS_MASK            UINT8_C(0x02)
#define BMI270_LEGACY_HIGH_G_STATUS_MASK              UINT8_C(0x04)
#define BMI270_LEGACY_LOW_G_STATUS_MASK               UINT8_C(0x04)
#define BMI270_LEGACY_TAP_STATUS_MASK                 UINT8_C(0x08)
#define BMI270_LEGACY_FLAT_STATUS_MASK                UINT8_C(0x10)
#define BMI270_LEGACY_NO_MOT_STATUS_MASK              UINT8_C(0x20)
#define BMI270_LEGACY_ANY_MOT_STATUS_MASK             UINT8_C(0x40)
#define BMI270_LEGACY_ORIENT_STATUS_MASK              UINT8_C(0x80)
#define BMI270_LEGACY_SINGLE_TAP_MASK                 UINT8_C(0x20)
#define BMI270_LEGACY_DOUBLE_TAP_MASK                 UINT8_C(0x40)
#define BMI270_LEGACY_TRIPLE_TAP_MASK                 UINT8_C(0x80)

/*! @name Status register for tap */
#define BMI270_LEGACY_TAP_STATUS_REG                  UINT8_C(0x20)

/*! @name Macros for high-g interrupt axis/sign */
#define BMI270_LEGACY_HIGH_G_DETECT_X                 UINT8_C(0x08)
#define BMI270_LEGACY_HIGH_G_DETECT_Y                 UINT8_C(0x10)
#define BMI270_LEGACY_HIGH_G_DETECT_Z                 UINT8_C(0x20)
#define BMI270_LEGACY_HIGH_G_DETECT_SIGN              UINT8_C(0x40)

/*! @name Mask definitions for feature interrupt mapping bits */
#define BMI270_LEGACY_INT_SIG_MOT_MASK                UINT8_C(0x01)
#define BMI270_LEGACY_INT_STEP_COUNTER_MASK           UINT8_C(0x02)
#define BMI270_LEGACY_INT_STEP_DETECTOR_MASK          UINT8_C(0x02)
#define BMI270_LEGACY_INT_STEP_ACTIVITY_MASK          UINT8_C(0x02)
#define BMI270_LEGACY_INT_HIGH_G_MASK                 UINT8_C(0x04)
#define BMI270_LEGACY_INT_LOW_G_MASK                  UINT8_C(0x04)
#define BMI270_LEGACY_INT_TAP_MASK                    UINT8_C(0x08)
#define BMI270_LEGACY_INT_FLAT_MASK                   UINT8_C(0x10)
#define BMI270_LEGACY_INT_NO_MOT_MASK                 UINT8_C(0x20)
#define BMI270_LEGACY_INT_ANY_MOT_MASK                UINT8_C(0x40)
#define BMI270_LEGACY_INT_ORIENT_MASK                 UINT8_C(0x80)
#define BMI270_LEGACY_INT_SINGLE_TAP_MASK             UINT8_C(0x20)
#define BMI270_LEGACY_INT_DOUBLE_TAP_MASK             UINT8_C(0x40)
#define BMI270_LEGACY_INT_TRIPLE_TAP_MASK             UINT8_C(0x80)

/*! @name Defines maximum number of feature interrupts */
#define BMI270_LEGACY_MAX_INT_MAP                     UINT8_C(14)

/***************************************************************************/

/*!     BMI270_LEGACY User Interface function prototypes
 ****************************************************************************/

/**
 * \ingroup bmi270_legacy
 * \defgroup bmi270_legacyApiInit Initialization
 * @brief Initialize the sensor and device structure
 */

/*!
 * \ingroup bmi270_legacyApiInit
 * \page bmi270_legacy_api_bmi270_legacy_init bmi270_legacy_init
 * \code
 * int8_t bmi270_legacy_init(struct bmi2_dev *dev);
 * \endcode
 * @details This API:
 *  1) updates the device structure with address of the configuration file.
 *  2) Initializes BMI270_LEGACY sensor.
 *  3) Writes the configuration file.
 *  4) Updates the feature offset parameters in the device structure.
 *  5) Updates the maximum number of pages, in the device structure.
 *
 * @param[in, out] dev      : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_init(struct bmi2_dev *dev);

/**
 * \ingroup bmi270_legacy
 * \defgroup bmi270_legacyApiSensor Feature Set
 * @brief Enable / Disable features of the sensor
 */

/*!
 * \ingroup bmi270_legacyApiSensor
 * \page bmi270_legacy_api_bmi270_legacy_sensor_enable bmi270_legacy_sensor_enable
 * \code
 * int8_t bmi270_legacy_sensor_enable(const uint8_t *sens_list, uint8_t n_sens, struct bmi2_dev *dev);
 * \endcode
 * @details This API selects the sensors/features to be enabled.
 *
 * @param[in]       sens_list   : Pointer to select the sensor/feature.
 * @param[in]       n_sens      : Number of sensors selected.
 * @param[in, out]  dev         : Structure instance of bmi2_dev.
 *
 * @note Sensors/features that can be enabled.
 *
 *@verbatim
 *    sens_list                |  Values
 * ----------------------------|-----------
 * BMI2_ACCEL                  |  0
 * BMI2_GYRO                   |  1
 * BMI2_AUX                    |  2
 * BMI2_SIG_MOTION             |  3
 * BMI2_ANY_MOTION             |  4
 * BMI2_NO_MOTION              |  5
 * BMI2_STEP_DETECTOR          |  6
 * BMI2_STEP_COUNTER           |  7
 * BMI2_STEP_ACTIVITY          |  8
 * BMI2_GYRO_GAIN_UPDATE       |  9
 * BMI2_ORIENTATION            |  14
 * BMI2_HIGH_G                 |  15
 * BMI2_LOW_G                  |  16
 * BMI2_FLAT                   |  17
 * BMI2_SINGLE_TAP             |  25
 * BMI2_DOUBLE_TAP             |  26
 * BMI2_TRIPLE_TAP             |  27
 * BMI2_TEMP                   |  31
 *@endverbatim
 *
 * @note :
 * example  uint8_t sens_list[2]  = {BMI2_ACCEL, BMI2_GYRO};
 *           uint8_t n_sens        = 2;
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_sensor_enable(const uint8_t *sens_list, uint8_t n_sens, struct bmi2_dev *dev);

/*!
 * \ingroup bmi270_legacyApiSensor
 * \page bmi270_legacy_api_bmi270_legacy_sensor_disable bmi270_legacy_sensor_disable
 * \code
 * int8_t bmi270_legacy_sensor_disable(const uint8_t *sens_list, uint8_t n_sens, struct bmi2_dev *dev);
 * \endcode
 * @details This API selects the sensors/features to be disabled.
 *
 * @param[in]       sens_list   : Pointer to select the sensor/feature.
 * @param[in]       n_sens      : Number of sensors selected.
 * @param[in, out]  dev         : Structure instance of bmi2_dev.
 *
 * @note Sensors/features that can be disabled.
 *
 *@verbatim
 *    sens_list                |  Values
 * ----------------------------|-----------
 * BMI2_ACCEL                  |  0
 * BMI2_GYRO                   |  1
 * BMI2_AUX                    |  2
 * BMI2_SIG_MOTION             |  3
 * BMI2_ANY_MOTION             |  4
 * BMI2_NO_MOTION              |  5
 * BMI2_STEP_DETECTOR          |  6
 * BMI2_STEP_COUNTER           |  7
 * BMI2_STEP_ACTIVITY          |  8
 * BMI2_GYRO_GAIN_UPDATE       |  9
 * BMI2_ORIENTATION            |  14
 * BMI2_HIGH_G                 |  15
 * BMI2_LOW_G                  |  16
 * BMI2_FLAT                   |  17
 * BMI2_SINGLE_TAP             |  25
 * BMI2_DOUBLE_TAP             |  26
 * BMI2_TRIPLE_TAP             |  27
 * BMI2_TEMP                   |  31
 *@endverbatim
 *
 * @note :
 * example  uint8_t sens_list[2]  = {BMI2_ACCEL, BMI2_GYRO};
 *           uint8_t n_sens        = 2;
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_sensor_disable(const uint8_t *sens_list, uint8_t n_sens, struct bmi2_dev *dev);

/**
 * \ingroup bmi270_legacy
 * \defgroup bmi270_legacyApiSensorC Sensor Configuration
 * @brief Enable / Disable feature configuration of the sensor
 */

/*!
 * \ingroup bmi270_legacyApiSensorC
 * \page bmi270_legacy_api_bmi270_legacy_set_sensor_config bmi270_legacy_set_sensor_config
 * \code
 * int8_t bmi270_legacy_set_sensor_config(struct bmi2_sens_config *sens_cfg, uint8_t n_sens, struct bmi2_dev *dev);
 * \endcode
 * @details This API sets the sensor/feature configuration.
 *
 * @param[in]       sens_cfg     : Structure instance of bmi2_sens_config.
 * @param[in]       n_sens       : Number of sensors selected.
 * @param[in, out]  dev          : Structure instance of bmi2_dev.
 *
 * @note Sensors/features that can be configured
 *
 *@verbatim
 *    sens_list                |  Values
 * ----------------------------|-----------
 * BMI2_SIG_MOTION             |  3
 * BMI2_ANY_MOTION             |  4
 * BMI2_NO_MOTION              |  5
 * BMI2_STEP_DETECTOR          |  6
 * BMI2_STEP_COUNTER           |  7
 * BMI2_STEP_ACTIVITY          |  8
 * BMI2_ORIENTATION            |  14
 * BMI2_HIGH_G                 |  15
 * BMI2_LOW_G                  |  16
 * BMI2_FLAT                   |  17
 * BMI2_TAP                    |  28
 *@endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_set_sensor_config(struct bmi2_sens_config *sens_cfg, uint8_t n_sens, struct bmi2_dev *dev);

/*!
 * \ingroup bmi270_legacyApiSensorC
 * \page bmi270_legacy_api_bmi270_legacy_get_sensor_config bmi270_legacy_get_sensor_config
 * \code
 * int8_t bmi270_legacy_get_sensor_config(struct bmi2_sens_config *sens_cfg, uint8_t n_sens, struct bmi2_dev *dev);
 * \endcode
 * @details This API gets the sensor/feature configuration.
 *
 * @param[in]       sens_cfg     : Structure instance of bmi2_sens_config.
 * @param[in]       n_sens       : Number of sensors selected.
 * @param[in, out]  dev          : Structure instance of bmi2_dev.
 *
 * @note Sensors/features whose configurations can be read.
 *
 *@verbatim
 *  sens_list                  |  Values
 * ----------------------------|-----------
 * BMI2_SIG_MOTION             |  3
 * BMI2_ANY_MOTION             |  4
 * BMI2_NO_MOTION              |  5
 * BMI2_STEP_DETECTOR          |  6
 * BMI2_STEP_COUNTER           |  7
 * BMI2_STEP_ACTIVITY          |  8
 * BMI2_ORIENTATION            |  14
 * BMI2_HIGH_G                 |  15
 * BMI2_LOW_G                  |  16
 * BMI2_FLAT                   |  17
 * BMI2_TAP                    |  28
 *@endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_get_sensor_config(struct bmi2_sens_config *sens_cfg, uint8_t n_sens, struct bmi2_dev *dev);

/**
 * \ingroup bmi270_legacy
 * \defgroup bmi270_legacyApiSensorD Feature Sensor Data
 * @brief Get feature sensor data
 */

/*!
 * \ingroup bmi270_legacyApiSensorD
 * \page bmi270_legacy_api_bmi270_legacy_get_feature_data bmi270_legacy_get_feature_data
 * \code
 * int8_t bmi270_legacy_get_feature_data(struct bmi2_feat_sensor_data *feature_data, uint8_t n_sens, struct bmi2_dev *dev);
 * \endcode
 * @details This API gets the feature data.
 *
 * @param[out] feature_data   : Structure instance of bmi2_feat_sensor_data.
 * @param[in]  n_sens         : Number of sensors selected.
 * @param[in]  dev            : Structure instance of bmi2_dev.
 *
 * @note Sensors/features whose data can be read
 *
 *@verbatim
 *  sens_list           |  Values
 * ---------------------|-----------
 * BMI2_STEP_COUNTER    |  7
 * BMI2_STEP_ACTIVITY   |  8
 * BMI2_ORIENTATION     |  14
 * BMI2_HIGH_G          |  15
 * BMI2_NVM_STATUS      |  38
 * BMI2_VFRM_STATUS     |  39
 *@endverbatim
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_get_feature_data(struct bmi2_feat_sensor_data *feature_data, uint8_t n_sens, struct bmi2_dev *dev);

/**
 * \ingroup bmi270_legacy
 * \defgroup bmi270_legacyApiGyroUG Gyro User Gain
 * @brief Set / Get Gyro User Gain of the sensor
 */

/*!
 * \ingroup bmi270_legacyApiGyroUG
 * \page bmi270_legacy_api_bmi270_legacy_update_gyro_user_gain bmi270_legacy_update_gyro_user_gain
 * \code
 * int8_t bmi270_legacy_update_gyro_user_gain(const struct bmi2_gyro_user_gain_config *user_gain, struct bmi2_dev *dev);
 * \endcode
 * @details This API updates the gyroscope user-gain.
 *
 * @param[in] user_gain      : Structure that stores user-gain configurations.
 * @param[in] dev            : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_update_gyro_user_gain(const struct bmi2_gyro_user_gain_config *user_gain, struct bmi2_dev *dev);

/*!
 * \ingroup bmi270_legacyApiGyroUG
 * \page bmi270_legacy_api_bmi270_legacy_read_gyro_user_gain bmi270_legacy_read_gyro_user_gain
 * \code
 * int8_t bmi270_legacy_read_gyro_user_gain(struct bmi2_gyro_user_gain_data *gyr_usr_gain, const struct bmi2_dev *dev);
 * \endcode
 * @details This API reads the compensated gyroscope user-gain values.
 *
 * @param[out] gyr_usr_gain   : Structure that stores gain values.
 * @param[in]  dev            : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_read_gyro_user_gain(struct bmi2_gyro_user_gain_data *gyr_usr_gain, struct bmi2_dev *dev);

/*!
 * \ingroup bmi270_legacyApiInt
 * \page bmi270_legacy_api_bmi270_legacy_map_feat_int bmi270_legacy_map_feat_int
 * \code
 * int8_t bmi270_legacy_map_feat_int(const struct bmi2_sens_int_config *sens_int, uint8_t n_sens, struct bmi2_dev *dev)
 * \endcode
 * @details This API maps/unmaps feature interrupts to that of interrupt pins.
 *
 * @param[in] sens_int     : Structure instance of bmi2_sens_int_config.
 * @param[in] n_sens       : Number of interrupts to be mapped.
 * @param[in] dev          : Structure instance of bmi2_dev.
 *
 * @return Result of API execution status
 * @retval 0 -> Success
 * @retval < 0 -> Fail
 */
int8_t bmi270_legacy_map_feat_int(const struct bmi2_sens_int_config *sens_int, uint8_t n_sens, struct bmi2_dev *dev);

/******************************************************************************/
/*! @name       C++ Guard Macros                                      */
/******************************************************************************/
#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _BMI270_LEGACY_H_ */
