/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "feetech.h"
#include "shell.h"
#include "shell_commands.h"
#include "stdio_uart.h"

#include <stdio.h>
#include <string.h>

#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))

typedef struct {
    const char *name;
    int addr;
} reg_name_addr_t;

static const reg_name_addr_t regs8[] = {
    { "ID", SCS15_ID },
    { "BAUD_RATE", SCS15_BAUD_RATE },
    { "RETURN_DELAY_TIME", SCS15_RETURN_DELAY_TIME },
    { "RETURN_LEVEL", SCS15_RETURN_LEVEL },
    { "LIMIT_TEMPERATURE", SCS15_LIMIT_TEMPERATURE },
    { "MAX_LIMIT_VOLTAGE", SCS15_MAX_LIMIT_VOLTAGE },
    { "MIN_LIMIT_VOLTAGE", SCS15_MIN_LIMIT_VOLTAGE },
    { "ALARM_LED", SCS15_ALARM_LED },
    { "ALARM_SHUTDOWN", SCS15_ALARM_SHUTDOWN },
    { "COMPLIANCE_P", SCS15_COMPLIANCE_P },
    { "COMPLIANCE_D", SCS15_COMPLIANCE_D },
    { "COMPLIANCE_I", SCS15_COMPLIANCE_I },
    { "CW_DEAD", SCS15_CW_DEAD },
    { "CCW_DEAD", SCS15_CCW_DEAD },
    { "TORQUE_ENABLE", SCS15_TORQUE_ENABLE },
    { "LED", SCS15_LED },
    { "LOCK", SCS15_LOCK },
    { "PRESENT_VOLTAGE", SCS15_PRESENT_VOLTAGE },
    { "PRESENT_TEMPERATURE", SCS15_PRESENT_TEMPERATURE },
    { "REGISTERED_INSTRUCTION", SCS15_REGISTERED_INSTRUCTION },
    { "ERROR", SCS15_ERROR },
    { "MOVING", SCS15_MOVING },
};

static const reg_name_addr_t regs16[] = {
    { "MODEL_NUMBER", SCS15_MODEL_NUMBER },
    { "VERSION", SCS15_VERSION },
    { "MIN_ANGLE_LIMIT", SCS15_MIN_ANGLE_LIMIT },
    { "MAX_ANGLE_LIMIT", SCS15_MAX_ANGLE_LIMIT },
    { "MAX_TORQUE", SCS15_MAX_TORQUE },
    { "PUNCH", SCS15_PUNCH },
    { "IMAX", SCS15_IMAX },
    { "OFFSET", SCS15_OFFSET },
    { "GOAL_POSITION", SCS15_GOAL_POSITION },
    { "GOAL_TIME", SCS15_GOAL_TIME },
    { "GOAL_SPEED", SCS15_GOAL_SPEED },
    { "PRESENT_POSITION", SCS15_PRESENT_POSITION },
    { "PRESENT_SPEED", SCS15_PRESENT_SPEED },
    { "PRESENT_LOAD", SCS15_PRESENT_LOAD },
    { "VIR_POSITION", SCS15_VIR_POSITION },
    { "CURRENT", SCS15_CURRENT },
};

static const int32_t baudrates[] = {
    1000000L,
    500000L,
    250000L,
    128000L,
    115200L,
    76800L,
    57600L,
    38400L,
};

static uint8_t feetech_buffer[128];
static uart_half_duplex_t stream;

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

    for (size_t i = 0 ; i < ARRAY_LEN(baudrates) ; i++) {
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

    for (size_t i = 0 ; i < ARRAY_LEN(regs8) ; i++) {
        if (strcmp(arg, regs8[i].name) == 0) {
            *reg8 = regs8[i].addr;
            return;
        }
    }

    for (size_t i = 0 ; i < ARRAY_LEN(regs16) ; i++) {
        if (strcmp(arg, regs16[i].name) == 0) {
            *reg16 = regs16[i].addr;
            return;
        }
    }

    printf("Error: Invalid register (%s)\n", arg);
}

void print_registers(void) {
    puts("available 8bits registers :");
    for (size_t i = 0 ; i < ARRAY_LEN(regs8) ; i++) {
        printf("\t%s\n", regs8[i].name);
    }

    puts("available 16bits registers :");
    for (size_t i = 0 ; i < ARRAY_LEN(regs16) ; i++) {
        printf("\t%s\n", regs16[i].name);
    }
}

static int cmd_init(int argc, char **argv) {
    int uart = -1;
    int baud = -1;
    uint32_t timeout = -1;

    if (argc != 3 && argc != 4) {
        printf("usage; %s <uart> <baudrate> [<timeout_us>]\n", argv[0]);
        puts("available baudrates :");
        for (size_t i = 0 ; i < ARRAY_LEN(baudrates) ; i++) {
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
        .dir = UART_HALF_DUPLEX_DIR_NONE,
    };

    int ret = uart_half_duplex_init(&stream, feetech_buffer, ARRAY_LEN(feetech_buffer), &params);

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

    printf("Successfully initialized Feetech TTL bus UART_DEV(%i)\n", uart);
    return 0;
}

static int cmd_ping(int argc, char **argv) {
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
    if (feetech_ping(&stream, id) == FEETECH_OK) {
        printf("Device %i responded\n", id);
    }
    else {
        printf("No response from %i\n", id);
    }
    return 0;
}

static int cmd_scan(int argc, char **argv) {
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
        if (feetech_ping(&stream, id) == FEETECH_OK) {
            printf("Device %i available\n", id);
        }
    }
    puts("End");
    return 0;
}

static int cmd_read(int argc, char **argv) {
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
    feetech_t dev;
    feetech_init(&dev, &stream, id);
    if (reg8 >= 0) {
        uint8_t val = 0;
        int ret = feetech_read8(&dev, reg8, &val);
        if (ret != FEETECH_OK) {
            printf("Error[%i] : No response from %i\n", ret, id);
            return -1;
        }
        printf("%i\n", (int)val);
    }
    else {
        uint16_t val = 0;
        int ret = feetech_read16(&dev, reg16, &val);
        if (ret != FEETECH_OK) {
            printf("Error[%i] : No response from %i\n", ret, id);
            return -1;
        }
        printf("%i\n", (int)val);
    }
    return 0;
}

static int cmd_write(int argc, char **argv) {
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
    feetech_t dev;
    feetech_init(&dev, &stream, id);
    if (reg8 >= 0) {
        int ret = feetech_write8(&dev, reg8, val);
        if (ret != FEETECH_OK) {
            printf("Error[%i] : No response from %i\n", ret, id);
            return -1;
        }
        printf("Written %i at address %i\n", (int)val, reg8);
    }
    else {
        int ret = feetech_write16(&dev, reg16, val);
        if (ret != FEETECH_OK) {
            printf("Error[%i] : No response from %i\n", ret, id);
            return -1;
        }
        printf("Written %i at address %i\n", (int)val, reg16);
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Initialize a Feetech TTL bus with a given baudrate", cmd_init },
    { "ping", "Ping a Feetech device", cmd_ping },
    { "scan", "Find all Feetech devices between min_id and max_id", cmd_scan },
    { "read", "Read a Feetech device register", cmd_read },
    { "write", "Write in a Feetech device register", cmd_write },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nManual Feetech device driver test");
    puts("===================================");
    puts("This application is intended for testing Feetech TTL bus\n");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
