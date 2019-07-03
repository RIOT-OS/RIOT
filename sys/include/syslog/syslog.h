/*
 * Copyright (C) 2019 Robin Lösch <robin@chilio.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_syslog Syslog Implementation
 * @ingroup     sys
 * @brief       Generic syslog implementation inspired by glibc
 *
 * This module is a generic syslog implementation. It reflects the glibc implementation of the syslog.h function family.
 * This module uses RIOT IPC functionality to transmit logs to a syslogd instance, instead of using UNIX domain sockets as glibc's implementation.
 *
 * The implementation is designed to use only static memory, by default the assumptions on the number of clients and the maximum length of a single message are rather conservative.
 * By default only a single client is used with a maximum length of 1024 byte for a single syslog message.
 * To increase this values set SYSLOG_MAX_LEN and SYSLOG_CLIENTS_NUMOF to higher values.
 * The openlog implementation has a complexity of O(n) where n is equal to SYSLOG_CLIENTS_NUMOF.
 *
 * A client is a single RIOT thread with a given PID. A call to openlog() uses a pool of clients.
 * A new client (the first call to openlog by a specific thread) uses a static buffer in this pool.
 * The client is addressed by its PID. Therefore, multiple calls to openlog from the same thread results in only a single client usage from the pool.
 *
 * @{
 * @file
 * @brief       Header definitions used by syslog
 *
 * @author      Robin Lösch <robin@chilio.net>
 */
#ifndef SYSLOG_SYSLOG_H
#define SYSLOG_SYSLOG_H

#include <stdarg.h>
#include "thread.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum Length of a syslog identity as defined in APP_NAME https://tools.ietf.org/html/rfc5424#section-6 */
#define IDENT_MAX_LEN (48)

/** Maximum Length of a single syslog message. Longer messages are truncated. Can be overwritten by the user */
#ifndef SYSLOG_MAX_LEN
  #define SYSLOG_MAX_LEN (1024)
#endif

/** Version of the used syslog protocol as defined in https://tools.ietf.org/html/rfc5424#section-6.2.2 */
#define SYSLOG_VERSION (1)

/** Maximum number of syslog clients. A client is a thread identified by its PID. If more than SYSLOG_CLIENTS_NUMOF threads issue a call to openlog it wont succeed */
#ifndef SYSLOG_CLIENTS_NUMOF
  #define SYSLOG_CLIENTS_NUMOF (5)
#endif

/** Set the priority mask up to a priority x. LOG_UPTO(LOG_DEBUG) include all priorities */
#ifndef LOG_UPTO
  #define LOG_UPTO(x) ((1 << ((x)+1)) - 1)
#endif

/** Macro to set all bits in a log mask (e. g. enable logging of all priorities) */
#define LOG_MASK_ALL (LOG_UPTO(SYSLOG_DEBUG))

/** Macro to set corresponding bit for a priority in a log bitmask */
#ifndef LOG_MASK
  #define LOG_MASK(x) (1<<x)
#endif

/** Test if a given option bit (x) in the option bitmask (mask) is set */
#ifndef OPT_MASK_TEST
  #define OPT_MASK_TEST(mask,x) ((mask & x) == x)
#endif

/** Test if a given priority bit (x) in the priority bitmask (mask) is set */
#ifndef LOG_MASK_TEST
  #define LOG_MASK_TEST(mask,x) OPT_MASK_TEST(mask,LOG_MASK(x))
#endif

/** Syslog NILVALUE according to https://tools.ietf.org/html/rfc5424#section-6 */
#define SYSLOG_NILVALUE "-"

/**
 * @brief Possible syslog priorities as used in the glibc implementation
 */
enum syslog_priority
{
    SYSLOG_EMERG = 0,
    SYSLOG_ALERT = 1,
    SYSLOG_CRIT = 2,
    SYSLOG_ERR = 3,
    SYSLOG_WARNING = 4,
    SYSLOG_NOTICE = 5,
    SYSLOG_INFO = 6,
    SYSLOG_DEBUG = 7,
};

/**
 * @brief Syslog options for openlog() calls
 *
 * @see openlog
 *
 * This options can be set for each syslog client when calling openlog. Bitwise OR can be used to set multiple options
 */
