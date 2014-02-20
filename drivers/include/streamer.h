/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the LGPLv2 License.
 * See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup     drivers
 * @brief       High-level byte stream handler driver API.
 * @details     This API allows unified access to all kinds of stream or packet
 *              handlers as for example radio chips, files, serial interfaces,
 *              network protocols, sockets, etc.
 *              If no byte order for any type in this API is given in the
 *              documentation, it is assumed to be in the byte-order of the
 *              host. Otherwise, the byte order must be formated as stated.
 * @{
 *
 * @file        streamer.h
 * @brief       High-level byte stream handler driver API definitions.
 *
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __STREAMER_H
#define __STREAMER_H

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "msg.h"

/**
 * @brief Message types for stream handler interface
 */
typedef enum {
    STREAMER_MSG_TYPE_CMD,      ///< Command was issued for the stream handler
    STREAMER_MSG_TYPE_RESULT,   ///< Resuld was issued from the stream handler
} streamer_msg_type_t;

/**
 * @brief Definition of option types for stream handlers.
 *
 * @details This defines the option types for stream handlers.
 *          Option values can be get with a STREAMER_CMD_GET_OPT command and set
 *          with a STREAMER_CMD_SET_OPT command to the stream handler's thread.
 *          If a streamer does not support the getting or setting of an option
 *          it answers with the STREAMER_RESULT_ERROR result and sets
 *          streamer_result_t.error.errno to EOPNOTSUPP.
 */
typedef enum __attribute__((packed)) {
    /**
     * @brief   Option type to get or set source addresses of the stream handler.
     * @details Expects values of type streamer_addr_len_t as getter and
     *          streamer_addr_list_t as setter.
     *          If a streamer does not support the given address length it
     *          answers with the STREAMER_RESULT_ERROR result and sets
     *          streamer_result_t.error.errno to EAFNOSUPPORT.
     *          If a streamer does not support the length of the given list it
     *          answers with the STREAMER_RESULT_ERROR result and sets
     *          streamer_result_t.error.errno to E2BIG.
     */
    STREAMER_OPT_ADDR = 0,

    /**
     * @brief   Option type to delete a source address or (if this is not
     *          supported) set the default address for the according length.
     * @details Expects values of type streamer_addr_list_t.
     *          If a streamer does not support the given address length it
     *          answers with the STREAMER_RESULT_ERROR result and sets
     *          streamer_result_t.error.errno to EAFNOSUPPORT.
     *          If a streamer does not support the length of the given list it
     *          answers with the STREAMER_RESULT_ERROR result and sets
     *          streamer_result_t.error.errno to E2BIG.
     */
    STREAMER_OPT_DEL_ADDR = 0,

    /**
     * @brief   Option type to get or set the radio channel of the streamer
     *          handler.
     * @details Expects values of type streamer_channel_t.
     */
    STREAMER_OPT_CHANNEL,

    /**
     * @brief   Option type to get or set the PAN ID of the streamer handler.
     * @details Expects values of type streamer_pan_id_t.
     */
    STREAMER_OPT_PAN_ID,

    /**
     * @brief   Option type to get or set the preferred length of the source
     *          address of the stream handler.
     * @details Expects values of type streamer_addr_len_t.
     */
    STREAMER_OPT_PREF_ADDR_LEN,

    /**
     * @brief   Option type to get the maximum fragment length of the stream
     *          handler.
     * @details Expects values of type size_t.
     */
    STREAMER_OPT_MAX_FRAG_LEN,

    /**
     * @brief   Option type to get the supported address lengths of the stream
     *          handler.
     * @details Expects values of type streamer_addr_len_list_t.
     */
    STREAMER_OPT_SUPP_ADDR_LEN,

    /**
     * @brief   Option type to get or update addresses the stream handler
     *          should ignore.
     * @details Expects values of type streamer_addr_list_t.
     *          Returns updated complete list of ignored addresses on success.
     */
    STREAMER_OPT_ADDR_IGN,

    /**
     * @brief   Option type to get or delete addresses the stream handler
     *          should ignore.
     * @details Expects values of type streamer_addr_list_t.
     *          Returns updated complete list of ignored addresses on success.
     */
    STREAMER_OPT_ADDR_IGN_DEL,

    /* ... */
} streamer_opt_t;

/**
 * @brief Definition of address length type.
 */
typedef uint8_t streamer_addr_len_t;

typedef struct __attribute__((packed)) streamer_addr_len_list_t {
    struct streamer_addr_len_list_t *next;  ///< next address length, NULL if last.
    streamer_addr_len_t addr_len;           ///< an address length
} streamer_addr_len_list_t;

/**
 * @brief Definition of address type for the stream handler option
 *        STREAMER_OPT_ADDR.
 */
typedef struct __attribute__((packed)) {
    void *addr;                         ///< The data of the address in network byte-order
    streamer_addr_len_t addr_len;       ///< The length of the address
} streamer_addr_t;

typedef struct __attribute__((packed)) streamer_addr_list_t {
    struct streamer_addr_list_t *next;      ///< next address, NULL if last.
    streamer_addr_t *addr;                  ///< an address
} streamer_addr_list_t;

/**
 * @brief Definition of channel type for the stream handler option
 *        STREAMER_OPT_CHANNEL.
 */
typedef uint16_t streamer_channel_t;

/**
 * @brief Definition of PAN ID type for the stream handler option
 *        STREAMER_OPT_PAN_ID.
 */
typedef uint16_t streamer_pan_id_t;

typedef struct __attribute__((packed)) {
    void *data;         ///< data in the buffer
    size_t data_len;    ///< length of the data
} streamer_buf_t;

/**
 * @brief   Definition of status types for stream handlers.
 *
 * @details This defines the status types for stream handlers.
 *          Status values can be get with a STREAMER_CMD_GET_STATUS command and set
 *          with a STREAMER_CMD_SET_STATUS command to the stream handler's thread.
 *          If a streamer does not support the getting or setting of a state
 *          it answers with the STREAMER_RESULT_ERROR result and sets
 *          streamer_result_t.error.nr to EOPNOTSUPP.
 */
typedef enum __attribute__((packed)) {
    /**
     * @brief   Stream handler is in receiving state (rx state for radios,
     *          listen for sockets e.g.)
     */
    STREAMER_STATUS_RECEIVING,
    STREAMER_STATUS_POWERDOWN,  ///< Stream handler is powered down.
} streamer_status_t;

/**
 * @brief   Definition of command types for stream handlers.
 * @details If a streamer does not support a command it answers with the
 *          STREAMER_RESULT_ERROR result and sets streamer_result_t.error.nr
 *          to EOPNOTSUPP.
 */
typedef enum __attribute__((packed)) {
    /**
     * @brief   Command type to register a thread to the stream handler's thread's events.
     * @details In case of of an error the stream handler answers with the
     *          STREAMER_RESULT_ERROR and sets streamer_result_t.error.nr
     *          accordingly
     */
    STREAMER_CMD_REG = 0,

    /**
     * @brief   Command type to let the stream handler write buffer to a stream.
     * @details It is up to the stream handler to fragment the stream into
     *          fragments if needed by the streaming medium.
     * @details In case of of an error the stream handler answers with the
     *          STREAMER_RESULT_ERROR and sets streamer_result_t.error.nr
     *          accordingly
     */
    STREAMER_CMD_WRITE,

    /**
     * @brief   Command type to let the stream handler read a stream into a buffer.
     * @details It is up to the stream handler to defragment received fragments
     *          into a stream of sufficient length.
     *          In case of of an error the stream handler answers with the
     *          STREAMER_RESULT_ERROR and sets streamer_result_t.error.nr
     *          accordingly
     */
    STREAMER_CMD_READ,

    /**
     * @brief   Command type to let the stream handler read a stream into a buffer
     *          from an address.
     * @details It is up to the stream handler to defragment received fragments
     *          into a stream of sufficient length.
     *          In case of of an error the stream handler answers with the
     *          STREAMER_RESULT_ERROR and sets streamer_result_t.error.nr
     *          accordingly
     */
    STREAMER_CMD_WRITE_TO,

    /**
     * @brief   Command type to let the stream handler write buffer to a stream
     *          to an address.
     * @details It is up to the stream handler to fragment the stream into
     *          fragments if needed by the streaming medium or return an
     *          appropriate error.
     *          In case of of an error the stream handler answers with the
     *          STREAMER_RESULT_ERROR and sets streamer_result_t.error.nr
     *          accordingly
     */
    STREAMER_CMD_READ_FROM,

    /**
     * @brief   Command type to get a stream handler option.
     * @details If a streamer does not support the getting of an option
     *          it answers with the STREAMER_RESULT_ERROR result and sets
     *          streamer_result_t.error.nr to EOPNOTSUPP.
     */
    STREAMER_CMD_GET_OPT,

    /**
     * @brief   Command type to set a stream handler option.
     * @details If a streamer does not support the setting of an option
     *          it answers with the STREAMER_RESULT_ERROR result and sets
     *          streamer_result_t.error.nr to EOPNOTSUPP.
     */
    STREAMER_CMD_SET_OPT,

    /**
     * @brief   Command type to get a stream handler's status.
     * @details If a streamer does not support the getting of a state
     *          it answers with the STREAMER_RESULT_ERROR result and sets
     *          streamer_result_t.error.nr to EOPNOTSUPP.
     */
    STREAMER_CMD_GET_STATUS,

    /**
     * @brief   Command type to set a stream handler's status.
     * @details If a streamer does not support the setting of a state
     *          it answers with the STREAMER_RESULT_ERROR result and sets
     *          streamer_result_t.error.nr to EOPNOTSUPP.
     */
    STREAMER_CMD_SET_STATUS,
} streamer_cmd_type_t;

/**
 * @brief   Definition of commands to stream handlers.
 */
typedef union __attribute__((packed)) {
    streamer_cmd_type_t type;       ///< The type of the command

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_REGISTER.
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_REG
        int pid;                    ///< The pid of the thread you want to register.
    } reg;

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_WRITE
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_WRITE
        streamer_buf_t buf;         ///< The buffer to write out from
    } write;

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_READ
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_READ
        streamer_buf_t buf;         ///< The buffer to read into
    } read;

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_WRITE_TO
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_WRITE_TO
        streamer_buf_t buf;         ///< The buffer to write out from
        streamer_addr_t *addr;      ///< The address to send the stream to.
    } write_to;

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_READ_FROM
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_READ_FROM
        streamer_buf_t buf;         ///< The buffer to read into
    } read_from;

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_GET_OPT
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_GET_OPT
        streamer_opt_t opt;         ///< The option to get
        /**
         * @brief   parameters (if needed) of options value type for the option
         *
         * @see streamer_opt_t
         */
        void *parameters;
    } get_opt;

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_SET_OPT
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_SET_OPT
        streamer_opt_t opt;         ///< The option to set
        void *value;                ///< The value for the option
    } set_opt;

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_GET_STATUS
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_GET_OPT
        streamer_status_t status;   ///< The status to get
    } get_status;

    /**
     * @brief   Definition of command subtype for STREAMER_CMD_SET_STATUS
     */
    struct {
        streamer_cmd_type_t type;   ///< The type of the command = STREAMER_CMD_SET_OPT
        streamer_status_t status;   ///< The status to set
        uint8_t value;              ///< 1 if stream handler is in this state, 0 if not.
    } set_status;
} streamer_cmd_t;

