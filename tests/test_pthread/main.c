#include <stdio.h>
#include <inttypes.h>

#include "pthread.h"

void* run(void* parameter) {
    uint32_t factorial;
    int n = (int) parameter;
    factorial = 1;

    printf("%p\n", parameter);

    if (n > 0) {
        for (int i = 1; i <= n; i++) {
            factorial *= i;
        }
    }

    pthread_exit((void *)factorial);

    return NULL;
}

int main(void) {
    pthread_t th_id;
    pthread_attr_t th_attr;

    int arg = 6;

    pthread_attr_init(&th_attr);
    pthread_create(&th_id, &th_attr, run, (void *) arg);
    uint32_t res;
    pthread_join(th_id, (void **) &res);
    printf("factorial = %" PRIu32 "\n", res);
    puts("finished");
    return 0;
}

