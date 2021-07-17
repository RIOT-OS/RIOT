/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx RF Core queue management
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#include "cc26xx_cc13xx_rfc_queue.h"

uint8_t *cc26x2_cc13x2_rfc_queue_recv(rfc_data_queue_t *queue)
{
    assert(queue);

    rfc_data_entry_t *start_entry = (rfc_data_entry_t *)queue->curr_entry;
    rfc_data_entry_t *curr_entry = start_entry;

    do {
        if (curr_entry->status == RFC_DATA_ENTRY_FINISHED) {
            return (uint8_t *)curr_entry;
        }

        curr_entry = (rfc_data_entry_t *)(curr_entry->next_entry);
    } while (curr_entry != start_entry);

    return NULL;
}
