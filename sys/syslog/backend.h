/*
 * Copyright (C) 2020 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_syslog
 *
 * @brief       Syslog backend definitions
 *
 * @{
 * @file
 *
 * @author      Vincent Dupont <vincent.dupont@continental-its.com>
 */

#ifndef BACKEND_H
#define BACKEND_H

#include <stdint.h>
#include <stdatomic.h>

#include "thread.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_SYSLOG_MAX_STRING_LEN
/**
 * @brief   Maximum length for a syslog message string
 */
#define CONFIG_SYSLOG_MAX_STRING_LEN    256
#endif

/**
 * @brief   Structure holding a datetime
 */
typedef struct {
  int tm_sec;   /**< Seconds. [0-60] */
  int tm_min;   /**< Minutes. [0-60] */
  int tm_hour;  /**< Hours.   [0-23] */
  int tm_mday;  /**< Day.     [1-31] */
  int tm_mon;   /**< Month.   [0-11] */
  int tm_year;  /**< Year - 1900     */
} tm_t;

typedef struct syslog_msg {
    /* metadata */
    kernel_pid_t proc_id;   /**< Message proc_id (thread PID) */
    uint8_t pri;            /**< Message priority */
    bool time_set;          /**< True if a time is set in @p time */
    tm_t time;              /**< Datetime info */
    const char *app_name;   /**< Message application name */
    /* msg */
    char msg[CONFIG_SYSLOG_MAX_STRING_LEN]; /**< Formatted message */
    size_t len;                             /**< Number of characters in @p msg, excluding null-termination */
    /* internal */
    atomic_int cnt;         /**< Internal reference counter */
} syslog_msg_t;

/**
 * @brief   syslog backend driver
 */
typedef struct {
    /**
     * @brief   syslog backend send function
     *
     * @param   msg     message to send
     */
    void (*send)(syslog_msg_t *);
} syslog_backend_t;

/**
 * @brief   syslog backends list
 */
extern const syslog_backend_t *syslog_backends[];

/**
 * @brief   number of syslog backends
 */
extern const size_t syslog_backends_numof;

/**
 * @brief   Increment syslog message reference counter
 *
 * @param   msg     syslog message
 */
void syslog_msg_get(syslog_msg_t *msg);

/**
 * @brief   Decrement syslog message reference counter and free when null
 *
 * @param   msg     syslog message
 */
void syslog_msg_put(syslog_msg_t *msg);

/**
 * @brief   Get rfc5424 prival from @p facility and @p priority
 *
 * @param[in]   facility    rfc5424 facility
 * @param[in]   priority    rfc5424 priority
 *
 * @return rfc5424 prival
 */
static inline uint8_t syslog_get_prival(uint8_t facility, uint8_t priority)
{
    return (((facility) << 3) + (priority & 0x7));
}

/**
 * @brief  Get priority from @p prival
 *
 * @param[in]   prival  prival
 *
 * @return priority
 */
static inline uint8_t syslog_get_priority(uint8_t prival)
{
    return prival & 0x7;
}

/**
 * @brief  Get facility from @p prival
 *
 * @param[in]   prival  prival
 *
 * @return facility
 */
static inline uint8_t syslog_get_facility(uint8_t prival)
{
    return prival >> 3;
}

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* BACKEND_H */
