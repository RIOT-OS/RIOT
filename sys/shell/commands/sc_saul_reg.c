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
#include <string.h>
#include <stdlib.h>

#include "saul_reg.h"
#include "fmt.h"

/* this function does not check, if the given device is valid */
static void probe(int num, saul_reg_t *dev)
{
    int dim;
    phydat_t res;

    dim = saul_reg_read(dev, &res);
    if (dim <= 0) {
        print_str("error: failed to read from device #");
        print_s32_dec(num);
        print_str("\n");
        return;
    }
    /* print results */
    print_str("Reading from #");
    print_s32_dec(num);
    print_str(" (");
    print_str(dev->name);
    print_str("|");
    print_str(saul_class_to_str(dev->driver->type));
    print_str(")\n");
    phydat_dump(&res, dim);
}

static void probe_all(void)
{
    saul_reg_t *dev = saul_reg;
    int i = 0;

    while (dev) {
        probe(i++, dev);
        print_str("\n");
        dev = dev->next;
    }
}

static void list(void)
{
    saul_reg_t *dev = saul_reg;
    int i = 0;

    if (dev) {
        print_str("ID\tClass\t\tName\n");
    }
    else {
        print_str("No devices found\n");
    }
    while (dev) {
        print_str("#");
        print_s32_dec(i++);
        print_str("\t");
        print_str(saul_class_to_str(dev->driver->type));
        print_str("\t");
        print_str(dev->name);
        print_str("\n");
        dev = dev->next;
    }
}

static void read(int argc, char **argv)
{
    int num;
    saul_reg_t *dev;

    if (argc < 3) {
        print_str("usage: ");
        print_str(argv[0]);
        print_str(" ");
        print_str(argv[1]);
        print_str(" <device id>|all\n");
        return;
    }
    if (strcmp(argv[2], "all") == 0) {
        probe_all();
        return;
    }
    /* get device id */
    num = atoi(argv[2]);
    dev = saul_reg_find_nth(num);
    if (dev == NULL) {
        print_str("error: undefined device id given\n");
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
        print_str("usage: ");
        print_str(argv[0]);
        print_str(" ");
        print_str(argv[1]);
        print_str(" <device id> <value 0> [<value 1> [<value 2]]\n");
        return;
    }
    num = atoi(argv[2]);
    dev = saul_reg_find_nth(num);
    if (dev == NULL) {
        print_str("error: undefined device given\n");
        return;
    }
    /* parse value(s) */
    memset(&data, 0, sizeof(data));
    dim = ((argc - 3) > (int)PHYDAT_DIM) ? (int)PHYDAT_DIM : (argc - 3);
    for (int i = 0; i < dim; i++) {
        data.val[i] = atoi(argv[i + 3]);
    }
    /* print values before writing */
    print_str("Writing to device #");
    print_s32_dec(num);
    print_str(" - ");
    print_str(dev->name);
    print_str("\n");
    phydat_dump(&data, dim);
    /* write values to device */
    dim = saul_reg_write(dev, &data);
    if (dim <= 0) {
        if (dim == -ENOTSUP) {
            print_str("error: device #");
            print_s32_dec(num);
            print_str(" is not writable\n");
        }
        else {
            print_str("error: failure to write to device #");
            print_s32_dec(num);
            print_str("\n");
        }
        return;
    }
    print_str("data successfully written to device #");
    print_s32_dec(num);
    print_str("\n");
}

int _saul(int argc, char **argv)
{
    if (argc < 2) {
        list();
    }
    else {
        if (strcmp(argv[1], "read") == 0) {
            read(argc, argv);
        }
        else if (strcmp(argv[1], "write") == 0) {
            write(argc, argv);
        }
        else {
            print_str("usage: ");
            print_str(argv[0]);
            print_str(" read|write\n");
        }
    }
    return 0;
}
