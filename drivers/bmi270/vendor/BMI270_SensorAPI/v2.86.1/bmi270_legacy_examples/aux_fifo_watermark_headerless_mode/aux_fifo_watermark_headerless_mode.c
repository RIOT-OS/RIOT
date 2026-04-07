/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmi270_legacy.h"
#include "bmm150.h"
#include "common.h"

/******************************************************************************/
/*!                Macro definition                                           */

/*! Macros to select the sensors                   */
#define ACCEL                           UINT8_C(0x00)
#define GYRO                            UINT8_C(0x01)
#define AUX                             UINT8_C(0x02)

/*! Buffer size allocated to store raw FIFO data */
#define BMI2_FIFO_RAW_DATA_BUFFER_SIZE  UINT16_C(1200)

/*! Length of data to be read from FIFO */
#define BMI2_FIFO_RAW_DATA_USER_LENGTH  UINT16_C(1200)

/*! Number of accel frames to be extracted from FIFO */

/*!
 * Calculation:
 * fifo_watermark_level = 650, accel_frame_len = 6, gyro_frame_len = 6, aux_frame_len = 8.
 * fifo_accel_frame_count = (650 / (6 + 6 + 8 )) = 32 frames
 */
#define BMI2_FIFO_ACCEL_FRAME_COUNT     UINT8_C(32)

/*! Number of gyro frames to be extracted from FIFO */
#define BMI2_FIFO_GYRO_FRAME_COUNT      UINT8_C(32)

/*! Number of aux frames to be extracted from FIFO. */
#define BMI2_FIFO_AUX_FRAME_COUNT       UINT8_C(32)

/*! Setting a watermark level in FIFO */
#define BMI2_FIFO_WATERMARK_LEVEL       UINT16_C(650)

/*****************************************************************************/
/*!         Structure declaration                                            */

/* Sensor initialization configuration. */
struct bmi2_dev aux_bmi2_dev;

/******************************************************************************/
/*!                        Global Variables                                   */

/* Array of accelerometer frames */
struct bmi2_sens_axes_data fifo_accel_data[BMI2_FIFO_ACCEL_FRAME_COUNT] = { { 0 } };

/* Array of gyro frames */
struct bmi2_sens_axes_data fifo_gyro_data[BMI2_FIFO_GYRO_FRAME_COUNT] = { { 0 } };

/* Array of aux frames */
struct bmi2_aux_fifo_data fifo_aux_data[BMI2_FIFO_AUX_FRAME_COUNT] = { { { 0 } } };

/* Number of bytes of FIFO data
 * NOTE : Dummy byte (for SPI Interface) required for FIFO data read must be given as part of array size
 */
uint8_t fifo_data[BMI2_FIFO_RAW_DATA_BUFFER_SIZE];

/*******************************************************************************/
/*!                 Functions                                                  */

/**
 * aux_i2c_read - Reads data from auxiliary sensor.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[out] reg_data    : Aux data pointer to store the read data.
 *  @param[in] length       : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
static int8_t aux_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);

/**
 * aux_i2c_write - Writes data to the auxiliary sensor.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data    : Aux data pointer to store the data being written.
 *  @param[in] length       : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
static int8_t aux_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 * @brief This function provides the delay for required time (Microsecond) as per the input provided in some of the
 * APIs.
 *
 *  @param[in] period       : The required wait time in microsecond.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return void.
 */
static void aux_delay_us(uint32_t period, void *intf_ptr);

/*!
 *  @brief This API is used to print the execution status.
 *
 *  @param[in] rslt     : Error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
static void bmm150_error_codes_print_result(int8_t rslt);

/******************************************************************************/
/*!            Functions                                                      */

