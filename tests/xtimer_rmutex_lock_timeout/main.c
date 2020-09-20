/*
 * Copyright (C) 2020 Freie Universität Berlin,
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
 * @brief       testing xtimer_rmutex_lock_timeout function
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
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
#define LONG_RMUTEX_TIMEOUT 1000

/* timeout smaller than XTIMER_BACKOFF to make sure it spins. */
#define SHORT_RMUTEX_TIMEOUT ((1 << XTIMER_SHIFT) + 1)

/**
 * Forward declarations
 */
static int cmd_test_xtimer_rmutex_lock_timeout_long_unlocked(int argc,
                                                             char **argv);
static int cmd_test_xtimer_rmutex_lock_timeout_long_locked(int argc,
                                                           char **argv);
static int cmd_test_xtimer_rmutex_lock_timeout_low_prio_thread(
    int argc, char **argv);

static int cmd_test_xtimer_rmutex_lock_timeout_short_unlocked(int argc,
                                                              char **argv);
static int cmd_test_xtimer_rmutex_lock_timeout_short_locked(int argc,
                                                            char **argv);

#define error_wrong_variables "Error: rmutex wrong variables in struct"
#define error_rmutex_taken "Error: rmutex taken"
#define error_timeout "Error: rmutex timed out"
#define shell_help_output "See main.c"

/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "t1", shell_help_output,
      cmd_test_xtimer_rmutex_lock_timeout_long_unlocked, },
    { "t2", shell_help_output,
      cmd_test_xtimer_rmutex_lock_timeout_long_locked, },
    { "t3", shell_help_output,
      cmd_test_xtimer_rmutex_lock_timeout_low_prio_thread, },
    { "t4", shell_help_output,
      cmd_test_xtimer_rmutex_lock_timeout_short_unlocked, },
    { "t5", shell_help_output,
      cmd_test_xtimer_rmutex_lock_timeout_short_locked, },
    { NULL, NULL, NULL }
};

/* main Thread PID */
static kernel_pid_t main_thread_pid;

/**
 * @brief   stack for
 *          cmd_test_xtimer_rmutex_lock_timeout_low_prio_thread
 */
static char t_stack[THREAD_STACKSIZE_SMALL];

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
 * @brief   this thread locks a rmutex
 */
static void *lock_rmutex_thread(void *arg)
{
    rmutex_t *test_rmutex = (rmutex_t *)arg;

    rmutex_lock(test_rmutex);
    return NULL;
}

/**
 * @brief   thread function for
 *          cmd_test_xtimer_rmutex_lock_timeout_low_prio_thread
 */
void *test_thread(void *arg)
{
    rmutex_t *test_rmutex = (rmutex_t *)arg;
    msg_t msg;

    rmutex_lock(test_rmutex);
    thread_wakeup(main_thread_pid);

    rmutex_unlock(test_rmutex);

    msg_send_sched_task_exit(&msg, main_thread_pid);
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout
 *
 * the rmutex is not locked before the function call and
 * the timer long. Meaning the timer will get removed
 * before triggering.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_long_unlocked(int argc,
                                                             char **argv)
{
    (void)argc;
    (void)argv;

    rmutex_t test_rmutex = RMUTEX_INIT;

    if (xtimer_rmutex_lock_timeout(&test_rmutex, LONG_RMUTEX_TIMEOUT) == 0) {
        /* rmutex has to be locked once */
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == thread_getpid() &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts(error_wrong_variables);
        }
    }
    else {
        puts(error_timeout);
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout
 *
 * the rmutex is locked from another thread before the
 * function call and the timer is long.
 * Meaning the timer will trigger
 * and remove the thread from the rmutex waiting list.
 * To loc
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_long_locked(int argc,
                                                           char **argv)
{
    (void)argc;
    (void)argv;

    rmutex_t test_rmutex = RMUTEX_INIT;

    /* lock rmutex from different thread */
    kernel_pid_t second_t_pid = thread_create( t_stack, sizeof(t_stack),
                                               THREAD_PRIORITY_MAIN - 1,
                                               THREAD_CREATE_STACKTEST,
                                               lock_rmutex_thread,
                                               (void *)&test_rmutex,
                                               "lock_thread");

    if (xtimer_rmutex_lock_timeout(&test_rmutex, LONG_RMUTEX_TIMEOUT) == 0) {
        puts(error_rmutex_taken);
    }
    else {
        /* rmutex has to be locked once */
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == second_t_pid &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts(error_wrong_variables);
        }
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout
 *
 * This function will create a new thread with lower prio
 * than the main thread (this function should be called from
 * the main thread). The new thread will get a rmutex and will
 * lock it. This function (main thread) calls xtimer_rmutex_lock_timeout.
 * The other thread will then unlock the rmutex. The main
 * thread gets the rmutex and wakes up. The timer will not
 * trigger because the main threads gets the rmutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_low_prio_thread(
    int argc, char **argv)
{
    (void)argc;
    (void)argv;

    main_thread_pid = thread_getpid();
    rmutex_t test_rmutex = RMUTEX_INIT;

    kernel_pid_t second_t_pid = thread_create( t_stack, sizeof(t_stack),
                                               THREAD_PRIORITY_MAIN + 1,
                                               THREAD_CREATE_STACKTEST,
                                               test_thread,
                                               (void *)&test_rmutex,
                                               "test_thread");
    (void)second_t_pid;
    thread_sleep();

    if (xtimer_rmutex_lock_timeout(&test_rmutex, LONG_RMUTEX_TIMEOUT) == 0) {
        /* rmutex has to be locked once */
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == thread_getpid() &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts(error_wrong_variables);
        }
    }
    else {
        puts(error_timeout);
    }

    /* to end the created thread */
    msg_t msg;
    msg_receive(&msg);

    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout when spinning
 *
 * The rmutex is locked before the function call and
 * the timer long. Meaning the timer will trigger before
 * xtimer_rmutex_lock_timeout tries to acquire the rmutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_short_locked(int argc,
                                                            char **argv)
{
    (void)argc;
    (void)argv;

    rmutex_t test_rmutex = RMUTEX_INIT;

    kernel_pid_t second_t_pid = thread_create( t_stack, sizeof(t_stack),
                                               THREAD_PRIORITY_MAIN - 1,
                                               THREAD_CREATE_STACKTEST,
                                               lock_rmutex_thread,
                                               (void *)&test_rmutex,
                                               "lock_thread");

    if (xtimer_rmutex_lock_timeout(&test_rmutex, SHORT_RMUTEX_TIMEOUT) == 0) {
        puts(error_rmutex_taken);
    }
    else {
        /* rmutex has to be locked once */
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == second_t_pid &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts(error_wrong_variables);
        }
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout when spinning
 *
 * the rmutex is not locked before the function is called and
 * the timer is short. Meaning the timer will trigger before
 * xtimer_rmutex_lock_timeout tries to acquire the rmutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_short_unlocked(int argc,
                                                              char **argv)
{
    (void)argc;
    (void)argv;

    rmutex_t test_rmutex = RMUTEX_INIT;

    if (xtimer_rmutex_lock_timeout(&test_rmutex, SHORT_RMUTEX_TIMEOUT) == 0) {
        /* rmutex has to be locked once */
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == thread_getpid() &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts(error_wrong_variables);
        }
    }
    else {
        puts(error_timeout);
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
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
