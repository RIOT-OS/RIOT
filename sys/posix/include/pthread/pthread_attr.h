/* Thread attribute handling.  */

#define PTHREAD_CREATE_DETACHED (1)
#define PTHREAD_CREATE_JOINABLE (0)

/* Initialize thread attribute *ATTR with default attributes
 (detachstate is PTHREAD_JOINABLE, scheduling policy is SCHED_OTHER,
 no user-provided stack).  */
int pthread_attr_init(pthread_attr_t *attr);

/* Destroy thread attribute *ATTR.  */
int pthread_attr_destroy(pthread_attr_t *attr);

/* Get detach state attribute.  */
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);

/* Set detach state attribute.  */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);

/* Get the size of the guard area created for stack overflow protection.  */
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);

/* Set the size of the guard area created for stack overflow protection.  */
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);

/* Return in *PARAM the scheduling parameters of *ATTR.  */
int pthread_attr_getschedparam(const pthread_attr_t * attr,
        struct sched_param * param);

/* Set scheduling parameters (priority, etc) in *ATTR according to PARAM.  */
int pthread_attr_setschedparam(pthread_attr_t * attr,
        const struct sched_param * param);

/* Return in *POLICY the scheduling policy of *ATTR.  */
int pthread_attr_getschedpolicy(const pthread_attr_t * attr, int * policy);

/* Set scheduling policy in *ATTR according to POLICY.  */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);

/* Return in *INHERIT the scheduling inheritance mode of *ATTR.  */
int pthread_attr_getinheritsched(const pthread_attr_t * attr, int * inherit);

/* Set scheduling inheritance mode in *ATTR according to INHERIT.  */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit);

/* Return in *SCOPE the scheduling contention scope of *ATTR.  */
int pthread_attr_getscope(const pthread_attr_t * attr, int * scope);

/* Set scheduling contention scope in *ATTR according to SCOPE.  */
int pthread_attr_setscope(pthread_attr_t *attr, int scope);

/* Return the previously set address for the stack.  */
int pthread_attr_getstackaddr(const pthread_attr_t * attr, void ** stackaddr);

/* Set the starting address of the stack of the thread to be created.
 Depending on whether the stack grows up or down the value must either
 be higher or lower than all the address in the memory block.  The
 minimal size of the block must be PTHREAD_STACK_MIN.  */
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);

/* Return the currently used minimal stack size.  */
int pthread_attr_getstacksize(const pthread_attr_t * attr, size_t * stacksize);

/* Add information about the minimum stack size needed for the thread
 to be started.  This size must never be less than PTHREAD_STACK_MIN
 and must also not exceed the system limits.  */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
