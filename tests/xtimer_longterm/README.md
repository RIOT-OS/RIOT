Expected result
===============
You should see a number of different messages, printed out on certain intervals:

`msg   -- 14min -- 14 ticks since` -> timeout by xtimer_msg every 14 minutes
`sleep -- 18min -- 18 ticks since` -> timeout by xtimer_usleep, every 18 minutes
`msg   -- 3min  -- 3 ticks since`  -> timeout by xtimer_msg, every 3 minutes
`sleep -- 5min  -- 5 ticks since`  -> timeout by xtimer_usleep, every 5 minutes
`TICK  -- 1min`  -> event created by aggregation counter every 1 min

Background
==========
This test is supposed to find out, if the `xtimer` behaves correctly when
scheduling multiple long and short term timers. For this we schedule two 'long'
and two 'mid'-term timers, while in parallel running one periodic fast timer.

The 'long' term timers are triggering every 14 and 18 min (so in this context
we consider this long-term...). The mid-term timers are set to 3 and 5 minutes.
Both kind of timers have one that is using `xtimer_usleep` and one that is
using `xtimer_set_msg`.

The short-term timer is triggered every 50ms and is using `xtimer_sleep_until`.
Each time this timer triggers, it increments a software counter, which triggers
then a message every minute. A 50ms interval should be small enough, to trigger
also for 16-bit wide timers at least once in every timer period.

On each mid- and long-term timer event, the output shows also the number of
fast timer (1min timer) ticks, that have been triggered since a timer was
triggered last. This number should be equal to the timers interval.

For reasonable results, you should run this test at least for some ours...
