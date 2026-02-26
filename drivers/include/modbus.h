/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_modbus Modbus
 * @ingroup     drivers
 * @brief       Modbus
 *
 * Low-level Modbus interface definitions for client and servers modes
 * (formerly known as master and slaves. This implementation focusses on the
 * Modbus protocol data unit (PDU). It provides both a client and server API,
 * as well as an interface for Modbus drivers (e.g., Modbus RTU).
 *
 * @{
 *
 * @file
 * @brief       Modbus interface definitions
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Modbus function codes
 */
typedef enum {
    MODBUS_FC_NONE                              = 0,    /**< no function */
    MODBUS_FC_READ_COILS                        = 1,    /**< read coils */
    MODBUS_FC_READ_DISCRETE_INPUTS              = 2,    /**< read discrete inputs */
    MODBUS_FC_READ_HOLDING_REGISTERS            = 3,    /**< read holding registers */
    MODBUS_FC_READ_INPUT_REGISTERS              = 4,    /**< read inputs registers */
    MODBUS_FC_WRITE_SINGLE_COIL                 = 5,    /**< write single coil  */
    MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER     = 6,    /**< write single register */
    MODBUS_FC_WRITE_MULTIPLE_COILS              = 15,   /**< write multiple coils */
    MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS  = 16    /**< write multiple registers */
} modbus_fc_t;

/**
 * @brief Modbus exception codes
 */
typedef enum {
    MODBUS_EXC_NONE                 = 0,    /**< no exception */
    MODBUS_EXC_ILLEGAL_FUNCTION     = 1,    /**< illegal function */
    MODBUS_EXC_ILLEGAL_ADDRESS      = 2,    /**< illegal data address */
    MODBUS_EXC_ILLEGAL_VALUE        = 3,    /**< illegal data value */
    MODBUS_EXC_SERVER_FAILURE       = 4,    /**< server failure */
    MODBUS_EXC_ACKNOWLEDGE          = 5,    /**< acknowledge */
    MODBUS_EXC_SERVER_BUSY          = 6,    /**< server busy */
    MODBUS_EXC_NEGATIVE_ACKNOWLEDGE = 7,    /**< negative acknowledge */
    MODBUS_EXC_MEMORY_PARITY_ERROR  = 8,    /**< memory parity error */
} modbus_exc_t;

/**
 * @brief   Modbus server flags
 */
typedef enum {
    MODBUS_SERVER_FLAG_NONE                 = 0x00, /**< no flags */
    MODBUS_SERVER_FLAG_RECEIVE_BROADCAST    = 0x01, /**< receive broadcast messages */
    MODBUS_SERVER_FLAG_RECEIVE_ANY          = 0x02  /**< receive any message */
} modbus_server_flags_t;

/**
 * @brief   Modbus receive flags.
 *
 * These flags will communicated to the receive function of the driver.
 */
typedef enum {
    MODBUS_RECV_FLAG_NONE       = 0x00,     /**< no flags */
    MODBUS_RECV_FLAG_REQUEST    = 0x01,     /**< a request is being received (will block) */
    MODBUS_RECV_FLAG_RESPONSE   = 0x02,     /**< a response is being received (will wait for
                                                 response timeout) */
    MODBUS_RECV_FLAG_START      = 0x10,     /**< receiving has been started */
    MODBUS_RECV_FLAG_MORE       = 0x20,     /**< more data is expected */
    MODBUS_RECV_FLAG_STOP       = 0x40,     /**< all data has been received */
} modbus_recv_flags_t;

/**
 * @brief   Modbus send flags.
 *
 * These flags will communicated to the send function of the driver.
 */
typedef enum {
    MODBUS_SEND_FLAG_NONE       = 0x00,     /**< no flags */
    MODBUS_SEND_FLAG_REQUEST    = 0x01,     /**< sending a request */
    MODBUS_SEND_FLAG_RESPONSE   = 0x02,     /**< sending a response */
} modbus_send_flags_t;

/**
 * @brief   Modbus address limits
 */
#define MODBUS_ID_BROADCAST             0   /**< broadcast identifier */
#define MODBUS_ID_INVALID               255 /**< invalid client identifier */
#define MODBUS_ID_MIN                   1   /**< first client identifier */
#define MODBUS_ID_MAX                   247 /**< last client identifier */

/**
 * @brief   Modbus PDU size limits
 */
#define MODBUS_PDU_MIN_REQUEST_SIZE     1   /**< minimal request size (function code) */
#define MODBUS_PDU_MIN_RESPONSE_SIZE    2   /**< minimal response size (function code +
                                                 exception) */
#define MODBUS_PDU_MAX_SIZE             253 /**< maximum size of PDU (function code +
                                                 up to 252 bytes of data) */

