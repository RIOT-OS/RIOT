#include <stdio.h>
#include "modbus_rtu.h"
#include "periph/gpio.h"
#include "thread.h"
#include "xtimer.h"
#include "msg.h"
#include <string.h>

#define SLAVE_ID 1
#define BAUDRATE 115200

static char stack_master[THREAD_STACKSIZE_DEFAULT];
static kernel_pid_t pid_master;
static modbus_rtu_t master;
static uint16_t regs_master[10];
static modbus_rtu_message_t message_master = {
    .id = SLAVE_ID,
    .func = MB_FC_READ_REGISTERS,
    .addr = 0,
    .regs = regs_master,
    .count = 10};

static void init_master(void) {
  master.uart = UART_DEV(2);
  master.baudrate = BAUDRATE;
  master.timeout = 1000000;
  master.id = 0;
  master.pin_tx_enable = GPIO_PIN(PORT_A, 1);

  if (modbus_rtu_init(&master)) {
    puts("fail UART init");
  }
}

static void *thread_master(void *arg __attribute__((unused))) {
  int res __attribute__((unused));
  init_master();

  while (1) {
    xtimer_usleep(master._rx_timeout * 3);
    puts("try request");
    res = modbus_rtu_send_request(&master, &message_master);
    if (res) {
      puts("fail request");
    } else {
      puts("ok request");
    }
  }
  return NULL;
}

static char stack_slave[THREAD_STACKSIZE_DEFAULT];
static kernel_pid_t pid_slave;
static modbus_rtu_t slave;
static uint16_t regs_slave[10];
static modbus_rtu_message_t message_slave = {
    .regs = regs_slave};

static void init_slave(void) {
  slave.uart = UART_DEV(0);
  slave.baudrate = BAUDRATE;
  slave.id = SLAVE_ID;
  slave.pin_tx_enable = GPIO_PIN(PORT_B, 15);

  if (modbus_rtu_init(&slave)) {
    puts("fail UART init");
  }
}

static void *thread_slave(void *arg __attribute__((unused))) {
  int res __attribute__((unused));
  init_slave();
  while (1) {
    puts("try poll");
    res = modbus_rtu_poll(&slave, &message_slave);
    if (res) {
      puts("fail poll");
    } else {
      assert(message_slave.id == SLAVE_ID);
      assert(message_slave.func == message_master.func);
      assert(message_slave.addr == message_master.addr);
      assert(message_slave.count == message_master.count);
      assert(message_slave.regs == regs_slave);
      modbus_rtu_send_response(&slave, &message_slave);
      puts("ok poll");
    }
  }

  return NULL;
}

int main(void) {
  // printf("%u", thread_getpid());

  pid_slave = thread_create(stack_slave, sizeof(stack_slave),
                            THREAD_PRIORITY_MAIN - 1, 0, thread_slave, NULL, NULL);
  pid_master = thread_create(stack_master, sizeof(stack_master),
                             THREAD_PRIORITY_MAIN - 1, 0, thread_master, NULL, NULL);

  return 0;
}