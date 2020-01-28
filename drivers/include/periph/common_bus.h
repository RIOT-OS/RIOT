/*
 * Copyright (C) 2020 Pieter du Preez <pdupreez@gmail.com>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_common_bus (I2C/SPI)
 * @ingroup     drivers_periph
 * @brief       Low-level I2C/SPI bus driver interface
 *
 * This is a kind of router for drivers that are able to support
 * either I2C or API bus interfaces.
 *
 * It is required to call the common_bus_setup function as start-up.
 * This function sets up function pointers for directing bus
 * communication to the desired bus.
 *
 * @{
 *
 * @file
 * @brief       Low-level I2C/SPI bus driver interface definition
 *
 * @author      Pieter du Preez <pdupreez@gmail.com>
 */

#ifndef PERIPH_COMMON_BUS_H
#define PERIPH_COMMON_BUS_H

#ifdef MODULE_PERIPH_SPI
#include "spi.h"
#endif

#ifdef MODULE_PERIPH_I2C
#include "i2c.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Supported bus types.
 */
typedef enum {
    COMMON_BUS_UNDEF = 0,
    COMMON_BUS_I2C,
    COMMON_BUS_SPI,
} common_bus_type_t;

#ifdef MODULE_PERIPH_SPI
typedef struct  {
    spi_t dev;
    gpio_t cs;
    spi_mode_t mode;
    spi_clk_t clk;
} spi_bus_t;
#endif

#ifdef MODULE_PERIPH_I2C
typedef struct {
    i2c_t dev;
    uint8_t addr;
} i2c_bus_t;
#endif

/**
 * @brief   A union of bus parameter types
 */

typedef union
{
#ifdef MODULE_PERIPH_SPI
    spi_bus_t spi;
#endif
#ifdef MODULE_PERIPH_I2C
    i2c_bus_t i2c;
#endif
#if !defined(MODULE_PERIPH_I2C) && !defined(MODULE_PERIPH_SPI)
    /**
     * @brief   A dummy variable that forces this union
     *          not to be empty if no I2C or SPI module was
     *          selected.
     */
    int dummy_to_avoid_union_has_no_members_warning;
#endif
} common_bus_params_t;

/**
 * @brief   Bus initialization function prototype
 *
 * @param[in] bus       bus parameters
 */
typedef int common_bus_init_t(const common_bus_params_t *bus);

/**
 * @brief   Bus acquire function prototype
 *
 * @param[in] bus       bus parameters
 */
typedef int common_bus_acquire_t(const common_bus_params_t *bus);

/**
 * @brief   Bus release function prototype
 *
 * @param[in] bus       bus parameters
 */
typedef void common_bus_release_t(const common_bus_params_t *bus);

/**
 * @brief   Bus read register function prototype
 *
 * @param[in]  bus      bus parameters
 * @param[in]  reg      register
 * @param[out] out      byte, read from register
 */
typedef int common_bus_read_reg_t(const common_bus_params_t *bus,
                                 uint16_t reg, uint8_t *out);
/**
 * @brief   Bus read registers function prototype
 *
 * @param[in]  bus      bus parameters
 * @param[in]  reg      register
 * @param[out] data     data, read from register
 * @param[in]  len      length of returned register contents
 */
typedef int common_bus_read_regs_t(const common_bus_params_t *bus,
                                  uint16_t reg, void *data, size_t len);

/**
 * @brief   Bus write register function prototype
 *
 * @param[in] bus       bus parameters
 * @param[in] reg       register
 * @param[in] data      data byte for register
 */
typedef int common_bus_write_reg_t(const common_bus_params_t *bus,
                                  uint8_t reg, uint8_t data);

/**
 * @brief   Function pointer structure for pivoting to a specified bus.
 */
typedef struct {
    /**
     * @brief The bus init function pointer
     */
    common_bus_init_t *common_bus_init;
    /**
     * @brief The bus acquire function pointer
     */
    common_bus_acquire_t *common_bus_acquire;
    /**
     * @brief The bus release function pointer
     */
    common_bus_release_t *common_bus_release;
    /**
     * @brief The bus read register function pointer
     */
    common_bus_read_reg_t *common_bus_read_reg;
    /**
     * @brief The bus read registers function pointer
     */
    common_bus_read_regs_t *common_bus_read_regs;
    /**
     * @brief The bus write register function pointer
     */
    common_bus_write_reg_t *common_bus_write_reg;
} common_bus_function_t;

/**
 * @brief   The transport struct contains the bus type, the bus
 *          and the common bus function pointers.
 */
typedef struct
{
    /**
     * @brief The bus type
     */
    common_bus_type_t type;
    /**
     * @brief The bus parameters
     */
    common_bus_params_t bus;
    /**
     * @brief The bus function pointers
     */
    common_bus_function_t f;
} common_transport_t;

/**
 * @brief   Function that must be called at start-up.
 */

void common_bus_setup(common_transport_t* transport);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_COMMON_BUS_H */
/** @} */