/**
 * @brief   Definition of command results for stream handlers.
 */
typedef enum __attribute__((packed)) {
    STREAMER_RESULT_ERROR,          ///< Result for error cases.
    STREAMER_RESULT_REGISTER,       ///< Result for STREAMER_CMD_REGISTER.
    STREAMER_RESULT_WRITE,          ///< Result for STREAMER_CMD_WRITE
    STREAMER_RESULT_READ,           ///< Result for STREAMER_CMD_READ
    STREAMER_RESULT_WRITE_TO,       ///< Result for STREAMER_CMD_WRITE_TO
    STREAMER_RESULT_READ_FROM,      ///< Result for STREAMER_CMD_READ_FROM
    STREAMER_RESULT_GET_OPT,        ///< Result for STREAMER_CMD_GET_OPT
    STREAMER_RESULT_SET_OPT,        ///< Result for STREAMER_CMD_SET_OPT
    STREAMER_RESULT_GET_STATUS,     ///< Result for STREAMER_CMD_GET_STATUS
    STREAMER_RESULT_SET_STATUS,     ///< Result for STREAMER_CMD_SET_STATUS
} streamer_result_type_t;

/**
 * @brief   Definion of command results for stream handlers.
 */
typedef struct __attribute__((packed)) {
    streamer_result_type_t type;    ///< The result type.

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_ERROR
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_ERROR.
        streamer_cmd_type_t cmd;        ///< The command which caused this error.
        int nr;                         ///< The errno of this error.
    } error;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_REGISTER
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_REGISTER.
        int pid;                        ///< The PID of the thread that is now registered to it.
    } reg;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_WRITE
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_WRITE.
        size_t bytes;                   ///< The number of bytes written.
    } write;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_READ
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_READ.
        size_t bytes;                   ///< The number of bytes read.
    } read;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_WRITE_TO
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_WRITE_TO.
        size_t bytes;                   ///< The number of bytes written.
    } write_to;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_READ_FROM
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_READ_FROM.
        size_t bytes;                   ///< The number of bytes read.
        streamer_addr_t *addr;          ///< The address the buffer was read from.
    } read_from;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_GET_OPT
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_GET_OPT.
        streamer_opt_t opt;             ///< The option this result is for.
        void *value;                    ///< The value of the option
    } get_opt;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_SET_OPT
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_SET_OPT.
        streamer_opt_t opt;             ///< The option you setted.
    } set_opt;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_GET_STATUS
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_GET_STATUS.
        streamer_status_t status;       ///< The status this result is for.
        uint8_t value;                  ///< 1 if stream handler is in this state, 0 if not.
    } get_status;

    /**
     * @brief   Definition of result subtype for STREAMER_CMD_SET_STATUS
     */
    struct {
        streamer_result_type_t type;    ///< The result type = STREAMER_RESULT_SET_STATUS.
        streamer_status_t status;       ///< The status want to set.
    } set_status;
}
streamer_result_t;

