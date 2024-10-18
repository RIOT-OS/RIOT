/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_pio Programmable IO (PIO)
 * @ingroup     drivers_periph
 * @brief       Low-level PIO peripheral driver
 *
 * Programmable IO (PIO) is a cycle accurate IO control interface to emulate
 * common IO standards like: I2C, SPI, UART or custom wire protocols as for the
 * well known WS2812B LEDs.
 * PIO programs are written in an MCU specific assembly language and they are
 * saved in .pio files. The programs have to be loaded in the PIOs instruction
 * memory. Once written to memory, a program can be executed by a state machines.
 * A state machine can only execute one program at a time but multiple state
 * machines can run the same program as access to instruction memory is shared.
 * The user is responsible to allocate/deallocate instruction memory and
 * lock/unlock state machines.
 *
 * PIO programs are very versatile and execution may depend on hardware quirks,
 * thus specific setup must be performed by the CPU PIO driver implementation
 * and the program code itself.
 * The CPU driver implementation provides a set of known programs for common
 * IO standards but user written programs are also possible and should be placed
 * in cpu/$(CPU_FAM)/pio to make use of the processor specific PIO interface.
 *
 * This API simply takes a look at PIO programs as something that resides in
 * memory and can be executed on demand, by a state machine.
 *
 * @experimental    This API is experimental and in an early state -
 *                  expect changes!
 * @{
 * @file
 * @brief       High-level PIO peripheral driver interface
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef PERIPH_PIO_H
#define PERIPH_PIO_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default PIO device access macro
 */
#ifndef PIO_DEV
#define PIO_DEV(x)          (x)
#endif

/**
 * @brief   Program location that states that the program
 *          has not been loaded into instruction memory
 */
#define PIO_PROGRAM_NOT_LOADED      (-1)

/**
 * @brief   Struct holding functions to be called on PIO interrupt requests
 *
 * The interrupt events and number of interrupts may be hardware dependent.
 */
struct pio_isr_vec;

/**
 * @brief   PIO index type
 */
typedef unsigned pio_t;

/**
 * @brief   PIO state machine index type
 */
typedef int pio_sm_t;

/**
 * @brief   Struct that models a PIO program
 */
typedef struct {
    unsigned instr_numof; /**< Number of instructions */
    int location;         /**< Program location in memory */
    bool written;         /**< Program was already written */
} pio_program_t;

/**
 * @brief   Initialize a PIO device
 *
 * @param[in]       pio         PIO index
 *
 * @note    No initialization is performed if
 *          "DISABLE_MODULE += periph_init_pio" is set.
 */
void pio_init(pio_t pio);

/**
 * @brief   Start automatically configured PIO programs
 *
 * @note    No execution is automatically started if
 *          "DISABLE_MODULE += periph_init_pio" is set.
 */
void pio_start_programs(void);

/**
 * @brief   Get exclusive access to one of the state machines of PIO @p pio
 *
 * @param[in]       pio         PIO index
 *
 * @return  Success: Index of now locked state machine
 *          Failure: negative integer
 */
pio_sm_t pio_sm_lock(pio_t pio);

/**
 * @brief   Release a previously locked state machine
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 */
void pio_sm_unlock(pio_t pio, pio_sm_t sm);

/**
 * @brief   Start a state machine and execute the previously loaded program
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 */
void pio_sm_start(pio_t pio, pio_sm_t sm);

/**
 * @brief   Stop the execution of a program
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 */
void pio_sm_stop(pio_t pio, pio_sm_t sm);

/**
 * @brief   Allocate the required instruction memory
 *          to load the given PIO program into
 *
 * @param[in]       pio         PIO index
 * @param[in, out]  prog        Pointer PIO program
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_alloc_program(pio_t pio, pio_program_t *prog);

/**
 * @brief   Convenience function which performs the usual PIO program
 *          resource acquisition.
 *
 *          @ref pio_alloc_program
 *          @ref pio_sm_lock
 *
 * @param[out]      pio_ptr     Destination to store the PIO index
 * @param[out]      sm_ptr      Destination to store the PIO state machine index
 * @param[in, out]  program     Program pointer of the program to be allocated
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_alloc_program_sm_lock_any(pio_t *pio_ptr, pio_sm_t *sm_ptr, pio_program_t *program);

/**
 * @brief   Release the allocated instruction memory occupied by @p prog
 *
 * @param[in]       pio         PIO index
 * @param[in]       prog        PIO program
 */
void pio_free_program(pio_t pio, pio_program_t *prog);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_PIO_H */
