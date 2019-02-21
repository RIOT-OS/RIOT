/*
 * Copyright 2016 Freie Universität Berlin
 * Copyright 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_checksum Checksum
 * @ingroup     sys_hashes
 * @brief       Checksum function libraries
 *
 * This module provides a number of checksum functions. Most notably is the
 * @ref sys_checksum_crc16_ccitt and the @ref sys_checksum_ucrc16 modules which
 * provide support for the CRC16 checksum.
 *
 * @ref sys_checksum_crc16_ccitt only provides an implementation of the CCITT
 * flavor of CRC16 (polynomial @$ x^{16} + x^{12} + x^{5} + 1 @$) for big-endian
 * numbers with starting seed `0x1d0f` (though others can be provided), while
 * @ref sys_checksum_ucrc16 is more generalized, since it takes the
 * hexadecimal representation of the polynomial as a parameter and provides
 * functions and standardized polynomials for both big- and little-endian
 * numbers.
 *
 * The caveat of @ref sys_checksum_ucrc16 is that it is significantly slower
 * (approximately factor 8) than @ref sys_checksum_crc16_ccitt since the latter
 * is able to calculate the checksum byte-wise, while the first calculates
 * needs to calculate it bit-wise. @ref sys_checksum_crc16_ccitt achieves this
 * advantage by using a look-up table that provides the checksum for every
 * possible byte-value. It thus trades of memory against speed. If your
 * platform is rather small equipped in memory you should prefer the
 * @ref sys_checksum_ucrc16 version.
 */
