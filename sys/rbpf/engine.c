/*
 * Copyright (C) 2023 Freie Universität Berlin
 * Copyright (C) 2023 Inria
 * Copyright (C) 2023 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel_defines.h"
#include "rbpf.h"
#include "rbpf/builtin_shared.h"
#include "rbpf/builtin_calls.h"
#include "rbpf/instruction.h"
#include "rbpf/config.h"

static bool _check_mem(const rbpf_application_t *rbpf, const intptr_t addr, size_t size,
                       uint8_t type)
{
    const intptr_t end = addr + size;

    for (const rbpf_mem_region_t *region = &rbpf->stack_region; region; region = region->next) {
        if ((addr >= (intptr_t)(region->start)) &&
            (end <= (intptr_t)(region->start + region->len)) &&
            (region->flags & type)) {

            return true;
        }
    }

    return false;
}

static bool _check_load(const rbpf_application_t *rbpf, const intptr_t addr, size_t size)
{
    return _check_mem(rbpf, addr, size, RBPF_MEM_REGION_READ);
}

static bool _check_store(const rbpf_application_t *rbpf, const intptr_t addr, size_t size)
{
    return _check_mem(rbpf, addr, size, RBPF_MEM_REGION_WRITE);
}

bool rbpf_store_allowed(const rbpf_application_t *rbpf, void *addr, size_t size)
{
    return _check_store(rbpf, (intptr_t)addr, size);
}

bool rbpf_load_allowed(const rbpf_application_t *rbpf, void *addr, size_t size)
{
    return _check_load(rbpf, (intptr_t)addr, size);
}

static rbpf_call_t _rbpf_get_call(uint32_t num)
{
    switch (num) {
    default:
        return rbpf_get_external_call(num);
    }
}

/**
 * This is a set of macros to easily implement the similar rBPF instructions
 */

/* Macro for the destination, source and immediate value */
#define DST regmap[(*instr)->dst]   /* DST is the register targeted by the instruction */
#define SRC regmap[(*instr)->src]   /* SRC is the source register from the instruction */
#define IMM (*instr)->immediate     /* And this one matches the immediate value in the instruction */

#define CONT_JUMP \
    if (jump_cond) { \
        return _rbpf_jump(rbpf, instr); \
    } \
    break;

/* Check if we implement 32 bit instructions */
#if (RBPF_ENABLE_ALU32)

/* Generate the ALU instruction, based on the opcode name and the operation
 * itself. ALU(ADD, +) generates the 2 or 4 instructions implementing the add
 * instruction, using '+' in C. Generates both the DST += SRC and DST += IMM */
#define ALU(OPCODE, OP)         \
    case BPF_INSTRUCTION_ALU64_ ## OPCODE ## _REG:         \
        DST = DST OP SRC;       \
        break;                   \
    case BPF_INSTRUCTION_ALU64_ ## OPCODE ## _IMM:       \
        DST = DST OP IMM;       \
        break;                   \
    case BPF_INSTRUCTION_ALU32_ ## OPCODE ## _REG:         \
        DST = (uint32_t)DST OP(uint32_t) SRC;   \
        break;                   \
    case BPF_INSTRUCTION_ALU32_ ## OPCODE ## _IMM:           \
        DST = (uint32_t)DST OP(uint32_t) IMM;   \
        break;
#else
#define ALU(OPCODE, OP)         \
    case BPF_INSTRUCTION_ALU64_ ## OPCODE ## _REG:         \
        DST = DST OP SRC;       \
        break;                   \
    case BPF_INSTRUCTION_ALU64_ ## OPCODE ## _IMM:       \
        DST = DST OP IMM;       \
        break;
#endif

