/*
 * Copyright (C) 2020 Denis Litvinov <li.denis.iv@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_modbus
 * @brief       Modbus
 *
 * @{
 * @file
 * @brief       Modbus interface implementation
 *
 * @author      Denis Litvinov <li.denis.iv@gmail.com>
 */
/*
  todo: send exeption
*/

#include "modbus_rtu.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "byteorder.h"
#include "xtimer.h"
#include "thread.h"
#include "periph/gpio.h"
#include "msg.h"
#include "checksum/ucrc16.h"
#include "mutex.h"

enum {
  ID = 0,  /* ID field */
  FUNC,    /* Function code position */
  ADD_HI,  /* Address high byte */
  ADD_LO,  /* Address low byte */
  NB_HI,   /* Number of coils or registers high byte */
  NB_LO,   /* Number of coils or registers low byte */
  BYTE_CNT /* byte counter */
};

/* bytes with crc16 */
#define MIN_SIZE_REQUEST (8)

/* use for set CRC */
#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define calcCRC(buffer, size) (ucrc16_calc_le(buffer, size, 0xA001, 0xFFFF))
/* return zero if check success */
#define checkCRC(buffer, size) (ucrc16_calc_le(buffer, size, 0xA001, 0xFFFF) != 0)

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
/* from buffer */
static inline void read_address(modbus_rtu_t *modbus) {
  modbus->msg->addr = byteorder_bebuftohs(modbus->buffer + ADD_HI);
}
/* to buffer */
static inline void write_address(modbus_rtu_t *modbus) {
  byteorder_htobebufs(modbus->buffer + ADD_HI, modbus->msg->addr);
}
/* from buffer */
static inline void read_count(modbus_rtu_t *modbus) {
  modbus->msg->count = byteorder_bebuftohs(modbus->buffer + NB_HI);
}
/* to buffer */
static inline void write_count(modbus_rtu_t *modbus) {
  byteorder_htobebufs(modbus->buffer + NB_HI, modbus->msg->count);
}

static void copy_bits(uint8_t *dst, uint16_t start_bit_dst,
                      const uint8_t *src, uint16_t start_bit_src, uint16_t count) {
  while (count > 0) {
    div_t s = div(start_bit_src, 8);
    div_t d = div(start_bit_dst, 8);
    uint8_t bit = src[s.quot] & (1 << s.rem);
    if (bit) {
      dst[d.quot] |= 1 << (d.rem);
    } else {
      dst[d.quot] &= ~(1 << (d.rem));
    }
    start_bit_dst++;
    start_bit_src++;
    count--;
  }
}

static void copy_buffer_to_registers(uint16_t *reg, const uint8_t *buff, uint8_t count) {
  while (count) {
    *reg = byteorder_bebuftohs(buff);
    count--;
    reg++;
    buff += sizeof(*reg);
  }
}

static void copy_registers_to_buffer(uint8_t *buff, const uint16_t *reg, uint8_t count) {
  while (count) {
    byteorder_htobebufs(buff, *reg);
    count--;
    reg++;
    buff += sizeof(*reg);
  }
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
  /* usec in sec / byte per second * 1.5 */
  modbus->rx_timeout = US_PER_SEC / (baudrate / 10) * 1.5;

  modbus->size_buffer = 0;
  modbus->msg = NULL;

  return 0;
}