/**
 * @brief   Size in bytes of a single register
 */
#define MODBUS_REGISTER_SIZE 2

/**
 * @brief   Forward declaration of Modbus driver descriptor
 */
typedef struct modbus_desc modbus_desc_t;

/**
 * @brief   Forward declaration of Modbus structure
 */
typedef struct modbus modbus_t;

/**
 * @brief   Forward declaration of Modbus server structure
 */
typedef struct modbus_server modbus_server_t;

/**
 * @brief   Forward declaration of Modbus message structure
 */
typedef struct modbus_message modbus_message_t;

/**
 * @brief   Server callback function for handling client requests
 *
 * This server callback is invoked when a client request message is received
 * for a specific server. The callback must handle the client request message
 * accordingly and reply with @p modbus_server_reply if necessary.
 *
 * For read requests, copy the requested data directly to @p message->data (up
 * to @p message->data_size bytes), or provide a pointer to a buffer of
 * adequate size. For write requests, read the provided data from
 * @p message->data (up to @p message->data_size bytes).
 *
 * Note that @p message->data is a plain buffer. Copy registers or bits using
 * the appropriate helpers (see @p modbus_copy_regs and @p modbus_copy_bits).
 *
 * If the client request message SHOULD NOT be handled, or if the client
 * request message is broadcasted, the callback must not invoke
 * @p modbus_server_reply.
 *
 * If the client request message CANNOT be handled, @p message->exc must be set
 * before invoking @p modbus_server_reply. Some example are:
 *
 * - @p MODBUS_EXC_ILLEGAL_FUNCTION if the function code is not supported
 * - @p MODBUS_EXC_ILLEGAL_ADDRESS if the address range is out of bounds
 * - @p MODBUS_EXC_ILLEGAL_VALUE for other reasons.
 *
 * The callback must return @p 0 to indicate successful processing of the
 * request. Any other return value will be returned to the caller (which is
 * usually @p modbus_server_listen).
 *
 * @param[in] modbus        Modbus context
 * @param[in] server        The server receiving the client request
 * @param[in] message       The client request message
 *
 * @note The message SHOULD be validated prior handling. You can use
 *       @p modbus_check_message for this purpose, and act accordingly.
 *
 * @retval                  0 on successful processing (regardless of whether
 *                          message is supported or not)
 * @retval                  != 0 on processing error (will be returned to
 *                          dispatcher)
 */
typedef int (modbus_server_cb_t)(modbus_t *modbus, modbus_server_t *server,
                                 modbus_message_t *message);

/**
 * @brief   Modbus server structure
 */
struct modbus_server {
    struct modbus_server *next;     /**< reference to the next server (internal) */

    uint8_t id;                     /**< address of server */
    uint8_t flags;                  /**< server flags (see @p modbus_server_flags_t) */

    modbus_server_cb_t *cb;         /**< request callback */
    void *arg;                      /**< optional context for the request callback */
};

/**
 * @brief   Modbus bus structure
 *
 * @note    A single instance can be used for client mode or server mode, but
 *          not at the same time.
 */
struct modbus {
    const modbus_desc_t *driver;            /**< Modbus bus driver */
    modbus_server_t *servers;               /**< linked list of servers */

    mutex_t lock;                           /**< bus lock for exclusive access */

    void *dev;                              /**< device-specific data */
    void *params;                           /**< device-specific parameters */

    uint8_t buffer[MODBUS_PDU_MAX_SIZE];    /**< buffer for requests and responses */
    size_t buffer_size;                     /**< current size of @p buffer in bytes */
};

/**
 * @brief   Modbus message structure for requests and responses
 */
struct modbus_message {
    uint8_t id;             /**< address of client (not used in responses) */

    modbus_fc_t func;       /**< function code */
    uint16_t addr;          /**< starting address */
    uint16_t count;         /**< number of register or bits */

    modbus_exc_t exc;       /**< exception code (used in responses) */

    uint8_t *data;          /**< pointer to data (registers or bits) */
    size_t data_size;       /**< size of data (in bytes) */
};

/**
 * @brief Modbus driver descriptor structure
 *
 * This structure defines the low-level driver interface that must be
 * implemented for each Modbus transport (e.g., RTU, TCP). The driver handles
 * initialization, message transmission, and reception.
 */
