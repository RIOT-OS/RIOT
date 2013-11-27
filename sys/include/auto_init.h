
/**
 * @defgroup    sys_autoinit Auto-init
 * @ingroup     sys
 * @brief       Autoconfigure libraries
 * @{
 *
 * @file        auto_init.h
 */

#ifndef AUTO_INIT_H
#define AUTO_INIT_H

#ifdef MODULE_HWTIMER
#include <hwtimer.h>
#endif

#ifdef MODULE_SHT11
#include <sht11.h>
#endif

#ifdef MODULE_GPIOINT
#include <gpioint.h>
#endif

#ifdef MODULE_CC110X
#include <cc1100-interface.h>
#endif

#ifdef MODULE_LTC4150
#include <ltc4150.h>
#endif

void auto_init(void);

/** @} */
#endif /* AUTO_INIT_H */