int modbus_rtu_send_request(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->msg = message;
  modbus->pid = thread_getpid();
  mutex_lock(&(modbus->mutex_buffer));
  int res = prepare_request(modbus);
  if (res != MB_ER_NONE) {
    mutex_unlock(&(modbus->mutex_buffer));
    return res;
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
  div_t d;
  switch (modbus->msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    write_count(modbus);
    /* id + func + addr + count */
    modbus->size_buffer = 6;
    break;
  case MB_FC_WRITE_COIL:
    d = div(modbus->msg->addr, 16);
    modbus->buffer[4] = ((modbus->msg->data[d.quot] >> d.rem) & 1u) ? 0xff : 0x00;
    modbus->buffer[5] = 0;
    /* id + func + addr + data */
    modbus->size_buffer = 6;
    break;
  case MB_FC_WRITE_REGISTER:
    copy_registers_to_buffer(modbus->buffer + 4, modbus->msg->data + modbus->msg->addr, 1);
    /* id + func + addr + data */
    modbus->size_buffer = 6;
    break;
  case MB_FC_WRITE_COILS:
    write_count(modbus);
    d = div(modbus->msg->count, 8);
    size = d.quot;
    if (d.rem) {
      size += 1;
    }
    modbus->buffer[BYTE_CNT] = size;
    /* (id + func + addr + count + size) + data */
    modbus->size_buffer = 7 + size;
    memset(modbus->buffer + 3, 0, size);
    copy_bits(modbus->buffer + 7, 0, (uint8_t *)modbus->msg->data, modbus->msg->addr, modbus->msg->count);
    break;
  case MB_FC_WRITE_REGISTERS:
    write_count(modbus);
    size = modbus->msg->count * 2;
    modbus->buffer[BYTE_CNT] = size;
    /* (id + func + addr + count + size) + data */
    modbus->size_buffer = 7 + size;
    copy_registers_to_buffer(modbus->buffer + 7, modbus->msg->data + modbus->msg->addr, modbus->msg->count);
    break;
  default:
    return MB_ER_ILLEGAL_FUNCTION;
    break;
  }

  return MB_ER_NONE;
}

static inline int get_response(modbus_rtu_t *modbus) {
  msg_t msg;
  if (xtimer_msg_receive_timeout(&msg, modbus->timeout) < 0) {
    return MB_ER_TIMEOUT; /* timeout; no has answer */
  } else {
    /* start aswer */
    if (modbus->buffer[ID] != modbus->msg->id) {
      return MB_ER_INVAL_ID;
    }

    if (wait_bytes(modbus, 5)) {
      return MB_ER_TIMEOUT;
    }

    if (modbus->buffer[FUNC] != modbus->msg->func) {
      /* if slave return error */
      if (((~0x80) & modbus->buffer[FUNC]) == modbus->msg->func) {
        mutex_lock(&(modbus->mutex_buffer));
        if (checkCRC(modbus->buffer, 5)) {
          mutex_unlock(&(modbus->mutex_buffer));
          return MB_ER_CRC;
        }
        mutex_unlock(&(modbus->mutex_buffer));
        return modbus->buffer[2];
      } else {
        return MB_ER_ILLEGAL_FUNCTION;
      }
    }
    uint8_t size;
    switch (modbus->buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
      /* id + func + size + data + crc16 */
      size = 3 + modbus->buffer[2] + 2;
      if (wait_bytes(modbus, size)) {
        return MB_ER_TIMEOUT;
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (checkCRC(modbus->buffer, size)) {
        mutex_unlock(&(modbus->mutex_buffer));
        return MB_ER_CRC;
      }
      copy_bits((uint8_t *)modbus->msg->data, modbus->msg->addr, modbus->buffer + 3, 0, modbus->msg->count);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      /* id + func + size + data + crc16 */
      size = 3 + modbus->buffer[2] + 2;
      if (wait_bytes(modbus, size)) {
        return MB_ER_TIMEOUT;
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (checkCRC(modbus->buffer, size)) {
        mutex_unlock(&(modbus->mutex_buffer));
        return MB_ER_CRC;
      }
      copy_buffer_to_registers(modbus->msg->data + modbus->msg->addr, modbus->buffer + 3, modbus->msg->count);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_WRITE_COIL:
    case MB_FC_WRITE_REGISTER:
    case MB_FC_WRITE_COILS:
    case MB_FC_WRITE_REGISTERS:
      if (wait_bytes(modbus, MIN_SIZE_REQUEST)) {
        return MB_ER_TIMEOUT;
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (checkCRC(modbus->buffer, modbus->size_buffer)) {
        mutex_unlock(&(modbus->mutex_buffer));
        return MB_ER_CRC;
      }
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    default:
      return MB_ER_ILLEGAL_FUNCTION;
      break;
    }

    modbus->size_buffer = 0;
    return MB_ER_NONE;
  }
}

int modbus_rtu_poll(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  modbus->pid = thread_getpid();
  modbus->msg = message;
  msg_t msg;
  int err = MB_ER_NONE;

  while (1) {
    msg_receive(&msg);
    if (modbus->buffer[ID] != modbus->id) {
      /* wait the end of transfer for other slave */
      while (xtimer_msg_receive_timeout(&msg, modbus->rx_timeout) >= 0) {
      }
      modbus->size_buffer = 0;
      continue;
    }
    modbus->msg->id = modbus->buffer[ID];

    if (wait_bytes(modbus, MIN_SIZE_REQUEST)) {
      err = MB_ER_TIMEOUT;
      goto exit;
    }
    modbus->msg->func = modbus->buffer[FUNC];
    uint8_t size;
    switch (modbus->buffer[FUNC]) {
    case MB_FC_READ_COILS:
    case MB_FC_READ_DISCRETE_INPUT:
    case MB_FC_READ_REGISTERS:
    case MB_FC_READ_INPUT_REGISTER:
      mutex_lock(&(modbus->mutex_buffer));
      if (checkCRC(modbus->buffer, 8)) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_CRC;
        goto exit;
      }
      read_address(modbus);
      read_count(modbus);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_WRITE_COIL:
      mutex_lock(&(modbus->mutex_buffer));
      if (checkCRC(modbus->buffer, 8)) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_CRC;
        goto exit;
      }
      read_address(modbus);
      if (modbus->msg->addr >= modbus->msg->data_size * 8) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_ILLEGAL_ADDRESS;
        goto exit;
      }
      copy_bits((uint8_t *)modbus->msg->data, modbus->msg->addr, modbus->buffer + 4, 0, 1);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_WRITE_REGISTER:
      mutex_lock(&(modbus->mutex_buffer));
      if (checkCRC(modbus->buffer, 8)) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_CRC;
        goto exit;
      }
      read_address(modbus);
      if (modbus->msg->addr * 2 >= modbus->msg->data_size) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_ILLEGAL_ADDRESS;
        goto exit;
      }
      copy_buffer_to_registers(modbus->msg->data + modbus->msg->addr, modbus->buffer + 4, 1);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_WRITE_COILS:
      /* (id + func + addr + count + size) + data + crc */
      size = 7 + modbus->buffer[BYTE_CNT] + 2;
      if (wait_bytes(modbus, size)) {
        err = MB_ER_TIMEOUT;
        goto exit;
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (checkCRC(modbus->buffer, size)) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_CRC;
        goto exit;
      }
      read_address(modbus);
      read_count(modbus);
      if ((modbus->msg->addr + modbus->msg->count) > (modbus->msg->data_size * 8)) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_ILLEGAL_ADDRESS;
        goto exit;
      }
      copy_bits((uint8_t *)modbus->msg->data, modbus->msg->addr, modbus->buffer + 7, 0, modbus->msg->count);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    case MB_FC_WRITE_REGISTERS:
      /* (id + func + addr + count + size) + data + crc */
      size = 7 + modbus->buffer[BYTE_CNT] + 2;
      if (wait_bytes(modbus, size)) {
        err = MB_ER_TIMEOUT;
        goto exit;
      }
      mutex_lock(&(modbus->mutex_buffer));
      if (checkCRC(modbus->buffer, size)) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_CRC;
        goto exit;
      }
      read_address(modbus);
      read_count(modbus);
      if (modbus->buffer[BYTE_CNT] != modbus->msg->count * 2) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_ILLEGAL_VALUE;
        goto exit;
      }
      if ((modbus->msg->addr * 2 + modbus->buffer[BYTE_CNT]) > modbus->msg->data_size) {
        mutex_unlock(&(modbus->mutex_buffer));
        err = MB_ER_ILLEGAL_ADDRESS;
        goto exit;
      }
      copy_buffer_to_registers(modbus->msg->data + modbus->msg->addr, modbus->buffer + 7, modbus->msg->count);
      mutex_unlock(&(modbus->mutex_buffer));
      break;
    default:
      err = MB_ER_ILLEGAL_FUNCTION;
      goto exit;
      break;
    }

    goto exit;
  }
