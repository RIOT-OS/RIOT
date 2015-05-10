/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file
 * @brief       Architecture dependent panic function
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef PANIC_ARCH_H
#define PANIC_ARCH_H

#ifdef __cplusplus
 extern "C" {
#endif

 /**
  * @brief architecture dependent handling of an panic case
  *
  * This function gives the CPU the possibility to execute architecture
  * dependent code in case of an severe error.
  */
void panic_arch(void);

#ifdef __cplusplus
}
#endif

#endif /* REBOOT_ARCH_H */
/** @} */
