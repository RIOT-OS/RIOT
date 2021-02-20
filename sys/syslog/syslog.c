/*
 * Copyright (C) 2020 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_syslog
 * @{
 * @file
 * @brief   Syslog implementation
 * @author  Vincent Dupont <vincent.dupont@continental-its.com>
 * @}
 */

#include <assert.h>

#include "syslog.h"
#include "syslog_backend.h"
#include "formatter.h"
#include "memarray.h"
#include "mutex.h"
#include "periph/rtc.h"
#include "log.h"
#include "clist.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef SYSLOG_THREAD_PRIO
#define SYSLOG_THREAD_PRIO          (THREAD_PRIORITY_MIN - 1)
#endif
#ifndef SYSLOG_THREAD_STACKSIZE
#define SYSLOG_THREAD_STACKSIZE     THREAD_STACKSIZE_MEDIUM
#endif

#ifndef SYSLOG_MSG_QUEUE_SIZE
#define SYSLOG_MSG_QUEUE_SIZE       32
#endif

#if !CONFIG_SYSLOG_SET_DEFAULT_IDENT
#define CONFIG_SYSLOG_DEFAULT_IDENT RIOT_APPLICATION
#endif

#ifndef CONFIG_SYSLOG_MEMARRAY
#define CONFIG_SYSLOG_MEMARRAY      0
#endif

enum {
    SYSLOG_MSG_QUEUE = 1,
};

struct syslog_entry {
#if CONFIG_SYSLOG_HAS_OPEN
    clist_node_t node;
#endif
    const char *ident;
    int facility;
    int option;
    int mask;
};

#if CONFIG_SYSLOG_MEMARRAY
#ifndef CONFIG_SYSLOG_MAX_ENTRIES
#define CONFIG_SYSLOG_MAX_ENTRIES   8
#endif

#ifndef CONFIG_SYSLOG_MAX_MSG
#define CONFIG_SYSLOG_MAX_MSG       64
#endif

#if CONFIG_SYSLOG_HAS_OPEN
static memarray_t mem_entries;
static syslog_entry_t entries[CONFIG_SYSLOG_MAX_ENTRIES];
#endif

static memarray_t mem_msgs;
static struct syslog_msg msgs[CONFIG_SYSLOG_MAX_MSG];

static mutex_t lock = MUTEX_INIT;
#endif /* CONFIG_SYSLOG_MEMARRAY */

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _stack[SYSLOG_THREAD_STACKSIZE];

#ifndef CONFIG_SYSLOG_HOSTNAME
#define CONFIG_SYSLOG_HOSTNAME      "-"
#endif

static const char *hostname = CONFIG_SYSLOG_HOSTNAME;

syslog_entry_t *syslog_default_entry;

#if CONFIG_SYSLOG_HAS_OPEN
static clist_node_t _logs;
static mutex_t _logs_lock = MUTEX_INIT;
#else
static syslog_entry_t _syslog_default_entry;
#endif

#if CONFIG_SYSLOG_MEMARRAY
static struct syslog_msg *_alloc_msg(void)
{
    mutex_lock(&lock);
    struct syslog_msg *msg = memarray_alloc(&mem_msgs);
    mutex_unlock(&lock);
    if (msg) {
        memset(msg, 0, sizeof(*msg));
        msg->size = sizeof(msg->msg);
    }
    return msg;
}

static void _free_msg(struct syslog_msg *msg)
{
    if (msg == NULL) {
        return;
    }
    mutex_lock(&lock);
    memarray_free(&mem_msgs, msg);
    mutex_unlock(&lock);
}

#if CONFIG_SYSLOG_HAS_OPEN
static syslog_entry_t *_alloc_entry(void)
{
    mutex_lock(&lock);
    syslog_entry_t *entry = memarray_alloc(&mem_entries);
    mutex_unlock(&lock);

    return entry;
}

static void _free_entry(syslog_entry_t *entry)
{
    if (entry == NULL) {
        return;
    }
    mutex_lock(&lock);
    memarray_free(&mem_entries, entry);
    mutex_unlock(&lock);
}
#endif
#else
static struct syslog_msg *_alloc_msg(void)
{
    syslog_msg_t *msg = calloc(1, sizeof(struct syslog_msg));
    if (msg) {
        msg->size = sizeof(msg->msg);
    }
    return msg;
}

static void _free_msg(struct syslog_msg *msg)
{
    free(msg);
}

#if CONFIG_SYSLOG_HAS_OPEN
static syslog_entry_t *_alloc_entry(void)
{
    return malloc(sizeof(syslog_entry_t));
}

static void _free_entry(syslog_entry_t *entry)
{
    free(entry);
}
#endif
#endif

