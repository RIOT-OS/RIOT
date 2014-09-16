/*
 * Copyright (C) 2010 Freie Universität Berlin
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
 */

#ifndef AUTO_INIT_H
#define AUTO_INIT_H

void auto_init(void);

/** @} */
#endif /* AUTO_INIT_H */
