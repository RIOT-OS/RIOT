/* Functions for handling conditional variables.  */

/* Initialize condition variable COND using attributes ATTR, or use
 the default values if later is NULL.  */
int pthread_cond_init(pthread_cond_t * cond,
        const pthread_condattr_t * cond_attr);

/* Destroy condition variable COND.  */
int pthread_cond_destroy(pthread_cond_t *cond);

/* Wake up one thread waiting for condition variable COND.  */
int pthread_cond_signal(pthread_cond_t *cond);

/* Wake up all threads waiting for condition variables COND.  */
int pthread_cond_broadcast(pthread_cond_t *cond);

/* Wait for condition variable COND to be signaled or broadcast.
 MUTEX is assumed to be locked before.

 This function is a cancellation point and therefore not marked with
 .  */
int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex);

/* Wait for condition variable COND to be signaled or broadcast until
 ABSTIME.  MUTEX is assumed to be locked before.  ABSTIME is an
 absolute time specification; zero is the beginning of the epoch
 (00:00:00 GMT, January 1, 1970).

 This function is a cancellation point and therefore not marked with
 .  */
int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex,
        const struct timespec * abstime);

/* Functions for handling condition variable attributes.  */

/* Initialize condition variable attribute ATTR.  */
int pthread_condattr_init(pthread_condattr_t *attr);

/* Destroy condition variable attribute ATTR.  */
int pthread_condattr_destroy(pthread_condattr_t *attr);

/* Get the process-shared flag of the condition variable attribute ATTR.  */
int pthread_condattr_getpshared(const pthread_condattr_t * attr, int * pshared);

/* Set the process-shared flag of the condition variable attribute ATTR.  */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);

/* Get the clock selected for the conditon variable attribute ATTR.  */
int pthread_condattr_getclock(const pthread_condattr_t * attr,
        clockid_t * clock_id);

/* Set the clock selected for the conditon variable attribute ATTR.  */
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);
