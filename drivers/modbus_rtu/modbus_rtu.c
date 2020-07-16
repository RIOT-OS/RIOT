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
#define MIN_SIZE_REQUEST (8)

#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define calcCRC(buffer, size) (ucrc16_calc_le(buffer, size, 0xA001, 0xFFFF))

static inline void enamdle_trasmit(modbus_rtu_t *modbus);
static inline void disamdle_trasmit(modbus_rtu_t *modbus);
static inline void send(modbus_rtu_t *modbus);
static inline int prepare_request(modbus_rtu_t *modbus);
static inline int get_response(modbus_rtu_t *modbus);
static void rx_cb(void *arg, uint8_t data);

static inline int wait_bytes(modbus_rtu_t *modbus, uint8_t num) {
  msg_t msg;
  while (modbus->size_buffer < num) {
    if (xtimer_msg_receive_timeout(&msg, modbus->rx_timeout) < 0) {
      return -1;
    }
  }
  return 0;
}
// from buffer
static inline void read_address(modbus_rtu_t *modbus) {
  modbus->msg->addr = ((modbus->buffer[ADD_HI]) << 8) | modbus->buffer[ADD_LO];
}
// to buffer
static inline void write_address(modbus_rtu_t *modbus) {
  modbus->buffer[ADD_HI] = highByte(modbus->msg->addr);
  modbus->buffer[ADD_LO] = lowByte(modbus->msg->addr);
}
// from buffer
static inline void read_count(modbus_rtu_t *modbus) {
  modbus->msg->count = ((modbus->buffer[NB_HI]) << 8) | modbus->buffer[NB_LO];
}
// to buffer
static inline void write_count(modbus_rtu_t *modbus) {
  modbus->buffer[NB_HI] = highByte(modbus->msg->count);
  modbus->buffer[NB_LO] = lowByte(modbus->msg->count);
}

int modbus_rtu_init(modbus_rtu_t *modbus, uint32_t baudrate) {
  int err = uart_init(modbus->uart, baudrate, rx_cb, modbus);
  if (err != UART_OK) {
    return err;
  }
  if (modbus->pin_rts != GPIO_UNDEF) {
    gpio_init(modbus->pin_rts, GPIO_OUT);
    gpio_write(modbus->pin_rts, !modbus->pin_rts_enable);
  }
  mutex_init(&(modbus->mutex_buffer));
  // usec in sec / byte per second * 1.5
  modbus->rx_timeout = 1000000 / (baudrate / 10) * 1.5;

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
    return -__LINE__;
  }
  send(modbus);
  modbus->size_buffer = 0;
  mutex_unlock(&(modbus->mutex_buffer));

  return get_response(modbus);
}

static inline int prepare_request(modbus_rtu_t *modbus) {
  modbus->buffer[ID] = modbus->msg->id;
  modbus->buffer[FUNC] = modbus->msg->func;
  write_address(modbus);
  uint8_t size;
  switch (modbus->msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    write_count(modbus);
    // id + func + addr + count
    modbus->size_buffer = 6;
    break;
  case MB_FC_WRITE_COIL:
    modbus->buffer[4] = (modbus->msg->data[0]) ? 0xff : 0x00;
    modbus->buffer[5] = 0;
    // id + func + addr + data
    modbus->size_buffer = 6;
    break;
  case MB_FC_WRITE_REGISTER:
    memcpy(modbus->buffer + 4, (char *)modbus->msg->data, 2);
    // id + func + addr + data
    modbus->size_buffer = 6;
    break;
  case MB_FC_WRITE_COILS:
    write_count(modbus);
    size = modbus->msg->count / 8 + 1;
    modbus->buffer[BYTE_CNT] = size;
    // (id + func + addr + count + size) + data
    modbus->size_buffer = 7 + size;
    memcpy(modbus->buffer + 7, modbus->msg->data, size);
    break;
  case MB_FC_WRITE_REGISTERS:
    write_count(modbus);
    size = modbus->msg->count * 2;
    modbus->buffer[BYTE_CNT] = size;
    // (id + func + addr + count + size) + data
    modbus->size_buffer = 7 + size;
    memcpy(modbus->buffer + 7, modbus->msg->data, size);
    break;
  default:
    return -__LINE__;
    break;
  }

  return 0;
}

