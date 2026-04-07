/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmi270_legacy.h"
#include "common.h"

/******************************************************************************/
/*!            Function                                                       */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Sensor initialization configuration. */
    struct bmi2_dev dev;

    /* Structure to define the feature configuration. */
    struct bmi2_sens_config config;

    /* Accel sensor and flat feature are listed in array. */
    uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_FLAT };

    /* Variable to get flat interrupt status. */
    uint16_t int_status = 0;

    /* Select features and their pins to be mapped to. */
    struct bmi2_sens_int_config sens_int = { .type = BMI2_FLAT, .hw_int_pin = BMI2_INT1 };

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&dev, BMI2_SPI_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270_legacy */
    rslt = bmi270_legacy_init(&dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Enable the selected sensors. */
        rslt = bmi270_legacy_sensor_enable(sens_list, 2, &dev);
        bmi2_error_codes_print_result(rslt);

        /* Configure the type of feature. */
        config.type = BMI2_FLAT;

        if (rslt == BMI2_OK)
        {
            /* Get default configurations for the type of feature selected. */
            rslt = bmi270_legacy_get_sensor_config(&config, 1, &dev);
            bmi2_error_codes_print_result(rslt);

            if (rslt == BMI2_OK)
            {
                /* Map the feature interrupt for flat. */
                rslt = bmi270_legacy_map_feat_int(&sens_int, 1, &dev);
                bmi2_error_codes_print_result(rslt);

                printf("Keep the board flat \n");

                /* Loop to get flat interrupt status. */
                do
                {
                    /* Clear buffer. */
                    int_status = 0;

                    /* To get the interrupt status of flat. */
                    rslt = bmi2_get_int_status(&int_status, &dev);
                    bmi2_error_codes_print_result(rslt);

                    /* To check the interrupt status of flat. */
                    if (int_status & BMI270_LEGACY_FLAT_STATUS_MASK)
                    {
                        printf("Flat interrupt is generated\n");
                        break;
                    }
                } while (rslt == BMI2_OK);
            }
        }
    }

    bmi2_coines_deinit();

    return rslt;
}
