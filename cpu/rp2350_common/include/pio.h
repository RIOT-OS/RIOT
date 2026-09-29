/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>

/**
 * @defgroup    cpu_rp2350_pio PIO Instructions
 * @ingroup     cpu_rp2350
 * @brief       Macros for PIO (Programmable I/O) instructions
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* JMP Instruction - Section 11.4.2                                           */
/* ========================================================================== */

/**
 * @name JMP instruction conditions
 * @{
 */
#define PIO_JMP_COND_ALWAYS     (0)     /**< Always jump */
#define PIO_JMP_COND_X_ZERO     (1)     /**< Jump if X is zero (!X) */
#define PIO_JMP_COND_X_DEC      (2)     /**< Jump if X non-zero pre-decrement (X--) */
#define PIO_JMP_COND_Y_ZERO     (3)     /**< Jump if Y is zero (!Y) */
#define PIO_JMP_COND_Y_DEC      (4)     /**< Jump if Y non-zero pre-decrement (Y--) */
#define PIO_JMP_COND_X_NE_Y     (5)     /**< Jump if X not equal Y */
#define PIO_JMP_COND_PIN        (6)     /**< Jump if input pin is high */
#define PIO_JMP_COND_OSRE_NOT_EMPTY (7) /**< Jump if OSR not empty (!OSRE) */
/** @} */

/**
 * @brief JMP instruction encoding
 *
 * Set program counter to address if condition is true.
 *
 * @param[in] cond      Condition (PIO_JMP_COND_*)
 * @param[in] addr      Target address (0-31)
 */
#define PIO_JMP(cond, addr) \
    (0b0000000000000000 | (((cond) & 0b111) << 5) | ((addr) & 0b11111))

/**
 * @brief JMP - unconditional jump
 *
 * @param[in] addr  Target address (0-31)
 */
#define PIO_JMP_ALWAYS(addr)    PIO_JMP(PIO_JMP_COND_ALWAYS, (addr))

/**
 * @brief JMP !X - jump if X is zero
 *
 * @param[in] addr  Target address (0-31)
 */
#define PIO_JMP_X_ZERO(addr)    PIO_JMP(PIO_JMP_COND_X_ZERO, (addr))

/**
 * @brief JMP X-- - jump if X non-zero, then decrement X
 *
 * @param[in] addr  Target address (0-31)
 */
#define PIO_JMP_X_DEC(addr)     PIO_JMP(PIO_JMP_COND_X_DEC, (addr))

/**
 * @brief JMP !Y - jump if Y is zero
 *
 * @param[in] addr  Target address (0-31)
 */
#define PIO_JMP_Y_ZERO(addr)    PIO_JMP(PIO_JMP_COND_Y_ZERO, (addr))

/**
 * @brief JMP Y-- - jump if Y non-zero, then decrement Y
 *
 * @param[in] addr  Target address (0-31)
 */
#define PIO_JMP_Y_DEC(addr)     PIO_JMP(PIO_JMP_COND_Y_DEC, (addr))

/**
 * @brief JMP X!=Y - jump if X not equal to Y
 *
 * @param[in] addr  Target address (0-31)
 */
#define PIO_JMP_X_NE_Y(addr)    PIO_JMP(PIO_JMP_COND_X_NE_Y, (addr))

/**
 * @brief JMP PIN - jump if input pin is high
 *
 * @param[in] addr  Target address (0-31)
 */
#define PIO_JMP_PIN(addr)       PIO_JMP(PIO_JMP_COND_PIN, (addr))

/**
 * @brief JMP !OSRE - jump if OSR not empty
 *
 * @param[in] addr  Target address (0-31)
 */
#define PIO_JMP_OSRE_NOT_EMPTY(addr) PIO_JMP(PIO_JMP_COND_OSRE_NOT_EMPTY, (addr))

/* ========================================================================== */
/* WAIT Instruction - Section 11.4.3                                          */
/* ========================================================================== */

/**
 * @name WAIT instruction sources
 * @{
 */
#define PIO_WAIT_SRC_GPIO       (0)     /**< Wait on GPIO (absolute) */
#define PIO_WAIT_SRC_PIN        (1)     /**< Wait on PIN (mapped) */
#define PIO_WAIT_SRC_IRQ        (2)     /**< Wait on IRQ flag */
#define PIO_WAIT_SRC_JMPPIN     (3)     /**< Wait on JMPPIN */
/** @} */

