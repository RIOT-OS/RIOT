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
/*!         Static Function Declaration                                       */

/*!
 *  @brief This internal API is used to set configurations for no-motion.
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
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Accel sensor and no-motion feature are listed in array. */
    uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_NO_MOTION };

    /* Variable to get no-motion interrupt status. */
    uint16_t int_status = 0;

    /* Variable to store board information */
    uint8_t board = 0;

    /* Sensor initialization configuration. */
    struct bmi2_dev bmi2_dev;

    /* Select features and their pins to be mapped to. */
    struct bmi2_sens_int_config sens_int = { .type = BMI2_NO_MOTION, .hw_int_pin = BMI2_INT1 };

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&bmi2_dev, BMI2_I2C_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270. */
    rslt = bmi270_init(&bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* Enable the selected sensors. */
        rslt = bmi270_sensor_enable(sens_list, 2, &bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {
            /* Set feature configurations for no-motion. */
            rslt = set_feature_config(&bmi2_dev);
            bmi2_error_codes_print_result(rslt);

            if (rslt == BMI2_OK)
            {
                /* Map the feature interrupt for no-motion. */
                rslt = bmi270_map_feat_int(&sens_int, 1, &bmi2_dev);
                bmi2_error_codes_print_result(rslt);
                printf("Do not move the board\n");

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

                /* Loop to get no-motion interrupt. */
                do
                {
                    if (interrupt_status == 1)
                    {
                        interrupt_status = 0;

                        /* Clear buffer. */
                        int_status = 0;

                        /* To get the interrupt status of no-motion. */
                        rslt = bmi2_get_int_status(&int_status, &bmi2_dev);
                        bmi2_error_codes_print_result(rslt);

                        /* To check the interrupt status of no-motion. */
                        if (int_status & BMI270_NO_MOT_STATUS_MASK)
                        {
                            printf("No-motion interrupt is generated\n");
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
 * @brief This internal API is used to set configurations for no-motion.
 */
static int8_t set_feature_config(struct bmi2_dev *bmi2_dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi2_sens_config config;

    /* Interrupt pin configuration */
    struct bmi2_int_pin_config pin_config = { 0 };

    /* Configure the type of feature. */
    config.type = BMI2_NO_MOTION;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_get_sensor_config(&config, 1, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    rslt = bmi2_get_int_pin_config(&pin_config, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* 1LSB equals 20ms. Default is 100ms, setting to 80ms. */
        config.cfg.no_motion.duration = 0x04;

        /* 1LSB equals to 0.48mg. Default is 70mg, setting to 50mg. */
        config.cfg.no_motion.threshold = 0x68;

        /* Set new configurations. */
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
