/**
 * @ingroup drivers_mrf24j40
 * @{
 *
 * @file
 * @brief Internal interfaces for MRF24J40 drivers
 *
 * @author Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 */
 
#ifndef MRF24J40_INTERNAL_H_
#define MRF24J40_INTERNAL_H_

#include <stdint.h>
#include "mrf24j40.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief Read from a register with a  at address `addr` from device `dev`. Register with 8bit address 
 *
 * @param[in] dev device to read from
 * @param[in] addr address of the register to read
 *
 * @return the value of the specified register
 */
 
uint8_t mrf24j40_reg_read_short(const mrf24j40_t *dev, const uint8_t addr);

/**
 * @brief Write to a register at address `addr` from device `dev`. Register with 8bit address
 *
 * @param[in] dev device to write to
 * @param[in] addr address of the register to write
 * @param[in] value value to write to the given register
 */
 
void mrf24j40_reg_write_short(const mrf24j40_t *dev, const uint8_t addr, const uint8_t value);
/**
 * @brief Read from a register with a  at address `addr` from device `dev`. Register with 10bit address
 *
 * @param[in] dev device to read from
 * @param[in] addr address of the register to read
 *
 * @return the value of the specified register
 */
 
uint8_t mrf24j40_reg_read_long(const mrf24j40_t *dev, const uint16_t addr);

/**
 * @brief Write to a register at address `addr` from device `dev`. Register with 10bit address
 *
 * @param[in] dev device to write to
 * @param[in] addr address of the register to write
 * @param[in] value value to write to the given register
 */
 
void mrf24j40_reg_write_short_long(const mrf24j40_t *dev, const uint16_t addr, const uint8_t value);