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
 * @brief   Allows to print out data dumps of memory regions in a similar fashion
 *          to the UNIX's
 *          <a href="http://pubs.opengroup.org/onlinepubs/9699919799/utilities/od.html">
 *              od
 *          </a> tool
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/utilities/od.html">
 *          od(1)
 *      </a>
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __OD_H_
#define __OD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Bit-mask to extract address offset format settings from flags
 */
#define OD_FLAGS_ADDRESS_MASK   (0xc000)

/**
 * @brief Bit-mask to extract byte format settings from flags
 */
#define OD_FLAGS_BYTES_MASK     (0x3e00)

/**
 * @brief Bit-mask to extract length information for byte format from flags
 */
#define OD_FLAGS_LENGTH_MASK    (0x00f7)

/**
 * @anchor od_flags_address
 * @name Address offset format flags
 * @brief Flags to define format of the address offset
 * @{
 */
#define OD_FLAGS_ADDRESS_OCTAL      (0x0000)    /**< octal address offset */
#define OD_FLAGS_ADDRESS_HEX        (0x4000)    /**< hexadecimal address offset */
#define OD_FLAGS_ADDRESS_DECIMAL    (0x8000)    /**< decimal address offset */
#define OD_FLAGS_ADDRESS_NONE       (0xc000)    /**< no address offset */
/** @} */

/**
 * @anchor od_flags_bytes
 * @name Bytes format flags
 * @brief Flags to define format of the byte output
 * @{
 */
/**
 * @brief Print `LENGTH` bytes as `LENGTH`-wide octal integer (`LENGTH` is defined
 *        in the lower significant byte of the flags)
 */
#define OD_FLAGS_BYTES_OCTAL    (0x0000)

/**
 * @brief Print bytes as their represented character in ASCII
 */
#define OD_FLAGS_BYTES_CHAR     (0x2000)

/**
 * @brief Print `LENGTH` bytes as `LENGTH`-wide decimal integer (`LENGTH` is
 *        defined in the lower significant byte of the flags)
 */
#define OD_FLAGS_BYTES_INT      (0x1000)

/**
 * @brief Alias for @ref OD_FLAGS_BYTES_INT
 */
#define OD_FLAGS_BYTES_DECIMAL  (OD_FLAGS_BYTES_INT)

/* XXX: No float support for now, but reserved 0x0800 for this */

/**
 * @brief Print `LENGTH` bytes as `LENGTH`-wide decimal unsigned integer
 *        (`LENGTH` is defined in the lower significant byte of the flags)
 */
#define OD_FLAGS_BYTES_UINT     (0x0400)

/**
 * @brief Print `LENGTH` bytes as `LENGTH`-wide hexadecimal integer
 *        (`LENGTH` is defined in the lower significant byte of the flags)
 */
#define OD_FLAGS_BYTES_HEX      (0x0200)

/** @} */


/**
 * @anchor od_flags_length
 * @name Bytes format length flags
 * @brief Flags to define format length of the byte output
 * @{
 */
#define OD_FLAGS_LENGTH_1       (0x0010)    /**< 1 byte */
#define OD_FLAGS_LENGTH_2       (0x0020)    /**< 2 byte */
#define OD_FLAGS_LENGTH_4       (0x0000)    /**< 4 byte and default */
#define OD_FLAGS_LENGTH_8       (0x0080)    /**< 8 byte */
#define OD_FLAGS_LENGTH_CHAR    (OD_FLAGS_LENGTH_1)    /**< alias for OD_FLAGS_LENGTH_1 */
#define OD_FLAGS_LENGTH_SHORT   (0x0002)    /**< sizeof(short) byte */
#define OD_FLAGS_LENGTH_LONG    (0x0004)    /**< sizeof(long) byte */
/** @} */

/**
 * @brief   Default value for parameter *width* of @ref od()
 */
#define OD_WIDTH_DEFAULT    (16)

/**
 * @brief Dumps memory stored at *data* up to *data_len* in octal, decimal, or
 *        hexadecimal representation to stdout
 *
 * @param[in] data      Data to dump.
 * @param[in] data_len  Length in bytes of *data* to output.
 * @param[in] width     Number of bytes per line. If *width* is 0,
 *                      @ref OD_WIDTH_DEFAULT is assumed as a default value.
 * @param[in] flags     Flags as defined in @ref od_flags_address and
 *                      @ref od_flags_bytes
 */
void od(const void *data, size_t data_len, uint8_t width, uint16_t flags);

/**
 * @brief Dumps memory stored at *data* up to *data_len* in octal, decimal, or
 *        hexadecimal representation to stdout with
 *        `flags == OD_FLAGS_ADDRESS_HEX | OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_1`.
 *
 * @param[in] data      Data to dump.
 * @param[in] data_len  Length in bytes of *data* to output.
 * @param[in] width     Number of bytes per line. If *width* is 0,
 *                      @ref OD_WIDTH_DEFAULT is assumed as a default value.
 */
static inline void od_hex_dump(const void *data, size_t data_len, uint8_t width)
{
    od(data, data_len, width, OD_FLAGS_ADDRESS_HEX | OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_1);
}

#ifdef __cplusplus
}
#endif

#endif /* __OD_H_ */
/** @} */
