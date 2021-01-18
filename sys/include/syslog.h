/*
 * Copyright (C) 2020 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_syslog Syslog
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Basic syslog implementation
 *
 * This module implements a syslog module, partially compatible with RFC 5424.
 * It implements the same log levels and facility, as well as string format than
 * RFC 5424. The streaming protocol is not implemented.
 *
 * This module provides a core that manages log entries, and format log strings.
 * The string usage is made by "backend" implementations. This module implements
 * two backends: stdio and file.
 * stdio backend simply prints log strings on stdio.
 * file backend print log strings in a log file and take care of file rotation.
 *
 * The core API is similar to Linux syslog API. The main difference being that
 * openlog returns a syslog entry pointer that must be passed to subsequent calls
 * to syslog API. syslog/vsyslog and closelog then takes an extra parameter.
 * For convenience, a default entry is provided and always open. openlog/closelog
 * is also an optional part. In that case only the default entry can be used.
 * LOG_* macros are also provided for convenience and compatibility with log.h.
 * The syslog entry used by LOG_* macro can be set by defining SYSLOG_ENTRY prior
 * to including (sys)log.h. If it is not defined, the default entry is used.
 *
 * @author      Vincent Dupont <vincent.dupont@continental-its.com>
 */

#ifndef SYSLOG_H
#define SYSLOG_H

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_SYSLOG_HAS_OPEN
/**
 * @brief   Syslog implement openlog/closelog
 */
#define CONFIG_SYSLOG_HAS_OPEN      0
#endif

/**
 * @brief Possible LOG facilities as used in the glibc syslog implementation
 */
enum {
    LOG_KERN        = 0,
    LOG_USER,
    LOG_MAIL,
    LOG_DAEMON,
    LOG_AUTH,
    LOG_SYSLOG,
    LOG_LPR,
    LOG_NEWS,
    LOG_UUCP,
    LOG_CRON,
    LOG_AUTHPRIV,
    LOG_FTP,
    LOG_LOCAL0      = 16,
    LOG_LOCAL1,
    LOG_LOCAL2,
    LOG_LOCAL3,
    LOG_LOCAL4,
    LOG_LOCAL5,
    LOG_LOCAL6,
    LOG_LOCAL7,
};

/**
 * @brief   Syslog entry type
 */
typedef struct syslog_entry syslog_entry_t;

/**
 * @brief   Syslog default entry
 */
extern syslog_entry_t *syslog_default_entry;

#ifndef SYSLOG_ENTRY
/**
 * @brief   Syslog entry used by LOG_* macros
 */
#define SYSLOG_ENTRY    syslog_default_entry
#endif

/**
 * @brief   Internal convenience macro to check log level
 */
#define CHECK_LOG_LEVEL(level) if ((level) <= LOG_LEVEL)

/**
 * @brief Log message if level <= LOG_LEVEL
 */
#ifdef __clang__    /* following pragmas required for clang 3.8.0 */
#define LOG(level, ...) do { \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wtautological-compare\"") \
    CHECK_LOG_LEVEL(level) syslog(SYSLOG_ENTRY, (level), __VA_ARGS__); } while (0U) \
    _Pragma("clang diagnostic pop")
#else
#define LOG(level, ...) do { \
    CHECK_LOG_LEVEL(level) syslog(SYSLOG_ENTRY, (level), __VA_ARGS__); } while (0U)
#endif /* __clang__ */

/**
 * @name Logging convenience defines
 * @{
 */
#define LOG_EMERG(...) LOG(LOG_EMERG, __VA_ARGS__)
#define LOG_ALERT(...) LOG(LOG_ALERT, __VA_ARGS__)
#define LOG_CRIT(...) LOG(LOG_CRIT, __VA_ARGS__)
#define LOG_ERROR(...) LOG(LOG_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_WARNING, __VA_ARGS__)
#define LOG_NOTICE(...) LOG(LOG_NOTICE, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_DEBUG, __VA_ARGS__)
/** @} */

