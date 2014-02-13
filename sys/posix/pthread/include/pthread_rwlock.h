/* Functions for handling read-write locks.  */

/* Initialize read-write lock RWLOCK using attributes ATTR, or use
 the default values if later is NULL.  */
int pthread_rwlock_init(pthread_rwlock_t *rwlock,
        const pthread_rwlockattr_t *attr);

/* Destroy read-write lock RWLOCK.  */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

/* Acquire read lock for RWLOCK.  */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);

/* Try to acquire read lock for RWLOCK.  */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);

/* Try to acquire read lock for RWLOCK or return after specfied time.  */
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
        const struct timespec *abstime);

/* Acquire write lock for RWLOCK.  */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

/* Try to acquire write lock for RWLOCK.  */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);

/* Try to acquire write lock for RWLOCK or return after specfied time.  */
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
        const struct timespec *abstime);

/* Unlock RWLOCK.  */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

/* Functions for handling read-write lock attributes.  */

/* Initialize attribute object ATTR with default values.  */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);

/* Destroy attribute object ATTR.  */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);

/* Return current setting of process-shared attribute of ATTR in PSHARED.  */
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr,
        int *pshared);

/* Set process-shared attribute of ATTR to PSHARED.  */
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

/* Return current setting of reader/writer preference.  */
int pthread_rwlockattr_getkind_np(const pthread_rwlockattr_t *attr, int *pref);

/* Set reader/write preference.  */
int pthread_rwlockattr_setkind_np(pthread_rwlockattr_t *attr, int pref);
