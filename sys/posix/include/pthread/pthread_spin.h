/* Functions to handle spinlocks.  */

/* Initialize the spinlock LOCK.  If PSHARED is nonzero the spinlock can
 be shared between different processes.  */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);

/* Destroy the spinlock LOCK.  */
int pthread_spin_destroy(pthread_spinlock_t *lock);

/* Wait until spinlock LOCK is retrieved.  */
int pthread_spin_lock(pthread_spinlock_t *lock);

/* Try to lock spinlock LOCK.  */
int pthread_spin_trylock(pthread_spinlock_t *lock);

/* Release spinlock LOCK.  */
int pthread_spin_unlock(pthread_spinlock_t *lock);