#if CONFIG_SYSLOG_HAS_OPEN || DOXYGEN
/**
 * @brief   Open a log entry in the syslog registry
 *
 * @param[in]   ident       string prepended in every message printed by this entry
 * @param[in]   option      provided for API compatibility, not used at the moment
 * @param[in]   facility    log facility
 *
 * @return  pointer to an allocated entry on success, NULL on error (entry cannot be allocated)
 */
syslog_entry_t *openlog(const char *ident, int option, int facility);

/**
 * @brief   Close a log entry
 *
 * @param[in]   entry       entry to close
 */
void closelog(syslog_entry_t *entry);
#endif

/**
 * @brief   Log a message in the syslog
 *
 * @param[in]   entry       log entry to use
 * @param[in]   priority    priority of the log message
 * @param[in]   format      message format string
 */
void syslog(syslog_entry_t *entry, int priority, const char *format, ...);

/**
 * @brief   Log a message in the syslog
 *
 * @param[in]   entry       log entry to use
 * @param[in]   priority    priority of the log message
 * @param[in]   format      message format string
 * @param[in]   ap          parameter list
 */
void vsyslog(syslog_entry_t *entry, int priority, const char *format, va_list ap);

/**
 * @brief   Set the log level for a log entry
 *
 * @param[in]   entry       log entry to use
 * @param[in]   mask        log level, 0 is ignored
 *
 * @return previous mask of the entry
 */
int setlogmask(syslog_entry_t *entry, int mask);

/**
 * @brief   Initialize the syslog module
 */
void syslog_init(void);

/**
 * @brief   Set syslog hostname
 *
 * @paran[in]   name    hostname to use, the buffer needs to remain allocated
 */
void syslog_set_hostname(const char *name);

/**
 * @brief   Change the log level of all entries
 *
 * @param[in]   level       log level, 0 is ignored
 *
 * @return previous greatest level of all entries
 */
int syslog_set_level(int level);

/**
 * @brief   Returns the current greatest log level
 *
 * @return log level
 */
int syslog_get_level(void);

/**
 * @brief   Get the next log in the entry list
 *
 * @param[in]   log     current log, NULL is passed for the first call
 *
 * @return  next entry in the list, NULL if the end of the list is reached
 */
const syslog_entry_t *syslog_get_next_log(const syslog_entry_t *log);

/**
 * @brief   Get the current mask of a given entry
 *
 * @param[in]   log     log entry
 *
 * @return log level
 */
int syslog_getmask(const syslog_entry_t *log);

/**
 * @brief   Get ident of a given entry
 *
 * @param[in]   log     log entry
 *
 * @return ident string
 */
const char *syslog_getident(const syslog_entry_t *log);

/* Backend-specific functions */

/* STDIO */

/**
 * @brief   Enable/disable stdio output
 *
 * @param[in]   print   true to enable stdio output, false to disable
 */
void syslog_backend_stdio_print(bool print);

/* File */

#ifndef CONFIG_SYSLOG_BACKEND_FILE_HAS_SUSPEND
/**
 * @brief   Enable suspend/resume file rotation
 */
#define CONFIG_SYSLOG_BACKEND_FILE_HAS_SUSPEND  1
#endif

/**
 * @brief   Start file backend
 *
 * Start syslog file backend. This should be called when the filesystem is
 * ready. It will initialize the file backend by looking into the log directory
 * and counting the current number of files.
 */
int syslog_backend_file_start(void);

/**
 * @brief   Stop file backend
 */
void syslog_backend_file_stop(void);

/**
 * @brief   Suspend log rotation
 *
 * If called multiple times, syslog_backend_file_resume_rotation() must be
 * called the same number of times so the rotation is resumed.
 */
void syslog_backend_file_suspend_rotation(void);

/**
 * @brief   Resume log rotation
 */
void syslog_backend_file_resume_rotation(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* SYSLOG_H */
