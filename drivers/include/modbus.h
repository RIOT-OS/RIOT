/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_modbus Modbus
 * @ingroup     drivers
 * @brief       Modbus interface definitions
 *
 * Modbus interface definitions for client and server modes (formerly known as
 * master and slave). This module provides four interfaces:
 *
 * - A client/server interface for handling Modbus requests and responses.
 * - A protocol data unit (PDU) interface, for translating between
 *   message types and byte buffers.
 * - A transport descriptor interface, for implementing different Modbus
 *   transports (e.g. Modbus RTU). Actual transport implementations are not
 *   provided by this module.
 * - Utility methods for working with Modbus messages, such as copying bits and
 *   registers.
 *
 * @{
 *
 * @file
 * @brief       Modbus interface definitions
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Modbus function codes
 *
 * This list is incomplete, but covers the most common function codes.
 */
typedef enum {
    MODBUS_FC_NONE                              = 0,    /**< no function (reserved) */
    MODBUS_FC_READ_COILS                        = 1,    /**< read coils */
    MODBUS_FC_READ_DISCRETE_INPUTS              = 2,    /**< read discrete inputs */
    MODBUS_FC_READ_HOLDING_REGISTERS            = 3,    /**< read holding registers */
    MODBUS_FC_READ_INPUT_REGISTERS              = 4,    /**< read input registers */
    MODBUS_FC_WRITE_SINGLE_COIL                 = 5,    /**< write single coil */
    MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER     = 6,    /**< write single register */
    MODBUS_FC_WRITE_MULTIPLE_COILS              = 15,   /**< write multiple coils */
    MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS  = 16,   /**< write multiple registers */
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
    MODBUS_SERVER_FLAG_RECEIVE_ANY          = 0x02, /**< receive any message */
} modbus_server_flags_t;

/**
 * @name    Modbus address limits
 * @{
 */
#define MODBUS_ID_BROADCAST             (0)     /**< broadcast identifier */
#define MODBUS_ID_INVALID               (255)   /**< invalid identifier */
#define MODBUS_ID_MIN                   (1)     /**< first valid identifier */
#define MODBUS_ID_MAX                   (247)   /**< last valid identifier */
/** @} */

/**
 * @name    Modbus PDU size limits
 * @{
 */
#define MODBUS_PDU_MIN_REQUEST_SIZE     (1)     /**< minimal request size (function code) */
#define MODBUS_PDU_MIN_RESPONSE_SIZE    (2)     /**< minimal response size (function code +
                                                     exception) */
#define MODBUS_PDU_MAX_SIZE             (253)   /**< maximum size of PDU (function code +
                                                     up to 252 bytes of data) */
/** @} */

/**
 * @brief   Size in bytes of a single register
 */
#define MODBUS_REGISTER_SIZE            (2)

/**
 * @brief   Forward declaration of Modbus transport descriptor
 */
typedef struct modbus_transport_desc modbus_transport_desc_t;

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
 * accordingly and reply with @ref modbus_server_reply if necessary.
 *
 * For read requests, copy the requested data directly to @c message->data (up
 * to @c message->data_size bytes), or provide a pointer to a buffer of
 * adequate size. For write requests, read the provided data from
 * @c message->data (up to @c message->data_size bytes).
 *
 * Note that @c message->data is a plain buffer. Copy registers or bits using
 * the appropriate helpers (see @ref modbus_copy_regs and
 * @ref modbus_copy_bits).
 *
 * If the client request message SHOULD NOT be handled, or if the client
 * request message is broadcasted, the callback must not invoke
 * @ref modbus_server_reply.
 *
 * If the client request message CANNOT be handled, @c message->exc must be
 * set before invoking @ref modbus_server_reply. Some examples are:
 *
 * - @ref MODBUS_EXC_ILLEGAL_FUNCTION if the function code is not supported
 * - @ref MODBUS_EXC_ILLEGAL_ADDRESS if the address range is out of bounds
 * - @ref MODBUS_EXC_ILLEGAL_VALUE for other reasons.
 *
 * The callback must return @c 0 to indicate successful processing of the
 * request. Any other return value will be returned to the caller (which is
 * usually @ref modbus_server_listen).
 *
 * @param[in] modbus        Modbus context
 * @param[in] server        The server receiving the client request
 * @param[in,out] message   The client request message
 *
 * @note The message SHOULD be validated prior handling. You can use
 *       @ref modbus_check_message for this purpose, and act accordingly.
 *
 * @retval 0                on successful processing (regardless of whether
 *                          the message is supported or not)
 * @return any other value on processing error (will be returned to the
 *         dispatcher)
 */
