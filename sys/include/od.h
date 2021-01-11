/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup sys_od Object dump
 * @ingroup  sys
 * @brief    Allows to print out data dumps of memory regions in hexadecimal or/and
             ASCII representation.
 *
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef OD_H
#define OD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @brief   Default value for parameter *width* of @ref od_hex_dump()
 */
#define OD_WIDTH_DEFAULT    (16)

/**
 * @brief Dumps memory stored at *data* byte-wise up to *data_len* in
 *        hexadecimal representation to stdout. If the pseudomodlue `od_string`
 *        is used (`USEMODULE += od_string`) the ASCII representation of *data* is
 *        also displayed.
 *        The displayed start address of *data* can be given as *offset*.
 *
 * @param[in] data      Data to dump.
 * @param[in] data_len  Length in bytes of *data* to output.
 * @param[in] width     Number of bytes per line. If *width* is 0,
 *                      @ref OD_WIDTH_DEFAULT is assumed as a default value.
 * @param[in] offset    Adds an offset to the printed memory addresses.
 *                      If the origin of the data is an address in memory,
 *                      this can be used to print the real addresses together
 *                      with the data.
 */
void od_hex_dump_ext(const void *data, size_t data_len, uint8_t width, uint32_t offset);

/**
 * @brief Dumps memory stored at *data* byte-wise up to *data_len* in
 *        hexadecimal representation to stdout. If the pseudomodlue `od_string`
 *        is used (`USEMODULE += od_string`) the ASCII representation of *data* is
 *        also displayed.
 *
 * @param[in] data      Data to dump.
 * @param[in] data_len  Length in bytes of *data* to output.
 * @param[in] width     Number of bytes per line. If *width* is 0,
 *                      @ref OD_WIDTH_DEFAULT is assumed as a default value.
 */
static inline void od_hex_dump(const void *data, size_t data_len, uint8_t width)
{
    od_hex_dump_ext(data, data_len, width, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* OD_H */
/** @} */
