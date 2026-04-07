/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmi270_legacy.h"
#include "bmm150.h"
#include "coines.h"
#include "common.h"

/******************************************************************************/
/*!                Macro definition                                           */

/*! Macro define limit to print data */
#define SAMPLE_COUNT   UINT8_C(100)

/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH  (9.80665f)

/*******************************************************************************/
/*!                 Static Functions Definitions                               */

/*!
 *  @brief This function configures accelerometer and gyroscope
 *
 *  @param[in] bmi       : Structure instance of bmi2_dev
 *
 *  @return None
 */
static void bmi2_accel_gyro_conf(struct bmi2_dev *bmi);

/*!
 *  @brief This function configures auxiliary sensor (bmm150)
 *
 *  @param[in] mag_odr     : ODR of magnetometer
 *  @param[in] bmm         : Structure instance of bmm150_dev
 *  @param[in] bmi         : Structure instance of bmi2_dev
 *
 *  @return None
 */
static void bmi2_aux_conf(uint8_t mag_odr, struct bmm150_dev *bmm, struct bmi2_dev *bmi);

/*!
 *  @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 *  range 2G, 4G, 8G or 16G.
 *
 *  @param[in] val       : LSB from each axis.
 *  @param[in] g_range   : Gravity range.
 *  @param[in] bit_width : Resolution for accel.
 *
 *  @return Value in meter per second squared.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width);

/*!
 *  @brief This function converts lsb to degree per second for 16 bit gyro at
 *  range 125, 250, 500, 1000 or 2000dps.
 *
 *  @param[in] val       : LSB from each axis.
 *  @param[in] dps       : Degree per second.
 *  @param[in] bit_width : Resolution for gyro.
 *
 *  @return Value in degree per second.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);

/**
 *  @brief - This function reads data from auxiliary sensor in data mode.
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
static int8_t bmi2_aux_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);

/**
 *  @brief - This function writes data to the auxiliary sensor in data mode.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[out] reg_data    : Aux data pointer to store the data being written.
 *  @param[in] length       : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
static int8_t bmi2_aux_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 *  @brief This function provides the delay for required time (Microsecond) as per the input provided in some of the
 *  APIs.
 *
 *  @param[in] period       : The required wait time in microsecond.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return void.
 */
static void bmi2_aux_delay_us(uint32_t period, void *intf_ptr);

/*!
 *  @brief Function to setup Auxiliary interface (BMM150)
 *
 *  @param[in] bmm  : Structure instance of bmm150_dev
 *  @param[in] bmi  : Structure instance of bmi2_dev
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t bmi2_aux_interface_init(struct bmm150_dev *bmm, struct bmi2_dev *bmi);

/*!
 *  @brief This API is used to print the execution status.
 *
 *  @param[in] api_name : Name of the API whose execution status has to be printed.
 *  @param[in] rslt     : Error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void bmm150_error_codes_print_result(const char api_name[], int8_t rslt);

/*******************************************************************************/
/*!                             Function                                       */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Variable to select the pull-up resistor which is set to trim register */
    uint8_t regdata;

    /* Variable to define limit to print data. */
    uint8_t limit = SAMPLE_COUNT;

    /* Variable to print data set count of data. */
    uint8_t count = 1;

    /* Accel, Gyro and Aux sensors are listed in array. */
    uint8_t sensor_list[3] = { BMI2_ACCEL, BMI2_GYRO, BMI2_AUX };

    /* Sensor initialization configuration. */
    struct bmm150_dev bmm;
    struct bmi2_dev bmi;

    /* bmm150 magnetometer data */
    struct bmm150_mag_data mag_data;

    /* Structure to define type of sensor and their respective data. */
    struct bmi2_sens_data sens_data = { { 0 } };

    /* Variables to define read the accel and gyro data in float */
    float acc_x = 0, acc_y = 0, acc_z = 0;
    float gyr_x = 0, gyr_y = 0, gyr_z = 0;

    /* Interface reference is given as a parameter
     * For I2C : BMI2_I2C_INTF
     * For SPI : BMI2_SPI_INTF
     */
    rslt = bmi2_interface_init(&bmi, BMI2_SPI_INTF);
    bmi2_error_codes_print_result(rslt);

    /*
     * Interface initialization for auxiliary sensor (bmm150)
     */
    rslt = bmi2_aux_interface_init(&bmm, &bmi);
    bmi2_error_codes_print_result(rslt);

    /* Initialize sensor */
    rslt = bmi270_legacy_init(&bmi);
    bmi2_error_codes_print_result(rslt);

    /* Pull-up resistor 10k is set to the trim regiter */
    regdata = BMI2_ASDA_PUPSEL_10K;
    rslt = bmi2_set_regs(BMI2_AUX_IF_TRIM, &regdata, 1, &bmi);
    bmi2_error_codes_print_result(rslt);

    /* Configure accel and gyro */
    bmi2_accel_gyro_conf(&bmi);

    /* Configure Auxiliary sensor (bmm150)
     * Mag ODR to be given as first parameter
     */
    bmi2_aux_conf(BMI2_AUX_ODR_100HZ, &bmm, &bmi);

    /* NOTE:
     * Accel, Gyro and Aux enable must be done after setting configurations
     */
    rslt = bmi2_sensor_enable(sensor_list, 3, &bmi);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, &bmi);
    bmi2_error_codes_print_result(rslt);

    printf("MAGNETOMETER, ACCEL, AND GYRO DATA IN DATA MODE\n");

