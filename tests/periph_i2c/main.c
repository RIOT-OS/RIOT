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
#include <errno.h>

#include "periph_conf.h"
#include "periph/i2c.h"
#include "shell.h"

#ifndef GET_I2C_CLK_SPD
#define GET_I2C_CLK_SPD(x)  (-1)
#endif

#ifndef I2C_ACK
#define I2C_ACK         (0)
#endif

#define INVALID_ARGS    puts("Error: Invalid number of arguments");

#define BUFSIZE         (128U)

#define CONVERT_ERROR   (-32768)

static int i2c_dev = -1;

static inline void print_i2c_read(i2c_t dev, uint16_t *reg, uint8_t *buf,
    int len)
{
    printf("Success: i2c_%i read %i byte(s) ", dev, len);
    if (reg != NULL) {
        printf("from reg 0x%02x ", *reg);
    }
    printf(": [");
    for (int i = 0; i < len; i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("0x%02x", buf[i]);
    }
    printf("]\n");
}

static inline int get_num(const char *str)
{
    errno = 0;
    char *temp;
    long val = strtol(str, &temp, 0);

    if (temp == str || *temp != '\0' ||
        ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE)) {
        val = CONVERT_ERROR;
    }
    return (int)val;
}

static int check_dev(void)
{
    if (i2c_dev < 0) {
        printf("Error: ENODEV [%d]\n", ENODEV);
        return 1;
    }
    return 0;
}

static int print_i2c_error(int res)
{
    if (res == -EOPNOTSUPP) {
        printf("Error: EOPNOTSUPP [%d]\n", -res);
        return 1;
    }
    else if (res == -EINVAL) {
        printf("Error: EINVAL [%d]\n", -res);
        return 1;
    }
    else if (res == -EAGAIN) {
        printf("Error: EAGAIN [%d]\n", -res);
        return 1;
    }
    else if (res == -ENXIO) {
        printf("Error: ENXIO [%d]\n", -res);
        return 1;
    }
    else if (res == -EIO) {
        printf("Error: EIO [%d]\n", -res);
        return 1;
    }
    else if (res == -ETIMEDOUT) {
        printf("Error: ETIMEDOUT [%d]\n", -res);
        return 1;
    }
    else if (res == I2C_ACK) {
        printf("Success: I2C_ACK [%d]\n", res);
        return 0;
    }
    puts("Error: Unknown Error");
    return 1;
}

int cmd_i2c_assign_dev(int argc, char **argv)
{
    int dev;

    if (argc != 2) {
        INVALID_ARGS;
        printf("Usage:\n%s: [DEVICE]\n", argv[0]);
        puts("    with DEVICE:");
        for (unsigned i = 0; i < I2C_NUMOF; i++) {
            printf("          %u -> i2c_%u\n", i, i);
        }
        return 1;
    }
    dev = atoi(argv[1]);
    if (dev < 0 || dev >= (int)I2C_NUMOF) {
        puts("Error: Given device not available");
        i2c_dev = -1;
        return 1;
    }
    printf("Success: i2c_%i assigned as master\n", dev);
    i2c_dev = dev;
    return 0;
}

