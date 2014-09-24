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
 * @brief       Test application for nrf24l01p network functions
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
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "msg.h"

#define __STDC_FORMAT_MACROS

#define ENABLE_DEBUG (1)
#include "debug.h"

#define SHELL_BUFSIZE (128U)

static void cmd_print_regs(int argc, char **argv);
static void cmd_init_transceiver(int argc, char **argv);
static void cmd_send(int argc, char **argv);
static void cmd_prxbuf(int argc, char **argv);


void printbin(unsigned byte);
void print_register(char reg, int num_bytes);

#define STATIC_RX_BUF_SIZE  128
static char static_rx_buf[STATIC_RX_BUF_SIZE];
static int j = 0;


char rx_handler_stack[KERNEL_CONF_STACKSIZE_MAIN];

static netdev_t dev_mem;
static netdev_t *dev = &dev_mem;


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

/**
 * @callback function on data receive
 */
int deliver_data_cb(netdev_t *dev, void *src, size_t src_len,
                    void *dest, size_t dest_len, void *payload,
                    size_t payload_len)
{
    LD3_TOGGLE;

    if (j > (STATIC_RX_BUF_SIZE - 1)) {
        j = 0;
    }

    for (size_t i = 0; i < payload_len; i++) {
        static_rx_buf[j] = ((char *)payload)[i];
        j++;
    }

    return 0;
}