#ifdef BMM150_USE_FIXED_POINT
    printf("\nMagnetometer data contains fraction part (last 4 bits) and decimal part in uT\n");
#elif BMM150_USE_FLOATING_POINT
    printf("\nMagnetometer data is in uT units\n");
#endif

    if (bmm.chip_id == BMM150_CHIP_ID)
    {
        printf("\nValid BMM150 (Aux) sensor - Chip ID : 0x%x\n", bmm.chip_id);

        printf(
            "\nData set, Status Reg, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Acc_ms2_X, Acc_ms2_Y, Acc_ms2_Z, Gyr_Raw_X, Gyr_Raw_Y, Gyr_Raw_Z, Gyro_DPS_X, Gyro_DPS_Y, Gyro_DPS_Z, Mag_X, Mag_Y, Mag_Z\n\n");

        for (;;)
        {
            rslt = bmi2_get_sensor_data(&sens_data, &bmi);
            bmi2_error_codes_print_result(rslt);

            if ((rslt == BMI2_OK) && (sens_data.status & BMI2_DRDY_AUX) && (sens_data.status & BMI2_DRDY_ACC) &&
                (sens_data.status & BMI2_DRDY_GYR))
            {
                /* Converting lsb to meter per second squared for 16 bit accelerometer at 2G range. */
                acc_x = lsb_to_mps2(sens_data.acc.x, (float)2, bmi.resolution);
                acc_y = lsb_to_mps2(sens_data.acc.y, (float)2, bmi.resolution);
                acc_z = lsb_to_mps2(sens_data.acc.z, (float)2, bmi.resolution);

                /* Converting lsb to degree per second for 16 bit gyro at 2000dps range. */
                gyr_x = lsb_to_dps(sens_data.gyr.x, (float)2000, bmi.resolution);
                gyr_y = lsb_to_dps(sens_data.gyr.y, (float)2000, bmi.resolution);
                gyr_z = lsb_to_dps(sens_data.gyr.z, (float)2000, bmi.resolution);

                /* Compensating the raw auxiliary data available from the BMM150 API. */
                rslt = bmm150_aux_mag_data(sens_data.aux_data, &mag_data, &bmm);
                bmm150_error_codes_print_result("bmm150_aux_mag_data", rslt);

                printf("%d, 0x%x, %d, %d, %d, %4.2f, %4.2f, %4.2f, %d, %d, %d, %4.2f, %4.2f, %4.2f, %ld, %ld, %ld\n",
                       count,
                       sens_data.status,
                       sens_data.acc.x,
                       sens_data.acc.y,
                       sens_data.acc.z,
                       acc_x,
                       acc_y,
                       acc_z,
                       sens_data.gyr.x,
                       sens_data.gyr.y,
                       sens_data.gyr.z,
                       gyr_x,
                       gyr_y,
                       gyr_z,
                       (long int)mag_data.x,
                       (long int)mag_data.y,
                       (long int)mag_data.z);

                count++;
                limit--;
            }

            if (limit == 0)
            {
                break;
            }
        }
    }
    else
    {
        printf("\nInvalid BMM150 (Aux) sensor - Chip ID : 0x%x\n", bmm.chip_id);
    }

    bmi2_coines_deinit();

    return rslt;
}

