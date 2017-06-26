/*
 * Copyright (C) 2017 Jonas Radtke <jonas.radtke@haw-hamburg.de> <jonas@radtke.dk>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
 * @ingroup     drivers_MLX90109
 * @{
 *
 * @file
 * @brief       Default parameters for MLX90109 driver
 *
 * @author      Jonas Radtke <jonas.radtke@haw-hamburg.de> <jonas@radtke.dk>
 */

#ifndef MLX90109_PARAMS_H
#define MLX90109_PARAMS_H

#include "board.h"
#include "mlx90109.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the MLX90109 driver
 * @{
 */
#ifndef MLX90109_PARAM_CLOCK_PIN
#define MLX90109_PARAM_CLOCK_PIN       (GPIO_PIN(0, 1))
#endif
#ifndef MLX90109_PARAM_DATA_PIN
#define MLX90109_PARAM_DATA_PIN   	   (GPIO_PIN(0, 4))
#endif
#ifndef MLX90109_PARAM_MODE_PIN
#define MLX90109_PARAM_MODE_PIN		  (0)
#endif
#ifndef MLX90109_PARAM_DATASELECT_PIN
#define MLX90109_PARAM_DATASELECT_PIN (0)
#endif
#ifndef MLX90109_PARAM_MODU_PIN
#define MLX90109_PARAM_MODU_PIN   (0)
#endif
#ifndef MLX90109_PARAM_SPEED
#define MLX90109_PARAM_SPEED      (4000)	//2000 baud or 4000 baud
#endif
#ifndef MLX90109_PARAM_CODE
#define MLX90109_PARAM_CODE       (2)	//  1 = Manchester, 2 = Biphase
#endif
/** @} */

/**
 * @brief   MLX90109 configuration
 */
#ifndef MLX90109_PARAMS
#define MLX90109_PARAMS           { .clock			= MLX90109_PARAM_CLOCK_PIN ,\
									.data			= MLX90109_PARAM_DATA_PIN ,\
									.mode			= MLX90109_PARAM_MODE_PIN ,\
									.dataSelect		= MLX90109_PARAM_DATASELECT_PIN ,\
									.modu			= MLX90109_PARAM_MODU_PIN ,},
#endif

static const mlx90109_params_t mlx90109_params[] ={
	MLX90109_PARAMS
};

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MLX90109_PARAMS_H */
/** @} */
 
