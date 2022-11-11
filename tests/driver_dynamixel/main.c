/*
 * Copyright (C) 2017 Inira
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "dynamixel.h"
#include "shell.h"
#include "stdio_uart.h"
#include "board.h"
#include "periph/gpio.h"
#include "container.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    const char *name;
    int addr;
} reg_name_addr_t;

static const reg_name_addr_t regs8[] = {
   { "VERSION", XL320_VERSION },
   { "ID", XL320_ID },
   { "BAUD_RATE", XL320_BAUD_RATE },
   { "RETURN_DELAY_TIME", XL320_RETURN_DELAY_TIME },
   { "CONTROL_MODE", XL320_CONTROL_MODE },
   { "LIMIT_TEMPERATURE", XL320_LIMIT_TEMPERATURE },
   { "LOWER_LIMIT_VOLTAGE", XL320_LOWER_LIMIT_VOLTAGE },
   { "UPPER_LIMIT_VOLTAGE", XL320_UPPER_LIMIT_VOLTAGE },
   { "RETURN_LEVEL", XL320_RETURN_LEVEL },
   { "ALARM_SHUTDOWN", XL320_ALARM_SHUTDOWN },
   { "TORQUE_ENABLE", XL320_TORQUE_ENABLE },
   { "LED", XL320_LED },
   { "D_GAIN", XL320_D_GAIN },
   { "I_GAIN", XL320_I_GAIN },
   { "P_GAIN", XL320_P_GAIN },
   { "PRESENT_VOLTAGE", XL320_PRESENT_VOLTAGE },
   { "PRESENT_TEMPERATURE", XL320_PRESENT_TEMPERATURE },
   { "REGISTERED_INST", XL320_REGISTERED_INST },
   { "MOVING", XL320_MOVING },
   { "ERROR", XL320_ERROR },
};

static const reg_name_addr_t regs16[] = {
   { "MODEL_NUMBER", XL320_MODEL_NUMBER },
   { "CW_ANGLE_LIMIT", XL320_CW_ANGLE_LIMIT },
   { "CCW_ANGLE_LIMIT", XL320_CCW_ANGLE_LIMIT },
   { "MAX_TORQUE", XL320_MAX_TORQUE },
   { "GOAL_POSITION", XL320_GOAL_POSITION },
   { "GOAL_VELOCITY", XL320_GOAL_VELOCITY },
   { "GOAL_TORQUE", XL320_GOAL_TORQUE },
   { "PRESENT_POSITION", XL320_PRESENT_POSITION },
   { "PRESENT_SPEED", XL320_PRESENT_SPEED },
   { "PRESENT_LOAD", XL320_PRESENT_LOAD },
   { "PUNCH", XL320_PUNCH },
};

static const int32_t baudrates[] = {
    1000000L,
    115200L,
    57600L,
    9600L,
};

static uint8_t dynamixel_buffer[128];
static uart_half_duplex_t stream;

#ifdef DXL_DIR_PIN
static void dir_init(uart_t uart)
{
    (void)uart;
    gpio_init(DXL_DIR_PIN, GPIO_OUT);
}

static void dir_enable_tx(uart_t uart)
{
    (void)uart;
    gpio_set(DXL_DIR_PIN);
}

static void dir_disable_tx(uart_t uart)
{
    (void)uart;
    gpio_clear(DXL_DIR_PIN);
}
#else
#define dir_init       NULL
#define dir_enable_tx  NULL
#define dir_disable_tx NULL
#endif

static int parse_uart(char *arg)
{
    unsigned uart = atoi(arg);
    if (uart >= UART_NUMOF) {
        printf("Error: Invalid UART_DEV device specified (%u).\n", uart);
        return -1;
    }
    else if (UART_DEV(uart) == STDIO_UART_DEV) {
        printf("Error: The selected UART_DEV(%u) is used for the shell!\n", uart);
        return -2;
    }
    return uart;
}

static int32_t parse_baud(char *arg)
{
    int32_t baud = atoi(arg);

    for (size_t i = 0 ; i < ARRAY_SIZE(baudrates); i++) {
        if (baud == baudrates[i]) {
            return baud;
        }
    }

    printf("Error: Invalid baudrate (%s)\n", arg);
    return -1;
}

static int parse_dev(char *arg)
{
    int dev = atoi(arg);
    if (dev < 0 || 254 < dev) {
        printf("Error: Invalid device id (%s)\n", arg);
        return -1;
    }
    return dev;
}

static void parse_reg(char *arg, int *reg8, int *reg16)
{
    *reg8 = -1;
    *reg16 = -1;

    for (size_t i = 0 ; i < ARRAY_SIZE(regs8); i++) {
        if (strcmp(arg, regs8[i].name) == 0) {
            *reg8 = regs8[i].addr;
            return;
        }
    }

    for (size_t i = 0 ; i < ARRAY_SIZE(regs16); i++) {
        if (strcmp(arg, regs16[i].name) == 0) {
            *reg16 = regs16[i].addr;
            return;
        }
    }

    printf("Error: Invalid register (%s)\n", arg);
}

void print_registers(void)
{
    puts("available 8bits registers :");
    for (size_t i = 0 ; i < ARRAY_SIZE(regs8); i++) {
        printf("\t%s\n", regs8[i].name);
    }

    puts("available 16bits registers :");
    for (size_t i = 0 ; i < ARRAY_SIZE(regs16); i++) {
        printf("\t%s\n", regs16[i].name);
    }
}

static int cmd_init(int argc, char **argv)
{
    int uart = -1;
    int baud = -1;
    uint32_t timeout = -1;

    if (argc != 3 && argc != 4) {
        printf("usage; %s <uart> <baudrate> [<timeout_us>]\n", argv[0]);
        puts("available baudrates :");
        for (size_t i = 0 ; i < ARRAY_SIZE(baudrates); i++) {
            printf("\t%ld\n", (long int)baudrates[i]);
        }
        return 1;
    }
    /* parse parameters */
    uart = parse_uart(argv[1]);
    if (uart < 0) {
        return -1;
    }

    baud = parse_baud(argv[2]);
    if (baud < 0) {
        return -1;
    }

    if (argc == 4) {
        timeout = (uint32_t)atol(argv[3]);
        if (timeout == 0) {
            printf("Error : Invalid timeout (%s)", argv[3]);
            return -1;
        }
    }

    /* init */
    uart_half_duplex_params_t params = {
        .uart = uart,
        .baudrate = baud,
        .dir = { dir_init, dir_enable_tx, dir_disable_tx },
    };

    int ret = uart_half_duplex_init(&stream, dynamixel_buffer,
                                    ARRAY_SIZE(dynamixel_buffer), &params);

    if (argc == 4) {
        stream.timeout_us = timeout;
    }

    if (ret == UART_HALF_DUPLEX_NODEV) {
        puts("Error: invalid UART device given");
        return -1;
    }
    if (ret == UART_HALF_DUPLEX_NOBAUD) {
        puts("Error: given baudrate is not applicable");
        return -1;
    }
    if (ret == UART_HALF_DUPLEX_INTERR) {
        puts("Error: internal error");
        return -1;
    }
    if (ret == UART_HALF_DUPLEX_NOMODE) {
        puts("Error: given mode is not applicable");
        return -1;
    }
    if (ret == UART_HALF_DUPLEX_NOBUFF) {
        puts("Error: invalid buffer given");
        return -1;
    }

    printf("Successfully initialized Dynamixel TTL bus UART_DEV(%i)\n", uart);
    return 0;
}

