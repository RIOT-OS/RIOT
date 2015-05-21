/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    nhdp NHDP
 * @ingroup     net
 * @brief       The MANET Neighborhood Discovery Protocol (RFC 6130)
 * @{
 *
 * @file
 * @brief       Interface for core functionality of NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef NHDP_H_
#define NHDP_H_

#include "timex.h"
#include "kernel_types.h"
#include "socket_base/socket.h"

#include "rfc5444/rfc5444_writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Definition for RIOTs debugging option */
#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @name    NHDP protocol macros
 *
 * @{
 */
#ifndef MANET_PORT
/** @brief Well-known MANET port from RFC 5498 */
#define MANET_PORT                  (269)
#endif

/** @brief Stack size for NHDP thread */
#if ENABLE_DEBUG
#define NHDP_STACK_SIZE      (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#else
#define NHDP_STACK_SIZE      (THREAD_STACKSIZE_DEFAULT)
#endif

/** @brief Buffer size in bytes for NHDP writer's msg buffer */
#define NHDP_WR_MSG_BUF_SIZE        (256)
/** @brief Buffer size in bytes for NHDP writer's tlv buffer */
#define NHDP_WR_TLV_BUF_SIZE        (512)

/* TODO: Determine a good value */
/** @brief Queue size for msg queue of NHDP */
#define NHDP_MSG_QUEUE_SIZE         (16)

/** @brief Maximum size of handled RFC5444 packets */
#define NHDP_MAX_RFC5444_PACKET_SZ  (128)

/** @brief Default hello interval in milliseconds */
#define NHDP_DEFAULT_HELLO_INT_MS   (2000)
/** @brief Default hold time in milliseconds */
#define NHDP_DEFAULT_HOLD_TIME_MS   (3 * NHDP_DEFAULT_HELLO_INT_MS)

/**
 * @brief   Initial pending flag value for new link tuples
 *
 *          Do not change (link quality currently not considered)
 */
#define NHDP_INITIAL_PENDING        (0)

/** @brief Maximum jitter for nhdp messages in milliseconds */
#define NHDP_HP_MAXJITTER_MS        (200)

#define NHDP_L_HOLD_TIME_MS         (NHDP_DEFAULT_HOLD_TIME_MS)
#define NHDP_N_HOLD_TIME_MS         (NHDP_DEFAULT_HOLD_TIME_MS)
#define NHDP_I_HOLD_TIME_MS         (NHDP_DEFAULT_HOLD_TIME_MS)
/** @} */

/**
 * @brief   MANET interface representation
 */
typedef struct nhdp_if_entry_t {
    kernel_pid_t if_pid;                        /**< PID of the interface's handling thread */
    vtimer_t if_timer;                          /**< Vtimer used for the periodic signaling */
    timex_t hello_interval;                     /**< Interval time for periodic HELLOs */
    timex_t validity_time;                      /**< Validity time for propagated information */
    uint16_t seq_no;                            /**< Sequence number of last send RFC5444 packet */
    struct rfc5444_writer_target wr_target;     /**< Interface specific writer target */
} nhdp_if_entry_t;

/**
 * @brief   Additional address types for link layer operation
 */
enum nhdp_address_type_t {
    AF_CC110X = AF_MAX + 3,
};

/**
 * @brief                   Initialize NHDP for operation
 *
 * Sets up NHDP's reader and writer. Call first before starting NHDP's thread
 * and registering interfaces.
 */
void nhdp_init(void);

/**
 * @brief                   Start NHDP's operational thread
 *
 * @return                  PID of NHDP's operational thread
 * @return                  KERNEL_PID_UNDEF on error
 */
kernel_pid_t nhdp_start(void);

/**
 * @brief                   Register an interface for NHDP operation with default values
 *
 * Registers the interface completely for NHDP operation. Registration includes a new
 * Local Information Base entry, a new Interface Information Base and starting
 * the periodical HELLO messaging.
 *
 * @note
 * Default values are a hello interval of 2 seconds and a validity time
 * of 6 seconds for propagated information.
 *
 * @param[in] if_pid        PID of the interface
 * @param[in] addr          A local address of this interface represented in bytes
 * @param[in] addr_size     Length in bytes of the local address
 * @param[in] addr_type     AF type of the local address
 * @param[in] max_pl_size   Maximum payload size for packets send over this interface
 *
 * @return                  0 on success
 * @return                  -1 on error
 * @return                  -2 on maximum number of interfaces registered
 */
int nhdp_register_if_default(kernel_pid_t if_pid, uint8_t *addr, size_t addr_size,
                             uint8_t addr_type, uint16_t max_pl_size);

/**
 * @brief                   Register an interface for NHDP operation
 *
 * Registers the interface completely for NHDP operation. Registration includes a new
 * Local Information Base entry, a new Interface Information Base and starting
 * the periodical HELLO messaging.
 *
 * @param[in] if_pid        PID of the interface
 * @param[in] addr          A local address of this interface represented in bytes
 * @param[in] addr_size     Length of the local address (number of bytes)
 * @param[in] addr_type     AF type of the given address
 * @param[in] max_pl_size   Maximum payload size for packets send over this interface
 * @param[in] hello_int_ms  Hello interval in ms for periodic message generation
 * @param[in] val_time_ms   Validity time in ms for propagated information
 *
 * @return                  0 on success
 * @return                  -1 on error
 * @return                  -2 on maximum number of interfaces registered
 */
int nhdp_register_if(kernel_pid_t if_pid, uint8_t *addr, size_t addr_size, uint8_t addr_type,
                     uint16_t max_pl_size, uint16_t hello_int_ms, uint16_t val_time_ms);

/**
 * @brief                   Register a non MANET interface in NHDP
 *
 * The registered interface does not take part in NHDP operation. Its addresses
 * will be represented only in other interfaces' messages. No periodical messages
 * are created for this interface and no message processing is done.
 *
 * @param[in] if_pid        PID of the interface
 * @param[in] addr          A local address of this interface represented in bytes
 * @param[in] addr_size     Length of the local address (number of bytes)
 * @param[in] addr_type     AF type of the given address
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int nhdp_register_non_manet_if(kernel_pid_t if_pid, uint8_t *addr, size_t addr_size,
                               uint8_t addr_type);

/**
 * @brief                   Register an additional address for an interface in NHDP
 *
 * The address is added to the Local Information Base entry of this interface and
 * will afterwards be propagated as a local address in newly created HELLO messages.
 *
 * @param[in] if_pid        PID of the interface
 * @param[in] addr          Additional local address of this interface represented in bytes
 * @param[in] addr_size     Length of the local address (number of bytes)
 * @param[in] addr_type     AF type of the given address
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int nhdp_add_address(kernel_pid_t if_pid, uint8_t *addr, size_t addr_size, uint8_t addr_type);

#ifdef __cplusplus
}
#endif

#endif /* NHDP_H_ */
/** @} */
