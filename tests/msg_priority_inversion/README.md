NOTE
====
THIS TEST WILL FAIL WHEN RUNNING RIOT USING THE DEFAULT CORE/KERNEL CONFIGURATION!

You need to select the `core_priority_inheritance` module (using
`USEMODULE += core_priority_inheritance`) to see this test succeeding.

Expected result
===============
When successful, you will see the 3 configured threads printing 6 different
events in a defined order. If the test passes, the output should look like this:

```
main(): This is RIOT! (Version: xxx)
Test for showing priority inversion when using msg_send_receive

If this tests succeeds, you should see 6 events appearing in order.
The expected output should look like this:
Event  1:      t3 - waiting for incoming message
Event  2:      t2 - starting infinite loop, potentially starving others
Event  3:      t1 - sending msg to t3 (msg_send_receive)
Event  4:      t3 - received message
Event  5:      t3 - sending reply
Event  6:      t1 - received reply

TEST OUTPUT:
Event  1:      t3 - waiting for incoming message
Event  2:      t2 - starting infinite loop, potentially starving others
Event  3:      t1 - sending msg to t3 (msg_send_receive)
Event  4:      t3 - received message
Event  5:      t3 - sending reply
Event  6:      t1 - received reply

   *** result: SUCCESS ***
```

Background
==========
Priority inversion is a known problem in real-time systems, leading in certain
constellations to lower priority threads indirectly blocking higher priority
threads. This test constructs a simple scenario, where a high priority thread
(`t1`) is trying to `send_receive` a message to a low priority thread (`t3`),
but never gets a reply as a medium priority thread (`t2`) is never letting `t3`
to be scheduled.
