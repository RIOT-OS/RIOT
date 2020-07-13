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
  if (modbus->pin_rts != GPIO_UNDEF) {
    gpio_init(modbus->pin_rts, GPIO_OUT);
    gpio_write(modbus->pin_rts, !modbus->pin_rts_enable);
  }
  mutex_init(&(modbus->mutex_buffer));
  // usec in sec / byte per second * 1.5
  modbus->rx_timeout = 1000000 / (modbus->baudrate / 10) * 1.5;

  modbus->size_buffer = 0;
  modbus->msg = NULL;

  puts("modbus_init");

  return 0;
}

int modbus_rtu_send_request(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->msg = message;
  modbus->pid = thread_getpid();
  mutex_lock(&(modbus->mutex_buffer));
  if (prepare_request(modbus) != 0) {
    mutex_unlock(&(modbus->mutex_buffer));
    return -1;
  }
  send(modbus);
  modbus->size_buffer = 0;
  mutex_unlock(&(modbus->mutex_buffer));

  return get_response(modbus);
}

static inline int prepare_request(modbus_rtu_t *modbus) {
  modbus->buffer[ID] = modbus->msg->id;
  modbus->buffer[FUNC] = modbus->msg->func;
  modbus->buffer[ADD_HI] = lowByte(modbus->msg->addr);
  modbus->buffer[ADD_LO] = lowByte(modbus->msg->addr);
  uint8_t size;
  switch (modbus->msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    modbus->buffer[NB_HI] = highByte(modbus->msg->count);
    modbus->buffer[NB_LO] = lowByte(modbus->msg->count);
    modbus->size_buffer = 6;
    break;
  // ...
  case MB_FC_WRITE_COILS:
    modbus->buffer[NB_HI] = highByte(modbus->msg->count);
    modbus->buffer[NB_LO] = lowByte(modbus->msg->count);
    size = modbus->msg->count / 8 + 1;
    modbus->buffer[BYTE_CNT] = size;
    modbus->size_buffer = 7;
    memcpy(modbus->buffer + modbus->size_buffer, modbus->msg->data, size);
    modbus->size_buffer += size;
    break;
  case MB_FC_WRITE_REGISTERS:
    modbus->buffer[NB_HI] = highByte(modbus->msg->count);
    modbus->buffer[NB_LO] = lowByte(modbus->msg->count);
    size = modbus->msg->count * 2;
    modbus->buffer[BYTE_CNT] = size;
    modbus->size_buffer = 7;
    memcpy(modbus->buffer + modbus->size_buffer, modbus->msg->data, size);
    modbus->size_buffer += size;
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
    if (modbus->buffer[ID] != modbus->msg->id) {
      return -2; // invalid id
    }

    while (modbus->size_buffer < MIN_SIZE_REQUEST) {
      if (xtimer_msg_receive_timeout(&msg, modbus->rx_timeout) < 0) {
        return -3;
      }
    }

    if (modbus->buffer[FUNC] != modbus->msg->func) {
      return -4;
    }
    uint8_t size;
    switch (modbus->buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
      // id + func + size + size of regs + crc16
      size = 3 + modbus->buffer[2] + 2;
      mutex_lock(&(modbus->mutex_buffer));
      if (calcCRC(modbus->buffer, size) != 0) {
        mutex_unlock(&(modbus->mutex_buffer));
        return -7;
      }
      memcpy(modbus->msg->data, modbus->buffer + 3, modbus->buffer[2]);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      // id + func + size + size of regs + crc16
      size = 3 + modbus->buffer[2] + 2;
      while (modbus->size_buffer < size) {
        if (xtimer_msg_receive_timeout(&msg, modbus->rx_timeout) < 0) {
          return -6;
        }
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (calcCRC(modbus->buffer, size) != 0) {
        mutex_unlock(&(modbus->mutex_buffer));
        return -7;
      }
      memcpy(modbus->msg->data, modbus->buffer + 3, modbus->buffer[2]);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_WRITE_COIL:
    case MB_FC_WRITE_REGISTER:
    case MB_FC_WRITE_COILS:
    case MB_FC_WRITE_REGISTERS:
      // todo: add check function
      if (calcCRC(modbus->buffer, modbus->size_buffer) != 0) {
        return -7;
      }
      break;
    default:
      return -8;
      break;
    }

    modbus->size_buffer = 0;
    return 0;
  }
}

int modbus_rtu_poll(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->pid = thread_getpid();
  modbus->msg = message;
  msg_t msg;

  while (1) {
    msg_receive(&msg);
    // printf("slave id: %u\n", modbus->buffer[ID]);
    if (modbus->buffer[ID] != modbus->id) {
      // wait the end of transfer for other slave
      while (xtimer_msg_receive_timeout(&msg, modbus->rx_timeout) >= 0) {
      }
      modbus->size_buffer = 0;
      continue;
    }
    modbus->msg->id = modbus->buffer[ID];

    // wait minimal size of request
    while (modbus->size_buffer < MIN_SIZE_REQUEST) {
      if (xtimer_msg_receive_timeout(&msg, modbus->rx_timeout) < 0) {
        goto error;
      }
    }
    modbus->msg->func = modbus->buffer[FUNC];
    uint8_t size;
    switch (modbus->buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      while (modbus->size_buffer < 8) {
        if (xtimer_msg_receive_timeout(&msg, modbus->rx_timeout) < 0) {
          goto error;
        }
      }
      if (calcCRC(modbus->buffer, 8) != 0) {
        goto error;
      }
      modbus->msg->addr = ((modbus->buffer[ADD_HI]) << 8) | modbus->buffer[ADD_LO];
      modbus->msg->count = ((modbus->buffer[NB_HI]) << 8) | modbus->buffer[NB_LO];
      break;
    case MB_FC_WRITE_COILS:
    case MB_FC_WRITE_REGISTERS:
      // (id + func + addr + count + size) + size of regs + crc
      size = 7 + modbus->buffer[BYTE_CNT] + 2;
      // thread_stack_print();
      while (modbus->size_buffer < size) {
        if (xtimer_msg_receive_timeout(&msg, modbus->rx_timeout) < 0) {
          goto error;
        }
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (calcCRC(modbus->buffer, size) != 0) {
        mutex_unlock(&(modbus->mutex_buffer));
        goto error;
      }
      modbus->msg->addr = ((modbus->buffer[ADD_HI]) << 8) | modbus->buffer[ADD_LO];
      modbus->msg->count = ((modbus->buffer[NB_HI]) << 8) | modbus->buffer[NB_LO];
      memcpy(modbus->msg->data, modbus->buffer + 7, modbus->buffer[BYTE_CNT]);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    default:
      goto error;
      break;
    }

    goto OK;
  }
error:
  modbus->size_buffer = 0;
  modbus->msg = NULL;
  return -1;
OK:
  modbus->size_buffer = 0;
  modbus->msg = NULL;
  return 0;
}

int modbus_rtu_send_response(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->msg = message;
  mutex_lock(&(modbus->mutex_buffer));
  modbus->buffer[ID] = modbus->msg->id;
  modbus->buffer[FUNC] = modbus->msg->func;
  switch (modbus->msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
    modbus->buffer[2] = modbus->msg->count / 8 + 1; // calc size for send
    modbus->size_buffer = 3 + modbus->buffer[2];
    memcpy(modbus->buffer + 3, modbus->msg->data, modbus->size_buffer);
    break;
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    modbus->buffer[2] = modbus->msg->count * 2; // calc size for send
    modbus->size_buffer = 3 + modbus->buffer[2];
    memcpy(modbus->buffer + 3, modbus->msg->data, modbus->size_buffer);
    break;
  case MB_FC_WRITE_COILS:
  case MB_FC_WRITE_REGISTERS:
    modbus->buffer[ADD_HI] = highByte(modbus->msg->addr);
    modbus->buffer[ADD_LO] = lowByte(modbus->msg->addr);
    modbus->buffer[NB_HI] = highByte(modbus->msg->count);
    modbus->buffer[NB_LO] = lowByte(modbus->msg->count);
    modbus->size_buffer = 6;
    break;
  default:
    mutex_unlock(&(modbus->mutex_buffer));
    return -1;
    break;
  }
  send(modbus);
  modbus->size_buffer = 0;
  mutex_unlock(&(modbus->mutex_buffer));
  return 0;
}

static inline void send(modbus_rtu_t *modbus) {
  uint16_t crc = calcCRC(modbus->buffer, modbus->size_buffer);
  modbus->buffer[modbus->size_buffer] = lowByte(crc);
  modbus->size_buffer++;
  modbus->buffer[modbus->size_buffer] = highByte(crc);
  modbus->size_buffer++;
  enamdle_trasmit(modbus);
  uart_write(modbus->uart, modbus->buffer, modbus->size_buffer);
  disamdle_trasmit(modbus);
}

static inline void enamdle_trasmit(modbus_rtu_t *modbus) {
  if (modbus->pin_rts != GPIO_UNDEF) {
    gpio_write(modbus->pin_rts, modbus->pin_rts_enable);
  }
}

static inline void disamdle_trasmit(modbus_rtu_t *modbus) {
  if (modbus->pin_rts != GPIO_UNDEF) {
    gpio_write(modbus->pin_rts, !modbus->pin_rts_enable);
  }
}

static void rx_cb(void *arg, uint8_t data) {
  static msg_t msg;
  modbus_rtu_t *modbus = (modbus_rtu_t *)arg;
  // printf("rx_cb: %x\n", data);

  if (mutex_trylock(&(modbus->mutex_buffer)) == 0) {
    goto exit;
  }

  if (modbus->msg == NULL) {
    goto exit;
  }

  modbus->buffer[modbus->size_buffer] = data;
  modbus->size_buffer++;

  msg_send(&msg, modbus->pid);

exit:
  mutex_unlock(&(modbus->mutex_buffer));
  return;
}