static inline int get_response(modbus_rtu_t *modbus) {
  msg_t msg;
  if (xtimer_msg_receive_timeout(&msg, modbus->timeout) < 0) {
    return -__LINE__; // timeout; no has answer
  } else {
    // start aswer
    if (modbus->buffer[ID] != modbus->msg->id) {
      return -__LINE__; // invalid id
    }

    if (wait_bytes(modbus, 6)) {
      return -__LINE__;
    }

    if (modbus->buffer[FUNC] != modbus->msg->func) {
      return -__LINE__;
    }
    uint8_t size;
    switch (modbus->buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
      // id + func + size + size of regs + crc16
      size = 3 + modbus->buffer[2] + 2;
      if (wait_bytes(modbus, size)) {
        return -__LINE__;
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (calcCRC(modbus->buffer, size) != 0) {
        mutex_unlock(&(modbus->mutex_buffer));
        return -__LINE__;
      }
      memcpy(modbus->msg->data, modbus->buffer + 3, modbus->buffer[2]);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      // id + func + size + size of regs + crc16
      size = 3 + modbus->buffer[2] + 2;
      if (wait_bytes(modbus, size)) {
        return -__LINE__;
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (calcCRC(modbus->buffer, size) != 0) {
        mutex_unlock(&(modbus->mutex_buffer));
        return -__LINE__;
      }
      memcpy(modbus->msg->data, modbus->buffer + 3, modbus->buffer[2]);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_WRITE_COIL:
    case MB_FC_WRITE_REGISTER:
    case MB_FC_WRITE_COILS:
    case MB_FC_WRITE_REGISTERS:
      if (wait_bytes(modbus, MIN_SIZE_REQUEST)) {
        return -__LINE__;
      }
      // todo: add check function
      if (calcCRC(modbus->buffer, modbus->size_buffer) != 0) {
        return -__LINE__;
      }
      break;
    default:
      return -__LINE__;
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

    if (wait_bytes(modbus, MIN_SIZE_REQUEST)) {
      goto error;
    }
    modbus->msg->func = modbus->buffer[FUNC];
    uint8_t size;
    switch (modbus->buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      if (calcCRC(modbus->buffer, 8) != 0) {
        goto error;
      }
      read_address(modbus);
      read_count(modbus);
      break;
    case MB_FC_WRITE_COIL:
      read_address(modbus);
      modbus->msg->data[0] = modbus->buffer[4];
      // write_bit(modbus->msg->data, modbus->msg->addr, modbus->buffer[4]);
      break;
    case MB_FC_WRITE_REGISTER:
      read_address(modbus);
      memcpy((char *)modbus->msg->data, modbus->buffer + 4, 2);
      break;
    case MB_FC_WRITE_COILS:
    case MB_FC_WRITE_REGISTERS:
      // (id + func + addr + count + size) + data + crc
      size = 7 + modbus->buffer[BYTE_CNT] + 2;
      if (wait_bytes(modbus, size)) {
        goto error;
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (calcCRC(modbus->buffer, size) != 0) {
        mutex_unlock(&(modbus->mutex_buffer));
        goto error;
      }
      read_address(modbus);
      read_count(modbus);
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
  uint8_t size;
  modbus->msg = message;
  mutex_lock(&(modbus->mutex_buffer));
  modbus->buffer[ID] = modbus->msg->id;
  modbus->buffer[FUNC] = modbus->msg->func;
  switch (modbus->msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
    size = modbus->msg->count / 8 + 1;
    modbus->buffer[2] = size;
    modbus->size_buffer = 3 + size;
    memcpy(modbus->buffer + 3, modbus->msg->data, size);
    break;
  case MB_FC_WRITE_COIL:
  case MB_FC_WRITE_REGISTER:
  case MB_FC_WRITE_COILS:
  case MB_FC_WRITE_REGISTERS:
    // don't change any data in buffer, send this back as is
    // size_buffer change for calculate CRC
    // todo: calculate CRC for MB_FC_WRITE_COIL no need, need reduce this
    // id + func + addr + val
    modbus->size_buffer = 6;
    break;
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    size = modbus->msg->count * 2;
    modbus->buffer[2] = size; // calc size for send
    modbus->size_buffer = 3 + size;
    memcpy(modbus->buffer + 3, modbus->msg->data, size);
    break;
  default:
    mutex_unlock(&(modbus->mutex_buffer));
    return -__LINE__;
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