typedef int (*modbus_server_cb_t)(modbus_t *modbus, modbus_server_t *server,
                                  modbus_message_t *message);

/**
 * @brief   Modbus server structure
 */
struct modbus_server {
    struct modbus_server *_next;    /**< reference to the next server (internal) */

    uint8_t id;                     /**< address of server */
    uint8_t flags;                  /**< server flags (see @ref modbus_server_flags_t) */

    modbus_server_cb_t cb;          /**< request callback */
    void *arg;                      /**< optional context for the request callback */
};

/**
 * @brief   Modbus bus structure
 *
 * All fields are managed by this module and the transport implementation, and
 * must not be modified by users.
 *
 * @note    A single instance can only be used for client mode or server mode,
 *          but not for both at the same time.
 */
struct modbus {
    const modbus_transport_desc_t *transport;   /**< Modbus transport descriptor */

    modbus_server_t *servers;                   /**< linked list of servers */
    mutex_t servers_lock;                       /**< lock protecting the server registry */

    mutex_t lock;                               /**< bus lock for exclusive access */

    void *dev;                                  /**< device-specific data */
    void *params;                               /**< device-specific parameters */

    uint8_t *buffer;                            /**< PDU buffer slice owned by the transport
                                                    (set by the transport init hook) */
    size_t buffer_capacity;                     /**< maximum bytes writable into @c buffer */
    size_t buffer_size;                         /**< current size of @c buffer in bytes */
};

/**
 * @brief   Modbus message structure for requests and responses
 */
struct modbus_message {
    uint8_t id;             /**< address of the server this message is
                                 directed to (in requests) or originates
                                 from (in responses) */

    modbus_fc_t func;       /**< function code */
    uint16_t addr;          /**< starting address */
    uint16_t count;         /**< number of registers or bits */

    modbus_exc_t exc;       /**< exception code (used in responses) */

    uint8_t *data;          /**< pointer to data (registers or bits) */
    size_t data_size;       /**< size of data (in bytes) */
};

/**
 * @brief Modbus transport descriptor structure
 *
 * This structure defines the low-level transport interface that must be
 * implemented for each Modbus transport (e.g., RTU, TCP). The transport
 * handles initialization, message transmission, and reception.
 */
struct modbus_transport_desc {
    /**
     * @brief Initialize the Modbus transport
     *
     * @param[in,out] modbus    Modbus context to initialize
     *
     * @retval 0                on success
     * @retval -ENODEV          on initialization failure
     * @return any other negative errno for transport-specific errors
     */
    int (*init)(modbus_t *modbus);

    /**
     * @brief Send a Modbus message
     *
     * The PDU to send is held in @c modbus->buffer (with length
     * @c modbus->buffer_size). The transport adds any address, framing and
     * checksum bytes around it.
     *
     * @param[in,out] modbus    Modbus context
     * @param[in] message       Message being sent (provides identifier and
     *                          flags)
     *
     * @retval 0                on success
     * @retval -EBUSY           if bus is not idle (when it should be)
     * @return any other negative errno for transport-specific errors
     */
    int (*send)(modbus_t *modbus, const modbus_message_t *message);

    /**
     * @brief Receive a Modbus message
     *
     * The transport reads a complete ADU, places the PDU in @c modbus->buffer,
     * sets @c modbus->buffer_size to the PDU length, and writes the address
     * byte into @c message->id.
     *
     * The @p request flag indicates how the transport should receive the
     * message:
     *
     * - when @c true, a request is being received (may block)
     * - when @c false, a response is being received (may time-out)
     *
     * @param[in,out] modbus    Modbus context
     * @param[out] message      Message to populate with the address (other
     *                          fields are decoded by the caller)
     * @param[in] request       @c true when receiving a request, @c false
     *                          when receiving a response
     *
     * @retval 0                on success
     * @retval -ETIMEDOUT       on timeout
     * @retval -EBADMSG         on framing or checksum errors
     * @retval -ENOBUFS         if buffer is too small
     * @return any other negative errno for transport-specific errors
     */
    int (*recv)(modbus_t *modbus, modbus_message_t *message, bool request);
};

