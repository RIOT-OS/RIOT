/**
 * Semaphore data struct and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    semaphore.h
 * @brief   data struct and prototypes for semaphores
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#ifndef _SIXLOWPAN_SEMAPHORE_H
#define _SIXLOWPAN_SEMAPHORE_H

#include <stdint.h>

#include "mutex.h"

typedef struct sem_t {
    int8_t value;
    int8_t locked;
    mutex_t mutex;
} sem_t;

void sem_init(sem_t *sem, int8_t value);
int sem_wait(sem_t *sem);
int sem_signal(sem_t *sem);

#endif /* _SIXLOWPAN_SEMAPHORE_H*/
