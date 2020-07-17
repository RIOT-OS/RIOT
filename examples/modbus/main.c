#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "modbus_rtu.h"
#include "periph/gpio.h"
#include "thread.h"
#include "xtimer.h"
#include "msg.h"

#define SLAVE_ID 1
#define BAUDRATE 115200

#define SLAVE_UART 2
#define MASTER_UART 0

static char stack_master[400];
static kernel_pid_t pid_master;
static modbus_rtu_t master;
static uint16_t regs_master1[16];
static uint16_t regs_master2[16];
static modbus_rtu_message_t message_master = {
    .id = SLAVE_ID,
    .addr = 0,
    .data = regs_master1,
    .data_size = sizeof(regs_master1),
    .count = 16};

static char stack_slave[400];
static kernel_pid_t pid_slave;
static modbus_rtu_t slave;
static uint16_t regs_slave[16];
static modbus_rtu_message_t message_slave = {
    .data = regs_slave,
    .data_size = sizeof(regs_slave)};

static void init_master(void) {
  master.uart = UART_DEV(MASTER_UART);
  master.timeout = 1000000;
  master.id = 0;
  master.pin_rts = GPIO_PIN(PORT_A, 1);
  master.pin_rts_enable = 0;

  if (modbus_rtu_init(&master, BAUDRATE)) {
    puts("fail UART init");
  }
}

static void *thread_master(void *arg __attribute__((unused))) {
  int res __attribute__((unused));
  init_master();
  // return NULL;
  while (1) {
    xtimer_usleep(master.rx_timeout * 3);
    srand(xtimer_now_usec());
    for (uint8_t i = 0; i < message_master.count; i++) {
      regs_master1[i] = rand();
    }

    puts("try MB_FC_WRITE_REGISTERS");
    message_master.data = regs_master1;
    message_master.func = MB_FC_WRITE_REGISTERS;
    res = modbus_rtu_send_request(&master, &message_master);
    if (res) {
      printf("fail MB_FC_WRITE_REGISTERS %d\n", res);
    } else {
      xtimer_usleep(master.rx_timeout * 3);
      puts("try MB_FC_READ_REGISTERS");
      message_master.data = regs_master2;
      message_master.func = MB_FC_READ_REGISTERS;
      res = modbus_rtu_send_request(&master, &message_master);
      if (res || memcmp(regs_master1, regs_master2, message_master.count * 2) != 0) {
        printf("fail MB_FC_READ_REGISTERS %d\n", res);
      } else {
      }
    }

    puts("try MB_FC_READ_DISCRETE_INPUT");
    message_master.data = regs_master1;
    message_master.func = MB_FC_READ_DISCRETE_INPUT;
    res = modbus_rtu_send_request(&master, &message_master);
    if (res || memcmp(regs_master1, regs_slave, sizeof(regs_master1)) != 0) {
      printf("fail MB_FC_READ_DISCRETE_INPUT %d\n", res);
    } else {
    }

    puts("try MB_FC_READ_INPUT_REGISTER");
    message_master.data = regs_master1;
    message_master.func = MB_FC_READ_INPUT_REGISTER;
    res = modbus_rtu_send_request(&master, &message_master);
    if (res || message_master.data[0] != regs_slave[0]) {
      printf("fail MB_FC_READ_INPUT_REGISTER %d\n", res);
    } else {
    }

    puts("try MB_FC_WRITE_COILS");
    message_master.data = regs_master1;
    message_master.func = MB_FC_WRITE_COILS;
    res = modbus_rtu_send_request(&master, &message_master);
    if (res) {
      printf("fail MB_FC_WRITE_COILS %d\n", res);
    } else {
      xtimer_usleep(master.rx_timeout * 3);
      puts("try MB_FC_READ_COILS");
      message_master.data = regs_master2;
      message_master.func = MB_FC_READ_COILS;
      res = modbus_rtu_send_request(&master, &message_master);
      if (res || memcmp(regs_master1, regs_master2, message_master.count / 8) != 0) {
        printf("out: %x in: %x\n", regs_master1[0], regs_master2[0]);
        printf("fail MB_FC_READ_COILS %d\n", res);
      } else {
      }
    }

    puts("try MB_FC_WRITE_COIL");
    message_master.data = regs_master1;
    regs_master1[0] = 1;
    message_master.func = MB_FC_WRITE_COIL;
    res = modbus_rtu_send_request(&master, &message_master);
    if (res) {
      printf("fail MB_FC_WRITE_COIL %d\n", res);
    } else {
      xtimer_usleep(master.rx_timeout * 3);
      puts("try MB_FC_READ_COILS");
      message_master.data = regs_master2;
      message_master.func = MB_FC_READ_COILS;
      res = modbus_rtu_send_request(&master, &message_master);
      if (res || (!!regs_master1[0]) != (!!regs_master2[0] & 1)) {
        printf("fail MB_FC_READ_COILS_2 %d\n", res);
      } else {
      }
    }

    puts("try MB_FC_WRITE_REGISTER");
    message_master.data = regs_master1;
    message_master.func = MB_FC_WRITE_REGISTER;
    res = modbus_rtu_send_request(&master, &message_master);
    if (res) {
      printf("fail MB_FC_WRITE_REGISTER %d\n", res);
    } else {
      xtimer_usleep(master.rx_timeout * 3);
      puts("try MB_FC_READ_INPUT_REGISTER");
      message_master.data = regs_master2;
      message_master.func = MB_FC_READ_REGISTERS;
      res = modbus_rtu_send_request(&master, &message_master);
      if (res || regs_master1[0] != regs_master2[0]) {
        printf("fail MB_FC_READ_REGISTERS_2 %d\n", res);
      } else {
      }
    }
  }
  return NULL;
}

static void init_slave(void) {
  slave.uart = UART_DEV(SLAVE_UART);
  slave.id = SLAVE_ID;
  slave.pin_rts = GPIO_PIN(PORT_B, 15);
  slave.pin_rts_enable = 1;

  if (modbus_rtu_init(&slave, BAUDRATE)) {
    puts("fail UART init");
  }
}

static void *thread_slave(void *arg __attribute__((unused))) {
  int res __attribute__((unused));
  init_slave();
  while (1) {
    res = modbus_rtu_poll(&slave, &message_slave);
    // printf("func: %u; addr: %u; count: %u\n", message_slave.func, message_slave.addr, message_slave.count);
    if (res) {
      printf("fail poll %d\n", res);
    } else {
      // assert(message_slave.id == SLAVE_ID);
      // assert(message_slave.func == message_master.func);
      // assert(message_slave.addr == message_master.addr);
      // assert(message_slave.count == message_master.count);
      // assert(message_slave.data == regs_slave);
      res = modbus_rtu_send_response(&slave, &message_slave);
      if (res) {
        printf("fail response %d\n", res);
      } else {
      }
    }
  }

  return NULL;
}

int main(void) {
  pid_slave = thread_create(stack_slave, sizeof(stack_slave),
                            THREAD_PRIORITY_MAIN - 1, 0, thread_slave, NULL, NULL);
  pid_master = thread_create(stack_master, sizeof(stack_master),
                             THREAD_PRIORITY_MAIN - 1, 0, thread_master, NULL, NULL);

  return 0;
}