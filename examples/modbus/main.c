#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "modbus_rtu.h"
#include "periph/gpio.h"
#include "thread.h"
#include "xtimer.h"
#include "msg.h"
#define ENABLE_DEBUG (1)
#include "debug.h"


#define USE_SLAVE
#define USE_MASTER

#define SLAVE_ID 1
#define BAUDRATE 115200

#ifdef USE_MASTER
#define MASTER_UART 0
static char stack_master[600];
static kernel_pid_t pid_master;
static modbus_rtu_t master;
static uint16_t regs_master1[16];
static uint16_t regs_master2[16] __attribute__((unused));
static modbus_rtu_message_t message_master = {
    .id = SLAVE_ID,
    .addr = 0,
    .data = regs_master1,
    .data_size = sizeof(regs_master1),
    .count = 16
};
#endif /* USE_MASTER */

#ifdef USE_SLAVE
#define SLAVE_UART 2
static char stack_slave[550];
static kernel_pid_t pid_slave;
static modbus_rtu_t slave;
static uint16_t regs_slave[16];
static modbus_rtu_message_t message_slave = {
    .data = regs_slave,
    .data_size = sizeof(regs_slave)
};
#endif  /* USE_SLAVE */

#ifdef USE_MASTER
static void init_master(void)
{
    master.uart = UART_DEV(MASTER_UART);
    master.timeout = 1000000;
    master.id = 0;
    master.pin_rts = GPIO_UNDEF;
    /* master.pin_rts = GPIO_PIN(PORT_A, 1); */
    master.pin_rts_enable = 0;

    if (modbus_rtu_init(&master, BAUDRATE)) {
        DEBUG("fail UART init\n");
    }
}

static void *thread_master(void *arg __attribute__((unused)))
{
    int res __attribute__((unused));

    init_master();
    /* return NULL; */
    while (1) {
        xtimer_usleep(master.rx_timeout * 3);
        srand(xtimer_now_usec());
        for (uint8_t i = 0; i < message_master.count; i++) {
            regs_master1[i] = rand();
        }

#if defined(MODBUS_RTU_USE_WRITE_REGISTERS)
        DEBUG("try MB_FC_WRITE_REGISTERS\n");
        message_master.data = regs_master1;
        message_master.func = MB_FC_WRITE_REGISTERS;
        res = modbus_rtu_send_request(&master, &message_master);
        if (res) {
            DEBUG("fail MB_FC_WRITE_REGISTERS %d\n", res);
        }
        else {
#if defined(MODBUS_RTU_USE_READ_REGISTERS)
            xtimer_usleep(master.rx_timeout * 3);
            DEBUG("try MB_FC_READ_REGISTERS\n");
            message_master.data = regs_master2;
            message_master.func = MB_FC_READ_REGISTERS;
            res = modbus_rtu_send_request(&master, &message_master);
            if (res ||
                memcmp(regs_master1, regs_master2,
                       message_master.count * 2) != 0) {
                DEBUG("fail MB_FC_READ_REGISTERS %d\n", res);
            }
            else {}
#endif      /* MODBUS_RTU_USE_READ_REGISTERS */
        }
#endif      /* MODBUS_RTU_USE_WRITE_REGISTERS */

#if defined(MODBUS_RTU_USE_READ_DISCRETE_INPUT)
        DEBUG("try MB_FC_READ_DISCRETE_INPUT\n");
        message_master.data = regs_master1;
        message_master.func = MB_FC_READ_DISCRETE_INPUT;
        res = modbus_rtu_send_request(&master, &message_master);
        #ifdef USE_SLAVE
        if (res ||
            memcmp(regs_master1, regs_slave, message_master.count / 8) != 0) {
            DEBUG("fail MB_FC_READ_DISCRETE_INPUT %d\n", res);
        }
        #else /* USE_SLAVE */
        if (res) {
            DEBUG("fail MB_FC_READ_DISCRETE_INPUT %d\n", res);
        }
        #endif /* USE_SLAVE */
        else {}
#endif      /* MODBUS_RTU_USE_READ_DISCRETE_INPUT */

#if defined(MODBUS_RTU_USE_READ_INPUT_REGISTER)
        DEBUG("try MB_FC_READ_INPUT_REGISTER\n");
        message_master.data = regs_master1;
        message_master.func = MB_FC_READ_INPUT_REGISTER;
        res = modbus_rtu_send_request(&master, &message_master);
        #ifdef USE_SLAVE
        if (res || message_master.data[0] != regs_slave[0]) {
            DEBUG("fail MB_FC_READ_INPUT_REGISTER %d\n", res);
        }
        #else /* USE_SLAVE */
        if (res) {
            DEBUG("fail MB_FC_READ_INPUT_REGISTER %d\n", res);
        }
        #endif /* USE_SLAVE */
        else {}
#endif      /* MODBUS_RTU_USE_READ_INPUT_REGISTER */

#if defined(MODBUS_RTU_USE_WRITE_COILS)
        DEBUG("try MB_FC_WRITE_COILS\n");
        message_master.data = regs_master1;
        message_master.func = MB_FC_WRITE_COILS;
        res = modbus_rtu_send_request(&master, &message_master);
        if (res) {
            DEBUG("fail MB_FC_WRITE_COILS %d\n", res);
        }
        else {
#if defined(MODBUS_RTU_USE_READ_COILS)
            xtimer_usleep(master.rx_timeout * 3);
            DEBUG("try MB_FC_READ_COILS\n");
            message_master.data = regs_master2;
            message_master.func = MB_FC_READ_COILS;
            res = modbus_rtu_send_request(&master, &message_master);
            if (res ||
                memcmp(regs_master1, regs_master2,
                       message_master.count / 8) != 0) {
                DEBUG("out: %x in: %x\n", regs_master1[0], regs_master2[0]);
                DEBUG("fail MB_FC_READ_COILS %d\n", res);
            }
            else {}
#endif      /* MODBUS_RTU_USE_READ_COILS */
        }
#endif      /* MODBUS_RTU_USE_WRITE_COILS */

#if defined(MODBUS_RTU_USE_WRITE_COIL)
        DEBUG("try MB_FC_WRITE_COIL\n");
        message_master.data = regs_master1;
        regs_master1[0] = 1;
        message_master.func = MB_FC_WRITE_COIL;
        res = modbus_rtu_send_request(&master, &message_master);
        if (res) {
            DEBUG("fail MB_FC_WRITE_COIL %d\n", res);
        }
        else {
#if defined(MODBUS_RTU_USE_READ_COILS)
            xtimer_usleep(master.rx_timeout * 3);
            DEBUG("try MB_FC_READ_COILS\n");
            message_master.data = regs_master2;
            message_master.func = MB_FC_READ_COILS;
            res = modbus_rtu_send_request(&master, &message_master);
            if (res || (!!regs_master1[0]) != (!!regs_master2[0] & 1)) {
                DEBUG("fail MB_FC_READ_COILS_2 %d\n", res);
            }
            else {}
#endif      /* MODBUS_RTU_USE_READ_COILS */
        }
#endif      /* MODBUS_RTU_USE_WRITE_COIL */

#if defined(MODBUS_RTU_USE_WRITE_REGISTER)
        DEBUG("try MB_FC_WRITE_REGISTER\n");
        message_master.data = regs_master1;
        message_master.func = MB_FC_WRITE_REGISTER;
        res = modbus_rtu_send_request(&master, &message_master);
        if (res) {
            DEBUG("fail MB_FC_WRITE_REGISTER %d\n", res);
        }
        else {
#if defined(MODBUS_RTU_USE_READ_REGISTERS)
            xtimer_usleep(master.rx_timeout * 3);
            DEBUG("try MB_FC_READ_INPUT_REGISTER\n");
            message_master.data = regs_master2;
            message_master.func = MB_FC_READ_REGISTERS;
            res = modbus_rtu_send_request(&master, &message_master);
            if (res || regs_master1[0] != regs_master2[0]) {
                DEBUG("fail MB_FC_READ_REGISTERS_2 %d\n", res);
            }
            else {}
#endif      /* MODBUS_RTU_USE_READ_REGISTERS */
        }
#endif      /* MODBUS_RTU_USE_WRITE_REGISTER */
    }
    return NULL;
}
#endif /* USE_MASTER */


