/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include <math.h>
#include "bmi270.h"
#include "common.h"
#include "coines.h"

/******************************************************************************/
/*!           Global variable Declaration                                     */

volatile uint8_t drdy_int_status = 0;

/******************************************************************************/
/*!                Macro definition                                           */

/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH    (9.80665f)

/*! Macros to select the sensors                   */
#define ACCEL            UINT8_C(0x00)
#define GYRO             UINT8_C(0x01)

/*! APP20 Board number */
#define BOARD_MCU_APP20  UINT8_C(0x03)

/*! APP30 Board number */
#define BOARD_MCU_APP30  UINT8_C(0x05)

/******************************************************************************/
/*!           Static Function Declaration                                     */

/*!
 *  @brief This internal API is used to set configurations for accel.
 *
 *  @param[in] bmi       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi);

/*!
 *  @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 *  range 2G, 4G, 8G or 16G.
 *
 *  @param[in] val       : LSB from each axis.
 *  @param[in] g_range   : Gravity range.
 *  @param[in] bit_width : Resolution for accel.
 *
 *  @return Accel values in meter per second squared.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width);

/*!
 *  @brief This function converts lsb to degree per second for 16 bit gyro at
 *  range 125, 250, 500, 1000 or 2000dps.
 *
 *  @param[in] val       : LSB from each axis.
 *  @param[in] dps       : Degree per second.
 *  @param[in] bit_width : Resolution for gyro.
 *
 *  @return Degree per second.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);

/*!
 * @brief This function gets board information
 *
 * @param[out] board       : Board value to determine as APP2.0 or APP3.0
 */
static void get_board_info(uint8_t *board);

/******************************************************************************/
/*!            Functions                                        */

/*!
 * @brief This internal API is used to set the data ready interrupt status
 */
static void drdy_int_callback(uint32_t param1, uint32_t param2)
{
    (void)param1;
    (void)param2;
    drdy_int_status = 1;
}

