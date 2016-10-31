/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 */

/**
 * @defgroup    boards_pic32-wifire Digilent PIC32 WiFire
 * @ingroup     boards
 * @brief       board configuration for the Digilent PIC32 WiFire
 * @{
 *
 * @file
 * @brief       board configuration for the Digilent PIC32 WiFire
 *
 * @author      Imagination Technologies.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "p32mz2048efg100.h"

/**
 * @brief The peripheral clock is required for the UART Baud rate calculation
 *        It is configured by the 'config' registers (see pic32_config_settings.c)
 */
#define PERIPHERAL_CLOCK (100000000)  /* Hz */

/**
 * @brief Set how many increments of the count register per uS
 *        needed by the timer code.
 */
#define TICKS_PER_US (100)

/**
 * @brief We are using a PIC32MZ device
 */
#define PIC32MZ (1)

#ifdef __cplusplus
}
#endif
/** @} */

#endif /* _BOARD_H_ */

