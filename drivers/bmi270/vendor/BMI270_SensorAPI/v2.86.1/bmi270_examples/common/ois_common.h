/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef _OIS_COMMON_H
#define _OIS_COMMON_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include "coines.h"
#include "bmi2_ois.h"

/******************************************************************************/
/* Structure declarations */
/******************************************************************************/

/*!
 * @brief  Structure to store the interface related configurations
 */
struct coines_ois_intf_config
{
    uint8_t dev_addr; /* Device address or Chip select of the interface selected */
    uint8_t bus; /* Bus instance of the interface selected */
};

/******************************************************************************/
/*!                       Function Definitions                                */

/*!
 *  @brief Function for reading the sensor's registers through SPI bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] length       : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BMI2_INTF_RET_SUCCESS -> Success
 *  @retval != BMI2_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BMI2_INTF_RETURN_TYPE bmi2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through SPI bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data     : Pointer to the data buffer whose data has to be written.
 *  @param[in] length       : No of bytes to write.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BMI2_INTF_RET_SUCCESS -> Success
 *  @retval != BMI2_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BMI2_INTF_RETURN_TYPE bmi2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 * @brief This function provides the delay for required time (Microsecond) as per the input provided in some of the
 * APIs.
 *
 *  @param[in] period       : The required wait time in microsecond.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return void.
 *
 */

void bmi2_delay_us(uint32_t period, void *intf_ptr);

/*!
 *  @brief Function to initialize the OIS (SPI) interface.
 *
 *  @param[in] ois_dev  : Structure instance of bmi2_ois_dev
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t bmi2_ois_interface_init(struct bmi2_ois_dev *ois_dev);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _OIS_COMMON_H */
