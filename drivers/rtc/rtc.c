/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_rtc
 * @brief       RTC (Real Time Clock) interface
 *
 * @note
 * The values used for setting and getting the time/alarm should
 * conform to the `struct tm` specification.
 * @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
 *
 * @{
 * @file
 * @brief       RTC interface definitions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stddef.h> /* for NULL */
#include <errno.h> /* for EINVAL etc. */
#include <string.h> /* for EINVAL etc. */
#include "rtc.h"
#include "mutex.h"
#include "clist.h"

#include "log.h" /* Using LOG_DEBUG for messages useful for application developers */
/* define ENABLE_DEBUG 1 to enable extra trace messages in this file */
#define ENABLE_DEBUG    (0)
#include "debug.h"

/* List handle for RTC registrations */
static clist_node_t _rtc_dev_list;
static mutex_t _rtc_list_mutex = MUTEX_INIT;

int rtc_register(rtc_t *rtc)
{
    DEBUG("rtc_register: %p\n", (void *)rtc);
    if (rtc == NULL) {
        return -EINVAL;
    }
    mutex_lock(&_rtc_list_mutex);
    clist_node_t *node = _rtc_dev_list.next;
    if (node != NULL) {
        /* list not empty */
        do {
            node = node->next;
            if (node == &rtc->list_entry) {
                mutex_unlock(&_rtc_list_mutex);
                LOG_DEBUG("rtc_register: '%s' already registered\n", rtc->name);
                return -EEXIST;
            }
        } while (node != _rtc_dev_list.next);
    }

    clist_rpush(&_rtc_dev_list, &rtc->list_entry);

    mutex_unlock(&_rtc_list_mutex);
    return 0;
}

int rtc_unregister(rtc_t *rtc)
{
    DEBUG("rtc_unregister: %p\n", (void *)rtc);
    if (rtc == NULL) {
        return -EINVAL;
    }
    mutex_lock(&_rtc_list_mutex);
    /* find rtc in the list and remove it */
    clist_node_t *node = clist_remove(&_rtc_dev_list, &rtc->list_entry);
    mutex_unlock(&_rtc_list_mutex);
    if (node == NULL) {
        /* not found */
        LOG_DEBUG("rtc_unregister: '%s' not registered!\n", rtc->name);
        return -ENOENT;
    }
    node->next = NULL;
    return 0;
}

rtc_t *rtc_find(const char *name)
{
    DEBUG("rtc_find: '%s'\n", name);
    mutex_lock(&_rtc_list_mutex);
    clist_node_t *node = _rtc_dev_list.next;
    if (name == NULL) {
        /* return arbitrary entry */
        mutex_unlock(&_rtc_list_mutex);
        return container_of(node, rtc_t, list_entry);
    }
    if (node != NULL) {
        /* list not empty */
        do {
            node = node->next;
            rtc_t *rtc = container_of(node, rtc_t, list_entry);
            if (rtc->name != NULL) {
                if (strcmp(rtc->name, name) == 0) {
                    mutex_unlock(&_rtc_list_mutex);
                    DEBUG("rtc_find: '%s' -> %p\n", name, (void *)rtc);
                    return rtc;
                }
            }
        } while (node != _rtc_dev_list.next);
    }
    mutex_unlock(&_rtc_list_mutex);

    LOG_DEBUG("rtc_find: '%s' not found\n", name);
    return NULL;
}

rtc_t *rtc_iterate(const rtc_t *cur)
{
    clist_node_t *node;
    mutex_lock(&_rtc_list_mutex);
    if (cur == NULL) {
        node = _rtc_dev_list.next;
        if (node == NULL) {
            /* empty list */
            mutex_unlock(&_rtc_list_mutex);
            return NULL;
        }
    }
    else {
        node = cur->list_entry.next;
        /* Threading notice: node will be NULL if cur was removed from the list
         * before this iteration */
        if ((node == NULL) || (node == _rtc_dev_list.next)) {
            mutex_unlock(&_rtc_list_mutex);
            return NULL;
        }
    }
    mutex_unlock(&_rtc_list_mutex);
    return container_of(node, rtc_t, list_entry);
}

/**
 * @internal
 * @brief Check device descriptor for null-pointers and then call the given function
 *
 * @param dev   device descriptor
 * @param table operation table name
 * @param func  function name
 * @param def   default return value if func is NULL
 * @param ...   variadic arguments passed to @p func
 *
 * @return -EINVAL if @p dev is NULL, or @p dev->table is NULL
 * @return @p def if the @p func member of @p dev->rtc_op is NULL
 * @return @p dev->rtc_op->func(...) otherwise
 */
#define DEV_WRAP_CHECK_FUNCPTR(dev, table, func, def, ...) \
    do {                                                    \
        if (((dev) == NULL) || ((dev)->table == NULL)) {    \
            return -EINVAL;                                 \
        }                                                   \
        if ((dev)->table->func == NULL) {                   \
            return (def);                                   \
        }                                                   \
        return (dev)->table->func(__VA_ARGS__);             \
    } while(0)

int rtc_init(rtc_t *rtc)
{
    DEBUG("rtc_init: %p\n", (void *)rtc);
    DEV_WRAP_CHECK_FUNCPTR(rtc, rtc_op, init, 0, rtc);
}

int rtc_set_time(rtc_t *rtc, const struct tm *time)
{
    DEBUG("rtc_set_time: %p, %p\n", (void *)rtc, (void *)time);
    DEV_WRAP_CHECK_FUNCPTR(rtc, rtc_op, set_time, -EPERM, rtc, time);
}

int rtc_get_time(rtc_t *rtc, struct tm *time)
{
    DEBUG("rtc_get_time: %p, %p\n", (void *)rtc, (void *)time);
    DEV_WRAP_CHECK_FUNCPTR(rtc, rtc_op, get_time, -EPERM, rtc, time);
}

int rtc_set_alarm(rtc_t *rtc, const struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    DEBUG("rtc_set_alarm: %p, %p, %p, %p\n", (void *)rtc, (void *)time, (void *)cb, (void *)arg);
    DEV_WRAP_CHECK_FUNCPTR(rtc, rtc_op, set_alarm, -EPERM, rtc, time, cb, arg);
}

int rtc_get_alarm(rtc_t *rtc, struct tm *time)
{
    DEBUG("rtc_get_alarm: %p, %p\n", (void *)rtc, (void *)time);
    DEV_WRAP_CHECK_FUNCPTR(rtc, rtc_op, get_alarm, -EPERM, rtc, time);
}

void rtc_clear_alarm(rtc_t *rtc)
{
    DEBUG("rtc_clear_alarm: %p\n", (void *)rtc);
    if ((rtc == NULL) || (rtc->rtc_op == NULL) || (rtc->rtc_op->clear_alarm == NULL)) {
        return;
    }
    rtc->rtc_op->clear_alarm(rtc);
}
