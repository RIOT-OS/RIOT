/*
 * Copyright (C) 2014 Freie Universität Berlin
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       RRTester application
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#include "xtimer.h"

// Dimensione dello stack di default per i thread
#define THREAD_STACKSIZE_DEFAULT (8192)

// Struttura per i parametri del thread
typedef struct
{
    char name[2];     // Nome del thread
    int service_time; // Tempo di servizio in secondi
} thread_params_t;

// Funzione eseguita dai thread
void *thread_func(void *arg)
{
    thread_params_t *params = (thread_params_t *)arg;
    int remaining_time = params->service_time;

    // Loop fino a quando il tempo di servizio non è esaurito
    while (remaining_time > 0)
    {
        printf("Thread %s: %d seconds remaining.\n", params->name, remaining_time);
        remaining_time--;

        // Sleep per 1 secondo
        xtimer_msleep(1000);
    }

    printf("Thread %s completed.\n", params->name);

    // Termina il thread
    return NULL;
}

// Funzione di inizializzazione del thread
kernel_pid_t init_thread(thread_params_t *params, size_t index)
{
    char *stack = malloc(THREAD_STACKSIZE_DEFAULT);
    if (stack == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate stack for Thread %s.\n", params->name);
        return -1;
    }

    // Imposta la priorità del thread sulla base della posizione nell'array
    // Con questa modifica, ogni thread avrà una priorità diversa in base alla
    // sua posizione nell'array 'thread_params'.
    // Questo permette al modulo di scheduling a feedback di regolare dinamicamente
    // la priorità dei thread durante l'esecuzione.
    uint8_t priority = THREAD_PRIORITY_MAIN + index;

    // Crea e avvia il thread
    kernel_pid_t pid = thread_create(stack,
                                     THREAD_STACKSIZE_DEFAULT,
                                     priority,
                                     THREAD_CREATE_SLEEPING | THREAD_CREATE_STACKTEST,
                                     thread_func,
                                     params,
                                     params->name);

    // Verifica se la creazione del thread ha avuto successo
    if (pid < 0)
    {
        fprintf(stderr, "Error: Unable to create Thread %s.\n", params->name);
        free(stack);
        return -1;
    }

    // Avvio del thread
    thread_wakeup(pid);

    printf("Created Thread %s with priority %d.\n", params->name, priority);

    return pid;
}

int main(void)
{
    printf("RRTester Application\n");

    // Definizione dei parametri per ciascun thread
    thread_params_t thread_params[] = {
        {"A", 3},
        {"B", 6},
        {"C", 4},
        {"D", 5},
        {"E", 2}};

    // Creazione e avvio dei thread
    for (size_t i = 0; i < sizeof(thread_params) / sizeof(thread_params[0]); i++)
    {
        kernel_pid_t pid = init_thread(&thread_params[i], i);
        if (pid < 0)
        {
            // Gestisci l'errore, ad esempio interrompendo la creazione di altri thread
            break;
        }
    }

    // Loop infinito per il thread principale
    while (1)
    {
        printf("Main thread running.\n");
        thread_sleep();
    }

    return 0;
}
