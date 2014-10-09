/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file        io_arch.h
 * @brief       Architecture dependent interface for the standard output
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __IO_ARCH_H
#define __IO_ARCH_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Define mapping between kernel internal and arch interfaces
 *
 * This mapping is done for compatibility of existing platforms,
 * new platforms should always use the *_arch_* interfaces.
 * @{
 */
#ifdef COREIF_NG
#define fw_puts         io_arch_puts
#endif
/** @} */

/**
 * @brief Write a number of characters to the std-out
 *
 * The stdout is on most platforms mapped to the UART0. This is however only a
 * weak convention and must not be true for all platforms.
 *
 * @param[in] data      the data that is to be written
 * @param[in] count     the number of bytes to write
 *
 * @return              the number of bytes that were actually written
 */
int io_arch_puts(char *data, int count);

#ifdef __cplusplus
}
#endif

#endif /* __IO_ARCH_H */
/** @} */
