/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Scheduling parameters and policies of pthreads.
 * @note    None of these functions are implemented, yet.
 * @note    Do not include this header file directly, but pthread.h.
 */

#ifndef __SYS__POSIX__PTHREAD_SCHEDULING__H
#define __SYS__POSIX__PTHREAD_SCHEDULING__H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief           Unimplemented.
 * @note            Due to the native of RIOT it is unlikely that this function will ever be implemented.
 * @param[in]       target_thread   Unused
 * @param[in]       policy          Unused
 * @param[in]       param           Unused
 * @returns         The function is unimplemented. Using it will cause a link time error.
 */
int pthread_setschedparam(pthread_t target_thread, int policy, const struct sched_param *param);

/**
 * @brief           Unimplemented.
 * @note            Due to the native of RIOT it is unlikely that this function will ever be implemented.
 * @param[in]       target_thread   Unused
 * @param[in,out]   policy          Unused
 * @param[out]      param           Unused
 * @returns         The function is unimplemented. Using it will cause a link time error.
 */
int pthread_getschedparam(pthread_t target_thread, int *policy, struct sched_param *param);

/**
 * @brief           Unimplemented.
 * @note            Due to the native of RIOT it is unlikely that this function will ever be implemented.
 * @param[in]       target_thread   Unused
 * @param[in]       prio            Unused
 * @returns         The function is unimplemented. Using it will cause a link time error.
 */
int pthread_setschedprio(pthread_t target_thread, int prio);

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
