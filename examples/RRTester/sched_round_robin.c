/*
 * Copyright (C) 2021 TUBA Freiberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Round Robin Scheduler implementation
 *@brief        Updated to a Feedback-scheduling implementation
 * @author      Karl Fessel <karl.fessel@ovgu.de>
 *
 * @}
 */

#include "thread.h"
#include "ztimer.h"
#include <stdio.h>

/* Costanti per la politica di scheduling a feedback */
#define NUM_FEEDBACK_QUEUES 3
#define FEEDBACK_QUANTUM_S 500 /* che corrisponde a 0.5 secondi*/

// Dichiarazioni di variabili
static int feedback_quantum_count = 0;
static int feedback_queue_levels[NUM_FEEDBACK_QUEUES] = {0};

/* Callback per la politica di scheduling a feedback */
static void _sched_feedback_cb(void *d);
/* Timer per la politica di scheduling a feedback */
static ztimer_t _feedback_timer = { .callback = _sched_feedback_cb };

/* Variabile per tracciare la coda corrente */
static uint8_t _current_feedback_queue = 0;

void sched_runq_callback(uint8_t prio);
/* Funzione di callback per il timer di scheduling a feedback */
void _sched_feedback_cb(void *d)
{
    (void)d;

    /* Logica per la politica di scheduling a feedback */
    thread_t *active_thread = thread_get_active();

    if (active_thread && active_thread->status == STATUS_RUNNING)
    {
        if (++feedback_quantum_count >= FEEDBACK_QUANTUM_S / NUM_FEEDBACK_QUEUES)
        {
            feedback_quantum_count = 0;
            feedback_queue_levels[active_thread->priority]++;
            active_thread->priority = feedback_queue_levels[active_thread->priority] % NUM_FEEDBACK_QUEUES;

            // Stampa delle informazioni sulla console
            printf("Thread %d switched to queue %d\n", (int)thread_getpid_of(active_thread), active_thread->priority);
        }
    }

    // Chiamata alla funzione sched_runq_callback per eventuali ulteriori azioni
    if (active_thread)
    {
        uint8_t active_priority = active_thread->priority;
        sched_runq_callback(active_priority);
    }
}

/* Implementazione della Callback. Gestisce lo scambio di contesto e stamperà le informazioni sulla console.
E' basata su una rotazione semplice delle code di feedback. */
static inline void _sched_feedback_remove(void)
{
    _current_feedback_queue = 0;
    ztimer_remove(NULL, &_feedback_timer);
}


static inline void _sched_feedback_set(uint8_t queue)
{
    if (queue >= NUM_FEEDBACK_QUEUES) {
        return;
    }
    _current_feedback_queue = queue;
    ztimer_set(NULL, &_feedback_timer, FEEDBACK_QUANTUM_S);
}

/* Funzioni di Set e Remove per il Timer */
void sched_feedback_init(void)
{
    // Inizializza _current_feedback_queue e verifica se applicabile alla priorità attiva
    _current_feedback_queue = 0;
    thread_t *active_thread = thread_get_active();
    if (active_thread)
    {
        sched_runq_callback(active_thread->priority);
    }
}
