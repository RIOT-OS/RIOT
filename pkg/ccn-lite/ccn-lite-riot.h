/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CCN_LITE_RIOT_H
#define CCN_LITE_RIOT_H

/**
 * @defgroup    pkg_ccnlite CCN-Lite stack
 * @ingroup     pkg
 * @brief       Provides a NDN implementation
 *
 * This package provides the CCN-Lite stack as a port of NDN for RIOT.
 *
 * @{
 */

/**
 * Use RIOT specific configuration in CCN-Lite
 */
#define CCNL_RIOT

#include <unistd.h>
#include "kernel_types.h"
#include "arpa/inet.h"
#include "net/packet.h"
#include "net/ethernet/hdr.h"
#include "sys/socket.h"
#include "ccnl-defs.h"
#include "ccnl-core.h"
#include "ccnl-headers.h"
#include "net/gnrc/netreg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the log level of the CCN-Lite stack
 */
#define LOG_LEVEL LOG_DEBUG
#include "log.h"

/**
 * @name Dynamic memory allocation used in CCN-Lite
 *
 * @{
 */
#define ccnl_malloc(s)                  malloc(s)
#define ccnl_calloc(n,s)                calloc(n,s)
#define ccnl_realloc(p,s)               realloc(p,s)
#define ccnl_free(p)                    free(p)
/**
 * @}
 */

/**
 * Closing an interface or socket from CCN-Lite
 */
#define ccnl_close_socket(s)            close(s)

/**
 * @name Log levels used by CCN-Lite debugging
 *
 * @{
 */
#define FATAL   LOG_ERROR
#define ERROR   LOG_ERROR
#define WARNING LOG_WARNING
#define INFO    LOG_INFO
#define DEBUG   LOG_DEBUG
#define TRACE   LOG_DEBUG
#define VERBOSE LOG_ALL
/**
 * @}
 */

/**
 * @name CCN-Lite's debugging macros
 *
 * @{
 */
#define DEBUGMSG(LVL, ...) do {       \
        if ((LVL)>debug_level) break;   \
        LOG(LVL, __VA_ARGS__);   \
    } while (0)

#define DEBUGMSG_CORE(...) DEBUGMSG(__VA_ARGS__)
#define DEBUGMSG_CFWD(...) DEBUGMSG(__VA_ARGS__)
#define DEBUGMSG_CUTL(...) DEBUGMSG(__VA_ARGS__)
#define DEBUGMSG_PIOT(...) DEBUGMSG(__VA_ARGS__)

#define DEBUGSTMT(LVL, ...) do { \
        if ((LVL)>debug_level) break; \
        __VA_ARGS__; \
     } while (0)

#define TRACEIN(...)                    do {} while(0)
#define TRACEOUT(...)                   do {} while(0)
/**
 * @}
 */

/**
 * Constant string
 */
#define CONSTSTR(s)                     s

/**
 * Stack size for CCN-Lite event loop
 */
#define CCNL_STACK_SIZE (THREAD_STACKSIZE_MAIN)

/**
 * Size of the message queue of CCN-Lite's event loop
 */
#define CCNL_QUEUE_SIZE     (8)

/**
 * Struct holding CCN-Lite's central relay information
 */
extern struct ccnl_relay_s theRelay;

/**
 * @brief   Start the main CCN-Lite event-loop
 *
 * @return  The PID of the event-loop's thread
 */
kernel_pid_t ccnl_start(void);

/**
 * @brief Opens a @ref net_gnrc_netif device for use with CCN-Lite
 *
 * @param[in] if_pid        The pid of the @ref net_gnrc_netif device driver
 * @param[in] netreg_type   The @ref net_gnrc_nettype @p if_pid should be
 *                          configured to use
 *
 * @return 0 on success,
 * @return -EINVAL if eventloop could not be registered for @p netreg_type
 */
int ccnl_open_netif(kernel_pid_t if_pid, gnrc_nettype_t netreg_type);

/**
 * @brief Sends out an Interest
 *
 * @param[in] suite     CCN packet format
 * @param[in] name      The name that is requested
 * @param[in] addr      The relay's address to send to
 * @param[in] addr_len  Length of @p addr
 * @param[in] chunknum  Number of the requested content chunk
 * @param[out] buf      Buffer to write the content chunk to
 * @param[in] buf_len   Size of @p buf
 *
 * @return 0 on successfully sent Interest
 * @return -1 if Interested couldn't be sent
 */
int ccnl_send_interest(int suite, char *name, uint8_t *addr, size_t addr_len,
                        unsigned int *chunknum, unsigned char *buf,
                        size_t buf_len);

/**
 * @brief Waits for incoming content chunk
 *
 * @return 0 if a content was received
 * @return -ETIMEDOUT if no chunk was received until timeout
 */
int ccnl_wait_for_chunk(void *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif
#endif /* CCN_LITE_RIOT_H */
/** @} */
