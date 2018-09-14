/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the SHT10/11/15 family of temperature and
 *              humidity sensors
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"
#include "sht1x_params.h"

#define SHT1X_NUM     (sizeof(sht1x_params) / sizeof(sht1x_params[0]))
extern sht1x_dev_t sht1x_devs[SHT1X_NUM];
static sht1x_dev_t *dev = &sht1x_devs[0];

static int run_tests(void)
{
    const char *resolution[] = { "high", "low" };
    const char *calibration[] = { "on", "off" };
    const char *heater[] = { "off", "on" };
    const char *crc[] = { "on", "off" };
    uint8_t conf = 0;

    puts("Reading the sensor three times for all possible configurations:");

    for (int i_res = 0; i_res < 2; i_res++) {
        if (i_res) {
            conf |= SHT1X_CONF_LOW_RESOLUTION;
        }
        else {
            conf &= ~(SHT1X_CONF_LOW_RESOLUTION);
        }

        for (int i_cal = 0; i_cal < 2; i_cal++) {
            if (i_cal) {
                conf |= SHT1X_CONF_SKIP_CALIBRATION;
            }
            else {
                conf &= ~(SHT1X_CONF_SKIP_CALIBRATION);
            }

            for (int i_heater = 0; i_heater < 2; i_heater++) {
                if (i_heater) {
                    conf |= SHT1X_CONF_ENABLE_HEATER;
                }
                else {
                    conf &= ~(SHT1X_CONF_ENABLE_HEATER);
                }

                for (int i_crc = 0; i_crc < 2; i_crc++) {
                    if (i_crc) {
                        conf |= SHT1X_CONF_SKIP_CRC;
                    }
                    else {
                        conf &= ~(SHT1X_CONF_SKIP_CRC);
                    }

                    printf("Resolution: %s, calibration: %s, heater: %s, "
                           "CRC-checking: %s\n",
                           resolution[i_res],
                           calibration[i_cal],
                           heater[i_heater],
                           crc[i_crc]);
                    switch (sht1x_configure(dev, conf)) {
                        case 0:
                            break;
                        case -EIO:
                            puts("Error: gpio_init() failed");
                            return -1;
                        case -EPROTO:
                            puts("Error: Sensor did not acknowledge command");
                            return -1;
                        case -ECANCELED:
                            puts("Error: Sensor did not apply configuration");
                            return -1;
                        case -EBADMSG:
                            puts("Error: CRC error while validating "
                                 "configuration");
                            return -1;
                        default:
                            /* Will never happen, but better safe than sorry */
                            puts("Unknown error");
                            return -1;
                    }

                    for (int i = 0; i < 3; i++) {
                        int16_t temp, hum;
                        switch (sht1x_read(dev, &temp, &hum)) {
                            case 0:
                                break;
                            case -EIO:
                                puts("Error: gpio_init() failed");
                                return -1;
                            case -EPROTO:
                                puts("Error: Sensor did not acknowledge "
                                     "command");
                                return -1;
                            case -ECANCELED:
                                puts("Error: Measurement timed out");
                                return -1;
                            default:
                                /* Won't happen, but better safe than sorry */
                                puts("Unknown error");
                                return -1;
                        }

                        printf("Temperature: %i.%02i°C, Humidity: %i.%02i%%\n",
                               (int)temp / 100, (int)temp % 100,
                               (int)hum / 100, (int)hum % 100);
                    }
                }
            }
        }
    }

    puts("Restoring default configuration");
    switch (sht1x_configure(dev, 0)) {
        case 0:
            break;
        case -EIO:
            puts("Error: gpio_init() failed");
            return -1;
        case -EPROTO:
            puts("Error: Sensor did not acknowledge command");
            return -1;
        case -ECANCELED:
            puts("Error: Sensor did not apply configuration");
            return -1;
        case -EBADMSG:
            puts("Error: CRC error while validating "
                 "configuration");
            return -1;
        default:
            /* Will never happen, but better safe than sorry */
            puts("Unknown error");
            return -1;
    }

    return 0;
}

int main(void)
{
    if (run_tests()) {
        puts("TESTS FAILED!");
    }
    else {
        puts("All automatic tests finished.");
    }

    puts("Dropping to shell for manual testing");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
