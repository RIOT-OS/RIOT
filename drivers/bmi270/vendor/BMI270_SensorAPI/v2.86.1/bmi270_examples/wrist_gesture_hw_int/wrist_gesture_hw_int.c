/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!            Header Files                                  */
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
 *  @brief This internal API is used to set the sensor configuration.
 *
 *  @param[in] bmi2_dev     : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t bmi2_set_config(struct bmi2_dev *bmi2_dev);

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

/* This function starts the execution of program */
int main(void)
{
    /* Variable to define result */
    int8_t rslt;

    /* Initialize status of wrist gesture interrupt */
    uint16_t int_status = 0;

    /* Sensor initialization configuration. */
    struct bmi2_dev bmi2_dev;

    /* Variable to store board information */
    uint8_t board = 0;

    /* Select features and their pins to be mapped to */
    struct bmi2_sens_int_config sens_int = { .type = BMI2_WRIST_GESTURE, .hw_int_pin = BMI2_INT2 };

    /* Sensor data structure */
    struct bmi2_feat_sensor_data sens_data = { 0 };

    /* The gesture movements are listed in array */
    const char *gesture_output[6] =
    { "unknown_gesture", "push_arm_down", "pivot_up", "wrist_shake_jiggle", "flick_in", "flick_out" };

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
        sens_data.type = BMI2_WRIST_GESTURE;

        /* Set the sensor configuration */
        rslt = bmi2_set_config(&bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {
            /* Map the feature interrupt */
            rslt = bmi270_map_feat_int(&sens_int, 1, &bmi2_dev);
            bmi2_error_codes_print_result(rslt);

            /* Get board information */
            get_board_info(&board);

            /*
             * Attach interrupt based on board
             * Note : Interrupt pin to be given based on pin_type
             */
            if (board == BOARD_MCU_APP20)
            {
                coines_attach_interrupt(COINES_SHUTTLE_PIN_21, interrupt_callback, COINES_PIN_INTERRUPT_FALLING_EDGE);
            }

#if !defined(MCU_APP20)
            else if (board == BOARD_MCU_APP30)
            {
                coines_attach_interrupt(COINES_MINI_SHUTTLE_PIN_1_7,
                                        interrupt_callback,
                                        COINES_PIN_INTERRUPT_FALLING_EDGE);
            }
#endif

            if (rslt == BMI2_OK)
            {
                printf("Flip the board in portrait/landscape mode:\n");

                /* Loop to print the wrist gesture data when interrupt occurs */
                for (;;)
                {
                    if (interrupt_status == 1)
                    {
                        interrupt_status = 0;

                        /* Get the interrupt status of the wrist gesture */
                        rslt = bmi2_get_int_status(&int_status, &bmi2_dev);
                        bmi2_error_codes_print_result(rslt);

                        if ((rslt == BMI2_OK) && (int_status & BMI270_WRIST_GEST_STATUS_MASK))
                        {
                            printf("Wrist gesture detected\n");

                            /* Get wrist gesture output */
                            rslt = bmi270_get_feature_data(&sens_data, 1, &bmi2_dev);
                            bmi2_error_codes_print_result(rslt);

                            printf("Wrist gesture = %d\r\n", sens_data.sens_data.wrist_gesture_output);

                            printf("Gesture output = %s\n", gesture_output[sens_data.sens_data.wrist_gesture_output]);
                            break;
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
 * @brief This internal API sets the sensor configuration
 */
static int8_t bmi2_set_config(struct bmi2_dev *bmi2_dev)
{
    /* Variable to define result */
    int8_t rslt;

    /* List the sensors which are required to enable */
    uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_WRIST_GESTURE };

    /* Structure to define the type of the sensor and its configurations */
    struct bmi2_sens_config config;

    /* Interrupt pin configuration */
    struct bmi2_int_pin_config pin_config = { 0 };

    /* Configure type of feature */
    config.type = BMI2_WRIST_GESTURE;

    /* Enable the selected sensors */
    rslt = bmi270_sensor_enable(sens_list, 2, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    rslt = bmi2_get_int_pin_config(&pin_config, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Get default configurations for the type of feature selected */
        rslt = bmi270_get_sensor_config(&config, 1, bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {
            config.cfg.wrist_gest.wearable_arm = BMI2_ARM_LEFT;

            /* Set the new configuration along with interrupt mapping */
            rslt = bmi270_set_sensor_config(&config, 1, bmi2_dev);
            bmi2_error_codes_print_result(rslt);

            /* Interrupt pin configuration */
            pin_config.pin_type = BMI2_INT2;
            pin_config.pin_cfg[1].input_en = BMI2_INT_INPUT_DISABLE;
            pin_config.pin_cfg[1].lvl = BMI2_INT_ACTIVE_LOW;
            pin_config.pin_cfg[1].od = BMI2_INT_PUSH_PULL;
            pin_config.pin_cfg[1].output_en = BMI2_INT_OUTPUT_ENABLE;
            pin_config.int_latch = BMI2_INT_NON_LATCH;

            rslt = bmi2_set_int_pin_config(&pin_config, bmi2_dev);
            bmi2_error_codes_print_result(rslt);
        }
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
