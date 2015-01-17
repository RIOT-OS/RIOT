/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k20
 * @{
 *
 * @file
 * @brief           K20 Memory size and offset definitions
 *
 * This file is primarily for the cpp preprocessor that generates the linker
 * script, thus it must not contain any C code. It can however be included if
 * such information is relevant
 *
 * @author          Finn Wilke <finn.wilke@fu-berlin.de>
 */


/**
 * @brief   Memory Configuration
 * @{
 */

#define CPU_MODEL_MK20DN32VLF5
/**
 * @brief Sizes
 * @{
 */
#if   defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DX32VLF5) || defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DX32VFT5)
#define CPU_ROM_SIZE 0x8000
#define CPU_SRAM_SIZE 0x2000

#elif defined(CPU_MODEL_MK20DN64VLF5) || defined(CPU_MODEL_MK20DX64VLF5) || defined(CPU_MODEL_MK20DN64VFT5) || defined(CPU_MODEL_MK20DX64VFT5)
#define CPU_ROM_SIZE 0x10000
#define CPU_SRAM_SIZE 0x4000

#elif defined(CPU_MODEL_MK20DN128VLF5) || defined(CPU_MODEL_MK20DX128VLF5) || defined(CPU_MODEL_MK20DN128VFT5) || defined(CPU_MODEL_MK20DX128VFT5)
#define CPU_ROM_SIZE 0x20000
#define CPU_SRAM_SIZE 0x4000

#else
#error "Unsupported Freescale K20 CPU"
#endif

#if   defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DN32VLF5) || defined(CPU_MODEL_MK20DN64VLF5) || defined(CPU_MODEL_MK20DN64VFT5) || defined(CPU_MODEL_MK20DN128VLF5) || defined(CPU_MODEL_MK20DN128VFT5)
#define CPU_FLEXNVM_SIZE 0
#define CPU_FLEXRAM_SIZE 0

#elif defined(CPU_MODEL_MK20DX32VLF5) || defined(CPU_MODEL_MK20DX32VFT5) || defined(CPU_MODEL_MK20DX64VLF5) || defined(CPU_MODEL_MK20DX64VFT5) || defined(CPU_MODEL_MK20DX128VLF5) || defined(CPU_MODEL_MK20DX128VFT5)
#define CPU_FLEXNVM_SIZE 0x8000
#define CPU_FLEXRAM_SIZE 0x800
#endif
/** @} */

/**
 * @brief Offsets
 * @{
 */
#define CPU_ROM_START 0x0
#define CPU_SRAM_START 0x20000000 - (CPU_SRAM_SIZE / 2)
#define CPU_FLEXRAM_START 0x14000000

#ifdef CPU_BOOTLOADER_OFFSET
#define CPU_APPLICATION_ROM_START (CPU_ROM_START + CPU_BOOTLOADER_OFFSET)
#define CPU_APPLICATION_ROM_SIZE CPU_ROM_SIZE - CPU_BOOTLOADER_OFFSET
#else
#define CPU_APPLICATION_ROM_START CPU_ROM_START
#define CPU_APPLICATION_ROM_SIZE CPU_ROM_SIZE
#endif
/** @} */

/** @} */
