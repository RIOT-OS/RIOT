/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_arduino
 * @brief       Wrapper to keep source code compatibility for Wire.h
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef MODULE_PERIPH_I2C
#error "No I2C support on your board"
#endif

#ifdef __cplusplus
#include "wireport.hpp"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
