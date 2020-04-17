/*
  todo: set state hi/low tx pin on transfer
  todo: use RTS if exist 
  todo: timeout beetwen byte
  todo: overflow buffer
  todo: add code error: 
    prepare_request,
    send, poll,
    modbus_rtu_send_request
  todo: add timer between end prev
*/

#include "modbus_rtu.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xtimer.h"
#include "thread.h"
#include "periph/gpio.h"
#include "msg.h"

enum MESSAGE {
  ID = 0,  //!< ID field
  FUNC,    //!< Function code position
  ADD_HI,  //!< Address high byte
  ADD_LO,  //!< Address low byte
  NB_HI,   //!< Number of coils or registers high byte
  NB_LO,   //!< Number of coils or registers low byte
  BYTE_CNT //!< byte counter
};

static void enamdle_trasmit(modbus_rtu_t *modbus);
static void disamdle_trasmit(modbus_rtu_t *modbus);
static void send(modbus_rtu_t *modbus);
static int prepare_request(modbus_rtu_t *modbus);
static void rx_cb_master(void *arg, uint8_t data);
static void rx_cb_slave(void *arg, uint8_t data);

int modbus_rtu_init(modbus_rtu_t *modbus) {
  int err;
  if (modbus->id == 0) {
    err = uart_init(modbus->uart, modbus->baudrate, rx_cb_master, modbus);
    // uint8_t ms[] = "mas";
    // uart_write(modbus->uart, ms, 3);
  } else {
    err = uart_init(modbus->uart, modbus->baudrate, rx_cb_slave, modbus);
    // uint8_t ms[] = "sla";
    // uart_write(modbus->uart, ms, 3);
  }
  if (err != UART_OK) {
    return err;
  }
  if (modbus->pin_tx_enable) {
    gpio_init(modbus->pin_tx_enable, GPIO_OUT);
  }
  // usec in sec / byte per second * 1.5
  modbus->_rx_timeout = 1000000 / (modbus->baudrate / 10) * 1.5;

  modbus->_size_buffer = 5; // todo: set size to 0
  modbus->_msg = NULL;

  puts("modbus_init");

  return 0;
}

int modbus_rtu_send_request(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->_msg = message;
  modbus->_pid = thread_getpid();
  if (prepare_request(modbus) != 0) {
    return -1;
  }
  send(modbus);

  msg_t msg;
  if (xtimer_msg_receive_timeout(&msg, modbus->timeout) < 0) {
    return -1; // timeout; no has answer
  } else {
    // receive aswer
    while (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) >= 0) {
    }
    return 0;
  }
  return 0;
}

static int prepare_request(modbus_rtu_t *modbus) {
  (void)(modbus);
  return 0;
}

static void send(modbus_rtu_t *modbus) {
  enamdle_trasmit(modbus);
  uart_write(modbus->uart, modbus->_buffer, modbus->_size_buffer);
  disamdle_trasmit(modbus);
}

int poll(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->_pid = thread_getpid();
  modbus->_msg = message;
  msg_t msg;

  while (1) {
    msg_receive(&msg);
    printf("slave id: %u\n", modbus->_buffer[ID]);
    if (modbus->_buffer[ID] != modbus->id) {
      // wait the end of transfer for other slave
      while (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) >= 0) {
      }

      modbus->_size_buffer = 0;
      continue;
    }

    while (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) >= 0) {
    }
    return 0;
  }
  return -1;
}

static inline void enamdle_trasmit(modbus_rtu_t *modbus) {
  // gpio_write
  gpio_set(modbus->pin_tx_enable);
}

static inline void disamdle_trasmit(modbus_rtu_t *modbus) {
  gpio_clear(modbus->pin_tx_enable);
}

static void rx_cb_master(void *arg, uint8_t data) {
  static msg_t msg;
  modbus_rtu_t *modbus = (modbus_rtu_t *)arg;
  // printf("rx_cb: %x\n", data);

  if (modbus->_msg == NULL) {
    return;
  }

  modbus->_buffer[modbus->_size_buffer] = data;
  modbus->_size_buffer++;

  msg_send(&msg, modbus->_pid);
}

static void rx_cb_slave(void *arg, uint8_t data) {
  static msg_t msg;
  modbus_rtu_t *modbus = (modbus_rtu_t *)arg;
  // printf("%x ", data);

  modbus->_buffer[modbus->_size_buffer] = data;
  modbus->_size_buffer++;

  msg_send(&msg, modbus->_pid);
}
