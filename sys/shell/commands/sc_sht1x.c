/*
 * Copyright (C) 2013 INRIA
 *               2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to access SHT10/SHT11/SHT15 sensors
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sht1x.h"
#include "sht1x_params.h"

#define SHT1X_NUM     ARRAY_SIZE(sht1x_params)

extern sht1x_dev_t sht1x_devs[SHT1X_NUM];

static sht1x_dev_t *get_dev(int argc, char **argv)
{
    switch (argc) {
    case 1:
        return &sht1x_devs[0];
    case 2:
    {
        int pos = atoi(argv[1]);
        if ((pos < 0) || (pos >= (int)SHT1X_NUM)) {
            printf("No SHT10/SHT11/SHT15 device with number %i\n", pos);
            return NULL;
        }
        return &sht1x_devs[pos];
    }
    default:
        break;
    }

    printf("Usage: %s [DEVICE_NUMBER]\n", argv[0]);
    return NULL;
}

static void error_msg(const char *msg)
{
    printf("[sht1x] Operation failed: %s\n", msg);
}

static int read_sensor(int16_t *temp, int16_t *hum, int argc, char **argv)
{
    const sht1x_dev_t *dev = get_dev(argc, argv);

    if (!dev) {
        return -1;
    }

    switch (sht1x_read(dev, temp, hum)) {
    case 0:
        break;
    case -EIO:
        error_msg("gpio_init() failed");
        return -1;
    case -EBADMSG:
        error_msg("CRC checksum error");
        return -1;
    case -ECANCELED:
        error_msg("Measurement timed out");
        return -1;
    case -EPROTO:
        error_msg("Sensor did not acknowledge command");
        return -1;
    default:
        /* Should never happen, but better safe the sorry */
        error_msg("Unknown error");
        return -1;
    }

    return 0;
}

int _get_humidity_handler(int argc, char **argv)
{
    int16_t hum;

    if (read_sensor(NULL, &hum, argc, argv)) {
        return -1;
    }

    printf("Relative humidity: %i.%02i%%\n", (int)hum / 100, (int)hum % 100);
    return 0;
}

int _get_temperature_handler(int argc, char **argv)
{
    int16_t temp;

    if (read_sensor(&temp, NULL, argc, argv)) {
        return -1;
    }

    printf("Temperature: %i.%02i°C\n", (int)temp / 100, (int)temp % 100);
    return 0;
}

int _get_weather_handler(int argc, char **argv)
{
    int16_t hum;
    int16_t temp;

    if (read_sensor(&temp, &hum, argc, argv)) {
        return -1;
    }

    printf("Relative humidity: %i.%02i%%\n", (int)hum / 100, (int)hum % 100);
    printf("Temperature: %i.%02i°C\n", (int)temp / 100, (int)temp % 100);
    return 0;
}

static void print_config(const sht1x_dev_t *dev)
{
    const char *vdds[] = { "5.0", "4.0", "3.5", "3.0", "2.5" };

    printf("Sensor VDD = %s\n", vdds[dev->vdd]);
    printf("Temperature offset [-t]: %i\n", (int)dev->temp_off);
    printf("Humidity offset [-h]: %i\n", (int)dev->hum_off);
    printf("Resolution [-r]: %s\n",
           (dev->conf & SHT1X_CONF_LOW_RESOLUTION) ? "low" : "high");
    printf("Skip calibration (faster reading) [-c]: %s\n",
           (dev->conf & SHT1X_CONF_SKIP_CALIBRATION) ? "yes" : "no");
    printf("Heater [-H]: %s\n",
           (dev->conf & SHT1X_CONF_ENABLE_HEATER) ? "on" : "off");
    printf("CRC checking [-C]: %s\n",
           (dev->conf & SHT1X_CONF_SKIP_CRC) ? "off" : "on");
}

static void unknown_parameter(int index, char **argv)
{
    printf("Unknown parameter \"%s\"\n"
           "Usage: \"%s [PARAMS]\", run \"%s --help\" for help\n",
           argv[index], argv[0], argv[0]);
}

static void missing_argument(int index, char **argv)
{
    printf("Missing argument for parameter \"%s\"\n"
           "Usage: \"%s [%s <ARG>][PARAMS]\", run \"%s --help\" for help\n",
           argv[index], argv[0], argv[index], argv[0]);
}

static void invalid_argument(int index, char **argv, const char *valid)
{
    printf("Invalid argument \"%s\" for parameter \"%s\"\n"
           "Valid arguments are: \"%s\", run \"%s --help\" for help\n",
           argv[index + 1], argv[index], valid, argv[0]);
}

