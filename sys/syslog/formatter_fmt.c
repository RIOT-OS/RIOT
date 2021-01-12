/*
 * Copyright (C) 2021 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_syslog
 * @{
 * @file
 * @brief   Syslog implementation - fmt formatter
 * @author  Vincent Dupont <vincent.dupont@continental-its.com>
 * @}
 */

#include "formatter.h"
#include "fmt.h"

ssize_t syslog_build_head(const struct syslog_msg *msg, const char *hostname, char *str, size_t len)
{
    /* PRI VERSION SP TIMESTAMP SP is 30 char long */
    assert(len > 30);
    char *p = str;
    /* <PRIVAL>VERSION */
    p += fmt_char(p, '<');
    p += fmt_s16_dec(p, msg->pri);
    p += fmt_str(p, ">1 ");
    if (msg->time_set) {
        /* DATE-FULLYEAR "-" DATE-MONTH "-" DATE-MDAY */
        p += fmt_s16_dec(p, msg->time.tm_year + 1900);
        p += fmt_char(p, '-');
        if (msg->time.tm_mon + 1 < 10) {
            p += fmt_char(p, '0');
        }
        p += fmt_s16_dec(p, msg->time.tm_mon + 1);
        p += fmt_char(p, '-');
        if (msg->time.tm_mday < 10) {
            p += fmt_char(p, '0');
        }
        p += fmt_s16_dec(p, msg->time.tm_mday);
        /* T */
        p += fmt_char(p, 'T');
        /* TIME-HOUR ":" TIME-MINUTE ":" TIME-SECOND */
        if (msg->time.tm_hour < 10) {
            p += fmt_char(p, '0');
        }
        p += fmt_s16_dec(p, msg->time.tm_hour);
        p += fmt_char(p, ':');
        if (msg->time.tm_min < 10) {
            p += fmt_char(p, '0');
        }
        p += fmt_s16_dec(p, msg->time.tm_min);
        p += fmt_char(p, ':');
        if (msg->time.tm_sec < 10) {
            p += fmt_char(p, '0');
        }
        p += fmt_s16_dec(p, msg->time.tm_sec);
        /* TIME-OFFSET (Z) */
        p += fmt_char(p, 'Z');
    }
    else {
        p += fmt_char(p, '-');
    }
    p += fmt_char(p, ' ');

    size_t used = p - str;
    /* keeping one spare for null char */
    if (len > used + fmt_strlen(hostname) + fmt_strlen(msg->app_name) + 3) {
        p += fmt_str(p, hostname);
        p += fmt_char(p, ' ');
        p += fmt_str(p, msg->app_name);
        p += fmt_char(p, ' ');
    }
    used = p - str;
    /* keeping one spare for null char */
    if (len > used + fmt_u16_dec(NULL, msg->proc_id) + 6) {
        p += fmt_u16_dec(p, msg->proc_id);
        p += fmt_char(p, ' ');
        p += fmt_char(p, '-');
        p += fmt_char(p, ' ');
        p += fmt_char(p, '-');
        p += fmt_char(p, ' ');
    }
    *p = '\0';

    return p - str;
}
