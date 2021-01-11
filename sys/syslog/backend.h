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

#ifndef SYSLOG_BACKEND_H
#define SYSLOG_BACKEND_H

#include <stdint.h>
#include <stdatomic.h>

#include "thread.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_SYSLOG_MAX_STRING_LEN
#define CONFIG_SYSLOG_MAX_STRING_LEN    256
#endif

struct syslog_msg {
    /* metadata */
    uint8_t pri;
    struct tm time;
    bool time_set;
    const char *app_name;
    kernel_pid_t proc_id;
    /* msg */
    char msg[CONFIG_SYSLOG_MAX_STRING_LEN];
    size_t len;
    /* internal */
    atomic_int cnt;
};

/**
 * @brief   syslog backend driver
 */
typedef struct {
    /**
     * @brief   syslog backend init
     */
    int (*init)(void);
    /**
     * @brief   syslog backend send function
     *
     * @param   msg     message to send
     */
    void (*send)(struct syslog_msg *);
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
void syslog_msg_get(struct syslog_msg *msg);

/**
 * @brief   Decrement syslog message reference counter and free when null
 *
 * @param   msg     syslog message
 */
void syslog_msg_put(struct syslog_msg *msg);

static inline uint8_t syslog_get_prival(uint8_t facility, uint8_t priority)
{
    return (((facility) << 3) + (priority & 0x7));
}

static inline uint8_t syslog_get_priority(uint8_t prival)
{
    return prival & 0x7;
}

static inline uint8_t syslog_get_facility(uint8_t prival)
{
    return prival >> 3;
}

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* SYSLOG_BACKEND_H */