/**
 * @name WAIT IRQ index modes (decoded from 2 MSBs of index)
 * @{
 */
#define PIO_WAIT_IRQ_IDXMODE_DIRECT (0) /**< Direct indexing (0-7) */
#define PIO_WAIT_IRQ_IDXMODE_PREV   (1) /**< Previous PIO block */
#define PIO_WAIT_IRQ_IDXMODE_REL    (2) /**< Relative (add state machine ID) */
#define PIO_WAIT_IRQ_IDXMODE_NEXT   (3) /**< Next PIO block */
/** @} */

/**
 * @brief WAIT instruction encoding
 *
 * Stall until some condition is met.
 *
 * @param[in] pol       Polarity: 1 to wait for 1, 0 to wait for 0
 * @param[in] src       Source (PIO_WAIT_SRC_*)
 * @param[in] index     Index value (interpretation depends on source)
 */
#define PIO_WAIT(pol, src, index) \
    (0b0010000000000000 | ((pol) ? 0b0000000010000000 : 0) | \
     (((src) & 0b11) << 5) | ((index) & 0b11111))

/**
 * @brief WAIT GPIO - wait on absolute GPIO pin
 *
 * @param[in] pol   Polarity (0 or 1)
 * @param[in] gpio  GPIO number
 */
#define PIO_WAIT_GPIO(pol, gpio) PIO_WAIT((pol), PIO_WAIT_SRC_GPIO, (gpio))

/**
 * @brief WAIT PIN - wait on mapped input pin
 *
 * @param[in] pol   Polarity (0 or 1)
 * @param[in] pin   Pin number (mapped via PINCTRL_IN_BASE)
 */
#define PIO_WAIT_PIN(pol, pin)   PIO_WAIT((pol), PIO_WAIT_SRC_PIN, (pin))

/**
 * @brief WAIT IRQ - wait on IRQ flag
 *
 * @param[in] pol       Polarity (0 or 1)
 * @param[in] idxmode   Index mode (PIO_WAIT_IRQ_IDXMODE_*)
 * @param[in] irq       IRQ number (0-7)
 */
#define PIO_WAIT_IRQ(pol, idxmode, irq) \
    PIO_WAIT((pol), PIO_WAIT_SRC_IRQ, (((idxmode) & 0b11) << 3) | ((irq) & 0b111))

/**
 * @brief WAIT IRQ with relative addressing
 *
 * @param[in] pol   Polarity (0 or 1)
 * @param[in] irq   IRQ number (0-7), low 2 bits added to state machine ID
 */
#define PIO_WAIT_IRQ_REL(pol, irq) \
    PIO_WAIT_IRQ((pol), PIO_WAIT_IRQ_IDXMODE_REL, (irq))

/**
 * @brief WAIT IRQ on previous PIO block
 *
 * @param[in] pol   Polarity (0 or 1)
 * @param[in] irq   IRQ number (0-7)
 */
#define PIO_WAIT_IRQ_PREV(pol, irq) \
    PIO_WAIT_IRQ((pol), PIO_WAIT_IRQ_IDXMODE_PREV, (irq))

/**
 * @brief WAIT IRQ on next PIO block
 *
 * @param[in] pol   Polarity (0 or 1)
 * @param[in] irq   IRQ number (0-7)
 */
#define PIO_WAIT_IRQ_NEXT(pol, irq) \
    PIO_WAIT_IRQ((pol), PIO_WAIT_IRQ_IDXMODE_NEXT, (irq))

/**
 * @brief WAIT JMPPIN - wait on jump pin
 *
 * @param[in] pol       Polarity (0 or 1)
 * @param[in] offset    Offset added to PINCTRL_JMP_PIN (0-3)
 */
#define PIO_WAIT_JMPPIN(pol, offset) \
    PIO_WAIT((pol), PIO_WAIT_SRC_JMPPIN, (offset))

/* ========================================================================== */
/* IN Instruction - Section 11.4.4                                            */
/* ========================================================================== */

/**
 * @name IN instruction sources
 * @{
 */
#define PIO_IN_SRC_PINS         (0)     /**< PINS */
#define PIO_IN_SRC_X            (1)     /**< Scratch register X */
#define PIO_IN_SRC_Y            (2)     /**< Scratch register Y */
#define PIO_IN_SRC_NULL         (3)     /**< NULL (all zeroes) */
#define PIO_IN_SRC_ISR          (6)     /**< Input shift register */
#define PIO_IN_SRC_OSR          (7)     /**< Output shift register */
/** @} */