/**
 * @brief   Send a command to a stream handler and reseive an answer in *msg*.
 *
 * @param[in]       streamer    The PID of the stream handler.
 * @param[in]       cmd         The command to send to  the stream handler.
 * @param[in,out]   msg         The msg_t to use as a sending device and also
 *                              to receive.
 */
static inline void streamer_send_receive_msg(int streamer,
        const streamer_cmd_t *cmd,
        msg_t *msg)
{
    msg->type = STREAMER_MSG_TYPE_CMD;
    msg->content.ptr = (char *) cmd;
    msg_send_receive(msg, msg, streamer);
}

/**
 * @brief   Checks a received stream handler's answer for errors and returns
 *          its errno if there is one, otherwise 0.
 *
 * @param[in]   msg The answer froma a stream handler.
 * @param[in]   cmd The expected command who could produce the suspected error.
 *
 * @return  0 if no error occured, EBADMSG if the checked command was not *cmd*,
 *          ENOSYS if *msg* is not of type STREAMER_MSG_TYPE_RESULT.
 */
static inline int streamer_check_error(const msg_t *msg, streamer_cmd_type_t cmd)
{
    if (msg->type == STREAMER_MSG_TYPE_RESULT) {
        if (((streamer_result_t *)msg->content.ptr)->type == STREAMER_RESULT_ERROR &&
            ((streamer_result_t *)msg->content.ptr)->error.cmd == cmd) {
            return ((streamer_result_t *)msg->content.ptr)->error.nr;
        }
        else if (((streamer_result_t *)msg->content.ptr)->type == STREAMER_RESULT_ERROR) {
            return EBADMSG;
        }

        return 0;
    }
    else {
        return ENOSYS;
    }
}


