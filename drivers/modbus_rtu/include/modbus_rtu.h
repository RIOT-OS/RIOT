#pragma once

/**
 * @defgroup    drivers_modbus ModBus
 * @ingroup     drivers
 * @brief       Modbus
 *
 * @{
 * @file
 * @brief       Modbus interface definitions
 *
 * @author      Denis Litvinov <li.denis.iv@gmail.com>
 */

#include "periph/uart.h"
#include "thread.h"
#include "mutex.h"

/**
 * @brief Modbus functions
 */
enum MB_FC {
  MB_FC_NONE = 0,                  /**< null operator */
  MB_FC_READ_COILS = 1,            /**< FCT=1 -> read coils or digital outputs */
  MB_FC_READ_DISCRETE_INPUT = 2,   /**< FCT=2 -> read digital inputs */
  MB_FC_READ_REGISTERS = 3,        /**< FCT=3 -> read registers or analog outputs */
  MB_FC_READ_INPUT_REGISTER = 4,   /**< FCT=4 -> read analog inputs */
  MB_FC_WRITE_COIL = 5,            /**< FCT=5 -> write single coil or output */
  MB_FC_WRITE_REGISTER = 6,        /**< FCT=6 -> write single register */
  MB_FC_WRITE_MULTIPLE_COILS = 15, /**< FCT=15 -> write multiple coils or outputs */
  MB_FC_WRITE_REGISTERS = 16       /**< FCT=16 -> write multiple registers */
};

//! Максимальный размер пакета протокола Modbus RTU.
#define MODBUS_RTU_PACKET_SIZE_MAX 256

//! Размер контрольной суммы протокола Modbus RTU.
#define MODBUS_RTU_CRC_SIZE 2

//! Размер полей сообщения протокола Modbus RTU.
#define MODBUS_RTU_FIELDS_SIZE (sizeof(modbus_rtu_id_t) + sizeof(modbus_rtu_func_t))
//! Размер полей и контрольной суммы сообщения протокола Modbus RTU.
#define MODBUS_RTU_FIELDS_CRC_SIZE (MODBUS_RTU_CRC_SIZE + MODBUS_RTU_FIELDS_SIZE)
//! Максимальный размер данных протокола Modbus RTU.
#define MODBUS_RTU_DATA_SIZE_MAX (MODBUS_RTU_PACKET_SIZE_MAX - MODBUS_RTU_FIELDS_CRC_SIZE)
//! Максимальный размер данных и контрольной суммы протокола Modbus RTU.
#define MODBUS_RTU_DATA_CRC_SIZE_MAX (MODBUS_RTU_DATA_SIZE_MAX + MODBUS_RTU_CRC_SIZE)

/**
 * @brief Modbus message structure.
 */
typedef struct {
  uint8_t id;     /**< ID of slave, on response not use */
  uint8_t func;   /**< function code */
  uint16_t addr;  /**< Starting Address */
  uint16_t count; /**< Quantity of Registers */
  uint16_t *regs; /**< Registers */
} modbus_rtu_message_t;

/**
 * @brief Modbus structure.
 */
typedef struct {
  /**
   * @brief  UART port.
   */
  uart_t uart;
  /**
   * @brief  baudrate.
   */
  uint32_t baudrate;
  /**
   * @brief  RTS pin. 0 - disable.
   */
  gpio_t pin_rts;
  /**
   * @brief  RTS signals when transmit.
   */
  int pin_rts_enable;
  /**
   * @brief  ID device. 0 - master, otherwice slave.
   */
  uint8_t id;
  /**
   * @brief   Before slave begin to response; usec.
   */
  uint32_t timeout;
  /**
   * @brief   Template buffer for receive message or send. **Must never be changed
   *          by the user.**
   * @internal
   */
  uint8_t _buffer[MODBUS_RTU_PACKET_SIZE_MAX];
  /**
   * @brief   Buffer size. **Must never be changed by the user.**
   * @internal
   */
  uint8_t _size_buffer;
  /**
   * @brief   Mutex for block buffer. **Must never be changed by the user.**
   * @internal
   */
  mutex_t _mutex_buffer;
  /**
   * @brief   Timeout beetwin dyte. **Must never be changed by the user.**
   * @internal
   */
  uint32_t _rx_timeout;
  /**
   * @brief   pointer on current message. **Must never be changed by the user.**
   * @internal
   */
  modbus_rtu_message_t *_msg;
  /**
   * @brief   PID thread. **Must never be changed by the user.**
   * @internal
   */
  kernel_pid_t _pid;
} modbus_rtu_t;

/**
 * @brief   Initialize the given modbus.
 *
 * Initialize fields of modbus and UART, RTS pin.
 *
 * @param[in] modbus    pointer modbus 
 *
 * @return              0 on success
 * @return              less 0 on error
 */
int modbus_rtu_init(modbus_rtu_t *modbus);
/**
 * @brief   Send request to slave.
 *
 * Send request to slave, blocking and wait response.
 *
 * @param[in] modbus    pointer modbus 
 * @param[in] message    pointer modbus message
 *
 * @return              0 on success
 * @return              less 0 on error
 */
int modbus_rtu_send_request(modbus_rtu_t *modbus, modbus_rtu_message_t *message);
/**
 * @brief   Wait request from master.
 *
 * Wait request from master, blocking.
 * Request will be writeт to message.
 * Use only slave.
 *
 * @param[in] modbus    pointer modbus 
 * @param[in] message   pointer modbus message
 *
 * @return              0 on success
 * @return              less 0 on error
 */
int modbus_rtu_poll(modbus_rtu_t *modbus, modbus_rtu_message_t *message);
/**
 * @brief   Send response to master.
 *
 * Response must be writeт to message.
 * Use only slave.
 * id, func, addr, count - must be set correctly, or not change if it been use in modbus_rtu_send_request
 *
 * @param[in] modbus    pointer modbus 
 * @param[in] message   pointer modbus message
 *
 * @return              0 on success
 * @return              less 0 on error
 */
int modbus_rtu_send_response(modbus_rtu_t *modbus, modbus_rtu_message_t *message);

/** @} */