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
 * @{
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
 * @brief   Modbus errors
 */
enum {
    MODBUS_OK,
    MODBUS_ERR_CRC,
    MODBUS_ERR_EXCEPTION,
    MODBUS_ERR_MESSAGE,
    MODBUS_ERR_REQUEST,
    MODBUS_ERR_RESPONSE,
    MODBUS_ERR_TIMEOUT,
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
 * @brief Size in bytes of a single register
 */
#define MODBUS_REGISTER_SIZE 2

/**
 * @brief   Generic structure for a Modbus device
 */
typedef struct {
    uint8_t id;             /**< ID of the device, 0 is reserved for master. */
} modbus_t;

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
 * @brief   Callback function for handling request
 *
 * @param[in] modbus    pointer to modbus device
 * @param[in] message   pointer to modbus message
 *
 * @return              @p MODBUS_OK on success, otherwise error
 */
typedef int (*modbus_request_cb_t)(modbus_t *modbus, modbus_message_t *message);

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
 * This ensures that the message is valid for further processing.
 *
 * @param[in] message       the message to validate
 *
 * @return int              MODBUS_OK if @p is valid, otherwise error
 */
int modbus_check_message(const modbus_message_t *message);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_H */
/** @} */
