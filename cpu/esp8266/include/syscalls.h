/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
