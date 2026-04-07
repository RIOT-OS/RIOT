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
    /* Sensor initialization configuration. */
    struct bmi2_dev dev;

    /* Status of api are returned to this variable. */
    int8_t rslt;

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
        /* This API runs the CRT process. */
        rslt = bmi2_do_crt(&dev);
        bmi2_error_codes_print_result(rslt);

        /* Do not move the board while doing CRT. If so, it will throw an abort error. */
        if (rslt == BMI2_OK)
        {
            printf("CRT successfully completed.");
        }
    }

    bmi2_coines_deinit();

    return rslt;
}
