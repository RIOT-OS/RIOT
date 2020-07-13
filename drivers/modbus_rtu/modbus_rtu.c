/*
  todo: overflow buffer
  todo: add code error:
    prepare_request,
    send, poll,
    modbus_rtu_send_request
  todo: add timer between request
  todo: add crc16
  todo: send exeption
  todo: handle response send and request get collision

  todo: implement:
      MB_FC_READ_DISCRETE_INPUT
      MB_FC_READ_INPUT_REGISTER
      MB_FC_WRITE_COIL
      MB_FC_WRITE_REGISTER

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
#include "mutex.h"

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
#define MIN_SIZE_REQUEST (6)

#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define calcCRC(buffer, size) (ucrc16_calc_le(buffer, size, 0xA001, 0xFFFF))

static inline void enamdle_trasmit(modbus_rtu_t *modbus);
static inline void disamdle_trasmit(modbus_rtu_t *modbus);
static inline void send(modbus_rtu_t *modbus);
static inline int prepare_request(modbus_rtu_t *modbus);
static inline int get_response(modbus_rtu_t *modbus);
static void rx_cb(void *arg, uint8_t data);

int modbus_rtu_init(modbus_rtu_t *modbus) {
  int err = uart_init(modbus->uart, modbus->baudrate, rx_cb, modbus);
  if (err != UART_OK) {
    return err;
  }
  if (modbus->pin_rts) {
    gpio_init(modbus->pin_rts, GPIO_OUT);
    gpio_write(modbus->pin_rts, !modbus->pin_rts_enable);
  }
  mutex_init(&(modbus->_mutex_buffer));
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
  mutex_lock(&(modbus->_mutex_buffer));
  if (prepare_request(modbus) != 0) {
    mutex_unlock(&(modbus->_mutex_buffer));
    return -1;
  }
  send(modbus);
  modbus->_size_buffer = 0;
  mutex_unlock(&(modbus->_mutex_buffer));

  return get_response(modbus);
}

static inline int prepare_request(modbus_rtu_t *modbus) {
  modbus->_buffer[ID] = modbus->_msg->id;
  modbus->_buffer[FUNC] = modbus->_msg->func;
  modbus->_buffer[ADD_HI] = lowByte(modbus->_msg->addr);
  modbus->_buffer[ADD_LO] = lowByte(modbus->_msg->addr);
  uint8_t size;
  switch (modbus->_msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    modbus->_buffer[NB_HI] = highByte(modbus->_msg->count);
    modbus->_buffer[NB_LO] = lowByte(modbus->_msg->count);
    modbus->_size_buffer = 6;
    break;
  // ...
  case MB_FC_WRITE_COILS:
    modbus->_buffer[NB_HI] = highByte(modbus->_msg->count);
    modbus->_buffer[NB_LO] = lowByte(modbus->_msg->count);
    size = modbus->_msg->count / 8 + 1;
    modbus->_buffer[BYTE_CNT] = size;
    modbus->_size_buffer = 7;
    memcpy(modbus->_buffer + modbus->_size_buffer, modbus->_msg->regs, size);
    modbus->_size_buffer += size;
    break;
  case MB_FC_WRITE_REGISTERS:
    modbus->_buffer[NB_HI] = highByte(modbus->_msg->count);
    modbus->_buffer[NB_LO] = lowByte(modbus->_msg->count);
    size = modbus->_msg->count * 2;
    modbus->_buffer[BYTE_CNT] = size;
    modbus->_size_buffer = 7;
    memcpy(modbus->_buffer + modbus->_size_buffer, modbus->_msg->regs, size);
    modbus->_size_buffer += size;
    break;
  default:
    return -15;
    break;
  }

  return 0;
}

static inline int get_response(modbus_rtu_t *modbus) {
  msg_t msg;
  if (xtimer_msg_receive_timeout(&msg, modbus->timeout) < 0) {
    return -99; // timeout; no has answer
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
    uint8_t size;
    switch (modbus->_buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
      // id + func + size + size of regs + crc16
      size = 3 + modbus->_buffer[2] + 2;
      mutex_lock(&(modbus->_mutex_buffer));
      if (calcCRC(modbus->_buffer, size) != 0) {
        mutex_unlock(&(modbus->_mutex_buffer));
        return -7;
      }
      memcpy(modbus->_msg->regs, modbus->_buffer + 3, modbus->_buffer[2]);
      mutex_unlock(&(modbus->_mutex_buffer));
      break;
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      // id + func + size + size of regs + crc16
      size = 3 + modbus->_buffer[2] + 2;
      while (modbus->_size_buffer < size) {
        if (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) < 0) {
          return -6;
        }
      }
      mutex_lock(&(modbus->_mutex_buffer));
      if (calcCRC(modbus->_buffer, size) != 0) {
        mutex_unlock(&(modbus->_mutex_buffer));
        return -7;
      }
      memcpy(modbus->_msg->regs, modbus->_buffer + 3, modbus->_buffer[2]);
      mutex_unlock(&(modbus->_mutex_buffer));
      break;
    case MB_FC_WRITE_COIL:
    case MB_FC_WRITE_REGISTER:
    case MB_FC_WRITE_COILS:
    case MB_FC_WRITE_REGISTERS:
      // todo: add check function
      if (calcCRC(modbus->_buffer, modbus->_size_buffer) != 0) {
        return -7;
      }
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

    // wait minimal size of request
    while (modbus->_size_buffer < MIN_SIZE_REQUEST) {
      if (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) < 0) {
        goto error;
      }
    }
    modbus->_msg->func = modbus->_buffer[FUNC];
    uint8_t size;
    switch (modbus->_buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      while (modbus->_size_buffer < 8) {
        if (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) < 0) {
          goto error;
        }
      }
      if (calcCRC(modbus->_buffer, 8) != 0) {
        goto error;
      }
      modbus->_msg->addr = ((modbus->_buffer[ADD_HI]) << 8) | modbus->_buffer[ADD_LO];
      modbus->_msg->count = ((modbus->_buffer[NB_HI]) << 8) | modbus->_buffer[NB_LO];
      break;
    case MB_FC_WRITE_COILS:
    case MB_FC_WRITE_REGISTERS:
      // (id + func + addr + count + size) + size of regs + crc
      size = 7 + modbus->_buffer[BYTE_CNT] + 2;
      // thread_stack_print();
      while (modbus->_size_buffer < size) {
        if (xtimer_msg_receive_timeout(&msg, modbus->_rx_timeout) < 0) {
          goto error;
        }
      }
      mutex_lock(&(modbus->_mutex_buffer));
      if (calcCRC(modbus->_buffer, size) != 0) {
        mutex_unlock(&(modbus->_mutex_buffer));
        goto error;
      }
      modbus->_msg->addr = ((modbus->_buffer[ADD_HI]) << 8) | modbus->_buffer[ADD_LO];
      modbus->_msg->count = ((modbus->_buffer[NB_HI]) << 8) | modbus->_buffer[NB_LO];
      memcpy(modbus->_msg->regs, modbus->_buffer + 7, modbus->_buffer[BYTE_CNT]);
      mutex_unlock(&(modbus->_mutex_buffer));
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
  mutex_lock(&(modbus->_mutex_buffer));
  modbus->_buffer[ID] = modbus->_msg->id;
  modbus->_buffer[FUNC] = modbus->_msg->func;
  switch (modbus->_msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
    modbus->_buffer[2] = modbus->_msg->count / 8 + 1; // calc size for send
    modbus->_size_buffer = 3 + modbus->_buffer[2];
    memcpy(modbus->_buffer + 3, modbus->_msg->regs, modbus->_size_buffer);
    break;
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    modbus->_buffer[2] = modbus->_msg->count * 2; // calc size for send
    modbus->_size_buffer = 3 + modbus->_buffer[2];
    memcpy(modbus->_buffer + 3, modbus->_msg->regs, modbus->_size_buffer);
    break;
  case MB_FC_WRITE_COILS:
  case MB_FC_WRITE_REGISTERS:
    modbus->_buffer[ADD_HI] = highByte(modbus->_msg->addr);
    modbus->_buffer[ADD_LO] = lowByte(modbus->_msg->addr);
    modbus->_buffer[NB_HI] = highByte(modbus->_msg->count);
    modbus->_buffer[NB_LO] = lowByte(modbus->_msg->count);
    modbus->_size_buffer = 6;
    break;
  default:
    mutex_unlock(&(modbus->_mutex_buffer));
    return -1;
    break;
  }
  send(modbus);
  modbus->_size_buffer = 0;
  mutex_unlock(&(modbus->_mutex_buffer));
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
  if (modbus->pin_rts) {
    gpio_write(modbus->pin_rts, modbus->pin_rts_enable);
  }
}

static inline void disamdle_trasmit(modbus_rtu_t *modbus) {
  if (modbus->pin_rts) {
    gpio_write(modbus->pin_rts, !modbus->pin_rts_enable);
  }
}

static void rx_cb(void *arg, uint8_t data) {
  static msg_t msg;
  modbus_rtu_t *modbus = (modbus_rtu_t *)arg;
  // printf("rx_cb: %x\n", data);

  if (mutex_trylock(&(modbus->_mutex_buffer)) == 0) {
    goto exit;
  }

  if (modbus->_msg == NULL) {
    goto exit;
  }

  modbus->_buffer[modbus->_size_buffer] = data;
  modbus->_size_buffer++;

  msg_send(&msg, modbus->_pid);

exit:
  mutex_unlock(&(modbus->_mutex_buffer));
  return;
}
