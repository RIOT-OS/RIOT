NOTE
====
THIS TEST WILL FAIL WHEN RUNNING RIOT USING THE DEFAULT CORE/KERNEL CONFIGURATION!

TODO: merge the `core_priority_inheritance` module to enable priority
      inheritance and with this successfully pass this test.

Expected result
===============
When successful, you will see the 3 configured threads printing 6 different
events in a defined order. If the test passes, the output should look like this:

```
main(): This is RIOT! (Version: xxx)
Test for showing priority inversion when using msg_send_receive

If this tests succeeds, you should see 6 events appearing in order.
The expected output should look like this:
Event  1:   t_low - waiting for incoming message
Event  2:  t_high - sending msg to t_low (msg_send_receive)
Event  3:   t_low - received message and do some quick work
Event  4:   t_mid - starting infinite loop, potentially starving others
Event  5:   t_low - sending reply
Event  6:  t_high - received reply

TEST OUTPUT:
Event  1:   t_low - waiting for incoming message
Event  2:  t_high - sending msg to t_low (msg_send_receive)
Event  3:   t_low - received message and do some quick work
Event  4:   t_mid - starting infinite loop, potentially starving others
Event  5:   t_low - sending reply
Event  6:  t_high - received reply

[SUCCESS]
```

Background
==========
Priority inversion is a known problem in real-time systems, leading in certain
constellations to lower priority threads indirectly blocking higher priority
threads. This test constructs a simple scenario, where a high priority thread
(`t_high`) is trying to `send_receive` a message to a low priority thread
(`t_low`), but never gets a reply as a medium priority thread (`t_mid`) is never
 letting `t_mid` to be scheduled.
