/**
 * System wide configuration struct.
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup config
 * @{
 * @file    config_c
 * @brief   Provides system configuration struct with default values.
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdint.h>
#include <config.h>

config_t sysconfig  = {
    0,      ///< default ID
    0,      ///< default radio address
    0,      ///< default radio channel
    "foobar", ///< default name
};
