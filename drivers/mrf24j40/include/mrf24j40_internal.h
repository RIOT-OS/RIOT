/**
 * @ingroup drivers_mrf24j40
 * @{
 *
 * @file
 * @brief Internal interfaces for MRF24J40 drivers
 *
 * @author Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 * @author <neo@nenaco.de>
 */

#ifndef MRF24J40_INTERNAL_H_
#define MRF24J40_INTERNAL_H_

#include <stdint.h>

#include "mrf24j40.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief initialization as decribed in datasheet
 */
void mrf24j40_init(mrf24j40_t *dev);

/**
 * @brief Read from a register with a  at address `addr` from device `dev`. Register with 8bit address
 *
 * @param[in] dev device to read from
 * @param[in] addr address of the register to read
 *
 * @return the value of the specified register
 */

uint8_t mrf24j40_reg_read_short(mrf24j40_t *dev, const uint8_t addr);

/**
 * @brief Write to a register at address `addr` from device `dev`. Register with 8bit address
 *
 * @param[in] dev device to write to
 * @param[in] addr address of the register to write
 * @param[in] value value to write to the given register
 */

void mrf24j40_reg_write_short(mrf24j40_t *dev, const uint8_t addr, const uint8_t value);
/**
 * @brief Read from a register with a  at address `addr` from device `dev`. Register with 10bit address
 *
 * @param[in] dev device to read from
 * @param[in] addr address of the register to read
 *
 * @return the value of the specified register
 */

uint8_t mrf24j40_reg_read_long(mrf24j40_t *dev, const uint16_t addr);

/**
 * @brief Write to a register at address `addr` from device `dev`. Register with 10bit address
 *
 * @param[in] dev device to write to
 * @param[in] addr address of the register to write
 * @param[in] value value to write to the given register
 */

void mrf24j40_reg_write_long(mrf24j40_t *dev, const uint16_t addr, const uint8_t value);


/**
 * @brief   Convenience function for reading the status of the given device
 *
 * @param[in] dev       device to read the status from
 *
 * @return              internal status of the given device
 */
uint8_t mrf24j40_get_status(mrf24j40_t *dev);


/**
 * @brief   Read a chunk of data from the TX Normal FIFO area of the given device
 *
 * @param[in]  dev      device to read from
 * @param[in]  offset   starting address to read from [valid 0x00-0x1ff]
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read from FIFO
 */
void mrf24j40_tx_normal_fifo_read(mrf24j40_t *dev, const uint16_t offset, uint8_t *data, const size_t len);

/**
 * @brief   Write a chunk of data into the TX Normal FIFO area of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] offset    address in the TX Normal FIFO to write to [valid 0x00-0x1ff]
 * @param[in] data      data to copy into FIFO
 * @param[in] len       number of bytes to write to FIFO
 */
void mrf24j40_tx_normal_fifo_write(mrf24j40_t *dev, const uint16_t offset, const uint8_t *data, const size_t len);

/**
 * @brief   Read a chunk of data from the RX_FIFO area of the given device
 *
 * @param[in]  dev      device to read from
 * @param[in]  offset   starting address to read from [valid 0x00-0x1ff]
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read from FIFO
 */
void mrf24j40_rx_fifo_read(mrf24j40_t *dev, const uint16_t offset, uint8_t *data, const size_t len);

/**
 * @brief   Write a chunk of data into the RX_FIFO area of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] offset    address in the RX FIFO to write to [valid 0x00-0x1ff]
 * @param[in] data      data to copy into FIFO
 * @param[in] len       number of bytes to write to FIFO
 */
void mrf24j40_rx_fifo_write(mrf24j40_t *dev, const uint16_t offset, const uint8_t *data, const size_t len);


/**
 * @brief   Convenience function for reading the status of the given device
 *
 * @param[in] dev       device to read the status from
 *
 * @return              internal status of the given device
 */
uint8_t mrf24j40_get_status(mrf24j40_t *dev);


/**
 * @brief   Wake up from sleep mode
 *
 * @param[in] dev       device to eventually wake up
 */
void mrf24j40_assert_awake(mrf24j40_t *dev);


/**
 * @brief   Set device to sleep mode
 *
 * @param[in] dev       device to eventually wake up
 */
void mrf24j40_assert_sleep(mrf24j40_t *dev);


/**
 * @brief   Trigger a hardware reset
 *
 * @param[in] dev       device to reset
 */
void mrf24j40_hardware_reset(mrf24j40_t *dev);


/**
 * @brief   Test routine to test FIFO values
 *
 * @param[in] dev
 */
void mrf24j40_print_tx_norm_buf(mrf24j40_t *dev);


/**
 * @brief   Test routine to test FIFO values
 *
 * @param[in] dev
 */
void mrf24j40_print_rx_buf(mrf24j40_t *dev);


/**
 * @brief   Set PHY parameters based on channel and page number
 *
 * @param[in] dev       device to configure
 */
void mrf24j40_configure_phy(mrf24j40_t *dev);


#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_INTERNAL_H */
/** @} */
