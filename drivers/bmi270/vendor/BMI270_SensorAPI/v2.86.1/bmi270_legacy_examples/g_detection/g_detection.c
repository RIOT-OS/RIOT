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

    /* Structure to define the type of sensor and its configurations. */
    struct bmi2_sens_config config[2];

    /* Accel sensor and high-g feature are listed in array. */
    uint8_t high_g_sens_list[2] = { BMI2_ACCEL, BMI2_HIGH_G };

    /* Accel sensor and low-g feature are listed in array. */
    uint8_t low_g_sens_list[2] = { BMI2_ACCEL, BMI2_LOW_G };

    /* Structure to define type of sensor and their respective data. */
    struct bmi2_feat_sensor_data sensor_data = { 0 };

    /* Variable to get high-g and low-g interrupt status. */
    uint16_t int_status = 0;

    /* Variables to store the output of high-g. */
    uint8_t high_g_out = 0;

    /* Select features and their pins to be mapped to. */
    struct bmi2_sens_int_config high_g_int = { .type = BMI2_HIGH_G, .hw_int_pin = BMI2_INT1 };
    struct bmi2_sens_int_config low_g_int = { .type = BMI2_LOW_G, .hw_int_pin = BMI2_INT2 };

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&dev, BMI2_I2C_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270_legacy. */
    rslt = bmi270_legacy_init(&dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Enable high-g feature. */
        rslt = bmi270_legacy_sensor_enable(high_g_sens_list, 2, &dev);
        bmi2_error_codes_print_result(rslt);

        /* Configure the type of feature. */
        config[0].type = BMI2_HIGH_G;
        config[1].type = BMI2_LOW_G;

        if (rslt == BMI2_OK)
        {
            /* Get default configurations for the type of feature selected. */
            rslt = bmi270_legacy_get_sensor_config(config, 2, &dev);
            bmi2_error_codes_print_result(rslt);

            if (rslt == BMI2_OK)
            {
                /* Map high_g feature interrupt to interrupt pin. */
                rslt = bmi270_legacy_map_feat_int(&high_g_int, 1, &dev);
                bmi2_error_codes_print_result(rslt);

                /* Sensor type of sensor to get data */
                sensor_data.type = BMI2_HIGH_G;

                /* To generate high-g interrupt */
                printf("\n\nMove the board fast towards upward direction\n");

                do
                {
                    /* Clear the buffer. */
                    int_status = 0;

                    /* To get the interrupt status of high-g. */
                    rslt = bmi2_get_int_status(&int_status, &dev);
                    bmi2_error_codes_print_result(rslt);

                    /* To check the interrupt status of high-g. */
                    if (int_status & BMI270_LEGACY_HIGH_G_STATUS_MASK)
                    {
                        printf("High-g interrupt is generated\n");

                        rslt = bmi270_legacy_get_feature_data(&sensor_data, 1, &dev);
                        bmi2_error_codes_print_result(rslt);

                        high_g_out = sensor_data.sens_data.high_g_output;

                        printf("The high-g output : 0x%x \n", high_g_out);

                        if (high_g_out & BMI270_LEGACY_HIGH_G_DETECT_X)
                        {
                            printf("High-g detected on X-Axis\n");
                        }

                        if (high_g_out & BMI270_LEGACY_HIGH_G_DETECT_Y)
                        {
                            printf("High-g detected on Y-Axis\n");
                        }

                        if (high_g_out & BMI270_LEGACY_HIGH_G_DETECT_Z)
                        {
                            printf("High-g detected on Z-Axis\n");
                        }

                        if (high_g_out & BMI270_LEGACY_HIGH_G_DETECT_SIGN)
                        {
                            printf("High-g detected on negative axis\n");
                        }

                        if ((high_g_out & BMI270_LEGACY_HIGH_G_DETECT_SIGN) == BMI2_DISABLE)
                        {
                            printf("High-g detected on positive axis\n");
                        }
                        else if ((high_g_out & BMI270_LEGACY_HIGH_G_DETECT_SIGN) &
                                 (~(high_g_out & BMI270_LEGACY_HIGH_G_DETECT_SIGN)))
                        {
                            printf("High-g detected on positive axis\n");
                        }

                        break;
                    }
                } while (rslt == BMI2_OK);

                /* Disable high-g feature. */
                rslt = bmi270_legacy_sensor_disable(high_g_sens_list, 2, &dev);
                bmi2_error_codes_print_result(rslt);

                /* Enable low-g feature. */
                rslt = bmi270_legacy_sensor_enable(low_g_sens_list, 2, &dev);
                bmi2_error_codes_print_result(rslt);

                /* Map low_g feature interrupt to interrupt pin. */
                rslt = bmi270_legacy_map_feat_int(&low_g_int, 1, &dev);
                bmi2_error_codes_print_result(rslt);

                /* To generate low-g interrupt */
                printf("\n\nDrop the board in free fall\n");

                do
                {
                    /* Clear the buffer. */
                    int_status = 0;

                    /* To get the interrupt status of low-g. */
                    rslt = bmi2_get_int_status(&int_status, &dev);
                    bmi2_error_codes_print_result(rslt);

                    /* To check the interrupt status of low-g. */
                    if (int_status & BMI270_LEGACY_LOW_G_STATUS_MASK)
                    {
                        printf("Low-g interrupt is generated\n");
                        break;
                    }
                } while (rslt == BMI2_OK);
            }
        }
    }

    bmi2_coines_deinit();

    return rslt;
}
