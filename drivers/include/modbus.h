/*
 * Copyright (C) 2020-2023 Denis Litvinov <li.denis.iv@gmail.com>
 * Copyright (C) 2023-2024 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_modbus Modbus
 * @ingroup     drivers
 * @brief       Modbus
 *
 * Low-level Modbus interface definitions.
 *
 * @{
 *
 * @file
 * @brief       Modbus interface definitions
 *
 * @author      Denis Litvinov <li.denis.iv@gmail.com>
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 */

#ifndef MODBUS_H
#define MODBUS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Modbus return codes
 */
enum {
    MODBUS_OK,              /**< no error */
    MODBUS_DROP,            /**< drop message */
    MODBUS_ERR_CRC,         /**< message CRC error */
    MODBUS_ERR_MESSAGE,     /**< message not valid */
    MODBUS_ERR_REQUEST,     /**< request message error */
    MODBUS_ERR_RESPONSE,    /**< response message error */
    MODBUS_ERR_TIMEOUT,     /**< timeout while waiting for message */
};

/**
 * @brief   Modbus functions
 */
enum {
    MODBUS_FC_NONE                              = 0,    /**< no function */
    MODBUS_FC_READ_COILS                        = 1,    /**< read coils */
    MODBUS_FC_READ_DISCRETE_INPUTS              = 2,    /**< read discrete inputs */
    MODBUS_FC_READ_HOLDING_REGISTERS            = 3,    /**< read holding registers */
    MODBUS_FC_READ_INPUT_REGISTERS              = 4,    /**< read inputs registers */
    MODBUS_FC_WRITE_SINGLE_COIL                 = 5,    /**< write single coil  */
    MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER     = 6,    /**< write single register */
    MODBUS_FC_WRITE_MULTIPLE_COILS              = 15,   /**< write multiple coils */
    MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS  = 16    /**< write multiple registers */
};

/**
 * @brief Modbus exception codes
 */
enum {
    MODBUS_EXC_NONE                 = 0,    /**< no exception */
    MODBUS_EXC_ILLEGAL_FUNCTION     = 1,    /**< illegal function */
    MODBUS_EXC_ILLEGAL_ADDRESS      = 2,    /**< illegal data address */
    MODBUS_EXC_ILLEGAL_VALUE        = 3,    /**< illegal data value */
    MODBUS_EXC_SERVER_FAILURE       = 4,    /**< server failure */
    MODBUS_EXC_ACKNOWLEDGE          = 5,    /**< acknowledge */
    MODBUS_EXC_SERVER_BUSY          = 6,    /**< server busy */
    MODBUS_EXC_NEGATIVE_ACKNOWLEDGE = 7,    /**< negative acknowledge */
    MODBUS_EXC_MEMORY_PARITY_ERROR  = 8,    /**< memory parity error */
};

/**
 * @brief   Modbus address limits
 */
#define MODBUS_ID_BROADCAST 0               /**< broadcast identifier */
#define MODBUS_ID_MIN       1               /**< first slave identifier */
#define MODBUS_ID_MAX       247             /**< last slave identifier */

/**
 * @brief Size in bytes of a single register
 */
#define MODBUS_REGISTER_SIZE 2

/**
 * @brief   Modbus message structure for requests and responses
 */
typedef struct {
    uint8_t id;             /**< address of slave (not used in responses) */

    uint8_t func;           /**< function code */
    uint16_t addr;          /**< starting address */
    uint16_t count;         /**< number of register or bits  */

    uint8_t exc;            /**< exception code (used in responses) */

    uint8_t *data;          /**< pointer to data (registers or bits) */
    uint8_t data_size;      /**< size of data in bytes */
} modbus_message_t;

