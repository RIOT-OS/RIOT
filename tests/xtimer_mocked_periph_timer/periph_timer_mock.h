#ifndef PERIPH_TIMER_MOCK_H
#define PERIPH_TIMER_MOCK_H

/* Put here any functions that could need to help communicate with the mock */

/*
 * Step the timer counter by 'ticks' and return the current ticks count after.
 */
uint32_t timer_mock_step(uint32_t ticks);

/*
 * Set the internal timer without handling any callbacks
 * Should fit the internal precisiong
 */
void timer_mock_set(uint32_t ticks);

/*
 * This simulates a function that would be called after the value of
 * `xtimer_now` is calculated
 *
 * And so deschedule after xtimer_now.
 */
void timer_mock_set_post_step_cb(void (*post_step_cb)(uint32_t current));

#endif /* PERIPH_TIMER_MOCK_H */
