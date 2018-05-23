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

#ifndef GET_I2C_CLK_SPD
#define GET_I2C_CLK_SPD(x)  (-1)
#endif

#define BUFSIZE        (128U)

static int i2c_dev = -1;

int cmd_i2c_assign_dev(int argc, char **argv)
{
    int dev;

    if (argc != 2) {
        puts("Error: Invalid number of arguments!");
        printf("Usage:\n%s: [DEVICE]\n", argv[0]);
        puts("    with DEVICE:");
        for (unsigned i = 0; i < I2C_NUMOF; i++) {
            printf("          %u -> I2C_%u\n", i, i);
        }
        return 1;
    }
    dev = atoi(argv[1]);
    if (dev < 0 || dev >= (int)I2C_NUMOF) {
        puts("Error: Given device not available");
        return 1;
    }
    else {
        printf("I2C_%i successfully initialized as master!\n", dev);
        i2c_dev = dev;
        return 0;
    }
    return 1;
}

int cmd_i2c_acquire(int argc, char **argv)
{
    int res;
    (void)argv;
    if (argc != 1) {
        puts("Error: Aquire: Invalid number of arguments!");
        return 1;
    }
    if (i2c_dev < 0) {
        puts("Error: no I2C device was initialized");
        return 1;
    }
    res = i2c_acquire(i2c_dev);
    if (res == 0) {
        printf("I2C_%i successfully acquired!\n", i2c_dev);
        return 0;
    }
    else {
        printf("Error: Device busy\n");
        return 1;
    }
    return 1;
}

int cmd_i2c_release(int argc, char **argv)
{
    (void)argv;
    if (argc != 1) {
        puts("Error: Aquire: Invalid number of arguments!");
        return 1;
    }
    if (i2c_dev < 0) {
        puts("Error: no I2C device was initialized");
        return 1;
    }
    i2c_release(i2c_dev);
    printf("I2C_%i successfully released!\n", i2c_dev);
    return 0;
}