#if CONFIG_SYSLOG_HAS_OPEN
syslog_entry_t *openlog(const char *ident, int option, int facility)
{
    syslog_entry_t *entry = _alloc_entry();
    assert(entry);

    entry->ident = ident;
    entry->option = option;
    entry->facility = facility;
    entry->mask = LOG_LEVEL;

    mutex_lock(&_logs_lock);
    clist_rpush(&_logs, &entry->node);
    mutex_unlock(&_logs_lock);

    return entry;
}

void closelog(syslog_entry_t *entry)
{
    mutex_lock(&_logs_lock);
    clist_remove(&_logs, &entry->node);
    mutex_unlock(&_logs_lock);

    _free_entry(entry);
}
#endif

void syslog(syslog_entry_t *entry, int priority, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsyslog(entry, priority, format, ap);
    va_end(ap);
}

static void _set_time(struct syslog_msg *msg)
{
#if defined(MODULE_PERIPH_RTC)
    struct tm time;
    rtc_get_time(&time);
    msg->time.tm_hour = time.tm_hour;
    msg->time.tm_min = time.tm_min;
    msg->time.tm_sec = time.tm_sec;
    msg->time.tm_year = time.tm_year;
    msg->time.tm_mon = time.tm_mon;
    msg->time.tm_mday = time.tm_mday;
    msg->time_set = true;
#else
    msg->time_set = false;
#endif
}

static int _queue_msg(struct syslog_msg *msg)
{
    if (_pid == KERNEL_PID_UNDEF) {
        return -1;
    }
    msg_t _msg;
    _msg.content.ptr = msg;
    _msg.type = SYSLOG_MSG_QUEUE;
    if (msg_try_send(&_msg, _pid) == 1) {
        DEBUG("syslog: msg queued\n");
        return 0;
    }

    DEBUG("syslog: unable to queue msg %d\n", (int)_pid);
    return -1;
}

#if IS_USED(MODULE_SYSLOG_FORMATTER_DEFAULT)
ssize_t syslog_build_head(struct syslog_msg *msg, const char *hostname)
{
    ssize_t ret;
    if (msg->time_set) {
        ret = snprintf(msg->msg, msg->size, "<%d>%d " TIME_FMT " %s %s %" PRIkernel_pid " - - ",
                       msg->pri, 1, TIME_PARAMS(msg->time), hostname, msg->app_name, msg->proc_id);
    }
    else {
        ret = snprintf(msg->msg, msg->size, "<%d>%d - %s %s %d - - ",
                       msg->pri, 1, hostname, msg->app_name, msg->proc_id);
    }
    if (ret < 0) {
        return ret;
    }
    if (ret >= (ssize_t)msg->size) {
        ret = msg->size - 1;
    }
    msg->len = ret;
    return ret;
}
#endif

#if IS_ACTIVE(SYSLOG_FORMATTER_NEEDS_DEFAULT_MSG)
ssize_t syslog_build_msg(struct syslog_msg *msg, const char *fmt, va_list ap)
{
    size_t rem = msg->size - msg->len;
    ssize_t ret = vsnprintf(msg->msg + msg->len, rem, fmt, ap);
    if (ret < 0) {
        return ret;
    }
    if ((size_t)ret >= rem) {
        ret = rem - 1;
    }
    msg->len += ret;
    return ret;
}
#endif

#if IS_ACTIVE(SYSLOG_FORMATTER_NEEDS_DEFAULT_FOOT)
ssize_t syslog_build_foot(struct syslog_msg *msg)
{
    (void)msg;
    return 0;
}
#endif

void vsyslog(syslog_entry_t *entry, int priority, const char *format, va_list ap)
{
    if (entry == NULL) {
        return;
    }

    if (priority > entry->mask) {
        return;
    }

    struct syslog_msg *msg = _alloc_msg();
    if (!msg) {
        DEBUG("syslog: unable to alloc msg\n");
        return;
    }

    atomic_store(&msg->cnt, 0);
    msg->app_name = entry->ident;
    msg->pri = syslog_get_prival(entry->facility, priority);
    msg->proc_id = thread_getpid();
    _set_time(msg);

    /* Eventually this should be done after the message is queued to avoid overloading
     * the caller with the string formatting */
    ssize_t len = syslog_build_head(msg, hostname);
    if (len < 0) {
        goto exit_err;
    }
    len = syslog_build_msg(msg, format, ap);
    if (len < 0) {
        goto exit_err;
    }
    len = syslog_build_foot(msg);
    if (len < 0) {
        goto exit_err;
    }

    if (_queue_msg(msg) < 0) {
        goto exit_err;
    }

    return;

exit_err:
    _free_msg(msg);
}
void syslog_msg_get(struct syslog_msg *msg)
{
    atomic_fetch_add(&msg->cnt, 1);
}