/**
 * @brief   Callback function for handling requests
 *
 * This callback is invoked when a request message is received. The callback
 * must check if the request message is addressed properly and decide if the
 * request can be handled.
 *
 * Read requests must ensure that @p message->data points to the requested
 * data, so that the implementation can return it in the response. To support
 * zero-copy, the callback can also write directly to @p message->data
 * (at most @p message->data_size bytes), but is also valid to point this
 * pointer to a different application buffer of sufficient size. In that case,
 * @p message->data_size must be set updated as well.
 *
 * Write requests can copy data from @p message->data to application buffers,
 * taking into account @p message->data_size.
 *
 * Note that @p message->data is a raw buffer. Copy registers or bits using
 * the appropriate helpers in @ref drivers_modbus.
 *
 * If the request message SHOULD NOT be handled or if the request message is
 * broadcasted, the callback must return @p MODBUS_DROP. This will ensure that
 * no response message is sent back, but will return @p MODBUS_OK to the
 * caller.
 *
 * If the request message CANNOT be handled, @p message->exc must be set and
 * @p MODBUS_OK must be returned. For example, when the function code is not
 * supported (see @p MODBUS_EXC_ILLEGAL_FUNCTION), the address range is out
 * of bounds (see @p MODBUS_EXC_ILLEGAL_ADDRESS), or the data value is invalid
 * (see @p MODBUS_EXC_ILLEGAL_VALUE). See @p modbus_check_message
 *
 * @p MODBUS_OK must be returned on success. Any other error code will be
 * returned to the caller.
 *
 * @param[in] message       pointer to the request message
 * @param[in] arg           optional context for the callback
 *
 * @return                  MODBUS_OK on success
 * @return                  MODBUS_DROP if message should be dropped
 * @return                  other error code on failure
 */
typedef int (*modbus_request_cb_t)(modbus_message_t *message, void *arg);

/**
 * @brief Compute the size in bytes of a @p count bits
 *
 * @param[in] count         number of bits

 * @return                  size in bytes to represent @p count bits
 */
uint16_t modbus_bit_count_to_size(uint16_t count);

/**
 * @brief Compute the size in bytes of a @p count registers
 *
 * @param[in] count         number of registers

 * @return                  size in bytes to represent @p count registers
 */
uint16_t modbus_reg_count_to_size(uint16_t count);

/**
 * @brief   Copy @p count of bits from @p src to @p dst
 *
 * When @p pad_zeroes is @c true, all bits in the byte range addressed by
 * @p start_bit_dst and @p start_bit_dst + @p count will be set to zero. This
 * can be useful when writing response messages to buffers that are not zeroed
 * out.
 *
 * @param[out] dst          pointer to destination
 * @param[in] start_bit_dst offset of the number of bits in @p dst
 * @param[in] src           pointer to source
 * @param[in] start_bit_src offset of the the number bits in @p src
 * @param[in] count         number of bits to copy
 * @param[in] pad_zeroes    whether to pad @p dst with zeroes
 */
void modbus_copy_bits(uint8_t *dst, uint16_t start_bit_dst,
                      const uint8_t *src, uint16_t start_bit_src,
                      uint16_t count, bool pad_zeroes);

/**
 * @brief   Copy a single bit from @p src to @p dst
 *
 * When @p pad_zeroes is @c true, all bits in the byte addressed by
 * @p start_bit_dst will be set to zero. This can be useful when writing
 * response messages to buffers that are not zeroed out.
 *
 * @param[out] dst          pointer to destination
 * @param[in] start_bit_dst offset of the number of bits in @p dst
 * @param[in] src           pointer to source
 * @param[in] start_bit_src offset of the the number bits in @p src
 * @param[in] pad_zeroes    whether to pad @p dst with zeroes
 */
void modbus_copy_bit(uint8_t *dst, uint16_t start_bit_dst,
                     const uint8_t *src, uint16_t start_bit_src,
                     bool pad_zeroes);

/**
 * @brief   Copy @p count of registers from @p src to @p dst
 *
 * @param[out] dst          pointer to destination
 * @param[in] src           pointer to source
 * @param[in] count         number of registers to copy
 */
void modbus_copy_regs(void *dst, const void *src, uint16_t count);

/**
 * @brief   Copy a single register from @p src to @p dst
 *
 * @param[out] dst          pointer to destination
 * @param[in] src           pointer to source
 */
void modbus_copy_reg(void *dst, const void *src);

/**
 * @brief   Validate a message
 *
 * Check if the message properties are within valid ranges. This ensures that
 * the message is valid for further processing.
 *
 * @param[in] message       the message to validate
 *
 * @return                  MODBUS_OK if @p message is valid
 * @return                  MODBUS_ERR_MESSAGE if @p message is not valid
 */
int modbus_check_message(const modbus_message_t *message);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_H */
/** @} */
