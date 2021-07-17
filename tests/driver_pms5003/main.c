/*
 * Copyright (C) 2021 Inria
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
 * @brief       PMS5003 driver test application
 *
 * @author      Francisco Molina <francois-xavier.molinas@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "ztimer.h"
#include "shell.h"

#include "pms5003.h"
#include "pms5003_params.h"

static pms5003_t _dev;
static pms5003_callbacks_t _cbs;
static pms5003_data_t _data;

void _print_data(pms5003_data_t *data_ptr)
{
    if (CONFIG_PMS5003_INDOOR_ENVIRONMENT) {
        printf("\tPM1.0: %" PRIu16 " ug/m3 (ultrafine particles)\n",
               data_ptr->values.mc_pm_1);
        printf("\tPM2.5: %" PRIu16 " ug/m3 (combustion particles, organic"
               " compounds, metals)\n", data_ptr->values.mc_pm_2p5);
        printf("\tPM10: %" PRIu16 " ug/m3 (dust, pollen, mould spores)\n",
               data_ptr->values.mc_pm_10);
    }
    else {
        printf("\tPM1.0: %" PRIu16 " ug/m3 (atmospheric environment)\n",
               data_ptr->values.amc_pm_1);
        printf("\tPM2.5: %" PRIu16 " ug/m3 (atmospheric environment)\n",
               data_ptr->values.amc_pm_2p5);
        printf("\tPM10: %" PRIu16 " ug/m3 (atmospheric environment)\n",
               data_ptr->values.amc_pm_10);
    }
    printf("\tNum particles <= 0.3µm: %" PRIu16 "/cm^3 \n",
           data_ptr->values.nc_pm_0p3);
    printf("\tNum particles <= 0.5µm: %" PRIu16 "/cm^3 \n",
           data_ptr->values.nc_pm_0p5);
    printf("\tNum particles <= 1µm: %" PRIu16 "/cm^3 \n",
           data_ptr->values.nc_pm_1);
    printf("\tNum particles <= 2p5µm: %" PRIu16 "/cm^3 \n",
           data_ptr->values.nc_pm_2p5);
    printf("\tNum particles <= 5µm: %" PRIu16 "/cm^3 \n",
           data_ptr->values.nc_pm_5);
    printf("\tNum particles <= 10µm: %" PRIu16 "/cm^3 \n",
           data_ptr->values.nc_pm_10);
}

void _data_ready_cb(const pms5003_data_t *data, void *arg)
{
    (void) arg;
    memcpy(&_data, data, sizeof(pms5003_data_t));
}

int _read(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    if (pms5003_read_measurement(&_dev, &_data) == 0) {
        _print_data(&_data);
    }
    else {
        puts("pms5003: fail to read");
    }
    return 0;
}

int _print(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    _print_data(&_data);
    return 0;
}

int _mode(int argc, char **argv)
{
    if (argc == 1) {
        printf("pms5003: mode %s\n", _dev.mode ? "active" : "passive");
    }
    else {
        if (!strcmp(argv[1], "active")) {
            if (pms5003_set_mode(&_dev, PMS5003_ACTIVE_MODE)) {
                puts("pms5003: failed to set active mode");
            }
        }
        else if (!strcmp(argv[1], "passive")) {
            if (pms5003_set_mode(&_dev, PMS5003_PASSIVE_MODE)) {
                puts("pms5003: failed to set passive mode");
            }
        }
        else {
            printf("pms5003: invalid mode %s, <active, passive>\n", argv[1]);
            return -1;
        }
    }
    return 0;

}

static const shell_command_t shell_commands[] = {
    { "read", "Read sensor data", _read },
    { "print", "Print sensor data", _print },
    { "mode", "Set sensor mode <active|passive>", _mode },
    { NULL, NULL, NULL }
};

int main(void)
{
    pms5003_init(&_dev, &pms5003_params[0]);
    _cbs.cb = _data_ready_cb;
    _cbs.arg = NULL;
    pms5003_add_callbacks(&_dev, &_cbs);
    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