/**
 * @brief   Register a thread for events from a stream handler's thread.
 *          Wrapper for STREAMER_CMD_REG
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   pid         The registrant's pid.
 *
 * @return  The registant's PID on success, 0 in case of an error. *errno* is
 *          set accordingly.
 */
static inline int streamer_register(int streamer, int pid)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.reg.type = STREAMER_CMD_REG;
    cmd.reg.pid = pid;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_REG);

    if (errno) {
        return 0;
    }

    return ((streamer_result_t *)msg.content.ptr)->reg.pid;
}

/**
 * @brief   Write to stream handler. Wrapper for STREAMER_CMD_WRITE
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   buf         A buffer
 * @param[in]   buf_len     The length of the buffer
 *
 * @return Number of bytes written on success, negative *errno* in case of an error.
 */
static inline int streamer_write(int streamer, const void *buf, size_t buf_len)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.write.type = STREAMER_CMD_WRITE;
    cmd.write.buf.data = (void *)buf;
    cmd.write.buf.data_len = buf_len;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_WRITE);

    if (errno) {
        return -errno;
    }

    return ((streamer_result_t *)msg.content.ptr)->write.bytes;
}

/**
 * @brief   Read to stream handler. Wrapper for STREAMER_CMD_READ
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[out]  buf         A buffer
 * @param[in]   buf_len     The length of the buffer
 *
 * @return Number of bytes read on success, negative *errno* in case of an error.
 */
