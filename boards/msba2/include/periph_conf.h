/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_msba2
 * @{
 *
 * @file
 * @brief       MSB-A2 peripheral configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#include "lpc2387.h"

/**
 * @brief PWM device and pinout configuration
 */
#define PWM_NUMOF           (1)
#define PWM_0_EN            (1)

/* PWM_0 device configuration */
#define PWM_0_CH0           (3)
#define PWM_0_CH0_MR        PWM1MR3
#define PWM_0_CH1           (4)
#define PWM_0_CH1_MR        PWM1MR4
#define PWM_0_CH2           (5)
#define PWM_0_CH2_MR        PWM1MR5
/* PWM_0 pin configuration */
#define PWM_0_PORT          PINSEL4
#define PWM_0_CH0_PIN       (2)
#define PWM_0_CH1_PIN       (3)
#define PWM_0_CH2_PIN       (4)
#define PWM_0_FUNC          (1)


#endif /* __PERIPH_CONF_H */
/** @} */
