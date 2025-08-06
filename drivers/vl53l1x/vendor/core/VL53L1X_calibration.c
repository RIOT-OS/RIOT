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
 * @file  vl53l1x_calibration.c
 * @brief Calibration functions implementation
 */
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

#define ALGO__PART_TO_PART_RANGE_OFFSET_MM	0x001E
#define MM_CONFIG__INNER_OFFSET_MM			0x0020
#define MM_CONFIG__OUTER_OFFSET_MM 			0x0022

int8_t VL53L1X_CalibrateOffset(uint16_t dev, uint16_t TargetDistInMm, int16_t *offset)
{
	uint8_t i, tmp;
	int16_t AverageDistance = 0;
	uint16_t distance;
	VL53L1X_ERROR status = 0;

	status |= VL53L1_WrWord(dev, ALGO__PART_TO_PART_RANGE_OFFSET_MM, 0x0);
	status |= VL53L1_WrWord(dev, MM_CONFIG__INNER_OFFSET_MM, 0x0);
	status |= VL53L1_WrWord(dev, MM_CONFIG__OUTER_OFFSET_MM, 0x0);
	status |= VL53L1X_StartRanging(dev);	/* Enable VL53L1X sensor */
	for (i = 0; i < 50; i++) {
		tmp = 0;
		while (tmp == 0){
			status |= VL53L1X_CheckForDataReady(dev, &tmp);
		}
		status |= VL53L1X_GetDistance(dev, &distance);
		status |= VL53L1X_ClearInterrupt(dev);
		AverageDistance = AverageDistance + distance;
	}
	status |= VL53L1X_StopRanging(dev);
	AverageDistance = AverageDistance / 50;
	*offset = TargetDistInMm - AverageDistance;
	status |= VL53L1_WrWord(dev, ALGO__PART_TO_PART_RANGE_OFFSET_MM, *offset*4);
	return status;
}

int8_t VL53L1X_CalibrateXtalk(uint16_t dev, uint16_t TargetDistInMm, uint16_t *xtalk)
{
	uint8_t i, tmp;
	float AverageSignalRate = 0;
	float AverageDistance = 0;
	float AverageSpadNb = 0;
	uint16_t distance = 0, spadNum;
	uint16_t sr;
	VL53L1X_ERROR status = 0;
	uint32_t calXtalk;

	status |= VL53L1_WrWord(dev, 0x0016,0);
	status |= VL53L1X_StartRanging(dev);
	for (i = 0; i < 50; i++) {
		tmp = 0;
		while (tmp == 0){
			status |= VL53L1X_CheckForDataReady(dev, &tmp);
		}
		status |= VL53L1X_GetSignalRate(dev, &sr);
		status |= VL53L1X_GetDistance(dev, &distance);
		status |= VL53L1X_ClearInterrupt(dev);
		AverageDistance = AverageDistance + distance;
		status = VL53L1X_GetSpadNb(dev, &spadNum);
		AverageSpadNb = AverageSpadNb + spadNum;
		AverageSignalRate =
		    AverageSignalRate + sr;
	}
	status |= VL53L1X_StopRanging(dev);
	AverageDistance = AverageDistance / 50;
	AverageSpadNb = AverageSpadNb / 50;
	AverageSignalRate = AverageSignalRate / 50;
	/* Calculate Xtalk value */
	calXtalk = (uint16_t)(512*(AverageSignalRate*(1-(AverageDistance/TargetDistInMm)))/AverageSpadNb);
	if(calXtalk  > 0xffff)
		calXtalk = 0xffff;
	*xtalk = (uint16_t)((calXtalk*1000)>>9);
	status |= VL53L1_WrWord(dev, 0x0016, (uint16_t)calXtalk);
	return status;
}