static inline int streamer_read(int streamer, void *buf, size_t buf_len)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.read.type = STREAMER_CMD_READ;
    cmd.read.buf.data = buf;
    cmd.read.buf.data_len = buf_len;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_READ);

    if (errno) {
        return -errno;
    }

    return ((streamer_result_t *)msg.content.ptr)->read.bytes;
}

/**
 * @brief   Write to an address via stream handler.
 *          Wrapper for STREAMER_CMD_WRITE_TO
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   buf         A buffer
 * @param[in]   buf_len     The length of the buffer
 * @param[in]   addr        The address to write to
 *
 * @return Number of bytes written on success, negative *errno* in case of an error.
 */
static inline int streamer_write_to(int streamer, const void *buf, size_t buf_len,
                                    const streamer_addr_t *addr)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.write_to.type = STREAMER_CMD_WRITE_TO;
    cmd.write_to.buf.data = (void *)buf;
    cmd.write_to.buf.data_len = buf_len;
    cmd.write_to.addr = (streamer_addr_t *)addr;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_WRITE_TO);

    if (errno) {
        return -errno;
    }

    return ((streamer_result_t *)msg.content.ptr)->write_to.bytes;
}

/**
 * @brief   Read from an address via stream handler.
 *          Wrapper for STREAMER_CMD_READ_FROM
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[out]  buf         A buffer
 * @param[in]   buf_len     The length of the buffer
 * @param[out]  addr        The address the data was read from to
 *
 * @return Number of bytes read on success, negative *errno* in case of an error.
 */
static inline int streamer_read_from(int streamer, void *buf, size_t buf_len,
                                     streamer_addr_t *addr)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.read.type = STREAMER_CMD_READ_FROM;
    cmd.read.buf.data = buf;
    cmd.read.buf.data_len = buf_len;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_READ_FROM);

    if (errno) {
        return -errno;
    }

    memcpy(addr, ((streamer_result_t *)msg.content.ptr)->read_from.addr,
           sizeof(streamer_addr_t));

    return ((streamer_result_t *)msg.content.ptr)->read_from.bytes;
}

