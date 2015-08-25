/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Application for testing low-level SPI driver implementations
 *
 * This implementation covers both, master and slave configurations.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "shell.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#define SHELL_BUFSIZE       (128U)

#define SPI_BUFSIZE         (512U)

/**
 * @brief   Shared SPI data buffer for SPI master devices
 */
static uint8_t buf[SPI_BUFSIZE];

/**
 * @brief   Save the number of received bytes in slave mode
 */
static size_t slave_cnt[SPI_NUMOF];

/**
 * @brief   Receive buffer in slave mode
 */
static uint8_t slave_buf[SPI_NUMOF][SPI_BUFSIZE];

void dump_hex(uint8_t *data, size_t len)
{
    for (int i = 0; i < len; i++) {
        printf("0x%02x ", data[i]);
    }
    puts("");
    for (int i = 0; i < len; i++) {
        if (data[i] > ' ' && data[i] <= '~') {
            printf("  %c  ", (char)data[i]);
        }
        else {
            printf(" --  ");
        }
    }
    puts("");
}

/**
 * @brief   Function called on connection state change in slave mode
 */
static uint8_t slave_cs_cb(void *arg, bool start)
{
    int dev = (int)arg;
    if (start) {
        slave_cnt[dev] = 1;
    }
    else {
        puts("RX:");
        dump_hex(slave_buf[dev], slave_cnt[dev]);
    }
    return 1;
}

/**
 * @brief   Function called on byte transferred in slave mode
 */
static uint8_t slave_data_cb(void *arg, uint8_t data)
{
    int dev = (int)arg;
    slave_buf[dev][slave_cnt[dev] - 1] = data;
    return (uint8_t)(slave_cnt[dev]++);
}

static void print_modes(void)
{
    puts("\tconf:");
    printf("\t%i - POL:0, PHASE:0 - data on first rising edge\n",
           SPI_CONF_FIRST_RISING);
    printf("\t%i - POL:0, PHASE:1 - data on second rising edge\n",
           SPI_CONF_SECOND_RISING);
    printf("\t%i - POL:1, PHASE:0 - data on fist falling edge\n",
           SPI_CONF_FIRST_FALLING);
    printf("\t%i - POL:1, PHASE:1 - data on second falling edge\n",
           SPI_CONF_SECOND_FALLING);
}

static void print_speeds(void)
{
    puts("\tspeed:");
    printf("\t%i - ~100KHz\n", SPI_SPEED_100KHZ);
    printf("\t%i - ~400KHz\n", SPI_SPEED_400KHZ);
    printf("\t%i - ~1MHz\n", SPI_SPEED_1MHZ);
    printf("\t%i - ~5MHz\n", SPI_SPEED_5MHZ);
    printf("\t%i - ~10MHz\n", SPI_SPEED_10MHZ);
}

static void print_pol(void)
{
    puts("\tCS polarity:");
    printf("\t%i - low active\n", SPI_CS_POL_LOW_ACTIVE);
    printf("\t%i - high active\n", SPI_CS_POL_HIGH_ACTIVE);
}

static int parse_dev(const char *str)
{
    int res = atoi(str);
    if (res >= SPI_NUMOF) {
        puts("error: unable to parse device");
        return -1;
    }
    return res;
}

static spi_cs_t parse_cs(const char *port_str, const char *pin_str)
{
    int pin = atoi(pin_str);
    int port = atoi(port_str);

    if (pin < 0 || port < 0) {
        return SPI_CS_UNDEF;
    }

    if (port_str[0] == 'h') {
        return SPI_CS_DEV(pin);
    }
    else {
        return (spi_cs_t)GPIO(port, pin);
    }
}

static int parse_speed(const char *str)
{
    int speed = atoi(str);
    if (speed == SPI_SPEED_100KHZ || speed == SPI_SPEED_400KHZ ||
        speed == SPI_SPEED_1MHZ || speed == SPI_SPEED_5MHZ ||
        speed == SPI_SPEED_10MHZ) {
        return speed;
    }
    puts("error: unable to parse speed value");
    return -1;
}

static int parse_conf(const char *str)
{
    int conf = atoi(str);
    if (conf == SPI_CONF_FIRST_RISING || conf == SPI_CONF_SECOND_RISING ||
        conf == SPI_CONF_FIRST_FALLING || conf == SPI_CONF_SECOND_FALLING) {
        return conf;
    }
    puts("error: unable to parse conf value");
    return -1;
}

static int parse_pol(const char *str)
{
    int pol = atoi(str);
    if (pol == SPI_CS_POL_LOW_ACTIVE || pol == SPI_CS_POL_HIGH_ACTIVE) {
        return pol;
    }
    puts("error: unable to parse CS polarity");
    return -1;
}