/* RX handler that waits for a message from the ISR */
void *nrf24l01p_rx_handler(void *arg)
{
    dev = ((netdev_t *)(&nrf24l01p_netdev));

    msg_t m;
    msg_t msg_q[1];
    msg_init_queue(msg_q, 1);
    unsigned int pid = thread_getpid();
    unsigned int get_pid;
    int res;

    puts("Registering nrf24l01p_rx_handler thread...");

    /* set pid */
    res = dev->driver->set_option(dev, NETDEV_OPT_NID, &pid, sizeof(int));

    if (res < 0) {
        printf("Error in set ID: %i\n", res);
    }

    res = dev->driver->get_option(dev, NETDEV_OPT_NID, &get_pid, (size_t *)sizeof(int));

    if (res < 0) {
        printf("Error in get ID: %i\n", res);
    }
    else if (pid == get_pid) {
        printf("ID was set sucessful to: %i\n", get_pid);
    }
    else {
        puts("Set/get ID mismatch");
    }

    while (msg_receive(&m)) {
        switch (m.type) {
            case RCV_PKT_NRF24L01P:
                LD4_TOGGLE;
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
 * @Initialize Transceiver
 */
void cmd_init_transceiver(int argc, char **argv)
{
    puts("Initialize Transceiver\n");

    dev = ((netdev_t *)(&nrf24l01p_netdev));

    int res;

    /* to get/set state */
    netdev_state_t state;

    /* for channel test*/
    uint8_t set_channel = INITIAL_RF_CHANNEL;
    uint8_t get_channel;

    /* for tx address test */
    netdev_nrf24l01p_addr_t get_address_tx;
    get_address_tx.type = NETDEV_NRF24L01P_ADDR_TYPE_TX;

    netdev_nrf24l01p_addr_t set_address_tx;
    set_address_tx.type = NETDEV_NRF24L01P_ADDR_TYPE_TX;
    set_address_tx.addr = 0xe9e9e9e9e9;

    /* for rx address test */
    nrf24l01p_rx_pipe_t set_rx_pipe = NRF24L01P_PIPE0;
    netdev_nrf24l01p_addr_t get_address_rx;
    get_address_rx.type = NETDEV_NRF24L01P_ADDR_TYPE_RX;
    get_address_rx.pipe = set_rx_pipe;

    netdev_nrf24l01p_addr_t set_address_rx;
    set_address_rx.type = NETDEV_NRF24L01P_ADDR_TYPE_RX;
    set_address_rx.pipe = set_rx_pipe;
    set_address_rx.addr = 0xe9e9e9e9e9;

    /* to set power */
    int set_power = 0;
    int get_power;

    /* get max. packet size */
    uint8_t get_max_packetsize;

    /* get protocol type */
    netdev_proto_t get_proto;

    /* call init function */
    res = dev->driver->init(dev);

    if (res < 0) {
        puts("Error during initialization");
    }

    /* get state */
    res = dev->driver->get_state(dev, &state);

    if (res < 0) {
        printf("Error in get state: %i\n", res);
    }

    switch (state) {
        case NETDEV_STATE_POWER_OFF:
            puts("State is: NETDEV_STATE_POWER_OFF");
            break;

        case NETDEV_STATE_POWER_SLEEP:
            puts("State is: NETDEV_STATE_POWER_SLEEP");
            break;

        case NETDEV_STATE_RX_MODE:
            puts("State is: NETDEV_STATE_RX_MODE");
            break;

        case NETDEV_STATE_POWER_IDLE:
            puts("Error. State not defined for this device");
            break;

        case NETDEV_STATE_PROMISCUOUS_MODE:
            puts("Error. State not defined for this device");
            break;
    }

    /* set channel */
    res = dev->driver->set_option(dev, NETDEV_OPT_CHANNEL, &set_channel, sizeof(uint8_t));

    if (res < 0) {
        printf("Error in set channel: %i\n", res);
    }

    res = dev->driver->get_option(dev, NETDEV_OPT_CHANNEL, &get_channel, (size_t *)sizeof(int));

    if (res < 0) {
        printf("Error in get channel: %i\n", res);
    }
    else if (set_channel == get_channel) {
        printf("Channel was set sucessful to: %i\n", get_channel);
    }
    else {
        puts("Set/get channel mismatch");
    }

    /* set tx address */
    res = dev->driver->set_option(dev, NETDEV_OPT_ADDRESS, &set_address_tx, sizeof(netdev_nrf24l01p_addr_t));

    if (res < 0) {
        printf("Error in set tx adress: %i\n", res);
    }

    res = dev->driver->get_option(dev, NETDEV_OPT_ADDRESS, &get_address_tx, (size_t *)sizeof(netdev_nrf24l01p_addr_t));

    if (res < 0) {
        printf("Error in get tx adress: %i\n", res);
    }
    else if (set_address_tx.addr == get_address_tx.addr) {
        printf("TX address was set sucessful to: %x%x\n", (int)(get_address_tx.addr), (int)(get_address_tx.addr >> 32));
    }
    else {
        puts("Set/get TX address mismatch");
    }

    /* set rx address */
    res = dev->driver->set_option(dev, NETDEV_OPT_ADDRESS, &set_address_rx, sizeof(netdev_nrf24l01p_addr_t));

    if (res < 0) {
        printf("Error in set rx adress: %i\n", res);
    }

    res = dev->driver->get_option(dev, NETDEV_OPT_ADDRESS, &get_address_rx, (size_t *)sizeof(netdev_nrf24l01p_addr_t));

    if (res < 0) {
        printf("Error in get rx adress: %i\n", res);
    }
    else if (set_address_rx.addr == get_address_rx.addr) {
        printf("RX address was set sucessful to: %x%x\n", (int)(get_address_rx.addr), (int)(get_address_rx.addr >> 32));
    }
    else {
        puts("Set/get RX address mismatch");
    }

    /* disable rx pipe 1 which is enabled by default */
    set_address_rx.type = NETDEV_NRF24L01P_ADDR_TYPE_RX;
    set_address_rx.pipe = NRF24L01P_PIPE1;
    set_address_rx.addr = 0;

    res = dev->driver->set_option(dev, NETDEV_OPT_ADDRESS, &set_address_rx, sizeof(netdev_nrf24l01p_addr_t));

    if (res < 0) {
        printf("Error in disable pipe: %i\n", res);
    }

    printf("Disabled PIPE %i\n", set_address_rx.pipe);

    /* set tx power*/
    res = dev->driver->set_option(dev, NETDEV_OPT_TX_POWER, &set_power, sizeof(int));

    if (res < 0) {
        printf("Error in set power: %i\n", res);
    }

    res = dev->driver->get_option(dev, NETDEV_OPT_TX_POWER, &get_power, (size_t *)sizeof(int));

    if (res < 0) {
        printf("Error in get power: %i\n", res);
    }

    printf("Power was set sucessful to: %i dB\n", get_power);

    /* get max. packet sitze */
    res = dev->driver->get_option(dev, NETDEV_OPT_MAX_PACKET_SIZE, &get_max_packetsize, (size_t *)sizeof(uint8_t));

    if (res < 0) {
        printf("Error in get packet size: %i\n", res);
    }

    printf("Max. packet size is: %i Byte\n", get_max_packetsize);

    /* get proto */
    res = dev->driver->get_option(dev, NETDEV_OPT_PROTO, &get_proto, (size_t *)sizeof(netdev_proto_t));

    if (res < 0) {
        printf("Error in get packet size: %i\n", res);
    }
    else if (get_proto == NETDEV_PROTO_PROPRIETARY) {
        puts("Protocol type is: NETDEV_PROTO_PROPRIETARY");
    }
    else {
        printf("Protocol type is: %i\n", get_proto);
    }

    /* set/get state */
    res = dev->driver->set_state(dev, NETDEV_STATE_RX_MODE);

    if (res < 0) {
        printf("Error in set state: %i\n", res);
    }

    res = dev->driver->get_state(dev, &state);

    if (res < 0) {
        printf("Error in get state: %i\n", res);
    }

    switch (state) {
        case NETDEV_STATE_POWER_OFF:
            puts("State is: NETDEV_STATE_POWER_OFF");
            break;

        case NETDEV_STATE_POWER_SLEEP:
            puts("State is: NETDEV_STATE_POWER_SLEEP");
            break;

        case NETDEV_STATE_RX_MODE:
            puts("State is: NETDEV_STATE_RX_MODE");
            break;

        case NETDEV_STATE_POWER_IDLE:
            puts("Error. State not defined for this device");
            break;

        case NETDEV_STATE_PROMISCUOUS_MODE:
            puts("Error. State not defined for this device");
            break;
    }

    /* Register callback function on receive */
    res = dev->driver->add_receive_data_callback(dev, deliver_data_cb);

    if (res < 0) {
        printf("Error in add rx callback: %i\n", res);
    }
    else {
        puts("Registrated callback function");
    }

    /* create thread that gets msg when data arrives */
    thread_create(
        rx_handler_stack, sizeof(rx_handler_stack), PRIORITY_MAIN - 1, 0,
        nrf24l01p_rx_handler, 0, "nrf24l01p_rx_handler");
}

/**
 * @send data
 */
void cmd_send(int argc, char **argv)
{
    puts("Send data");

    int res;
    int tx_buf_len = 64;
    char tx_buf[(tx_buf_len)];

    for (int i = 0; i < (tx_buf_len); i++) {
        tx_buf[i] = i;
    }

    /* send data */
    res = dev->driver->send_data(dev, 0, 0, 0, tx_buf, (size_t)(tx_buf_len));

    if (res < 0) {
        printf("Error in send data: %i\n", res);
    }
}

/**
 * @print rx_buf
 */
void cmd_prxbuf(int argc, char **argv)
{
    for (int i = 0; i < STATIC_RX_BUF_SIZE; i++) {
        if ((i % 4) == 0 && (i != 0)) {
            printf("\n");
        }

        printf("%i ", static_rx_buf[i]);
    }

    printf("\n");
}


/**
 * @print registers
 */
void cmd_print_regs(int argc, char **argv)
{

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
 * define some additional shell commands
 */
static const shell_command_t shell_commands[] = {
    { "it", "Initialize Transceiver", cmd_init_transceiver },
    { "prgs", "print registers", cmd_print_regs },
    { "send", "send data", cmd_send },
    { "prxbuf", "print receive buffer", cmd_prxbuf },
    { NULL, NULL, NULL }
};

int shell_getchar(void)
{
    return (int)getchar();
}

void shell_putchar(int c)
{
    putchar((char)c);
}

int main(void)
{
    shell_t shell;
    puts("Welcome to RIOT!");
    puts("Initializing shell...");
    shell_init(&shell, shell_commands, SHELL_BUFSIZE, shell_getchar, shell_putchar);
    puts("Starting shell...");
    shell_run(&shell);
    return 0;
}
