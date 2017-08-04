NOTE
====
THIS TEST WILL FAIL WHEN RUNNING RIOT USING THE DEFAULT CORE/KERNEL CONFIGURATION!

You need to select the `core_priority_inheritance` module (using
`USEMODULE += core_priority_inheritance`) to see this test succeeding.

Expected result
===============
When successful, you will see the 5 configured threads printing 15 different
events in a defined order. If the test passes, the output should look like this:

```
main(): This is RIOT! (Version: xxx)
Simple test for showing the effect of priority inversion

If this tests succeeds, you should see 15 events appearing in order.
The expected output should look like this:
Event  1:      t5 - locking mutex A
Event  2:      t5 - holding mutex A
Event  3:      t5 - locking mutex B
Event  4:      t5 - holding mutex B
Event  5:      t3 - locking mutex A
Event  6:      t1 - locking mutex B
Event  7:      t2 - starting extensive loop, potentially starving others
Event  8:      t5 - unlocking mutex B
Event  9:      t1 - holding mutex B
Event 10:      t1 - unlocking mutex B
Event 11:      t2 - finished extensive loop
Event 12:      t4 - starting infinite loop, potentially starving others
Event 13:      t5 - unlocking mutex A
Event 14:      t3 - holding mutex A
Event 15:      t3 - unlocking mutex A

TEST OUTPUT:
Event  1:      t5 - locking mutex A
Event  2:      t5 - holding mutex A
Event  3:      t5 - locking mutex B
Event  4:      t5 - holding mutex B
Event  5:      t3 - locking mutex A
Event  6:      t1 - locking mutex B
Event  7:      t2 - starting extensive loop, potentially starving others
Event  8:      t5 - unlocking mutex B
Event  9:      t1 - holding mutex B
Event 10:      t1 - unlocking mutex B
Event 11:      t2 - finished extensive loop
Event 12:      t4 - starting infinite loop, potentially starving others
Event 13:      t5 - unlocking mutex A
Event 14:      t3 - holding mutex A
Event 15:      t3 - unlocking mutex A

   *** result: SUCCESS ***
```

Background
==========
Priority inversion is a known problem in real-time systems, leading in certain
constellations to lower priority threads indirectly blocking higher priority
threads. This test constructs a scenario with two, nested mutexes being shared
among three different threads, while having two 'bogus' threads trying to starve
the others out.

The interesting part to observe here is the priority, that is assigned to the
lowest prio thread `t5`. `t5`s priority will change from `PRIO_MAIN + 5` ->
`PRIO_MAIN + 3` -> `PRIO_MAIN + 1` -> `PRIO_MAIN + 3` -> PRIO_MAIN + 5`.
