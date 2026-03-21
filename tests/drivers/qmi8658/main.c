/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the QMI8658 IMU driver
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "ztimer.h"
#include "qmi8658.h"
#include "qmi8658_params.h"

#define SLEEP_MSEC              (500UL)

static void _print_datarate(uint8_t odr)
{
    switch (odr) {
    case QMI8658_DATA_RATE_8KHZ:                puts("8kHz");       break;
    case QMI8658_DATA_RATE_4KHZ:                puts("4kHz");       break;
    case QMI8658_DATA_RATE_2KHZ:                puts("2kHz");       break;
    case QMI8658_DATA_RATE_1KHZ:                puts("1kHz");       break;
    case QMI8658_DATA_RATE_500HZ:               puts("500Hz");      break;
    case QMI8658_DATA_RATE_250HZ:               puts("250Hz");      break;
    case QMI8658_DATA_RATE_125HZ:               puts("125Hz");      break;
    case QMI8658_DATA_RATE_62_5HZ:              puts("62.5Hz");     break;
    case QMI8658_DATA_RATE_31_25HZ:             puts("31.25Hz");    break;
    case QMI8658_ACC_LOWPWR_DATA_RATE_128HZ:    puts("128Hz");      break;
    case QMI8658_ACC_LOWPWR_DATA_RATE_21HZ:     puts("21Hz");       break;
    case QMI8658_ACC_LOWPWR_DATA_RATE_11HZ:     puts("11Hz");       break;
    case QMI8658_ACC_LOWPWR_DATA_RATE_3HZ:      puts("3Hz");        break;
    default: printf("WARNING: invalid ODR setting %x", odr);
    }
}

int main(void)
{
    qmi8658_t dev;
    int16_t temp_value;
    qmi8658_3d_data_t acc_value;
    qmi8658_3d_data_t gyro_value;

    puts("QMI8658 test application");
    printf("Initializing QMI8658 sensor at I2C_%i... ", qmi8658_params->i2c);

    if (qmi8658_init(&dev, qmi8658_params) < 0) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    printf("Driver configuration:\nAccelerometer data rate:                   ");
    _print_datarate(dev.params.acc_odr);
    printf("Accelerometer low power mode data rate:    ");
    _print_datarate(dev.params.acc_lowpwr_odr);
    printf("Accelerometer data range:                  ");
    switch (dev.params.acc_fs) {
    case QMI8658_ACC_FS_2G:     puts("±2G");    break;
    case QMI8658_ACC_FS_4G:     puts("±4G");    break;
    case QMI8658_ACC_FS_8G:     puts("±8G");    break;
    case QMI8658_ACC_FS_16G:    puts("±16G");   break;
    default: printf("WARNING: invalid range setting %x", dev.params.acc_fs);
    }
    printf("Gyroscope data rate:                       ");
    _print_datarate(dev.params.gyro_odr);
    printf("Gyroscope data range:                      ");
    switch (dev.params.gyro_fs) {
    case QMI8658_GYRO_FS_16DPS:     puts("±16°/s");     break;
    case QMI8658_GYRO_FS_32DPS:     puts("±32°/s");     break;
    case QMI8658_GYRO_FS_64DPS:     puts("±64°/s");     break;
    case QMI8658_GYRO_FS_128DPS:    puts("±128°/s");    break;
    case QMI8658_GYRO_FS_256DPS:    puts("±256°/s");    break;
    case QMI8658_GYRO_FS_512DPS:    puts("±512°/s");    break;
    case QMI8658_GYRO_FS_1024DPS:   puts("±1024°/s");   break;
    case QMI8658_GYRO_FS_2048DPS:   puts("±2048°/s");   break;
    default: printf("WARNING: invalid range setting %x", dev.params.gyro_fs);
    }
    puts("");

    puts("Putting QMI8658 sensor in IMU mode");
    if (qmi8658_set_mode(&dev, QMI8658_NORMAL_ACC_GYRO) < 0) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    while (1) {
        if (qmi8658_read_acc(&dev, &acc_value) == 0) {
            printf("Accelerometer x: %i y: %i z: %i\n", acc_value.x,
                   acc_value.y,
                   acc_value.z);
        }
        else {
            puts("[ERROR] reading accelerometer!\n");
        }

        if (qmi8658_read_gyro(&dev, &gyro_value) == 0) {
            printf("Gyroscope x: %i y: %i z: %i\n", gyro_value.x,
                   gyro_value.y,
                   gyro_value.z);
        }
        else {
            puts("[ERROR] reading gyroscope!\n");
        }

        if (qmi8658_read_temp(&dev, &temp_value) == 0) {
            printf("Temperature [in °C x 100]: %i \n", temp_value);
        }
        else {
            puts("[ERROR] reading temperature!\n");
        }

        puts("");
        ztimer_sleep(ZTIMER_MSEC, SLEEP_MSEC);
    }

    return 0;
}
