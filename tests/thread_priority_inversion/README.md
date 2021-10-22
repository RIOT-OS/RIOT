Priority Inversion
==================

Scenario: A low priority thread holds a shared resource when a mid priority and a high
priority thread become runnable. The high priority thread needs exclusive access to the
resource the low priority one is holding and blocks. This mid priority thread does not
compete with the other for resources but takes a long to complete. The scheduler must
prefer the low priority thread over the mid priority thread while it is holding the
resource the high priority thread is waiting for - otherwise the high priority thread
effectively waits for the mid priority thread, which is an inversion of priority.

Output On Failure
-----------------

```
main(): This is RIOT! (Version: ...)
low priority thread has started
low priority thread started to work on its task
high priority thread has started
mid priority thread has started
mid priority thread started to work on its task
    ... this ...
    ... takes ...
    ... bloody ...
    ... ages ...
    ... to ...
    ... complete ...
mid priority thread is done
low priority thread is done
high priority thread started to work on its task
high priority thread is done
==> Priority inversion occurred
TEST FAILED
```


Output On Success
-----------------

```
main(): This is RIOT! (Version: ...)
low priority thread has started
low priority thread started to work on its task
high priority thread has started
low priority thread is done
high priority thread started to work on its task
high priority thread is done
mid priority thread has started
mid priority thread started to work on its task
    ... this ...
    ... takes ...
    ... bloody ...
    ... ages ...
    ... to ...
    ... complete ...
mid priority thread is done
TEST PASSED
```
