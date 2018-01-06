#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "thread.h"
#include "mutex.h"
#include "xtimer.h"

mutex_t res_mtx_A;
mutex_t res_mtx_B;


char stack_prio_1[THREAD_STACKSIZE_MAIN];
char stack_prio_2[THREAD_STACKSIZE_MAIN];
char stack_prio_3[THREAD_STACKSIZE_MAIN];


void *t_prio_1_handler(void *arg)
{
  (void) arg;
  /* starting working loop immediately */
  while(1){
    printf("prio_1: L(A)\n");
    mutex_lock(&res_mtx_A);
    xtimer_sleep(2);

    printf("prio_1: L(B)\n");
    mutex_lock(&res_mtx_B);

    printf("prio_1: U(B)\n");
    mutex_unlock(&res_mtx_B);

    printf("prio_1: U(A)\n");
    mutex_unlock(&res_mtx_A);
  }
  return NULL;
}


void *t_prio_2_handler(void *arg)
{
  (void) arg;
  xtimer_sleep(1);
  while(1){
    printf("prio_2: L(B)\n");
    mutex_lock(&res_mtx_B);
    xtimer_sleep(2);

    printf("prio_2: L(A)\n");
    mutex_lock(&res_mtx_A);

    printf("prio_2: U(A)\n");
    mutex_unlock(&res_mtx_A);

    printf("prio_2: U(B)\n");
    mutex_unlock(&res_mtx_B);
  }
  return NULL;
}



void *t_prio_3_handler(void *arg)
{
  (void) arg;
  xtimer_sleep(2);
  while(1){
    printf("prio_3: L(A)\n");
    mutex_lock(&res_mtx_A);

    printf("prio_3: U(A)\n");
    mutex_unlock(&res_mtx_A);
  }
  return NULL;
}

kernel_pid_t pid_prio_1;
kernel_pid_t pid_prio_2;
kernel_pid_t pid_prio_3;

int main(void)
{
  xtimer_init();
  mutex_init(&res_mtx_A);
  mutex_init(&res_mtx_B);


  pid_prio_1 = thread_create(stack_prio_1, sizeof(stack_prio_1),
      THREAD_PRIORITY_MAIN - 1,
      THREAD_CREATE_STACKTEST,
      t_prio_1_handler, NULL,
      "t_low");

  pid_prio_2 = thread_create(stack_prio_2, sizeof(stack_prio_2),
      THREAD_PRIORITY_MAIN - 2,
      THREAD_CREATE_STACKTEST,
      t_prio_2_handler, NULL,
      "t_low");

  pid_prio_3 = thread_create(stack_prio_3, sizeof(stack_prio_3),
      THREAD_PRIORITY_MAIN - 3,
      THREAD_CREATE_STACKTEST,
      t_prio_3_handler, NULL,
      "t_low");

  thread_sleep();
  return 0;
}
