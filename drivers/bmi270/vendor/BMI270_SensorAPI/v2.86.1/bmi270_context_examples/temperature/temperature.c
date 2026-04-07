/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmi270_context.h"
#include "common.h"

/******************************************************************************/
/*!         Static Function Declaration                                       */

/*!
 *  @brief This internal API is used to set configurations for gyro.
 *
 *  @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_gyro_config(struct bmi2_dev *dev);

/******************************************************************************/
/*!            Functions                                        */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Variable to define limit to print temperature data. */
    uint8_t limit = 50;

    uint8_t indx = 0;

    /* Sensor initialization configuration. */
    struct bmi2_dev bmi;

    /* Assign gyro sensor to variable. */
    uint8_t sens_list[2] = { BMI2_GYRO, BMI2_TEMP };

    uint16_t temperature_data;

    /* Variable to store temperature */
    float temperature_value;

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&bmi, BMI2_SPI_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270_context. */
    rslt = bmi270_context_init(&bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Gyro configuration settings. */
        rslt = set_gyro_config(&bmi);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {
            /* NOTE:
             * Gyro and Temperature enable must be done after setting configurations
             */

            /* Enable the selected sensors. */
            rslt = bmi2_sensor_enable(sens_list, 2, &bmi);
            bmi2_error_codes_print_result(rslt);

            printf("\nSample, Temperature(Degree C)\n");

            while (indx <= limit)
            {
                rslt = bmi2_get_temperature_data(&temperature_data, &bmi);
                bmi2_error_codes_print_result(rslt);

                temperature_value = (float)((((float)((int16_t)temperature_data)) / 512.0) + 23.0);

                printf("%d, %f\n", indx, temperature_value);

                /* As per datasheet, a delay of 10ms is required to get temperature data when gyro is in normal mode */
                bmi.delay_us(10000, bmi.intf_ptr);

                indx++;
            }
        }
    }

    bmi2_coines_deinit();

    return rslt;
}

/*!
 *  @brief This internal API is used to set configurations for gyro.
 */
static int8_t set_gyro_config(struct bmi2_dev *dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi2_sens_config config;

    /* Configure the type of feature. */
    config.type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(&config, 1, dev);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT2, dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config.cfg.gyr.odr = BMI2_GYR_ODR_100HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config.cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config.cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config.cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config.cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the gyro configurations. */
        rslt = bmi2_set_sensor_config(&config, 1, dev);
    }

    return rslt;
}