/*******************************************************************************/
/*!               Static  Functions Definitions                                */

/*!
 * @brief This function configures accelerometer and gyroscope
 */
static void bmi2_accel_gyro_conf(struct bmi2_dev *bmi)
{
    int8_t rslt;

    struct bmi2_sens_config config[2];

    config[0].type = BMI2_ACCEL;
    config[1].type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(config, 2, bmi);
    bmi2_error_codes_print_result(rslt);

    /* Configurations for accel. */
    config[0].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;
    config[0].cfg.acc.bwp = BMI2_ACC_OSR2_AVG2;
    config[0].cfg.acc.odr = BMI2_ACC_ODR_100HZ;
    config[0].cfg.acc.range = BMI2_ACC_RANGE_2G;

    /* Configurations for gyro. */
    config[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
    config[1].cfg.gyr.noise_perf = BMI2_GYR_RANGE_2000;
    config[1].cfg.gyr.bwp = BMI2_GYR_OSR2_MODE;
    config[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
    config[1].cfg.gyr.range = BMI2_GYR_RANGE_2000;
    config[1].cfg.gyr.ois_range = BMI2_GYR_OIS_2000;

    /* Set new configurations for accel and gyro */
    rslt = bmi2_set_sensor_config(config, 2, bmi);
    bmi2_error_codes_print_result(rslt);
}

/*!
 * @brief This function configures auxiliary sensor (bmm150)
 */
static void bmi2_aux_conf(uint8_t mag_odr, struct bmm150_dev *bmm, struct bmi2_dev *bmi)
{
    int8_t rslt;

    struct bmi2_sens_config bmi2_conf;
    struct bmm150_settings bmm1_sett;
    uint8_t bmi2_sens = BMI2_AUX;

    switch (mag_odr)
    {
        case BMI2_AUX_ODR_0_78HZ:
        case BMI2_AUX_ODR_1_56HZ:
        case BMI2_AUX_ODR_3_12HZ:
        case BMI2_AUX_ODR_6_25HZ:
        case BMI2_AUX_ODR_12_5HZ:
            bmm1_sett.preset_mode = BMM150_PRESETMODE_HIGHACCURACY;
            break;
        case BMI2_AUX_ODR_25HZ:
        case BMI2_AUX_ODR_50HZ:
            bmm1_sett.preset_mode = BMM150_PRESETMODE_ENHANCED;
            break;
        case BMI2_AUX_ODR_100HZ:
            bmm1_sett.preset_mode = BMM150_PRESETMODE_REGULAR;
            break;
        case BMI2_AUX_ODR_200HZ:
        case BMI2_AUX_ODR_400HZ:
        case BMI2_AUX_ODR_800HZ:
            bmm1_sett.preset_mode = BMM150_PRESETMODE_LOWPOWER;
            break;
        default:
            mag_odr = 0;
            printf("Error : Unsupported Auxiliary Magnetometer ODR : %u\n", mag_odr);
            break;
    }

    bmi2_conf.type = BMI2_AUX;
    bmi2_conf.cfg.aux.odr = mag_odr;
    bmi2_conf.cfg.aux.aux_en = BMI2_ENABLE;
    bmi2_conf.cfg.aux.i2c_device_addr = BMM150_DEFAULT_I2C_ADDRESS;
    bmi2_conf.cfg.aux.fcu_write_en = BMI2_ENABLE;
    bmi2_conf.cfg.aux.manual_en = BMI2_ENABLE;
    bmi2_conf.cfg.aux.aux_rd_burst = BMI2_AUX_READ_LEN_3;
    bmi2_conf.cfg.aux.man_rd_burst = BMI2_AUX_READ_LEN_3;
    bmi2_conf.cfg.aux.read_addr = BMM150_REG_DATA_X_LSB;

    rslt = bmi2_set_sensor_config(&bmi2_conf, 1, bmi);
    bmi2_error_codes_print_result(rslt);

    bmm1_sett.pwr_mode = BMM150_POWERMODE_FORCED;

    if (mag_odr != 0) /* Proceed only if ODR is valid */
    {
        rslt = bmm150_init(bmm);
        bmm150_error_codes_print_result("bmm150_init", rslt);

        rslt = bmm150_set_presetmode(&bmm1_sett, bmm);
        bmm150_error_codes_print_result("bmm150_set_presetmode", rslt);

        rslt = bmm150_set_op_mode(&bmm1_sett, bmm);
        bmm150_error_codes_print_result("bmm150_set_op_mode", rslt);

        bmi2_conf.cfg.aux.manual_en = BMI2_DISABLE;
        rslt = bmi2_set_sensor_config(&bmi2_conf, 1, bmi);
        bmi2_error_codes_print_result(rslt);
    }
    else
    {
        bmi2_conf.cfg.aux.aux_en = BMI2_DISABLE;
        bmi2_conf.cfg.aux.fcu_write_en = BMI2_DISABLE;
        bmi2_conf.cfg.aux.manual_en = BMI2_ENABLE;

        rslt = bmi2_set_sensor_config(&bmi2_conf, 1, bmi);
        bmi2_error_codes_print_result(rslt);

        rslt = bmi2_sensor_disable(&bmi2_sens, 1, bmi);
        bmi2_error_codes_print_result(rslt);
    }
}

/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (dps / (half_scale)) * (val);
}

/*!
 * @brief This function reads the data from auxiliary sensor in data mode.
 */
static int8_t bmi2_aux_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int8_t rslt;

    struct bmi2_dev *dev = (struct bmi2_dev *)intf_ptr;

    rslt = bmi2_read_aux_man_mode(reg_addr, reg_data, (uint16_t)length, dev);

    return rslt;
}

