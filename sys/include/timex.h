#ifndef __TIMEX_H
#define __TIMEX_H 

#include <stdint.h>

typedef struct timex_t {
    uint32_t seconds;
    uint32_t nanoseconds;
} timex_t;

/* a+b */
timex_t timex_add(const timex_t a, const timex_t b); 

/* a-b*/
timex_t timex_sub(const timex_t a, const timex_t b);

timex_t timex_set(uint32_t seconds, uint32_t nanoseconds);

/**
 * @brief Compares two timex values.
 *
 * @return -1 when a is smaller, 0 if equal, 1 if a is bigger
 */
timex_t timex_sub(const timex_t a, const timex_t b);

/**
 * @brief Corrects timex_t structure so that nanoseconds < 1000000
 */
void timex_normalize(timex_t *time);

#endif /* __TIMEX_H */