int cmd_i2c_read_reg(int argc, char **argv)
{
    int res;
    uint8_t addr, reg, flags;
    uint8_t data[1];

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (argc < 3) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s ADDR REG FLAG\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);
    reg = atoi(argv[2]);
    flags = atoi(argv[3]);

    printf("i2c_read_reg(I2C_%i, 0x%02x, 0x%02x)\n", i2c_dev, addr, reg);
    res = i2c_read_reg(i2c_dev, addr, reg, data, flags);

    if (res == I2C_ADDR_NACK) {
        puts("Error: addr NACK");
        return 1;
    }
    else if (res == I2C_ERR) {
        puts("Error: i2c return error");
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("I2C_%i: successfully read 1 byte from reg 0x%02x:\n  [",
            i2c_dev, reg);
        printf("0x%02x, ", data[0]);
        puts("]");
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_read_regs(int argc, char **argv)
{
    int res;
    uint8_t addr, reg, flags;
    int len;
    uint8_t data[BUFSIZE];

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (argc < 4) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s ADDR REG LENGTH FLAG\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);
    reg = atoi(argv[2]);
    len = atoi(argv[3]);
    flags = atoi(argv[4]);

    if (len < 1 || len > (int)BUFSIZE) {
        puts("Error: invalid LENGTH parameter given");
        return 1;
    }
    else {
        printf("i2c_read_regs(I2C_%i, 0x%02x, 0x%02x, %i)\n", i2c_dev, addr,
            reg, len);
        res = i2c_read_regs(i2c_dev, addr, reg, data, len, flags);
    }

    if (res == I2C_ADDR_NACK) {
        puts("Error: addr NACK");
        return 1;
    }
    else if (res == I2C_ERR) {
        puts("Error: i2c return error");
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("I2C_%i: successfully read %i bytes from reg 0x%02x:\n  [",
            i2c_dev, len, reg);
        for (int i = 0; i < len; i++) {
            printf("0x%02x, ", data[i]);
        }
        puts("]");
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_read_byte(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint8_t flags;
    uint8_t data[1];

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (argc < 3) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s ADDR FLAG\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);
    flags = atoi(argv[2]);

    printf("i2c_read_byte(I2C_%i, 0x%02x, 0x%02x)\n", i2c_dev, addr, flags);
    res = i2c_read_byte(i2c_dev, addr, data, flags);

    if (res == I2C_ADDR_NACK) {
        puts("Error: addr NACK");
        return 1;
    }
    else if (res == I2C_ERR) {
        puts("Error: i2c return error");
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("I2C_%i: successfully read 1 byte:\n  [", i2c_dev);
        printf("0x%02x, ", data[0]);
        puts("]");
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_read_bytes(int argc, char **argv)
{
    int res;
    uint16_t addr;
    int len;
    uint8_t flags;
    uint8_t data[BUFSIZE];

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (argc < 4) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s ADDR LENGTH FLAG\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);
    len = atoi(argv[2]);
    flags = atoi(argv[3]);

    if (len < 1 || len > (int)BUFSIZE) {
        puts("Error: invalid LENGTH parameter given\n");
        return 1;
    }
    else {
        printf("i2c_read_bytes(I2C_%i, 0x%02x, %i, 0x%02x)\n", i2c_dev, addr,
            len, flags);
        res = i2c_read_bytes(i2c_dev, addr, data, len, flags);
    }

    if (res == I2C_ADDR_NACK) {
        puts("Error: addr NACK");
        return 1;
    }
    else if (res == I2C_ERR) {
        puts("Error: i2c return error");
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("I2C_%i: successfully read %i bytes:\n  [", i2c_dev, len);
        for (int i = 0; i < len; i++) {
            printf("0x%02x, ", data[i]);
        }
        puts("]");
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_write_byte(int argc, char **argv)
{
    int res;
    uint8_t addr;
    uint8_t flags;
    uint8_t data;

    if (i2c_dev < 0) {
        puts("Error: no I2C device was initialized");
        return 1;
    }
    if (argc != 4) {
        puts("Error: invalid number of arguments given");
        printf("Usage:\n%s: ADDR FLAG BYTE0\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);
    flags = atoi(argv[2]);
    data = atoi(argv[3]);

    printf("i2c_write_byte(I2C_%i, 0x%02x, 0x%02x, [0x%02x", i2c_dev, addr,
        flags, data);
    puts("])");
    res = i2c_write_byte(i2c_dev, addr, data, flags);

    if (res == I2C_ADDR_NACK) {
        puts("Error: addr NACK");
        return 1;
    }
    else if (res == I2C_DATA_NACK) {
        puts("Error: data NACK");
        return 1;
    }
    else if (res == I2C_ERR) {
        puts("Error: i2c return error");
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("I2C_%i: successfully wrote 1 byte to the bus\n", i2c_dev);
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_write_bytes(int argc, char **argv)
{
    int res;
    uint8_t addr;
    int len = argc - 3;
    uint8_t flags;
    uint8_t data[BUFSIZE];

    if (i2c_dev < 0) {
        puts("Error: no I2C device was initialized");
        return 1;
    }
    if (argc < 4) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s: ADDR FLAG BYTE0 [BYTE1 [BYTE_n [...]]]\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);
    flags = atoi(argv[2]);
    for (int i = 0; i < len; i++) {
        data[i] = atoi(argv[i + 3]);
    }

    printf("i2c_write_bytes(I2C_%i, 0x%02x, 0x%02x, [", i2c_dev, addr, flags);
    for (int i = 0; i < len; i++) {
        printf(", 0x%02x", data[i]);
    }
    puts("])");
    res = i2c_write_bytes(i2c_dev, addr, data, len, flags);

    if (res == I2C_ADDR_NACK) {
        puts("Error: addr NACK");
        return 1;
    }
    else if (res == I2C_DATA_NACK) {
        puts("Error: data NACK");
        return 1;
    }
    else if (res == I2C_ERR) {
        puts("Error: i2c return error");
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("I2C_%i: successfully wrote %i bytes\n", i2c_dev, len);
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_write_reg(int argc, char **argv)
{
    int res;
    uint8_t addr, reg, flags;
    uint8_t data;

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (argc != 5) {
        puts("Error: invalid number of arguments given");
        printf("Usage:\n%s ADDR REG FLAG BYTE0\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);
    reg = atoi(argv[2]);
    flags = atoi(argv[3]);
    data = atoi(argv[4]);

    printf("i2c_write_reg(I2C_%i, 0x%02x, 0x%02x, 0x%02x, [0x%02x", i2c_dev,
        addr, reg, flags, data);
    puts("])");
    res = i2c_write_reg(i2c_dev, addr, reg, data, flags);

    if (res == I2C_ADDR_NACK) {
        puts("Error: addr NACK");
        return 1;
    }
    else if (res == I2C_DATA_NACK) {
        puts("Error: data NACK");
        return 1;
    }
    else if (res == I2C_ERR) {
        puts("Error: i2c return error");
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("I2C_%i: successfully wrote 1 byte\n", i2c_dev);
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_write_regs(int argc, char **argv)
{
    int res;
    uint8_t addr, reg, flags;
    int len = argc - 4;
    uint8_t data[BUFSIZE];

    if (i2c_dev < 0) {
        puts("Error: no I2C device initialized");
        return 1;
    }
    if (len < 1) {
        puts("Error: not enough arguments given");
        printf("Usage:\n%s ADDR REG FLAG BYTE0 [BYTE1 ...]\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);
    reg = atoi(argv[2]);
    flags = atoi(argv[3]);
    for (int i = 0; i < len; i++) {
        data[i] = atoi(argv[i + 4]);
    }

    printf("i2c_write_regs(I2C_%i, 0x%02x, 0x%02x, 0x%02x[", i2c_dev, addr,
        reg, flags);
    for (int i = 0; i < len; i++) {
        printf("0x%02x, ", data[i]);
    }
    puts("])");
    res = i2c_write_regs(i2c_dev, addr, reg, data, len, flags);

    if (res == I2C_ADDR_NACK) {
        puts("Error: addr NACK");
        return 1;
    }
    else if (res == I2C_DATA_NACK) {
        puts("Error: data NACK");
        return 1;
    }
    else if (res == I2C_ERR) {
        puts("Error: i2c return error");
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("I2C_%i: successfully wrote %i bytes to register 0x%02x\n",
            i2c_dev, len, reg);
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_get_devs(int argc, char **argv)
{
    (void)argv;
    if (argc != 1) {
        puts("Error: No args needed :)\n");
        return 1;
    }
    printf("Amount of i2c devices: %d\n", I2C_NUMOF);
    return 0;
}

int cmd_i2c_get_speed(int argc, char **argv)
{
    (void)argv;
    if (argc != 1) {
        puts("Error: No args needed :)");
        return 1;
    }
    if (i2c_dev < 0) {
        puts("Error: no I2C device was initialized");
        return 1;
    }

    switch (GET_I2C_CLK_SPD(i2c_dev)) {
        case I2C_SPEED_LOW:
        printf("I2C%d clk speed: 0 -> ~10kbit/s",i2c_dev);
        break;

        case I2C_SPEED_NORMAL:
        printf("I2C%d clk speed: 1 -> ~100kbit/s",i2c_dev);
        break;

        case I2C_SPEED_FAST:
        printf("I2C%d clk speed: 2 -> ~400kbit/s",i2c_dev);
        break;

        case I2C_SPEED_FAST_PLUS:
        printf("I2C%d clk speed: 3 -> ~1Mbit/s",i2c_dev);
        break;

        case I2C_SPEED_HIGH:
        printf("I2C%d clk speed: 4 -> ~10kbit/s",i2c_dev);
        break;

        default:
        printf("I2C%d clk speed: ? -> NOT DEFINED\n",i2c_dev);
        return 1;
        break;
    }
    printf("\n");

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "i2c_assign_dev", "Selects the device", cmd_i2c_assign_dev },
    { "i2c_acquire", "Get access to the I2C bus", cmd_i2c_acquire },
    { "i2c_release", "Release to the I2C bus", cmd_i2c_release },
    { "i2c_read_reg", "Read byte from register", cmd_i2c_read_reg },
    { "i2c_read_regs", "Read bytes from registers", cmd_i2c_read_regs },
    { "i2c_read_byte", "Read byte from the I2C device", cmd_i2c_read_byte },
    { "i2c_read_bytes", "Read bytes from the I2C device", cmd_i2c_read_bytes },
    { "i2c_write_byte", "Write byte to the I2C device", cmd_i2c_write_byte },
    { "i2c_write_bytes", "Write bytes to the I2C device", cmd_i2c_write_bytes },
    { "i2c_write_reg", "Write byte to register", cmd_i2c_write_reg },
    { "i2c_write_regs", "Write bytes to registers", cmd_i2c_write_regs },
    { "i2c_get_devs", "Gets amount of supported i2c devices", cmd_i2c_get_devs },
    { "i2c_get_speed", "Get the boards clk speed", cmd_i2c_get_speed },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Test for the low-level I2C driver");

    /* define own shell commands */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
