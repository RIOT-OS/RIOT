/*
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_ps PS
 * @ingroup     sys
 * @brief       Show list with all threads
 * @{
 *
 * @file
 * @brief       List information about all active threads
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __PS_H
#define __PS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print information to all active threads to stdout.
 */
void ps(void);

#ifdef __cplusplus
}
#endif

#endif /* __PS_H */
/** @} */