/**
 * @brief IN instruction encoding
 *
 * Shift bit count bits from source into the ISR.
 *
 * @param[in] src       Source (PIO_IN_SRC_*)
 * @param[in] bitcount  Number of bits to shift (1-32, 32 encoded as 0)
 */
#define PIO_IN(src, bitcount) \
    (0b0100000000000000 | (((src) & 0b111) << 5) | ((bitcount) & 0b11111))

/**
 * @brief IN PINS - shift bits from pins into ISR
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_IN_PINS(bits)       PIO_IN(PIO_IN_SRC_PINS, (bits) & 0b11111)

/**
 * @brief IN X - shift bits from X into ISR
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_IN_X(bits)          PIO_IN(PIO_IN_SRC_X, (bits) & 0b11111)

/**
 * @brief IN Y - shift bits from Y into ISR
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_IN_Y(bits)          PIO_IN(PIO_IN_SRC_Y, (bits) & 0b11111)

/**
 * @brief IN NULL - shift zero bits into ISR
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_IN_NULL(bits)       PIO_IN(PIO_IN_SRC_NULL, (bits) & 0b11111)

/**
 * @brief IN ISR - shift bits from ISR into ISR
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_IN_ISR(bits)        PIO_IN(PIO_IN_SRC_ISR, (bits) & 0b11111)

/**
 * @brief IN OSR - shift bits from OSR into ISR
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_IN_OSR(bits)        PIO_IN(PIO_IN_SRC_OSR, (bits) & 0b11111)

/* ========================================================================== */
/* OUT Instruction - Section 11.4.5                                           */
/* ========================================================================== */

/**
 * @name OUT instruction destinations
 * @{
 */
#define PIO_OUT_DEST_PINS       (0)     /**< PINS */
#define PIO_OUT_DEST_X          (1)     /**< Scratch register X */
#define PIO_OUT_DEST_Y          (2)     /**< Scratch register Y */
#define PIO_OUT_DEST_NULL       (3)     /**< NULL (discard data) */
#define PIO_OUT_DEST_PINDIRS    (4)     /**< PINDIRS */
#define PIO_OUT_DEST_PC         (5)     /**< Program counter */
#define PIO_OUT_DEST_ISR        (6)     /**< Input shift register */
#define PIO_OUT_DEST_EXEC       (7)     /**< Execute as instruction */
/** @} */

/**
 * @brief OUT instruction encoding
 *
 * Shift bit count bits out of the OSR and write to destination.
 *
 * @param[in] dest      Destination (PIO_OUT_DEST_*)
 * @param[in] bitcount  Number of bits to shift (1-32, 32 encoded as 0)
 */
#define PIO_OUT(dest, bitcount) \
    (0b0110000000000000 | (((dest) & 0b111) << 5) | ((bitcount) & 0b11111))

/**
 * @brief OUT PINS - shift bits from OSR to pins
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_OUT_PINS(bits)      PIO_OUT(PIO_OUT_DEST_PINS, (bits) & 0b11111)

/**
 * @brief OUT X - shift bits from OSR to X
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_OUT_X(bits)         PIO_OUT(PIO_OUT_DEST_X, (bits) & 0b11111)

/**
 * @brief OUT Y - shift bits from OSR to Y
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_OUT_Y(bits)         PIO_OUT(PIO_OUT_DEST_Y, (bits) & 0b11111)

/**
 * @brief OUT NULL - discard bits from OSR
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_OUT_NULL(bits)      PIO_OUT(PIO_OUT_DEST_NULL, (bits) & 0b11111)

/**
 * @brief OUT PINDIRS - shift bits from OSR to pin directions
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_OUT_PINDIRS(bits)   PIO_OUT(PIO_OUT_DEST_PINDIRS, (bits) & 0b11111)

/**
 * @brief OUT PC - shift bits from OSR to program counter (jump)
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_OUT_PC(bits)        PIO_OUT(PIO_OUT_DEST_PC, (bits) & 0b11111)

/**
 * @brief OUT ISR - shift bits from OSR to ISR
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_OUT_ISR(bits)       PIO_OUT(PIO_OUT_DEST_ISR, (bits) & 0b11111)

/**
 * @brief OUT EXEC - execute bits from OSR as instruction
 *
 * @param[in] bits  Number of bits (1-32)
 */
