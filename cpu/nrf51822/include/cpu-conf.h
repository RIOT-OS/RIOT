/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_nrf51822
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "nrf51.h"
#include "nrf51_bitfields.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Kernel configuration
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (512)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (512)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (256)
/** @} */

/**
 * @name UART0 buffer size definition for compatibility reasons
 *
 * TODO: remove once the remodeling of the uart0 driver is done
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif
/** @} */

/**
 * @name Length in bytes for reading CPU_ID
 */
#define CPUID_ID_LEN                    (8)

/**
 * @name CPU Flash configuration
 * @{
 */
#define FLASH_PAGE_SIZE             (1024)         /**< Page size of flash memory */
#define FLASH_NUM_PAGES             (256)          /**< Number of flash pages */
#define FLASH_WRITE_ALIGN           (4)            /**< number of bytes must be written at once */
#define FLASH_ERASED_WORD_VALUE     (0xffffffff)   /**< value of erased data words */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