static int send(int argc, char **argv, bool ascii)
{
    int dev;
    spi_cs_t cs;
    size_t len = 0;
    bool cont = false;

    if (argc < 5) {
        printf("usage: %s <dev> <port> <pin> <data> [c]\n", argv[0]);
        puts("\tdev: SPI device to use");
        puts("\tport: Port of CS pin, 'h' for HW chip select");
        puts("\tpin: PIN to use for CS or number of HW CS line");
        if (ascii) {
            puts("\tdata: ASCII string of data to send");
        }
        else {
            puts("\tdata: RAW data in hex format, byte wise 0xYY 0xZZ...");
        }
        puts("\tc: if c is given, the transfer will not be terminated");
    }

    /* parse device and chip select pin */
    dev = parse_dev(argv[1]);
    cs = parse_cs(argv[2], argv[3]);
    if (dev < 0 || cs == SPI_CS_UNDEF) {
        return 1;
    }

    /* parse data */
    if (ascii) {
        strcpy((char *)buf, argv[4]);
        if (argc >= 5 && argv[5][0] == 'c') {
            cont = true;
        }
    }
    else {
        for (int i = 2; i < argc; i++) {
            if (argv[i][0] == 'c') {
                cont = true;
                break;
            }
            else {
                buf[len++] = (uint8_t)strtol(argv[i], NULL, 0);
            }
        }
    }

    /* transfer data */
    puts("TX:");
    dump_hex(buf, len);
    if (len) {
        spi_acquire(SPI_DEV(dev));
        spi_transfer_bytes(SPI_DEV(dev), cs, cont, buf, buf, len);
        spi_release(SPI_DEV(dev));
    }
    puts("RX:");
    dump_hex(buf, len);

    return 0;
}

int cmd_init_master(int argc, char **argv)
{
    int dev, speed, conf, res;

    if (argc < 4) {
        printf("usage: %s <dev> <conf> <speed>\n", argv[0]);
        puts("\tdev: SPI device to use");
        print_modes();
        print_speeds();
    }

    /* parse params */
    dev = parse_dev(argv[1]);
    speed = parse_speed(argv[2]);
    conf = parse_conf(argv[3]);
    if (dev < 0 || speed < 0 || conf < 0) {
        return 1;
    }

    /* initialize device. As this is a manual application, we get away without
     * locking the bus first... */
    res = spi_init_master(SPI_DEV(dev), conf, speed);
    if (res == -1) {
        puts("error in spi_init_master(): given device invalid");
        return 1;
    }
    if (res == -2) {
        puts("error in spi_init_master(): given speed no supported");
        return 1;
    }
    if (res == -3) {
        puts("error in spi_init_master(): configuration not supported");
        return 1;
    }
    if (res < -3) {
        puts("error in spi_init_master(): unspecified error");
        return 1;
    }
    printf("SPI_DEV(%i) successfully initialized as master\n", dev);
    return 0;
}

int cmd_init_slave(int argc, char **argv)
{
    int dev, conf, pol, res;
    spi_cs_t cs;

    if (argc < 6) {
        printf("usage: %s <dev> <conf> <port> <pin> <pol>\n", argv[0]);
        puts("\tdev: SPI device to use");
        puts("\tport: Port of CS pin, H for HW chip select");
        puts("\tpin: PIN to use for CS or number of HW CS line");
        print_modes();
        print_pol();
    }

    /* parse parameters */
    dev = parse_dev(argv[1]);
    conf = parse_conf(argv[2]);
    cs = parse_cs(argv[3], argv[4]);
    pol = parse_pol(argv[5]);
    if (dev < 0 || conf < 0 || pol < 0 || cs == SPI_CS_UNDEF) {
        return 1;
    }

    /* initialize device as SPI slave */
    res = spi_init_slave(SPI_DEV(dev), cs, pol, conf,
                         slave_cs_cb, slave_data_cb, (void *)dev);
    if (res == -1) {
        puts("error in spi_init_slave(): given device invalid");
        return 1;
    }
    if (res == -2) {
        puts("error in spi_init_slave(): slave mode not supported by device");
        return 1;
    }
    if (res < -2) {
        puts("error in spi_init_slave(): unspecified error");
        return 1;
    }
    printf("SPI_DEV(%i) successfully initialized as SPI slave\n", dev);
    return 0;
}

int cmd_ascii(int argc, char **argv)
{
    return send(argc, argv, true);
}

int cmd_raw(int argc, char **argv)
{
    return send(argc, argv, false);
}

static const shell_command_t shell_commands[] = {
    { "init_master", "initialize SPI device in master mode", cmd_init_master },
    { "init_slave", "initialize SPI device in slave mode", cmd_init_slave },
    { "send", "transfer string to slave (only in master mode)", cmd_ascii },
    { "send_hex", "transfer hex encoded bytes(master mode only)", cmd_raw },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_t shell;

    puts("\nManual SPI peripheral driver test application");
    puts("=============================================");
    puts("With this application it is possible to test all SPI devices that\n"
         "may be defined for a platform. You can configure available devices\n"
         "as master or slave, using the provided shell commands.\n\n"
         "When configured as master, you can test your SPI device by sending\n"
         "ASCII strings or by sending hex encoded binary data\n\n"
         "When configured as slave, the device will simply answer with\n"
         "counting numbers for each byte transferred, starting with 0x01\n");

    /* run the shell */
    shell_init(&shell, shell_commands, SHELL_BUFSIZE, getchar, putchar);
    shell_run(&shell);

    return 0;
}