/* Generate jump type instructions, similar to the ALU instructions */
#define COND_JMP(SIGN, OPCODE, CMP_OP)              \
    case BPF_INSTRUCTION_JMP_ ## OPCODE ## _REG:                  \
        { \
            bool jump_cond = (SIGN ## nt64_t)DST CMP_OP(SIGN ## nt64_t) SRC; \
            CONT_JUMP;                           \
        } \
    case BPF_INSTRUCTION_JMP_ ## OPCODE ## _IMM:                 \
        { \
            bool jump_cond = (SIGN ## nt64_t)DST CMP_OP(SIGN ## nt64_t) IMM; \
            CONT_JUMP;                           \
        } \

/* Generate all the different regular load variants */
#define MEM(SIZEOP, SIZE)                     \
    case BPF_INSTRUCTION_MEM_STX ## SIZEOP:                       \
        if (!_check_store(rbpf, DST + (*instr)->offset, sizeof(SIZE))) { \
            return RBPF_ILLEGAL_MEM; \
        } \
        *(SIZE *)(uintptr_t)(DST + (*instr)->offset) = SRC;   \
        break;                               \
    case BPF_INSTRUCTION_MEM_ST ## SIZEOP:                      \
        if (!_check_store(rbpf, DST + (*instr)->offset, sizeof(SIZE))) { \
            return RBPF_ILLEGAL_MEM; \
        } \
        *(SIZE *)(uintptr_t)(DST + (*instr)->offset) = IMM;   \
        break;                               \
    case BPF_INSTRUCTION_MEM_LDX ## SIZEOP:                      \
        if (!_check_load(rbpf, SRC + (*instr)->offset, sizeof(SIZE))) { \
            return RBPF_ILLEGAL_MEM; \
        } \
        DST = *(const SIZE *)(uintptr_t)(SRC + (*instr)->offset);   \
        break;

static inline int _rbpf_over_max_jumps(const rbpf_application_t *rbpf)
{
    return !(rbpf->flags & RBPF_CONFIG_NO_RETURN) && rbpf->branches_remaining == 0;
}

/* Asking the compiler to really inline this call is the easiest way to speed up the VM */
static inline int __attribute__((always_inline)) _rbpf_jump(rbpf_application_t *rbpf, const bpf_instruction_t **instr)
{
    *instr += (*instr)->offset;
    rbpf->branches_remaining--;
    if (_rbpf_over_max_jumps(rbpf)) {
        return RBPF_OUT_OF_BRANCHES;
    }
    return RBPF_CONTINUE;
}

/* Asking the compiler to really inline this call is the easiest way to speed up the VM */
static inline int __attribute__((always_inline)) _rbpf_instruction(rbpf_application_t *rbpf,
                                    const bpf_instruction_t **instr, uint64_t regmap[11])
{
    switch ((*instr)->opcode) {

    /* Macros implementing the instruction code for the simple ALU(32|64) based operations */
    ALU(ADD,  +)
    ALU(SUB,  -)
    ALU(AND,  &)
    ALU(OR,   |)
    ALU(LSH, <<)
    ALU(RSH, >>)
    ALU(XOR,  ^)
    ALU(MUL,  *)

    /* These need additional checks inside */
    case BPF_INSTRUCTION_ALU64_MOD_REG:
        if (SRC == 0) {
            return RBPF_ILLEGAL_DIV;
        }
        DST = DST % SRC;
        break;
    case BPF_INSTRUCTION_ALU64_MOD_IMM:
        if (IMM == 0) {
            return RBPF_ILLEGAL_DIV;
        }
        DST = DST % IMM;
        break;
#if (RBPF_ENABLE_ALU32)
    case BPF_INSTRUCTION_ALU32_MOD_REG:
        if (SRC == 0) {
            return RBPF_ILLEGAL_DIV;
        }
        DST = (uint32_t)DST % (uint32_t)SRC;
        break;
    case BPF_INSTRUCTION_ALU32_MOD_IMM:
        if (IMM == 0) {
            return RBPF_ILLEGAL_DIV;
        }
        DST = (uint32_t)DST % (uint32_t)IMM;
        break;
#endif

    /* These need additional checks inside */
    case BPF_INSTRUCTION_ALU64_DIV_REG:
        if (SRC == 0) {
            return RBPF_ILLEGAL_DIV;
        }
        DST = DST / SRC;
        break;
    case BPF_INSTRUCTION_ALU64_DIV_IMM:
        if (IMM == 0) {
            return RBPF_ILLEGAL_DIV;
        }
        DST = DST / IMM;
        break;
#if (RBPF_ENABLE_ALU32)
    case BPF_INSTRUCTION_ALU32_DIV_REG:
        if (SRC == 0) {
            return RBPF_ILLEGAL_DIV;
        }
        DST = (uint32_t)DST / (uint32_t)SRC;
        break;
    case BPF_INSTRUCTION_ALU32_DIV_IMM:
        if (IMM == 0) {
            return RBPF_ILLEGAL_DIV;
        }
        DST = (uint32_t)DST / (uint32_t)IMM;
        break;
#endif

    /* These only have an immediate argument variant */
    case BPF_INSTRUCTION_ALU64_NEG_IMM:
        DST = -(int64_t)DST;
        break;

#if (RBPF_ENABLE_ALU32)
    case BPF_INSTRUCTION_ALU32_NEG_IMM:
        DST = -(int32_t)DST;
        break;

    /* MOV doesn't have an operation associated (breaks the pattern) */
    case BPF_INSTRUCTION_ALU32_MOV_IMM:
        DST = (uint32_t)IMM;
        break;
    case BPF_INSTRUCTION_ALU32_MOV_REG:
        DST = (uint32_t)SRC;
        break;
#endif
    case BPF_INSTRUCTION_ALU64_MOV_IMM:
        DST = IMM;
        break;
    case BPF_INSTRUCTION_ALU64_MOV_REG:
        DST = SRC;
        break;

    /* Arithmetic shift also don't really fit the pattern */
    case BPF_INSTRUCTION_ALU64_ARSH_REG:
        (*(int64_t *)&DST) >>= SRC;
        break;
    case BPF_INSTRUCTION_ALU64_ARSH_IMM:
        (*(int64_t *)&DST) >>= IMM;
        break;
#if (RBPF_ENABLE_ALU32)
    case BPF_INSTRUCTION_ALU32_ARSH_REG:
        DST = (int32_t)DST >> SRC;
        break;
    case BPF_INSTRUCTION_ALU32_ARSH_IMM:
        DST =  (int32_t)DST >> IMM;
        break;
#endif

    /* Double word memory load, takes up two instructions, but acts as one */
    case BPF_INSTRUCTION_MEM_LDDW:
        DST = (uint64_t)(*instr)->immediate;
        DST |= ((uint64_t)(((*instr) + 1)->immediate)) << 32;
        (*instr)++;
        break;

    /* Custom instruction to load an address as double word relative to the application data.
     * Takes up two instructions, but acts as one */
    case BPF_INSTRUCTION_MEM_LDDWD:
        DST = (intptr_t)rbpf_application_data(rbpf);
        DST += (uint64_t)(*instr)->immediate;
        DST += ((uint64_t)(((*instr) + 1)->immediate)) << 32;
        (*instr)++;
        break;

    /* Custom instruction to load an address as double word relative to the application rodata.
     * Takes up two instructions, but acts as one */
    case BPF_INSTRUCTION_MEM_LDDWR:
        DST = (intptr_t)rbpf_application_rodata(rbpf);
        DST += (uint64_t)(*instr)->immediate;
        DST += ((uint64_t)(((*instr) + 1)->immediate)) << 32;
        (*instr)++;
        break;

/* Regular memory instructions with different sizes */
        MEM(B, uint8_t)
        MEM(H, uint16_t)
        MEM(W, uint32_t)
        MEM(DW, uint64_t)

    case BPF_INSTRUCTION_JMP_ALWAYS:
        return _rbpf_jump(rbpf, instr);

        /* generate jump instructions */
        COND_JMP(ui, EQ, ==)
        COND_JMP(ui, GT, >)
        COND_JMP(ui, GE, >=)
        COND_JMP(ui, LT, <)
        COND_JMP(ui, LE, <=)
        COND_JMP(ui, SET, &)
        COND_JMP(ui, NE, !=)
        COND_JMP(i, SGT, >)
        COND_JMP(i, SGE, >=)
        COND_JMP(i, SLT, <)
        COND_JMP(i, SLE, <=)

    case BPF_INSTRUCTION_CALL:
    {
        rbpf_call_t call = _rbpf_get_call((*instr)->immediate);
        if (call) {
            regmap[0] = (*(call))(rbpf,
                                  regmap);
            break;
        }
        else {
            return RBPF_ILLEGAL_CALL;
        }
    }
    case BPF_INSTRUCTION_RETURN:
        return RBPF_OK;

    default:
        return RBPF_ILLEGAL_INSTRUCTION;
    }
    return RBPF_CONTINUE;
}

int rbpf_engine_run(rbpf_application_t *rbpf, const void *ctx, int64_t *result)
{
    int res = RBPF_OK;

    rbpf->branches_remaining = RBPF_BRANCHES_ALLOWED;
    uint64_t regmap[11] = { 0 };

    regmap[1] = (uint64_t)(uintptr_t)ctx;
    regmap[10] = (uint64_t)(uintptr_t)(rbpf->stack + RBPF_STACK_SIZE);

    const bpf_instruction_t *instr = (const bpf_instruction_t *)rbpf_application_text(rbpf);

    res = rbpf_application_verify_preflight(rbpf);
    if (res < 0) {
        return res;
    }

    do {
        res = _rbpf_instruction(rbpf, &instr, regmap);
        instr++;
    } while (res > 0);
    *result = regmap[0];
    return res;
}

