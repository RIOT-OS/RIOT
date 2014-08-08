/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       SPI test application
 *
 * @author      Maxime Blanloeil <maxime.blanloeil@phelma.grenoble-inp.fr>
 *
 * @}
 */

#include <stdio.h>
#include <board.h>

#include "periph/spi.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "sam3x8e.h"

#define SHELL_BUFFER_SIZE       128
#define BUF_SEND_LEN 			10

static int shell_read(void);
static void shell_write(int);
static void cmd_init_start_slave(int argc, char **argv);
static void cmd_init_master(int argc, char **argv);
static void cmd_send_master_byte(int argc, char **argv);
static void cmd_send_master_bytes(int argc, char **argv);
static void cmd_send_master_8x1_byte(int argc, char **argv);
static void cmd_prxbuf(int argc, char **argv);
static void cmd_clearbuf(int argc, char **argv);

char test_buf[BUF_SEND_LEN] = {0};
char buf_count = 0;

char test_irq(char data)
{
    if (buf_count < BUF_SEND_LEN) {
        test_buf[buf_count] = data;
        buf_count++;
    }
    else {
        buf_count = 0;
    }

    return data;
}

/**
 * define some additional shell commands
 */
static const shell_command_t shell_commands[] = {
    { "is", "init slave", cmd_init_start_slave },
    { "im", "init master", cmd_init_master },
    { "smb", "send master byte", cmd_send_master_byte },
    { "smbs", "send master bytes", cmd_send_master_bytes },
    { "smb81", "send master 8x1 bytes", cmd_send_master_8x1_byte },
    { "prxbuf", "print rx buffer from cb function", cmd_prxbuf },
    { "clearbuf", "clear rx buffer from cb function", cmd_clearbuf },
    { NULL, NULL, NULL }
};

/**
 * @init start slave
 */
void cmd_init_start_slave(int argc, char **argv)
{
    puts("Init Start Slave\n");

    (void) argc;
    (void) argv;

    spi_poweron(SPI_0);
    spi_init_slave(SPI_0, SPI_CONF_FIRST_RISING, test_irq);

}

/**
 * @init master
 */
void cmd_init_master(int argc, char **argv)
{
    puts("Init Master\n");

    (void) argc;
    (void) argv;

    spi_poweron(SPI_0);
    uint32_t speed = 255;	/* this is the divider not the freq.! */
    spi_init_master(SPI_0, SPI_CONF_FIRST_RISING, speed);

}

/**
 * @send master one byte
 */
void cmd_send_master_byte(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Send Master 1 Byte\n");

    char data_return, data_send = 90;

    spi_transfer_byte(SPI_0, data_send, &data_return);

    printf("One Byte transferred: %x, received: %x\n", data_send, data_return);

}

/**
 * @send master multiple byte
 */
void cmd_send_master_bytes(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Send Master multiple Bytes\n");

    char buf_send[BUF_SEND_LEN] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    char buf_return[BUF_SEND_LEN];

    spi_transfer_bytes(SPI_0, buf_send, buf_return, BUF_SEND_LEN);

    for (int i = 0; i < BUF_SEND_LEN; i++) {
        printf("Send %d : %x\n Reveice %d: %x\n", i, buf_send[i], i, buf_return[i]);
    }
}

/**
 * @send master one 8 x 1 byte
 */
void cmd_send_master_8x1_byte(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Send Master 8 x 1 Byte\n");

    char data_return = 0, data_send = 1;

    for (int i = 0; i < 8; i++) {
        spi_transfer_byte(SPI_0, data_send, &data_return);
        printf("One Byte transferred: %x, received: %x\n", data_send, data_return);
        data_send++;
    }
}

/**
 * @print buffer written in cb function
 */
void cmd_prxbuf(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Buffer print function\n");

    buf_count = 0;

    int i;

    for (i = 0; i < BUF_SEND_LEN; i++) {
        printf("sequence buffer: %x \n", test_buf[i]);
    }
}

/**
 * @clear buffer written in cb function
 */
void cmd_clearbuf(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Buffer clear function\n");

    buf_count = 0;

    int i;

    for (i = 0; i < BUF_SEND_LEN; i++) {
        test_buf[i] =  0;
    }
}


/**
 * @brief proxy for reading a char from std-in and passing it to the shell
 */
int shell_read(void)
{
    return (int) getchar();
}

/**
 * @brief proxy for taking a character from the shell and writing it to std-out
 */
void shell_write(int c)
{
    putchar((char)c);
}

int main(void)
{
    shell_t shell;

    puts("Welcome to RIOT!");

    puts("Initializing shell...");
    shell_init(&shell, shell_commands, SHELL_BUFFER_SIZE, shell_read,
               shell_write);

    puts("Starting shell...");
    shell_run(&shell);
    return 0;
}

