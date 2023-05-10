/*
 * Copyright (C) 2019 Freie Universität Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       testing xtimer_mutex_lock_timeout function
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include "shell.h"
#include "xtimer.h"
#include "thread.h"
#include "msg.h"
#include "irq.h"

/* XTIMER_SHIFT can be undefined when using xtimer_on_ztimer on boards
 * incompatible with xtimers tick conversion, e.g. the waspmote-pro
 */
#ifndef XTIMER_SHIFT
#define XTIMER_SHIFT    (0)
#endif

/* timeout at one millisecond (1000 us) to make sure it does not spin. */
#define LONG_MUTEX_TIMEOUT 1000

/* timeout smaller than XTIMER_BACKOFF to make sure it spins. */
#define SHORT_MUTEX_TIMEOUT ((1 << XTIMER_SHIFT) + 1)

/* main Thread PID */
static kernel_pid_t main_thread_pid;

/**
 * Forward declarations
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_unlocked(int argc,
                                                            char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_long_locked(int argc,
                                                          char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_low_prio_thread(int argc,
                                                              char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_short_unlocked(int argc,
                                                             char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_short_locked(int argc,
                                                           char **argv);

/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "mutex_timeout_long_unlocked", "unlocked mutex (no-spin timeout)",
      cmd_test_xtimer_mutex_lock_timeout_long_unlocked, },
    { "mutex_timeout_long_locked", "locked mutex (no-spin timeout)",
      cmd_test_xtimer_mutex_lock_timeout_long_locked, },
    { "mutex_timeout_long_locked_low",
      "lock low-prio-locked-mutex from high-prio-thread (no-spin timeout)",
      cmd_test_xtimer_mutex_lock_timeout_low_prio_thread, },
    { "mutex_timeout_short_unlocked", "unlocked mutex (spin timeout)",
      cmd_test_xtimer_mutex_lock_timeout_short_unlocked, },
    { "mutex_timeout_short_locked", "locked mutex (spin timeout)",
      cmd_test_xtimer_mutex_lock_timeout_short_locked, },
    { NULL, NULL, NULL }
};

/**
 * @brief   stack for
 *          cmd_test_xtimer_mutex_lock_timeout_low_prio_thread
 *          not enough stack for doing printf only use puts
 */
static char t_stack[THREAD_STACKSIZE_DEFAULT];

/**
 * @brief   send message and suicide thread
 *
 * This function will send a message to a thread without yielding
 * and terminates the calling thread. This can be used to wakeup a
 * thread and terminating yourself.
 * This function calls sched_task_exit()
 *
 * @param[in] m Pointer to preallocated @ref msg_t structure, must
 * not be NULL.
 * @param[in] target_pid PID of target thread
 *
 */
static NORETURN void msg_send_sched_task_exit(msg_t *m, kernel_pid_t target_pid)
{
    (void)irq_disable();
    msg_send_int(m, target_pid);
    sched_task_exit();
}

/**
 * @brief   thread function for
 *          cmd_test_xtimer_mutex_lock_timeout_low_prio_thread
 */
void *thread_low_prio_test(void *arg)
{
    mutex_t *test_mutex = (mutex_t *)arg;
    msg_t msg;

    puts("THREAD low prio: start");

    mutex_lock(test_mutex);
    thread_wakeup(main_thread_pid);

    mutex_unlock(test_mutex);

    puts("THREAD low prio: exiting low");
    msg_send_sched_task_exit(&msg, main_thread_pid);
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 *
 * the mutex is not locked before the function call and
 * the timer long. Meaning the timer will get removed
 * before triggering.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_unlocked(int argc,
                                                            char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout");
    mutex_t test_mutex = MUTEX_INIT;

    if (xtimer_mutex_lock_timeout(&test_mutex, LONG_MUTEX_TIMEOUT) == 0) {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    else {
        puts("error: mutex timed out");
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 *
 * the mutex is locked before the function call and
 * the timer long. Meaning the timer will trigger
 * and remove the thread from the mutex waiting list.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_locked(int argc,
                                                          char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout");
    mutex_t test_mutex = MUTEX_INIT;
    mutex_lock(&test_mutex);

    if (xtimer_mutex_lock_timeout(&test_mutex, LONG_MUTEX_TIMEOUT) == 0) {
        puts("Error: mutex taken");
    }
    else {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 *
 * This function will create a new thread with lower prio
 * than the main thread (this function should be called from
 * the main thread). The new thread will get a mutex and will
 * lock it. This function (main thread) calls xtimer_mutex_lock_timeout.
 * The other thread will then unlock the mutex. The main
 * thread gets the mutex and wakes up. The timer will not
 * trigger because the main threads gets the mutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 always
 */
static int cmd_test_xtimer_mutex_lock_timeout_low_prio_thread(int argc,
                                                              char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout with thread");
    mutex_t test_mutex = MUTEX_INIT;
    main_thread_pid = thread_getpid();
    int current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);
    kernel_pid_t test_thread = thread_create(t_stack, sizeof(t_stack),
                                             THREAD_PRIORITY_MAIN + 1,
                                             THREAD_CREATE_STACKTEST,
                                             thread_low_prio_test,
                                             (void *)&test_mutex,
                                             "thread_low_prio_test");
    (void)test_thread;

    thread_sleep();

    puts("MAIN THREAD: calling xtimer_mutex_lock_timeout");

    if (xtimer_mutex_lock_timeout(&test_mutex, LONG_MUTEX_TIMEOUT) == 0) {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    else {
        puts("error: mutex timed out");
    }

    current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);

    /* to end the created thread */
    msg_t msg;
    puts("MAIN THREAD: waiting for created thread to end");
    msg_receive(&msg);

    current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);

    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout when spinning
 *
 * The mutex is locked before the function call and
 * the timer short. Meaning the timer will trigger before
 * xtimer_mutex_lock_timeout tries to acquire the mutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_short_locked(int argc,
                                                           char **argv)
{
    (void)argc;
    (void)argv;
    puts(
        "starting test: xtimer mutex lock timeout with short timeout and locked mutex");
    mutex_t test_mutex = MUTEX_INIT;
    mutex_lock(&test_mutex);

    if (xtimer_mutex_lock_timeout(&test_mutex, SHORT_MUTEX_TIMEOUT) == 0) {
        puts("Error: mutex taken");
    }
    else {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout when spinning
 *
 * the mutex is not locked before the function is called and
 * the timer is short. Meaning the timer will trigger before
 * xtimer_mutex_lock_timeout tries to acquire the mutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_short_unlocked(int argc,
                                                             char **argv)
{
    (void)argc;
    (void)argv;
    puts(
        "starting test: xtimer mutex lock timeout with short timeout and unlocked mutex");
    mutex_t test_mutex = MUTEX_INIT;

    if (xtimer_mutex_lock_timeout(&test_mutex, SHORT_MUTEX_TIMEOUT) == 0) {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    else {
        puts("Error: mutex timed out");
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   main function starting shell
 *
 * @return 0 on success
 */
int main(void)
{
    puts("Starting shell...");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
