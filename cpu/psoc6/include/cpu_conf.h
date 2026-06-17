#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"
#include <stdbool.h>

// fill this with the headers of other PSOCs MCUs
//#ifdef CPU_PSOC6
#include "vendor/cy8c6248bzi_s2d44.h"
#include "vendor/cy_device.h"
//#endif

// periph_conf.h (board-specific)

#define CPU_DEFAULT_IRQ_PRIO    (1U)

#if defined(CPU_CORE_CORTEX_M0PLUS)
    #define CPU_IRQ_NUMOF           (16U)
#else
    #define CPU_IRQ_NUMOF           (168U)
#endif

#ifndef __CORE_CMSIS_INCLUDED
#define __CORE_CMSIS_INCLUDED

#if defined(CPU_CORE_CORTEX_M0PLUS)
    #include "core_cm0plus.h"
#else
    #include "core_cm4.h"
#endif

#endif

#endif
