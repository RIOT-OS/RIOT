/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_netreg
 * @{
 *
 * @file
 * @brief       Implementation of the global network module registry
 *
 * The register is implemented as a table of lists. Each table row consists of
 * a list header of entries for a given module. As module IDs are assigned
 * continuously and are used as array indices, the lookup is done with a
 * complexity of O(1).
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "netreg.h"



/**
 * @brief   Compute the needed size for the registry
 *
 * The size is computed to fit one pointer and 2 netreg_entry_t for each network
 * module.
 */
#ifndef NETREG_REGISTRY_SIZE
#define NETREG_REGISTRY_SIZE        (5 * sizeof(void*) * NETMOD_NUMOF)
#endif

/**
 * @brief   Allocate memory for the registry
 */
static char *buffer[NETREG_REGISTRY_SIZE];

/**
 * @brief   Head for the list of free entries
 */
static netreg_entry_t *next_free;

/**
 * @brief   Array holding the list heads
 */
static netreg_entry_t *modules = (netreg_entry_t *)buffer;


void netreg_init(void)
{
    netreg_entry_t *last = (netreg_entry_t *)&buffer[NETMOD_NUMOF * sizeof(netreg_entry_t *)];

    /* set list head */
    next_free = last;

    /* null out initial list heads */
    for (int i = 0; i < NETMOD_NUMOF; i++) {
        modules[i] = NULL;
    }

    /* add the rest of the buffer to the list of free entries */
    for (int i = ((NETMOD_NUMOF + 1) * sizeof(netreg_entry_t *);
         i < (sizeof(buffer) - sizeof(netreg_entry_t);
         i += sizeof(netreg_entry_t)) {
         = (netreg_entry_t *)buffer[i];
        last.next = (netreg_entry_t *)&buffer[i];
        last = last->next;
    }
    last.next = NULL;
}

int netreg_register(netmod_t mdoule, kernel_pid_t pid)
{
    netreg_entry_t *newone, *tmp;

    /* check if space is available and reserve space*/
    if (next_free == NULL) {
        return -1;
    }
    else {
        newone = next_free;
        newone->next = NULL;
        next_free = next_free->next;
    }

    /* add to correct list */
    do {
        tmp = modules[module];
    } while (tmp->next != NULL);
    tmp->next = newone;

    return 0;
}

int netreg_unregister(netmod_t module, kernel_pid_t pid)
{
    netreg_entry_t *tmp = modules[module];

    /* see if the first entry is a hit */
    if (tmp != NULL && tmp->module_pid == pid) {
        modules[module] == tmp->next;
        return 0;
    }

    while (tmp->next != NULL) {
        if (tmp->next->module_pid == pid) {
            tmp->next = tmp->next->next;
            return 0;
        }
        tmp = tmp->next;
    }

    /* if we end up here, the PID was not found in the list */
    return -1;
}

kernel_pid_t netreg_lookup(netreg_entry_t *entry, netmod_t module)
{
    /* are there entries for the given module? */
    entry = modules[module];
    if (entry != NULL) {
        return entry->module_pid;
    }
    else {
        return KERNEL_PID_UNDEF;
    }
}

kernel_pid_t netreg_getnext(netreg_entry_t *entry)
{
    entry = entry->next;
    if (entry != NULL) {
        return entry->module_pid;
    }
    else {
        return KERNEL_PID_UNDEF;
    }
}
