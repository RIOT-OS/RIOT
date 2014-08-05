/*
 * Copyright (C) 2014 University of Applied Sciences
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
#include "sam3x8e.h"

#define SHELL_BUFFER_SIZE       128
#define BUF_SEND_LEN 			8

static int shell_read(void);
static void shell_write(int);
static void cmd_init_start_slave(int argc, char **argv);
static void cmd_init_master(int argc, char **argv);
static void cmd_send_master_byte(int argc, char **argv);
static void cmd_send_master_bytes(int argc, char **argv);
//static void cmd_test_nrf(int argc, char **argv);
static void cmd_prxbuf(int argc, char **argv);
static void cmd_clearbuf(int argc, char **argv);

char test_buf[BUF_SEND_LEN]={0};
char buf_count = 0;

char test_irq(char c) {
	test_buf[buf_count] = c;
	buf_count++;
	//printf("my_cool_uebergabeparameter_was: %d\n", c);
	return 6;
}

/**
 * define some additional shell commands
 */
static const shell_command_t shell_commands[] = {
		{ "is", "init slave", cmd_init_start_slave },
		{ "im", "init master", cmd_init_master },
		{ "smb", "send master byte", cmd_send_master_byte },
		{ "smbs", "send master bytes", cmd_send_master_bytes },
		//{ "nrf", "test nrf", cmd_test_nrf },
		{ "prxbuf", "print rx buffer from cb function", cmd_prxbuf },
		{ "clearbuf", "clear rx buffer from cb function", cmd_clearbuf },
		{ NULL, NULL, NULL } };

/**
 * @init start slave
 */
void cmd_init_start_slave(int argc, char **argv) {
	puts("Init Start Slave\n");

	(void) argc;
	(void) argv;

	spi_poweron(SPI_0);
	spi_init_slave(SPI_0, SPI_CONF_FIRST_RISING, test_irq);

	int mr, sr, imr, csr;
	mr = SPI_0_DEV->SPI_MR;
	sr = SPI_0_DEV->SPI_SR;
	imr = SPI_0_DEV->SPI_IMR;
	csr = SPI_0_DEV->SPI_CSR[0];

	printf("mr: %x\n sr: %x\n imr: %x\n csr: %x\n", mr, sr, imr, csr);

}

/**
 * @init master
 */
void cmd_init_master(int argc, char **argv) {
	puts("Init Master\n");

	(void) argc;
	(void) argv;

	spi_poweron(SPI_0);

	uint32_t speed = 7;	/* this is the divider not the freq.! */
	spi_init_master(SPI_0, SPI_CONF_FIRST_RISING, speed);

	uint32_t mr, sr, imr, csr;
	mr = SPI_0_DEV->SPI_MR;
	sr = SPI_0_DEV->SPI_SR;
	imr = SPI_0_DEV->SPI_IMR;
	csr = SPI_0_DEV->SPI_CSR[0];

	printf("mr: %x\n sr: %x\n imr: %x\n csr: %x\n", mr, sr, imr, csr);
}

/**
 * @send master one byte
 */
void cmd_send_master_byte(int argc, char **argv) {

	(void) argc;
	(void) argv;

	puts("Send Master 8 x 1 Byte\n");

	int i;
	char data_return, data_send = 0x0;
	for (i = 0; i < 8; i++) {
		spi_transfer_byte(SPI_0, data_send, &data_return);
		printf("sent : %x\n", data_send);
		printf("receive: %x\n", data_return);
		data_send++;
	}
}

/**
 * @send master multiple byte
 */

void cmd_send_master_bytes(int argc, char **argv) {

	(void) argc;
	(void) argv;
	vtimer_init();
	puts("Send Master multiple Bytes\n");

	int i, trans_bytes;
	
	char buf_send[BUF_SEND_LEN] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
	char buf_return[BUF_SEND_LEN];

	  
	trans_bytes = spi_transfer_bytes(SPI_0, buf_send, buf_return, BUF_SEND_LEN);

	printf("trans_bytes: %x\n", trans_bytes);

	for(i=0; i<BUF_SEND_LEN; i++){
	
		printf("send %d : %x\n reveice %d: %x\n", i, buf_send[i], i, buf_return[i]);
	}
}

