/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file        flags.h
 * @brief       Misc flag definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _FLAGS_H
#define _FLAGS_H

#define CREATE_SLEEPING     (1u << 0)
#define AUTO_FREE           (1u << 1)
#define CREATE_WOUT_YIELD   (1u << 2)
#define CREATE_STACKTEST    (1u << 3)
#define CREATE_NOMSG        (1u << 4)

/** @} */
#endif // _FLAGS_H
