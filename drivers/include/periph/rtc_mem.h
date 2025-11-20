/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @defgroup    drivers_periph_rtc_mem Low-Power RTC Memory
 * @ingroup     drivers_periph_rtc
 * @brief       Low-level RTC Memory peripheral driver
 *
 * This API provides an interface to access low-power memory present on some RTCs.
 * This memory is retained even when the rest of the system is powered off.
 *
 * @{
 * @file
 * @brief       Low-level RTC memory peripheral driver interface definitions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get the amount of RTC memory.
 *
 * @return  The usable amount of RTC memory in bytes
 */
size_t rtc_mem_size(void);

/**
 * @brief Read from RTC memory
 *
 * @note  Reading beyond @ref rtc_mem_size are illegal and trigger an
 *        assertion / be discarded.
 *
 * @param[in]  offset   Offset to the start of RTC memory in bytes
 * @param[out] data     Destination buffer
 * @param[in]  len      Amount of bytes to read
 */
void rtc_mem_read(unsigned offset, void *data, size_t len);

/**
 * @brief Write to RTC memory
 *
 * @note  Writing beyond @ref rtc_mem_size are illegal and trigger an
 *        assertion / be discarded.
 *
 * @param[in] offset    Offset to the start of RTC memory in bytes
 * @param[in] data      Source buffer
 * @param[in] len       Amount of bytes to write
 */
void rtc_mem_write(unsigned offset, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