#define PIO_OUT_EXEC(bits)      PIO_OUT(PIO_OUT_DEST_EXEC, (bits) & 0b11111)

/* ========================================================================== */
/* PUSH Instruction - Section 11.4.6                                          */
/* ========================================================================== */

/**
 * @brief PUSH instruction encoding
 *
 * Push the contents of the ISR into the RX FIFO as a single 32-bit word.
 * Clear ISR to all-zeroes.
 *
 * @param[in] iffull    If 1, do nothing unless shift count reached threshold
 * @param[in] block     If 1, stall execution if RX FIFO is full
 */
#define PIO_PUSH(iffull, block) \
    (0b1000000000000000 | ((iffull) ? 0b0000000001000000 : 0) | ((block) ? 0b0000000000100000 : 0))

/**
 * @brief PUSH instruction - block by default
 */
#define PIO_PUSH_BLOCK()        PIO_PUSH(0, 1)

/**
 * @brief PUSH instruction - non-blocking
 */
#define PIO_PUSH_NOBLOCK()      PIO_PUSH(0, 0)

/**
 * @brief PUSH instruction - if full, blocking
 */
#define PIO_PUSH_IFFULL()       PIO_PUSH(1, 1)

/**
 * @brief PUSH instruction - if full, non-blocking
 */
#define PIO_PUSH_IFFULL_NOBLOCK() PIO_PUSH(1, 0)

/* ========================================================================== */
/* PULL Instruction - Section 11.4.7                                          */
/* ========================================================================== */

/**
 * @brief PULL instruction encoding
 *
 * Load a 32-bit word from the TX FIFO into the OSR.
 *
 * @param[in] ifempty   If 1, do nothing unless shift count reached threshold
 * @param[in] block     If 1, stall if TX FIFO is empty. If 0, copy X to OSR
 */
#define PIO_PULL(ifempty, block) \
    (0b1000000010000000 | ((ifempty) ? 0b0000000001000000 : 0) | ((block) ? 0b0000000000100000 : 0))

/**
 * @brief PULL instruction - block by default
 */
#define PIO_PULL_BLOCK()        PIO_PULL(0, 1)

/**
 * @brief PULL instruction - non-blocking
 */
#define PIO_PULL_NOBLOCK()      PIO_PULL(0, 0)

/**
 * @brief PULL instruction - if empty, blocking
 */
#define PIO_PULL_IFEMPTY()      PIO_PULL(1, 1)

/**
 * @brief PULL instruction - if empty, non-blocking
 */
#define PIO_PULL_IFEMPTY_NOBLOCK() PIO_PULL(1, 0)

/* ========================================================================== */
/* MOV to RX FIFO - Section 11.4.8                                            */
/* ========================================================================== */

/**
 * @brief MOV to RX FIFO instruction encoding
 *
 * Write the ISR to a selected RX FIFO entry (indexed access).
 * Requires SHIFTCTRL_FJOIN_RX_PUT configuration.
 *
 * @param[in] idxi      If 1, index by immediate. If 0, index by Y register
 * @param[in] index     RX FIFO entry index (0-3) or 0 when indexing by Y
 */
#define PIO_MOV_RXFIFO_ISR(idxi, index) \
    (0b1000000000010000 | ((idxi) ? 0b0000000000010000 : 0) | ((index) & 0b0000000000001111))

/**
 * @brief MOV rxfifo[y], isr - index by Y register
 */
#define PIO_MOV_RXFIFO_Y_ISR()  PIO_MOV_RXFIFO_ISR(0, 0)

/**
 * @brief MOV rxfifo[index], isr - index by immediate value
 *
 * @param[in] idx   RX FIFO entry index (0-3)
 */
#define PIO_MOV_RXFIFO_IDX_ISR(idx) PIO_MOV_RXFIFO_ISR(1, (idx))

/* ========================================================================== */
/* MOV from RX FIFO - Section 11.4.9                                          */
/* ========================================================================== */

/**
 * @brief MOV from RX FIFO instruction encoding
 *
 * Read the selected RX FIFO entry into the OSR (indexed access).
 * Requires SHIFTCTRL_FJOIN_RX_GET configuration.
 *
 * @param[in] idxi      If 1, index by immediate. If 0, index by Y register
 * @param[in] index     RX FIFO entry index (0-3) or 0 when indexing by Y
 */
