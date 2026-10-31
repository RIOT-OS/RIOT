Testing Context Switches from ISR under Load
============================================

This tests launches two threads, t1 and t2, which each try to lock its own mutex for 500 times. A
hardware timer is unlocking one of the mutexes in turn. Half of the time the timeout is set so
short, that the timer IRQ triggers before the context switch is completed, the other half of the
time the timer should trigger after the context switch is done.

If the context switch implementation fails to disable IRQs until the context switch is fully
completed, this will result in (parts) of the previous context still being on the stack when the
next ISR is executed. Over time, this will accumulate data on the stack and eventually the stack
will overflow. Since the stack test is enabled, this should hopefully be caught and the test
applications panics.

If the stack does not overflow during the test, it is assumed that context switches from ISR under
load do not accumulate data on the stack. Hence, this is considered a pass.
