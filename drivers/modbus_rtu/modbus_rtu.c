/*
  todo: disble pin and set state hi/low tx pin on transfer
  todo: use RTS if exist 
  todo: timeout beetwen byte
  todo: overflow buffer
  todo: add code error: 
    prepare_request,
    send, poll,
    modbus_rtu_send_request
  todo: add timer between request
  fixme: then slave copy message in buffer and on uart receive byte
  todo: add crc16

*/

#include "modbus_rtu.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xtimer.h"
#include "thread.h"
#include "periph/gpio.h"
#include "msg.h"
#include "checksum/ucrc16.h"

enum MESSAGE {
  ID = 0,  //!< ID field
  FUNC,    //!< Function code position
  ADD_HI,  //!< Address high byte
  ADD_LO,  //!< Address low byte
  NB_HI,   //!< Number of coils or registers high byte
  NB_LO,   //!< Number of coils or registers low byte
  BYTE_CNT //!< byte counter
};

// bytes with crc16
#define MIN_SIZE_REQUEST (8)

#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define calcCRC(buffer, size) (ucrc16_calc_le(buffer, size, 0xA001, 0xFFFF))

static inline void enamdle_trasmit(modbus_rtu_t *modbus);
static inline void disamdle_trasmit(modbus_rtu_t *modbus);
static inline void send(modbus_rtu_t *modbus);
static inline int prepare_request(modbus_rtu_t *modbus);
static inline int get_response(modbus_rtu_t *modbus);
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

  modbus->_size_buffer = 0;
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
  modbus->_size_buffer = 0;

  return get_response(modbus);
}

static inline int prepare_request(modbus_rtu_t *modbus) {
  modbus->_buffer[ID] = modbus->_msg->id;
  modbus->_buffer[FUNC] = modbus->_msg->func;
  modbus->_buffer[ADD_HI] = modbus->_msg->addr;
  modbus->_buffer[ADD_LO] = modbus->_msg->addr;

  switch (modbus->_msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    modbus->_buffer[NB_HI] = highByte(modbus->_msg->count);
    modbus->_buffer[NB_LO] = lowByte(modbus->_msg->count);
    modbus->_size_buffer = 6;
    break;
  default:
    return -1;
    break;
  }

  return 0;
}

static inline int get_response(modbus_rtu_t *modbus) {
  msg_t msg;
  if (xtimer_msg_receive_timeout(&msg, modbus->timeout) < 0) {
    return -1; // timeout; no has answer
  } else {
    // start aswer
    if (modbus->_buffer[ID] != modbus->_msg->id) {
      return -2; // invalid id
    }

    while (modbus->_size_buffer < MIN_SIZE_REQUEST) {
      if (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) < 0) {
        return -3;
      }
    }

    if (modbus->_buffer[FUNC] != modbus->_msg->func) {
      return -4;
    }
    switch (modbus->_buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
      return -5;
      break;
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:; // some magick
      // id + func + size + data[size] + crc16
      uint8_t size = 3 + modbus->_buffer[2] + 2;
      while (modbus->_size_buffer < size) {
        if (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) < 0) {
          return -6;
        }
      }
      if (calcCRC(modbus->_buffer, modbus->_size_buffer) != 0) {
        return -7;
      }
      memcpy(modbus->_msg->regs, modbus->_buffer + 3, modbus->_buffer[2]);
      modbus->_msg->count = modbus->_buffer[2] >> 1;
      break;
    default:
      return -8;
      break;
    }

    modbus->_size_buffer = 0;
    return 0;
  }
}

int modbus_rtu_poll(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->_pid = thread_getpid();
  modbus->_msg = message;
  msg_t msg;

  while (1) {
    msg_receive(&msg);
    // printf("slave id: %u\n", modbus->_buffer[ID]);
    if (modbus->_buffer[ID] != modbus->id) {
      // wait the end of transfer for other slave
      while (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) >= 0) {
      }
      modbus->_size_buffer = 0;
      continue;
    }
    modbus->_msg->id = modbus->_buffer[ID];

    //  6 is minimal size of request
    while (modbus->_size_buffer < MIN_SIZE_REQUEST) {
      if (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) < 0) {
        goto error;
      }
    }
    modbus->_msg->func = modbus->_buffer[FUNC];
    switch (modbus->_buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
      goto error;
      break;
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      if (calcCRC(modbus->_buffer, 8) != 0) {
        goto error;
      }
      modbus->_msg->addr = ((modbus->_buffer[ADD_HI]) << 8) | modbus->_buffer[ADD_LO];
      modbus->_msg->count = ((modbus->_buffer[NB_HI]) << 8) | modbus->_buffer[NB_LO];
      break;
    default:
      goto error;
      break;
    }

    goto OK;
  }
error:
  modbus->_size_buffer = 0;
  modbus->_msg = NULL;
  return -1;
OK:
  modbus->_size_buffer = 0;
  modbus->_msg = NULL;
  return 0;
}

int modbus_rtu_send_response(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->_msg = message;
  modbus->_buffer[ID] = modbus->_msg->id;
  modbus->_buffer[FUNC] = modbus->_msg->func;
  switch (modbus->_msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
    return -1;
    break;
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    modbus->_buffer[2] = modbus->_msg->count * 2; // calc size for send
    modbus->_size_buffer = 3 + modbus->_buffer[2];
    memcpy(modbus->_buffer + 3, modbus->_msg->regs, modbus->_size_buffer);
    // printf("buff size: %u \n", modbus->_size_buffer);
    break;
  default:
    return -1;
    break;
  }
  send(modbus);
  modbus->_size_buffer = 0;
  return 0;
}

static inline void send(modbus_rtu_t *modbus) {
  uint16_t crc = calcCRC(modbus->_buffer, modbus->_size_buffer);
  modbus->_buffer[modbus->_size_buffer] = lowByte(crc);
  modbus->_size_buffer++;
  modbus->_buffer[modbus->_size_buffer] = highByte(crc);
  modbus->_size_buffer++;
  enamdle_trasmit(modbus);
  uart_write(modbus->uart, modbus->_buffer, modbus->_size_buffer);
  disamdle_trasmit(modbus);
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
  // printf("%x_", data);

  modbus->_buffer[modbus->_size_buffer] = data;
  modbus->_size_buffer++;

  msg_send(&msg, modbus->_pid);
}