#define PIO_MOV_OSR_RXFIFO(idxi, index) \
    (0b1000000010010000 | ((idxi) ? 0b0000000000010000 : 0) | ((index) & 0b0000000000001111))

/**
 * @brief MOV osr, rxfifo[y] - index by Y register
 */
#define PIO_MOV_OSR_RXFIFO_Y()  PIO_MOV_OSR_RXFIFO(0, 0)

/**
 * @brief MOV osr, rxfifo[index] - index by immediate value
 *
 * @param[in] idx   RX FIFO entry index (0-3)
 */
#define PIO_MOV_OSR_RXFIFO_IDX(idx) PIO_MOV_OSR_RXFIFO(1, (idx))

/* ========================================================================== */
/* MOV Instruction - Section 11.4.10                                          */
/* ========================================================================== */

/**
 * @name MOV instruction destinations
 * @{
 */
#define PIO_MOV_DEST_PINS       (0)     /**< PINS (uses same pin mapping as OUT) */
#define PIO_MOV_DEST_X          (1)     /**< Scratch register X */
#define PIO_MOV_DEST_Y          (2)     /**< Scratch register Y */
#define PIO_MOV_DEST_PINDIRS    (3)     /**< PINDIRS (uses same pin mapping as OUT) */
#define PIO_MOV_DEST_EXEC       (4)     /**< Execute data as instruction */
#define PIO_MOV_DEST_PC         (5)     /**< Program counter */
#define PIO_MOV_DEST_ISR        (6)     /**< Input shift register */
#define PIO_MOV_DEST_OSR        (7)     /**< Output shift register */
/** @} */

/**
 * @name MOV instruction operations
 * @{
 */
#define PIO_MOV_OP_NONE         (0)     /**< No operation */
#define PIO_MOV_OP_INVERT       (1)     /**< Bitwise complement */
#define PIO_MOV_OP_BITREV       (2)     /**< Bit-reverse */
/** @} */

/**
 * @name MOV instruction sources
 * @{
 */
#define PIO_MOV_SRC_PINS        (0)     /**< PINS (uses same pin mapping as IN) */
#define PIO_MOV_SRC_X           (1)     /**< Scratch register X */
#define PIO_MOV_SRC_Y           (2)     /**< Scratch register Y */
#define PIO_MOV_SRC_NULL        (3)     /**< NULL (all zeroes) */
#define PIO_MOV_SRC_STATUS      (5)     /**< STATUS (configured by EXECCTRL_STATUS_SEL) */
#define PIO_MOV_SRC_ISR         (6)     /**< Input shift register */
#define PIO_MOV_SRC_OSR         (7)     /**< Output shift register */
/** @} */

/**
 * @brief MOV instruction encoding
 *
 * Copy data from Source to Destination.
 *
 * @param[in] dest  Destination (PIO_MOV_DEST_*)
 * @param[in] op    Operation (PIO_MOV_OP_*)
 * @param[in] src   Source (PIO_MOV_SRC_*)
 */
#define PIO_MOV(dest, op, src) \
    (0b1010000000000000 | (((dest) & 0b111) << 5) | (((op) & 0b11) << 3) | ((src) & 0b111))

/* ========================================================================== */
/* IRQ Instruction - Section 11.4.11                                          */
/* ========================================================================== */

/**
 * @name IRQ instruction index modes
 * @{
 */
#define PIO_IRQ_IDXMODE_DIRECT  (0)     /**< Direct indexing (0-7) */
#define PIO_IRQ_IDXMODE_PREV    (1)     /**< Previous PIO block */
#define PIO_IRQ_IDXMODE_REL     (2)     /**< Relative (add state machine ID) */
#define PIO_IRQ_IDXMODE_NEXT    (3)     /**< Next PIO block */
/** @} */

/**
 * @brief IRQ instruction encoding
 *
 * Set or clear the IRQ flag selected by index.
 *
 * @param[in] clr       If 1, clear the flag instead of raising it
 * @param[in] wait      If 1, halt until the raised flag is lowered again
 * @param[in] idxmode   Index mode (PIO_IRQ_IDXMODE_*)
 * @param[in] index     IRQ index (0-7)
 */
