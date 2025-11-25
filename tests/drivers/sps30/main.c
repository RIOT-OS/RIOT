/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test application for the Sensirion SPS30 device driver
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "timex.h"
#include "ztimer.h"
#include "sps30.h"
#include "sps30_params.h"

#define TEST_START_DELAY_MS         (2 * MS_PER_SEC)
#define SENSOR_RESET_DELAY_MS       (10 * MS_PER_SEC)
#define SENSOR_STARTUP_DELAY_MS     (10 * MS_PER_SEC)
#define SENSOR_SLEEP_WAKE_DELAY_MS  (5 * MS_PER_SEC)
#define POLL_FOR_READY_MS           (1 * MS_PER_SEC)
#define NUM_OF_MEASUREMENTS         (10U)

#define TYPE_MC_STR  "MC PM"
#define TYPE_NC_STR  "NC PM"
#define TYPE_TPS_STR "TPS"
#define MC_UNIT_STR  "[µg/m³]"
#define NC_UNIT_STR  "[#/cm³]"
#define TPS_UNIT_STR "[µm]"

/* prints the result of an operation and returns true if an error occurred */
static bool _print_error(const char *msg, sps30_error_code_t ec)
{
    printf("sps30_%s: [%s]\n", msg, (ec == SPS30_OK) ? "OK" :
                              (ec == SPS30_CRC_ERROR ? "CRC_ERROR"
                                                     : "I2C_ERROR"));
    return ec != SPS30_OK;
}

static void _print_val_row(char *typ1, char *typ2, char *unit, float val)
{
    printf("| %-5s %4s:%3"PRIu32".%03"PRIu32" %-8s |\n", typ1, typ2,
           (uint32_t)val, ((uint32_t)((val + 0.0005) * 1000)) % 1000, unit);
}

int main(void)
{
    sps30_t dev;
    sps30_data_t data;
    sps30_error_code_t ec;
    char str[SPS30_SER_ART_LEN];
    uint32_t ci = 0;   /* clean interval */
    uint32_t nci = 0;  /* new clean interval */
    bool error = false;
    unsigned cnt = NUM_OF_MEASUREMENTS;

    ztimer_sleep(ZTIMER_MSEC, TEST_START_DELAY_MS);

    puts("SPS30 test application\n");

    ec = sps30_init(&dev, &sps30_params[0]);
    error |= _print_error("init", ec);

    ec = sps30_read_article_code(&dev, str, sizeof(str));
    if (ec == SPS30_OK) {
        printf("Article code: %s\n", str);
    } else {
        error |= _print_error("read_article_code", ec);
    }

    ec = sps30_read_serial_number(&dev, str, sizeof(str));
    if (ec == SPS30_OK) {
        printf("Serial: %s\n", str);
    } else {
        error |= _print_error("read_serial_number", ec);
    }

    ec = sps30_start_fan_clean(&dev);
    error |= _print_error("start_fan_clean", ec);

    /* wait long enough for the fan clean to be done and the fan to settle */
    ztimer_sleep(ZTIMER_MSEC, 2 * SPS30_FAN_CLEAN_S);

    /* read the currently set value from the sensor */
    ec = sps30_read_ac_interval(&dev, &ci);
    error |= _print_error("read_ac_interval", ec);

    nci = ci + 1;
    ec = sps30_write_ac_interval(&dev, nci);
    error |= _print_error("write_ac_interval", ec);

    /* resetting the sensor so the updated value can be read */
    ec = sps30_reset(&dev);
    error |= _print_error("reset", ec);

    ztimer_sleep(ZTIMER_MSEC, SENSOR_RESET_DELAY_MS);

    /* Put the sensor in sleep */
    ec = sps30_sleep(&dev);
    error |= _print_error("sleep", ec);
    ztimer_sleep(ZTIMER_MSEC, SENSOR_SLEEP_WAKE_DELAY_MS);

    /* Wake-up the sensor */
    ec = sps30_wakeup(&dev);
    error |= _print_error("wake-up", ec);
    ztimer_sleep(ZTIMER_MSEC, SENSOR_SLEEP_WAKE_DELAY_MS);

    /* start the sensor again again... */
    ec = sps30_start_measurement(&dev);
    error |= _print_error("start_measurement", ec);

    ztimer_sleep(ZTIMER_MSEC, SENSOR_STARTUP_DELAY_MS);

    ec = sps30_read_ac_interval(&dev, &ci);
    error |= _print_error("read_ac_interval", ec);

    if (ci != nci) {
        printf("ERROR: the auto-clean interval was not updated properly! (%"
               PRIu32" != %"PRIu32")\n", ci, nci);
    }

    /* restore the default auto-clean cycle */
    ec = sps30_write_ac_interval(&dev, SPS30_DEFAULT_ACI_S);
    error |= _print_error("write_ac_interval", ec);

    while (cnt) {
        int err_code;
        bool ready = sps30_data_ready(&dev, &err_code);

        if (!ready) {
            if (err_code != SPS30_OK) {
                error |= _print_error("data_ready", err_code);
                cnt--; /* if errors happen, stop after NUM_OF_MEASUREMENTS */
            }
            /* try again after some time */
            ztimer_sleep(ZTIMER_MSEC, POLL_FOR_READY_MS);
            continue;
        }

        ec = sps30_read_measurement(&dev, &data);

        if (ec == SPS30_OK) {
            puts("\nv==== SPS30 measurements ====v");
            _print_val_row(TYPE_MC_STR, "1.0", MC_UNIT_STR, data.mc_pm1);
            _print_val_row(TYPE_MC_STR, "2.5", MC_UNIT_STR, data.mc_pm2_5);
            _print_val_row(TYPE_MC_STR, "4.0", MC_UNIT_STR, data.mc_pm4);
            _print_val_row(TYPE_MC_STR, "10.0", MC_UNIT_STR, data.mc_pm10);
            _print_val_row(TYPE_NC_STR, "0.5", NC_UNIT_STR, data.nc_pm0_5);
            _print_val_row(TYPE_NC_STR, "1.0", NC_UNIT_STR, data.nc_pm1);
            _print_val_row(TYPE_NC_STR, "2.5", NC_UNIT_STR, data.nc_pm2_5);
            _print_val_row(TYPE_NC_STR, "4.0", NC_UNIT_STR, data.nc_pm4);
            _print_val_row(TYPE_NC_STR, "10.0", NC_UNIT_STR, data.nc_pm10);
            _print_val_row(TYPE_TPS_STR, "", TPS_UNIT_STR, data.ps);
            puts("+----------------------------+");
            puts("| MC:  Mass Concentration    |");
            puts("| NC:  Number Concentration  |");
            puts("| TPS: Typical Particle Size |");
            printf("^========= %2u / %2u ==========^\n\n",
                   NUM_OF_MEASUREMENTS - cnt + 1, NUM_OF_MEASUREMENTS);
        } else {
            error |= _print_error("read_measurement", ec);
        }

        cnt--;
    }

    ec = sps30_stop_measurement(&dev);
    error |= _print_error("stop_measurement", ec);

    if (error) {
        puts("sps30 test: [FAILED]");
    }
    else {
        puts("sps30 test: [SUCCESS]");
    }

    return 0;
}