/**
 * @brief    Get option for stream handler. Wrapper for STREAMER_CMD_GET_OPT.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   opt         The option to get.
 *
 * @return The value of option *opt* or NULL on error. *errno* is set accordingly.
 */
static inline void *streamer_get_opt(int streamer, streamer_opt_t opt,
                                     void *opt_parameters)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.get_opt.type = STREAMER_CMD_GET_OPT;
    cmd.get_opt.opt = opt;
    cmd.get_opt.parameters = opt_parameters;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_GET_OPT);

    if (errno) {
        return NULL;
    }

    return ((streamer_result_t *)msg.content.ptr)->get_opt.value;
}

/**
 * @brief    Get option for stream handler. Wrapper for STREAMER_CMD_SET_OPT.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   opt         The option to set.
 * @param[in]   value       The value to set *option* to.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_set_opt(int streamer, streamer_opt_t opt,
                                   const void *value)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.set_opt.type = STREAMER_CMD_SET_OPT;
    cmd.set_opt.opt = opt;
    cmd.set_opt.value = (void *)value;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_SET_OPT);

    if (errno) {
        return -errno;
    }

    return 0;
}

/**
 * @brief    Get status for stream handler. Wrapper for STREAMER_CMD_GET_STATUS.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   status      The status to get.
 *
 * @return  1 if status is set, 0 if not.
 */
static inline uint8_t streamer_get_status(int streamer,
        streamer_status_t status)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.get_status.type = STREAMER_CMD_GET_STATUS;
    cmd.get_status.status = status;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_GET_STATUS);

    if (errno) {
        return 0;
    }

    return ((streamer_result_t *)msg.content.ptr)->get_status.value;
}

/**
 * @brief    Activate staus for stream handler. Wrapper for STREAMER_CMD_SET_STATUS.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   status      The status to activate.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_set_status(int streamer,
                                      streamer_status_t status)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.set_status.type = STREAMER_CMD_SET_OPT;
    cmd.set_status.status = status;
    cmd.set_status.value = 1;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_SET_STATUS);

    if (errno) {
        return -errno;
    }

    return 0;
}

/**
 * @brief    Activate staus for stream handler. Wrapper for STREAMER_CMD_SET_STATUS.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   status      The status to activate.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_unset_status(int streamer, streamer_status_t status)
{
    msg_t msg;
    streamer_cmd_t cmd;

    cmd.set_status.type = STREAMER_CMD_SET_OPT;
    cmd.set_status.status = status;
    cmd.set_status.value = 0;

    streamer_send_receive_msg(streamer, &cmd, &msg);
    errno = streamer_check_error(&msg, STREAMER_CMD_SET_STATUS);

    if (errno) {
        return -errno;
    }

    return 0;
}

/**
 * @brief   Get the preferred address length of a stream handler
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  The preferred address length of the stream handler
 */
static inline streamer_addr_len_t streamer_get_pref_addr_len(int streamer)
{
    return *((streamer_addr_len_t *)streamer_get_opt(streamer,
            STREAMER_OPT_PREF_ADDR_LEN, NULL));
}

/**
 * @brief   Set the preferred address length of a stream handler
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   addr_len    The preferred address length to set.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_set_pref_addr_len(int streamer,
        streamer_addr_len_t addr_len)
{
    return streamer_set_opt(streamer, STREAMER_OPT_PREF_ADDR_LEN, &addr_len);
}

/**
 * @brief   Get all addresses associated to the stream handler by their length.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   addr_len    The address length to get.
 *
 * @return  List of all addresses associated to the stream handler of the given
 *          length.
 */