int main(void)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Variable to select the pull-up resistor which is set to trim register */
    uint8_t regdata;

    /* Initialize the interrupt status of accel, gyro and aux. */
    uint16_t int_status = 0;

    /* Accel, Gyro and Aux sensors are listed in array. */
    uint8_t sensor_list[3] = { BMI2_ACCEL, BMI2_GYRO, BMI2_AUX };

    /* Structure to define the type of the sensor and its configurations. */
    struct bmi2_sens_config config[3];

    /* Sensor initialization configuration. */
    struct bmm150_dev aux_bmm150_dev;

    /* bmm150 settings configuration */
    struct bmm150_settings settings;

    /* bmm150 magnetometer data */
    struct bmm150_mag_data mag_data;

    uint16_t index = 0;

    uint16_t accel_frame_length = BMI2_FIFO_ACCEL_FRAME_COUNT;

    uint16_t gyro_frame_length = BMI2_FIFO_GYRO_FRAME_COUNT;

    uint16_t aux_frame_length = BMI2_FIFO_AUX_FRAME_COUNT;

    uint16_t fifo_length = 0;

    int8_t try = 1;

    /* Initialize FIFO frame structure. */
    struct bmi2_fifo_frame fifoframe = { 0 };

    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;
    config[AUX].type = BMI2_AUX;

    /* To enable the i2c interface settings for bmm150. */
    uint8_t aux_bmm150_dev_addr = BMM150_DEFAULT_I2C_ADDRESS;
    aux_bmm150_dev.intf_ptr = &aux_bmm150_dev_addr;
    aux_bmm150_dev.read = aux_i2c_read;
    aux_bmm150_dev.write = aux_i2c_write;
    aux_bmm150_dev.delay_us = aux_delay_us;

    /* As per datasheet, aux interface with bmi270_legacy will support only for I2C */
    aux_bmm150_dev.intf = BMM150_I2C_INTF;

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&aux_bmi2_dev, BMI2_SPI_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270_legacy. */
    rslt = bmi270_legacy_init(&aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Pull-up resistor 2k is set to the trim regiter */
    regdata = BMI2_ASDA_PUPSEL_2K;
    rslt = bmi2_set_regs(BMI2_AUX_IF_TRIM, &regdata, 1, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_legacy_get_sensor_config(config, 3, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Configurations for accel. */
    config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;
    config[ACCEL].cfg.acc.bwp = BMI2_ACC_OSR2_AVG2;
    config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_50HZ;
    config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

    /* Configurations for gyro. */
    config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
    config[GYRO].cfg.gyr.noise_perf = BMI2_GYR_RANGE_2000;
    config[GYRO].cfg.gyr.bwp = BMI2_GYR_OSR2_MODE;
    config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_50HZ;
    config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;
    config[GYRO].cfg.gyr.ois_range = BMI2_GYR_OIS_2000;

    /* Configurations for aux. */
    config[AUX].cfg.aux.odr = BMI2_AUX_ODR_50HZ;
    config[AUX].cfg.aux.aux_en = BMI2_ENABLE;
    config[AUX].cfg.aux.i2c_device_addr = BMM150_DEFAULT_I2C_ADDRESS;
    config[AUX].cfg.aux.fcu_write_en = BMI2_ENABLE;
    config[AUX].cfg.aux.man_rd_burst = BMI2_AUX_READ_LEN_3;
    config[AUX].cfg.aux.read_addr = BMM150_REG_DATA_X_LSB;

    /* Set new configurations for accel, gyro and aux. */
    rslt = bmi270_legacy_set_sensor_config(config, 3, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* NOTE:
     * Accel and gyro enable must be done after setting configurations
     */
    rslt = bmi270_legacy_sensor_enable(sensor_list, 3, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmm150. */
    rslt = bmm150_init(&aux_bmm150_dev);
    bmm150_error_codes_print_result(rslt);

    /* Set the power mode to normal mode. */
    settings.pwr_mode = BMM150_POWERMODE_NORMAL;
    rslt = bmm150_set_op_mode(&settings, &aux_bmm150_dev);
    bmm150_error_codes_print_result(rslt);

    rslt = bmi270_legacy_get_sensor_config(config, 3, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Disable manual mode so that the data mode is enabled. */
    config[AUX].cfg.aux.manual_en = BMI2_DISABLE;

    /* Set the aux configurations. */
    rslt = bmi270_legacy_set_sensor_config(config, 3, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Before setting FIFO, disable the advance power save mode. */
    rslt = bmi2_set_adv_power_save(BMI2_DISABLE, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Initially disable all configurations in fifo. */
    rslt = bmi2_set_fifo_config(BMI2_FIFO_ALL_EN, BMI2_DISABLE, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Update FIFO structure. */
    /* Mapping the buffer to store the fifo data. */
    fifoframe.data = fifo_data;

    /* Length of FIFO frame. */
    /* To read sensortime, extra 3 bytes are added to fifo user length. */
    fifoframe.length = BMI2_FIFO_RAW_DATA_USER_LENGTH;

    /* Set FIFO configuration by enabling accel, gyro and timestamp.
     * NOTE 1: The header mode is enabled by default.
     * NOTE 2: By default the FIFO operating mode is in FIFO mode.
     * NOTE 3: Sensortime is enabled by default */
    printf("FIFO is configured in headerless mode\n");
    rslt = bmi2_set_fifo_config(BMI2_FIFO_ALL_EN, BMI2_ENABLE, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* To enable headerless mode, disable the header. */
    if (rslt == BMI2_OK)
    {
        rslt = bmi2_set_fifo_config(BMI2_FIFO_HEADER_EN, BMI2_DISABLE, &aux_bmi2_dev);
        bmi2_error_codes_print_result(rslt);
    }

    /* FIFO water-mark interrupt is enabled. */
    fifoframe.data_int_map = BMI2_FWM_INT;

    /* Map water-mark interrupt to the required interrupt pin. */
    rslt = bmi2_map_data_int(fifoframe.data_int_map, BMI2_INT1, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Set water-mark level. */
    fifoframe.wm_lvl = BMI2_FIFO_WATERMARK_LEVEL;

    /* Set the water-mark level if water-mark interrupt is mapped. */
    rslt = bmi2_set_fifo_wm(fifoframe.wm_lvl, &aux_bmi2_dev);
    bmi2_error_codes_print_result(rslt);

#ifdef BMM150_USE_FIXED_POINT
    printf("Magnetometer data contains fraction part (last 4 bits) and decimal part\n\n");
#endif

    if (aux_bmm150_dev.chip_id == BMM150_CHIP_ID)
    {
        printf("\nValid BMM150 (Aux) sensor - Chip ID : 0x%x\n", aux_bmm150_dev.chip_id);

        while (try <= 10)
        {
            /* Read FIFO data on interrupt. */
            rslt = bmi2_get_int_status(&int_status, &aux_bmi2_dev);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (int_status & BMI2_FWM_INT_STATUS_MASK))
            {
                printf("\nIteration : %d\n", try);

                accel_frame_length = BMI2_FIFO_ACCEL_FRAME_COUNT;

                gyro_frame_length = BMI2_FIFO_GYRO_FRAME_COUNT;

                aux_frame_length = BMI2_FIFO_AUX_FRAME_COUNT;

                rslt = bmi2_get_fifo_length(&fifo_length, &aux_bmi2_dev);
                bmi2_error_codes_print_result(rslt);

                /* Updating FIFO length to be read based on available length and dummy byte updation */
                fifoframe.length = fifo_length + aux_bmi2_dev.dummy_byte;

                printf("\nFIFO data bytes available : %d \n", fifo_length);
                printf("\nFIFO data bytes requested : %d \n", fifoframe.length);

                /* Read FIFO data. */
                rslt = bmi2_read_fifo_data(&fifoframe, &aux_bmi2_dev);
                bmi2_error_codes_print_result(rslt);

                /* Read FIFO data on interrupt. */
                rslt = bmi2_get_int_status(&int_status, &aux_bmi2_dev);
                bmi2_error_codes_print_result(rslt);

                if (rslt == BMI2_OK)
                {
                    printf("\nFIFO accel frames requested : %d \n", accel_frame_length);

                    /* Parse the FIFO data to extract accelerometer data from the FIFO buffer. */
                    rslt = bmi2_extract_accel(fifo_accel_data, &accel_frame_length, &fifoframe, &aux_bmi2_dev);
                    printf("\nFIFO accel frames extracted : %d \n", accel_frame_length);

                    printf("\nFIFO gyro frames requested : %d \n", gyro_frame_length);

                    /* Parse the FIFO data to extract gyro data from the FIFO buffer. */
                    (void)bmi2_extract_gyro(fifo_gyro_data, &gyro_frame_length, &fifoframe, &aux_bmi2_dev);
                    printf("\nFIFO gyro frames extracted : %d \n", gyro_frame_length);

                    printf("\nFIFO aux frames requested : %d \n", aux_frame_length);

                    /* Parse the FIFO data to extract aux data from the FIFO buffer. */
                    (void)bmi2_extract_aux(fifo_aux_data, &aux_frame_length, &fifoframe, &aux_bmi2_dev);
                    printf("\nFIFO aux frames extracted : %d \n", aux_frame_length);

                    printf("\nExtracted accel frames\n");

                    printf("ACCEL_DATA, X, Y, Z\n");

                    /* Print the parsed accelerometer data from the FIFO buffer. */
                    for (index = 0; index < accel_frame_length; index++)
                    {
                        printf("%d, %d, %d, %d\n",
                               index,
                               fifo_accel_data[index].x,
                               fifo_accel_data[index].y,
                               fifo_accel_data[index].z);
                    }

                    printf("\nExtracted gyro frames\n");

                    printf("GYRO_DATA, X, Y, Z\n");

                    /* Print the parsed gyro data from the FIFO buffer. */
                    for (index = 0; index < gyro_frame_length; index++)
                    {
                        printf("%d, %d, %d, %d\n",
                               index,
                               fifo_gyro_data[index].x,
                               fifo_gyro_data[index].y,
                               fifo_gyro_data[index].z);
                    }

                    printf("\nExtracted AUX frames\n");

                    printf("AUX_DATA, Mag_uT_X, Mag_uT_Y, Mag_uT_Z\n");

                    /* Print the parsed aux data from the FIFO buffer. */
                    for (index = 0; index < aux_frame_length; index++)
                    {
                        /* Compensating the raw auxiliary data available from the BMM150 API. */
                        rslt = bmm150_aux_mag_data(fifo_aux_data[index].data, &mag_data, &aux_bmm150_dev);
                        bmm150_error_codes_print_result(rslt);

                        printf("%d, %ld, %ld, %ld\n",
                               index,
                               (long int)mag_data.x,
                               (long int)mag_data.y,
                               (long int)mag_data.z);
                    }

                    try++;
                }
            }
        }
    }
    else
    {
        printf("\nInvalid BMM150 (Aux) sensor - Chip ID : 0x%x\n", aux_bmm150_dev.chip_id);
    }

    bmi2_coines_deinit();

    return rslt;
}

/*!
 * @brief This function reads the data from auxiliary sensor
 */
static int8_t aux_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int8_t rslt;

    (void)intf_ptr;

    rslt = bmi2_read_aux_man_mode(reg_addr, reg_data, (uint16_t)length, &aux_bmi2_dev);

    return rslt;
}

/*!
 * @brief This function writes the data to auxiliary sensor
 */
static int8_t aux_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int8_t rslt;

    (void)intf_ptr;

    rslt = bmi2_write_aux_man_mode(reg_addr, reg_data, (uint16_t)length, &aux_bmi2_dev);

    return rslt;
}

/*!
 * Delay function map to COINES platform
 */
static void aux_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;

    coines_delay_usec(period);
}

/*!
 *  @brief Prints the execution status of the APIs.
 */
static void bmm150_error_codes_print_result(int8_t rslt)
{
    if (rslt != BMM150_OK)
    {
        switch (rslt)
        {
            case BMM150_E_NULL_PTR:
                printf("Error [%d] : Null pointer error.", rslt);
                printf(
                    "It occurs when the user tries to assign value (not address) to a pointer, which has been initialized to NULL.\r\n");
                break;

            case BMM150_E_COM_FAIL:
                printf("Error [%d] : Communication failure error.", rslt);
                printf(
                    "It occurs due to read/write operation failure and also due to power failure during communication\r\n");
                break;

            case BMM150_E_DEV_NOT_FOUND:
                printf("Error [%d] : Device not found error. It occurs when the device chip id is incorrectly read\r\n",
                       rslt);
                break;

            case BMM150_E_INVALID_CONFIG:
                printf("Error [%d] : Invalid sensor configuration.", rslt);
                printf(" It occurs when there is a mismatch in the requested feature with the available one\r\n");
                break;

            default:
                printf("Error [%d] : Unknown error code\r\n", rslt);
                break;
        }
    }
}
