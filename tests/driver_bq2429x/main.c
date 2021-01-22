/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the BQ24298x device driver
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @file
 */

#include <stdio.h>
#include <stdlib.h>

#include "bq2429x_params.h"
#include "shell.h"
#include "_util.h"

static bq2429x_t _dev;

/* Status and faults */
static int _bq2429x_get_status_cmd(int argc, char **argv);
static int _bq2429x_get_fault_cmd(int argc, char **argv);
/* OTG */
static int _bq2429x_enable_otg_cmd(int argc, char **argv);
static int _bq2429x_disable_otg_cmd(int argc, char **argv);
/* Charge */
static int _bq2429x_enable_chg_cmd(int argc, char **argv);
static int _bq2429x_disable_chg_cmd(int argc, char **argv);
/* Set charge parameters */
static int _bq2429x_set_vlim_cmd(int argc, char **argv);
static int _bq2429x_set_ilim_cmd(int argc, char **argv);
static int _bq2429x_set_ichg_cmd(int argc, char **argv);
static int _bq2429x_set_vreg_cmd(int argc, char **argv);
/* Get charge parameters */
static int _bq2429x_get_vlim_cmd(int argc, char **argv);
static int _bq2429x_get_ilim_cmd(int argc, char **argv);
static int _bq2429x_get_ichg_cmd(int argc, char **argv);
static int _bq2429x_get_vreg_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    /* Status and faults */
    { "bq2429x_get_status", "Get device status", _bq2429x_get_status_cmd },
    { "bq2429x_get_fault", "Get device faults", _bq2429x_get_fault_cmd },
    /* OTG */
    { "bq2429x_enable_otg", "Enable OTG mode", _bq2429x_enable_otg_cmd },
    { "bq2429x_disable_otg", "Disable OTG mode", _bq2429x_disable_otg_cmd },
    /* Charge */
    { "bq2429x_enable_charge", "Enable battery charge",
      _bq2429x_enable_chg_cmd },
    { "bq2429x_disable_charge", "Disable battery charge",
      _bq2429x_disable_chg_cmd },
    /* Set charge parameters */
    { "bq2429x_set_vlim", "Set input voltage limit (0=3880 mV .. 15=5080 mV)",
      _bq2429x_set_vlim_cmd },
    { "bq2429x_set_ilim", "Set input current limit (0=100 mA .. 7=3000 mA)",
      _bq2429x_set_ilim_cmd },
    { "bq2429x_set_ichg", "Set charge current (0=512 mA .. 5=4544 mA)",
      _bq2429x_set_ichg_cmd },
    { "bq2429x_set_vreg", "Set charge voltage limit (0=3504 mV .. 9=4400 mV)",
      _bq2429x_set_vreg_cmd },
    /* Get charge parameters */
    { "bq2429x_get_vlim", "Get input voltage limit", _bq2429x_get_vlim_cmd },
    { "bq2429x_get_ilim", "Get input current limit", _bq2429x_get_ilim_cmd },
    { "bq2429x_get_ichg", "Get charge current", _bq2429x_get_ichg_cmd },
    { "bq2429x_get_vreg", "Get charge voltage limit", _bq2429x_get_vreg_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{

    puts("BQ2429x driver test application\n");

    if (bq2429x_init(&_dev, &bq2429x_params[0]) != BQ2429X_OK) {
        puts("[Failed]");
        return 1;
    }

    puts("[OK]\n");

    puts("All up, running the shell now.");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}

static int _bq2429x_get_status_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int ret;
    bq2429x_status_t status;
    if ((ret = bq2429x_get_status(&_dev, &status)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }

    printf("Device status:\n");
    printf("==============\n\n");

    printf("VBUS: %s\n", _util_vbus_stat_to_str(status.vbus));
    printf("Charge: %s\n", _util_chrg_stat_to_str(status.chrg));
    printf("DPM: %s\n", status.dpm ? "VINDPM or IINDPM" : "Not in DPM");
    printf("Power Good: %s\n", status.pg ? "Yes" : "No");
    printf("Thermal Regulation: %s\n", status.therm ? "In thermal retulation"
                                                    : "Normal");
    printf("In VSYSMIN Regulation (BAT < VSYSMIN): %s\n", status.vsys ? "Yes"
                                                                      : "No");

    return 0;
}

static int _bq2429x_get_fault_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int ret;
    bq2429x_fault_t fault;
    if ((ret = bq2429x_get_fault(&_dev, &fault)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }

    printf("Device faults:\n");
    printf("==============\n\n");

    printf("Watchdog: %s\n", fault.watchdog ? "Yes" : "No");
    printf("OTG: %s\n", fault.otg ? "Yes" : "No");
    printf("Charge: %s\n", _util_chrg_fault_to_str(fault.chrg));
    printf("Battery: %s\n", fault.bat ? "Yes" : "No");
    printf("NTC Fault 0: %s\n", fault.ntc_fault_0 ? "Yes" : "No");
    printf("NTC Fault 1: %s\n", fault.ntc_fault_1 ? "Yes" : "No");

    return 0;
}

static int _bq2429x_enable_otg_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int ret;
    if ((ret = bq2429x_enable_otg(&_dev))) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    puts("OTG enabled");
    return 0;
}

static int _bq2429x_disable_otg_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int ret;
    if ((ret = bq2429x_disable_otg(&_dev))) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    puts("OTG disabled");
    return 0;
}

