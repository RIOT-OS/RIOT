/*
 * Copyright (C) 2010,2015 Freie Universität Berlin
 * Copyright (C) 2010 Kaspar Schleiser
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_autoinit Auto-init
 * @ingroup     sys
 * @brief       Auto initialize modules
 * @note        This feature can be used by any application by adding auto_init to
 *              USEMODULE in the application's Makefile. auto_init will initialize
 *              any other included module that does not require a parameter in
 *              its init function, i.e. if the prototype looks like this: void
 *              MODULE_init(void). Most timer modules or simple drivers can be
 *              initialized by auto_init. The modules will be initialized in
 *              the context of the main thread right before the main function
 *              gets called. Be aware that most modules expect to be
 *              initialized only once, so do not call a module's init function
 *              when using auto_init unless you know what you're doing.
 *
 * @{
 *
 * @file        auto_init.h
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef AUTO_INIT_H
#define AUTO_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes all high level modules that do not require parameters for
 *        initialization or uses default values.
 *
 *        This function gets called - if not explicitely disabled - by @ref
 *        kernel_init right before jumping into @e main.
 */
void auto_init(void);

/**
 * @brief Initialize network interfaces and register them with ng_netif
 *
 *        This function must be implemented in the board or in the application,
 *        in a subfolder that must be named auto_init_ng_netif
 *        This function is called under two conditions:
 *          1. the ng_netif module is used (USEMODULE contains ng_netif)
 *          2. the board or the application contains a subfolder called
 *             auto_init_ng_netif
 *        If the board and the application both contain the mentioned subfolder,
 *        the contents from the applications subfolder have a higher priority
 *        and will be compiled and linked, while the board's subfolder is then
 *        ignored.
 */
void auto_init_ng_netif(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AUTO_INIT_H */