/*!
 * @brief This function writes the data to auxiliary sensor in data mode.
 */
static int8_t bmi2_aux_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int8_t rslt;

    struct bmi2_dev *dev = (struct bmi2_dev *)intf_ptr;

    rslt = bmi2_write_aux_man_mode(reg_addr, reg_data, (uint16_t)length, dev);

    return rslt;
}

/*!
 * Delay function map to COINES platform
 */
static void bmi2_aux_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;

    coines_delay_usec(period);
}

/*!
 *  @brief Function to setup Auxiliary interface (BMM150)
 */
int8_t bmi2_aux_interface_init(struct bmm150_dev *bmm, struct bmi2_dev *bmi)
{
    int8_t rslt = BMI2_OK;

    if (bmm != NULL)
    {
        /* To enable the i2c interface settings for bmm150. */
        bmm->intf_ptr = bmi;
        bmm->read = bmi2_aux_i2c_read;
        bmm->write = bmi2_aux_i2c_write;
        bmm->delay_us = bmi2_aux_delay_us;

        /* As per datasheet, aux interface will support only for I2C */
        bmm->intf = BMM150_I2C_INTF;
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 *  @brief Prints the execution status of the APIs.
 */
void bmm150_error_codes_print_result(const char api_name[], int8_t rslt)
{
    if (rslt != BMM150_OK)
    {
        printf("%s\t", api_name);

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
