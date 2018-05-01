/*
 * Copyright (C) 2014,2017 Freie Universität Berlin
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application PCA95xx GPIO expanders
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "irq.h"
#include "pca95xx.h"
#include "pca95xx_params.h"
#include "shell.h"

static pca95xx_t pca95xx_dev = { .params.i2c   = PCA95XX_PARAM_I2C,
                                 .params.addr  = PCA95XX_PARAM_ADDR,
                                 .params.flags = PCA95XX_PARAM_FLAGS };

static pca95xx_int_t pca95xx_int_dev = { .params.i2c     = PCA95XX_PARAM_I2C,
                                         .params.addr    = PCA95XX_PARAM_ADDR,
                                         .params.int_pin = PCA95XX_PARAM_INT_PIN };

static void cb(void *arg)
{
    (void)arg;
    puts("PCA95xx pin change interrupt");
}

static int init_pin(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pca95xx_dev.params.pin = atoi(argv[1]);

    if (pca95xx_init(&pca95xx_dev, &(pca95xx_dev.params)) < 0) {
        printf("Error initializing PCA95xx pin %i\n", pca95xx_dev.params.pin);
        return 1;
    }

    return 0;
}

static int init_out(int argc, char **argv)
{
    pca95xx_dev.params.flags |= PCA95XX_HIGH_DRIVE | PCA95XX_LOW_DRIVE;

    return init_pin(argc, argv);
}

static int init_in(int argc, char **argv)
{
    pca95xx_dev.params.flags &= ~(PCA95XX_HIGH_DRIVE | PCA95XX_LOW_DRIVE);

    return init_pin(argc, argv);
}

static int init_od(int argc, char **argv)
{
    pca95xx_dev.params.flags &= ~PCA95XX_HIGH_DRIVE;
    pca95xx_dev.params.flags |= PCA95XX_LOW_DRIVE;

    return init_pin(argc, argv);
}

static int read(int argc, char **argv)
{
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    if (pca95xx_read(&pca95xx_dev, pin)) {
        printf("PCA95xx pin %i is HIGH\n", pin);
    }
    else {
        printf("PCA95xx pin %i is LOW\n", pin);
    }

    return 0;
}

static int set(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pca95xx_set(&pca95xx_dev, atoi(argv[1]));

    return 0;
}

static int clear(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pca95xx_clear(&pca95xx_dev, atoi(argv[1]));

    return 0;
}

static int toggle(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pca95xx_toggle(&pca95xx_dev, atoi(argv[1]));

    return 0;
}


static int write(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <pin> <value>\n", argv[0]);
        return 1;
    }

    pca95xx_write(&pca95xx_dev, atoi(argv[1]), atoi(argv[2]));

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init_out", "init as output (push-pull mode)", init_out },
    { "init_in", "init as input", init_in },
    { "init_od", "init as output (open-drain mode)", init_od },
    { "read", "read pin status", read },
    { "set", "set pin to HIGH", set },
    { "clear", "set pin to LOW", clear },
    { "toggle", "toggle pin", toggle },
    { "write", "write value to pin", write },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("PCA95xx I2C GPIO expander driver test\n");

    /* Enable pin change interrupts */
    pca95xx_int_dev.cb = cb;
    pca95xx_int_init(&pca95xx_int_dev, &(pca95xx_int_dev.params));

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