#ifdef USE_SLAVE
static void init_slave(void)
{
    slave.uart = UART_DEV(SLAVE_UART);
    slave.id = SLAVE_ID;
    slave.pin_rts = GPIO_UNDEF;
    /* slave.pin_rts = GPIO_PIN(PORT_B, 15); */
    slave.pin_rts_enable = 1;

    if (modbus_rtu_init(&slave, BAUDRATE)) {
        DEBUG("fail UART init\n");
    }
}

static void *thread_slave(void *arg __attribute__((unused)))
{
    int res __attribute__((unused));

    init_slave();
    while (1) {
        res = modbus_rtu_poll(&slave, &message_slave);
        /* DEBUG("func: %u; addr: %u; count: %u\n", message_slave.func, message_slave.addr, message_slave.count); */
        if (res) {
            DEBUG("fail poll %d\n", res);
        }
        else {
            /* assert(message_slave.id == SLAVE_ID); */
            /* assert(message_slave.func == message_master.func); */
            /* assert(message_slave.addr == message_master.addr); */
            /* assert(message_slave.count == message_master.count); */
            /* assert(message_slave.data == regs_slave); */
            res = modbus_rtu_send_response(&slave, &message_slave);
            if (res) {
                DEBUG("fail response %d\n", res);
            }
            else {}
        }
    }

    return NULL;
}
#endif /* USE_SLAVE */

int main(void)
{
#ifdef USE_SLAVE
    pid_slave = thread_create(stack_slave, sizeof(stack_slave),
                              THREAD_PRIORITY_MAIN - 1, 0, thread_slave, NULL,
                              NULL);
#endif /* USE_SLAVE */

#ifdef USE_MASTER
    pid_master = thread_create(stack_master, sizeof(stack_master),
                               THREAD_PRIORITY_MAIN - 1, 0, thread_master, NULL,
                               NULL);
#endif /* USE_MASTER */
    return 0;
}
