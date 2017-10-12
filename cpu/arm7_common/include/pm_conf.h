/*
 * Copyright (C) 2017 TU Braunschweig, Robert Hartung
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief           CPU specific power management options
 *
 * @author          Robert Hartung <hartung@ibr.cs.tu-bs.de>
 */

#ifndef PM_CONF_COMMON_H
#define PM_CONF_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Power management configuration
 *
 * Select the required implementations from the common CPUs or fallbacks
 * @{
 */
#define NEEDS_ARM7_PM_REBOOT
#define NEEDS_PM_OFF
#define NEEDS_PM_SET_LOWEST
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PM_CONF_COMMON_H */
/** @} */
