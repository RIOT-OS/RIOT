/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/**
 * @file  vl53l1x_calibration.h
 * @brief Calibration Functions definition
 */

#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

/**
 * @brief This function performs the offset calibration.\n
 * The function returns the offset value found and programs the offset compensation into the device.
 * @param TargetDistInMm target distance in mm, ST recommended 100 mm
 * Target reflectance = grey17%
 * @return 0:success, !=0: failed
 * @return offset pointer contains the offset found in mm
 */
int8_t VL53L1X_CalibrateOffset(uint16_t dev, uint16_t TargetDistInMm, int16_t *offset);

/**
 * @brief This function performs the xtalk calibration.\n
 * The function returns the xtalk value found and programs the xtalk compensation to the device
 * @param TargetDistInMm target distance in mm\n
 * The target distance : the distance where the sensor start to "under range"\n
 * due to the influence of the photons reflected back from the cover glass becoming strong\n
 * It's also called inflection point\n
 * Target reflectance = grey 17%
 * @return 0: success, !=0: failed
 * @return xtalk pointer contains the xtalk value found in cps (number of photons in count per second)
 */
int8_t VL53L1X_CalibrateXtalk(uint16_t dev, uint16_t TargetDistInMm, uint16_t *xtalk);

#endif