static int _bq2429x_enable_chg_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int ret;
    if ((ret = bq2429x_enable_charge(&_dev))) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    puts("Battery charge enabled");
    return 0;
}

static int _bq2429x_disable_chg_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int ret;
    if ((ret = bq2429x_disable_charge(&_dev))) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    puts("Battery charge disabled");
    return 0;
}

static int _bq2429x_set_vlim_cmd(int argc, char **argv)
{
    if (argc < 2 || argc > 2) {
        printf("usage: %s [vlim]\n", argv[0]);
        return 1;
    }

    int vlim = atoi(argv[1]);
    if (vlim < 0) {
        puts("error: only positive values allowed");
        return 1;
    }
    if (vlim > BQ2429X_VLIM_5080) {
        puts("error: maximum value is 5.08 V (15)");
        return 1;
    }

    int ret;
    if ((ret = bq2429x_set_vlim(&_dev, vlim)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    printf("VLIM set to %s\n", _util_vlim_to_str(vlim));

    return 0;
}

static int _bq2429x_set_ilim_cmd(int argc, char **argv)
{
    if (argc < 2 || argc > 2) {
        printf("usage: %s [ilim]\n", argv[0]);
        return 1;
    }

    int ilim = atoi(argv[1]);
    if (ilim < 0) {
        puts("error: only positive values allowed");
        return 1;
    }
    if (ilim > BQ2429X_ILIM_3000) {
        puts("error: maximum value is 3.0 A (7)");
        return 1;
    }

    int ret;
    if ((ret = bq2429x_set_ilim(&_dev, ilim)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    printf("ILIM set to %s\n", _util_ilim_to_str(ilim));

    return 0;
}

static int _bq2429x_set_ichg_cmd(int argc, char **argv)
{
    const bq2429x_charge_current_t ichg_lookup_table[] = {
        BQ2429X_ICHG_512,
        BQ2429X_ICHG_1024,
        BQ2429X_ICHG_2048,
        BQ2429X_ICHG_3008,
        BQ2429X_ICHG_4032,
        BQ2429X_ICHG_4544,
    };

    if (argc < 2 || argc > 2) {
        printf("usage: %s [ichg]\n", argv[0]);
        return 1;
    }

    int i = atoi(argv[1]);
    if (i < 0) {
        puts("error: only positive values allowed");
        return 1;
    }
    if (i >= (int)ARRAY_SIZE(ichg_lookup_table)) {
        int last = ARRAY_SIZE(ichg_lookup_table) - 1;
        printf("error: maximum value is %s (%d)\n",
               _util_ichg_to_str(ichg_lookup_table[last]), last);
        return 1;
    }

    bq2429x_charge_current_t ichg = ichg_lookup_table[i];
    int ret;
    if ((ret = bq2429x_set_ichg(&_dev, ichg)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    printf("ICHG set to %s\n", _util_ichg_to_str(ichg));

    return 0;
}

static int _bq2429x_set_vreg_cmd(int argc, char **argv)
{
    const bq2429x_charge_voltage_limit_t vreg_lookup_table[] = {
        BQ2429X_VREG_3504,
        BQ2429X_VREG_3600,
        BQ2429X_VREG_3808,
        BQ2429X_VREG_3904,
        BQ2429X_VREG_4000,
        BQ2429X_VREG_4112,
        BQ2429X_VREG_4208,
        BQ2429X_VREG_4304,
        BQ2429X_VREG_4352,
        BQ2429X_VREG_4400,
    };

    if (argc < 2 || argc > 2) {
        printf("usage: %s [vreg]\n", argv[0]);
        return 1;
    }

    int i = atoi(argv[1]);
    if (i < 0) {
        puts("error: only positive values allowed");
        return 1;
    }
    if (i >= (int)ARRAY_SIZE(vreg_lookup_table)) {
        int last = ARRAY_SIZE(vreg_lookup_table) - 1;
        printf("error: maximum value is %s (%d)\n",
               _util_vreg_to_str(vreg_lookup_table[last]), last);
        return 1;
    }

    bq2429x_charge_voltage_limit_t vreg = vreg_lookup_table[i];
    int ret;
    if ((ret = bq2429x_set_vreg(&_dev, vreg)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    printf("VREG set to %s\n", _util_vreg_to_str(vreg));

    return 0;
}

static int _bq2429x_get_vlim_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    bq2429x_input_voltage_limit_t vlim;
    int ret;

    if ((ret = bq2429x_get_vlim(&_dev, &vlim)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    printf("VLIM: %s\n", _util_vlim_to_str(vlim));

    return 0;
}

static int _bq2429x_get_ilim_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    bq2429x_input_current_limit_t ilim;
    int ret;

    if ((ret = bq2429x_get_ilim(&_dev, &ilim)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    printf("ILIM: %s\n", _util_ilim_to_str(ilim));

    return 0;
}

static int _bq2429x_get_ichg_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    bq2429x_charge_current_t ichg;
    int ret;

    if ((ret = bq2429x_get_ichg(&_dev, &ichg)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    printf("ICHG: %s\n", _util_ichg_to_str(ichg));

    return 0;
}

static int _bq2429x_get_vreg_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    bq2429x_charge_voltage_limit_t vreg;
    int ret;

    if ((ret = bq2429x_get_vreg(&_dev, &vreg)) != BQ2429X_OK) {
        printf("[Failed] ret=%d\n", ret);
        return 1;
    }
    printf("VREG: %s\n", _util_vreg_to_str(vreg));

    return 0;
}
