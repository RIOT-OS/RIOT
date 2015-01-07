/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``ubjson`` module
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef TESTS__UBJSON_H__
#define TESTS__UBJSON_H__

#include "embUnit.h"

#include "ubjson.h"

#ifdef __cplusplus
extern "C" {
#endif

ssize_t test_ubjson_write_fun(ubjson_cookie_t *restrict cookie, const void *buf, size_t len);
ssize_t test_ubjson_read_fun(ubjson_cookie_t *restrict cookie, void *buf, size_t len);

void test_ubjson_test(void (*sender_fun)(void), void (*receiver_fun)(void));

void test_ubjson_empty_array(void);
void test_ubjson_empty_object(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef TESTS__UBJSON_H__ */
/** @} */