struct modbus_desc {
    /**
     * @brief Initialize the Modbus driver
     *
     * @param[in,out] modbus    Modbus context to initialize
     *
     * @retval 0 on success
     * @retval -ENODEV on initialization failure
     */
    int (*init)(modbus_t *modbus);

    /**
     * @brief Send a Modbus message
     *
     * @param[in,out] modbus    Modbus context
     * @param[in] message       Message to send
     * @param[in] flags         Send flags (see @c modbus_send_flags_t)
     *
     * @retval 0 on success
     * @return error code on send failure
     */
    int (*send)(modbus_t *modbus, modbus_message_t *message, uint8_t flags);

    /**
     * @brief Receive a Modbus message
     *
     * This method will be invoked with the flags:
     *
     * - @c MODBUS_RECV_START indicate the start of a new message reception
     * - @c MODBUS_RECV_MORE indicate that more data is expected
     * - @c MODBUS_RECV_STOP indicate the end of the message reception
     *
     * In addition:
     *
     * - @c MODBUS_RECV_REQUEST indicates that a request is being received (may block)
     * - @c MODBUS_RECV_RESPONSE indicates that a response is being received (may time-out)
     *
     * This function must populate @p message->id with the identifier when the
     * @c MODBUS_RECV_START flag is set.
     *
     * @param[in,out] modbus    Modbus context
     * @param[out] message      Buffer to store received message
     * @param[in] size          Maximum number of bytes to receive
     * @param[in] flags         Reception flags (see @c modbus_recv_flags_t)
     *
     * @retval 0 on success
     * @retval -ETIMEDOUT on timeout
     * @retval -EBADMSG on message errors
     * @retval -ENOBUFS if buffer is too small
     */
    int (*recv)(modbus_t *modbus, modbus_message_t *message, size_t size, uint8_t flags);
};

/**
 * @brief Initialize a Modbus instance with the given driver and device.
 *
 * @param[in,out] modbus    Modbus context to initialize
 * @param[in] driver        Driver descriptor providing init/send/recv hooks
 * @param[in] dev           Driver-specific device state (owned by caller)
 * @param[in] params        Optional driver parameters (can be NULL)
 *
 * @retval 0 on success
 * @retval -ENODEV on failure
 */
int modbus_init(modbus_t *modbus, const modbus_desc_t *driver, void *dev, void *params);

/**
 * @brief Send a unicast Modbus request and receive the response.
 *
 * @param[in,out] modbus    Modbus context
 * @param[in,out] message   Request to send; filled with response data on success
 *
 * @retval 0 on success
 * @retval -EBUSY if bus is not idle (when it should be)
 * @retval -EPROTO error code on protocol errors
 *
 * @pre The @p modbus_check_message function returned without an error.
 */
int modbus_client_request(modbus_t *modbus, modbus_message_t *message);

/**
 * @brief Send a broadcast Modbus request (no response expected).
 *
 * This method will set @c message->id to @p MODBUS_ID_BROADCAST before sending.
 *
 * @param[in,out] modbus    Modbus context
 * @param[in] message       Request to broadcast
 *
 * @retval 0 on success
 * @retval -EBUSY if bus is not idle (when it should be)
 *
 * @pre The @p modbus_check_message function returned without an error.
 */
int modbus_client_broadcast(modbus_t *modbus, modbus_message_t *message);

/**
 * @brief Listen for a client request message and dispatch it to registered
 *        servers.
 *
 * This method will block until a client request message is received. When a
 * message is received, it will be dispatched to the appropriate server, or
 * servers in case of a broadcast message.
 *
 * If a message cannot be read or dispatched, an error code will be returned.
 * This may happen if there are communication errors, or if a server fails to
 * handle the request.
 *
 * After executing this method, @p message SHOULD not be used, as its data
 * may be stale.
 *
 * @param[in,out] modbus    Modbus context
 * @param[in,out] message   Message to store the received request into
 *
 * @retval 0 on success
 * @retval -EBADMSG on message errors
 * @retval -EPROTO on protocol errors
 * @retval -ETIMEDOUT on message reception timeout
 */
int modbus_server_listen(modbus_t *modbus, modbus_message_t *message);

/**
 * @brief Send a server response for the given request.
 *
 * @param[in,out] modbus    Modbus context
 * @param[in] message       Response to send (function/exception/data as set by callback)
 *
 * @retval 0 on success
 * @retval -EBUSY if bus is not idle (when it should be)
 *
 * @pre The @p modbus_check_message function returned without an error.
 * @pre The value of @p message->id is not equal to @c MODBUS_ID_BROADCAST.
 */
