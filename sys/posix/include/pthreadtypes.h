#ifndef PTHREADTYPES_H_
#define PTHREADTYPES_H_

typedef unsigned long int pthread_t;

typedef struct pthread_attr
{
    uint8_t detached;
    char *ss_sp;
    size_t ss_size;
} pthread_attr_t;

struct sched_param {
    int todo; /* TODO */
};

/* Once-only execution */
typedef int pthread_once_t;
/* Single execution handling.  */
#define PTHREAD_ONCE_INIT 0

typedef unsigned long int pthread_barrier_t;
typedef unsigned long int pthread_barrierattr_t;

typedef unsigned long int pthread_cond_t;
typedef unsigned long int pthread_condattr_t;

typedef mutex_t pthread_mutex_t;
typedef unsigned long int pthread_mutexattr_t;

typedef unsigned long int pthread_rwlock_t;
typedef unsigned long int pthread_rwlockattr_t;

typedef volatile int pthread_spinlock_t;

#endif /* PTHREADTYPES_H_ */
