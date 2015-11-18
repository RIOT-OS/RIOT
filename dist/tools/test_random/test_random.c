#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef MODULE_ENTROPY
#include "entropy.h"
#define ENTROPY_FREQ_DEFAULT 1024
#endif

uint32_t genrand_uint32(void);
void genrand_init(uint32_t s);

#define N_WORDS (2<<24)

int main(int argc, char *argv[]) {
    uint32_t random;
    uint32_t n = N_WORDS;

#ifdef MODULE_ENTROPY
    uint32_t entropy_freq;

    if (argc>1) {
        entropy_freq = atoi(argv[1]);
    } else {
        entropy_freq = ENTROPY_FREQ_DEFAULT;
    }

    fprintf(stderr, "Injecting a random byte every %u times.\n", entropy_freq);
#endif

    while(n--) {

#ifdef MODULE_ENTROPY
        if (! (n % entropy_freq)) {
            fprintf(stderr, "Collecting entropy... %u\n", n);
            entropy_collect();
        }
#endif

        random = genrand_uint32();
        write(STDOUT_FILENO, &random, 4);
    }

    return 0;
}
