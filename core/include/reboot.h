/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file
 * @brief       Reboot function
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef REBOOT_H_
#define REBOOT_H_

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Immediately reboots the system.
 *
 * This function is used by core_panic() when the DEVELHELP macro is not defined.
 */
void reboot(void);

#ifdef __cplusplus
}
#endif

#endif /* REBOOT_H_ */
/** @} */
