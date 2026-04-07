/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include <stdlib.h>

#include "bmi270.h"
#include "coines.h"
#include "common.h"
#include "coines.h"

/******************************************************************************/
/*!           Global variable Declaration                                     */

volatile uint8_t interrupt_status = 0;

/******************************************************************************/
/*!                Macro definition                                           */

/*! APP20 Board number */
#define BOARD_MCU_APP20  UINT8_C(0x03)

/*! APP30 Board number */
#define BOARD_MCU_APP30  UINT8_C(0x05)

/******************************************************************************/
/*!           Static Function Declaration                                     */

/*!
 *  @brief This internal API is used to set configurations for accel.
 *
 *  @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_accel_config(struct bmi2_dev *dev);

/*!
 * @brief This function gets board information
 *
 * @param[out] board       : Board value to determine as APP2.0 or APP3.0
 */
static void get_board_info(uint8_t *board);

/******************************************************************************/
/*!            Functions                                                      */

/*!
 * @brief This internal API is used to set the data ready interrupt status
 */
static void drdy_int_callback(uint32_t param1, uint32_t param2)
{
    (void)param1;
    (void)param2;
    interrupt_status = 1;
}

/* This function starts the execution of program. */
int main(void)
{
    /* Sensor initialization configuration. */
    struct bmi2_dev dev;

    /* Status of api are returned to this variable. */
    int8_t rslt;

    struct bmi2_remap remapped_axis = { 0 };
    uint8_t board = 0;

    /* Select sensors for enable and setting */
    uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };

    char data_array[13][12] =
    { { 0 }, { "BMI2_X" }, { "BMI2_Y" }, { 0 }, { "BMI2_Z" }, { 0 }, { 0 }, { 0 }, { 0 }, { "BMI2_NEG_X" },
      { "BMI2_NEG_Y" }, { 0 }, { "BMI2_NEG_Z" } };

    /* Structure to define type of sensor and their respective data. */
    struct bmi2_sens_data sensor_data = { { 0 } };

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&dev, BMI2_SPI_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize the sensor by enabling write configuration */
    rslt = bmi270_init(&dev);
    bmi2_error_codes_print_result(rslt);

    /* Accel configuration settings. */
    rslt = set_accel_config(&dev);
    bmi2_error_codes_print_result(rslt);

    /* NOTE:
     * Accel and Gyro enable must be done after setting configurations
     */
    rslt = bmi2_sensor_enable(sens_list, 2, &dev);
    bmi2_error_codes_print_result(rslt);

    rslt = bmi2_get_remap_axes(&remapped_axis, &dev);
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
        coines_attach_interrupt(COINES_MINI_SHUTTLE_PIN_1_6, drdy_int_callback, COINES_PIN_INTERRUPT_FALLING_EDGE);
    }
