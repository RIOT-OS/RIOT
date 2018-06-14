/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2018 Acutam Automation, LLC
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
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "saul_reg.h"

/* this function does not check if the given device is valid */
static void probe(unsigned num, saul_ctxt_ptr_t ctxt_ptr, void *arg)
{
    (void)arg;

    int dim;
    phydat_t res;

    dim = saul_reg_read(ctxt_ptr.reg, ctxt_ptr.ctxt, &res);
    if (dim <= 0) {
        printf("error: failed to read from device #%u\n", num);
        return;
    }

    /* print results */
    printf("Reading from #%u (", num);

    /* replace context item number in name */
    printf(ctxt_ptr.reg->name, ctxt_ptr.num);

    printf("|%s)\n", saul_class_to_str(ctxt_ptr.reg->driver->type));

    phydat_dump(&res, dim);
}

static void probe_all(void)
{
    saul_reg_iter(probe, NULL);
}

static void list_print_item(unsigned num, saul_ctxt_ptr_t ctxt_ptr, void *arg)
{
    (void)arg;

    printf("#%u\t%s\t", num, saul_class_to_str(ctxt_ptr.reg->driver->type));

    /* replace context item number in name */
    printf(ctxt_ptr.reg->name, ctxt_ptr.num);

    /* print newline */
    puts("");
}

static void list(void)
{
    if (saul_reg) {
        puts("ID\tClass\t\tName");
    }
    else {
        puts("No devices found");
    }

    saul_reg_iter(list_print_item, NULL);
}

static void read(int argc, char **argv)
{
    unsigned num;
    saul_ctxt_ptr_t ctxt_ptr;

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
    if (saul_reg_find_nth(&ctxt_ptr, num) == NULL) {
        puts("error: undefined device id given");
        return;
    }
    probe(num, ctxt_ptr, NULL);
}

static void write(int argc, char **argv)
{
    int num, dim;
    saul_ctxt_ptr_t ctxt_ptr;
    phydat_t data;

    if (argc < 4) {
        printf("usage: %s %s <device id> <value 0> [<value 1> [<value 2]]\n",
               argv[0], argv[1]);
        return;
    }
    num = atoi(argv[2]);
    if (saul_reg_find_nth(&ctxt_ptr, num) == NULL) {
        puts("error: undefined device given");
        return;
    }
    /* parse value(s) */
    memset(&data, 0, sizeof(data));
    dim = ((argc - 3) > (int)PHYDAT_DIM) ? (int)PHYDAT_DIM : (argc - 3);
    for (int i = 0; i < dim; i++) {
        data.val[i] = atoi(argv[i + 3]);
    }

    /* print values before writing */
    printf("Writing to device #%i - ", num);
    /* replace context in name */
    printf(ctxt_ptr.reg->name, ctxt_ptr.num);
    puts("");

    phydat_dump(&data, dim);
    /* write values to device */
    dim = saul_reg_write(ctxt_ptr.reg, ctxt_ptr.ctxt, &data);
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
    else {
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
