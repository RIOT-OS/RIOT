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

static char stack_master[400];
static kernel_pid_t pid_master;
static modbus_rtu_t master;
static uint16_t regs_master1[10];
static uint16_t regs_master2[10];
static modbus_rtu_message_t message_master = {
    .id = SLAVE_ID,
    .addr = 0,
    .data = regs_master1,
    .count = 10};

static char stack_slave[400];
static kernel_pid_t pid_slave;
static modbus_rtu_t slave;
static uint16_t regs_slave[10];
static modbus_rtu_message_t message_slave = {
    .data = regs_slave};

static void init_master(void) {
  master.uart = UART_DEV(2);
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
    // assert(message_slave.count == message_master.count);
    if (res) {
      printf("fail MB_FC_WRITE_REGISTERS %d\n", res);
    } else {
      // puts("ok request");

      xtimer_usleep(master.rx_timeout * 3);
      puts("try MB_FC_READ_REGISTERS");
      message_master.data = regs_master2;
      message_master.func = MB_FC_READ_REGISTERS;
      res = modbus_rtu_send_request(&master, &message_master);
      // assert(message_slave.count == message_master.count);
      if (res || memcmp(regs_master1, regs_master2, message_master.count * 2) != 0) {
        printf("fail MB_FC_READ_REGISTERS %d\n", res);
      } else {
        // puts("ok request");
      }
    }

    puts("try MB_FC_WRITE_COILS");
    message_master.data = regs_master1;
    message_master.func = MB_FC_WRITE_COILS;
    res = modbus_rtu_send_request(&master, &message_master);
    // assert(message_slave.count == message_master.count);
    if (res) {
      printf("fail MB_FC_WRITE_COILS %d\n", res);
    } else {
      // puts("ok request");

      xtimer_usleep(master.rx_timeout * 3);
      puts("try MB_FC_READ_COILS");
      message_master.data = regs_master2;
      message_master.func = MB_FC_READ_COILS;
      res = modbus_rtu_send_request(&master, &message_master);
      // assert(message_slave.count == message_master.count);
      if (res /*|| memcmp(regs_master1, regs_master2, message_master.count / 8 + 1) != 0*/) {
        printf("fail MB_FC_READ_COILS %d\n", res);
      } else {
        // puts("ok request");
      }
    }

    puts("try MB_FC_WRITE_COIL");
    message_master.data = regs_master1;
    regs_master1[0] = 1;
    message_master.func = MB_FC_WRITE_COIL;
    res = modbus_rtu_send_request(&master, &message_master);
    // assert(message_slave.count == message_master.count);
    if (res) {
      printf("fail MB_FC_WRITE_COIL %d\n", res);
    } else {
      xtimer_usleep(master.rx_timeout * 3);
      puts("try MB_FC_READ_COILS");
      message_master.data = regs_master2;
      message_master.func = MB_FC_READ_COILS;
      res = modbus_rtu_send_request(&master, &message_master);
      // assert(message_slave.count == message_master.count);
      if (res || (!!regs_master1[0]) != (!!regs_master2[0] & 1)) {
        printf("fail MB_FC_READ_COILS 2 %d\n", res);
      } else {
        // puts("ok request");
      }
    }
  }
  return NULL;
}

static void init_slave(void) {
  slave.uart = UART_DEV(0);
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
    puts("try poll");
    res = modbus_rtu_poll(&slave, &message_slave);
    if (res) {
      puts("fail poll");
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
        puts("ok poll");
      }
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