#endif

    printf("\nAXIS_REMAP_FUNC_TEST 1\n");
    printf("Get default values of re-mapped axes sensor data\n");

    printf("Re-mapped x value = %s\n", data_array[remapped_axis.x]);
    printf("Re-mapped y value = %s\n", data_array[remapped_axis.y]);
    printf("Re-mapped z value = %s\n", data_array[remapped_axis.z]);

    printf("Expected Re-mapped x value = BMI2_X\n");
    printf("Expected Re-mapped y value = BMI2_Y\n");
    printf("Expected Re-mapped z value = BMI2_Z\n");

    if ((remapped_axis.x == BMI2_X) && (remapped_axis.y == BMI2_Y) && (remapped_axis.z == BMI2_Z))
    {
        printf(">> PASS\n");
    }
    else
    {
        printf("AXIS_REMAP_FUNC_TEST 1 Failed\n");
        exit(COINES_E_FAILURE);
    }

    printf("Print un-mapped data\n");

    /* Loop to print the accel data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC))
            {
                printf("Accel ::  X : %d    Y : %d    Z :%d\n", sensor_data.acc.x, sensor_data.acc.y,
                       sensor_data.acc.z);

                break;
            }
        }
    }

    /* Loop to print the gyro data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            /* To get the status of gyro data ready interrupt. */
            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR))
            {
                printf("Gyro ::  X : %d     Y : %d    Z :%d\n", sensor_data.gyr.x, sensor_data.gyr.y,
                       sensor_data.gyr.z);

                break;
            }
        }
    }

    printf("\nAXIS_REMAP_FUNC_TEST 2\n");
    printf("Get sensor data of re-mapped axes\n");

    remapped_axis.x = BMI2_NEG_Y;
    remapped_axis.y = BMI2_Z;
    remapped_axis.z = BMI2_NEG_X;

    rslt = bmi2_set_remap_axes(&remapped_axis, &dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        rslt = bmi2_get_remap_axes(&remapped_axis, &dev);
        bmi2_error_codes_print_result(rslt);
        if (rslt == BMI2_OK)
        {
            printf("Re-mapped x value = %s\n", data_array[remapped_axis.x]);
            printf("Re-mapped y value = %s\n", data_array[remapped_axis.y]);
            printf("Re-mapped z value = %s\n", data_array[remapped_axis.z]);
        }

        printf("Expected Re-mapped x value = BMI2_NEG_Y\n");
        printf("Expected Re-mapped y value = BMI2_Z\n");
        printf("Expected Re-mapped z value = BMI2_NEG_X\n");

        if ((remapped_axis.x == BMI2_NEG_Y) && (remapped_axis.y == BMI2_Z) && (remapped_axis.z == BMI2_NEG_X))
        {
            printf(">> PASS\n");
        }
        else
        {
            printf("AXIS_REMAP_FUNC_TEST 2 Failed\n");
            exit(COINES_E_FAILURE);
        }
    }

    printf("Print mapped data\n");

    /* Loop to print the accel data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC))
            {
                printf("Accel ::  X : %d    Y : %d    Z :%d\n", sensor_data.acc.x, sensor_data.acc.y,
                       sensor_data.acc.z);

                break;
            }
        }
    }

    /* Loop to print the gyro data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            /* To get the status of gyro data ready interrupt. */
            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR))
            {
                printf("Gyro ::  X : %d     Y : %d    Z :%d\n", sensor_data.gyr.x, sensor_data.gyr.y,
                       sensor_data.gyr.z);

                break;
            }
        }
    }

    printf("\nAXIS_REMAP_FUNC_TEST 3\n");
    printf("Get sensor data of re-mapped axes - 2nd combination\n");

    remapped_axis.x = BMI2_NEG_Z;
    remapped_axis.y = BMI2_NEG_X;
    remapped_axis.z = BMI2_Y;

    rslt = bmi2_set_remap_axes(&remapped_axis, &dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        rslt = bmi2_get_remap_axes(&remapped_axis, &dev);
        bmi2_error_codes_print_result(rslt);
        if (rslt == BMI2_OK)
        {
            printf("Re-mapped x value = %s\n", data_array[remapped_axis.x]);
            printf("Re-mapped y value = %s\n", data_array[remapped_axis.y]);
            printf("Re-mapped z value = %s\n", data_array[remapped_axis.z]);
        }

        printf("Expected Re-mapped x value = BMI2_NEG_Z\n");
        printf("Expected Re-mapped y value = BMI2_NEG_X\n");
        printf("Expected Re-mapped z value = BMI2_Y\n");

        if ((remapped_axis.x == BMI2_NEG_Z) && (remapped_axis.y == BMI2_NEG_X) && (remapped_axis.z == BMI2_Y))
        {
            printf(">> PASS\n");
        }
        else
        {
            printf("AXIS_REMAP_FUNC_TEST 3 Failed\n");
            exit(COINES_E_FAILURE);
        }
    }

    printf("Print mapped data\n");

    /* Loop to print the accel data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC))
            {
                printf("Accel ::  X : %d    Y : %d    Z :%d\n", sensor_data.acc.x, sensor_data.acc.y,
                       sensor_data.acc.z);

                break;
            }
        }
    }

    /* Loop to print the gyro data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            /* To get the status of gyro data ready interrupt. */
            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR))
            {
                printf("Gyro ::  X : %d     Y : %d    Z :%d\n", sensor_data.gyr.x, sensor_data.gyr.y,
                       sensor_data.gyr.z);

                break;
            }
        }
    }

    printf("\nAXIS_REMAP_FUNC_TEST 4\n");
    printf("Get sensor data of re-mapped axes - 3rd combination\n");

    remapped_axis.x = BMI2_Y;
    remapped_axis.y = BMI2_Z;
    remapped_axis.z = BMI2_X;

    rslt = bmi2_set_remap_axes(&remapped_axis, &dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        rslt = bmi2_get_remap_axes(&remapped_axis, &dev);
        bmi2_error_codes_print_result(rslt);
        if (rslt == BMI2_OK)
        {
            printf("Re-mapped x value = %s\n", data_array[remapped_axis.x]);
            printf("Re-mapped y value = %s\n", data_array[remapped_axis.y]);
            printf("Re-mapped z value = %s\n", data_array[remapped_axis.z]);
        }

        printf("Expected Re-mapped x value = BMI2_Y\n");
        printf("Expected Re-mapped y value = BMI2_Z\n");
        printf("Expected Re-mapped z value = BMI2_X\n");

        if ((remapped_axis.x == BMI2_Y) && (remapped_axis.y == BMI2_Z) && (remapped_axis.z == BMI2_X))
        {
            printf(">> PASS\n");
        }
        else
        {
            printf("AXIS_REMAP_FUNC_TEST 4 Failed\n");
            exit(COINES_E_FAILURE);
        }
    }

    printf("Print mapped data\n");

    /* Loop to print the accel data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC))
            {
                printf("Accel ::  X : %d    Y : %d    Z :%d\n", sensor_data.acc.x, sensor_data.acc.y,
                       sensor_data.acc.z);

                break;
            }
        }
    }

    /* Loop to print the gyro data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            /* To get the status of gyro data ready interrupt. */
            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR))
            {
                printf("Gyro ::  X : %d     Y : %d    Z :%d\n", sensor_data.gyr.x, sensor_data.gyr.y,
                       sensor_data.gyr.z);

                break;
            }
        }
    }

    printf("\nAXIS_REMAP_FUNC_TEST 5\n");
    printf("Get sensor data of re-mapped axes - 4th combination\n");

    remapped_axis.x = BMI2_NEG_X;
    remapped_axis.y = BMI2_NEG_Y;
    remapped_axis.z = BMI2_NEG_Z;

    rslt = bmi2_set_remap_axes(&remapped_axis, &dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        rslt = bmi2_get_remap_axes(&remapped_axis, &dev);
        bmi2_error_codes_print_result(rslt);
        if (rslt == BMI2_OK)
        {
            printf("Re-mapped x value = %s\n", data_array[remapped_axis.x]);
            printf("Re-mapped y value = %s\n", data_array[remapped_axis.y]);
            printf("Re-mapped z value = %s\n", data_array[remapped_axis.z]);
        }

        printf("Expected Re-mapped x value = BMI2_NEG_X\n");
        printf("Expected Re-mapped y value = BMI2_NEG_Y\n");
        printf("Expected Re-mapped z value = BMI2_NEG_Z\n");

        if ((remapped_axis.x == BMI2_NEG_X) && (remapped_axis.y == BMI2_NEG_Y) && (remapped_axis.z == BMI2_NEG_Z))
        {
            printf(">> PASS\n");
        }
        else
        {
            printf("AXIS_REMAP_FUNC_TEST 5 Failed\n");
            exit(COINES_E_FAILURE);
        }
    }

    printf("Print mapped data\n");

    /* Loop to print the accel data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_ACC))
            {
                printf("Accel ::  X : %d    Y : %d    Z :%d\n", sensor_data.acc.x, sensor_data.acc.y,
                       sensor_data.acc.z);

                break;
            }
        }
    }

    /* Loop to print the gyro data when interrupt occurs. */
    for (;;)
    {
        if (interrupt_status == 1)
        {
            interrupt_status = 0;

            /* To get the status of gyro data ready interrupt. */
            rslt = bmi2_get_sensor_data(&sensor_data, &dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sensor_data.status & BMI2_DRDY_GYR))
            {
                printf("Gyro ::  X : %d     Y : %d    Z :%d\n", sensor_data.gyr.x, sensor_data.gyr.y,
                       sensor_data.gyr.z);

                break;
            }
        }
    }

    bmi2_coines_deinit();

    return rslt;
}

/*!
 * @brief This internal API is used to set configurations for accel.
 */
static int8_t set_accel_config(struct bmi2_dev *dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer configuration. */
    struct bmi2_sens_config config;

    /* Interrupt pin configuration */
    struct bmi2_int_pin_config pin_config = { 0 };

    /* Configure the type of feature. */
    config.type = BMI2_ACCEL;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(&config, 1, dev);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, dev);
    bmi2_error_codes_print_result(rslt);

    rslt = bmi2_get_int_pin_config(&pin_config, dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Output Data Rate. By default ODR is set as 100Hz for accel. */
        config.cfg.acc.odr = BMI2_ACC_ODR_100HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config.cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config.cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config.cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel configurations. */
        rslt = bmi2_set_sensor_config(&config, 1, dev);
        bmi2_error_codes_print_result(rslt);

        /* Interrupt pin configuration */
        pin_config.pin_type = BMI2_INT1;
        pin_config.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;
        pin_config.pin_cfg[0].lvl = BMI2_INT_ACTIVE_LOW;
        pin_config.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
        pin_config.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
        pin_config.int_latch = BMI2_INT_NON_LATCH;

        rslt = bmi2_set_int_pin_config(&pin_config, dev);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
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
