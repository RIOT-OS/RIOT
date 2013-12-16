
/**
 * @defgroup    sys_autoinit Auto-init
 * @ingroup     sys
 * @brief       Auto initialize modules
 * @note        This feature can be used by any project by adding auto_init to
 *              USEMODULE in the project's Makefile. auto_init will initialize
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

#ifdef MODULE_HWTIMER
#include "hwtimer.h"
#endif

#ifdef MODULE_SHT11
#include "sht11.h"
#endif

#ifdef MODULE_GPIOINT
#include "gpioint.h"
#endif

#ifdef MODULE_CC110X
#include "cc1100-interface.h"
#endif

#ifdef MODULE_LTC4150
#include "ltc4150.h"
#endif

void auto_init(void);

/** @} */
#endif /* AUTO_INIT_H */
