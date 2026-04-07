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
/*!                Macro definition                                           */

/**\name Orientation output macros */
#define BMI270_LEGACY_FACE_UP            UINT8_C(0x00)
#define BMI270_LEGACY_FACE_DOWN          UINT8_C(0x01)

#define BMI270_LEGACY_PORTRAIT_UP_RIGHT  UINT8_C(0x00)
#define BMI270_LEGACY_LANDSCAPE_LEFT     UINT8_C(0x01)
#define BMI270_LEGACY_PORTRAIT_UP_DOWN   UINT8_C(0x02)
#define BMI270_LEGACY_LANDSCAPE_RIGHT    UINT8_C(0x03)

/******************************************************************************/
/*!            Function                                                       */

/* This function starts the execution of program. */
int main(void)
{
    /* Structure to define the type of sensor and its configurations. */
    struct bmi2_sens_config config;

    /* Sensor initialization configuration. */
    struct bmi2_dev dev;

    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Variables to store the output of orientation. */
    uint8_t orientation_out = 0;
    uint8_t orientation_faceup_down = 0;

    /* Structure to define type of sensor and their respective data. */
    struct bmi2_feat_sensor_data sensor_data = { 0 };

    /* Accel sensor and orientation feature are listed in array. */
    uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_ORIENTATION };

    /* Variable to get orientation interrupt status. */
    uint16_t int_status = 0;

    /* Select features and their pins to be mapped to. */
    struct bmi2_sens_int_config sens_int = { .type = BMI2_ORIENTATION, .hw_int_pin = BMI2_INT2 };

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&dev, BMI2_SPI_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270_legacy. */
    rslt = bmi270_legacy_init(&dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Enable the selected sensors. */
        rslt = bmi270_legacy_sensor_enable(sens_list, 2, &dev);
        bmi2_error_codes_print_result(rslt);

        /* Configure the type of feature. */
        config.type = BMI2_ORIENTATION;

        if (rslt == BMI2_OK)
        {
            /* Get default configurations for the type of feature selected. */
            rslt = bmi270_legacy_get_sensor_config(&config, 1, &dev);
            bmi2_error_codes_print_result(rslt);

            /* Set orientation configurations */
            config.cfg.orientation.ud_en = 0x01;
            config.cfg.orientation.mode = 0x02;
            config.cfg.orientation.blocking = 0x01;
            config.cfg.orientation.theta = 0x33;
            config.cfg.orientation.hysteresis = 0x80;

            rslt = bmi270_legacy_set_sensor_config(&config, 1, &dev);
            bmi2_error_codes_print_result(rslt);

            if (rslt == BMI2_OK)
            {
                /* Map the feature interrupt for orientation. */
                rslt = bmi270_legacy_map_feat_int(&sens_int, 1, &dev);
                bmi2_error_codes_print_result(rslt);

                /* Move the sensor at different orientations to get orientation interrupt. */
                printf("Move the board at different orientations\n");

                /* Sensor type of sensor to get data */
                sensor_data.type = BMI2_ORIENTATION;

                for (;;)
                {
                    /* To get the interrupt status of orientation. */
                    rslt = bmi2_get_int_status(&int_status, &dev);
                    bmi2_error_codes_print_result(rslt);

                    /* To check the interrupt status of orientation. */
                    if ((rslt == BMI2_OK) && (int_status & BMI270_LEGACY_ORIENT_STATUS_MASK))
                    {
                        printf("Orientation interrupt is generated\n");
                        rslt = bmi270_legacy_get_feature_data(&sensor_data, 1, &dev);

                        orientation_out = sensor_data.sens_data.orient_output.portrait_landscape;
                        orientation_faceup_down = sensor_data.sens_data.orient_output.faceup_down;

                        printf("The Orientation output is %d \n", orientation_out);
                        printf("The Orientation faceup/down output is %d\n", orientation_faceup_down);

                        switch (orientation_out)
                        {
                            case BMI270_LEGACY_LANDSCAPE_LEFT:
                                printf("Orientation state is landscape left\n");
                                break;
                            case BMI270_LEGACY_LANDSCAPE_RIGHT:
                                printf("Orientation state is landscape right\n");
                                break;
                            case BMI270_LEGACY_PORTRAIT_UP_DOWN:
                                printf("Orientation state is portrait upside down\n");
                                break;
                            case BMI270_LEGACY_PORTRAIT_UP_RIGHT:
                                printf("Orientation state is portrait upright\n");
                                break;
                            default:
                                break;
                        }

                        switch (orientation_faceup_down)
                        {
                            case BMI270_LEGACY_FACE_UP:
                                printf("Orientation state is face up\n");
                                break;
                            case BMI270_LEGACY_FACE_DOWN:
                                printf("Orientation state is face down\n");
                                break;
                            default:
                                break;
                        }

                        break;
                    }
                }
            }
        }
    }

    bmi2_coines_deinit();

    return rslt;
}
