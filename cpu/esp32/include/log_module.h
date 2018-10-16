/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Log module to realize consistent log messages
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief log_write overridden function
 *
 * @param[in] level (unused)
 * @param[in] format String that the function will print
 */
void log_write(unsigned level, const char *format, ...);

/**
 * @brief log_write overridden function, tagged version
 *
 * @param[in] level     Level of the message
 * @param[in] tag       Additional information like function or module
 * @param[in] format    String that the function will print
 */
void log_write_tagged(unsigned level, const char *tag, const char *format, ...);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* LOG_MODULE_H */
