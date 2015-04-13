/*
 * Copyright (C) 2014 Freie Universität Berlin
 * 
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cebit_sensor
 * @{
 *
 * @file        sense.h
 * @brief       CeBIT 2014 demo application - sensor node
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef __SENSE_H
#define __SENSE_H

/**
 * @brief       Start a sensor thread and start detecting events.
 */
void sense_init(void);

#endif /* __SENSE_H */