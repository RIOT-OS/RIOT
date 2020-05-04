# Test for `irq_disable()` and `irq_restore()`

This test checks if `irq_disable()` and `irq_restore()` work as intended. For
that, the test uses two variables *a* and *b*, both initialized with 0. The main
test will schedule a timer that expires in *T/2*. It will then set *a* to 1 and
waits for *T* until it also sets *b* to one. The expectation is that during the
ISR (the timer callback) *a* has a value of 1, but *b* still has a value of 0.

The test is repeated, but this time the main thread calls `irq_disable()`
before setting *a* and `irq_restore()` after setting *b*. It is now expected
that - even though the timer again expires after setting *a* and before setting
*b* - the execution of the ISR is delayed until `irq_restore()` is called. Thus,
both *a* and *b* should already contain the new value in the ISR.
