/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
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
 * @brief       Test application for nrf24l01p lowlevel functions
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#ifndef SPI_PORT
#error "SPI_PORT not defined"
#endif
#ifndef CE_PIN
#error "CE_PIN not defined"
#endif
#ifndef CS_PIN
#error "CS_PIN not defined"
#endif
#ifndef IRQ_PIN
#error "IRQ_PIN not defined"
#endif


#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <board.h>
#include <time.h>

#include "nrf24l01p.h"
#include "nrf24l01p_settings.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "vtimer.h"
#include "hwtimer.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "msg.h"

#define TEST_RX_MSG                1

#define SHELL_BUFFER_SIZE        128

static int shell_read(void);
static void shell_write(int);
static void cmd_send(int argc, char **argv);
static void cmd_print_regs(int argc, char **argv);
static void cmd_its(int argc, char **argv);


void printbin(unsigned byte);
void print_register(char reg, int num_bytes);


static nrf24l01p_t nrf24l01p_0;

/**
 * define some additional shell commands
 */
static const shell_command_t shell_commands[] = {
    { "prgs", "print registers", cmd_print_regs },
    { "it", "init transceiver", cmd_its },
    { "send", "send 32 bytes data", cmd_send },
    { NULL, NULL, NULL }
};

void prtbin(unsigned byte)
{
    for (char i = 0; i < 8; i++) {
        printf("%u", (byte >> (7 - i)) & 0x0001);
    }

    puts("\n");
}

/**
 * @print register
 */
void print_register(char reg, int num_bytes)
{

    vtimer_init();

    char buf_return[num_bytes];
    int ret;


    gpio_clear(CS_PIN);
    vtimer_usleep(1);
    ret = spi_transfer_regs(SPI_PORT, (CMD_R_REGISTER | (REGISTER_MASK & reg)), 0, buf_return, num_bytes);
    gpio_set(CS_PIN);

    if (ret < 0) {
        printf("Error in read access\n");
    }
    else {
        if (num_bytes < 2) {
            printf("0x%x returned: ", reg);

            for (int i = 0; i < num_bytes; i++) {
                prtbin(buf_return[i]);
            }
        }
        else {
            printf("0x%x returned: ", reg);

            for (int i = 0; i < num_bytes; i++) {
                printf("%x ", buf_return[i]);
            }

            printf("\n\n");
        }
    }
}

char rx_handler_stack[KERNEL_CONF_STACKSIZE_MAIN];

/* RX handler that waits for a message from the ISR */
void *nrf24l01p_rx_handler(void *arg)
{
    msg_t msg_q[1];
    msg_init_queue(msg_q, 1);
    unsigned int pid = thread_getpid();
    char rx_buf[NRF24L01P_MAX_DATA_LENGTH];

    puts("Registering nrf24l01p_rx_handler thread...");
    nrf24l01p_register(&nrf24l01p_0, &pid);

    msg_t m;

    while (msg_receive(&m)) {
        printf("nrf24l01p_rx_handler got a message: ");

        switch (m.type) {
            case RCV_PKT_NRF24L01P:
                puts("Received packet.");

                /* CE low */
                nrf24l01p_stop((nrf24l01p_t *)m.content.ptr);

                /* read payload */
                nrf24l01p_read_payload((nrf24l01p_t *)m.content.ptr, rx_buf, NRF24L01P_MAX_DATA_LENGTH);

                /* flush rx fifo */
                nrf24l01p_flush_rx_fifo((nrf24l01p_t *)m.content.ptr);

                /* CE high */
                nrf24l01p_start((nrf24l01p_t *)m.content.ptr);

                /* print rx buffer */
                for (int i = 0; i < NRF24L01P_MAX_DATA_LENGTH; i++) {
                    printf("%i ", rx_buf[i]);
                }

                puts("");

                break;

            default:
                puts("stray message.");
                break;
        }
    }

    puts("nrf24l01p_rx_handler: this should not have happened!");

    return NULL;
}

/**
 * @init transceiver
 */
void cmd_its(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("Init Transceiver\n");

    nrf24l01p_init(&nrf24l01p_0, SPI_PORT, CE_PIN, CS_PIN, IRQ_PIN);

    /* create thread that gets msg when data arrives */
    thread_create(
        rx_handler_stack, sizeof(rx_handler_stack), PRIORITY_MAIN - 1, 0,
        nrf24l01p_rx_handler, 0, "nrf24l01p_rx_handler");

    /* setup device as receiver */
    nrf24l01p_set_rxmode(&nrf24l01p_0);

    cmd_print_regs(0, 0);
}

/**
 * @set TX mode
 */
void cmd_send(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("Send");

    int status = 0;
    char tx_buf[NRF24L01P_MAX_DATA_LENGTH];

    /* fill TX buffer with numbers 32..1 */
    for (int i = 0; i < sizeof(tx_buf); i++) {
        tx_buf[i] = NRF24L01P_MAX_DATA_LENGTH - i;
    }
    /* power on the device */
    nrf24l01p_on(&nrf24l01p_0);
    /* setup device as transmitter */
    nrf24l01p_set_txmode(&nrf24l01p_0);
    /* load data to transmit into device */
    nrf24l01p_preload(&nrf24l01p_0, tx_buf, NRF24L01P_MAX_DATA_LENGTH);
    /* trigger transmitting */
    nrf24l01p_transmit(&nrf24l01p_0);
    /* wait while data is pysically transmitted  */
    hwtimer_wait(DELAY_DATA_ON_AIR);

    status = nrf24l01p_get_status(&nrf24l01p_0);
    if (status & TX_DS) {
        puts("Sent Packet");
    }
    /* setup device as receiver */
    nrf24l01p_set_rxmode(&nrf24l01p_0);
}

/**
 * @print registers
 */
void cmd_print_regs(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("################## Print Registers ###################\n");


    puts("REG_CONFIG: ");
    print_register(REG_CONFIG, 1);

    puts("REG_EN_AA: ");
    print_register(REG_EN_AA, 1);

    puts("REG_EN_RXADDR: ");
    print_register(REG_EN_RXADDR, 1);

    puts("REG_SETUP_AW: ");
    print_register(REG_SETUP_AW, 1);

    puts("REG_SETUP_RETR: ");
    print_register(REG_SETUP_RETR, 1);

    puts("REG_RF_CH: ");
    print_register(REG_RF_CH, 1);

    puts("REG_RF_SETUP: ");
    print_register(REG_RF_SETUP, 1);

    puts("REG_STATUS: ");
    print_register(REG_STATUS, 1);

    puts("REG_OBSERVE_TX: ");
    print_register(REG_OBSERVE_TX, 1);

    puts("REG_RPD: ");
    print_register(REG_RPD, 1);

    puts("REG_RX_ADDR_P0: ");
    print_register(REG_RX_ADDR_P0, INITIAL_ADDRESS_WIDTH);

    puts("REG_TX_ADDR: ");
    print_register(REG_TX_ADDR, INITIAL_ADDRESS_WIDTH);

    puts("REG_RX_PW_P0: ");
    print_register(REG_RX_PW_P0, 1);

    puts("REG_FIFO_STATUS: ");
    print_register(REG_FIFO_STATUS, 1);

    puts("REG_DYNPD: ");
    print_register(REG_DYNPD, 1);

    puts("REG_FEATURE: ");
    print_register(REG_FEATURE, 1);
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
