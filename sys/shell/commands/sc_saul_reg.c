/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "saul_reg.h"

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
    printf("Reading from #%i (%s|%s)\n", num, dev->name,
           saul_class_to_str(dev->driver->type));
    phydat_dump(&res, dim);
}

static void probe_all(void)
{
    saul_reg_t *dev = saul_reg_get();
    int i = 0;

    while (dev) {
        probe(i++, dev);
        puts("");
        dev = dev->next;
    }
}

static void list(void)
{
    saul_reg_t *dev = saul_reg_get();
    int i = 0;

    if (dev) {
        puts("ID\tClass\t\tName");
    }
    else {
        puts("No devices found");
    }
    while (dev) {
        printf("#%i\t%s\t%s\n",
               i++, saul_class_to_str(dev->driver->type), dev->name);
        dev = dev->next;
    }
}

static void read(int argc, char **argv)
{
    int num;
    saul_reg_t *dev;

    if (argc < 3) {
        printf("usage: %s %s <device id>|all\n", argv[0], argv[1]);
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
        puts("error: undefined device id given");
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
        puts("error: undefined device given");
        return;
    }
    /* parse value(s) */
    memset(&data, 0, sizeof(data));
    dim = ((argc - 3) > (int)PHYDAT_DIM) ? (int)PHYDAT_DIM : (argc - 3);
    for (int i = 0; i < dim; i++) {
        data.val[i] = (int16_t)atoi(argv[i + 3]);
    }
    /* print values before writing */
    printf("Writing to device #%i - %s\n", num, dev->name);
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

int _saul(int argc, char **argv)
{
    if (argc < 2) {
        list();
    }
    else if (argc >= 2) {
        if (strcmp(argv[1], "read") == 0) {
            read(argc, argv);
        }
        else if (strcmp(argv[1], "write") == 0) {
            write(argc, argv);
        }
        else {
            printf("usage: %s read|write\n", argv[0]);
        }
    }
    return 0;
}
