/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Reboot API
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef REBOOT_H
#define REBOOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "arch/reboot_arch.h"

/**
 * @brief   Reboot the system
 */
void reboot(void);

#ifdef __cplusplus
}
#endif

#endif /* REBOOT_H */
/** @} */
