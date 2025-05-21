/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_riscv_common
 * @{
 *
 * @file
 * @brief           RISC-V CPU configuration options
 *
 * @author          Koen Zandberg
 */


#include "vendor/riscv_csr.h"
#include "cpu_conf_common.h"

/**
 * @name Configuration of default stack sizes
 * @{
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF   (256)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (1024)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE           (256)
#endif
/** @} */

/**
 * @brief   Attribute for memory sections required by SRAM PUF
 */
#define PUF_SRAM_ATTRIBUTES __attribute__((used, section(".noinit")))

/**
 * @brief   Declare the heap_stats function as available
 */
#define HAVE_HEAP_STATS

/**
 * @brief   This arch uses the inlined irq API.
 */
#define IRQ_API_INLINED     (1)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
