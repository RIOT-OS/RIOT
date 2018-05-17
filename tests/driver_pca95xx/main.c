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

#include "gpio.h"
#include "irq.h"
#include "pca95xx.h"
#include "pca95xx_params.h"
#include "shell.h"

/* Define device descriptor */
pca95xx_dev_t pca95xx_dev;

static void cb(void *arg)
{
    printf("PCA95xx PCINT on pin %i\n", (uint8_t)arg);
}

static int init_out(int argc, char **argv)
{
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    return pca95xx_init_pin(&pca95xx_dev, pin, GPIO_OUT);
}

static int init_in(int argc, char **argv)
{
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    return pca95xx_init_pin(&pca95xx_dev, pin, GPIO_IN);
}

static int init_od(int argc, char **argv)
{
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    return pca95xx_init_pin(&pca95xx_dev, pin, GPIO_OD);
}

static int init_int(int argc, char **argv)
{
    /* pin must fit into a void pointer */
    uint8_t pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    pca95xx_init_int(&pca95xx_dev, pin, GPIO_IN, GPIO_BOTH, cb, (void *)pin);

    return 0;
}

static int irq_en(int argc, char **argv)
{
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    pca95xx_irq_enable(&pca95xx_dev, pin);

    return 0;
}

static int irq_dis(int argc, char **argv)
{
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    pca95xx_irq_disable(&pca95xx_dev, pin);

    return 0;
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
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    pca95xx_set(&pca95xx_dev, pin);

    return 0;
}

static int clear(int argc, char **argv)
{
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    pca95xx_clear(&pca95xx_dev, pin);

    return 0;
}

static int toggle(int argc, char **argv)
{
    int pin;

    if (argc < 2) {
        printf("usage: %s <pin>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);

    pca95xx_toggle(&pca95xx_dev, pin);

    return 0;
}


static int write(int argc, char **argv)
{
    int pin;
    int val;

    if (argc < 3) {
        printf("usage: %s <pin> <value>\n", argv[0]);
        return 1;
    }

    pin = atoi(argv[1]);
    val = atoi(argv[2]);

    pca95xx_write(&pca95xx_dev, pin, val);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init_out", "init as output (push-pull mode)", init_out },
    { "init_in", "init as input", init_in },
    { "init_od", "init as output (open-drain mode)", init_od },
    { "init_int", "configure interrupt", init_int },
    { "irq_en", "enable interrupt", irq_en },
    { "irq_dis" "disable interrupt", irq_dis },
    { "read", "read pin status", read },
    { "set", "set pin to HIGH", set },
    { "clear", "set pin to LOW", clear },
    { "toggle", "toggle pin", toggle },
    { "write", "write value to pin", write },
    { NULL, NULL, NULL }
};

int main(void)
{
    int ret;

    puts("PCA95xx I2C GPIO expander driver test");

    puts("Initializing PCA95xx");
    ret = pca95xx_init(&pca95xx_dev, &pca95xx_params);
    if (ret < 0) {
        printf("Error: pca95xx_init returned %i", ret);
    }

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