int modbus_server_reply(modbus_t *modbus, modbus_message_t *message);

/**
 * @brief Register a server handler on a Modbus context.
 *
 * The server will be added to the head of the list of servers.
 *
 * @param[in,out] modbus    Modbus context
 * @param[in,out] server    Server descriptor to add (must remain valid)
 *
 * @retval 0 on successful addition
 * @retval -EEXIST if a server with the same address already exists
 */
int modbus_server_add(modbus_t *modbus, modbus_server_t *server);

/**
 * @brief Unregister a server handler from a Modbus context.
 *
 * @param[in,out] modbus    Modbus context
 * @param[in] server        Server descriptor to remove
 *
 * @retval 0 on successful removal
 * @retval -ENOENT if the server was not found
 */
int modbus_server_remove(modbus_t *modbus, modbus_server_t *server);

/**
 * @brief Look up a registered server by its address.
 *
 * @param[in] modbus       Modbus context
 * @param[in] id           Server address to search for (must not be @c MODBUS_ID_BROADCAST)
 *
 * @return Pointer to the server if found, @c NULL otherwise.
 */
modbus_server_t * modbus_server_get(modbus_t *modbus, uint8_t id);

/**
 * @brief Iterate over registered servers matching a given address.
 *
 * If @p id is @c MODBUS_ID_BROADCAST, only servers with the
 * @p MODBUS_SERVER_FLAG_RECEIVE_BROADCAST flag set will be returned.
 *
 * @param[in] modbus        Modbus context
 * @param[in] prev          Previous server returned by this function, or
 *                          @c NULL to start iteration
 * @param[in] id            Server address to search for
 *
 * @return Pointer to the next matching server, or @c NULL if no more servers
 *         match.
 */
modbus_server_t * modbus_server_iter(modbus_t *modbus, modbus_server_t *prev, uint8_t id);

/**
 * @brief Compute the size in bytes of a @p count bits
 *
 * @param[in] count         Number of bits

 * @return                  Size in bytes to represent @p count bits
 */
size_t modbus_bit_count_to_size(uint16_t count);

/**
 * @brief Compute the size in bytes of a @p count registers
 *
 * @param[in] count         Number of registers

 * @return                  Size in bytes to represent @p count registers
 */
size_t modbus_reg_count_to_size(uint16_t count);

/**
 * @brief   Copy @p count of bits from @p src to @p dst
 *
 * When @p pad_zeroes is @c true, all bits in the byte range addressed by
 * @p start_bit_dst and @p start_bit_dst + @p count will be set to zero. This
 * can be useful when writing response messages to buffers that are not zeroed
 * out.
 *
 * @param[out] dst          Pointer to destination
 * @param[in] start_bit_dst Offset of the number of bits in @p dst
 * @param[in] src           Pointer to source
 * @param[in] start_bit_src Offset of the the number bits in @p src
 * @param[in] count         Number of bits to copy
 * @param[in] pad_zeroes    Whether to pad @p dst with zeroes
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
 * @param[out] dst          Pointer to destination
 * @param[in] start_bit_dst Offset of the number of bits in @p dst
 * @param[in] src           Pointer to source
 * @param[in] start_bit_src Offset of the the number bits in @p src
 * @param[in] pad_zeroes    Whether to pad @p dst with zeroes
 */
void modbus_copy_bit(uint8_t *dst, uint16_t start_bit_dst,
                     const uint8_t *src, uint16_t start_bit_src,
                     bool pad_zeroes);

/**
 * @brief   Copy @p count of registers from @p src to @p dst
 *
 * @param[out] dst          Pointer to destination
 * @param[in] src           Pointer to source
 * @param[in] count         Number of registers to copy
 */
void modbus_copy_regs(void *dst, const void *src, const uint16_t count);

/**
 * @brief   Copy a single register from @p src to @p dst
 *
 * @param[out] dst          Pointer to destination
 * @param[in] src           Pointer to source
 */
void modbus_copy_reg(void *dst, const void *src);

/**
 * @brief   Validate a message
 *
 * Check if the message properties are within valid ranges. This ensures that
 * the message is valid for further processing.
 *
 * @param[in] message       The message to validate
 *
 * @retval                  0 if @p message is valid
 * @retval                  -EINVAL if @p message is not valid
 */
int modbus_check_message(const modbus_message_t *message);

#ifdef __cplusplus
}
#endif

/** @} */