int cmd_i2c_acquire(int argc, char **argv)
{
    int res;

    (void)argv;
    (void)argc;
    if (check_dev() == 1) {
        return 1;
    }

    printf("Command: i2c_acquire(i2c_%i)\n", i2c_dev);
    res = i2c_acquire(i2c_dev);
    if (res == I2C_ACK) {
        printf("Success: i2c_%i acquired\n", i2c_dev);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_release(int argc, char **argv)
{
    int res;

    (void)argv;
    (void)argc;
    if (check_dev() == 1) {
        return 1;
    }

    printf("Command: i2c_release(i2c_%i)\n", i2c_dev);
    res = i2c_release(i2c_dev);

    if (res == I2C_ACK) {
        printf("Success: i2c_%i released\n", i2c_dev);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_read_reg(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint16_t reg;
    uint8_t flags = 0;
    uint8_t data;

    if (check_dev() == 1) {
        return 1;
    }

    if (argc < 3 || argc > 4) {
        INVALID_ARGS;
        printf("Usage:\n%s ADDR REG [FLAG]\n", argv[0]);
        return 1;
    }

    addr = get_num(argv[1]);
    reg = get_num(argv[2]);
    if (argc == 4) {
        flags = get_num(argv[3]);
    }

    printf("Command: i2c_read_reg(i2c_%i, 0x%02x, 0x%02x)\n", i2c_dev, addr, reg);
    res = i2c_read_reg(i2c_dev, addr, reg, &data, flags);

    if (res == I2C_ACK) {
        print_i2c_read(i2c_dev, &reg, &data, 1);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_read_regs(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint16_t reg;
    uint8_t flags = 0;
    int len;
    uint8_t data[BUFSIZE];

    if (check_dev() == 1) {
        return 1;
    }

    if (argc < 4 || argc > 5) {
        INVALID_ARGS;
        printf("Usage:\n%s ADDR REG LENGTH [FLAG]\n", argv[0]);
        return 1;
    }

    addr = get_num(argv[1]);
    reg = get_num(argv[2]);
    len = get_num(argv[3]);
    if (argc == 5) {
        flags = get_num(argv[4]);
    }

    if (len < 1 || len > (int)BUFSIZE) {
        puts("Error: invalid LENGTH parameter given");
        return 1;
    }
    else {
        printf("Command: i2c_read_regs(i2c_%i, 0x%02x, 0x%02x, %i)\n", i2c_dev,
            addr, reg, len);
        res = i2c_read_regs(i2c_dev, addr, reg, data, len, flags);
    }

    if (res == I2C_ACK) {
        print_i2c_read(i2c_dev, &reg, data, len);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_read_byte(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint8_t flags = 0;
    uint8_t data;

    if (check_dev() == 1) {
        return 1;
    }

    if (argc < 2 || argc > 3) {
        INVALID_ARGS;
        printf("Usage:\n%s ADDR [FLAG]\n", argv[0]);
        return 1;
    }

    addr = get_num(argv[1]);
    if (argc == 3) {
        flags = get_num(argv[2]);
    }

    printf("Command: i2c_read_byte(i2c_%i, 0x%02x, 0x%02x)\n", i2c_dev, addr, flags);
    res = i2c_read_byte(i2c_dev, addr, &data, flags);

    if (res == I2C_ACK) {
        print_i2c_read(i2c_dev, NULL, &data, 1);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_read_bytes(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint8_t flags = 0;
    int len;
    uint8_t data[BUFSIZE];

    if (check_dev() == 1) {
        return 1;
    }

    if (argc < 3 || argc > 4) {
        INVALID_ARGS;
        printf("Usage:\n%s ADDR LENGTH [FLAG]\n", argv[0]);
        return 1;
    }

    addr = get_num(argv[1]);
    len = get_num(argv[2]);
    if (argc == 4) {
        flags = get_num(argv[3]);
    }

    if (len < 1 || len > (int)BUFSIZE) {
        puts("Error: invalid LENGTH parameter given");
        return 1;
    }
    else {
        printf("Command: i2c_read_bytes(i2c_%i, 0x%02x, %i, 0x%02x)\n", i2c_dev,
         addr, len, flags);
        res = i2c_read_bytes(i2c_dev, addr, data, len, flags);
    }

    if (res == I2C_ACK) {
        print_i2c_read(i2c_dev, NULL, data, len);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_write_byte(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint8_t flags = 0;
    uint8_t data;

    if (check_dev() == 1) {
        return 1;
    }

    if (argc < 3 || argc > 4) {
        INVALID_ARGS;
        printf("Usage:\n%s: ADDR BYTE [FLAG]\n", argv[0]);
        return 1;
    }

    addr = get_num(argv[1]);
    data = get_num(argv[2]);
    if (argc == 4) {
        flags = get_num(argv[3]);
    }

    printf("Command: i2c_write_byte(i2c_%i, 0x%02x, 0x%02x, [0x%02x", i2c_dev, addr,
        flags, data);
    puts("])");
    res = i2c_write_byte(i2c_dev, addr, data, flags);

    if (res == I2C_ACK) {
        printf("Success: i2c_%i wrote 1 byte to the bus\n", i2c_dev);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_write_bytes(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint8_t flags = 0;
    int len = argc - 3;
    uint8_t data[BUFSIZE];

    if (check_dev() == 1) {
        return 1;
    }

    if (argc < 4) {
        INVALID_ARGS;
        printf("Usage:\n%s: ADDR FLAG BYTE0 [BYTE1 [BYTE_n [...]]]\n", argv[0]);
        return 1;
    }

    addr = get_num(argv[1]);
    flags = get_num(argv[2]);
    for (int i = 0; i < len; i++) {
        data[i] = get_num(argv[i + 3]);
    }

    printf("Command: i2c_write_bytes(i2c_%i, 0x%02x, 0x%02x, [", i2c_dev, addr,
        flags);
    for (int i = 0; i < len; i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("0x%02x", data[i]);
    }
    puts("])");
    res = i2c_write_bytes(i2c_dev, addr, data, len, flags);

    if (res == I2C_ACK) {
        printf("Success: i2c_%i wrote %i bytes\n", i2c_dev, len);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_write_reg(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint16_t reg;
    uint8_t flags = 0;
    uint8_t data;

    if (check_dev() == 1) {
        return 1;
    }

    if (argc < 4 || argc > 5) {
        INVALID_ARGS;
        printf("Usage:\n%s ADDR REG BYTE [FLAG]\n", argv[0]);
        return 1;
    }

    addr = get_num(argv[1]);
    reg = get_num(argv[2]);
    data = get_num(argv[3]);
    if (argc == 5) {
        flags = get_num(argv[4]);
    }

    printf("Command: i2c_write_reg(i2c_%i, 0x%02x, 0x%02x, 0x%02x, [0x%02x", i2c_dev,
        addr, reg, flags, data);
    puts("])");
    res = i2c_write_reg(i2c_dev, addr, reg, data, flags);

    if (res == I2C_ACK) {
        printf("Success: i2c_%i wrote 1 byte\n", i2c_dev);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_write_regs(int argc, char **argv)
{
    int res;
    uint16_t addr;
    uint16_t reg;
    uint8_t flags = 0;
    int len = argc - 4;
    uint8_t data[BUFSIZE];

    if (check_dev() == 1) {
        return 1;
    }

    if (argc < 5) {
        INVALID_ARGS;
        printf("Usage:\n%s ADDR REG FLAG BYTE0 [BYTE1 ...]\n", argv[0]);
        return 1;
    }

    addr = get_num(argv[1]);
    reg = get_num(argv[2]);
    flags = get_num(argv[3]);
    for (int i = 0; i < len; i++) {
        data[i] = get_num(argv[i + 4]);
    }

    printf("Command: i2c_write_regs(i2c_%i, 0x%02x, 0x%02x, 0x%02x, [", i2c_dev,
        addr, reg, flags);
    for (int i = 0; i < len; i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("0x%02x", data[i]);
    }
    puts("])");
    res = i2c_write_regs(i2c_dev, addr, reg, data, len, flags);

    if (res == I2C_ACK) {
        printf("Success: i2c_%i wrote %i bytes to reg 0x%02x\n",
            i2c_dev, len, reg);
        return 0;
    }
    return print_i2c_error(res);
}

int cmd_i2c_get_devs(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    printf("Success: Amount of i2c devices: [%d]\n", I2C_NUMOF);
    return 0;
}

int cmd_i2c_get_speed(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    if (check_dev() == 1) {
        return 1;
    }

    switch (GET_I2C_CLK_SPD(i2c_dev)) {
        case I2C_SPEED_LOW:
            printf("Success: i2c_%d clk speed: 0 -> ~10kbit/s: [10000]",i2c_dev);
            break;

        case I2C_SPEED_NORMAL:
            printf("Success: i2c_%d clk speed: 1 -> ~100kbit/s: [100000]",i2c_dev);
            break;

        case I2C_SPEED_FAST:
            printf("Success: i2c_%d clk speed: 2 -> ~400kbit/s: [400000]",i2c_dev);
            break;

        case I2C_SPEED_FAST_PLUS:
            printf("Success: i2c_%d clk speed: 3 -> ~1Mbit/s: [1000000]",i2c_dev);
            break;

        case I2C_SPEED_HIGH:
            printf("Success: i2c_%d clk speed: 4 -> ~3.2Mbit/s: [3200000]",i2c_dev);
            break;

        default:
            printf("Error: i2c_%d clk speed: ? -> NOT DEFINED\n",i2c_dev);
            return 1;
    }
    printf("\n");

    return 0;
}

int cmd_i2c_get_id(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    puts("Success: [periph_i2c]");
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
    { "i2c_get_id", "Get the id of the fw", cmd_i2c_get_id },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Start: Test for the low-level I2C driver");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
