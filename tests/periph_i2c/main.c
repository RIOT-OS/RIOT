/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the low-level I2C peripheral driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "periph_conf.h"
#include "periph/i2c.h"
#include "shell.h"
#include "posix_io.h"
#include "board_uart0.h"

#define BUFSIZE        (128U)

static int i2c_dev = -1;

int cmd_init_master(int argc, char **argv)
{
    int dev, speed, res;

    if (argc != 3) {
        puts("Error: Init: Invalid number of arguments!");
        printf("Usage:\n%s: [DEVICE] [SPEED]\n", argv[0]);
        puts("    with DEVICE:");
        for (int i = 0; i < I2C_NUMOF; i++) {
            printf("          %i -> I2C_%i\n", i, i);
        }
        puts("         SPEED:");
        puts("          0 -> SPEED_LOW (10kbit/s)");
        puts("          1 -> SPEED_NORMAL (100kbit/s)");
        puts("          2 -> SPEED_FAST (400kbit/s)");
        puts("          3 -> SPEED_FAST_PLUS (1Mbit/s)");
        puts("          4 -> SPEED_HIGH (3.4Mbit/s)\n");
        return 1;
    }

    dev = atoi(argv[1]);
    speed = atoi(argv[2]);

    res = i2c_init_master(dev, speed);
    if (res == -1) {
        puts("Error: Init: Given device not available");
        return 1;
    }
    else if (res == -2) {
        puts("Error: Init: Unsupported speed value");
        return 1;
    }
    else {
        printf("I2C_%i successfully initialized as master!\n", dev);
        i2c_dev = dev;
    }

    return 0;
}

int cmd_init_slave(int argc, char **argv)
{
    int dev, addr, res;

    if (argc != 3) {
        puts("Error: Invalid number of arguments!");
        printf("Usage:\n%s: [DEVICE] [ADDRESS]\n", argv[0]);
        puts("    with DEVICE:");
        for (int i = 0; i < I2C_NUMOF; i++) {
            printf("          %i -> I2C_%i\n", i, i);
        }
        puts("         ADDRESS: value between 0 and 127");
        return 1;
    }

    dev = atoi(argv[1]);
    addr = atoi(argv[1]);

    res = i2c_init_slave(dev, addr);
    if (res == -1) {
        puts("Error: Init: Given device not available");
        return 1;
    }
    else if (res == -2) {
        puts("Error: Init: Invalid address given");
        return 1;
    }
    else {
        printf("I2C_%i successfully initialized as slave with address %i!\n", dev, addr);
        i2c_dev = dev;
    }

    return 0;
}

int cmd_write(int argc, char **argv)
{
    int res;
    uint8_t addr;
    int length = argc - 2;
    char data[BUFSIZE];

    if (i2c_dev < 0) {
        puts("Error: no I2C device was initialized");
        return 1;
    }
    if (argc < 3) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s: ADDR BYTE0 [BYTE1 [BYTE_n [...]]]\n", argv[0]);
        return 1;
    }

    addr = (uint8_t)atoi(argv[1]);
    for (int i = 0; i < length; i++) {
        data[i] = (char)atoi(argv[i + 2]);
    }

    if (length == 1) {
        printf("i2c_write_byte(I2C_%i, 0x%02x, 0x%02x)\n", i2c_dev, addr, (unsigned int)data[0]);
        res = i2c_write_byte(i2c_dev, addr, data[0]);
    }
    else {
        printf("i2c_write_bytes(I2C_%i, 0x%02x, [", i2c_dev, addr);
        for (int i = 0; i < length; i++) {
            printf(", 0x%02x", (unsigned int)data[i]);
        }
        puts("])");
        res = i2c_write_bytes(i2c_dev, addr, data, length);
    }

    if (res < 0) {
        puts("Error: no bytes were written");
        return 1;
    }
    else {
        printf("I2C_%i: successfully wrote %i bytes to the bus\n", i2c_dev, res);
        return 0;
    }
}

