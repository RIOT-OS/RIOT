Introduction
============

This test verifies that a thread can call "thread_yield_higher()" with IRQs
disabled, and that the actual context switch is deferred until IRQs are
re-enabled.

For the test to be successful, the printed messages must be in order, as follows:

```
1. post yield
2. second thread scheduled
3. post irq enable
```