int _sht_config_handler(int argc, char **argv)
{
    uint8_t set_conf = 0;
    uint8_t unset_conf = 0;
    int16_t temp_off = INT16_MAX;
    int16_t hum_off = INT16_MAX;
    int dev_num = 0;

    if ((argc == 2) && (strcmp("--help", argv[1]) == 0)) {
        printf("Usage: \"%s [PARAMS]\n"
               "\n"
               "Supported parameters:\n"
               "  -d <NUM>\n"
               "    Use SHT10/11/15 sensor number <NUM>. Default: 0\n"
               "\n"
               "  -t <OFFSET>\n"
               "    Add <OFFSET> (in e-2°C) to all temperature measurements\n"
               "\n"
               "  -h <OFFSET>\n"
               "    Add <OFFSET> (in e-2%%) to all humidity measurements\n"
               "\n"
               "  -r l/h\n"
               "    Set resolution to low/high. Low resolution trades "
               "precision for speed\n"
               "\n"
               "  -H y/n\n"
               "    Turns heater on/off. Can increase temperature by up to "
               "10°C\n"
               "\n"
               "  -C y/n\n"
               "    Turns on/off CRC checking. No checking trades robustness "
               "for speed\n",
               argv[0]);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if ((argv[i][0] != '-') || (!argv[i][1]) || (argv[i][2])) {
            unknown_parameter(i, argv);
            return -1;
        }

        switch (argv[i][1]) {
        case 'd':
            if (++i >= argc) {
                missing_argument(i - 1, argv);
                return -1;
            }
            dev_num = atoi(argv[i]);
            if ((dev_num < 0) || (dev_num >= (int)SHT1X_NUM)) {
                printf("No SHT10/11/15 sensor with number %i\n", dev_num);
                return -1;
            }
            break;
        case 't':
            if (++i >= argc) {
                missing_argument(i - 1, argv);
                return -1;
            }
            temp_off = atoi(argv[i]);
            break;
        case 'h':
            if (++i >= argc) {
                missing_argument(i - 1, argv);
                return -1;
            }
            hum_off = atoi(argv[i]);
            break;
        case 'r':
            if (++i >= argc) {
                missing_argument(i - 1, argv);
                return -1;
            }
            if ((!argv[i][0]) || (argv[i][1])) {
                invalid_argument(i - 1, argv, "l, h");
                return -1;
            }
            switch (argv[i][0]) {
            case 'l':
                set_conf |= SHT1X_CONF_LOW_RESOLUTION;
                break;
            case 'h':
                unset_conf |= SHT1X_CONF_LOW_RESOLUTION;
                break;
            default:
                invalid_argument(i - 1, argv, "l, h");
                return -1;
            }
            break;
        case 'c':
            if (++i >= argc) {
                missing_argument(i - 1, argv);
                return -1;
            }
            if ((!argv[i][0]) || (argv[i][1])) {
                invalid_argument(i - 1, argv, "y, n");
                return -1;
            }
            switch (argv[i][0]) {
            case 'y':
                set_conf |= SHT1X_CONF_SKIP_CALIBRATION;
                break;
            case 'n':
                unset_conf |= SHT1X_CONF_SKIP_CALIBRATION;
                break;
            default:
                invalid_argument(i - 1, argv, "y, n");
                return -1;
            }
            break;
        case 'H':
            if (++i >= argc) {
                missing_argument(i - 1, argv);
                return -1;
            }
            if ((!argv[i][0]) || (argv[i][1])) {
                invalid_argument(i - 1, argv, "y, n");
                return -1;
            }
            switch (argv[i][0]) {
            case 'y':
                set_conf |= SHT1X_CONF_ENABLE_HEATER;
                break;
            case 'n':
                unset_conf |= SHT1X_CONF_ENABLE_HEATER;
                break;
            default:
                invalid_argument(i - 1, argv, "y, n");
                return -1;
            }
            break;
        case 'C':
            if (++i >= argc) {
                missing_argument(i - 1, argv);
                return -1;
            }
            if ((!argv[i][0]) || (argv[i][1])) {
                invalid_argument(i - 1, argv, "y, n");
                return -1;
            }
            switch (argv[i][0]) {
            case 'y':
                unset_conf |= SHT1X_CONF_SKIP_CRC;
                break;
            case 'n':
                set_conf |= SHT1X_CONF_SKIP_CRC;
                break;
            default:
                invalid_argument(i - 1, argv, "y, n");
                return -1;
            }
            break;
        default:
            unknown_parameter(i, argv);
            return -1;
        }
    }

    if ((set_conf) || (unset_conf)) {
        /* Apply new configuration */
        uint8_t new_conf = sht1x_devs[dev_num].conf;
        new_conf &= ~(unset_conf);
        new_conf |= set_conf;
        switch (sht1x_configure(&sht1x_devs[dev_num], new_conf)) {
        case 0:
            break;
        case -EIO:
            error_msg("gpio_init() failed");
            return -1;
        case -EBADMSG:
            error_msg("CRC checksum error");
            return -1;
        case -ECANCELED:
            error_msg("Sensor did not apply configuration");
            return -1;
        case -EPROTO:
            error_msg("Sensor did not acknowledge command");
            return -1;
        default:
            /* Should never happen, but better safe the sorry */
            error_msg("Unknown error");
            return -1;
        }
    }

    if (temp_off != INT16_MAX) {
        if ((temp_off > 2000) || (temp_off < -2000)) {
            printf("A temperature offset of %i.%02i°C is unreasonable\n",
                   (int)temp_off / 100, (int)temp_off % 100);
            return -1;
        }
        sht1x_devs[dev_num].temp_off = temp_off;
    }

    if (hum_off != INT16_MAX) {
        if ((hum_off > 1000) || (hum_off < -1000)) {
            printf("A humidity offset of %i.%02i%% is unreasonable\n",
                   (int)hum_off / 100, (int)hum_off % 100);
            return -1;
        }
        sht1x_devs[dev_num].hum_off = hum_off;
    }

    print_config(&sht1x_devs[dev_num]);
    return 0;
}
