/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief SPI test application
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <board.h>
#include <time.h>

#include "periph/spi.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "thread.h"
#include "vtimer.h"
#include "shell.h"
#include "shell_commands.h"
#include "stm32f407xx.h"

#define USE_SPI_0_MASTER         1

#define SHELL_BUFFER_SIZE        128
#define BUF_SEND_LEN             10

static int shell_read(void);
static void shell_write(int);
static void cmd_init_start_slave(int argc, char **argv);
static void cmd_init_master(int argc, char **argv);
static void cmd_send_master_byte(int argc, char **argv);
static void cmd_send_master_bytes(int argc, char **argv);
static void cmd_send_master_8x1_byte(int argc, char **argv);
static void cmd_test_nrf(int argc, char **argv);
static void cmd_prxbuf(int argc, char **argv);
static void cmd_clearbuf(int argc, char **argv);

char test_buf[BUF_SEND_LEN] = {0};
char buf_count = 0;

char test_irq(char data)
{
    test_buf[buf_count] = data;
    buf_count++;
    /* printf("my_cool_uebergabeparameter_was: %x\n", data); */
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
    { "nrf", "test nrf", cmd_test_nrf },
    { "prxbuf", "print rx buffer from cb function", cmd_prxbuf },
    { "clearbuf", "clear rx buffer from cb function", cmd_clearbuf },
    { NULL, NULL, NULL }
};

/**
 * @init start slave
 */
void cmd_init_start_slave(int argc, char **argv)
{
    puts("Init start Slave\n");

    (void) argc;
    (void) argv;

    spi_poweron(SPI_0);
    spi_init_slave(SPI_0, SPI_CONF_FIRST_RISING, test_irq);

    int cr1_reg, cr2_reg;
    cr1_reg = (SPI_0_DEV->CR1);
    cr2_reg = (SPI_0_DEV->CR2);
    printf("CR1 Register Slave: %x\nCR2 Register Slave: %x\n", cr1_reg, cr2_reg);

}

/**
 * @init master
 */
void cmd_init_master(int argc, char **argv)
{
    printf("Init Master USE_SPI_0_MASTER: %i\n", USE_SPI_0_MASTER);

    (void) argc;
    (void) argv;

    gpio_init_out(GPIO_7, GPIO_NOPULL); /* GPIO_7 is mapped to pin PD6 */
    gpio_set(GPIO_7);

#if USE_SPI_0_MASTER
    spi_poweron(SPI_0);
#else
    spi_poweron(SPI_1);
#endif

#if USE_SPI_0_MASTER
    spi_init_master(SPI_0, SPI_CONF_FIRST_RISING, SPI_SPEED_400KHZ);
#else
    spi_init_master(SPI_1, SPI_CONF_FIRST_RISING, SPI_SPEED_400KHZ);
#endif

    int cr1_reg, cr2_reg, i2cfgr;

#if USE_SPI_0_MASTER
    cr1_reg = (SPI_0_DEV->CR1);
    cr2_reg = (SPI_0_DEV->CR2);
    i2cfgr  = SPI_0_DEV->I2SCFGR;
#else
    cr1_reg = (SPI_1_DEV->CR1);
    cr2_reg = (SPI_1_DEV->CR2);
#endif

    printf("CR1 Register Master: %x\nCR2 Register Master: %x\n i2cfgr Register Master: %x\n", cr1_reg, cr2_reg, i2cfgr);

}

/**
 * @send master one byte
 */
void cmd_send_master_byte(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Send Master 1 Byte\n");

    char data_return, data_send = 1;

    gpio_clear(GPIO_7);

#if USE_SPI_0_MASTER
    spi_transfer_byte(SPI_0, data_send, &data_return);
#else
    spi_transfer_byte(SPI_1, data_send, &data_return);
#endif
    gpio_set(GPIO_7);
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

    char buf_send[BUF_SEND_LEN] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1,};
    char buf_return[BUF_SEND_LEN];

    gpio_clear(GPIO_7);

#if USE_SPI_0_MASTER
    spi_transfer_bytes(SPI_0, buf_send, buf_return, BUF_SEND_LEN);
#else
    spi_transfer_bytes(SPI_1, buf_send, buf_return, BUF_SEND_LEN);
#endif

    gpio_set(GPIO_7);

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

    char data_return, data_send = 1;

    for (int i = 0; i < 8; i++) {
        gpio_clear(GPIO_7);

#if USE_SPI_0_MASTER
        spi_transfer_byte(SPI_0, data_send, &data_return);
#else
        spi_transfer_byte(SPI_1, data_send, &data_return);
#endif
        gpio_set(GPIO_7);
        printf("One Byte transferred: %x, received: %x\n", data_send, data_return);
        data_send++;
    }
}

/**
 * @test nrf
 */
void cmd_test_nrf(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Test call nrf24l01+\n");
    /*
    #define R_REGISTER      0x00
    #define W_REGISTER      0x20
    #define NOP             0xFF

    #define REGISTER_MASK   0x1F
    #define	CONFIG_PWR_UP   0x02
    #define	CONFIG_PRIM_RX  0x01
    #define	CONFIG_CRCO     0x04

    #define CONFIG          0x00

    #define DUMMY           0xAA

    char data_return, data_send;

    	// Try to get status register while command NOP
    	spi_transfer_byte(SPI_0, NOP, &data_return);
    	gpio_set(GPIO_7);
    	printf("NOP returned: %x\n", data_return);


    	// Try to write to CONFIG register to set power up
    	gpio_clear(GPIO_7);
    	spi_transfer_reg(SPI_0, (W_REGISTER | (REGISTER_MASK & CONFIG)), CONFIG_CRCO, &data_return);
    	gpio_set(GPIO_7);
    	printf("wrote to config register. answer should be random: %x\n", data_return);


    	// Try to read out CONFIG register
    	gpio_clear(GPIO_7);
    	spi_transfer_reg(SPI_0, (R_REGISTER | (REGISTER_MASK & CONFIG)), DUMMY, &data_return);
    	gpio_set(GPIO_7);
    	printf("read out config register. answer: %x\n", data_return);


    	// Try to clear CONF reg again
    	gpio_clear(GPIO_7);
    	spi_transfer_reg(SPI_0, (W_REGISTER | (REGISTER_MASK & CONFIG)), CONFIG_CRCO, &data_return);
    	gpio_set(GPIO_7);
    	printf("after clear config register. answer should be random: %x\n", data_return);


    	// Try to read out CONFIG register
    	gpio_clear(GPIO_7);
    	spi_transfer_reg(SPI_0, (R_REGISTER | (REGISTER_MASK & CONFIG)), DUMMY, &data_return);
    	gpio_set(GPIO_7);
    	printf("read out config register ater clear. answer: %x\n", data_return);
    */
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

    for (int i = 0; i < BUF_SEND_LEN; i++) {
        printf("Sequence buffer: %x \n", test_buf[i]);
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

    for (int i = 0; i < BUF_SEND_LEN; i++) {
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