exit:
  modbus->size_buffer = 0;
  modbus->msg = NULL;
  return err;
}

int modbus_rtu_send_response(modbus_rtu_t *modbus, modbus_rtu_message_t *message) {
  uint8_t size;
  div_t d;
  modbus->msg = message;
  mutex_lock(&(modbus->mutex_buffer));
  modbus->buffer[ID] = modbus->msg->id;
  modbus->buffer[FUNC] = modbus->msg->func;
  switch (modbus->msg->func) {
  case MB_FC_READ_COILS:
  case MB_FC_READ_DISCRETE_INPUT:
    d = div(modbus->msg->count, 8);
    size = d.quot;
    if (d.rem) {
      size += 1;
    }
    if (size > modbus->msg->data_size) {
      mutex_unlock(&(modbus->mutex_buffer));
      return MB_ER_ILLEGAL_ADDRESS;
    }
    modbus->buffer[2] = size;
    modbus->size_buffer = 3 + size;
    memset(modbus->buffer + 3, 0, size);
    copy_bits(modbus->buffer + 3, 0, (uint8_t *)modbus->msg->data, modbus->msg->addr, modbus->msg->count);
    break;
  case MB_FC_WRITE_COIL:
  case MB_FC_WRITE_REGISTER:
  case MB_FC_WRITE_COILS:
  case MB_FC_WRITE_REGISTERS:
    /* don't change any data in buffer, send this back as is 
     * size_buffer change for calculate CRC 
     * todo: calculate CRC for MB_FC_WRITE_COIL no need, need reduce this 
     * id + func + addr + val */
    modbus->size_buffer = 6;
    break;
  case MB_FC_READ_REGISTERS:
  case MB_FC_READ_INPUT_REGISTER:
    size = modbus->msg->count * 2;
    if (size > modbus->msg->data_size) {
      mutex_unlock(&(modbus->mutex_buffer));
      return MB_ER_ILLEGAL_ADDRESS;
    }
    modbus->buffer[2] = size; /* calc size for send */
    modbus->size_buffer = 3 + size;
    copy_registers_to_buffer(modbus->buffer + 3, modbus->msg->data + modbus->msg->addr, modbus->msg->count);
    break;
  default:
    mutex_unlock(&(modbus->mutex_buffer));
    return MB_ER_ILLEGAL_FUNCTION;
    break;
  }
  send(modbus);
  modbus->size_buffer = 0;
  mutex_unlock(&(modbus->mutex_buffer));
  return MB_ER_NONE;
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

/** @} */
