/* Mutex handling.  */

#include <time.h>

#include "kernel.h"
#include "mutex.h"

/* Initialize a mutex.  */
int pthread_mutex_init(pthread_mutex_t *mutex,
        const pthread_mutexattr_t *mutexattr);

/* Destroy a mutex.  */
int pthread_mutex_destroy(pthread_mutex_t *mutex);

/* Try locking a mutex.  */
int pthread_mutex_trylock(pthread_mutex_t *mutex);

/* Lock a mutex.  */
int pthread_mutex_lock(pthread_mutex_t *mutex);

/* Wait until lock becomes available, or specified time passes. */
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
        const struct timespec *abstime);

/* Unlock a mutex.  */
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/* Get the priority ceiling of MUTEX.  */
int pthread_mutex_getprioceiling(const pthread_mutex_t *mutex,
        int *prioceiling);

/* Set the priority ceiling of MUTEX to PRIOCEILING, return old
 priority ceiling value in *OLD_CEILING.  */
int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling,
        int *old_ceiling);

/* Functions for handling mutex attributes.  */

/* Initialize mutex attribute object ATTR with default attributes
 (kind is PTHREAD_MUTEX_TIMED_NP).  */
int pthread_mutexattr_init(pthread_mutexattr_t *attr);

/* Destroy mutex attribute object ATTR.  */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);

/* Get the process-shared flag of the mutex attribute ATTR.  */
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr,
        int *pshared);

/* Set the process-shared flag of the mutex attribute ATTR.  */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);

/* Return in *KIND the mutex kind attribute in *ATTR.  */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *kind);

/* Set the mutex kind attribute in *ATTR to KIND (either PTHREAD_MUTEX_NORMAL,
 PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_ERRORCHECK, or
 PTHREAD_MUTEX_DEFAULT).  */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind);

/* Return in *PROTOCOL the mutex protocol attribute in *ATTR.  */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr,
        int *protocol);

/* Set the mutex protocol attribute in *ATTR to PROTOCOL (either
 PTHREAD_PRIO_NONE, PTHREAD_PRIO_INHERIT, or PTHREAD_PRIO_PROTECT).  */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);

/* Return in *PRIOCEILING the mutex prioceiling attribute in *ATTR.  */
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr,
        int *prioceiling);

/* Set the mutex prioceiling attribute in *ATTR to PRIOCEILING.  */
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling);

/* Get the robustness flag of the mutex attribute ATTR.  */
int pthread_mutexattr_getrobust(const pthread_mutexattr_t *attr,
        int *robustness);

/* Set the robustness flag of the mutex attribute ATTR.  */
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robustness);
