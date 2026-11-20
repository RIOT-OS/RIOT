#ifndef PERIPH_TIMER_MOCK_H
#define PERIPH_TIMER_MOCK_H

/* Put here any functions that could need to help communicate with the mock */

/*
 * Step the timer counter by 'ticks' and return the current ticks count after.
 */
uint32_t timer_mock_step(uint32_t ticks);

/*
 * Set the internal timer without handling any callbacks
 * Should fit the internal precision
 */
void timer_mock_set(uint32_t ticks);

/*
 * This simulates a function that would be called after the value of
 * `timer_read` is calculated. It is executed after evaluating the return value.
 */
void timer_mock_set_post_read_cb(void (*post_read_cb)(uint32_t current));

#endif /* PERIPH_TIMER_MOCK_H */
