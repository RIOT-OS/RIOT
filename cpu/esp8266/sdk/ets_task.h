/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef ETS_TASK_H
#define ETS_TASK_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_ESP_SDK

#include "ets_sys.h"

extern uint8_t ets_task_min_prio;

/* Task control block definition as used for ETS task functions */

typedef struct {
    ETSTask   task;     /* +0   task function */
    ETSEvent* queue;    /* +4   event queue (ring buffer) */
    uint8_t   qlength;  /* +8   event queue length */
    uint8_t   qposw;    /* +9   event queue position for write */
    uint8_t   qposr;    /* +10  event queue position for read */
    uint8_t   qpending; /* +11  pending events */
    uint32_t  maskbit;  /* +12  task mask bit */
} ets_task_tcb_t;

/* ROM variables, defined in esp8266.riot-os.app.ld */
/* source: disassembly of boot rom at https://github.com/trebisky/esp8266 */

extern uint8_t         ets_task_min_prio;  /* 0x3fffc6fc */
extern void*           ets_idle_cb;        /* 0x3fffdab0 */
extern void*           ets_idle_arg;       /* 0x3fffdab4 */
extern uint32_t        ets_task_exec_mask; /* 0x3fffdab8 */
extern ets_task_tcb_t  ets_task_tab[32];   /* 0x3fffdac0 */

extern uint32_t ets_post (uint32_t prio, ETSSignal sig, ETSParam par);

void ets_tasks_run (void);
void ets_tasks_init (void);

#else /* MODULE_ESP_SDK */

#define ets_tasks_run()
#define ets_tasks_init()

#endif /* MODULE_ESP_SDK */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ETS_TASK_H */