#define PIO_IRQ(clr, wait, idxmode, index) \
    (0b1100000000000000 | ((clr) ? 0b0000000001000000 : 0) | ((wait) ? 0b0000000000100000 : 0) | \
     (((idxmode) & 0b11) << 3) | ((index) & 0b111))

/**
 * @brief IRQ set - raise IRQ without waiting
 *
 * @param[in] num   IRQ number (0-7)
 */
#define PIO_IRQ_SET(num)        PIO_IRQ(0, 0, PIO_IRQ_IDXMODE_DIRECT, (num))

/**
 * @brief IRQ nowait - raise IRQ without waiting (same as SET)
 *
 * @param[in] num   IRQ number (0-7)
 */
#define PIO_IRQ_NOWAIT(num)     PIO_IRQ(0, 0, PIO_IRQ_IDXMODE_DIRECT, (num))

/**
 * @brief IRQ wait - raise IRQ and wait for it to be cleared
 *
 * @param[in] num   IRQ number (0-7)
 */
#define PIO_IRQ_WAIT(num)       PIO_IRQ(0, 1, PIO_IRQ_IDXMODE_DIRECT, (num))

/**
 * @brief IRQ clear - clear IRQ flag
 *
 * @param[in] num   IRQ number (0-7)
 */
#define PIO_IRQ_CLEAR(num)      PIO_IRQ(1, 0, PIO_IRQ_IDXMODE_DIRECT, (num))

/**
 * @brief IRQ set with relative addressing
 *
 * @param[in] num   IRQ number (0-7), low 2 bits added to state machine ID
 */
#define PIO_IRQ_SET_REL(num)    PIO_IRQ(0, 0, PIO_IRQ_IDXMODE_REL, (num))

/**
 * @brief IRQ wait with relative addressing
 *
 * @param[in] num   IRQ number (0-7), low 2 bits added to state machine ID
 */
#define PIO_IRQ_WAIT_REL(num)   PIO_IRQ(0, 1, PIO_IRQ_IDXMODE_REL, (num))

/**
 * @brief IRQ set targeting previous PIO block
 *
 * @param[in] num   IRQ number (0-7)
 */
#define PIO_IRQ_SET_PREV(num)   PIO_IRQ(0, 0, PIO_IRQ_IDXMODE_PREV, (num))

/**
 * @brief IRQ set targeting next PIO block
 *
 * @param[in] num   IRQ number (0-7)
 */
#define PIO_IRQ_SET_NEXT(num)   PIO_IRQ(0, 0, PIO_IRQ_IDXMODE_NEXT, (num))

/* ========================================================================== */
/* SET Instruction - Section 11.4.12                                          */
/* ========================================================================== */

/**
 * @name SET instruction destinations
 * @{
 */
#define PIO_SET_DEST_PINS       (0)     /**< PINS */
#define PIO_SET_DEST_X          (1)     /**< Scratch register X */
#define PIO_SET_DEST_Y          (2)     /**< Scratch register Y */
#define PIO_SET_DEST_PINDIRS    (4)     /**< PINDIRS */
/** @} */

/**
 * @brief SET instruction encoding
 *
 * Write immediate value to destination.
 *
 * @param[in] dest  Destination (PIO_SET_DEST_*)
 * @param[in] data  5-bit immediate value (0-31)
 */
#define PIO_SET(dest, data) \
    (0b1110000000000000 | (((dest) & 0b111) << 5) | ((data) & 0b11111))

/**
 * @brief SET pins - set pin values
 *
 * @param[in] val   Value to set (0-31)
 */
#define PIO_SET_PINS(val)       PIO_SET(PIO_SET_DEST_PINS, (val))

/**
 * @brief SET x - set scratch register X
 *
 * @param[in] val   Value to set (0-31)
 */
#define PIO_SET_X(val)          PIO_SET(PIO_SET_DEST_X, (val))

/**
 * @brief SET y - set scratch register Y
 *
 * @param[in] val   Value to set (0-31)
 */
#define PIO_SET_Y(val)          PIO_SET(PIO_SET_DEST_Y, (val))

/**
 * @brief SET pindirs - set pin directions
 *
 * @param[in] val   Value to set (0-31)
 */
#define PIO_SET_PINDIRS(val)    PIO_SET(PIO_SET_DEST_PINDIRS, (val))

#ifdef __cplusplus
}
#endif

/** @} */
