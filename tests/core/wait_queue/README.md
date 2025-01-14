Test for the wait queue
=======================

This test disables the early exit optimization of the `QUEUE_WAIT()` macro as
it hinders the testing of some edge cases and only trivially plays a role in
correctness. However, for completeness, the [wait_queue_optimized
test](../wait_queue_optimized/) runs this same test with the early exit
optimization on, but some sections of the test are not performed.