int cmd_write_reg(int argc, char **argv)
{
    int res;
    uint8_t addr, reg;
    int length = argc - 3;
    char data[BUFSIZE];

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (length < 1) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s ADDR REG BYTE0 [BYTE1 ...]\n", argv[0]);
        return 1;
    }

    addr = (uint8_t)atoi(argv[1]);
    reg = (uint8_t)atoi(argv[2]);
    for (int i = 0; i < length; i++) {
        data[i] = (char)atoi(argv[i + 3]);
    }

    if (length == 1) {
        printf("i2c_write_reg(I2C_%i, 0x%02x, 0x%02x, 0x%02x)\n",
               i2c_dev, addr, reg, (unsigned int)data[0]);
        res = i2c_write_reg(i2c_dev, addr, reg, data[0]);
    }
    else {
        printf("i2c_write_regs(I2C_%i, 0x%02x, 0x%02x, [", i2c_dev, addr, reg);
        for (int i = 0; i < length; i++) {
            printf("0x%02x, ", (unsigned int)data[i]);
        }
        puts("])");
        res = i2c_write_regs(i2c_dev, addr, reg, data, length);
    }

    if (res < 1) {
        puts("Error: no bytes were written");
        return 1;
    }
    else {
        printf("I2C_%i: successfully wrote %i bytes to register 0x%02x\n", i2c_dev, res, reg);
        return 0;
    }
}

int cmd_read(int argc, char **argv)
{
    int res;
    uint8_t addr;
    int length;
    char data[BUFSIZE];

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (argc < 3) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s ADDR LENGTH]\n", argv[0]);
        return 1;
    }

    addr = (uint8_t)atoi(argv[1]);
    length = atoi(argv[2]);

    if (length < 1 || length > BUFSIZE) {
        puts("Error: invalid LENGTH parameter given\n");
        return 1;
    }
    else if (length == 1) {
        printf("i2c_read_byte(I2C_%i, 0x%02x, char *res)\n", i2c_dev, addr);
        res = i2c_read_byte(i2c_dev, addr, data);
    }
    else {
        printf("i2c_read_bytes(I2C_%i, 0x%02x, char *res, %i)\n", i2c_dev, addr, length);
        res = i2c_read_bytes(i2c_dev, addr, data, length);
    }

    if (res < 1) {
        puts("Error: no bytes were read");
        return 1;
    }
    else {
        printf("I2C_%i: successfully read %i bytes:\n  [", i2c_dev, res);
        for (int i = 0; i < res; i++) {
            printf("0x%02x, ", (unsigned int)data[i]);
        }
        puts("])");
        return 0;
    }
}

int cmd_read_reg(int argc, char **argv)
{
    int res;
    uint8_t addr, reg;
    int length;
    char data[BUFSIZE];

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (argc < 4) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s ADDR REG LENGTH]\n", argv[0]);
        return 1;
    }

    addr = (uint8_t)atoi(argv[1]);
    reg = (uint8_t)atoi(argv[2]);
    length = atoi(argv[3]);

    if (length < 1 || length > BUFSIZE) {
        puts("Error: invalid LENGTH parameter given");
        return 1;
    }
    else if (length == 1) {
        printf("i2c_read_reg(I2C_%i, 0x%02x, 0x%02x, char *res)\n", i2c_dev, addr, reg);
        res = i2c_read_reg(i2c_dev, addr, reg, data);
    }
    else {
        printf("i2c_read_regs(I2C_%i, 0x%02x, 0x%02x, char *res, %i)\n", i2c_dev, addr, reg, length);
        res = i2c_read_regs(i2c_dev, addr, reg, data, length);
    }

    if (res < 1) {
        puts("Error: no bytes were read");
        return 1;
    }
    else {
        printf("I2C_%i: successfully read %i bytes from reg 0x%02x:\n  [", i2c_dev, res, reg);
        for (int i = 0; i < res; i++) {
            printf("0x%02x, ", (unsigned int)data[i]);
        }
        puts("])");
        return 0;
    }

}

static const shell_command_t shell_commands[] = {
    { "init_master", "Initialize I2C as master", cmd_init_master },
    { "init_slave", "Initialize I2C as slave", cmd_init_slave },
    { "w", "write bytes to given address", cmd_write },
    { "wr", "write to register ", cmd_write_reg },
    { "r", "read bytes from given address", cmd_read },
    { "rr", "read bytes from register", cmd_read_reg },
    { NULL, NULL, NULL }
};

static int shell_readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void shell_putchar(int c)
{
    (void) putchar(c);
}

int main(void)
{
    shell_t shell;

    puts("Test for the low-level I2C driver");

    /* prepare I/O for shell */
    board_uart0_init();
    posix_open(uart0_handler_pid, 0);

    /* define own shell commands */
    shell_init(&shell, shell_commands, UART0_BUFSIZE, shell_readc, shell_putchar);
    shell_run(&shell);

    return 0;
}
