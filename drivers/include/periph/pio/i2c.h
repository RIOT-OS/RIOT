/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_periph_pio_i2c PIO I2C program
 * @ingroup     drivers_periph
 * @brief       PIO I2C program interface
 *
 * @experimental    This API is experimental and in an early state -
 *                  expect changes!
 * @{
 * @file
 * @brief       PIO I2C program interface
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include "periph/i2c.h"
#include "periph/pio.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   PIO I2C descriptor type compatible with @ref i2c_t
 */
typedef i2c_t pio_i2c_t;

/**
 * @brief   PIO I2C program type
 */
typedef struct pio_program_i2c {
    pio_program_t base; /**< PIO base program */
    unsigned ref_mask;  /**< Mask of referencing PIO state machines */
} pio_program_i2c_t;

/**
 * @brief   PIO I2C emulated bus type
 */
typedef struct pio_i2c_bus {
    pio_t pio;              /**< PIO index */
    pio_sm_t sm;            /**< State machine index */
    mutex_t mtx;            /**< Mutex to protect the bus from concurrent accesses */
} pio_i2c_bus_t;

/**
 * @brief   Get access to a PIO I2C instance configured with PIO_I2C_CONFIG
 *
 * @param[in]       id      PIO I2C ID
 *
 * @return  PIO I2C objects
 */
pio_i2c_bus_t *pio_i2c_get(pio_i2c_t id);

/**
 * @brief   Query the number of PIO I2C instances configured with PIO_I2C_CONFIG
 *
 * @return  Number of PIO I2C instances
 */
unsigned pio_i2c_numof(void);

/**
 * @brief   Get const I2C program reference
 *
 * @param[in]       pio         PIO index
 *
 * @return  PIO I2C program allocated to PIO @p pio
 */
const pio_program_i2c_t *pio_i2c_get_program(pio_t pio);

/**
 * @brief   Create, allocate, and write a PIO I2C program
 *
 * This function does nothing if the program is already
 * created, allocated, and written.
 *
 * @param[in]       pio         PIO index
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_i2c_init_program(pio_t pio);

/**
 * @brief   Free a PIO I2C program
 *
 * @param[in]       pio         PIO index
 */
void pio_i2c_deinit_program(pio_t pio);

/**
 * @brief   Acquire a PIO state machine of PIO @p pio to run the PIO I2C program
 *
 * @param[in]       pio         PIO index
 * @param[out]      i2c         PIO I2C bus
 *
 * @return  A valid state machine index or a negative number on error
 */
pio_sm_t pio_i2c_sm_lock(pio_t pio, pio_i2c_bus_t *i2c);

/**
 * @brief   Release a PIO state machine of PIO @p pio
 *
 * @param[in, out]  i2c         PIO I2C bus
 */
void pio_i2c_sm_unlock(pio_i2c_bus_t *i2c);

/**
 * @brief   Start PIO I2C programs configured with PIO_I2C_CONFIG
 *
 * @note    No execution is started if
 *          "DISABLE_MODULE += pio_autostart_i2c" is set
 */
void pio_i2c_start_programs(void);

/**
 * @brief   Stop PIO I2C programs configured with PIO_I2C_CONFIG
 */
void pio_i2c_stop_programs(void);

/**
 * @brief   Write a PIO I2C program to instruction memory
 *
 * @param[in]       pio         PIO index
 * @param[in, out]  pro         Allocated PIO I2C program
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_i2c_write_program(pio_t pio, pio_program_i2c_t *pro);

/**
 * @brief   Setup a state machine to run the I2C program
 *
 * @pre     The program @p pro must have been allocated.
 *
 * @param[out]      bus         PIO I2C bus
 * @param[in]       pro         Shared program base
 * @param[in]       sda         SDA pin
 * @param[in]       scl         SCL pin
 * @param[in]       irq         IRQ line, 0 or 1
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_i2c_init(pio_i2c_bus_t *bus,
                 const pio_program_i2c_t *pro,
                 gpio_t sda, gpio_t scl, unsigned irq);

/**
 * @brief   Get exclusive access to the emulated I2C bus
 *
 * @param[in]       bus         PIO I2C bus
 */
void pio_i2c_acquire(pio_i2c_bus_t *bus);

/**
 * @brief   Release emulated I2C bus
 *
 * @param[in]       bus         PIO I2C bus
 */
void pio_i2c_release(pio_i2c_bus_t *bus);

/**
 * @brief   Emulate @ref i2c_read_regs
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       addr        7-bit or 10-bit device address (right-aligned)
 * @param[in]       reg         Register address to read from (8- or 16-bit right-aligned)
 * @param[out]      data        Memory location to store received data
 * @param[in]       len         The number of bytes to read into @p data
 * @param[in]       flags       Optional flags (see @ref i2c_flags_t)
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_i2c_read_regs(pio_t pio, pio_sm_t sm, uint16_t addr,
                      uint16_t reg, void *data, size_t len, uint8_t flags);

/**
 * @brief   Emulate @ref i2c_read_reg
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       addr        7-bit or 10-bit device address (right-aligned)
 * @param[in]       reg         Register address to read from (8- or 16-bit right-aligned)
 * @param[out]      data        Memory location to store received data
 * @param[in]       flags       Optional flags (see @ref i2c_flags_t)
 *
 * @return  Success: 0
 *          Failure: != 0
 */
static inline int pio_i2c_read_reg(pio_t pio, pio_sm_t sm, uint16_t addr,
                                   uint16_t reg, void *data, uint8_t flags)
{
    return pio_i2c_read_regs(pio, sm, addr, reg, data, 1, flags);
}

/**
 * @brief   Emulate @ref i2c_read_bytes
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       addr        7-bit or 10-bit device address (right-aligned)
 * @param[out]      data        Memory location to store received data
 * @param[in]       len         The number of bytes to read into @p data
 * @param[in]       flags       Optional flags (see @ref i2c_flags_t)
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_i2c_read_bytes(pio_t pio, pio_sm_t sm, uint16_t addr,
                       void *data, size_t len, uint8_t flags);

/**
 * @brief   Emulate @ref i2c_read_byte
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       addr        7-bit or 10-bit device address (right-aligned)
 * @param[out]      data        Memory location to store received data
 * @param[in]       flags       Optional flags (see @ref i2c_flags_t)
 *
 * @return  Success: 0
 *          Failure: != 0
 */
static inline int pio_i2c_read_byte(pio_t pio, pio_sm_t sm, uint16_t addr,
                                    void *data, uint8_t flags)
{
    return pio_i2c_read_bytes(pio, sm, addr, data, 1, flags);
}

/**
 * @brief   Emulate @ref i2c_write_bytes
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       addr        7-bit or 10-bit device address (right-aligned)
 * @param[in]       data        Array holding the bytes to write to the device
 * @param[in]       len         The number of bytes to write
 * @param[in]       flags       Optional flags (see @ref i2c_flags_t)
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_i2c_write_bytes(pio_t pio, pio_sm_t sm, uint16_t addr,
                        const void *data, size_t len, uint8_t flags);

/**
 * @brief   Emulate @ref i2c_write_byte
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       addr        7-bit or 10-bit device address (right-aligned)
 * @param[in]       data        Byte to write to the device
 * @param[in]       flags       Optional flags (see @ref i2c_flags_t)
 *
 * @return  Success: 0
 *          Failure: != 0
 */
static inline int pio_i2c_write_byte(pio_t pio, pio_sm_t sm, uint16_t addr,
                                     uint8_t data, uint8_t flags)
{
    return pio_i2c_write_bytes(pio, sm, addr, &data, 1, flags);
}

/**
 * @brief   Emulate @ref i2c_write_regs
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       addr        7-bit or 10-bit device address (right-aligned)
 * @param[in]       reg         register address to read from (8- or 16-bit, right-aligned)
 * @param[in]       data        Array holding the bytes to write to the device
 * @param[in]       len         The number of bytes to write
 * @param[in]       flags       Optional flags (see @ref i2c_flags_t)
 *
 * @return  Success: 0
 *          Failure: != 0
 */
int pio_i2c_write_regs(pio_t pio, pio_sm_t sm, uint16_t addr,
                       uint16_t reg, const void *data, size_t len, uint8_t flags);

/**
 * @brief   Emulate @ref i2c_write_reg
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       addr        7-bit or 10-bit device address (right-aligned)
 * @param[in]       reg         register address to read from (8- or 16-bit, right-aligned)
 * @param[in]       data        Array holding the bytes to write to the device
 * @param[in]       flags       Optional flags (see @ref i2c_flags_t)
 *
 * @return  Success: 0
 *          Failure: != 0
 */
static inline int pio_i2c_write_reg(pio_t pio, pio_sm_t sm, uint16_t addr,
                                    uint16_t reg, uint8_t data, uint8_t flags)
{
    return pio_i2c_write_regs(pio, sm, addr, reg, &data, 1, flags);
}

#ifdef __cplusplus
}
#endif
/** @} */
