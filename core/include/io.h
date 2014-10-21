/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_io IO Interface
 * @brief       Interface to system io functions
 * @ingroup     core
 * @{
 *
 * @file        io.h
 * @brief       Prototypes for system io functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef IO_H
#define IO_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Firmware putstring implementation
 *
 * @param[in] data  characters to be written
 * @param[in] count number of characters to be written
 */
int fw_puts(char *data, int count);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* IO_H */
