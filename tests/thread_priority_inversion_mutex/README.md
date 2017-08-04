NOTE
====
THIS TEST WILL FAIL WHEN RUNNING RIOT USING THE DEFAULT CORE/KERNEL CONFIGURATION!

TODO: merge the `core_priority_inheritance` module to enable priority
      inheritance and with this successfully pass this test.

Expected result
===============
When successful, you will see the three configured threads printing 7 different
events in a defined order. If the test passes, the output should look like this:

```
main(): This is RIOT! (Version: xxx)
Simple test for showing the effect of priority inversion

If this tests succeeds, you should see 7 events appearing in order.
The expected output should look like this:
Event  1:   t_low - locking mutex
Event  2:   t_low - holding mutex
Event  3:  t_high - locking mutex
Event  4:   t_mid - starting infinite loop, potentially starving others
Event  5:   t_low - unlocking mutex
Event  6:  t_high - holding mutex
Event  7:  t_high - unlocking mutex

TEST OUTPUT:
Event  1:   t_low - locking mutex
Event  2:   t_low - holding mutex
Event  3:  t_high - locking mutex
Event  4:   t_mid - starting infinite loop, potentially starving others
Event  5:   t_low - unlocking mutex
Event  6:  t_high - holding mutex
Event  7:  t_high - unlocking mutex

   *** result: SUCCESS ***
```

Background
==========
Priority inversion is a known problem in real-time systems, leading in certain
constellations to lower priority threads indirectly blocking higher priority
threads. This test application constructs a simple situation, where exactly this
occurs: t_low owns a mutex, which t_high is waiting on. At that point, t_mid
starts and preempts t_low (as t_mid has a higher priority than t_low),
preventing t_low from unlocking the mutex shared with t_high. So t_mid is
indirectly preventing t_high from running.