enum syslog_options
{
    LOG_PID    = 0x01, /**< Include the PID of the logging proccess in every log message. The identity part is used as follows: "ident[pid]" */
    LOG_CONS   = 0x02, /**< Log to system console (e. g. printf) if no syslogd pid is available */
    LOG_ODELAY = 0x04, /**< Currently not implemented. @see https://linux.die.net/man/3/syslog for details */
    LOG_NDELAY = 0x08, /**< Currently not implemented. @see https://linux.die.net/man/3/syslog for details */
    LOG_NOWAIT = 0x10, /**< Currently not implemented. @see https://linux.die.net/man/3/syslog for details */
    LOG_PERROR = 0x20, /**< Log output on stderr in addition to the system logger. */
};

/**
 * @brief Wrapper structure for IPC transfer
 */
typedef struct syslog_msg {
    size_t len; /**< Length of the message */
    char *ptr;  /**< Pointer to the actual syslog message string */
} syslog_msg_t;

/**
 * @brief Description of a syslog client
 *
 * A pool of SYSLOG_CLIENTS_NUMOF clients can be used simultaneously. A client is considered as a single thread and is identified by its PID.
 *
 */
typedef struct syslog_client {
    kernel_pid_t pid;           /**< The PID of the logging thread */
    int facility;               /**< The facility as used in openlog */
    int mask;                   /**< The mask which selects which messages actually gets logged. Defaults to all priorities */
    int opt;                    /**< Options as set in the last call of openlog */
    char ident[IDENT_MAX_LEN];  /**< Identity of the calling thread (normally a threads name) */
    char buf[SYSLOG_MAX_LEN];   /**< Proccess specific buffer for syslog messages */
    syslog_msg_t msg;           /**< Includes the length and a pointer to buf. A pointer to this struct is transferred via IPC to the system logger */
} syslog_client_t;


/**
 * @brief Auto init function executed on RIOT startup
 *
 * This function is executed by RIOT's auto_init() function on startup.
 * It is used to setup data stuctures and hooks needed by syslog.
 * If the log_mqueue modules is used for example, this function injects a syslog wrapper into all facility callbacks except facility 0
 */
void auto_init_syslog(void);

/**
 * @brief Open a connection to the system logger
 *
 * A call to this function register a client (e. g. a thread) in the system logger. This creates a struct for a syslog client.
 * Max @see SYSLOG_CLIENTS_NUMOF are possible. Multiple calls to this function from the same thread overwrite the options from the first call.
 *
 * For possible options and their implementation state see syslog_options.
 * Possible facility values are documented in @see syslog_facility
 *
 * This implementation is intended to work as documented in https://www.gnu.org/software/libc/manual/html_node/openlog.html#openlog
 *
 * @param[in] ident     Identity of a logging proccess. Is included in every syslog message
 * @param[in] option    ORed List of syslog_options
 * @param[in] facility  facility value of the logging proccess
 */
void openlog(const char *ident, int option, int facility);

/**
 * @brief Send a syslog message to the system logger
 *
 * This function implicitly calls vsyslog after the format string is parsed into a stdargs argument list
 *
 * @param[in] facility_priority Priority value @see syslog_priority for a detailed list of possible values
 * @param[in] format Format string which gets traversed using stdarg argument list
 */
void syslog (int facility_priority, const char *format, ...);

/**
 * @brief Send a syslog message to the system logger
 *
 * @param[in] facility_priority Priority value @see syslog_priority for a detailed list of possible values
 * @param[in] format Format string
 * @param[in] ap stdarg argument list as defined in stdarg.h. Gets applied to the format string
 */
void vsyslog(int facility_priority, const char *format, va_list ap);

/**
 * @brief Close a connection to the system logger
 *
 * Resets a client struct and release it back to the pool.
 */
void closelog(void);

/**
 * @brief Set the logmask to select which messages gets logged to the system logger
 *
 * @param[in] mask Mask which is checked on every call to syslog(). Can be constructed by applieing LOG_MASK(syslog_priority).
 * Multiple values can be included by using a logical OR e. g. (LOG_MASK(LOG_INFO) | LOG_MASK(LOG_ERR)) etc.
 *
 * @return int The old mask value
 */
int setlogmask(int mask);

#ifdef __cplusplus
}
#endif
/** @} */

#endif /* SYSLOG_SYSLOG_H */
