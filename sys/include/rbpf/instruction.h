/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_bpf_instructions eBPF instructions
 * @ingroup     sys_bpf
 * @brief       Definitions for eBPF instructions
 *
 * @{
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef BPF_INSTRUCTION_H
#define BPF_INSTRUCTION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BPF_INSTRUCTION_CLS_MASK        0x07

#define BPF_INSTRUCTION_CLS_LD          0x00
#define BPF_INSTRUCTION_CLS_LDX         0x01
#define BPF_INSTRUCTION_CLS_ST          0x02
#define BPF_INSTRUCTION_CLS_STX         0x03
#define BPF_INSTRUCTION_CLS_ALU32       0x04
#define BPF_INSTRUCTION_CLS_BRANCH      0x05
#define BPF_INSTRUCTION_CLS_ALU64       0x07

#define BPF_INSTRUCTION_MEM_CLS_MASK    0x07
#define BPF_INSTRUCTION_MEM_SZ_MASK     0x18
#define BPF_INSTRUCTION_MEM_MDE_MASK    0xE0

#define BPF_INSTRUCTION_ALU_CLS_MASK    0x07
#define BPF_INSTRUCTION_ALU_S_MASK      0x08
#define BPF_INSTRUCTION_ALU_OP_MASK     0xf0

#define BPF_INSTRUCTION_LDX_LDX         0x60

#define BPF_INSTRUCTION_STX_ST          0x60

#define BPF_INSTRUCTION_STX_STX         0x60


#define BPF_INSTRUCTION_ALU_ADD         0x00
#define BPF_INSTRUCTION_ALU_SUB         0x10
#define BPF_INSTRUCTION_ALU_MUL         0x20
#define BPF_INSTRUCTION_ALU_DIV         0x30
#define BPF_INSTRUCTION_ALU_OR          0x40
#define BPF_INSTRUCTION_ALU_AND         0x50
#define BPF_INSTRUCTION_ALU_LSH         0x60
#define BPF_INSTRUCTION_ALU_RSH         0x70
#define BPF_INSTRUCTION_ALU_NEG         0x80
#define BPF_INSTRUCTION_ALU_MOD         0x90
#define BPF_INSTRUCTION_ALU_XOR         0xA0
#define BPF_INSTRUCTION_ALU_MOV         0xB0
#define BPF_INSTRUCTION_ALU_ARSH        0xC0

#define BPF_INSTRUCTION_BRANCH_JA       0x00
#define BPF_INSTRUCTION_BRANCH_JEQ      0x10
#define BPF_INSTRUCTION_BRANCH_JGT      0x20
#define BPF_INSTRUCTION_BRANCH_JGE      0x30
#define BPF_INSTRUCTION_BRANCH_JLT      0xa0
#define BPF_INSTRUCTION_BRANCH_JLE      0xb0
#define BPF_INSTRUCTION_BRANCH_JSET     0x40
#define BPF_INSTRUCTION_BRANCH_JNE      0x50
#define BPF_INSTRUCTION_BRANCH_JSGT     0x60
#define BPF_INSTRUCTION_BRANCH_JSGE     0x70
#define BPF_INSTRUCTION_BRANCH_JSLT     0xc0
#define BPF_INSTRUCTION_BRANCH_JSLE     0xd0
#define BPF_INSTRUCTION_BRANCH_CALL     0x80
#define BPF_INSTRUCTION_BRANCH_EXIT     0x90

#define BPF_INSTRUCTION_ALU_BYTESWAP    0xd0

#define BPF_INSTRUCTION_ALU64_ADD_REG    (0x0f)
#define BPF_INSTRUCTION_ALU64_SUB_REG    (0x1f)
#define BPF_INSTRUCTION_ALU64_MUL_REG    (0x2f)
#define BPF_INSTRUCTION_ALU64_DIV_REG    (0x3f)
#define BPF_INSTRUCTION_ALU64_OR_REG     (0x4f)
#define BPF_INSTRUCTION_ALU64_AND_REG    (0x5f)
#define BPF_INSTRUCTION_ALU64_LSH_REG    (0x6f)
#define BPF_INSTRUCTION_ALU64_RSH_REG    (0x7f)
#define BPF_INSTRUCTION_ALU64_MOD_REG    (0x9f)
#define BPF_INSTRUCTION_ALU64_XOR_REG    (0xaf)
#define BPF_INSTRUCTION_ALU64_MOV_REG    (0xbf)
#define BPF_INSTRUCTION_ALU64_ARSH_REG   (0xcf)

#define BPF_INSTRUCTION_ALU64_ADD_IMM    (0x07)
#define BPF_INSTRUCTION_ALU64_SUB_IMM    (0x17)
#define BPF_INSTRUCTION_ALU64_MUL_IMM    (0x27)
#define BPF_INSTRUCTION_ALU64_DIV_IMM    (0x37)
#define BPF_INSTRUCTION_ALU64_OR_IMM     (0x47)
#define BPF_INSTRUCTION_ALU64_AND_IMM    (0x57)
#define BPF_INSTRUCTION_ALU64_LSH_IMM    (0x67)
#define BPF_INSTRUCTION_ALU64_RSH_IMM    (0x77)
#define BPF_INSTRUCTION_ALU64_NEG_IMM    (0x87)
#define BPF_INSTRUCTION_ALU64_MOD_IMM    (0x97)
#define BPF_INSTRUCTION_ALU64_XOR_IMM    (0xa7)
#define BPF_INSTRUCTION_ALU64_MOV_IMM    (0xb7)
#define BPF_INSTRUCTION_ALU64_ARSH_IMM   (0xc7)

#define BPF_INSTRUCTION_ALU32_ADD_REG    (0x0c)
#define BPF_INSTRUCTION_ALU32_SUB_REG    (0x1c)
#define BPF_INSTRUCTION_ALU32_MUL_REG    (0x2c)
#define BPF_INSTRUCTION_ALU32_DIV_REG    (0x3c)
#define BPF_INSTRUCTION_ALU32_OR_REG     (0x4c)
#define BPF_INSTRUCTION_ALU32_AND_REG    (0x5c)
#define BPF_INSTRUCTION_ALU32_LSH_REG    (0x6c)
#define BPF_INSTRUCTION_ALU32_RSH_REG    (0x7c)
#define BPF_INSTRUCTION_ALU32_MOD_REG    (0x9c)
#define BPF_INSTRUCTION_ALU32_XOR_REG    (0xac)
#define BPF_INSTRUCTION_ALU32_MOV_REG    (0xbc)
#define BPF_INSTRUCTION_ALU32_ARSH_REG   (0xcc)

#define BPF_INSTRUCTION_ALU32_ADD_IMM    (0x04)
#define BPF_INSTRUCTION_ALU32_SUB_IMM    (0x14)
#define BPF_INSTRUCTION_ALU32_MUL_IMM    (0x24)
#define BPF_INSTRUCTION_ALU32_DIV_IMM    (0x34)
#define BPF_INSTRUCTION_ALU32_OR_IMM     (0x44)
#define BPF_INSTRUCTION_ALU32_AND_IMM    (0x54)
#define BPF_INSTRUCTION_ALU32_LSH_IMM    (0x64)
#define BPF_INSTRUCTION_ALU32_RSH_IMM    (0x74)
#define BPF_INSTRUCTION_ALU32_NEG_IMM    (0x84)
#define BPF_INSTRUCTION_ALU32_MOD_IMM    (0x94)
#define BPF_INSTRUCTION_ALU32_XOR_IMM    (0xa4)
#define BPF_INSTRUCTION_ALU32_MOV_IMM    (0xb4)
#define BPF_INSTRUCTION_ALU32_ARSH_IMM   (0xc4)


#define BPF_INSTRUCTION_JMP_ALWAYS  (0x05)
#define BPF_INSTRUCTION_JMP_EQ_IMM  (0x15)
#define BPF_INSTRUCTION_JMP_GT_IMM  (0x25)
#define BPF_INSTRUCTION_JMP_GE_IMM  (0x35)
#define BPF_INSTRUCTION_JMP_SET_IMM (0x45)
#define BPF_INSTRUCTION_JMP_NE_IMM  (0x55)
#define BPF_INSTRUCTION_JMP_SGT_IMM (0x65)
#define BPF_INSTRUCTION_JMP_SGE_IMM (0x75)
#define BPF_INSTRUCTION_JMP_LT_IMM  (0xa5)
#define BPF_INSTRUCTION_JMP_LE_IMM  (0xb5)
#define BPF_INSTRUCTION_JMP_SLT_IMM (0xc5)
#define BPF_INSTRUCTION_JMP_SLE_IMM (0xd5)

#define BPF_INSTRUCTION_JMP_EQ_REG  (0x1d)
#define BPF_INSTRUCTION_JMP_GT_REG  (0x2d)
#define BPF_INSTRUCTION_JMP_GE_REG  (0x3d)
#define BPF_INSTRUCTION_JMP_SET_REG (0x4d)
#define BPF_INSTRUCTION_JMP_NE_REG  (0x5d)
#define BPF_INSTRUCTION_JMP_SGT_REG (0x6d)
#define BPF_INSTRUCTION_JMP_SGE_REG (0x7d)
#define BPF_INSTRUCTION_JMP_LT_REG  (0xad)
#define BPF_INSTRUCTION_JMP_LE_REG  (0xbd)
#define BPF_INSTRUCTION_JMP_SLT_REG (0xcd)
#define BPF_INSTRUCTION_JMP_SLE_REG (0xdd)

#define BPF_INSTRUCTION_MEM_LDDW    (0x18)
#define BPF_INSTRUCTION_MEM_LDDWD   (0xB8)
#define BPF_INSTRUCTION_MEM_LDDWR   (0xD8)

#define BPF_INSTRUCTION_MEM_STXW    (0x63)
#define BPF_INSTRUCTION_MEM_STXH    (0x6b)
#define BPF_INSTRUCTION_MEM_STXB    (0x73)
#define BPF_INSTRUCTION_MEM_STXDW   (0x7b)

#define BPF_INSTRUCTION_MEM_STW     (0x62)
#define BPF_INSTRUCTION_MEM_STH     (0x6a)
#define BPF_INSTRUCTION_MEM_STB     (0x72)
#define BPF_INSTRUCTION_MEM_STDW    (0x7a)

#define BPF_INSTRUCTION_MEM_LDXW    (0x61)
#define BPF_INSTRUCTION_MEM_LDXH    (0x69)
#define BPF_INSTRUCTION_MEM_LDXB    (0x71)
#define BPF_INSTRUCTION_MEM_LDXDW   (0x79)

#define BPF_INSTRUCTION_CALL        (0x85)
#define BPF_INSTRUCTION_RETURN      (0x95)

#define BPF_REG_NUM                 (11) /**< Number of registers available in BPF */
#define BPF_REG_RETURN_ARGUMENT     (0)  /**< Register used for call return argument */
#define BPF_REG_CONTEXT             (1)  /**< Register used for storing the VM context */
#define BPF_REG_STACK_PTR           (10) /**< Register used as read-only stack pointer */
/**
 * @brief eBPF instruction format
 *
 * Always in host byte order
 */
typedef struct __attribute__((packed)) {
    uint8_t opcode;
    unsigned dst : 4;
    unsigned src : 4;
    int16_t offset;
    int32_t immediate;
} bpf_instruction_t;

#ifdef __cplusplus
}
#endif
#endif /* BPF_INSTRUCTION_H */
/** @} */
