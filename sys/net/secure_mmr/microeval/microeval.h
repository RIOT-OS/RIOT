/*
 * microeval.h - Header for the logging functions of the secure routing.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */


/*
 * @file        microeval.h
 * @brief       Logging of the secure routing.
 *              It enables the logging during the secure routing process.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics.
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @date        21.09.2013 18:25:17
 *
 **/



#ifndef _MICROEVAL_H_
#define _MICROEVAL_H_

void ulogdf_impl_simple(char *format, ...);
void ulogdf_impl(char *s, int log_level, char *filename, int line,
                 const char *functionname, ...);
void ulogdf_direct(char *s, ...);
void ulog_result(int res); // 0 = success, negative = failure

#define CURRENTLOG_LEVEL LOG_DEBUG

#define LOG_ALL       0
#define LOG_CRITICAL  1
#define LOG_IMPORTANT 2
#define LOG_INFO      3
#define LOG_DEBUG     4

#define ULOG_ENABLED  0

#if ULOG_ENABLED
#define ulog ulogdf_impl_simple
#define ulogdf(x, ...) ulogdf_impl(x, LOG_ALL, __FILE__, __LINE__,          \
                                  __FUNCTION__, ##__VA_ARGS__)
#define ulog_info(x, ...) ulogdf_impl(x, LOG_INFO, __FILE__, __LINE__,      \
                                      __FUNCTION__, ##__VA_ARGS__)
#define ulog_debug(x, ...) ulogdf_impl(x, LOG_DEBUG, __FILE__, __LINE__,    \
                                      __FUNCTION__, ##__VA_ARGS__)
#define ulog_error(x, ...) ulogdf_impl(x, LOG_CRITICAL, __FILE__, __LINE__, \
                                               __FUNCTION__, ##__VA_ARGS__)
#define ulog_force printf
#else
#define ulog(x, ...)
#define ulog_force printf
#define ulogdf(x, ...)
#define ulog_info(x, ...)
#define ulog_debug(x, ...)
#define ulog_error(x, ...)
#endif

#endif
