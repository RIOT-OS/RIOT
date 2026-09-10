/*
 * SPDX-FileCopyrightText: 2017, 2019 JP Bonn, Ken Rabold
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_riscv_common
 * @{
 *
 * @file
 * @brief       Thread context frame stored on stack.
 *
 * @author      JP Bonn
 */

#if !defined(__ASSEMBLER__)
#include <stdint.h>
#endif /* __ASSEMBLER__ */

#ifdef __cplusplus
extern "C" {
#endif

#define REGSZ (__riscv_xlen / 8)    /**< size of one register in bytes */

#if __riscv_xlen == 64
/**
 * @brief   XLEN-sized load instruction mnemonic
 */
#  define REG_L ld
/**
 * @brief   XLEN-sized store instruction mnemonic
 */
#  define REG_S sd
#else
/**
 * @brief   XLEN-sized load instruction mnemonic
 */
#  define REG_L lw
/**
 * @brief   XLEN-sized store instruction mnemonic
 */
#  define REG_S sw
#endif

#if !defined(__ASSEMBLER__)

/**
 * @brief   Stores the registers and PC for a context switch.
 *
 * This also defines context_switch_frame offsets for assembly language.  The
 * structure is sized to maintain 16 byte stack alignment per the ABI.
 * https://github.com/riscv/riscv-elf-psabi-doc
 *
 */
struct context_switch_frame {
    /* Callee saved registers */
    uintptr_t s0;                   /**< s0 register */
    uintptr_t s1;                   /**< s1 register */
    uintptr_t s2;                   /**< s2 register */
    uintptr_t s3;                   /**< s3 register */
    uintptr_t s4;                   /**< s4 register */
    uintptr_t s5;                   /**< s5 register */
    uintptr_t s6;                   /**< s6 register */
    uintptr_t s7;                   /**< s7 register */
    uintptr_t s8;                   /**< s8 register */
    uintptr_t s9;                   /**< s9 register */
    uintptr_t s10;                  /**< s10 register */
    uintptr_t s11;                  /**< s11 register */
    /* Caller saved registers */
    uintptr_t ra;                   /**< ra register */
    uintptr_t t0;                   /**< t0 register */
    uintptr_t t1;                   /**< t1 register */
    uintptr_t t2;                   /**< t2 register */
    uintptr_t t3;                   /**< t3 register */
    uintptr_t t4;                   /**< t4 register */
    uintptr_t t5;                   /**< t5 register */
    uintptr_t t6;                   /**< t6 register */
    uintptr_t a0;                   /**< a0 register */
    uintptr_t a1;                   /**< a1 register */
    uintptr_t a2;                   /**< a2 register */
    uintptr_t a3;                   /**< a3 register */
    uintptr_t a4;                   /**< a4 register */
    uintptr_t a5;                   /**< a5 register */
    uintptr_t a6;                   /**< a6 register */
    uintptr_t a7;                   /**< a7 register */
    /* Saved PC for return from ISR */
    uintptr_t pc;                   /**< program counter */
    uintptr_t pad[3];               /**< padding to maintain 16 byte alignment */
};

#endif /* __ASSEMBLER__ */

/**
 * @name Register offsets
 * @{
 */
/* These values are checked for correctness in context_frame.c */
#define s0_OFFSET     (0 * REGSZ)
#define s1_OFFSET     (1 * REGSZ)
#define s2_OFFSET     (2 * REGSZ)
#define s3_OFFSET     (3 * REGSZ)
#define s4_OFFSET     (4 * REGSZ)
#define s5_OFFSET     (5 * REGSZ)
#define s6_OFFSET     (6 * REGSZ)
#define s7_OFFSET     (7 * REGSZ)
#define s8_OFFSET     (8 * REGSZ)
#define s9_OFFSET     (9 * REGSZ)
#define s10_OFFSET    (10 * REGSZ)
#define s11_OFFSET    (11 * REGSZ)
#define ra_OFFSET     (12 * REGSZ)
#define t0_OFFSET     (13 * REGSZ)
#define t1_OFFSET     (14 * REGSZ)
#define t2_OFFSET     (15 * REGSZ)
#define t3_OFFSET     (16 * REGSZ)
#define t4_OFFSET     (17 * REGSZ)
#define t5_OFFSET     (18 * REGSZ)
#define t6_OFFSET     (19 * REGSZ)
#define a0_OFFSET     (20 * REGSZ)
#define a1_OFFSET     (21 * REGSZ)
#define a2_OFFSET     (22 * REGSZ)
#define a3_OFFSET     (23 * REGSZ)
#define a4_OFFSET     (24 * REGSZ)
#define a5_OFFSET     (25 * REGSZ)
#define a6_OFFSET     (26 * REGSZ)
#define a7_OFFSET     (27 * REGSZ)
#define pc_OFFSET     (28 * REGSZ)
#define pad_OFFSET    (29 * REGSZ)
/** @} */

/**
 * @brief Size of context switch frame
 */
#define CONTEXT_FRAME_SIZE (pad_OFFSET + 3 * REGSZ)

/**
 * @brief Offset of stack pointer in struct _thread
 */
#define SP_OFFSET_IN_THREAD 0

#ifdef __cplusplus
}
#endif

/** @} */
