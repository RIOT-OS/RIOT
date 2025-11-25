/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       SAUL registry shell commands
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "flash_utils.h"
#include "saul_reg.h"
#include "shell.h"

static const char *_devname(saul_reg_t *dev) {
    if (dev->name == NULL) {
        return "(no name)";
    } else {
        return dev->name;
    }
}

/* this function does not check, if the given device is valid */
static void probe(int num, saul_reg_t *dev)
{
    int dim;
    phydat_t res;

    dim = saul_reg_read(dev, &res);
    if (dim <= 0) {
        printf("error: failed to read from device #%i\n", num);
        return;
    }
    /* print results */
    printf("Reading from #%i (%s|", num, _devname(dev));
    saul_class_print(dev->driver->type);
    printf(")\n");
    phydat_dump(&res, dim);
}

static void probe_all(void)
{
    saul_reg_t *dev = saul_reg;
    int i = 0;

    while (dev) {
        probe(i++, dev);
        puts("");
        dev = dev->next;
    }
}

static void list(void)
{
    saul_reg_t *dev = saul_reg;
    int i = 0;

    if (dev) {
        printf("ID\tClass\t\tName\n");
    }
    else {
        printf("No devices found\n");
    }
    while (dev) {
        printf("#%i\t", i++);
        saul_class_print(dev->driver->type);
        printf("\t%s\n", _devname(dev));
        dev = dev->next;
    }
}

static void read(int argc, char **argv)
{
    int num;
    saul_reg_t *dev;

    if (argc < 3) {
        printf("usage: %s %s <device id>|all\n",
                     argv[0], argv[1]);
        return;
    }
    if (flash_strcmp(argv[2], TO_FLASH("all")) == 0) {
        probe_all();
        return;
    }
    /* get device id */
    num = atoi(argv[2]);
    dev = saul_reg_find_nth(num);
    if (dev == NULL) {
        printf("error: undefined device id given\n");
        return;
    }
    probe(num, dev);
}

static void write(int argc, char **argv)
{
    int num, dim;
    saul_reg_t *dev;
    phydat_t data;

    if (argc < 4) {
        printf("usage: %s %s <device id> <value 0> [<value 1> [<value 2]]\n",
                     argv[0], argv[1]);
        return;
    }
    num = atoi(argv[2]);
    dev = saul_reg_find_nth(num);
    if (dev == NULL) {
        printf("error: undefined device given\n");
        return;
    }
    /* parse value(s) */
    memset(&data, 0, sizeof(data));
    dim = ((argc - 3) > (int)PHYDAT_DIM) ? (int)PHYDAT_DIM : (argc - 3);
    for (int i = 0; i < dim; i++) {
        data.val[i] = atoi(argv[i + 3]);
    }
    /* print values before writing */
    printf("Writing to device #%i - %s\n", num, _devname(dev));
    phydat_dump(&data, dim);
    /* write values to device */
    dim = saul_reg_write(dev, &data);
    if (dim <= 0) {
        if (dim == -ENOTSUP) {
            printf("error: device #%i is not writable\n", num);
        }
        else {
            printf("error: failure to write to device #%i\n", num);
        }
        return;
    }
    printf("data successfully written to device #%i\n", num);
}

static int _saul(int argc, char **argv)
{
    if (argc < 2) {
        list();
    }
    else {
        if (flash_strcmp(argv[1], TO_FLASH("read")) == 0) {
            read(argc, argv);
        }
        else if (flash_strcmp(argv[1], TO_FLASH("write")) == 0) {
            write(argc, argv);
        }
        else {
            printf("usage: %s read|write\n", argv[0]);
        }
    }
    return 0;
}

SHELL_COMMAND(saul, "interact with sensors and actuators using SAUL", _saul);