/**
 * @brief Initialize a Modbus instance with the given transport and device.
 *
 * @param[in,out] modbus    Modbus context to initialize (must not be NULL)
 * @param[in] transport     Transport descriptor providing the init, send and
 *                          recv hooks (must not be NULL)
 * @param[in] dev           Transport-specific device state, owned by the
 *                          caller (must not be NULL)
 * @param[in] params        Optional transport parameters (can be NULL)
 *
 * @retval 0                on success
 * @return any negative errno returned by the transport init hook (typically
 *         @c -ENODEV on initialization failure)
 */
int modbus_init(modbus_t *modbus, const modbus_transport_desc_t *transport,
                void *dev, void *params);

/**
 * @brief Send a unicast Modbus request and receive the response.
 *
 * A server may answer with an exception response. In that case, this function
 * returns @c 0 and @c message->exc is set to the exception code.
 *
 * @param[in,out] modbus    Modbus context (must not be NULL)
 * @param[in,out] message   Request to send; filled with response data on
 *                          success (must not be NULL)
 *
 * @retval 0                on success (@c message->exc indicates whether the
 *                          server responded with an exception)
 * @retval -EBUSY           if bus is not idle (when it should be)
 * @retval -EBADMSG         if the function code is unsupported, or if the
 *                          response is malformed
 * @retval -EINVAL          if the message is inconsistent (e.g. data size
 *                          too small for the requested count)
 * @retval -ENOBUFS         if a buffer is too small for the request or
 *                          response
 * @retval -ETIMEDOUT       if no response was received in time
 * @retval -EPROTO          on protocol errors (e.g. response from an
 *                          unexpected server, or function code mismatch)
 * @return any other negative errno returned by the transport
 *
 * @pre @p message has been validated with @ref modbus_check_message.
 */
int modbus_client_request(modbus_t *modbus, modbus_message_t *message);

/**
 * @brief Send a broadcast Modbus request (no response expected).
 *
 * This function will set @c message->id to @ref MODBUS_ID_BROADCAST before
 * sending.
 *
 * @param[in,out] modbus    Modbus context (must not be NULL)
 * @param[in,out] message   Request to broadcast (must not be NULL)
 *
 * @retval 0                on success
 * @retval -EBUSY           if bus is not idle (when it should be)
 * @retval -EINVAL          if the function code is not a write function
 *                          code, or if the message is inconsistent
 * @retval -EBADMSG         if the function code is unsupported
 * @retval -ENOBUFS         if the buffer is too small for the request
 * @return any other negative errno returned by the transport
 *
 * @pre @p message has been validated with @ref modbus_check_message.
 */
int modbus_client_broadcast(modbus_t *modbus, modbus_message_t *message);

/**
 * @brief Listen for a client request message and dispatch it to registered
 *        servers for processing.
 *
 * This function will block until a client request message is received. When a
 * message is received, it will be dispatched to the appropriate server, or
 * servers in case of a broadcast message.
 *
 * If a message cannot be read or dispatched, an error code will be returned.
 * This may happen if there are communication errors, or if a server fails to
 * handle the request.
 *
 * After executing this function, @p message SHOULD NOT be used, as its data
 * may be stale.
 *
 * @param[in,out] modbus    Modbus context (must not be NULL)
 * @param[in,out] message   Message to store the received request into (must
 *                          not be NULL)
 *
 * @retval 0                on success
 * @retval -EBADMSG         on malformed or rejected requests (e.g.
 *                          out-of-range quantities)
 * @retval -ENOBUFS         if the receive buffer is too small
 * @retval -ENOENT          if no server is registered for the request
 * @retval -EPROTO          on protocol errors
 * @retval -ETIMEDOUT       on message reception timeout
 * @return any non-zero value returned by a server callback, or any other
 *         negative errno returned by the transport
 */
int modbus_server_listen(modbus_t *modbus, modbus_message_t *message);

/**
 * @brief Send a server response for the given request.
 *
 * @param[in,out] modbus    Modbus context (must not be NULL)
 * @param[in,out] message   Response to send, with function code, exception
 *                          and data as set by the callback (must not be
 *                          NULL); its data pointer is invalidated once the
 *                          response has been sent
 *
 * @retval 0                on success
 * @retval -EBUSY           if bus is not idle (when it should be)
 * @retval -EBADMSG         if the function code is unsupported
 * @retval -EINVAL          if the message is inconsistent (e.g. data size
 *                          too small for the requested count)
 * @retval -ENOBUFS         if the buffer is too small for the response
 * @return any other negative errno returned by the transport
 *
 * @pre @p message has been validated with @ref modbus_check_message.
 * @pre The value of @c message->id is not equal to @ref MODBUS_ID_BROADCAST.
 */
