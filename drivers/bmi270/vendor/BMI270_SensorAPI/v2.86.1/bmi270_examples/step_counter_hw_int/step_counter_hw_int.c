/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmi270.h"
#include "common.h"

/******************************************************************************/
/*!           Global variable Declaration                                     */

volatile uint8_t interrupt_status = 0;

/******************************************************************************/
/*!                  Macros                                                   */

/*! APP20 Board number */
#define BOARD_MCU_APP20  UINT8_C(0x03)

/*! APP30 Board number */
#define BOARD_MCU_APP30  UINT8_C(0x05)

/******************************************************************************/
/*!         Static function declaration                                       */

/*!
 *  @brief This internal API is used to set configurations for step counter.
 *
 *  @param[in] bmi2_dev       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_feature_config(struct bmi2_dev *bmi2_dev);

/*!
 * @brief This function gets board information
 *
 * @param[out] board       : Board value to determine as APP2.0 or APP3.0
 */
static void get_board_info(uint8_t *board);

/******************************************************************************/
/*!            Functions                                        */

/*!
 * @brief This internal API is used to set the interrupt status
 */
static void interrupt_callback(uint32_t param1, uint32_t param2)
{
    (void)param1;
    (void)param2;
    interrupt_status = 1;
}

/* This function starts the execution of program. */
int main(void)
{
    /* Sensor initialization configuration. */
    struct bmi2_dev bmi2_dev;

    /* Structure to define type of sensor and their respective data. */
    struct bmi2_feat_sensor_data sensor_data = { 0 };

    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Accel sensor and step counter feature are listed in array. */
    uint8_t sensor_sel[2] = { BMI2_ACCEL, BMI2_STEP_COUNTER };

    /* Variable to get step counter interrupt status. */
    uint16_t int_status = 0;

    /* Variable to store board information */
    uint8_t board = 0;

    /* Select features and their pins to be mapped to. */
    struct bmi2_sens_int_config sens_int = { .type = BMI2_STEP_COUNTER, .hw_int_pin = BMI2_INT1 };

    /* Type of sensor to get step counter data. */
    sensor_data.type = BMI2_STEP_COUNTER;

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&bmi2_dev, BMI2_SPI_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270. */
    rslt = bmi270_init(&bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Enable the selected sensor. */
        rslt = bmi270_sensor_enable(sensor_sel, 2, &bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {
            /* Set the feature configuration for step counter. */
            rslt = set_feature_config(&bmi2_dev);
            bmi2_error_codes_print_result(rslt);

            printf("Step counter watermark level set to 1 (20 steps)\n");

            if (rslt == BMI2_OK)
            {
                /* Map the step counter feature interrupt. */
                rslt = bmi270_map_feat_int(&sens_int, 1, &bmi2_dev);
                bmi2_error_codes_print_result(rslt);

                /* Move the board in steps for 20 times to get step counter interrupt. */
                printf("Move the board in steps\n");

                /* Get board information */
                get_board_info(&board);

                /*
                 * Attach interrupt based on board
                 * Note : Interrupt pin to be given based on pin_type
                 */
                if (board == BOARD_MCU_APP20)
                {
                    coines_attach_interrupt(COINES_SHUTTLE_PIN_20, interrupt_callback,
                                            COINES_PIN_INTERRUPT_FALLING_EDGE);
                }

#if !defined(MCU_APP20)
                else if (board == BOARD_MCU_APP30)
                {
                    coines_attach_interrupt(COINES_MINI_SHUTTLE_PIN_1_6,
                                            interrupt_callback,
                                            COINES_PIN_INTERRUPT_FALLING_EDGE);
                }
#endif

                /* Loop to get number of steps counted. */
                do
                {
                    if (interrupt_status == 1)
                    {
                        interrupt_status = 0;

                        /* To get the interrupt status of the step counter. */
                        rslt = bmi2_get_int_status(&int_status, &bmi2_dev);
                        bmi2_error_codes_print_result(rslt);

                        /* To check the interrupt status of the step counter. */
                        if (int_status & BMI270_STEP_CNT_STATUS_MASK)
                        {
                            printf("Step counter interrupt occurred when watermark level (20 steps) is reached\n");

                            /* Get step counter output. */
                            rslt = bmi270_get_feature_data(&sensor_data, 1, &bmi2_dev);
                            bmi2_error_codes_print_result(rslt);

                            /* Print the step counter output. */
                            printf("No of steps counted  = %lu",
                                   (long unsigned int)sensor_data.sens_data.step_counter_output);

                            break;
                        }
                    }
                } while (rslt == BMI2_OK);
            }
        }
    }

    bmi2_coines_deinit();

    return rslt;
}

/*!
 * @brief This internal API is used to set configurations for step counter.
 */
static int8_t set_feature_config(struct bmi2_dev *bmi2_dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi2_sens_config config;

    /* Interrupt pin configuration */
    struct bmi2_int_pin_config pin_config = { 0 };

    /* Configure the type of sensor. */
    config.type = BMI2_STEP_COUNTER;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_get_sensor_config(&config, 1, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    rslt = bmi2_get_int_pin_config(&pin_config, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Setting water-mark level to 1 for step counter to get interrupt after 20 step counts. Every 20 steps once
         * output triggers. */
        config.cfg.step_counter.watermark_level = 1;

        /* Set new configuration. */
        rslt = bmi270_set_sensor_config(&config, 1, bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        /* Interrupt pin configuration */
        pin_config.pin_type = BMI2_INT1;
        pin_config.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;
        pin_config.pin_cfg[0].lvl = BMI2_INT_ACTIVE_LOW;
        pin_config.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
        pin_config.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
        pin_config.int_latch = BMI2_INT_NON_LATCH;

        rslt = bmi2_set_int_pin_config(&pin_config, bmi2_dev);
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
