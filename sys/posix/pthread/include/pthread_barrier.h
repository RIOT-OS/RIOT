/* Functions to handle barriers.  */

/* Initialize BARRIER with the attributes in ATTR.  The barrier is
 opened when COUNT waiters arrived.  */
int pthread_barrier_init(pthread_barrier_t *barrier,
        const pthread_barrierattr_t *attr, unsigned int count);

/* Destroy a previously dynamically initialized barrier BARRIER.  */
int pthread_barrier_destroy(pthread_barrier_t *barrier);

/* Wait on barrier BARRIER.  */
int pthread_barrier_wait(pthread_barrier_t *barrier);

/* Initialize barrier attribute ATTR.  */
int pthread_barrierattr_init(pthread_barrierattr_t *attr);

/* Destroy previously dynamically initialized barrier attribute ATTR.  */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);

/* Get the process-shared flag of the barrier attribute ATTR.  */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr,
        int *pshared);

/* Set the process-shared flag of the barrier attribute ATTR.  */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);