int modbus_server_reply(modbus_t *modbus, modbus_message_t *message);

/**
 * @brief Register a server handler on a Modbus context.
 *
 * The server will be added to the head of the list of servers.
 *
 * @note This function is safe to call from a different thread while the
 *       listener is running. The server struct must remain valid until
 *       @ref modbus_server_remove is called and @ref modbus_server_listen
 *       has returned.
 *
 * @param[in,out] modbus    Modbus context (must not be NULL)
 * @param[in,out] server    Server descriptor to add (must not be NULL, and
 *                          must remain valid)
 *
 * @retval 0                on successful addition
 * @retval -EINVAL          if the server identifier is out of range (see
 *                          @ref MODBUS_ID_MIN and @ref MODBUS_ID_MAX)
 * @retval -EEXIST          if a server with the same address already exists
 */
int modbus_server_add(modbus_t *modbus, modbus_server_t *server);

/**
 * @brief Unregister a server handler from a Modbus context.
 *
 * @note This function is safe to call from a different thread while the
 *       listener is running. After this function returns, the server will no
 *       longer be dispatched in subsequent calls to @ref modbus_server_listen.
 *       Do not call this function from within a server callback.
 *
 * @param[in,out] modbus    Modbus context (must not be NULL)
 * @param[in] server        Server descriptor to remove (must not be NULL)
 *
 * @retval 0                on successful removal
 * @retval -ENOENT          if the server was not found
 */
int modbus_server_remove(modbus_t *modbus, modbus_server_t *server);

/**
 * @brief Look up a registered server by its address.
 *
 * @param[in] modbus        Modbus context (must not be NULL)
 * @param[in] id            Server address to search for (must not be
 *                          @ref MODBUS_ID_BROADCAST)
 *
 * @return Pointer to the server if found, @c NULL otherwise.
 */
modbus_server_t *modbus_server_get(const modbus_t *modbus, uint8_t id);

/**
 * @brief Iterate over registered servers matching a given address.
 *
 * If @p id is @ref MODBUS_ID_BROADCAST, only servers with the
 * @ref MODBUS_SERVER_FLAG_RECEIVE_BROADCAST or
 * @ref MODBUS_SERVER_FLAG_RECEIVE_ANY flag set will be returned.
 *
 * @param[in] modbus        Modbus context (must not be NULL)
 * @param[in] prev          Previous server returned by this function, or
 *                          @c NULL to start iteration
 * @param[in] id            Server address to search for
 *
 * @return Pointer to the next matching server, or @c NULL if no more servers
 *         match.
 */
modbus_server_t *modbus_server_iter(const modbus_t *modbus,
                                    modbus_server_t *prev, uint8_t id);

/**
 * @brief Compute the size in bytes of @p count bits
 *
 * @param[in] count         Number of bits
 *
 * @return                  Size in bytes to represent @p count bits
 */
size_t modbus_bit_count_to_size(uint16_t count);

/**
 * @brief   Compute the size in bytes of @p count registers
 *
 * @param[in] count         Number of registers
 *
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
 * @param[out] dst          Pointer to destination (must not be NULL)
 * @param[in] start_bit_dst Bit offset in @p dst
 * @param[in] src           Pointer to source (must not be NULL)
 * @param[in] start_bit_src Bit offset in @p src
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
 * @param[out] dst          Pointer to destination (must not be NULL)
 * @param[in] start_bit_dst Bit offset in @p dst
 * @param[in] src           Pointer to source (must not be NULL)
 * @param[in] start_bit_src Bit offset in @p src
 * @param[in] pad_zeroes    Whether to pad @p dst with zeroes
 */
void modbus_copy_bit(uint8_t *dst, uint16_t start_bit_dst,
                     const uint8_t *src, uint16_t start_bit_src,
                     bool pad_zeroes);

/**
 * @brief   Copy @p count of registers from @p src to @p dst
 *
 * @param[out] dst          Pointer to destination (must not be NULL)
 * @param[in] src           Pointer to source (must not be NULL)
 * @param[in] count         Number of registers to copy
 */
void modbus_copy_regs(void *dst, const void *src, uint16_t count);

/**
 * @brief   Copy a single register from @p src to @p dst
 *
 * @param[out] dst          Pointer to destination (must not be NULL)
 * @param[in] src           Pointer to source (must not be NULL)
 */