static int cmd_ping(int argc, char **argv)
{
    int id = -1;

    if (argc != 2) {
        printf("usage; %s <dev_id>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    id = parse_dev(argv[1]);
    if (id < 0) {
        return -1;
    }

    /* ping */
    if (dynamixel_ping(&stream, id) == DYNAMIXEL_OK) {
        printf("Device %i responded\n", id);
    }
    else {
        printf("No response from %i\n", id);
    }
    return 0;
}

static int cmd_scan(int argc, char **argv)
{
    int min = -1;
    int max = -1;

    if (argc == 3) {
        min = atoi(argv[1]);
        max = atoi(argv[2]);
        if (min < 0) {
            return -1;
        }
        if (max > 254) {
            return -1;
        }
        if (max < min) {
            return -1;
        }
    }
    else if (argc == 1) {
        min = 0;
        max = 254;
    }
    else {
        printf("usage; %s [<min_id> <max_id>]\n", argv[0]);
        return 1;
    }

    /* ping */
    puts("Scanning...");
    for (int id = min ; id < max ; id++) {
        if (dynamixel_ping(&stream, id) == DYNAMIXEL_OK) {
            printf("Device %i available\n", id);
        }
    }
    puts("End");
    return 0;
}

static int cmd_read(int argc, char **argv)
{
    int id = -1;
    int reg8 = -1;
    int reg16 = -1;

    if (argc != 3) {
        printf("usage; %s <dev_id> <reg>\n", argv[0]);
        print_registers();
        return 1;
    }
    /* parse parameters */
    id = parse_dev(argv[1]);
    if (id < 0) {
        return -1;
    }

    parse_reg(argv[2], &reg8, &reg16);
    if (reg8 < 0 && reg16 < 0) {
        return -1;
    }

    /* read */
    dynamixel_t dev;
    dynamixel_init(&dev, &stream, id);
    if (reg8 >= 0) {
        uint8_t val = 0;
        int ret = dynamixel_read8(&dev, reg8, &val);
        if (ret != DYNAMIXEL_OK) {
            printf("Error[%i] : No response from %i\n", ret, id);
            return -1;
        }
        printf("%i\n", (int)val);
    }
    else {
        uint16_t val = 0;
        int ret = dynamixel_read16(&dev, reg16, &val);
        if (ret != DYNAMIXEL_OK) {
            printf("Error[%i] : No response from %i\n", ret, id);
            return -1;
        }
        printf("%i\n", (int)val);
    }
    return 0;
}

static int cmd_write(int argc, char **argv)
{
    int id = -1;
    int reg8 = -1;
    int reg16 = -1;

    if (argc != 4) {
        printf("usage; %s <dev_id> <reg> <value>\n", argv[0]);
        print_registers();
        return 1;
    }
    /* parse parameters */
    id = parse_dev(argv[1]);
    if (id < 0) {
        return -1;
    }

    parse_reg(argv[2], &reg8, &reg16);
    if (reg8 < 0 && reg16 < 0) {
        return -1;
    }

    int val = atoi(argv[3]);
    if (val < 0) {
        return -1;
    }

    /* read */
    dynamixel_t dev;
    dynamixel_init(&dev, &stream, id);
    if (reg8 >= 0) {
        int ret = dynamixel_write8(&dev, reg8, val);
        if (ret != DYNAMIXEL_OK) {
            printf("Error[%i] : No response from %i\n", ret, id);
            return -1;
        }
        printf("Written %i at address %i\n", (int)val, reg8);
    }
    else {
        int ret = dynamixel_write16(&dev, reg16, val);
        if (ret != DYNAMIXEL_OK) {
            printf("Error[%i] : No response from %i\n", ret, id);
            return -1;
        }
        printf("Written %i at address %i\n", (int)val, reg16);
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Initialize a Dynamixel TTL bus with a given baudrate", cmd_init },
    { "ping", "Ping a Dynamixel device", cmd_ping },
    { "scan", "Find all Dynamixel devices between min_id and max_id", cmd_scan },
    { "read", "Read a Dynamixel device register", cmd_read },
    { "write", "Write in a Dynamixel device register", cmd_write },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nManual Dynamixel device driver test");
    puts("===================================");
    puts("This application is intended for testing Dynamixel TTL bus\n");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