void syslog_msg_put(struct syslog_msg *msg)
{
    if (atomic_fetch_sub(&msg->cnt, 1) == 1) {
        _free_msg(msg);
    }
}

static void _dispatch_msg(struct syslog_msg *msg)
{
    syslog_msg_get(msg);
    for (size_t i = 0; i < syslog_backends_numof; i++) {
        syslog_backends[i]->send(msg);
    }
    syslog_msg_put(msg);
}

static void *_syslog_thread(void *arg)
{
    (void)arg;

    msg_t queue[SYSLOG_MSG_QUEUE_SIZE];
    msg_init_queue(queue, ARRAY_SIZE(queue));

    msg_t msg;
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
        case SYSLOG_MSG_QUEUE:
            DEBUG("syslog: dispatching msg\n");
            _dispatch_msg(msg.content.ptr);
            break;
        default:
            DEBUG("syslog: unknown message received\n");
            break;
        }
    }

    return NULL;
}

void syslog_set_hostname(const char *name)
{
    hostname = name;
}

#if CONFIG_SYSLOG_HAS_OPEN
struct levels {
    int prev_level;
    int level;
};

static int _set_level(clist_node_t *node, void *arg)
{
    syslog_entry_t *entry = container_of(node, syslog_entry_t, node);
    struct levels *levels = arg;

    if (levels->prev_level < entry->mask) {
        levels->prev_level = entry->mask;
    }
    if (levels->level) {
        entry->mask = levels->level;
    }

    return 0;
}
#endif

int syslog_set_level(int level)
{
#if CONFIG_SYSLOG_HAS_OPEN
    struct levels levels = {
        .prev_level = 0,
        .level = level,
    };
    mutex_lock(&_logs_lock);
    clist_foreach(&_logs, _set_level, &levels);
    mutex_unlock(&_logs_lock);

    return levels.prev_level;
#else
    return setlogmask(syslog_default_entry, level);
#endif
}

int syslog_get_level(void)
{
#if CONFIG_SYSLOG_HAS_OPEN
    struct levels levels = {
        .prev_level = 0,
        .level = 0,
    };
    mutex_lock(&_logs_lock);
    clist_foreach(&_logs, _set_level, &levels);
    mutex_unlock(&_logs_lock);

    return levels.prev_level;
#else
    return syslog_getmask(syslog_default_entry);
#endif
}

int setlogmask(syslog_entry_t *entry, int mask)
{
    int prev_mask = entry->mask;

    if (mask) {
        entry->mask = mask;
    }

    return prev_mask;
}

void syslog_init(void)
{
#if CONFIG_SYSLOG_MEMARRAY
#if CONFIG_SYSLOG_HAS_OPEN
    memarray_init(&mem_entries, entries, sizeof(syslog_entry_t), ARRAY_SIZE(entries));
#endif
    memarray_init(&mem_msgs, msgs, sizeof(struct syslog_msg), ARRAY_SIZE(msgs));
#endif

    _pid = thread_create(_stack, sizeof(_stack), SYSLOG_THREAD_PRIO, THREAD_CREATE_STACKTEST,
                         _syslog_thread, NULL, "syslog");

#if CONFIG_SYSLOG_HAS_OPEN
    syslog_default_entry = openlog(CONFIG_SYSLOG_DEFAULT_IDENT, 0, LOG_KERN);
#else
    syslog_default_entry = &_syslog_default_entry;
    syslog_default_entry->ident = CONFIG_SYSLOG_DEFAULT_IDENT;
    syslog_default_entry->option = 0;
    syslog_default_entry->facility = LOG_KERN;
    syslog_default_entry->mask = LOG_LEVEL;
#endif
}

const syslog_entry_t *syslog_get_next_log(const syslog_entry_t *log)
{
#if CONFIG_SYSLOG_HAS_OPEN
    if (!log) {
        if (_logs.next) {
            return container_of(_logs.next->next, syslog_entry_t, node);
        }
        else {
            return NULL;
        }
    }
    else if (_logs.next && log->node.next != _logs.next->next) {
        return container_of(log->node.next, syslog_entry_t, node);
    }
    else {
        return NULL;
    }
#else
    if (!log) {
        return syslog_default_entry;
    }
    return NULL;
#endif
}

int syslog_getmask(const syslog_entry_t *log)
{
    if (!log) {
        return 0;
    }
    return log->mask;
}

const char *syslog_getident(const syslog_entry_t *log)
{
    if (!log) {
        return NULL;
    }
    return log->ident;
}
