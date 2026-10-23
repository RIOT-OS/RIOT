#include <stdint.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#include "periph/timer.h"
#include "periph_timer_mock.h"


static struct {
    tim_t dev;
    unsigned long freq;
    timer_cb_t cb;
    void *cb_arg;
    /* Handle only one channel for now */
    int channel;
    unsigned int timeout_value;

    uint32_t current;

    void (*post_read_cb)(uint32_t current);
} mock_timer;

static void _timer_mock_step_one(void);


void timer_mock_set_post_read_cb(void (*post_read_cb)(uint32_t current))
{
    mock_timer.post_read_cb = post_read_cb;
}

uint32_t timer_mock_step(uint32_t ticks)
{
    DEBUG("timer_mock_step(%" PRIu32 ") from %" PRIu32 "\n",
          ticks, mock_timer.current);
    for (uint32_t i = 0; i < ticks; i++) {
        _timer_mock_step_one();
    }
    return mock_timer.current;
}

static void _timer_mock_step_one(void)
{
    /* TODO handle callbacks for timer */
    mock_timer.current = (mock_timer.current == mock_timer.timeout_value ?
                          0 : mock_timer.current + 1);
    if (mock_timer.current == mock_timer.timeout_value) {
        printf("periph_timer_mock: TODO Callback not handled\n");
    }
}

void timer_mock_set(uint32_t ticks)
{
    DEBUG("timer_mock_set(%" PRIu32 ")\n", ticks);
    /* TODO This should be lower than `mock_timer.timeout_value` to be handled
     * properly.*/
    mock_timer.current = ticks;
}

static void _timer_cb_mock(void *arg, int channel)
{
    (void)arg;
    (void)channel;
}

/*
 * Mock of the original functions
 */
int timer_init_mock(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    DEBUG("timer_init_mock(%u): %" PRIu32 ", %p, %p\n", dev, freq, cb, arg);


    mock_timer.dev = dev;
    mock_timer.freq = freq;
    mock_timer.cb = cb;
    mock_timer.cb_arg = arg;

    mock_timer.current = 0;

    mock_timer.post_read_cb = NULL;

    (void)_timer_cb_mock;

    return 0;
}

unsigned int timer_read_mock(tim_t dev)
{
    DEBUG("timer_read_mock(%u)\n", dev);
    uint32_t now = timer_mock_step(1);

    if (mock_timer.post_read_cb) {
        mock_timer.post_read_cb(now);
    }

    return now;
}

int timer_set_absolute_mock(tim_t dev, int channel, unsigned int value)
{
    assert(channel == 0); /* Not handled more than one channel for the moment */
    DEBUG("timer_set_absolute_mock(%u): channel %i, value %u\n", dev, channel, value);
    mock_timer.channel = channel;
    mock_timer.timeout_value = value;
    return 1;
}