/**
 * @test nrf
 */
/*
void cmd_test_nrf(int argc, char **argv) {

	(void) argc;
	(void) argv;

	puts("Test call nrf24l01+\n");
/*
#define R_REGISTER		0x00
#define W_REGISTER		0x20
#define NOP				0xFF

#define REGISTER_MASK	0x1F
#define	CONFIG_PWR_UP	0x02
#define	CONFIG_PRIM_RX	0x01
#define	CONFIG_CRCO		0x04

#define CONFIG      	0x00

#define DUMMY			0xAA

char data_return, data_send;

	gpio_init_out(GPIO_7, GPIO_NOPULL); // GPIO_7 is mapped to pin PD6 -> see attached spreadsheet
	gpio_set(GPIO_7);


	// Try to get status register while command NOP
	gpio_clear(GPIO_7);
	spi_transfer_byte(SPI_0, NOP, &data_return);
	gpio_set(GPIO_7);
	printf("NOP returned: %x\n", data_return);

	data_send = CONFIG_CRCO;

	// Try to write to CONFIG register to set power up
	gpio_clear(GPIO_7);
	spi_transfer_reg(SPI_0, (W_REGISTER | (REGISTER_MASK & CONFIG)), data_send, &data_return);
	gpio_set(GPIO_7);
	printf("wrote to config register. answer should be random: %x\n", data_return);


	// Try to read out CONFIG register
	gpio_clear(GPIO_7);
	spi_transfer_reg(SPI_0, (R_REGISTER | (REGISTER_MASK & CONFIG)), DUMMY, &data_return);
	gpio_set(GPIO_7);
	printf("read out config register. answer: %x\n", data_return);


	data_send = CONFIG_CRCO;

	// Try to clear CONF reg again
	gpio_clear(GPIO_7);
	spi_transfer_reg(SPI_0, (W_REGISTER | (REGISTER_MASK & CONFIG)), data_send, &data_return);
	gpio_set(GPIO_7);
	printf("after clear config register. answer should be random: %x\n", data_return);


	// Try to read out CONFIG register
	gpio_clear(GPIO_7);
	spi_transfer_reg(SPI_0, (R_REGISTER | (REGISTER_MASK & CONFIG)), DUMMY, &data_return);
	gpio_set(GPIO_7);
	printf("read out config register ater clear. answer: %x\n", data_return);
*/

/*
}


/**
 * @print buffer written in cb function
 */

void cmd_prxbuf(int argc, char **argv) {

	(void) argc;
	(void) argv;

	puts("Buffer print function\n");

	buf_count = 0;

	int i;

	for(i=0; i<BUF_SEND_LEN; i++){
		printf("sequence buffer: %x \n", test_buf[i]);
	}
}

/**
 * @clear buffer written in cb function
 */

void cmd_clearbuf(int argc, char **argv) {

	(void) argc;
	(void) argv;

	puts("Buffer clear function\n");

	buf_count = 0;

	int i;
	for(i=0; i<BUF_SEND_LEN; i++){
		test_buf[i] =  0;
	}
}


/**
 * @brief proxy for reading a char from std-in and passing it to the shell
 */
int shell_read(void) {
	return (int) getchar();
}

/**
 * @brief proxy for taking a character from the shell and writing it to std-out
 */
void shell_write(int c) {
	putchar((char )c);
}

int main(void) {
	shell_t shell;

	puts("Welcome to RIOT!");

	puts("Initializing shell...");
	shell_init(&shell, shell_commands, SHELL_BUFFER_SIZE, shell_read,
			shell_write);

	puts("Starting shell...");
	shell_run(&shell);
	return 0;
}