/* This function starts the execution of program. */
int main(void)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Variable to define limit to print accel data. */
    uint8_t limit = 100;

    /* Assign accel and gyro sensor to variable. */
    uint8_t sensor_list[2] = { BMI2_ACCEL, BMI2_GYRO };

    /* Sensor initialization configuration. */
    struct bmi2_dev bmi;

    /* Structure to define type of sensor and their respective data. */
    struct bmi2_sens_data sensor_data = { { 0 } };

    uint8_t indx = 1;
    uint8_t board = 0;

    float acc_x = 0, acc_y = 0, acc_z = 0;
    float gyr_x = 0, gyr_y = 0, gyr_z = 0;
    struct bmi2_sens_config config;

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&bmi, BMI2_I2C_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270. */
    rslt = bmi270_init(&bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Accel and gyro configuration settings. */
        rslt = set_accel_gyro_config(&bmi);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {
            /* NOTE:
             * Accel and Gyro enable must be done after setting configurations
             */
            rslt = bmi2_sensor_enable(sensor_list, 2, &bmi);
            bmi2_error_codes_print_result(rslt);

            /* Get board information */
            get_board_info(&board);

            /*
             * Attach interrupt based on board
             * Note : Interrupt pin to be given based on pin_type
             */
            if (board == BOARD_MCU_APP20)
            {
                coines_attach_interrupt(COINES_SHUTTLE_PIN_20, drdy_int_callback, COINES_PIN_INTERRUPT_FALLING_EDGE);
            }

#if !defined(MCU_APP20)
            else if (board == BOARD_MCU_APP30)
            {
                coines_attach_interrupt(COINES_MINI_SHUTTLE_PIN_1_6,
                                        drdy_int_callback,
                                        COINES_PIN_INTERRUPT_FALLING_EDGE);
            }
#endif
            if (rslt == BMI2_OK)
            {
                config.type = BMI2_ACCEL;

                /* Get the accel configurations. */
                rslt = bmi2_get_sensor_config(&config, 1, &bmi);
                bmi2_error_codes_print_result(rslt);

                printf(
                    "\nData set, Accel Range, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Acc_ms2_X, Acc_ms2_Y, Acc_ms2_Z, Gyr_Raw_X, Gyr_Raw_Y, Gyr_Raw_Z, Gyro_DPS_X, Gyro_DPS_Y, Gyro_DPS_Z\n\n");

                /* Loop to print accel and gyro data when interrupt occurs. */
                while (indx <= limit)
                {
                    if (drdy_int_status == 1)
                    {
                        drdy_int_status = 0;

                        /* To get the data ready interrupt status of accel and gyro. */
                        rslt = bmi2_get_sensor_data(&sensor_data, &bmi);
                        bmi2_error_codes_print_result(rslt);

                        if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC) &&
                            (sensor_data.status & BMI2_DRDY_GYR))
                        {
                            /* Converting lsb to meter per second squared for 16 bit accelerometer at 2G range. */
                            acc_x = lsb_to_mps2(sensor_data.acc.x, (float)2, bmi.resolution);
                            acc_y = lsb_to_mps2(sensor_data.acc.y, (float)2, bmi.resolution);
                            acc_z = lsb_to_mps2(sensor_data.acc.z, (float)2, bmi.resolution);

                            /* Converting lsb to degree per second for 16 bit gyro at 2000dps range. */
                            gyr_x = lsb_to_dps(sensor_data.gyr.x, (float)2000, bmi.resolution);
                            gyr_y = lsb_to_dps(sensor_data.gyr.y, (float)2000, bmi.resolution);
                            gyr_z = lsb_to_dps(sensor_data.gyr.z, (float)2000, bmi.resolution);

                            printf("%d, %d, %d, %d, %d, %4.2f, %4.2f, %4.2f, %d, %d, %d, %4.2f, %4.2f, %4.2f\n",
                                   indx,
                                   config.cfg.acc.range,
                                   sensor_data.acc.x,
                                   sensor_data.acc.y,
                                   sensor_data.acc.z,
                                   acc_x,
                                   acc_y,
                                   acc_z,
                                   sensor_data.gyr.x,
                                   sensor_data.gyr.y,
                                   sensor_data.gyr.z,
                                   gyr_x,
                                   gyr_y,
                                   gyr_z);

                            indx++;
                        }
                    }
                }
            }
        }
    }

    bmi2_coines_deinit();

    return rslt;
}

/*!
 * @brief This internal API is used to set configurations for accel and gyro.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer and gyro configuration. */
    struct bmi2_sens_config config[2];

    /* Interrupt pin configuration */
    struct bmi2_int_pin_config pin_config = { 0 };

    /* Configure the type of feature. */
    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;

    /* Get default configuration for hardware Interrupt */
    rslt = bmi2_get_int_pin_config(&pin_config, bmi);
    bmi2_error_codes_print_result(rslt);

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(config, 2, bmi);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config[ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_200HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config[GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config[GYRO].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Interrupt pin configuration */
        pin_config.pin_type = BMI2_INT1;
        pin_config.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;
        pin_config.pin_cfg[0].lvl = BMI2_INT_ACTIVE_LOW;
        pin_config.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
        pin_config.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
        pin_config.int_latch = BMI2_INT_NON_LATCH;

        /* Set Hardware interrupt pin configuration */
        rslt = bmi2_set_int_pin_config(&pin_config, bmi);
        bmi2_error_codes_print_result(rslt);

        /* Set the accel and gyro configurations. */
        rslt = bmi2_set_sensor_config(config, 2, bmi);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}

/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (dps / (half_scale)) * (val);
}

/*!
 * @brief This function gets board information
 */
static void get_board_info(uint8_t *board)
{
    struct coines_board_info board_info;
    int16_t result;

    result = coines_get_board_info(&board_info);

    if (result == COINES_SUCCESS)
    {
        (*board) = board_info.board;
    }
}