static inline streamer_addr_list_t *streamer_get_addresses_by_len(int streamer,
        streamer_addr_len_t addr_len)
{
    return (streamer_addr_list_t *)streamer_get_opt(streamer, STREAMER_OPT_ADDR,
                                                    &addr_len);
}

/**
 * @brief   Get (the first) address associated to the stream handler by its length.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   addr_len    The address length to get.
 *
 * @return  (The first) address associated to the stream handler of the given
 *          length.
 */
static inline streamer_addr_t *streamer_get_address_by_len(int streamer,
                                                           streamer_addr_len_t addr_len)
{
    streamer_addr_list_t *addr_list;

    if ((addr_list = streamer_get_addresses_by_len(streamer, addr_len))) {
        return addr_list->addr;
    }
    else {
        return NULL;
    }
}

/**
 * @brief   Get all addresses associated to the stream handler.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  List of all addresses associated to the stream handler.
 */
static inline streamer_addr_list_t *streamer_get_addresses(int streamer)
{
    return (streamer_addr_list_t *)streamer_get_address_by_len(streamer,
            streamer_get_pref_addr_len(streamer));
}

/**
 * @brief   Get (the first) address associated to the stream handler.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  (The first) addresses associated to the stream handler.
 */
static inline streamer_addr_t *streamer_get_address(int streamer)
{
    return streamer_get_address_by_len(streamer,
                                       streamer_get_pref_addr_len(streamer));
}

/**
 * @brief   Add addresses to the stream handler.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   addresses   The addresses to add.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_add_addresses(int streamer,
                                         const streamer_addr_list_t *addresses)
{
    return streamer_set_opt(streamer, STREAMER_OPT_ADDR, addresses);
}

/**
 * @brief   Delete addresses to the stream handler.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   addresses   The addresses to delete.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_del_addresses(int streamer,
                                         const streamer_addr_list_t *addresses)
{
    return streamer_set_opt(streamer, STREAMER_OPT_DEL_ADDR, addresses);
}

/**
 * @brief   Add an address to the stream handler.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   address     The address to add.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_add_address(int streamer, const void *addr_data,
                                       streamer_addr_len_t addr_len)
{
    streamer_addr_list_t addr_list;
    streamer_addr_t addr;

    addr.addr_len = addr_len;

    addr_list.next = NULL;
    addr_list.addr = &addr;

    return streamer_add_addresses(streamer, &addr_list);
}

/**
 * @brief   Delete an address to the stream handler.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   address     The address to delete.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_del_address(int streamer, const void *addr_data,
                                       streamer_addr_len_t addr_len)
{
    streamer_addr_list_t addr_list;
    streamer_addr_t addr;

    addr.addr_len = addr_len;

    addr_list.next = NULL;
    addr_list.addr = &addr;

    return streamer_del_addresses(streamer, &addr_list);
}

/**
 * @brief   Get radio channel associated to the stream handler
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  The radio channel associated to the stream handler.
 *          *errno* is set to 0 in case of success, otherwise
 *          it is set accordingly.
 */
static inline streamer_channel_t streamer_get_channel(int streamer)
{
    return *((streamer_channel_t *)streamer_get_opt(streamer,
                                                    STREAMER_OPT_CHANNEL,
                                                    NULL));
}

/**
 * @brief   Set radio channel for the stream handler
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   channel     The new channel for the stream handler.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_set_channel(int streamer, streamer_channel_t channel)
{
    return streamer_set_opt(streamer, STREAMER_OPT_CHANNEL, &channel);
}

/**
 * @brief   Get PAN ID associated to the stream handler
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  The PAN ID associated to the stream handler.
 *          *errno* is set to 0 in case of success, otherwise
 *          it is set accordingly.
 */
static inline streamer_pan_id_t streamer_get_pan_id(int streamer)
{
    return *((streamer_pan_id_t *)streamer_get_opt(streamer,
                                                   STREAMER_OPT_PAN_ID,
                                                   NULL));
}

