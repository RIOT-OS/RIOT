/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Implementation of required system calls
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */


#include "syscalls_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

#define MALLOC_CAP_DEFAULT  MALLOC_CAP_8BIT

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif
