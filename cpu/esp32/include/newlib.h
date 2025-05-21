/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Wrapper for sys/features.h
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is just a wrapper for `newlib.h` to define `_NANO_FORMATTED_IO`
 * correctly if the `newlib_nano` module is used. In difference to other
 * toolchains, ESP32x toolchains don't use different `newlib.h` versions.
 */


#include "kernel_defines.h"

#ifndef DOXYGEN

#if IS_USED(MODULE_NEWLIB_NANO)
/* newlib nano formatted io is used */
#define _NANO_FORMATTED_IO  1
#endif

#include_next <newlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