/**
 * @brief   Set PAN ID for the stream handler
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   channel     The new PAN ID for the stream handler.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_set_pan_id(int streamer, streamer_pan_id_t pan_id)
{
    return streamer_set_opt(streamer, STREAMER_OPT_PAN_ID, &pan_id);
}

/**
 * @brief   Get maximum fragment length the stream handler supports.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  The maximum fragment length the stream handler supports.
 *          *errno* is set to 0 in case of success, otherwise
 *          it is set accordingly.
 */
static inline size_t streamer_get_max_frag_len(int streamer)
{
    return *((size_t *)streamer_get_opt(streamer, STREAMER_OPT_MAX_FRAG_LEN,
                                        NULL));
}

/**
 * @brief   Get supported address lengths the stream handler supports.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  List of supported address length of the stream handler on success,
 *          NULL otherwise.
 *          *errno* is set to 0 in case of success, otherwise
 *          it is set accordingly.
 */
static inline streamer_addr_len_list_t *streamer_get_supp_addr_len(int streamer)
{
    return (streamer_addr_len_list_t *)streamer_get_opt(streamer,
                                                        STREAMER_OPT_SUPP_ADDR_LEN,
                                                        NULL);
}

/**
 * @brief   Get addresses the stream handler ignores.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  List of addresses the stream handler ignores on success,
 *          NULL otherwise.
 *          *errno* is set to 0 in case of success, otherwise
 *          it is set accordingly.
 */
static inline streamer_addr_list_t *streamer_get_addr_ign(int streamer)
{
    return (streamer_addr_list_t *)streamer_get_opt(streamer,
                                                    STREAMER_OPT_ADDR_IGN,
                                                    NULL);
}

/**
 * @brief   Add addresses the stream handler should ignore.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   addrs       Addresses the stream handler should ignore.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_add_addr_ign(int streamer,
                                        streamer_addr_list_t *addrs)
{
    return streamer_set_opt(streamer, STREAMER_OPT_ADDR_IGN, addrs);
}

/**
 * @brief   Delete addresses from the stream handler ignore list.
 *
 * @param[in]   streamer    The PID of the stream handler.
 * @param[in]   addrs       Addresses the stream handler should no longter
 *                          ignore.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_del_addr_ign(int streamer,
                                        streamer_addr_list_t *addrs)
{
    return streamer_set_opt(streamer, STREAMER_OPT_ADDR_IGN_DEL, addrs);
}

/**
 * @brief   Get the stream handlers transceiving state.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  1, if the stream handler is receiving, 0 if not.
 */
static inline int streamer_get_transceiving_state(int streamer)
{
    return streamer_get_status(streamer, STREAMER_STATUS_RECEIVING);
}

/**
 * @brief   Set the stream handlers to receiving.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_set_receiving(int streamer)
{
    return streamer_set_status(streamer, STREAMER_STATUS_RECEIVING);
}

/**
 * @brief   Set the stream handlers to not receiving.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_set_transmitting(int streamer)
{
    return streamer_unset_status(streamer, STREAMER_STATUS_RECEIVING);
}

/**
 * @brief   Get the stream handlers power state.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  1, if the stream handler is powered down, 0 if not.
 */
static inline int streamer_get_power_state(int streamer)
{
    return streamer_get_status(streamer, STREAMER_STATUS_POWERDOWN);
}

/**
 * @brief   Power down the stream handlers.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_powerdown(int streamer)
{
    return streamer_set_status(streamer, STREAMER_STATUS_POWERDOWN);
}

/**
 * @brief   Power up the stream handlers.
 *
 * @param[in]   streamer    The PID of the stream handler.
 *
 * @return  0 on success, negative *errno* on failure.
 */
static inline int streamer_set_powerup(int streamer)
{
    return streamer_unset_status(streamer, STREAMER_STATUS_POWERDOWN);
}

#endif /* __STREAMER_H */
/**
 * @}
 */