void modbus_copy_reg(void *dst, const void *src);

/**
 * @brief   Validate a message
 *
 * Check if the message properties are within valid ranges. This ensures that
 * the message is valid for further processing.
 *
 * @param[in] message       The message to validate (must not be NULL)
 *
 * @retval 0                if @p message is valid
 * @retval -EINVAL          if @p message is not valid
 */
int modbus_check_message(const modbus_message_t *message);

/**
 * @brief   Encode a request PDU into @p buf.
 *
 * The function ignores @c message->id, which is handled by the transport.
 *
 * @param[out] buf          PDU buffer (must not be NULL)
 * @param[in] len           Capacity of @p buf in bytes
 * @param[in] message       Request message to encode (must not be NULL)
 *
 * @return number of bytes written into @p buf on success
 * @retval 0                if @p message has an unsupported function code
 * @retval -EINVAL          if @p message is inconsistent (e.g. data is NULL
 *                          or data size too small for the requested count)
 * @retval -ENOBUFS         if @p buf is too small
 *
 * @pre @p message has been validated with @ref modbus_check_message.
 */
ssize_t modbus_encode_request(uint8_t *buf, size_t len,
                              const modbus_message_t *message);

/**
 * @brief   Encode a response PDU into @p buf.
 *
 * If @c message->exc is non-zero, an exception response is encoded instead.
 *
 * The function ignores @c message->id, which is handled by the transport.
 *
 * @param[out] buf          PDU buffer (must not be NULL)
 * @param[in] len           Capacity of @p buf in bytes
 * @param[in] message       Response message to encode (must not be NULL)
 *
 * @return number of bytes written into @p buf on success
 * @retval 0                if @p message has an unsupported function code
 * @retval -EINVAL          if @p message is inconsistent (e.g. data is NULL
 *                          or data size too small for the requested count)
 * @retval -ENOBUFS         if @p buf is too small
 *
 * @pre @p message has been validated with @ref modbus_check_message.
 */
ssize_t modbus_encode_response(uint8_t *buf, size_t len,
                               const modbus_message_t *message);

/**
 * @brief   Decode a request PDU from @p buf.
 *
 * Fills @c message->func, @c addr, @c count, @c exc, @c data and @c data_size.
 *
 * The @c message->id field is left untouched, which is set by the transport.
 *
 * On success, @c message->data points into @p buf (zero-copy) and is valid
 * until @p buf is reused.
 *
 * Requests with out-of-range quantities are rejected as malformed, so that
 * @c message->count and @c message->data_size are always safe to use by a
 * server callback.
 *
 * @param[in,out] buf       PDU buffer (must not be NULL)
 * @param[in] len           Length of the PDU
 * @param[out] message      Message to decode into (must not be NULL)
 *
 * @retval 0                on success (an unsupported function code is
 *                          reported via @c message->exc set to
 *                          @ref MODBUS_EXC_ILLEGAL_FUNCTION)
 * @retval -EBADMSG         if the PDU is malformed (truncated or byte count
 *                          mismatch)
 * @retval -EPROTO          if the exception bit is set on a request
 * @retval -ERANGE          if the requested quantity is out of range (e.g. too
 *                          many registers or bits)
 */
int modbus_decode_request(uint8_t *buf, size_t len, modbus_message_t *message);

/**
 * @brief   Decode a response PDU from @p buf.
 *
 * Fills @c message->func, @c exc, @c data and @c data_size. The
 * @c message->addr and @c count fields are read as inputs to validate response
 * framing.
 *
 * The @c message->id field is left untouched, which is set by the transport.
 *
 * On success, @c message->data points into @p buf (zero-copy) and is valid
 * until @p buf is reused. If @c message->data is non-NULL on entry, the data
 * is copied into the caller-provided buffer instead.
 *
 * @param[in,out] buf       PDU buffer (must not be NULL)
 * @param[in] len           Length of the PDU
 * @param[in,out] message   Message to decode into (must not be NULL)
 *
 * @retval 0                on success
 * @retval -EBADMSG         if the PDU is malformed (truncated)
 * @retval -ENOBUFS         if the caller-provided data buffer is too small
 * @retval -EPROTO          on protocol errors (size mismatch, addr/count
 *                          echo mismatch, exception with
 *                          @ref MODBUS_EXC_NONE code)
 */
int modbus_decode_response(uint8_t *buf, size_t len, modbus_message_t *message);

#ifdef __cplusplus
}
#endif

/** @} */
